// ============================================================================
// GA_Weapon_StaffBasic.cpp
// Path: Source/SpellRise/Combat/Spells/Abilities/GA_Weapon_StaffBasic.cpp
// ============================================================================

#include "SpellRise/Combat/Spells/Abilities/GA_Weapon_StaffBasic.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"
#include "SpellRise/Combat/Spells/Data/DA_StaffWeaponData.h"
#include "SpellRise/Weapons/Actors/SpellRiseWeaponActor.h"

#include "SpellRise/Combat/Spells/Projectiles/ASpellRiseMagicProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Animation/AnimInstance.h"
#include "Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"

namespace SR_StaffTags
{
	inline const FGameplayTag& DefaultCastPauseTag()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Spell.CastPause"), false);
		return Tag;
	}
}

// ✅ GameplayAbility não é Actor -> use World->GetNetMode() com segurança
static ENetMode SR_GetNetModeSafe(const UGameplayAbility* Ability)
{
	if (!Ability) return NM_Standalone;
	if (UWorld* World = Ability->GetWorld())
	{
		return World->GetNetMode();
	}
	return NM_Standalone;
}

UGA_Weapon_StaffBasic::UGA_Weapon_StaffBasic()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bUseCasting = true;
	CommitPolicy = ESpellRiseCommitPolicy::CommitOnFire;
	bCancelIfReleasedEarly = false;

	// Input local -> GAS replicates ability activation appropriately
	bReplicateInputDirectly = true;
}

const UDA_StaffWeaponData* UGA_Weapon_StaffBasic::GetWeaponData() const
{
	USpellRiseWeaponComponent* WC = GetWeaponComponent();
	return WC ? WC->GetEquippedStaffWeapon() : nullptr;
}

USpellRiseWeaponComponent* UGA_Weapon_StaffBasic::GetWeaponComponent() const
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	return Avatar ? Avatar->FindComponentByClass<USpellRiseWeaponComponent>() : nullptr;
}

void UGA_Weapon_StaffBasic::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UGameplayAbility::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const UDA_StaffWeaponData* WeaponDA = GetWeaponData();

	UE_LOG(LogTemp, Warning, TEXT("[StaffGA] Activate Char=%s WeaponDA=%s EquippedWeapon=%s"),
		*GetNameSafe(GetAvatarActorFromActorInfo()),
		*GetNameSafe(WeaponDA),
		*GetNameSafe(GetWeaponComponent() ? GetWeaponComponent()->GetEquippedWeapon() : nullptr));

	if (!WeaponDA)
	{
		UE_LOG(LogTemp, Error, TEXT("[StaffGA] WeaponDA NULL (staff not equipped)."));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (!WeaponDA->SpellConfig.CastMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("[StaffGA] CastMontage NULL in WeaponDA=%s"), *GetNameSafe(WeaponDA));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// Cosmetic cast start (client/standalone only)
	if (SR_GetNetModeSafe(this) != NM_DedicatedServer)
	{
		if (WeaponDA->FeedbackConfig.CastStartSound && GetAvatarActorFromActorInfo())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				WeaponDA->FeedbackConfig.CastStartSound,
				GetAvatarActorFromActorInfo()->GetActorLocation());
		}
	}

	// Interrupt Tag priority:
	// 1) Ability InterruptEventTag
	// 2) WeaponDA SpellConfig.CancelEventTag
	const FGameplayTag EffectiveInterruptTag = InterruptEventTag.IsValid()
		? InterruptEventTag
		: WeaponDA->SpellConfig.CancelEventTag;

	if (EffectiveInterruptTag.IsValid())
	{
		StaffWaitInterruptEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			EffectiveInterruptTag,
			nullptr,
			false,
			false);

		if (StaffWaitInterruptEventTask)
		{
			StaffWaitInterruptEventTask->EventReceived.AddDynamic(this, &UGA_Weapon_StaffBasic::Staff_OnInterruptEvent);
			StaffWaitInterruptEventTask->ReadyForActivation();
		}
	}

	StaffMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		WeaponDA->SpellConfig.CastMontage,
		1.0f,
		NAME_None,
		false);

	if (StaffMontageTask)
	{
		StaffMontageTask->OnCompleted.AddDynamic(this, &UGA_Weapon_StaffBasic::Staff_OnMontageCompleted);
		StaffMontageTask->OnInterrupted.AddDynamic(this, &UGA_Weapon_StaffBasic::Staff_OnMontageInterrupted);
		StaffMontageTask->OnCancelled.AddDynamic(this, &UGA_Weapon_StaffBasic::Staff_OnMontageCancelled);
		StaffMontageTask->ReadyForActivation();
	}

	StartCastFlow_Local();
}

