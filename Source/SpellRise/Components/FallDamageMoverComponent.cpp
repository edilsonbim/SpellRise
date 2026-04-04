// FallDamageMoverComponent.cpp
#include "SpellRise/Components/FallDamageMoverComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DefaultMovementSet/CharacterMoverComponent.h"
#include "GameplayEffect.h"
#include "MoverComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseFallDamageMover, Log, All);

UFallDamageMoverComponent::UFallDamageMoverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(false);
}

void UFallDamageMoverComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<APawn>(GetOwner());
	OwnerMover = GetOwner() ? GetOwner()->FindComponentByClass<UMoverComponent>() : nullptr;

	if (!GetOwner() || !GetOwner()->HasAuthority() || !OwnerPawn || !OwnerMover)
	{
		SetComponentTickEnabled(false);
		return;
	}

	OwnerMover->OnMovementModeChanged.AddDynamic(this, &UFallDamageMoverComponent::HandleMoverMovementModeChanged);
	LastMovementModeName = OwnerMover->GetMovementModeName();

	if (IsCurrentModeFalling())
	{
		StartFallTracking();
	}
}

void UFallDamageMoverComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner() || !GetOwner()->HasAuthority() || !bFallTrackingActive || !OwnerMover)
	{
		return;
	}

	const float DownSpeedAbs = FMath::Max(0.f, -OwnerMover->GetVelocity().Z);
	MaxFallSpeedAbs = FMath::Max(MaxFallSpeedAbs, DownSpeedAbs);
}

void UFallDamageMoverComponent::HandleMoverMovementModeChanged(const FName& PreviousMovementModeName, const FName& NewMovementModeName)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !OwnerMover)
	{
		return;
	}

	const bool bWasFalling = IsFallingModeName(PreviousMovementModeName);
	const bool bIsFalling = IsFallingModeName(NewMovementModeName);
	LastMovementModeName = NewMovementModeName;

	UE_LOG(LogSpellRiseFallDamageMover, Verbose, TEXT("[FALL MOVER] Mode %s -> %s (WasFalling=%d IsFalling=%d)"),
		*PreviousMovementModeName.ToString(),
		*NewMovementModeName.ToString(),
		bWasFalling ? 1 : 0,
		bIsFalling ? 1 : 0);

	if (bIsFalling && !bFallTrackingActive)
	{
		StartFallTracking();
		return;
	}

	if (bWasFalling && !bIsFalling && bFallTrackingActive)
	{
		FHitResult FloorHit;
		const bool bHasFloorHit = OwnerMover->TryGetFloorCheckHitResult(FloorHit);
		EvaluateLanding(bHasFloorHit ? FloorHit : FHitResult());
	}
}

void UFallDamageMoverComponent::StartFallTracking()
{
	if (!OwnerPawn || !OwnerMover)
	{
		return;
	}

	bFallTrackingActive = true;
	FallStartZ = OwnerPawn->GetActorLocation().Z;
	FallStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	MaxFallSpeedAbs = FMath::Max(0.f, -OwnerMover->GetVelocity().Z);
}

void UFallDamageMoverComponent::StopFallTracking()
{
	bFallTrackingActive = false;
	FallStartZ = 0.f;
	FallStartTime = 0.f;
	MaxFallSpeedAbs = 0.f;
	bIgnoreNextFallDamage = false;
}

void UFallDamageMoverComponent::EvaluateLanding(const FHitResult& Hit)
{
	if (!OwnerPawn)
	{
		return;
	}

	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	const float FallDuration = Now - FallStartTime;
	const float DeltaZ = FallStartZ - OwnerPawn->GetActorLocation().Z;

	if (bIgnoreNextFallDamage || HasFallImmunity() || IsSafeSurface(Hit))
	{
		StopFallTracking();
		return;
	}

	if (FallDuration < MinFallTime || DeltaZ < MinFallHeight || MaxFallSpeedAbs < SafeSpeed)
	{
		StopFallTracking();
		return;
	}

	const float Severity = ComputeSeverity();
	if (Severity <= 0.f)
	{
		StopFallTracking();
		return;
	}

	const float DamagePercent = ComputeDamagePercent(Severity);
	float Damage = GetMaxHealth() * DamagePercent;
	Damage = ApplyMitigations(Damage, Hit);

	if (Damage > 0.f)
	{
		ApplyFallDamage(Damage, Severity, Hit);
	}

	StopFallTracking();
}

bool UFallDamageMoverComponent::IsCurrentModeFalling() const
{
	const UCharacterMoverComponent* CharacterMover = Cast<UCharacterMoverComponent>(OwnerMover);
	if (CharacterMover)
	{
		return CharacterMover->IsFalling();
	}

	return IsFallingModeName(LastMovementModeName);
}

