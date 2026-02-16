// GA_Weapon_MeleeLight.cpp (COMPLETO - corrigido para DS + Owning Client)
// - Predição local de transição de seção (cliente) + autoridade no server
// - Helpers implementados (corrige LNK2019 / members missing)
// - ComboIndex sincroniza pela seção atual do montage
// - Server continua podendo decidir via timer, mas o client não fica preso em Hit1

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

// --------------------------
// Helpers (NOVO)
// --------------------------

bool UGA_Weapon_MeleeLight::IsMontagePlaying() const
{
	if (!CachedAttackMontage) return false;

	if (UAbilitySystemComponent* ASC = GetASC())
	{
		return ASC->GetCurrentMontage() == CachedAttackMontage.Get();
	}

	if (UAnimInstance* Anim = GetAvatarAnimInstance())
	{
		return Anim->Montage_IsPlaying(CachedAttackMontage.Get());
	}

	return false;
}

int32 UGA_Weapon_MeleeLight::GetCurrentComboIndexFromMontage() const
{
	if (!CachedAttackMontage || CachedComboSections.Num() == 0)
	{
		return ComboIndex;
	}

	FName CurrentSection = NAME_None;

	// Prefer AnimInstance query (mais confiável para seção atual)
	if (UAnimInstance* Anim = GetAvatarAnimInstance())
	{
		if (Anim->Montage_IsPlaying(CachedAttackMontage.Get()))
		{
			CurrentSection = Anim->Montage_GetCurrentSection(CachedAttackMontage.Get());
		}
	}

	// Fallback: se não achou, mantém ComboIndex
	if (CurrentSection.IsNone())
	{
		return ComboIndex;
	}

	for (int32 i = 0; i < CachedComboSections.Num(); i++)
	{
		if (CachedComboSections[i] == CurrentSection)
		{
			return i;
		}
	}

	return ComboIndex;
}

void UGA_Weapon_MeleeLight::SetNextSection_LocalOrASC(const FName& From, const FName& To)
{
	if (!CachedAttackMontage) return;

	if (UAbilitySystemComponent* ASC = GetASC())
	{
		ASC->CurrentMontageSetNextSectionName(From, To);
		return;
	}

	if (UAnimInstance* Anim = GetAvatarAnimInstance())
	{
		Anim->Montage_SetNextSection(From, To, CachedAttackMontage.Get());
	}
}

// --------------------------
// Weapon cache/init
// --------------------------

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
		const UDA_MeleeWeaponData* Data = WeaponComp->GetEquippedMeleeWeapon();
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

void UGA_Weapon_MeleeLight::ClearAutoChainLinks() const
{
	// Mata autochain do asset em runtime
	if (!CachedAttackMontage || CachedComboSections.Num() == 0)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = GetASC())
	{
		for (const FName& Sec : CachedComboSections)
		{
			ASC->CurrentMontageSetNextSectionName(Sec, NAME_None);
		}
		return;
	}

	if (UAnimInstance* Anim = GetAvatarAnimInstance())
	{
		for (const FName& Sec : CachedComboSections)
		{
			Anim->Montage_SetNextSection(Sec, NAME_None, CachedAttackMontage.Get());
		}
	}
}

// --------------------------
// Server timer decide avanço
// --------------------------

void UGA_Weapon_MeleeLight::ScheduleServerSectionEndTimer()
{
	if (!IsServer())
	{
		return;
	}

	AActor* Avatar = GetAvatar();
	if (!Avatar || !CachedAttackMontage)
	{
		return;
	}

	UWorld* World = Avatar->GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(ServerSectionEndTimer);

	// Re-leitura segura do índice pela seção atual
	ComboIndex = GetCurrentComboIndexFromMontage();
	if (!CachedComboSections.IsValidIndex(ComboIndex))
	{
		return;
	}

	const int32 SectionIdx = CachedAttackMontage->GetSectionIndex(CachedComboSections[ComboIndex]);
	if (SectionIdx == INDEX_NONE)
	{
		return;
	}

	const float AttackSpeedMult = GetAttackSpeedMultiplierFromASC();
	const float EffectiveRate = FMath::Clamp(MontagePlayRate * AttackSpeedMult, 0.05f, 5.0f);

	const float SectionLen = CachedAttackMontage->GetSectionLength(SectionIdx);

	const float LeadTime = 0.05f;
	const float Delay = FMath::Max(0.02f, (SectionLen / EffectiveRate) - LeadTime);

	World->GetTimerManager().SetTimer(
		ServerSectionEndTimer,
		this,
		&UGA_Weapon_MeleeLight::OnServerSectionEnd,
		Delay,
		false
	);
}

