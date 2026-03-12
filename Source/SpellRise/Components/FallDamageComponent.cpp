// FallDamageComponent.cpp
#include "SpellRise/Components/FallDamageComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"

#include "PhysicalMaterials/PhysicalMaterial.h"

#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"

UFallDamageComponent::UFallDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(false); // lógica só no servidor
}

void UFallDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		SetComponentTickEnabled(false);
	}
}

void UFallDamageComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner() || !GetOwner()->HasAuthority() || !bFallTrackingActive || !OwnerCharacter)
	{
		return;
	}

	const float DownSpeedAbs = FMath::Max(0.f, -OwnerCharacter->GetVelocity().Z);
	MaxFallSpeedAbs = FMath::Max(MaxFallSpeedAbs, DownSpeedAbs);
}

void UFallDamageComponent::OnMovementModeChanged()
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !OwnerCharacter)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[FALL MMC] MovementMode=%d ShouldTrack=%d"),
		OwnerCharacter->GetCharacterMovement() ? (int32)OwnerCharacter->GetCharacterMovement()->MovementMode : -1,
		ShouldTrackFall() ? 1 : 0);

	if (ShouldTrackFall())
	{
		StartFallTracking();
	}
	else if (bFallTrackingActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FALL MMC] Stop tracking due movement mode change"));
		StopFallTracking();
	}
}

void UFallDamageComponent::OnLanded(const FHitResult& Hit)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !OwnerCharacter)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[FALL LANDED] Active=%d MaxSpeed=%.2f StartZ=%.2f CurrentZ=%.2f"),
		bFallTrackingActive ? 1 : 0,
		MaxFallSpeedAbs,
		FallStartZ,
		OwnerCharacter->GetActorLocation().Z);

	if (!bFallTrackingActive)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	const float FallDuration = Now - FallStartTime;
	const float DeltaZ = FallStartZ - OwnerCharacter->GetActorLocation().Z;

	UE_LOG(LogTemp, Warning, TEXT("[FALL CHECK] Duration=%.2f DeltaZ=%.2f MinTime=%.2f MinHeight=%.2f SafeSpeed=%.2f"),
		FallDuration,
		DeltaZ,
		MinFallTime,
		MinFallHeight,
		SafeSpeed);

	if (bIgnoreNextFallDamage || HasFallImmunity() || IsSafeSurface(Hit))
	{
		UE_LOG(LogTemp, Warning, TEXT("[FALL EXIT] Ignored by immunity/safe surface"));
		StopFallTracking();
		return;
	}

	if (FallDuration < MinFallTime || DeltaZ < MinFallHeight || MaxFallSpeedAbs < SafeSpeed)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FALL EXIT] Threshold fail"));
		StopFallTracking();
		return;
	}

	const float Severity = ComputeSeverity();
	UE_LOG(LogTemp, Warning, TEXT("[FALL SEVERITY] %.3f"), Severity);

	if (Severity <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FALL EXIT] Severity <= 0"));
		StopFallTracking();
		return;
	}

	const float DamagePercent = ComputeDamagePercent(Severity);
	float Damage = GetMaxHealth() * DamagePercent;
	Damage = ApplyMitigations(Damage, Hit);

	UE_LOG(LogTemp, Warning, TEXT("[FALL DAMAGE] Percent=%.3f MaxHealth=%.2f Final=%.2f"),
		DamagePercent,
		GetMaxHealth(),
		Damage);

	if (Damage > 0.f)
	{
		ApplyFallDamage(Damage, Severity, Hit);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[FALL EXIT] Final damage <= 0"));
	}

	StopFallTracking();
}

void UFallDamageComponent::StartFallTracking()
{
	if (!OwnerCharacter)
	{
		return;
	}

	bFallTrackingActive = true;
	FallStartZ = OwnerCharacter->GetActorLocation().Z;
	FallStartTime = GetWorld()->GetTimeSeconds();
	MaxFallSpeedAbs = FMath::Max(0.f, -OwnerCharacter->GetVelocity().Z);

	UE_LOG(LogTemp, Warning, TEXT("[FALL START] Z=%.2f Time=%.2f VelZ=%.2f"),
		FallStartZ,
		FallStartTime,
		OwnerCharacter->GetVelocity().Z);
}

void UFallDamageComponent::StopFallTracking()
{
	UE_LOG(LogTemp, Warning, TEXT("[FALL STOP]"));

	bFallTrackingActive = false;
	FallStartZ = 0.f;
	FallStartTime = 0.f;
	MaxFallSpeedAbs = 0.f;
	bIgnoreNextFallDamage = false;
}

