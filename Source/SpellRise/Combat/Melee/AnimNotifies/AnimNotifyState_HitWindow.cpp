#include "AnimNotifyState_HitWindow.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

// NÃO inclua CollisionProfile.h - vamos usar outro método

#include "SpellRise/Combat/Melee/Trace/WeaponTraceComponent.h"
#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"
#include "SpellRise/Weapons/Actors/SpellRiseWeaponActor.h"

UAnimNotifyState_HitWindow::UAnimNotifyState_HitWindow()
{
	// defaults already set in header
}

static AActor* SR_ResolveRootOwnerForCombat(AActor* Owner)
{
	if (!Owner) return nullptr;

	if (AActor* Outer = Owner->GetOwner())
	{
		if (AActor* Outer2 = Outer->GetOwner())
		{
			return Outer2;
		}
		return Outer;
	}

	return Owner;
}

static UWeaponTraceComponent* SR_FindTraceComponent(AActor* Owner)
{
	if (!Owner) return nullptr;

	if (UWeaponTraceComponent* Trace = Owner->FindComponentByClass<UWeaponTraceComponent>())
	{
		return Trace;
	}

	if (AActor* Outer = Owner->GetOwner())
	{
		if (UWeaponTraceComponent* Trace = Outer->FindComponentByClass<UWeaponTraceComponent>())
		{
			return Trace;
		}

		if (AActor* Outer2 = Outer->GetOwner())
		{
			return Outer2->FindComponentByClass<UWeaponTraceComponent>();
		}
	}

	return nullptr;
}

static UAbilitySystemComponent* SR_FindASC(AActor* Owner)
{
	if (!Owner) return nullptr;

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
	{
		return ASC;
	}

	if (AActor* Outer = Owner->GetOwner())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Outer))
		{
			return ASC;
		}

		if (AActor* Outer2 = Outer->GetOwner())
		{
			return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Outer2);
		}
	}

	return nullptr;
}

USceneComponent* UAnimNotifyState_HitWindow::ResolveWeaponComponent(AActor* Owner, USkeletalMeshComponent* MeshComp) const
{
	if (WeaponComponent)
	{
		return WeaponComponent;
	}

	if (!Owner || !MeshComp)
	{
		return MeshComp;
	}

	AActor* CombatOwner = SR_ResolveRootOwnerForCombat(Owner);

	if (CombatOwner)
	{
		if (USpellRiseWeaponComponent* WC = CombatOwner->FindComponentByClass<USpellRiseWeaponComponent>())
		{
			if (ASpellRiseWeaponActor* WeaponActor = WC->GetEquippedWeaponActor())
			{
				if (WeaponActor->Mesh)
				{
					return WeaponActor->Mesh;
				}

				if (UPrimitiveComponent* Prim = WeaponActor->FindComponentByClass<UPrimitiveComponent>())
				{
					return Prim;
				}
				
				if (USceneComponent* Root = WeaponActor->GetRootComponent())
				{
					return Root;
				}
			}
		}
	}

	return MeshComp;
}

// --- CORREÇÃO: Converter ETraceTypeQuery para ECollisionChannel SEM UCollisionProfile ---
static ECollisionChannel ConvertTraceTypeToCollisionChannel(ETraceTypeQuery TraceType)
{
	switch (TraceType)
	{
		case TraceTypeQuery1:  return ECC_Visibility;
		case TraceTypeQuery2:  return ECC_Camera;
		case TraceTypeQuery3:  return ECC_WorldStatic;  // Player
		case TraceTypeQuery4:  return ECC_WorldDynamic; // Weapon
		case TraceTypeQuery5:  return ECC_Pawn;         // Projectile
		case TraceTypeQuery6:  return ECC_PhysicsBody;  // Pawn
		default:               return ECC_WorldDynamic;
	}
}

void UAnimNotifyState_HitWindow::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) 
	{
		UE_LOG(LogTemp, Warning, TEXT("[HitWindow] MeshComp is null"));
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) 
	{
		UE_LOG(LogTemp, Warning, TEXT("[HitWindow] Owner is null"));
		return;
	}

	UWeaponTraceComponent* TraceComp = SR_FindTraceComponent(Owner);
	if (!TraceComp) 
	{
		UE_LOG(LogTemp, Warning, TEXT("[HitWindow] WeaponTraceComponent not found on %s"), *GetNameSafe(Owner));
		return;
	}

	UAbilitySystemComponent* SourceASC = SR_FindASC(Owner);
	if (!SourceASC) 
	{
		UE_LOG(LogTemp, Warning, TEXT("[HitWindow] AbilitySystemComponent not found on %s"), *GetNameSafe(Owner));
		return;
	}

	// --- CORREÇÃO: Usar função auxiliar em vez de UCollisionProfile ---
	ECollisionChannel CollisionChannel = ConvertTraceTypeToCollisionChannel(TraceChannel);

	FWeaponTraceConfig Config;
	Config.WeaponComponent = ResolveWeaponComponent(Owner, MeshComp);
	Config.SocketBase = SocketBase;
	Config.SocketTip = SocketTip;
	Config.Radius = Radius;
	Config.TraceChannel = CollisionChannel;
	Config.bDrawDebug = bDrawDebug;
	Config.DebugLifeTime = DebugLifeTime;

	TraceComp->BeginTraceWindow(Config, SourceASC);
	
	UE_LOG(LogTemp, Verbose, TEXT("[HitWindow] Started on %s with weapon %s"), 
		*GetNameSafe(Owner), 
		*GetNameSafe(Config.WeaponComponent));
}

void UAnimNotifyState_HitWindow::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (UWeaponTraceComponent* TraceComp = SR_FindTraceComponent(Owner))
	{
		TraceComp->EndTraceWindow();
		UE_LOG(LogTemp, Verbose, TEXT("[HitWindow] Ended on %s"), *GetNameSafe(Owner));
	}
}