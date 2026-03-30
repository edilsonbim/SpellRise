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

	if (bCommitAbilityOnActivate && !CommitAbility(Handle, ActorInfo, ActivationInfo))
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

	// Buffer controlado: clique rápido pode ser reconhecido ao abrir a janela,
	// mas sem consumir mais de uma continuação por janela.
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

	// Libera o filtro do primeiro clique após soltar o botão,
	// evitando que um tap rápido "mate" o próximo input válido.
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
		// Force manual combo flow: never auto-chain to the next section.
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

	// Segurança: se a seção anterior perdeu HitScanEnd por timing de notify/blend,
	// garantimos reset antes de abrir nova janela.
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
	// Se não houve input dentro da janela, não deve "bufferizar" para depois.
	// Isso garante que o combo encerre naturalmente ao final da seção atual.
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

	// Se não conseguir resolver a seção atual, cai para o índice runtime (melhor que travar).
	const int32 ResolvedIndex = (CurrentSectionIndex != INDEX_NONE) ? CurrentSectionIndex : CurrentComboIndex;
	const int32 NextIndex = ResolvedIndex + 1;
	if (!ComboSections.IsValidIndex(NextIndex))
	{
		UE_LOG(
			LogSpellRiseCombo,
			Verbose,
			TEXT("[Combo] Advance blocked: no next section (Current=%s ResolvedIdx=%d NumSections=%d)."),
			*CurrentSection.ToString(),
			ResolvedIndex,
			ComboSections.Num());
		return false;
	}

	const FName NextSection = ComboSections[NextIndex];

	// Importante: NÃO "jump" imediato. Encadeia a próxima seção para começar
	// exatamente ao final da seção atual, mantendo sincronismo do montage.
	StopHitScanIfActive();
	AnimInstance->Montage_SetNextSection(CurrentSection, NextSection, ComboMontage);
	CurrentComboIndex = NextIndex;

	bQueuedNextInput = false;
	QueuedInputTimeSeconds = -1.0f;
	bConsumedThisWindow = true;

	K2_OnComboAdvanced(CurrentComboIndex, NextSection);
	UE_LOG(
		LogSpellRiseCombo,
		Verbose,
		TEXT("[Combo] Advanced to section %s (Index=%d)."),
		*NextSection.ToString(),
		CurrentComboIndex);
	return true;
}

bool USpellRiseGA_MeleeCombo::IsPayloadFromAvatar(const FGameplayEventData& Payload) const
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		return false;
	}

	// Notify events em montage frequentemente chegam sem instigator/target.
	// Como a WaitGameplayEvent está amarrada ao AvatarActor do ASC dono da ability,
	// payload anônimo aqui é válido para o contexto da ability ativa.
	const AActor* PayloadTarget = Payload.Target.Get();
	const AActor* PayloadInstigator = Payload.Instigator.Get();
	if (!PayloadTarget && !PayloadInstigator)
	{
		return true;
	}

	// If payload has explicit source/target, use them to filter foreign actors.
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
	UE_LOG(LogSpellRiseCombo, Verbose, TEXT("[Combo] HitScan force-stop."));
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