bool UFallDamageMoverComponent::IsFallingModeName(const FName& ModeName) const
{
	if (ModeName.IsNone())
	{
		return false;
	}

	return ModeName.ToString().Contains(TEXT("Falling"), ESearchCase::IgnoreCase);
}

bool UFallDamageMoverComponent::HasFallImmunity() const
{
	if (const UAbilitySystemComponent* ASC = GetASC())
	{
		static const FGameplayTag TagSafeFall =
			FGameplayTag::RequestGameplayTag(TEXT("State.SafeFall"), false);

		static const FGameplayTag TagInvulnerable =
			FGameplayTag::RequestGameplayTag(TEXT("State.Invulnerable"), false);

		return ASC->HasMatchingGameplayTag(TagSafeFall)
			|| ASC->HasMatchingGameplayTag(TagInvulnerable);
	}

	return false;
}

bool UFallDamageMoverComponent::IsSafeSurface(const FHitResult& Hit) const
{
	const UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
	if (!PhysMat)
	{
		return false;
	}

	const FString Name = PhysMat->GetName();
	return Name.Contains(TEXT("Water")) || Name.Contains(TEXT("Hay"));
}

float UFallDamageMoverComponent::ComputeSeverity() const
{
	const float Alpha = (MaxFallSpeedAbs - SafeSpeed) / FMath::Max(1.f, (FatalSpeed - SafeSpeed));
	return FMath::Clamp(Alpha, 0.f, 1.f);
}

float UFallDamageMoverComponent::ComputeDamagePercent(float Severity) const
{
	if (FallDamagePercentCurve)
	{
		return FMath::Max(0.f, FallDamagePercentCurve->GetFloatValue(Severity));
	}

	return FMath::Pow(Severity, 1.35f);
}

float UFallDamageMoverComponent::ApplyMitigations(float Damage, const FHitResult& Hit) const
{
	float Result = Damage;

	const float MaxStaminaValue = GetMaxStamina();
	if (MaxStaminaValue > 0.f)
	{
		const float StaminaPct = GetCurrentStamina() / MaxStaminaValue;

		if (StaminaPct >= 0.60f)
		{
			Result *= (1.f - StaminaMitigationAt60);
		}
		else if (StaminaPct >= 0.30f)
		{
			Result *= (1.f - StaminaMitigationAt30);
		}
	}

	const UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
	if (PhysMat && PhysMat->GetName().Contains(TEXT("Mud")))
	{
		Result *= 0.85f;
	}

	return FMath::Max(0.f, Result);
}

UAbilitySystemComponent* UFallDamageMoverComponent::GetASC() const
{
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
}

float UFallDamageMoverComponent::GetMaxHealth() const
{
	const UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return 0.f;
	}

	return ASC->GetNumericAttribute(UResourceAttributeSet::GetMaxHealthAttribute());
}

float UFallDamageMoverComponent::GetCurrentStamina() const
{
	const UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return 0.f;
	}

	return ASC->GetNumericAttribute(UResourceAttributeSet::GetStaminaAttribute());
}

float UFallDamageMoverComponent::GetMaxStamina() const
{
	const UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return 0.f;
	}

	return ASC->GetNumericAttribute(UResourceAttributeSet::GetMaxStaminaAttribute());
}

void UFallDamageMoverComponent::ApplyFallDamage(float Damage, float Severity, const FHitResult& Hit)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC || !GE_FallDamage)
	{
		UE_LOG(LogSpellRiseFallDamageMover, Warning, TEXT("[FALL MOVER] ASC ou GE_FallDamage ausente."));
		return;
	}

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddHitResult(Hit);
	Context.AddSourceObject(GetOwner());

	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_FallDamage, 1.f, Context);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return;
	}

	static const FGameplayTag TagDataDamage =
		FGameplayTag::RequestGameplayTag(TEXT("Data.Damage"), false);
	static const FGameplayTag TagDataFallSeverity =
		FGameplayTag::RequestGameplayTag(TEXT("Data.FallSeverity"), false);
	static const FGameplayTag TagDamageTypeFall =
		FGameplayTag::RequestGameplayTag(TEXT("Data.DamageType.Fall"), false);

	if (TagDamageTypeFall.IsValid())
	{
		FGameplayTagContainer DamageTags;
		DamageTags.AddTag(TagDamageTypeFall);
		SpecHandle.Data->AppendDynamicAssetTags(DamageTags);
	}

	SpecHandle.Data->SetSetByCallerMagnitude(TagDataDamage, Damage);
	SpecHandle.Data->SetSetByCallerMagnitude(TagDataFallSeverity, Severity);
	SpecHandle.Data->SetSetByCallerMagnitude(TagDamageTypeFall, 1.f);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