void UGA_Weapon_StaffBasic::StartCastFlow_Local()
{
	bStaffHasFired = false;
	bStaffPausedMontage = false;
	bStaffCastCompleted = false;
	bStaffHoldPointReached = false;
	bStaffInputReleased = false;
	StaffTimeHeld = 0.f;

	if (CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid() && CastingStateTag.IsValid())
	{
		CurrentActorInfo->AbilitySystemComponent->AddLooseGameplayTag(CastingStateTag);
	}

	BP_OnCastStart();

	const UDA_StaffWeaponData* WeaponDA = GetWeaponData();
	if (!WeaponDA)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	const float EffectiveCastTime = (WeaponDA->SpellConfig.CastTime > 0.f)
		? WeaponDA->SpellConfig.CastTime
		: CastTime;

	StaffWaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	if (StaffWaitInputReleaseTask)
	{
		StaffWaitInputReleaseTask->OnRelease.AddDynamic(this, &UGA_Weapon_StaffBasic::Staff_OnInputReleased_Local);
		StaffWaitInputReleaseTask->ReadyForActivation();
	}

	const FGameplayTag PauseTag = WeaponDA->SpellConfig.CastPauseEventTag.IsValid()
		? WeaponDA->SpellConfig.CastPauseEventTag
		: SR_StaffTags::DefaultCastPauseTag();

	if (PauseTag.IsValid())
	{
		StaffWaitCastPauseTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			PauseTag,
			nullptr,
			false,
			false);

		if (StaffWaitCastPauseTask)
		{
			StaffWaitCastPauseTask->EventReceived.AddDynamic(this, &UGA_Weapon_StaffBasic::Staff_OnCastPauseEvent);
			StaffWaitCastPauseTask->ReadyForActivation();
		}
	}

	if (EffectiveCastTime > 0.f)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				StaffCastTimerHandle,
				this,
				&UGA_Weapon_StaffBasic::OnCastCompleted_Local,
				EffectiveCastTime,
				false);
		}
	}
	else
	{
		OnCastCompleted_Local();
	}
}

void UGA_Weapon_StaffBasic::Staff_OnCastPauseEvent(FGameplayEventData Payload)
{
	// ✅ HOLD POINT: pausa a animação, mas NÃO completa cast e NÃO limpa timer.
	if (!IsActive() || bStaffHasFired)
	{
		return;
	}

	bStaffHoldPointReached = true;
	PauseMontageAtHoldPoint();
}

void UGA_Weapon_StaffBasic::OnCastCompleted_Local()
{
	if (!IsActive() || bStaffHasFired || bStaffCastCompleted)
	{
		return;
	}

	bStaffCastCompleted = true;
	BP_OnCastCompleted();

	// ✅ Só segura a animação se realmente chegou no hold point via notify
	if (bStaffHoldPointReached)
	{
		PauseMontageAtHoldPoint();
	}

	// ✅ Só dispara se o input já foi solto
	if (bStaffInputReleased)
	{
		FireSpell();
	}
}

