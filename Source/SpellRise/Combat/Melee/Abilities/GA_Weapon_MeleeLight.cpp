#include "GA_Weapon_MeleeLight.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Math/UnrealMathUtility.h"

#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"
#include "SpellRise/Combat/Melee/Runtime/SpellRiseMeleeComponent.h"
#include "SpellRise/Combat/Melee/Data/DA_MeleeWeaponData.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"

UGA_Weapon_MeleeLight::UGA_Weapon_MeleeLight()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	bReplicateInputDirectly = true;

	StopBlendOutTime = 0.15f;
}

bool UGA_Weapon_MeleeLight::IsServer() const
{
	return CurrentActorInfo && CurrentActorInfo->IsNetAuthority();
}

bool UGA_Weapon_MeleeLight::IsLocallyControlled() const
{
	return CurrentActorInfo && CurrentActorInfo->IsLocallyControlled();
}

AActor* UGA_Weapon_MeleeLight::GetAvatar() const
{
	return (CurrentActorInfo && CurrentActorInfo->AvatarActor.IsValid())
		? CurrentActorInfo->AvatarActor.Get()
		: nullptr;
}

APawn* UGA_Weapon_MeleeLight::GetAvatarPawn() const
{
	return Cast<APawn>(GetAvatar());
}

UAbilitySystemComponent* UGA_Weapon_MeleeLight::GetASC() const
{
	return CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr;
}

UAnimInstance* UGA_Weapon_MeleeLight::GetAvatarAnimInstance() const
{
	AActor* Avatar = GetAvatar();
	if (!Avatar) return nullptr;

	if (ACharacter* Char = Cast<ACharacter>(Avatar))
	{
		if (USkeletalMeshComponent* MeshComp = Char->GetMesh())
		{
			return MeshComp->GetAnimInstance();
		}
	}

	if (USkeletalMeshComponent* MeshComp = Avatar->FindComponentByClass<USkeletalMeshComponent>())
	{
		return MeshComp->GetAnimInstance();
	}

	return nullptr;
}

float UGA_Weapon_MeleeLight::GetAimYaw() const
{
	const AActor* Avatar = GetAvatar();
	if (!Avatar) return 0.f;

	const APawn* Pawn = Cast<APawn>(Avatar);
	const AController* C = Pawn ? Pawn->GetController() : nullptr;

	const FRotator YawRot = C ? C->GetControlRotation() : Avatar->GetActorRotation();
	return YawRot.Yaw;
}

void UGA_Weapon_MeleeLight::CacheWeaponData()
{
	CachedMeleeComponent.Reset();
	CachedAttackMontage = nullptr;
	CachedComboSections.Empty();
	CachedMaxComboHits = 3;
	CachedComboWindowBeginTag = FGameplayTag();
	CachedComboWindowEndTag = FGameplayTag();

	AActor* Avatar = GetAvatar();
	if (!Avatar) return;

	USpellRiseWeaponComponent* WeaponComp = Avatar->FindComponentByClass<USpellRiseWeaponComponent>();
	if (!WeaponComp) return;

	CachedMeleeComponent = WeaponComp->GetMeleeComponent();

	if (WeaponComp->GetEquippedWeapon())
	{
		const UDA_MeleeWeaponData* Data = WeaponComp->GetEquippedWeapon();
		CachedAttackMontage = Data->AttackMontage;
		CachedComboSections = Data->ComboConfig.ComboSections;
		CachedMaxComboHits = Data->ComboConfig.MaxComboHits;
		CachedComboWindowBeginTag = Data->ComboConfig.ComboWindowBeginTag;
		CachedComboWindowEndTag = Data->ComboConfig.ComboWindowEndTag;
	}

	if (!CachedAttackMontage)
	{
		CachedAttackMontage = AttackMontage_Legacy;
	}

	if (CachedComboSections.Num() == 0)
	{
		CachedComboSections = { FName("Hit1"), FName("Hit2"), FName("Hit3") };
	}

	if (!CachedComboWindowBeginTag.IsValid())
	{
		CachedComboWindowBeginTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.Begin"), false);
	}
	if (!CachedComboWindowEndTag.IsValid())
	{
		CachedComboWindowEndTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.End"), false);
	}
}

void UGA_Weapon_MeleeLight::InitializeMeleeComponent()
{
	AActor* Avatar = GetAvatar();
	if (!Avatar) return;

	USpellRiseWeaponComponent* WeaponComp = Avatar->FindComponentByClass<USpellRiseWeaponComponent>();
	if (!WeaponComp) return;

	WeaponComp->InitializeMeleeComponent();
	CachedMeleeComponent = WeaponComp->GetMeleeComponent();
}

