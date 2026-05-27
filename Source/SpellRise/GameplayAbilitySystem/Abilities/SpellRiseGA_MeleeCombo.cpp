// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGA_MeleeCombo.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseCombo, Log, All);

USpellRiseGA_MeleeCombo::USpellRiseGA_MeleeCombo()
{
	ComboWindowStartTag = FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.Start"), false);
	ComboWindowEndTag = FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.End"), false);
	HitScanStartTag = FGameplayTag::RequestGameplayTag(TEXT("Event.HitScan.Start"), false);
	HitScanEndTag = FGameplayTag::RequestGameplayTag(TEXT("Event.HitScan.End"), false);
}

void USpellRiseGA_MeleeCombo::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ResetRuntimeState();
	bIgnoreFirstActivationPress = true;

	if (!ComboMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	InitializeComboSections();
	if (ComboSections.Num() <= 0)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	StartComboTasks();
}

void USpellRiseGA_MeleeCombo::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	StopHitScanIfActive();
	StopComboTasks();
	ResetRuntimeState();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USpellRiseGA_MeleeCombo::NativeOnAbilityInputPressed(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::NativeOnAbilityInputPressed(Handle, ActorInfo, ActivationInfo);

	if (bIgnoreFirstActivationPress)
	{
		bIgnoreFirstActivationPress = false;
		return;
	}



	bQueuedNextInput = true;
	QueuedInputTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	if (!bComboWindowOpen)
	{
		return;
	}

	TryAdvanceComboSection();
}

void USpellRiseGA_MeleeCombo::NativeOnAbilityInputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::NativeOnAbilityInputReleased(Handle, ActorInfo, ActivationInfo);



	bIgnoreFirstActivationPress = false;
}