void UGA_Weapon_StaffBasic::Staff_OnInputReleased_Local(float HeldTime)
{
	if (!IsActive() || bStaffHasFired)
	{
		return;
	}

	bStaffInputReleased = true;
	StaffTimeHeld = HeldTime;

	if (bStaffCastCompleted)
	{
		FireSpell();
		return;
	}

	if (bCancelIfReleasedEarly)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UGA_Weapon_StaffBasic::PauseMontageAtHoldPoint()
{
	if (bStaffPausedMontage)
	{
		return;
	}

	const UDA_StaffWeaponData* WeaponDA = GetWeaponData();
	if (!WeaponDA || !WeaponDA->SpellConfig.CastMontage)
	{
		return;
	}

	if (UAnimInstance* AnimInstance = CurrentActorInfo ? CurrentActorInfo->GetAnimInstance() : nullptr)
	{
		AnimInstance->Montage_SetPlayRate(WeaponDA->SpellConfig.CastMontage, 0.f);
		bStaffPausedMontage = true;
	}
}

void UGA_Weapon_StaffBasic::ResumeMontage()
{
	const UDA_StaffWeaponData* WeaponDA = GetWeaponData();
	if (!WeaponDA || !WeaponDA->SpellConfig.CastMontage)
	{
		return;
	}

	if (UAnimInstance* AnimInstance = CurrentActorInfo ? CurrentActorInfo->GetAnimInstance() : nullptr)
	{
		AnimInstance->Montage_SetPlayRate(WeaponDA->SpellConfig.CastMontage, 1.f);
	}

	bStaffPausedMontage = false;
}

void UGA_Weapon_StaffBasic::FireSpell()
{
	if (!IsActive() || bStaffHasFired)
	{
		return;
	}

	bStaffHasFired = true;

	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("[StaffGA] CommitAbility FAILED - ending."));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// ✅ Retoma a montage para tocar a seção de release até o final
	ResumeMontage();

	// Cosmetic fire feedback (client/standalone only)
	if (SR_GetNetModeSafe(this) != NM_DedicatedServer)
	{
		const UDA_StaffWeaponData* WeaponDA = GetWeaponData();
		USpellRiseWeaponComponent* WC = GetWeaponComponent();

		if (WeaponDA && WC)
		{
			FTransform MuzzleXf;
			const FName SocketName = WeaponDA->SpellConfig.SpellMuzzleSocket.IsNone()
				? WC->GetDefaultSpellMuzzleSocketName()
				: WeaponDA->SpellConfig.SpellMuzzleSocket;

			if (WC->GetWeaponSocketTransform(SocketName, MuzzleXf))
			{
				if (WeaponDA->FeedbackConfig.MuzzleEffect)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponDA->FeedbackConfig.MuzzleEffect, MuzzleXf);
				}
				if (WeaponDA->FeedbackConfig.FireSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, WeaponDA->FeedbackConfig.FireSound, MuzzleXf.GetLocation());
				}
			}
		}
	}

	// Server spawns projectile (replicates to clients)
	if (CurrentActorInfo && CurrentActorInfo->IsNetAuthority())
	{
		SpawnProjectile_Server();
	}

	if (!StaffMontageTask)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGA_Weapon_StaffBasic::SpawnProjectile_Server()
{
	// --- SUA IMPLEMENTAÇÃO ORIGINAL (inalterada) ---
	const UDA_StaffWeaponData* WeaponDA = GetWeaponData();
	USpellRiseWeaponComponent* WC = GetWeaponComponent();
	AActor* Avatar = GetAvatarActorFromActorInfo();
	UWorld* World = GetWorld();

	UE_LOG(LogTemp, Warning, TEXT("[StaffGA] SpawnProjectile_Server Char=%s WeaponDA=%s ProjClass=%s ImpactCue=%s"),
		*GetNameSafe(Avatar),
		*GetNameSafe(WeaponDA),
		WeaponDA ? *GetNameSafe(WeaponDA->SpellConfig.ProjectileClass) : TEXT("NULL"),
		WeaponDA ? *WeaponDA->FeedbackConfig.ImpactCueTag.ToString() : TEXT("None"));

	if (!WeaponDA || !WC || !Avatar || !World)
	{
		return;
	}

	if (!WeaponDA->SpellConfig.ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StaffGA] ProjectileClass NULL in WeaponDA=%s"), *GetNameSafe(WeaponDA));
		return;
	}

	const TSubclassOf<UGameplayEffect> DamageGE = WeaponDA->SpellConfig.ProjectileDamageEffectClass;

	// 1) Muzzle transform
	FTransform MuzzleXform;
	const FName SocketName = WeaponDA->SpellConfig.SpellMuzzleSocket.IsNone()
		? WC->GetDefaultSpellMuzzleSocketName()
		: WeaponDA->SpellConfig.SpellMuzzleSocket;

	if (!WC->GetWeaponSocketTransform(SocketName, MuzzleXform))
	{
		const FVector Start = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 80.f + FVector(0, 0, 60.f);
		const FRotator Rot = Avatar->GetActorRotation();
		MuzzleXform = FTransform(Rot, Start);
	}

	const FVector MuzzleLoc = MuzzleXform.GetLocation();

	// 2) Viewpoint on SERVER
	FVector ViewLoc = MuzzleLoc;
	FRotator ViewRot = Avatar->GetActorRotation();

	APawn* Pawn = Cast<APawn>(Avatar);
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->GetPlayerViewPoint(ViewLoc, ViewRot);
	}
	else
	{
		if (Pawn) { ViewLoc = Pawn->GetPawnViewLocation(); }
		if (Controller) { ViewRot = Controller->GetControlRotation(); }
	}

	// 3) Authoritative trace
	const float TraceRange = (WeaponDA->SpellConfig.MaxRange > 0.f) ? WeaponDA->SpellConfig.MaxRange : 30000.f;
	const FVector TraceStart = ViewLoc;
	const FVector TraceEnd = TraceStart + (ViewRot.Vector() * TraceRange);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(StaffSpellAimTrace), false);
	QueryParams.AddIgnoredActor(Avatar);

	if (AActor* WeaponActor = WC->GetEquippedWeaponActor())
	{
		QueryParams.AddIgnoredActor(WeaponActor);
	}

	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
	const FVector AimPoint = bHit ? Hit.ImpactPoint : TraceEnd;

	FVector AimDir = (AimPoint - MuzzleLoc);
	if (!AimDir.Normalize())
	{
		AimDir = ViewRot.Vector();
	}

	MuzzleXform.SetRotation(AimDir.Rotation().Quaternion());

	// 4) Spawn
	FActorSpawnParameters Params;
	Params.Owner = Avatar;
	Params.Instigator = Pawn;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Spawned = World->SpawnActor<AActor>(WeaponDA->SpellConfig.ProjectileClass, MuzzleXform, Params);
	if (!Spawned)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StaffGA] SpawnActor FAILED for ProjectileClass=%s"),
			*GetNameSafe(WeaponDA->SpellConfig.ProjectileClass));
		return;
	}

	// 5) Init projectile (Impact via GameplayCue Tag)
	if (ASpellRiseMagicProjectileBase* Proj = Cast<ASpellRiseMagicProjectileBase>(Spawned))
	{
		FGameplayTagContainer DynTags;
		if (WeaponDA->SpellConfig.DamageTypeTag.IsValid())
		{
			DynTags.AddTag(WeaponDA->SpellConfig.DamageTypeTag);
		}

		Proj->InitProjectile(
			Avatar,
			DamageGE,
			WeaponDA->GetFinalSpellBaseDamage(),
			DynTags,
			WeaponDA->FeedbackConfig.ImpactCueTag,
			const_cast<UDA_StaffWeaponData*>(WeaponDA)
		);
	}
}