float UGA_Weapon_MeleeLight::GetAttackSpeedMultiplierFromASC() const
{
	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC) return 1.f;

	const float ASM = ASC->GetNumericAttribute(UDerivedStatsAttributeSet::GetAttackSpeedMultiplierAttribute());
	return FMath::Clamp(ASM, AttackSpeedMin, AttackSpeedMax);
}

void UGA_Weapon_MeleeLight::ApplyCurrentMontagePlayRate(float NewRate) const
{
	NewRate = FMath::Clamp(NewRate, 0.05f, 5.0f);

	if (UAbilitySystemComponent* ASC = GetASC())
	{
		ASC->CurrentMontageSetPlayRate(NewRate);
		return;
	}

	if (UAnimInstance* Anim = GetAvatarAnimInstance())
	{
		if (CachedAttackMontage && Anim->Montage_IsPlaying(CachedAttackMontage.Get()))
		{
			Anim->Montage_SetPlayRate(CachedAttackMontage.Get(), NewRate);
		}
	}
}

int32 UGA_Weapon_MeleeLight::ExtractComboIndexFromPayload(const FGameplayEventData& Payload) const
{
	const int32 Idx = FMath::RoundToInt(Payload.EventMagnitude);
	return FMath::Clamp(Idx, 0, FMath::Max(0, CachedMaxComboHits - 1));
}

void UGA_Weapon_MeleeLight::StartComboAt(int32 NewIndex)
{
	if (!CachedComboSections.IsValidIndex(NewIndex) || NewIndex >= CachedMaxComboHits)
	{
		FinishCombo(false);
		return;
	}

	ComboIndex = NewIndex;

	UAnimInstance* Anim = GetAvatarAnimInstance();
	if (!Anim || !CachedAttackMontage)
	{
		FinishCombo(true);
		return;
	}

	const float AttackSpeedMult = GetAttackSpeedMultiplierFromASC();
	const float EffectiveRate = FMath::Clamp(MontagePlayRate * AttackSpeedMult, 0.05f, 5.0f);

	if (Anim->Montage_IsPlaying(CachedAttackMontage.Get()))
	{
		if (UAbilitySystemComponent* ASC = GetASC())
		{
			ASC->CurrentMontageJumpToSection(CachedComboSections[ComboIndex]);
		}
		else
		{
			Anim->Montage_JumpToSection(CachedComboSections[ComboIndex], CachedAttackMontage.Get());
		}

		ApplyCurrentMontagePlayRate(EffectiveRate);
		return;
	}

	if (MontageTask)
	{
		MontageTask->EndTask();
		MontageTask = nullptr;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		CachedAttackMontage.Get(),
		EffectiveRate,
		CachedComboSections[ComboIndex],
		false,
		1.0f
	);

	if (!MontageTask)
	{
		FinishCombo(true);
		return;
	}

	MontageTask->OnCompleted.AddDynamic(this, &UGA_Weapon_MeleeLight::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Weapon_MeleeLight::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Weapon_MeleeLight::OnMontageCancelled);
	MontageTask->ReadyForActivation();
}

void UGA_Weapon_MeleeLight::FireLocalStartAttackForWindow(int32 WindowIdx)
{
	// Gameplay should originate from owning client (predicted) and be validated on server.
	if (!IsLocallyControlled())
	{
		return;
	}

	if (!CachedMeleeComponent.IsValid())
	{
		InitializeMeleeComponent();
	}

	USpellRiseMeleeComponent* MC = CachedMeleeComponent.Get();
	if (!MC)
	{
		return;
	}

	MC->LocalStartAttack(
		/*AttackId=*/AttackId,
		/*Variation=*/static_cast<uint8>(FMath::Clamp(WindowIdx, 0, 255)),
		/*AimYaw=*/GetAimYaw()
	);

	if (bDebug)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MeleeLight] LocalStartAttack fired. WindowIdx=%d AttackId=%d"), WindowIdx, AttackId);
	}
}

void UGA_Weapon_MeleeLight::StartWaitingForComboWindowBegin()
{
	if (!CachedComboWindowBeginTag.IsValid())
		return;

	if (WaitComboBeginTask)
	{
		WaitComboBeginTask->EndTask();
		WaitComboBeginTask = nullptr;
	}

	WaitComboBeginTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CachedComboWindowBeginTag, nullptr,
		/*OnlyTriggerOnce=*/false,
		/*OnlyMatchExact=*/false
	);

	if (!WaitComboBeginTask) return;

	WaitComboBeginTask->EventReceived.AddDynamic(this, &UGA_Weapon_MeleeLight::OnComboWindowBegin);
	WaitComboBeginTask->ReadyForActivation();
}