bool UFallDamageComponent::ShouldTrackFall() const
{
	if (!OwnerCharacter)
	{
		return false;
	}

	const UCharacterMovementComponent* Move = OwnerCharacter->GetCharacterMovement();
	return Move && Move->MovementMode == MOVE_Falling;
}

bool UFallDamageComponent::HasFallImmunity() const
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

bool UFallDamageComponent::IsSafeSurface(const FHitResult& Hit) const
{
	const UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
	if (!PhysMat)
	{
		return false;
	}

	const FString Name = PhysMat->GetName();
	return Name.Contains(TEXT("Water")) || Name.Contains(TEXT("Hay"));
}

float UFallDamageComponent::ComputeSeverity() const
{
	const float Alpha = (MaxFallSpeedAbs - SafeSpeed) / FMath::Max(1.f, (FatalSpeed - SafeSpeed));
	return FMath::Clamp(Alpha, 0.f, 1.f);
}

float UFallDamageComponent::ComputeDamagePercent(float Severity) const
{
	if (FallDamagePercentCurve)
	{
		return FMath::Max(0.f, FallDamagePercentCurve->GetFloatValue(Severity));
	}

	return FMath::Pow(Severity, 1.35f);
}

float UFallDamageComponent::ApplyMitigations(float Damage, const FHitResult& Hit) const
{
	float Result = Damage;

	const float MaxStamina = GetMaxStamina();
	if (MaxStamina > 0.f)
	{
		const float StaminaPct = GetCurrentStamina() / MaxStamina;

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

UAbilitySystemComponent* UFallDamageComponent::GetASC() const
{
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
}

float UFallDamageComponent::GetMaxHealth() const
{
	const UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return 0.f;
	}

	return ASC->GetNumericAttribute(UResourceAttributeSet::GetMaxHealthAttribute());
}

float UFallDamageComponent::GetCurrentStamina() const
{
	const UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return 0.f;
	}

	return ASC->GetNumericAttribute(UResourceAttributeSet::GetStaminaAttribute());
}

float UFallDamageComponent::GetMaxStamina() const
{
	const UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return 0.f;
	}

	return ASC->GetNumericAttribute(UResourceAttributeSet::GetMaxStaminaAttribute());
}

void UFallDamageComponent::ApplyFallDamage(float Damage, float Severity, const FHitResult& Hit)
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC || !GE_FallDamage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FALL APPLY] ASC or GE_FallDamage missing"));
		return;
	}

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddHitResult(Hit);
	Context.AddSourceObject(GetOwner());

	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_FallDamage, 1.f, Context);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[FALL APPLY] Invalid SpecHandle"));
		return;
	}

	static const FGameplayTag TagDataDamage =
		FGameplayTag::RequestGameplayTag(TEXT("Data.Damage"), false);

	static const FGameplayTag TagDataFallSeverity =
		FGameplayTag::RequestGameplayTag(TEXT("Data.FallSeverity"), false);

	static const FGameplayTag TagDamageTypeFall =
		FGameplayTag::RequestGameplayTag(TEXT("Data.DamageType.Fall"), false);

	UE_LOG(LogTemp, Warning, TEXT("[FALL APPLY] GE=%s Damage=%.2f Severity=%.3f TagsValid D=%d S=%d F=%d"),
		*GetNameSafe(GE_FallDamage),
		Damage,
		Severity,
		TagDataDamage.IsValid() ? 1 : 0,
		TagDataFallSeverity.IsValid() ? 1 : 0,
		TagDamageTypeFall.IsValid() ? 1 : 0);

	SpecHandle.Data->SetSetByCallerMagnitude(TagDataDamage, Damage);
	SpecHandle.Data->SetSetByCallerMagnitude(TagDataFallSeverity, Severity);
	SpecHandle.Data->SetSetByCallerMagnitude(TagDamageTypeFall, 1.f);

	UE_LOG(LogTemp, Warning, TEXT("[FALL APPLY] SBC Readback Damage=%.2f Severity=%.3f Fall=%.2f"),
		SpecHandle.Data->GetSetByCallerMagnitude(TagDataDamage, false, -1.f),
		SpecHandle.Data->GetSetByCallerMagnitude(TagDataFallSeverity, false, -1.f),
		SpecHandle.Data->GetSetByCallerMagnitude(TagDamageTypeFall, false, -1.f));

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}