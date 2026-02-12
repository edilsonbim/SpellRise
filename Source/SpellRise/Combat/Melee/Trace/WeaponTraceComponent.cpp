#include "WeaponTraceComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"

// ---------------------------------------------------------
// Gameplay Tags (lazy init) - AAA + legacy fallback
// ---------------------------------------------------------
namespace SpellRiseTraceTags
{
	inline const FGameplayTag& Data_BaseWeaponDamage()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.BaseWeaponDamage"), /*ErrorIfNotFound=*/false);
		return Tag;
	}

	inline const FGameplayTag& Data_DamageScaling()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.DamageScaling"), /*ErrorIfNotFound=*/false);
		return Tag;
	}

	// Legacy
	inline const FGameplayTag& Data_BaseDamage_Legacy()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.BaseDamage"), /*ErrorIfNotFound=*/false);
		return Tag;
	}
}

UWeaponTraceComponent::UWeaponTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// Legacy default: Data.BaseDamage (mantido por compat)
	SetByCallerDamageTag = SpellRiseTraceTags::Data_BaseDamage_Legacy();
}

void UWeaponTraceComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(true);
}

void UWeaponTraceComponent::BeginTraceWindow(const FWeaponTraceConfig& InConfig, UAbilitySystemComponent* InSourceASC)
{
	Config = InConfig;
	SourceASC = InSourceASC;

	bTracing = true;
	bHasPrev = false;
	HitActors.Reset();
}

void UWeaponTraceComponent::EndTraceWindow()
{
	bTracing = false;
	bHasPrev = false;
	HitActors.Reset();
}

void UWeaponTraceComponent::SetDamagePayload(TSubclassOf<UGameplayEffect> InDamageEffect, float InBaseDamage)
{
	DamageEffect = InDamageEffect;
	BaseDamage = InBaseDamage;
}

void UWeaponTraceComponent::ResetHitActors()
{
	HitActors.Reset();
}

void UWeaponTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bTracing)
	{
		return;
	}

	TraceStep();
}

bool UWeaponTraceComponent::GetSocketWorldLocation(const FName& Socket, FVector& OutLocation) const
{
	if (!Config.WeaponComponent || Socket.IsNone())
	{
		return false;
	}

	// USceneComponent tem GetSocketLocation se for primitive/skeletal; mas SceneComponent base não.
	// Para ser genérico: tenta GetSocketLocation via interface de PrimitiveComponent:
	if (const UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Config.WeaponComponent))
	{
		if (Prim->DoesSocketExist(Socket))
		{
			OutLocation = Prim->GetSocketLocation(Socket);
			return true;
		}
	}

	return false;
}

void UWeaponTraceComponent::TraceStep()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Config.WeaponComponent)
	{
		return;
	}

	// ✅ Autoridade real deve ser do ASC (mais confiável que Owner->HasAuthority())
	const bool bServerAuth =
		(SourceASC != nullptr) &&
		SourceASC->IsOwnerActorAuthoritative();

	FVector BaseNow, TipNow;
	if (!GetSocketWorldLocation(Config.SocketBase, BaseNow) || !GetSocketWorldLocation(Config.SocketTip, TipNow))
	{
		return;
	}

	if (!bHasPrev)
	{
		PrevBase = BaseNow;
		PrevTip  = TipNow;
		bHasPrev = true;
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponTrace), false, Owner);

	// Trace em 2 linhas (Base e Tip) para capturar volume melhor
	auto SweepLine = [&](const FVector& From, const FVector& To)
	{
		TArray<FHitResult> Hits;
		const bool bHit = World->SweepMultiByChannel(
			Hits,
			From,
			To,
			FQuat::Identity,
			Config.TraceChannel,
			FCollisionShape::MakeSphere(Config.Radius),
			Params
		);

		if (Config.bDrawDebug)
		{
			DrawDebugLine(World, From, To, FColor::Cyan, false, Config.DebugLifeTime, 0, 1.0f);
			DrawDebugSphere(World, To, Config.Radius, 10, FColor::Cyan, false, Config.DebugLifeTime, 0, 1.0f);
		}

		// ✅ Dano só no server (via ASC authority)
		if (bHit && bServerAuth)
		{
			for (const FHitResult& HR : Hits)
			{
				AActor* HitActor = HR.GetActor();
				if (!HitActor || HitActor == Owner)
				{
					continue;
				}

				if (HitActors.Contains(HitActor))
				{
					continue;
				}

				HitActors.Add(HitActor);
				ApplyDamageToActor(HitActor);
			}
		}
	};

	SweepLine(PrevBase, BaseNow);
	SweepLine(PrevTip,  TipNow);

	PrevBase = BaseNow;
	PrevTip  = TipNow;
}

void UWeaponTraceComponent::ApplyDamageToActor(AActor* TargetActor)
{
	if (!TargetActor || !SourceASC || !DamageEffect)
	{
		return;
	}

	// ✅ Hard gate: mesmo que alguém chame isso por engano no client, não aplica
	if (!SourceASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		return;
	}

	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.AddSourceObject(this);

	FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageEffect, 1.0f, Ctx);
	if (!Spec.IsValid() || !Spec.Data.IsValid())
	{
		return;
	}

	const float SafeBase = FMath::Max(0.f, BaseDamage);

	// ✅ AAA: seta o novo tag (ExecCalc lê esse primeiro)
	if (SpellRiseTraceTags::Data_BaseWeaponDamage().IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(SpellRiseTraceTags::Data_BaseWeaponDamage(), SafeBase);
	}

	// ✅ Legacy: mantém compatibilidade total
	if (SetByCallerDamageTag.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(SetByCallerDamageTag, SafeBase);
	}

	// ✅ Scaling padrão (1.0) para não depender de legacy
	if (SpellRiseTraceTags::Data_DamageScaling().IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(SpellRiseTraceTags::Data_DamageScaling(), 1.0f);
	}

	SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
}