void USpellRiseGA_MeleeCombo::OnComboMontageCompleted()
{
	StopHitScanIfActive();
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void USpellRiseGA_MeleeCombo::OnComboMontageBlendedIn()
{
	UAnimInstance* AnimInstance = GetComboAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	for (int32 Index = 0; Index + 1 < ComboSections.Num(); ++Index)
	{

		AnimInstance->Montage_SetNextSection(ComboSections[Index], NAME_None, ComboMontage);
	}
}

void USpellRiseGA_MeleeCombo::OnComboMontageBlendOut()
{
	StopHitScanIfActive();
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void USpellRiseGA_MeleeCombo::OnComboMontageInterrupted()
{
	StopHitScanIfActive();
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void USpellRiseGA_MeleeCombo::OnComboMontageCancelled()
{
	StopHitScanIfActive();
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void USpellRiseGA_MeleeCombo::OnComboWindowStart(FGameplayEventData Payload)
{
	if (!IsPayloadFromAvatar(Payload))
	{
		return;
	}



	StopHitScanIfActive(&Payload);

	bComboWindowOpen = true;
	bConsumedThisWindow = false;
	K2_OnComboWindowOpened(CurrentComboIndex);
	TryAdvanceComboSection();
}

void USpellRiseGA_MeleeCombo::OnComboWindowEnd(FGameplayEventData Payload)
{
	if (!IsPayloadFromAvatar(Payload))
	{
		return;
	}

	bComboWindowOpen = false;
	bConsumedThisWindow = false;
	StopHitScanIfActive(&Payload);


	bQueuedNextInput = false;
	QueuedInputTimeSeconds = -1.0f;
	K2_OnComboWindowClosed(CurrentComboIndex);
}

void USpellRiseGA_MeleeCombo::OnHitScanStart(FGameplayEventData Payload)
{
	if (!IsPayloadFromAvatar(Payload))
	{
		return;
	}

	bHitScanActive = true;
	K2_OnHitScanStart(Payload);
}

void USpellRiseGA_MeleeCombo::OnHitScanEnd(FGameplayEventData Payload)
{
	if (!IsPayloadFromAvatar(Payload))
	{
		return;
	}

	bHitScanActive = false;
	K2_OnHitScanEnd(Payload);
}

void USpellRiseGA_MeleeCombo::InitializeComboSections()
{
	if (ComboSections.Num() > 0)
	{
		return;
	}

	for (const FCompositeSection& Section : ComboMontage->CompositeSections)
	{
		if (!Section.SectionName.IsNone())
		{
			ComboSections.Add(Section.SectionName);
		}
	}
}

UAnimInstance* USpellRiseGA_MeleeCombo::GetComboAnimInstance() const
{
	if (CurrentActorInfo)
	{
		if (UAnimInstance* ActorInfoAnim = CurrentActorInfo->GetAnimInstance())
		{
			return ActorInfoAnim;
		}
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !Character->GetMesh())
	{
		return nullptr;
	}

	return Character->GetMesh()->GetAnimInstance();
}

void USpellRiseGA_MeleeCombo::StartComboTasks()
{
	const FName StartSection = ComboSections.IsValidIndex(0) ? ComboSections[0] : NAME_None;

	ComboMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		ComboMontage,
		PlayRate,
		StartSection,
		bStopMontageWhenAbilityEnds);

	if (ComboMontageTask)
	{
		ComboMontageTask->OnBlendedIn.AddDynamic(this, &USpellRiseGA_MeleeCombo::OnComboMontageBlendedIn);
		ComboMontageTask->OnCompleted.AddDynamic(this, &USpellRiseGA_MeleeCombo::OnComboMontageCompleted);
		ComboMontageTask->OnBlendOut.AddDynamic(this, &USpellRiseGA_MeleeCombo::OnComboMontageBlendOut);
		ComboMontageTask->OnInterrupted.AddDynamic(this, &USpellRiseGA_MeleeCombo::OnComboMontageInterrupted);
		ComboMontageTask->OnCancelled.AddDynamic(this, &USpellRiseGA_MeleeCombo::OnComboMontageCancelled);
		ComboMontageTask->ReadyForActivation();
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	if (ComboWindowStartTag.IsValid())
	{
		ComboWindowStartTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ComboWindowStartTag, AvatarActor, false, true);
		if (ComboWindowStartTask)
		{
			ComboWindowStartTask->EventReceived.AddDynamic(this, &USpellRiseGA_MeleeCombo::OnComboWindowStart);
			ComboWindowStartTask->ReadyForActivation();
		}
	}

	if (ComboWindowEndTag.IsValid())
	{
		ComboWindowEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ComboWindowEndTag, AvatarActor, false, true);
		if (ComboWindowEndTask)
		{
			ComboWindowEndTask->EventReceived.AddDynamic(this, &USpellRiseGA_MeleeCombo::OnComboWindowEnd);
			ComboWindowEndTask->ReadyForActivation();
		}
	}

	if (HitScanStartTag.IsValid())
	{
		HitScanStartTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitScanStartTag, AvatarActor, false, true);
		if (HitScanStartTask)
		{
			HitScanStartTask->EventReceived.AddDynamic(this, &USpellRiseGA_MeleeCombo::OnHitScanStart);
			HitScanStartTask->ReadyForActivation();
		}
	}

	if (HitScanEndTag.IsValid())
	{
		HitScanEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitScanEndTag, AvatarActor, false, true);
		if (HitScanEndTask)
		{
			HitScanEndTask->EventReceived.AddDynamic(this, &USpellRiseGA_MeleeCombo::OnHitScanEnd);
			HitScanEndTask->ReadyForActivation();
		}
	}
}