void UGA_Weapon_MeleeLight::OnServerSectionEnd()
{
	if (!IsServer())
	{
		return;
	}

	ComboIndex = GetCurrentComboIndexFromMontage();

	const bool bCanAdvance = (ComboIndex < CachedMaxComboHits - 1);
	const bool bWantsAdvance = bComboInputQueued && bCanAdvance;

	bComboInputQueued = false;

	if (bWantsAdvance && CachedComboSections.IsValidIndex(ComboIndex) && CachedComboSections.IsValidIndex(ComboIndex + 1))
	{
		const FName From = CachedComboSections[ComboIndex];
		const FName To   = CachedComboSections[ComboIndex + 1];

		SetNextSection_LocalOrASC(From, To);

		UE_LOG(LogTemp, Warning, TEXT("[GA][Server] Advance: %s -> %s | ComboIndex=%d"),
			*From.ToString(), *To.ToString(), ComboIndex);

		ComboIndex++;
		ScheduleServerSectionEndTimer();
		return;
	}

	// Não avançou: deixa terminar e finaliza no OnMontageCompleted()
}

// --------------------------
// Combo start / montage task
// --------------------------

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
		ClearAutoChainLinks();
		ScheduleServerSectionEndTimer();
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

	ClearAutoChainLinks();
	ScheduleServerSectionEndTimer();
}

void UGA_Weapon_MeleeLight::FireLocalStartAttackForWindow(int32 WindowIdx)
{
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
		AttackId,
		static_cast<uint8>(FMath::Clamp(WindowIdx, 0, 255)),
		GetAimYaw()
	);
}

// --------------------------
// Events / activation
// --------------------------

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
		false,
		false
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
		false,
		false
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

	StartComboAt(0);
}

void UGA_Weapon_MeleeLight::InputPressed(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (!CurrentActorInfo) return;
	if (!CachedAttackMontage || CachedComboSections.Num() == 0) return;
	if (!IsMontagePlaying()) return;

	// Atualiza índice pela seção atual
	ComboIndex = GetCurrentComboIndexFromMontage();
	if (ComboIndex >= CachedMaxComboHits - 1) return;

	// Buffer para o server decidir
	bComboInputQueued = true;

	// ✅ Predição local: se o player apertou, já seta próxima seção no owning client
	// (sem isso, como você matou o autochain, o client fica preso no Hit1)
	if (IsLocallyControlled() && CachedComboSections.IsValidIndex(ComboIndex + 1))
	{
		const FName From = CachedComboSections[ComboIndex];
		const FName To   = CachedComboSections[ComboIndex + 1];

		SetNextSection_LocalOrASC(From, To);

		UE_LOG(LogTemp, Warning, TEXT("[GA][ClientPredict] SetNextSection %s -> %s | ComboIndex=%d"),
			*From.ToString(), *To.ToString(), ComboIndex);
	}

	UE_LOG(LogTemp, Warning, TEXT("[GA] InputPressed LC=%d SV=%d ComboIndex=%d Max=%d Queued=%d"),
		IsLocallyControlled() ? 1 : 0,
		IsServer() ? 1 : 0,
		ComboIndex, CachedMaxComboHits, bComboInputQueued ? 1 : 0);
}

void UGA_Weapon_MeleeLight::OnComboWindowBegin(FGameplayEventData Payload)
{
	const int32 WindowIdx = ExtractComboIndexFromPayload(Payload);

	if (IsLocallyControlled())
	{
		bAcceptingComboInput = true;
	}

	// Sempre dispara o hit window no owning client (é ele que chama LocalStartAttack)
	FireLocalStartAttackForWindow(WindowIdx);
}

void UGA_Weapon_MeleeLight::OnComboWindowEnd(FGameplayEventData Payload)
{
	(void)Payload;

	if (IsLocallyControlled())
	{
		bAcceptingComboInput = false;
	}
}

void UGA_Weapon_MeleeLight::OnMontageCompleted()
{
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

// --------------------------
// End / cleanup
// --------------------------

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
	AActor* Avatar = GetAvatar();
	if (Avatar)
	{
		if (UWorld* World = Avatar->GetWorld())
		{
			World->GetTimerManager().ClearTimer(ServerSectionEndTimer);
		}
	}

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
