#include "GA_Weapon_MeleeLight.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"
#include "SpellRise/Weapons/Data/DA_WeaponDefinition.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Math/UnrealMathUtility.h"

UGA_Weapon_MeleeLight::UGA_Weapon_MeleeLight()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// ✅ faz o server receber InputPressed/InputReleased para esta GA
	bReplicateInputDirectly = true;

	MaxComboHits = 3;
	ComboSections = { FName("Hit1"), FName("Hit2"), FName("Hit3") };

	ComboWindowBeginTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.Begin"), false);
	ComboWindowEndTag   = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.End"), false);
}

bool UGA_Weapon_MeleeLight::IsServer() const
{
	return CurrentActorInfo && CurrentActorInfo->IsNetAuthority();
}

AActor* UGA_Weapon_MeleeLight::GetAvatar() const
{
	return (CurrentActorInfo && CurrentActorInfo->AvatarActor.IsValid())
		? CurrentActorInfo->AvatarActor.Get()
		: nullptr;
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

void UGA_Weapon_MeleeLight::CacheWeaponDataAndMontage()
{
	CachedWeaponDA = nullptr;
	CachedAttackMontage = nullptr;

	AActor* Avatar = GetAvatar();
	if (!Avatar) return;

	if (USpellRiseWeaponComponent* WeaponComp = Avatar->FindComponentByClass<USpellRiseWeaponComponent>())
	{
		CachedWeaponDA = WeaponComp->GetEquippedWeapon();
	}

	// Prefer DA montage (AAA), fallback GA
	if (CachedWeaponDA && CachedWeaponDA->LightAttack.AttackMontage)
	{
		CachedAttackMontage = CachedWeaponDA->LightAttack.AttackMontage;
	}
	else
	{
		CachedAttackMontage = AttackMontage;
	}
}

float UGA_Weapon_MeleeLight::GetBaseDamageFromDAOrLegacy(int32 HitIndex) const
{
	if (CachedWeaponDA)
	{
		return FMath::Max(0.f, CachedWeaponDA->GetLightAttackFinalBaseDamage());
	}

	if (BaseDamagePerHit.Num() >= 3)
	{
		return FMath::Max(0.f, BaseDamagePerHit[FMath::Clamp(HitIndex, 0, 2)]);
	}

	return FMath::Max(0.f, BaseDamage);
}

float UGA_Weapon_MeleeLight::GetDamageScalingFromDA(int32 HitIndex) const
{
	if (!CachedWeaponDA) return 1.f;

	const TArray<float>& Arr = CachedWeaponDA->LightAttack.DamageScalingPerHit;
	if (!Arr.IsValidIndex(HitIndex)) return 1.f;

	return FMath::Max(0.f, Arr[HitIndex]);
}

float UGA_Weapon_MeleeLight::GetAttackSpeedMultiplierFromASC() const
{
	if (!CurrentActorInfo || !CurrentActorInfo->AbilitySystemComponent.IsValid())
	{
		return 1.f;
	}

	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	if (!ASC) return 1.f;

	const float ASM = ASC->GetNumericAttribute(UDerivedStatsAttributeSet::GetAttackSpeedMultiplierAttribute());
	return FMath::Clamp(ASM, AttackSpeedMin, AttackSpeedMax);
}

void UGA_Weapon_MeleeLight::ApplyCurrentMontagePlayRate(float NewRate) const
{
	NewRate = FMath::Clamp(NewRate, 0.05f, 5.0f);

	// Prefer ASC montage control (GAS aware)
	if (CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid())
	{
		if (UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get())
		{
			ASC->CurrentMontageSetPlayRate(NewRate);
			return;
		}
	}

	// Fallback direct anim instance
	if (UAnimInstance* Anim = GetAvatarAnimInstance())
	{
		if (CachedAttackMontage && Anim->Montage_IsPlaying(CachedAttackMontage))
		{
			Anim->Montage_SetPlayRate(CachedAttackMontage, NewRate);
		}
	}
}

void UGA_Weapon_MeleeLight::TryApplyHitOnceForCurrentWindow()
{
	if (bDidHitThisWindow) return;

	AActor* Avatar = GetAvatar();
	if (!Avatar || !DamageGE) return;

	USpellRiseWeaponComponent* WeaponComp = Avatar->FindComponentByClass<USpellRiseWeaponComponent>();
	if (!WeaponComp)
	{
		if (bDebug && IsServer())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Melee] Missing SpellRiseWeaponComponent on %s"), *GetNameSafe(Avatar));
		}
		return;
	}

	bDidHitThisWindow = true;

	const float DamageBase = GetBaseDamageFromDAOrLegacy(ComboIndex);
	const float Scaling    = GetDamageScalingFromDA(ComboIndex);

	WeaponComp->PerformLightAttack_TraceAndApplyDamage(
		Avatar,
		DamageGE,
		DamageBase,
		Scaling,
		(ECollisionChannel)TraceChannel.GetValue(),
		bDebug
	);
}

void UGA_Weapon_MeleeLight::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid() || !DamageGE)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CacheWeaponDataAndMontage();

	if (!CachedAttackMontage || ComboSections.Num() == 0)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ComboIndex = 0;
	bAcceptingComboInput = false;
	bComboInputQueued = false;

	StartWaitingForComboWindowBegin();
	StartWaitingForComboWindowEnd();

	StartComboAt(0);
}