void UGA_Weapon_StaffBasic::Staff_OnInterruptEvent(FGameplayEventData Payload)
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_Weapon_StaffBasic::Staff_OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Weapon_StaffBasic::Staff_OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Weapon_StaffBasic::Staff_OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Weapon_StaffBasic::ClearTasksAndTimer()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StaffCastTimerHandle);
	}

	if (StaffWaitInputReleaseTask)
	{
		StaffWaitInputReleaseTask->EndTask();
		StaffWaitInputReleaseTask = nullptr;
	}

	if (StaffWaitCastPauseTask)
	{
		StaffWaitCastPauseTask->EndTask();
		StaffWaitCastPauseTask = nullptr;
	}

	if (StaffWaitInterruptEventTask)
	{
		StaffWaitInterruptEventTask->EndTask();
		StaffWaitInterruptEventTask = nullptr;
	}

	if (StaffMontageTask)
	{
		StaffMontageTask->EndTask();
		StaffMontageTask = nullptr;
	}

	if (CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid() && CastingStateTag.IsValid())
	{
		CurrentActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(CastingStateTag);
	}
}

void UGA_Weapon_StaffBasic::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	// Cosmetic cancel feedback (client/standalone only)
	if (SR_GetNetModeSafe(this) != NM_DedicatedServer)
	{
		const UDA_StaffWeaponData* WeaponDA = GetWeaponData();
		if (WeaponDA && WeaponDA->FeedbackConfig.CancelSound && GetAvatarActorFromActorInfo())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				WeaponDA->FeedbackConfig.CancelSound,
				GetAvatarActorFromActorInfo()->GetActorLocation());
		}
	}

	ResumeMontage();

	USpellRiseGA_CastSpellBase::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Weapon_StaffBasic::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	ClearTasksAndTimer();

	if (bWasCancelled && bUseCasting)
	{
		BP_OnCastCancelled();
	}

	ResumeMontage();

	USpellRiseGA_CastSpellBase::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