void USpellRiseGA_MeleeCombo::StopComboTasks()
{
	if (ComboMontageTask)
	{
		ComboMontageTask->EndTask();
		ComboMontageTask = nullptr;
	}

	if (ComboWindowStartTask)
	{
		ComboWindowStartTask->EndTask();
		ComboWindowStartTask = nullptr;
	}

	if (ComboWindowEndTask)
	{
		ComboWindowEndTask->EndTask();
		ComboWindowEndTask = nullptr;
	}

	if (HitScanStartTask)
	{
		HitScanStartTask->EndTask();
		HitScanStartTask = nullptr;
	}

	if (HitScanEndTask)
	{
		HitScanEndTask->EndTask();
		HitScanEndTask = nullptr;
	}
}

void USpellRiseGA_MeleeCombo::ResetRuntimeState()
{
	CurrentComboIndex = 0;
	bComboWindowOpen = false;
	bQueuedNextInput = false;
	bConsumedThisWindow = false;
	bIgnoreFirstActivationPress = false;
	bHitScanActive = false;
	QueuedInputTimeSeconds = -1.0f;
}

bool USpellRiseGA_MeleeCombo::TryAdvanceComboSection()
{
	if (!bComboWindowOpen || !bQueuedNextInput || bConsumedThisWindow)
	{
		return false;
	}

	if (!IsQueuedInputStillValid())
	{
		bQueuedNextInput = false;
		QueuedInputTimeSeconds = -1.0f;
		return false;
	}

	UAnimInstance* AnimInstance = GetComboAnimInstance();
	if (!AnimInstance)
	{
		return false;
	}

	UAnimMontage* ActiveMontage = AnimInstance->GetCurrentActiveMontage();
	if (!ActiveMontage)
	{
		return false;
	}

	if (ActiveMontage != ComboMontage)
	{
		return false;
	}

	const FName CurrentSection = AnimInstance->Montage_GetCurrentSection(ComboMontage);
	if (CurrentSection.IsNone())
	{
		return false;
	}
	const int32 CurrentSectionIndex = ComboMontage->GetSectionIndex(CurrentSection);


	const int32 ResolvedIndex = (CurrentSectionIndex != INDEX_NONE) ? CurrentSectionIndex : CurrentComboIndex;
	const int32 NextIndex = ResolvedIndex + 1;
	if (!ComboSections.IsValidIndex(NextIndex))
	{
		return false;
	}

	const FName NextSection = ComboSections[NextIndex];



	StopHitScanIfActive();
	AnimInstance->Montage_SetNextSection(CurrentSection, NextSection, ComboMontage);
	CurrentComboIndex = NextIndex;

	bQueuedNextInput = false;
	QueuedInputTimeSeconds = -1.0f;
	bConsumedThisWindow = true;

	K2_OnComboAdvanced(CurrentComboIndex, NextSection);
	return true;
}

bool USpellRiseGA_MeleeCombo::IsPayloadFromAvatar(const FGameplayEventData& Payload) const
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		return false;
	}




	const AActor* PayloadTarget = Payload.Target.Get();
	const AActor* PayloadInstigator = Payload.Instigator.Get();
	if (!PayloadTarget && !PayloadInstigator)
	{
		return true;
	}


	if (PayloadTarget && PayloadTarget != Avatar)
	{
		return false;
	}
	if (PayloadInstigator && PayloadInstigator != Avatar)
	{
		return false;
	}

	return true;
}

void USpellRiseGA_MeleeCombo::StopHitScanIfActive(const FGameplayEventData* Payload)
{
	if (!bHitScanActive)
	{
		return;
	}

	bHitScanActive = false;
	if (Payload)
	{
		K2_OnHitScanEnd(*Payload);
		return;
	}

	FGameplayEventData EmptyPayload;
	K2_OnHitScanEnd(EmptyPayload);
}

bool USpellRiseGA_MeleeCombo::IsQueuedInputStillValid() const
{
	if (!bQueuedNextInput)
	{
		return false;
	}

	if (QueuedInputTimeSeconds < 0.0f)
	{
		return true;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return true;
	}

	return (World->GetTimeSeconds() - QueuedInputTimeSeconds) <= InputBufferGraceSeconds;
}