void UGA_Weapon_MeleeLight::StartComboAt(int32 NewIndex)
{
	ComboIndex = FMath::Clamp(NewIndex, 0, MaxComboHits - 1);
	bDidHitThisWindow = false;

	if (!ComboSections.IsValidIndex(ComboIndex))
	{
		ComboIndex = FMath::Clamp(ComboIndex, 0, ComboSections.Num() - 1);
		if (!ComboSections.IsValidIndex(ComboIndex))
		{
			FinishCombo(true);
			return;
		}
	}

	UAnimInstance* Anim = GetAvatarAnimInstance();
	if (!CachedAttackMontage)
	{
		FinishCombo(true);
		return;
	}

	const float AttackSpeedMult = GetAttackSpeedMultiplierFromASC();
	const float EffectiveRate   = FMath::Clamp(MontagePlayRate * AttackSpeedMult, 0.05f, 5.0f);

	// If already playing: jump section and update rate
	if (Anim && Anim->Montage_IsPlaying(CachedAttackMontage))
	{
		if (CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid())
		{
			if (UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get())
			{
				ASC->CurrentMontageJumpToSection(ComboSections[ComboIndex]);
			}
		}
		else
		{
			Anim->Montage_JumpToSection(ComboSections[ComboIndex], CachedAttackMontage);
		}

		ApplyCurrentMontagePlayRate(EffectiveRate);

		if (bDebug && IsServer())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Melee] JumpToSection=%s Rate=%.2f (Base=%.2f ASM=%.2f)"),
				*ComboSections[ComboIndex].ToString(), EffectiveRate, MontagePlayRate, AttackSpeedMult);
		}

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
		CachedAttackMontage,
		EffectiveRate,
		ComboSections[ComboIndex],
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
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Weapon_MeleeLight::OnMontageInterrupted);
	MontageTask->ReadyForActivation();

	if (bDebug && IsServer())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Melee] Start Montage Rate=%.2f (Base=%.2f ASM=%.2f)"),
			EffectiveRate, MontagePlayRate, AttackSpeedMult);
	}
}

void UGA_Weapon_MeleeLight::StartWaitingForComboWindowBegin()
{
	if (!ComboWindowBeginTag.IsValid()) return;

	WaitComboBeginTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, ComboWindowBeginTag, nullptr,
		true,
		false
	);

	if (!WaitComboBeginTask) return;

	WaitComboBeginTask->EventReceived.AddDynamic(this, &UGA_Weapon_MeleeLight::OnComboWindowBegin);
	WaitComboBeginTask->ReadyForActivation();
}

void UGA_Weapon_MeleeLight::StartWaitingForComboWindowEnd()
{
	if (!ComboWindowEndTag.IsValid()) return;

	WaitComboEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, ComboWindowEndTag, nullptr,
		true,
		false
	);

	if (!WaitComboEndTask) return;

	WaitComboEndTask->EventReceived.AddDynamic(this, &UGA_Weapon_MeleeLight::OnComboWindowEnd);
	WaitComboEndTask->ReadyForActivation();
}

void UGA_Weapon_MeleeLight::InputPressed(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (!bAcceptingComboInput) return;
	if (ComboIndex >= MaxComboHits - 1) return;

	bComboInputQueued = true;
}

void UGA_Weapon_MeleeLight::OnComboWindowBegin(FGameplayEventData Payload)
{
	StartWaitingForComboWindowBegin();

	bAcceptingComboInput = true;
	bComboInputQueued = false;

	if (IsServer())
	{
		TryApplyHitOnceForCurrentWindow();
	}
}

void UGA_Weapon_MeleeLight::OnComboWindowEnd(FGameplayEventData Payload)
{
	StartWaitingForComboWindowEnd();

	bAcceptingComboInput = false;

	// Client predicts montage advance
	if (bServerAuthoritativeComboFlow && !IsServer())
	{
		if (bComboInputQueued)
		{
			bComboInputQueued = false;
			const int32 Next = ComboIndex + 1;
			if (Next <= MaxComboHits - 1)
			{
				StartComboAt(Next);
			}
		}
		return;
	}

	// Server decides advance
	if (bComboInputQueued)
	{
		bComboInputQueued = false;
		const int32 Next = ComboIndex + 1;
		StartComboAt(Next);
		return;
	}

	// No input: let montage finish naturally
}

void UGA_Weapon_MeleeLight::OnMontageCompleted()
{
	if (!bServerAuthoritativeComboFlow || IsServer())
	{
		FinishCombo(false);
	}
}

void UGA_Weapon_MeleeLight::OnMontageInterrupted()
{
	if (!bServerAuthoritativeComboFlow || IsServer())
	{
		FinishCombo(true);
	}
}

void UGA_Weapon_MeleeLight::FinishCombo(bool bCancelled)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bCancelled);
}

void UGA_Weapon_MeleeLight::StopAllTasks()
{
	if (MontageTask) { MontageTask->EndTask(); MontageTask = nullptr; }
	if (WaitComboBeginTask) { WaitComboBeginTask->EndTask(); WaitComboBeginTask = nullptr; }
	if (WaitComboEndTask) { WaitComboEndTask->EndTask(); WaitComboEndTask = nullptr; }
}

void UGA_Weapon_MeleeLight::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// ✅ If cancelling, stop montage explicitly
	if (bWasCancelled)
	{
		StopAttackMontage();
	}

	StopAllTasks();

	bAcceptingComboInput = false;
	bComboInputQueued = false;
	ComboIndex = 0;
	bDidHitThisWindow = false;

	CachedWeaponDA = nullptr;
	CachedAttackMontage = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Weapon_MeleeLight::StopAttackMontage()
{
	if (!CachedAttackMontage) return;

	if (CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid())
	{
		if (UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get())
		{
			ASC->CurrentMontageStop(StopBlendOutTime);
			return;
		}
	}

	if (UAnimInstance* Anim = GetAvatarAnimInstance())
	{
		Anim->Montage_Stop(StopBlendOutTime, CachedAttackMontage);
	}
}