void UGA_Weapon_MeleeLight::StartWaitingForComboWindowEnd()
{
	if (!CachedComboWindowEndTag.IsValid())
		return;

	if (WaitComboEndTask)
	{
		WaitComboEndTask->EndTask();
		WaitComboEndTask = nullptr;
	}

	WaitComboEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CachedComboWindowEndTag, nullptr,
		/*OnlyTriggerOnce=*/false,
		/*OnlyMatchExact=*/false
	);

	if (!WaitComboEndTask) return;

	WaitComboEndTask->EventReceived.AddDynamic(this, &UGA_Weapon_MeleeLight::OnComboWindowEnd);
	WaitComboEndTask->ReadyForActivation();
}

void UGA_Weapon_MeleeLight::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CacheWeaponData();

	if (!CachedAttackMontage || CachedComboSections.Num() == 0)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	InitializeMeleeComponent();

	ComboIndex = 0;
	bAcceptingComboInput = false;
	bComboInputQueued = false;

	StartWaitingForComboWindowBegin();
	StartWaitingForComboWindowEnd();

	// Plays Hit1. The actual gameplay (attack start) is fired when ComboWindowBegin event arrives.
	StartComboAt(0);
}

void UGA_Weapon_MeleeLight::InputPressed(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	// Only owning client queues combo input.
	if (!IsLocallyControlled()) return;

	if (!bAcceptingComboInput) return;
	if (ComboIndex >= CachedMaxComboHits - 1) return;

	bComboInputQueued = true;

	if (bDebug)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MeleeLight] Input queued (ComboIndex=%d)"), ComboIndex);
	}
}

void UGA_Weapon_MeleeLight::OnComboWindowBegin(FGameplayEventData Payload)
{
	const int32 WindowIdx = ExtractComboIndexFromPayload(Payload);

	// input window begins (owning client)
	if (IsLocallyControlled())
	{
		bAcceptingComboInput = true;
	}

	// Align internal index (optional but useful)
	ComboIndex = WindowIdx;

	// ✅ This is the AAA timing hook: start attack when the montage window opens.
	FireLocalStartAttackForWindow(WindowIdx);
}

void UGA_Weapon_MeleeLight::OnComboWindowEnd(FGameplayEventData Payload)
{
	(void)Payload;

	if (IsLocallyControlled())
	{
		bAcceptingComboInput = false;
	}

	const bool bCanAdvance = (ComboIndex < CachedMaxComboHits - 1);
	const bool bWantsAdvance = bComboInputQueued && bCanAdvance;

	bComboInputQueued = false;

	if (bWantsAdvance)
	{
		StartComboAt(ComboIndex + 1);
		return;
	}

	StopAttackMontage();
	FinishCombo(true);
}

void UGA_Weapon_MeleeLight::OnMontageCompleted()
{
	// Montage ended naturally; if you want auto-chain, do it here. For now, end.
	FinishCombo(false);
}

void UGA_Weapon_MeleeLight::OnMontageInterrupted()
{
	FinishCombo(true);
}

void UGA_Weapon_MeleeLight::OnMontageCancelled()
{
	FinishCombo(true);
}

void UGA_Weapon_MeleeLight::StopAttackMontage()
{
	if (!CachedAttackMontage) return;

	if (UAbilitySystemComponent* ASC = GetASC())
	{
		ASC->CurrentMontageStop(StopBlendOutTime);
		return;
	}

	if (UAnimInstance* Anim = GetAvatarAnimInstance())
	{
		Anim->Montage_Stop(StopBlendOutTime, CachedAttackMontage.Get());
	}
}

void UGA_Weapon_MeleeLight::StopAllTasks()
{
	if (MontageTask) { MontageTask->EndTask(); MontageTask = nullptr; }
	if (WaitComboBeginTask) { WaitComboBeginTask->EndTask(); WaitComboBeginTask = nullptr; }
	if (WaitComboEndTask) { WaitComboEndTask->EndTask(); WaitComboEndTask = nullptr; }
}

void UGA_Weapon_MeleeLight::FinishCombo(bool bCancelled)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bCancelled);
}

void UGA_Weapon_MeleeLight::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	if (bWasCancelled)
	{
		StopAttackMontage();
	}

	StopAllTasks();

	bAcceptingComboInput = false;
	bComboInputQueued = false;
	ComboIndex = 0;

	CachedMeleeComponent.Reset();
	CachedAttackMontage = nullptr;
	CachedComboSections.Empty();
	CachedMaxComboHits = 3;
	CachedComboWindowBeginTag = FGameplayTag();
	CachedComboWindowEndTag = FGameplayTag();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
