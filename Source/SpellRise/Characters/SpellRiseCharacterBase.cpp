// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Characters/SpellRiseCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/InputComponent.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameplayCameraComponentBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "GameplayEffect.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "NavigationMarkerComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UnrealType.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "SpellRise/Components/CatalystComponent.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h"
#include "SpellRise/Equipment/SpellRiseWeaponComponent.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityHotbarComponent.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/Inventory/SpellRiseFullLootSubsystem.h"
#include "SpellRise/Security/SpellRiseAuditTrail.h"
#include "SpellRise/UI/SpellRiseDamageEdgeWidget.h"
#include "SpellRise/UI/SpellRiseDeathScreenWidget.h"
#include "EquippableItem.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "InventoryFunctionLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseDeathLoot, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseSecurity, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseCharacterRuntime, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseEquipTrace, Log, All);

namespace SpellRiseTags
{
	static const FGameplayTag& State_Dead()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Dead"));
		return Tag;
	}

	static const FGameplayTag& State_DerivedStats_Active()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("State.DerivedStats.Active"));
		return Tag;
	}

	static const FGameplayTag& Status_Frozen()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Status.Frozen"));
		return Tag;
	}

	static const FGameplayTag& Debuff_Ice()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Debuff.Ice"));
		return Tag;
	}

	static const FGameplayTag& Status_Bleeding()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Status.Bleeding"), false);
		return Tag;
	}

	static const FGameplayTag& Debuff_ManaRegenPenalty()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Debuff.ManaRegenPenalty"), false);
		return Tag;
	}

	static const FGameplayTag& State_Resource_StaminaRegenPaused()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Resource.StaminaRegenPaused"), false);
		return Tag;
	}

	static const FGameplayTag& State_Casting()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Casting"), false);
		return Tag;
	}

	static const FGameplayTag& State_Drawing()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Drawing"), false);
		return Tag;
	}

	static const FGameplayTag& State_Sprinting()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Sprinting"), false);
		return Tag;
	}

	static const FGameplayTag& Event_Abilities_Changed()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Abilities.Changed"));
		return Tag;
	}

	static const FGameplayTag& Input_Primary()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Primary"));
		return Tag;
	}

	static const FGameplayTag& Input_Secondary()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Secondary"));
		return Tag;
	}

	static const FGameplayTag& Input_Interact()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Utility.Interact"));
		return Tag;
	}

	static const FGameplayTag& Input_AbilitySlot(int32 SlotIndex)
	{
		switch (SlotIndex)
		{
		case 0:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill1"));
			return Tag;
		}
		case 1:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill2"));
			return Tag;
		}
		case 2:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill3"));
			return Tag;
		}
		case 3:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill4"));
			return Tag;
		}
		case 4:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill5"));
			return Tag;
		}
		case 5:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill6"));
			return Tag;
		}
		case 6:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill7"));
			return Tag;
		}
		case 7:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill8"));
			return Tag;
		}
		case 8:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Common1"), false);
			return Tag;
		}
		case 9:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Common2"), false);
			return Tag;
		}
		case 10:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Common3"), false);
			return Tag;
		}
		case 11:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Common4"), false);
			return Tag;
		}
		case 12:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Common5"), false);
			return Tag;
		}
		case 13:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Common6"), false);
			return Tag;
		}
		case 14:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Common7"), false);
			return Tag;
		}
		case 15:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Common8"), false);
			return Tag;
		}
		default:
		{
			static FGameplayTag InvalidTag;
			return InvalidTag;
		}
		}
	}
}

namespace
{
	bool SR_IsValidArchetype(ESpellRiseArchetype Archetype)
	{
		switch (Archetype)
		{
		case ESpellRiseArchetype::None:
		case ESpellRiseArchetype::Warrior:
		case ESpellRiseArchetype::Assassin:
		case ESpellRiseArchetype::Mage:
		case ESpellRiseArchetype::Battlemage:
		case ESpellRiseArchetype::Cleric:
			return true;
		default:
			return false;
		}
	}

	bool SR_IsValidAbilityWheelInputTag(const FGameplayTag& Tag)
	{
		if (!Tag.IsValid())
		{
			return true;
		}

		for (int32 SlotIndex = 0; SlotIndex < 16; ++SlotIndex)
		{
			if (Tag.MatchesTagExact(SpellRiseTags::Input_AbilitySlot(SlotIndex)))
			{
				return true;
			}
		}

		return false;
	}

}

ASpellRiseCharacterBase::ASpellRiseCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CatalystComponent = CreateDefaultSubobject<UCatalystComponent>(TEXT("CatalystComponent"));
	if (CatalystComponent)
	{
		CatalystComponent->bCatalystEnabled = bEnableCatalyst;
	}

	FallDamageComponent = CreateDefaultSubobject<UFallDamageComponent>(TEXT("FallDamageComponent"));
	WeaponComponent = CreateDefaultSubobject<USpellRiseWeaponComponent>(TEXT("WeaponComponent"));

	DeadStateTag = SpellRiseTags::State_Dead();
	BleedingBlocksRegenTag = SpellRiseTags::Status_Bleeding();
	ManaRegenPenaltyTag = SpellRiseTags::Debuff_ManaRegenPenalty();
	StaminaRegenPausedTag = SpellRiseTags::State_Resource_StaminaRegenPaused();
	StaminaRegenPausedWhileTags.AddTag(SpellRiseTags::State_Casting());
	StaminaRegenPausedWhileTags.AddTag(SpellRiseTags::State_Drawing());
	StaminaRegenPausedWhileTags.AddTag(SpellRiseTags::State_Sprinting());

	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	ForceServerAnimTick();

	AllowedServerEventTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.Start"), false));
	AllowedServerEventTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.Input"), false));
	AllowedServerEventTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.End"), false));
	AllowedServerEventTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Event.HitScan.Start"), false));
	AllowedServerEventTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Event.HitScan.End"), false));
	AllowedServerEventTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Event.ShootProjectile"), false));

	static ConstructorHelpers::FObjectFinder<UAnimMontage> HitReactionMontageFinder(
		TEXT("/Game/Combat/Animations/Montage_HitReaction.Montage_HitReaction"));
	if (HitReactionMontageFinder.Succeeded())
	{
		HitReactionMontage = HitReactionMontageFinder.Object;
	}
}

void ASpellRiseCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ApplyAnimationPresentationPolicy();
	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();
	ValidateAnimationPresentationPolicy();
}

void ASpellRiseCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (FParse::Param(FCommandLine::Get(), TEXT("SpellRiseSkipLocalHUDFlow")))
	{
		TArray<UGameplayCameraComponentBase*> GameplayCameraComponents;
		GetComponents<UGameplayCameraComponentBase>(GameplayCameraComponents);
		for (UGameplayCameraComponentBase* GameplayCameraComponent : GameplayCameraComponents)
		{
			if (!GameplayCameraComponent)
			{
				continue;
			}

			GameplayCameraComponent->DeactivateCamera(true);
			GameplayCameraComponent->Deactivate();
			GameplayCameraComponent->SetComponentTickEnabled(false);
		}
	}

	ResetLocalDeathPresentation();
	SyncDeadStateFromASC(TEXT("BeginPlay"));

	if (CatalystComponent)
	{
		CatalystComponent->bCatalystEnabled = bEnableCatalyst;
	}

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();
	ApplyAnimationPresentationPolicy();
	ValidateAnimationPresentationPolicy();

	InitASCActorInfo();
	BindASCDelegates();
	SetCharacterInputEnabled(!IsDead());
	RefreshRuntimeTickPolicy();

	TArray<UNavigationMarkerComponent*> NavigationMarkers;
	GetComponents<UNavigationMarkerComponent>(NavigationMarkers);
	for (UNavigationMarkerComponent* Marker : NavigationMarkers)
	{
		if (!Marker)
		{
			continue;
		}

		Marker->bOnlyVisibleToOwningPlayer = true;
		Marker->RefreshMarker();
	}

}

void ASpellRiseCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ASCInitializationRetryTimerHandle);
		World->GetTimerManager().ClearTimer(ResourceRegenTimerHandle);
	}
	UnbindNarrativeInventoryWeightDelegates();
	ResetLocalDeathPresentation();
	Super::EndPlay(EndPlayReason);
}

namespace
{
	static bool IsTalentTreeComponentForDeathCleanup(const UActorComponent* Component)
	{
		if (!Component || !Component->GetClass())
		{
			return false;
		}

		const FString ComponentName = Component->GetName();
		const FString ClassPath = Component->GetClass()->GetPathName();
		return ComponentName.Contains(TEXT("TalentTreeComponent"), ESearchCase::IgnoreCase)
			|| ClassPath.Contains(TEXT("TalentTreeComponent"), ESearchCase::IgnoreCase);
	}

	static bool HasGrantedTalentStateForDeathCleanup(const ASpellRiseCharacterBase* Character)
	{
		if (!Character)
		{
			return false;
		}

		TArray<const AActor*> Owners;
		Owners.Add(Character);
		if (const APlayerState* PlayerState = Character->GetPlayerState())
		{
			Owners.Add(PlayerState);
		}

		for (const AActor* Owner : Owners)
		{
			if (!Owner)
			{
				continue;
			}

			TArray<UActorComponent*> Components;
			Owner->GetComponents(Components);
			for (UActorComponent* Component : Components)
			{
				if (!IsTalentTreeComponentForDeathCleanup(Component))
				{
					continue;
				}

				const FArrayProperty* GrantedTalentsProperty = FindFProperty<FArrayProperty>(Component->GetClass(), TEXT("GrantedTalents"));
				if (!GrantedTalentsProperty)
				{
					continue;
				}

				FScriptArrayHelper GrantedTalentsHelper(GrantedTalentsProperty, GrantedTalentsProperty->ContainerPtrToValuePtr<void>(Component));
				if (GrantedTalentsHelper.Num() > 0)
				{
					return true;
				}
			}
		}

		return false;
	}
}

void ASpellRiseCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasValidASCActorInfo())
	{
		ScheduleASCInitializationRetry();
	}

	if (!IsLocallyControlled())
	{
		if (!HasAuthority() && GetSpellRiseASC())
		{
			GetSpellRiseASC()->SR_ClearAbilityInput();
		}
		return;
	}

	SR_ProcessAbilityInput(DeltaTime, false);
}

void ASpellRiseCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!IsLocallyControlled() || !PlayerInputComponent)
	{
		return;
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (IMC_Default)
				{
					Subsystem->AddMappingContext(IMC_Default, 0);
				}
			}
		}
	}



}

void ASpellRiseCharacterBase::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	if (!bForce)
	{
		if (bInventoryWeightMovementBlocked
			|| InventoryWeightMovementInputScale <= KINDA_SMALL_NUMBER
			|| GameplayMovementInputScale <= KINDA_SMALL_NUMBER)
		{
			return;
		}

		ScaleValue *= InventoryWeightMovementInputScale * GameplayMovementInputScale;
	}

	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

void ASpellRiseCharacterBase::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!IsLocallyControlled() || !GetSpellRiseASC() || !InputTag.IsValid())
	{
		return;
	}

	for (int32 SlotIndex = 0; SlotIndex < 8; ++SlotIndex)
	{
		if (InputTag.MatchesTagExact(SpellRiseTags::Input_AbilitySlot(SlotIndex)))
		{
			AbilityWheelInputPressed(SlotIndex);
			return;
		}
	}

	GetSpellRiseASC()->SR_AbilityInputTagPressed(InputTag);
}

void ASpellRiseCharacterBase::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!IsLocallyControlled() || !GetSpellRiseASC() || !InputTag.IsValid())
	{
		return;
	}

	for (int32 SlotIndex = 0; SlotIndex < 8; ++SlotIndex)
	{
		if (InputTag.MatchesTagExact(SpellRiseTags::Input_AbilitySlot(SlotIndex)))
		{
			AbilityWheelInputReleased(SlotIndex);
			return;
		}
	}

	GetSpellRiseASC()->SR_AbilityInputTagReleased(InputTag);
}

void ASpellRiseCharacterBase::ForceServerAnimTick()
{
	TArray<USkeletalMeshComponent*> SkeletalMeshes;
	GetComponents<USkeletalMeshComponent>(SkeletalMeshes);

	for (USkeletalMeshComponent* MeshComp : SkeletalMeshes)
	{
		if (!IsValid(MeshComp))
		{
			continue;
		}

		if (GetNetMode() == NM_DedicatedServer && !bForceAnimationTickOnDedicatedServer)
		{
			MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesWhenNotRendered;
			MeshComp->bEnableUpdateRateOptimizations = true;
			MeshComp->SetComponentTickEnabled(false);
			continue;
		}

		MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		MeshComp->bEnableUpdateRateOptimizations = false;
		MeshComp->SetComponentTickEnabled(true);
	}
}

void ASpellRiseCharacterBase::EnsureAnimInstanceInitialized()
{
	if (GetNetMode() == NM_DedicatedServer && !bForceAnimationTickOnDedicatedServer)
	{
		return;
	}

	TArray<USkeletalMeshComponent*> SkeletalMeshes;
	GetComponents<USkeletalMeshComponent>(SkeletalMeshes);

	for (USkeletalMeshComponent* MeshComp : SkeletalMeshes)
	{
		if (!IsValid(MeshComp))
		{
			continue;
		}

		if (MeshComp->AnimClass && !MeshComp->GetAnimInstance())
		{
			MeshComp->InitAnim(true);
		}
	}
}

void ASpellRiseCharacterBase::RefreshRuntimeTickPolicy()
{
	const bool bNeedsLocalAbilityInputTick = GetNetMode() != NM_DedicatedServer && IsLocallyControlled() && !IsDead();
	SetActorTickEnabled(bNeedsLocalAbilityInputTick);
}

void ASpellRiseCharacterBase::ApplyAnimationPresentationPolicy()
{
	if (!bTreatVisualOverrideAsPresentationOnly || IsDead())
	{
		return;
	}

	USkeletalMeshComponent* VisualMesh = GetVisualMeshComponent();
	if (!VisualMesh)
	{
		return;
	}

	if (VisualMesh == GetMesh() && !VisualMesh->GetFName().IsEqual(VisualMeshComponentName))
	{
		return;
	}

	VisualMesh->SetGenerateOverlapEvents(false);
	VisualMesh->SetNotifyRigidBodyCollision(false);

	if (bEnforceAliveVisualMeshPresentationCollision)
	{
		if (!AliveVisualMeshCollisionProfileName.IsNone())
		{
			VisualMesh->SetCollisionProfileName(AliveVisualMeshCollisionProfileName, true);
		}
		else
		{
			VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ASpellRiseCharacterBase::ValidateAnimationPresentationPolicy() const
{
#if !UE_BUILD_SHIPPING
	if (!bTreatVisualOverrideAsPresentationOnly)
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning,
			TEXT("[AnimationPolicy] VisualOverride presentation policy disabled. Character=%s Standard=%d"),
			*GetNameSafe(this),
			static_cast<int32>(AnimationRuntimeStandard));
		return;
	}

	const USkeletalMeshComponent* VisualMesh = GetVisualMeshComponent();
	if (!VisualMesh)
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning,
			TEXT("[AnimationPolicy] Visual mesh missing. Character=%s VisualMeshComponentName=%s"),
			*GetNameSafe(this),
			*VisualMeshComponentName.ToString());
		return;
	}

	if (VisualMesh == GetMesh() && !VisualMesh->GetFName().IsEqual(VisualMeshComponentName))
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Verbose,
			TEXT("[AnimationPolicy] Using root character mesh as visual fallback. Character=%s Standard=%d"),
			*GetNameSafe(this),
			static_cast<int32>(AnimationRuntimeStandard));
	}

	if (!IsDead()
		&& VisualMesh->GetCollisionEnabled() != ECollisionEnabled::NoCollision
		&& VisualMesh->GetCollisionResponseToChannel(ECC_Pawn) != ECR_Ignore)
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning,
			TEXT("[AnimationPolicy] Alive visual mesh should not block/overlap Pawn. Character=%s Mesh=%s Profile=%s"),
			*GetNameSafe(this),
			*GetNameSafe(VisualMesh),
			*VisualMesh->GetCollisionProfileName().ToString());
	}

	if (!IsDead()
		&& VisualMesh->GetCollisionEnabled() != ECollisionEnabled::NoCollision
		&& VisualMesh->GetCollisionResponseToChannel(ECC_Visibility) != ECR_Ignore)
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning,
			TEXT("[AnimationPolicy] Alive visual mesh should ignore Visibility; use Pawn/object traces for targeting. Character=%s Mesh=%s Profile=%s"),
			*GetNameSafe(this),
			*GetNameSafe(VisualMesh),
			*VisualMesh->GetCollisionProfileName().ToString());
	}
#endif
}

USkeletalMeshComponent* ASpellRiseCharacterBase::FindCharacterSkeletalMeshComponentByName(FName ComponentName) const
{
	if (ComponentName.IsNone())
	{
		return nullptr;
	}

	TArray<USkeletalMeshComponent*> SkeletalMeshes;
	GetComponents<USkeletalMeshComponent>(SkeletalMeshes);

	for (USkeletalMeshComponent* MeshComp : SkeletalMeshes)
	{
		if (!IsValid(MeshComp))
		{
			continue;
		}

		if (MeshComp->GetFName() == ComponentName)
		{
			return MeshComp;
		}
	}

	return nullptr;
}

UChildActorComponent* ASpellRiseCharacterBase::FindCharacterChildActorComponentByName(FName ComponentName) const
{
	if (ComponentName.IsNone())
	{
		return nullptr;
	}

	TArray<UChildActorComponent*> ChildActorComponents;
	GetComponents<UChildActorComponent>(ChildActorComponents);

	for (UChildActorComponent* ChildActorComp : ChildActorComponents)
	{
		if (!IsValid(ChildActorComp))
		{
			continue;
		}

		if (ChildActorComp->GetFName() == ComponentName)
		{
			return ChildActorComp;
		}
	}

	return nullptr;
}

USkeletalMeshComponent* ASpellRiseCharacterBase::ResolveSkeletalMeshFromChildActorComponent(FName ComponentName) const
{
	UChildActorComponent* ChildActorComp = FindCharacterChildActorComponentByName(ComponentName);
	if (!ChildActorComp)
	{
		return nullptr;
	}

	AActor* ChildActor = ChildActorComp->GetChildActor();
	if (!IsValid(ChildActor))
	{
		return nullptr;
	}

	if (USkeletalMeshComponent* DirectMesh = ChildActor->FindComponentByClass<USkeletalMeshComponent>())
	{
		return DirectMesh;
	}

	TArray<USkeletalMeshComponent*> ChildMeshes;
	ChildActor->GetComponents<USkeletalMeshComponent>(ChildMeshes);
	for (USkeletalMeshComponent* MeshComp : ChildMeshes)
	{
		if (IsValid(MeshComp))
		{
			return MeshComp;
		}
	}

	return nullptr;
}

UCameraComponent* ASpellRiseCharacterBase::FindCharacterCameraComponentByName(FName ComponentName) const
{
	if (ComponentName.IsNone())
	{
		return nullptr;
	}

	TArray<UCameraComponent*> CameraComponents;
	GetComponents<UCameraComponent>(CameraComponents);

	for (UCameraComponent* CameraComp : CameraComponents)
	{
		if (!IsValid(CameraComp))
		{
			continue;
		}

		if (CameraComp->GetFName() == ComponentName)
		{
			return CameraComp;
		}
	}

	return nullptr;
}

USkeletalMeshComponent* ASpellRiseCharacterBase::GetVisualMeshComponent() const
{
	if (USkeletalMeshComponent* VisualOverrideMesh = ResolveSkeletalMeshFromChildActorComponent(VisualMeshComponentName))
	{
		return VisualOverrideMesh;
	}

	if (USkeletalMeshComponent* VisualMesh = FindCharacterSkeletalMeshComponentByName(VisualMeshComponentName))
	{
		return VisualMesh;
	}

	return GetMesh();
}

FVector ASpellRiseCharacterBase::GetDamageNumberWorldLocation() const
{
	if (const USkeletalMeshComponent* VisualMesh = GetVisualMeshComponent())
	{
		const FBoxSphereBounds MeshBounds = VisualMesh->Bounds;
		return FVector(
			MeshBounds.Origin.X,
			MeshBounds.Origin.Y,
			MeshBounds.Origin.Z + MeshBounds.BoxExtent.Z + 20.f);
	}

	return GetActorLocation() + FVector(0.f, 0.f, 110.f);
}

USkeletalMeshComponent* ASpellRiseCharacterBase::GetEquipmentAttachMeshComponent() const
{
	if (USkeletalMeshComponent* AttachMeshFromChildActor = ResolveSkeletalMeshFromChildActorComponent(EquipmentAttachMeshComponentName))
	{
		return AttachMeshFromChildActor;
	}

	if (USkeletalMeshComponent* AttachMesh = FindCharacterSkeletalMeshComponentByName(EquipmentAttachMeshComponentName))
	{
		return AttachMesh;
	}

	return GetVisualMeshComponent();
}

USkeletalMeshComponent* ASpellRiseCharacterBase::GetEquipmentAttachMeshComponentForSocket(FName TargetSocket) const
{
	if (TargetSocket == NAME_None)
	{
		return GetEquipmentAttachMeshComponent();
	}

	if (UChildActorComponent* ChildActorComp = FindCharacterChildActorComponentByName(EquipmentAttachMeshComponentName))
	{
		if (AActor* ChildActor = ChildActorComp->GetChildActor())
		{
			TArray<USkeletalMeshComponent*> ChildMeshes;
			ChildActor->GetComponents<USkeletalMeshComponent>(ChildMeshes);
			for (USkeletalMeshComponent* MeshComp : ChildMeshes)
			{
				if (IsValid(MeshComp) && MeshComp->DoesSocketExist(TargetSocket))
				{
					return MeshComp;
				}
			}
		}
	}

	if (USkeletalMeshComponent* NamedAttachMesh = FindCharacterSkeletalMeshComponentByName(EquipmentAttachMeshComponentName))
	{
		if (NamedAttachMesh->DoesSocketExist(TargetSocket))
		{
			return NamedAttachMesh;
		}
	}

	if (USkeletalMeshComponent* MainMesh = GetMesh())
	{
		if (MainMesh->DoesSocketExist(TargetSocket))
		{
			return MainMesh;
		}
	}

	return GetEquipmentAttachMeshComponent();
}

UCameraComponent* ASpellRiseCharacterBase::GetActiveAimCameraComponent() const
{
	if (UCameraComponent* NamedCamera = FindCharacterCameraComponentByName(AimCameraComponentName))
	{
		return NamedCamera;
	}

	TArray<UCameraComponent*> CameraComponents;
	GetComponents<UCameraComponent>(CameraComponents);

	for (UCameraComponent* CameraComp : CameraComponents)
	{
		if (IsValid(CameraComp) && CameraComp->IsActive())
		{
			return CameraComp;
		}
	}

	for (UCameraComponent* CameraComp : CameraComponents)
	{
		if (IsValid(CameraComp))
		{
			return CameraComp;
		}
	}

	return nullptr;
}

USpellRiseEquipmentManagerComponent* ASpellRiseCharacterBase::GetSpellRiseEquipmentManager() const
{
	return FindComponentByClass<USpellRiseEquipmentManagerComponent>();
}

USpellRiseWeaponComponent* ASpellRiseCharacterBase::GetSpellRiseWeaponComponent() const
{
	return WeaponComponent ? WeaponComponent.Get() : FindComponentByClass<USpellRiseWeaponComponent>();
}

void ASpellRiseCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ASpellRiseCharacterBase, SelectedAbilityInputTag, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ASpellRiseCharacterBase, Archetype, COND_None, REPNOTIFY_Always);
}

void ASpellRiseCharacterBase::MultiRefreshEquipmentVisuals_Implementation()
{
}

void ASpellRiseCharacterBase::ServerHandleNarrativeItemActivationForEquipment_Implementation(UObject* ItemObject, bool bShouldEquip)
{
	if (!HasAuthority())
	{
		return;
	}

	UEquippableItem* EquippableItem = Cast<UEquippableItem>(ItemObject);
	if (!EquippableItem)
	{
		UE_LOG(LogSpellRiseEquipTrace, Warning,
			TEXT("Character.HandleItemActivation abortado. Item=%s ShouldEquip=%s"),
			*GetNameSafe(ItemObject),
			bShouldEquip ? TEXT("true") : TEXT("false"));
		return;
	}

	if (EquippableItem->GetOwningPawn() != this)
	{
		const bool bOwnedByController = (EquippableItem->GetOwningController() != nullptr && EquippableItem->GetOwningController() == GetController());
		if (bOwnedByController)
		{
			UE_LOG(LogSpellRiseEquipTrace, Verbose,
				TEXT("Character.HandleItemActivation ownership aceito via controller. Item=%s Controller=%s"),
				*GetNameSafe(EquippableItem),
				*GetNameSafe(GetController()));
		}
		else
		{
		UE_LOG(LogSpellRiseEquipTrace, Warning,
			TEXT("Character.HandleItemActivation rejeitado por ownership. Item=%s OwningPawn=%s Character=%s"),
			*GetNameSafe(EquippableItem),
			*GetNameSafe(EquippableItem->GetOwningPawn()),
			*GetNameSafe(this));
		return;
		}
	}

	UE_LOG(LogSpellRiseEquipTrace, Log,
		TEXT("Character.HandleItemActivation. Item=%s ShouldEquip=%s HasAuthority=%s"),
		*GetNameSafe(EquippableItem),
		bShouldEquip ? TEXT("true") : TEXT("false"),
		HasAuthority() ? TEXT("true") : TEXT("false"));

	if (USpellRiseWeaponComponent* ResolvedWeaponComponent = GetSpellRiseWeaponComponent())
	{
		if (ResolvedWeaponComponent->IsWeaponItem(EquippableItem))
		{
			const bool bHandledByWeaponComponent = bShouldEquip
				? ResolvedWeaponComponent->EquipWeapon(EquippableItem)
				: ResolvedWeaponComponent->UnequipWeaponItem(EquippableItem);
			if (bHandledByWeaponComponent)
			{
				return;
			}
		}
	}

	USpellRiseEquipmentManagerComponent* ResolvedEquipmentManager = GetSpellRiseEquipmentManager();
	if (!ResolvedEquipmentManager)
	{
		UE_LOG(LogSpellRiseEquipTrace, Warning,
			TEXT("Character.HandleItemActivation abortado: EquipmentManager ausente para item nao-weapon. Item=%s ShouldEquip=%s"),
			*GetNameSafe(EquippableItem),
			bShouldEquip ? TEXT("true") : TEXT("false"));
		return;
	}

	const bool bHandledBySpellRise = bShouldEquip
		? ResolvedEquipmentManager->RequestEquipItem(EquippableItem)
		: ResolvedEquipmentManager->RequestUnequipItem(EquippableItem);

	if (bHandledBySpellRise)
	{
		return;
	}

	const bool bLooksLikeWeaponItem = (EquippableItem->GetClass()->FindPropertyByName(TEXT("WeaponClass")) != nullptr);
	if (bLooksLikeWeaponItem)
	{
		UE_LOG(LogSpellRiseEquipTrace, Warning,
			TEXT("Character.HandleItemActivation weapon rejeitada pelo SpellRiseEquipmentManager sem fallback Narrative. Item=%s"),
			*GetNameSafe(EquippableItem));
		return;
	}

	UE_LOG(LogSpellRiseEquipTrace, Warning,
		TEXT("Character.HandleItemActivation fallback para Narrative Equipment. Item=%s ShouldEquip=%s"),
		*GetNameSafe(EquippableItem),
		bShouldEquip ? TEXT("true") : TEXT("false"));

	UEquipmentComponent* EquipmentComponent = FindComponentByClass<UEquipmentComponent>();
	if (!EquipmentComponent)
	{
		UE_LOG(LogSpellRiseEquipTrace, Warning,
			TEXT("Character.HandleItemActivation fallback falhou: EquipmentComponent ausente. Item=%s"),
			*GetNameSafe(EquippableItem));
		return;
	}

	EquipmentComponent->ApplyEquippableVisual(EquippableItem, bShouldEquip);
}

void ASpellRiseCharacterBase::SetArchetype(ESpellRiseArchetype NewArchetype)
{
	if (HasAuthority())
	{
		const ESpellRiseArchetype OldArchetype = Archetype;
		Archetype = NewArchetype;
		ApplyArchetypeToPrimaries_Server();
		HandleArchetypeChanged(OldArchetype);
		return;
	}

	FString RejectReason;
	if (!CanIssueOwnerServerRpc(RejectReason))
	{
		return;
	}

	ServerSetArchetype(NewArchetype);
}

void ASpellRiseCharacterBase::ServerSetArchetype_Implementation(ESpellRiseArchetype NewArchetype)
{
	FString RejectReason;
	if (!ValidateServerRpcOwnerContext(RejectReason))
	{
		AuditRejectedServerRpc(TEXT("ServerSetArchetype"), RejectReason);
		return;
	}

	if (IsDead())
	{
		AuditRejectedServerRpc(TEXT("ServerSetArchetype"), TEXT("character_is_dead"));
		return;
	}

	if (!CheckServerRpcRateLimit(
		ArchetypeRpcRateState,
		ServerArchetypeRpcRateLimitWindowSeconds,
		ServerArchetypeRpcRateLimitMaxCountPerWindow,
		TEXT("ServerSetArchetype"),
		RejectReason))
	{
		AuditRejectedServerRpc(TEXT("ServerSetArchetype"), RejectReason);
		return;
	}

	if (!SR_IsValidArchetype(NewArchetype))
	{
		AuditRejectedServerRpc(TEXT("ServerSetArchetype"), TEXT("invalid_archetype_value"));
		return;
	}

	if (Archetype == NewArchetype)
	{
		return;
	}

	const ESpellRiseArchetype OldArchetype = Archetype;
	Archetype = NewArchetype;
	ApplyArchetypeToPrimaries_Server();
	HandleArchetypeChanged(OldArchetype);
}

void ASpellRiseCharacterBase::ApplyArchetypeToPrimaries_Server()
{
	if (!HasAuthority() || !GetSpellRiseASC())
	{
		return;
	}

	constexpr float Baseline = 20.f;

	float dSTR = 0.f;
	float dAGI = 0.f;
	float dINT = 0.f;
	float dWIS = 0.f;

	switch (Archetype)
	{
	case ESpellRiseArchetype::Warrior:
		dSTR = 20.f;
		dAGI = 20.f;
		break;
	case ESpellRiseArchetype::Assassin:
		dAGI = 20.f;
		dSTR = 10.f;
		dWIS = 10.f;
		break;
	case ESpellRiseArchetype::Mage:
		dINT = 20.f;
		dWIS = 20.f;
		break;
	case ESpellRiseArchetype::Battlemage:
		dSTR = 10.f;
		dAGI = 10.f;
		dINT = 10.f;
		dWIS = 10.f;
		break;
	case ESpellRiseArchetype::Cleric:
		dWIS = 30.f;
		dSTR = 10.f;
		break;
	case ESpellRiseArchetype::None:
	default:
		break;
	}

	GetSpellRiseASC()->SetNumericAttributeBase(UCombatAttributeSet::GetStrengthAttribute(), Baseline + dSTR);
	GetSpellRiseASC()->SetNumericAttributeBase(UCombatAttributeSet::GetAgilityAttribute(), Baseline + dAGI);
	GetSpellRiseASC()->SetNumericAttributeBase(UCombatAttributeSet::GetIntelligenceAttribute(), Baseline + dINT);
	GetSpellRiseASC()->SetNumericAttributeBase(UCombatAttributeSet::GetWisdomAttribute(), Baseline + dWIS);

	RecalculateDerivedStats();
	ApplyDerivedStatsInfinite();
}

void ASpellRiseCharacterBase::ServerSetSelectedAbilityInputTag_Implementation(FGameplayTag NewTag)
{
	FString RejectReason;
	if (!ValidateServerRpcOwnerContext(RejectReason))
	{
		AuditRejectedServerRpc(TEXT("ServerSetSelectedAbilityInputTag"), RejectReason);
		return;
	}

	if (IsDead())
	{
		AuditRejectedServerRpc(TEXT("ServerSetSelectedAbilityInputTag"), TEXT("character_is_dead"));
		return;
	}

	if (!CheckServerRpcRateLimit(
		AbilitySelectionRpcRateState,
		ServerAbilitySelectionRpcRateLimitWindowSeconds,
		ServerAbilitySelectionRpcRateLimitMaxCountPerWindow,
		TEXT("ServerSetSelectedAbilityInputTag"),
		RejectReason))
	{
		AuditRejectedServerRpc(TEXT("ServerSetSelectedAbilityInputTag"), RejectReason);
		return;
	}

	if (!SR_IsValidAbilityWheelInputTag(NewTag))
	{
		AuditRejectedServerRpc(TEXT("ServerSetSelectedAbilityInputTag"), FString::Printf(TEXT("invalid_ability_wheel_tag(%s)"), *NewTag.ToString()));
		return;
	}

	if (!GetSpellRiseASC())
	{
		AuditRejectedServerRpc(TEXT("ServerSetSelectedAbilityInputTag"), TEXT("missing_asc_context"));
		return;
	}

	if (SelectedAbilityInputTag.MatchesTagExact(NewTag))
	{
		return;
	}

	const FGameplayTag OldTag = SelectedAbilityInputTag;
	SelectedAbilityInputTag = NewTag;
	HandleSelectedAbilityInputTagChanged(OldTag);
}

void ASpellRiseCharacterBase::SelectAbilitySlot(int32 SlotIndex)
{
	FGameplayTag NewTag;

	if (ASpellRisePlayerState* SpellRisePlayerState = GetPlayerState<ASpellRisePlayerState>())
	{
		if (USpellRiseAbilityHotbarComponent* Hotbar = SpellRisePlayerState->GetAbilityHotbarComponent())
		{
			FSpellRiseAbilityHotbarSlot Slot;
			if (Hotbar->GetSlot(SlotIndex, Slot))
			{
				NewTag = Slot.AbilityInputTag;
			}
		}
	}

	if (!NewTag.IsValid() && SlotIndex >= 0 && SlotIndex < 16)
	{
		NewTag = SpellRiseTags::Input_AbilitySlot(SlotIndex);
	}

	if (!HasAuthority())
	{
		FString RejectReason;
		if (!CanIssueOwnerServerRpc(RejectReason))
		{
		}
		else
		{
		ServerSetSelectedAbilityInputTag(NewTag);
		}
	}

	const FGameplayTag OldTag = SelectedAbilityInputTag;
	SelectedAbilityInputTag = NewTag;
	HandleSelectedAbilityInputTagChanged(OldTag);
}

void ASpellRiseCharacterBase::AbilityWheelInputPressed(int32 SlotIndex)
{
	SelectAbilitySlot(SlotIndex);

	if (!IsLocallyControlled() || !GetSpellRiseASC())
	{
		return;
	}

	if (ASpellRisePlayerState* SpellRisePlayerState = GetPlayerState<ASpellRisePlayerState>())
	{
		if (USpellRiseAbilityHotbarComponent* Hotbar = SpellRisePlayerState->GetAbilityHotbarComponent())
		{
			Hotbar->AbilitySlotPressed(SlotIndex);
			return;
		}
	}

	if (SlotIndex < 0 || SlotIndex >= 16)
	{
		return;
	}

	const FGameplayTag InputTag = SpellRiseTags::Input_AbilitySlot(SlotIndex);
	if (!InputTag.IsValid())
	{
		return;
	}

	if (USpellRiseAbilitySystemComponent* SRASC = Cast<USpellRiseAbilitySystemComponent>(GetSpellRiseASC()))
	{
		SRASC->SR_AbilityInputTagPressed(InputTag);
	}
}

void ASpellRiseCharacterBase::AbilityWheelInputReleased(int32 SlotIndex)
{
	if (!IsLocallyControlled() || !GetSpellRiseASC())
	{
		return;
	}

	if (ASpellRisePlayerState* SpellRisePlayerState = GetPlayerState<ASpellRisePlayerState>())
	{
		if (USpellRiseAbilityHotbarComponent* Hotbar = SpellRisePlayerState->GetAbilityHotbarComponent())
		{
			Hotbar->AbilitySlotReleased(SlotIndex);
			ClearSelectedAbility();
			return;
		}
	}

	if (SlotIndex < 0 || SlotIndex >= 16)
	{
		return;
	}

	const FGameplayTag InputTag = SpellRiseTags::Input_AbilitySlot(SlotIndex);
	if (!InputTag.IsValid())
	{
		return;
	}

	if (USpellRiseAbilitySystemComponent* SRASC = Cast<USpellRiseAbilitySystemComponent>(GetSpellRiseASC()))
	{
		SRASC->SR_AbilityInputTagReleased(InputTag);
		ClearSelectedAbility();
	}
}

void ASpellRiseCharacterBase::ClearSelectedAbility()
{
	FGameplayTag NewTag;
	if (USpellRiseAbilitySystemComponent* SRASC = Cast<USpellRiseAbilitySystemComponent>(GetSpellRiseASC()))
	{
		SRASC->SR_ClearSelectedSpellAbility();
	}

	if (!HasAuthority())
	{
		FString RejectReason;
		if (!CanIssueOwnerServerRpc(RejectReason))
		{
		}
		else
		{
			ServerSetSelectedAbilityInputTag(NewTag);
		}
	}

	const FGameplayTag OldTag = SelectedAbilityInputTag;
	SelectedAbilityInputTag = NewTag;
	HandleSelectedAbilityInputTagChanged(OldTag);
}

void ASpellRiseCharacterBase::OnRep_Archetype(ESpellRiseArchetype OldArchetype)
{
	HandleArchetypeChanged(OldArchetype);
}

void ASpellRiseCharacterBase::OnRep_SelectedAbilityInputTag(const FGameplayTag& OldTag)
{
	HandleSelectedAbilityInputTagChanged(OldTag);
}

void ASpellRiseCharacterBase::HandleArchetypeChanged(ESpellRiseArchetype OldArchetype)
{
	BP_OnArchetypeChanged(Archetype, OldArchetype);
}

void ASpellRiseCharacterBase::HandleSelectedAbilityInputTagChanged(const FGameplayTag& OldTag)
{
	SyncASCSelectedSpellFromReplicatedTag();
	BP_OnSelectedAbilityInputTagChanged(SelectedAbilityInputTag, OldTag);
}

bool ASpellRiseCharacterBase::IsDead() const
{
	if (const USpellRiseAbilitySystemComponent* ASC = GetSpellRiseASC())
	{
		if (DeadStateTag.IsValid())
		{
			return ASC->HasMatchingGameplayTag(DeadStateTag);
		}
	}

	return bIsDead;
}

void ASpellRiseCharacterBase::SyncDeadStateFromASC(const TCHAR* Context)
{
	const bool bOldMirror = bIsDead;
	const bool bDerivedDead = IsDead();
	bIsDead = bDerivedDead;

	if (bOldMirror != bIsDead)
	{
	}
}

void ASpellRiseCharacterBase::SyncASCSelectedSpellFromReplicatedTag()
{
	if (USpellRiseAbilitySystemComponent* SRASC = GetSpellRiseASC())
	{
		SRASC->SR_SetSelectedSpellAbilityByInputTag(SelectedAbilityInputTag);
	}
}

void ASpellRiseCharacterBase::SR_ProcessAbilityInput(float DeltaSeconds, bool bGamePaused)
{
	USpellRiseAbilitySystemComponent* SRASC = GetSpellRiseASC();
	if (!SRASC)
	{
		return;
	}

	SRASC->SR_ProcessAbilityInput(DeltaSeconds, bGamePaused);
}

void ASpellRiseCharacterBase::SR_ClearAbilityInput()
{
	if (GetSpellRiseASC())
	{
		GetSpellRiseASC()->SR_ClearAbilityInput();
	}
}

UAbilitySystemComponent* ASpellRiseCharacterBase::GetAbilitySystemComponent() const
{
	return GetSpellRiseASC();
}

USpellRiseAbilitySystemComponent* ASpellRiseCharacterBase::GetSpellRiseASC() const
{
	const ASpellRisePlayerState* SRPlayerState = GetPlayerState<ASpellRisePlayerState>();
	return SRPlayerState ? SRPlayerState->GetSpellRiseASC() : nullptr;
}

void ASpellRiseCharacterBase::InitASCActorInfo()
{
	USpellRiseAbilitySystemComponent* PreviousASC = GetSpellRiseASC();
	ASpellRisePlayerState* SRPlayerStateBeforeInit = GetPlayerState<ASpellRisePlayerState>();
	if (!InitializeAbilitySystemFromPlayerState())
	{
		ScheduleASCInitializationRetry();
	}

	if (!GetSpellRiseASC())
	{
		return;
	}

	if (PreviousASC && PreviousASC != GetSpellRiseASC())
	{
		PreviousASC->RegisterGameplayTagEvent(DeadStateTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		PreviousASC->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetHealthAttribute()).RemoveAll(this);
		PreviousASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetStrengthAttribute()).RemoveAll(this);
		PreviousASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetAgilityAttribute()).RemoveAll(this);
		PreviousASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetIntelligenceAttribute()).RemoveAll(this);
		PreviousASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetWisdomAttribute()).RemoveAll(this);
		PreviousASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMoveSpeedAttribute()).RemoveAll(this);
		PreviousASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute()).RemoveAll(this);
		PreviousASC->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetCarryWeightAttribute()).RemoveAll(this);
		PreviousASC->RegisterGameplayTagEvent(SpellRiseTags::Status_Frozen(), EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		PreviousASC->RegisterGameplayTagEvent(SpellRiseTags::Debuff_Ice(), EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

		bASCDelegatesBound = false;
		ASCDelegatesBoundSource = nullptr;
	}

	AActor* OwnerActor = this;
	if (ASpellRisePlayerState* SRPlayerState = GetPlayerState<ASpellRisePlayerState>())
	{
		OwnerActor = SRPlayerState;
	}

	GetSpellRiseASC()->SetReplicationMode(AscReplicationMode);
	GetSpellRiseASC()->InitAbilityActorInfo(OwnerActor, this);
	GetSpellRiseASC()->RefreshAbilityActorInfo();
	RefreshMovementSpeedFromAttributes(TEXT("InitASCActorInfo"));

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ASCInitializationRetryTimerHandle);
	}
}

void ASpellRiseCharacterBase::ScheduleASCInitializationRetry()
{
	UWorld* World = GetWorld();
	if (!World || HasValidASCActorInfo())
	{
		return;
	}

	if (World->GetTimerManager().IsTimerActive(ASCInitializationRetryTimerHandle))
	{
		return;
	}

	World->GetTimerManager().SetTimer(
		ASCInitializationRetryTimerHandle,
		this,
		&ASpellRiseCharacterBase::HandleASCInitializationRetry,
		0.1f,
		false);
}

void ASpellRiseCharacterBase::HandleASCInitializationRetry()
{
	InitASCActorInfo();
	BindASCDelegates();
}

bool ASpellRiseCharacterBase::HasValidASCActorInfo() const
{
	const USpellRiseAbilitySystemComponent* ASC = GetSpellRiseASC();
	if (!ASC)
	{
		return false;
	}

	const FGameplayAbilityActorInfo* ActorInfo = ASC->AbilityActorInfo.Get();
	return ActorInfo && ActorInfo->OwnerActor.Get() && ActorInfo->AvatarActor.Get() == this;
}

void ASpellRiseCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UE_LOG(LogSpellRiseCharacterRuntime, Log,
		TEXT("[Control][PossessedBy] Character=%s Controller=%s PlayerState=%s Local=%d Authority=%d"),
		*GetNameSafe(this),
		*GetNameSafe(NewController),
		*GetNameSafe(GetPlayerState()),
		IsLocallyControlled() ? 1 : 0,
		HasAuthority() ? 1 : 0);

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();

	InitASCActorInfo();
	BindASCDelegates();
	SyncDeadStateFromASC(TEXT("PossessedBy"));
	ResetLocalDeathPresentation();
	SetCharacterInputEnabled(!IsDead());
	RefreshRuntimeTickPolicy();

	if (!GetSpellRiseASC())
	{
		return;
	}

	if (!HasAuthority())
	{
		return;
	}

	ResetDeathStateAndResources_Server();
	ApplyArchetypeToPrimaries_Server();

	if (ASpellRisePlayerState* SRPlayerState = GetPlayerState<ASpellRisePlayerState>())
	{
		SRPlayerState->GrantAbilities(StartingAbilities);
	}

	ApplyRegenStartupEffects();
	ApplyStartupEffects();
	StartResourceRegen_Server();
}

void ASpellRiseCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	UE_LOG(LogSpellRiseCharacterRuntime, Log,
		TEXT("[Control][OnRep_Controller] Character=%s Controller=%s PlayerState=%s Local=%d Authority=%d Dead=%d"),
		*GetNameSafe(this),
		*GetNameSafe(GetController()),
		*GetNameSafe(GetPlayerState()),
		IsLocallyControlled() ? 1 : 0,
		HasAuthority() ? 1 : 0,
		IsDead() ? 1 : 0);

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();
	InitASCActorInfo();
	BindASCDelegates();
	SyncDeadStateFromASC(TEXT("OnRep_Controller"));
	ResetLocalDeathPresentation();
	SetCharacterInputEnabled(!IsDead());
	RefreshRuntimeTickPolicy();
}

void ASpellRiseCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	UE_LOG(LogSpellRiseCharacterRuntime, Log,
		TEXT("[Control][OnRep_PlayerState] Character=%s Controller=%s PlayerState=%s Local=%d Authority=%d Dead=%d"),
		*GetNameSafe(this),
		*GetNameSafe(GetController()),
		*GetNameSafe(GetPlayerState()),
		IsLocallyControlled() ? 1 : 0,
		HasAuthority() ? 1 : 0,
		IsDead() ? 1 : 0);

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();

	InitASCActorInfo();
	BindASCDelegates();
	SyncDeadStateFromASC(TEXT("OnRep_PlayerState"));
	ResetLocalDeathPresentation();
	SetCharacterInputEnabled(!IsDead());
	RefreshRuntimeTickPolicy();

	if (!GetSpellRiseASC())
	{
	}
}

void ASpellRiseCharacterBase::BindASCDelegates()
{
	if (!GetSpellRiseASC())
	{
		return;
	}

	if (ASCDelegatesBoundSource != GetSpellRiseASC())
	{
		bASCDelegatesBound = false;
		ASCDelegatesBoundSource = GetSpellRiseASC();
	}

	if (bASCDelegatesBound)
	{
		return;
	}

	if (DeadStateTag.IsValid())
	{
		GetSpellRiseASC()
			->RegisterGameplayTagEvent(DeadStateTag, EGameplayTagEventType::NewOrRemoved)
			.RemoveAll(this);
	}
	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetHealthAttribute())
		.RemoveAll(this);
	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetStrengthAttribute())
		.RemoveAll(this);
	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetAgilityAttribute())
		.RemoveAll(this);
	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetIntelligenceAttribute())
		.RemoveAll(this);
	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetWisdomAttribute())
		.RemoveAll(this);
	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMoveSpeedAttribute())
		.RemoveAll(this);
	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute())
		.RemoveAll(this);
	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetCarryWeightAttribute())
		.RemoveAll(this);
	GetSpellRiseASC()
		->RegisterGameplayTagEvent(SpellRiseTags::Status_Frozen(), EGameplayTagEventType::NewOrRemoved)
		.RemoveAll(this);
	GetSpellRiseASC()
		->RegisterGameplayTagEvent(SpellRiseTags::Debuff_Ice(), EGameplayTagEventType::NewOrRemoved)
		.RemoveAll(this);

	bASCDelegatesBound = true;

	if (DeadStateTag.IsValid())
	{
		GetSpellRiseASC()
			->RegisterGameplayTagEvent(DeadStateTag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &ASpellRiseCharacterBase::OnDeadTagChanged);
	}

	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnHealthChanged);

	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetStrengthAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);

	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetAgilityAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);

	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetIntelligenceAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);

	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetWisdomAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);

	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMoveSpeedAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnMoveSpeedChanged);

	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnMoveSpeedChanged);

	GetSpellRiseASC()
		->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetCarryWeightAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnCarryWeightChanged);

	GetSpellRiseASC()
		->RegisterGameplayTagEvent(SpellRiseTags::Status_Frozen(), EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &ASpellRiseCharacterBase::OnFrozenStatusChanged);
	GetSpellRiseASC()
		->RegisterGameplayTagEvent(SpellRiseTags::Debuff_Ice(), EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &ASpellRiseCharacterBase::OnFrozenStatusChanged);

	SyncNarrativeInventoryWeightCapacityFromCarryWeight(TEXT("BindASCDelegates"));
	RefreshMovementSpeedFromAttributes(TEXT("BindASCDelegates"));
}

bool ASpellRiseCharacterBase::InitializeAbilitySystemFromPlayerState()
{
	ASpellRisePlayerState* SRPlayerState = GetPlayerState<ASpellRisePlayerState>();
	if (!SRPlayerState)
	{
		return false;
	}

	USpellRiseAbilitySystemComponent* PlayerStateASC = SRPlayerState->GetSpellRiseASC();
	if (!PlayerStateASC)
	{
		return false;
	}
	BasicAttributeSet = SRPlayerState->GetBasicAttributeSet();
	CombatAttributeSet = SRPlayerState->GetCombatAttributeSet();
	ResourceAttributeSet = SRPlayerState->GetResourceAttributeSet();
	CatalystAttributeSet = SRPlayerState->GetCatalystAttributeSet();
	DerivedStatsAttributeSet = SRPlayerState->GetDerivedStatsAttributeSet();

	return true;
}

UNarrativeInventoryComponent* ASpellRiseCharacterBase::ResolveNarrativeInventoryComponent() const
{
	if (APlayerState* OwningPlayerState = GetPlayerState())
	{
		return UInventoryFunctionLibrary::GetInventoryComponentFromTarget(OwningPlayerState);
	}

	return nullptr;
}

void ASpellRiseCharacterBase::SyncNarrativeInventoryWeightCapacityFromCarryWeight(const TCHAR* Context)
{
	if (!ResourceAttributeSet)
	{
		return;
	}

	const float CarryWeight = ResourceAttributeSet->GetCarryWeight();
	if (!FMath::IsFinite(CarryWeight) || CarryWeight <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	UNarrativeInventoryComponent* Inventory = ResolveNarrativeInventoryComponent();
	if (!Inventory)
	{
		return;
	}
	BindNarrativeInventoryWeightDelegates(Inventory);

	const float DesiredWeightCapacity = FMath::Max(0.f, CarryWeight);
	if (FMath::IsNearlyEqual(Inventory->GetWeightCapacity(), DesiredWeightCapacity, 0.01f))
	{
		RefreshInventoryEncumbranceMovement(Context);
		return;
	}

	Inventory->SetWeightCapacity(DesiredWeightCapacity);
	RefreshInventoryEncumbranceMovement(Context);

	UE_LOG(LogSpellRiseCharacterRuntime, Verbose,
		TEXT("[InventoryWeight][Sync] Character=%s Inventory=%s Capacity=%.2f Context=%s Authority=%d Local=%d"),
		*GetNameSafe(this),
		*GetNameSafe(Inventory),
		DesiredWeightCapacity,
		Context ? Context : TEXT("unknown"),
		HasAuthority() ? 1 : 0,
		IsLocallyControlled() ? 1 : 0);
}

void ASpellRiseCharacterBase::BindNarrativeInventoryWeightDelegates(UNarrativeInventoryComponent* Inventory)
{
	if (CachedNarrativeInventoryForWeight == Inventory)
	{
		return;
	}

	UnbindNarrativeInventoryWeightDelegates();
	CachedNarrativeInventoryForWeight = Inventory;

	if (CachedNarrativeInventoryForWeight)
	{
		CachedNarrativeInventoryForWeight->OnInventoryUpdated.RemoveDynamic(this, &ASpellRiseCharacterBase::OnNarrativeInventoryUpdatedForWeight);
		CachedNarrativeInventoryForWeight->OnInventoryUpdated.AddUniqueDynamic(this, &ASpellRiseCharacterBase::OnNarrativeInventoryUpdatedForWeight);
	}
}

void ASpellRiseCharacterBase::UnbindNarrativeInventoryWeightDelegates()
{
	if (CachedNarrativeInventoryForWeight)
	{
		CachedNarrativeInventoryForWeight->OnInventoryUpdated.RemoveDynamic(this, &ASpellRiseCharacterBase::OnNarrativeInventoryUpdatedForWeight);
		CachedNarrativeInventoryForWeight = nullptr;
	}
}

void ASpellRiseCharacterBase::RefreshMovementSpeedFromAttributes(const TCHAR* Context)
{
	if (!HasAuthority() && !IsLocallyControlled())
	{
		return;
	}

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!Movement)
	{
		return;
	}

	const USpellRiseAbilitySystemComponent* ASC = GetSpellRiseASC();
	const float AttributeMoveSpeed = ASC
		? ASC->GetNumericAttribute(UCombatAttributeSet::GetMoveSpeedAttribute())
		: 0.f;
	const float AttributeMoveSpeedMultiplier = ASC
		? ASC->GetNumericAttribute(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute())
		: 1.f;

	const float SpeedBase = AttributeMoveSpeed > KINDA_SMALL_NUMBER ? AttributeMoveSpeed : BaseWalkSpeed;
	float SafeAttributeMultiplier = FMath::Clamp(AttributeMoveSpeedMultiplier, 0.1f, 3.0f);
	const bool bFrozen = ASC
		&& (ASC->HasMatchingGameplayTag(SpellRiseTags::Status_Frozen())
			|| ASC->HasMatchingGameplayTag(SpellRiseTags::Debuff_Ice()));
	if (bFrozen)
	{
		SafeAttributeMultiplier = FMath::Min(SafeAttributeMultiplier, FMath::Clamp(FrozenStatusSpeedMultiplier, 0.1f, 1.0f));
	}
	const float SafeInventoryMultiplier = FMath::Max(0.f, InventoryWeightMovementInputScale);
	const float DesiredMaxWalkSpeed = FMath::Clamp(SpeedBase * SafeAttributeMultiplier * SafeInventoryMultiplier, 0.f, 1800.f);
	const float BaseSpeedForInput = FMath::Max(BaseWalkSpeed, KINDA_SMALL_NUMBER);
	GameplayMovementInputScale = FMath::Clamp((SpeedBase * SafeAttributeMultiplier) / BaseSpeedForInput, 0.f, 3.f);
	const float PreviousMaxWalkSpeed = Movement->MaxWalkSpeed;

	Movement->MaxWalkSpeed = DesiredMaxWalkSpeed;

	if (bFrozen || !FMath::IsNearlyEqual(PreviousMaxWalkSpeed, DesiredMaxWalkSpeed, 1.0f))
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Log,
			TEXT("[MovementSpeed][Refresh] Character=%s Context=%s Base=%.2f AttrMoveSpeed=%.2f AttrMultiplier=%.2f Frozen=%d InventoryMultiplier=%.2f GameplayInputScale=%.2f OldMaxWalkSpeed=%.2f NewMaxWalkSpeed=%.2f Velocity2D=%.2f Authority=%d Local=%d"),
			*GetNameSafe(this),
			Context ? Context : TEXT("unknown"),
			BaseWalkSpeed,
			AttributeMoveSpeed,
			SafeAttributeMultiplier,
			bFrozen ? 1 : 0,
			SafeInventoryMultiplier,
			GameplayMovementInputScale,
			PreviousMaxWalkSpeed,
			DesiredMaxWalkSpeed,
			Movement->Velocity.Size2D(),
			HasAuthority() ? 1 : 0,
			IsLocallyControlled() ? 1 : 0);
	}
}

void ASpellRiseCharacterBase::RefreshInventoryEncumbranceMovement(const TCHAR* Context)
{
	if (!HasAuthority() && !IsLocallyControlled())
	{
		return;
	}

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	UNarrativeInventoryComponent* Inventory = ResolveNarrativeInventoryComponent();
	if (!Movement || !Inventory)
	{
		return;
	}

	BindNarrativeInventoryWeightDelegates(Inventory);

	const float CurrentWeight = FMath::Max(0.f, Inventory->GetCurrentWeight());
	const float WeightCapacity = FMath::Max(0.f, Inventory->GetWeightCapacity());

	float SpeedMultiplier = 1.f;
	int32 NewMoveState = 0;
	if (WeightCapacity <= KINDA_SMALL_NUMBER)
	{
		if (CurrentWeight > KINDA_SMALL_NUMBER)
		{
			SpeedMultiplier = 0.f;
			NewMoveState = 2;
		}
	}
	else if (CurrentWeight > WeightCapacity * ImmobilizedWeightCapacityMultiplier)
	{
		SpeedMultiplier = 0.f;
		NewMoveState = 2;
	}
	else if (CurrentWeight > WeightCapacity)
	{
		SpeedMultiplier = EncumberedWeightSpeedMultiplier;
		NewMoveState = 1;
	}

	InventoryWeightMovementInputScale = SpeedMultiplier;
	RefreshMovementSpeedFromAttributes(Context);

	const bool bShouldBlockMovement = NewMoveState == 2;
	if (bShouldBlockMovement)
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
		bInventoryWeightMovementBlocked = true;

		if (AController* OwningController = GetController())
		{
			OwningController->SetIgnoreMoveInput(true);
		}
	}
	else if (bInventoryWeightMovementBlocked)
	{
		bInventoryWeightMovementBlocked = false;

		if (!IsDead() && Movement->MovementMode == MOVE_None)
		{
			Movement->SetMovementMode(MOVE_Walking);
		}

		if (AController* OwningController = GetController())
		{
			OwningController->SetIgnoreMoveInput(IsDead());
		}
	}

	if (CachedInventoryEncumbranceMoveState != NewMoveState)
	{
		CachedInventoryEncumbranceMoveState = NewMoveState;
		UE_LOG(LogSpellRiseCharacterRuntime, Log,
			TEXT("[InventoryWeight][Movement] Character=%s State=%d CurrentWeight=%.2f Capacity=%.2f MaxWalkSpeed=%.2f Context=%s Authority=%d Local=%d"),
			*GetNameSafe(this),
			NewMoveState,
			CurrentWeight,
			WeightCapacity,
			Movement->MaxWalkSpeed,
			Context ? Context : TEXT("unknown"),
			HasAuthority() ? 1 : 0,
			IsLocallyControlled() ? 1 : 0);
	}
}

void ASpellRiseCharacterBase::OnNarrativeInventoryUpdatedForWeight()
{
	RefreshInventoryEncumbranceMovement(TEXT("InventoryUpdated"));
}

void ASpellRiseCharacterBase::OnPrimaryChanged(const FOnAttributeChangeData& Data)
{
	if (!HasAuthority() || !GetSpellRiseASC())
	{
		return;
	}

	RecalculateDerivedStats();
	ApplyDerivedStatsInfinite();
}

void ASpellRiseCharacterBase::OnCarryWeightChanged(const FOnAttributeChangeData& Data)
{
	SyncNarrativeInventoryWeightCapacityFromCarryWeight(TEXT("CarryWeightChanged"));
}

void ASpellRiseCharacterBase::OnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	RefreshMovementSpeedFromAttributes(TEXT("MoveSpeedChanged"));
}

void ASpellRiseCharacterBase::OnFrozenStatusChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	RefreshMovementSpeedFromAttributes(NewCount > 0 ? TEXT("FrozenApplied") : TEXT("FrozenRemoved"));
}

void ASpellRiseCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!GetSpellRiseASC() || GetSpellRiseASC()->GetAvatarActor() != this)
	{
		return;
	}

	if (HasAuthority() && Data.NewValue < Data.OldValue)
	{
		RefreshCombatLockFromDamage_Server(Data.OldValue - Data.NewValue);
	}

	if (IsLocallyControlled() &&
		bEnableLocalDamageScreenEffect &&
		Data.NewValue < Data.OldValue &&
		Data.NewValue > 0.f)
	{
		TriggerLocalDamageScreenEffect();
	}

	if (IsDead())
	{
		return;
	}

	const float NewHealth = Data.NewValue;
	if (NewHealth > 0.f)
	{
		return;
	}

	if (!HasAuthority() || !GetSpellRiseASC())
	{
		return;
	}

	bIsDead = true;

	if (GE_Death)
	{
		FGameplayEffectContextHandle Context = GetSpellRiseASC()->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = GetSpellRiseASC()->MakeOutgoingSpec(GE_Death, 1.f, Context);
		if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
		{
			GetSpellRiseASC()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		else if (DeadStateTag.IsValid())
		{
			GetSpellRiseASC()->AddLooseGameplayTag(DeadStateTag);
		}
	}
	else if (DeadStateTag.IsValid())
	{
		GetSpellRiseASC()->AddLooseGameplayTag(DeadStateTag);
	}

	GetSpellRiseASC()->CancelAllAbilities();
	StopResourceRegen_Server();
	SyncDeadStateFromASC(TEXT("OnHealthChanged"));
	RefreshRuntimeTickPolicy();
	ProcessFullLootDrop_Server(GetActorLocation());
	ScheduleCorpseDespawn_Server();
	ScheduleRespawn_Server();
}

void ASpellRiseCharacterBase::ApplyStartupEffects()
{
	if (!GetSpellRiseASC() || !HasAuthority())
	{
		return;
	}

	RecalculateDerivedStats();
	ApplyDerivedStatsInfinite();
}

void ASpellRiseCharacterBase::ApplyDerivedStatsInfinite()
{
	if (!GetSpellRiseASC() || !HasAuthority())
	{
		return;
	}

	if (!GE_DerivedStatsInfinite)
	{
		return;
	}

	GetSpellRiseASC()->RemoveActiveGameplayEffectBySourceEffect(GE_DerivedStatsInfinite, GetSpellRiseASC());

	FGameplayEffectContextHandle Context = GetSpellRiseASC()->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = GetSpellRiseASC()->MakeOutgoingSpec(GE_DerivedStatsInfinite, 1.f, Context);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return;
	}

	GetSpellRiseASC()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	SyncNarrativeInventoryWeightCapacityFromCarryWeight(TEXT("ApplyDerivedStatsInfinite"));
}

void ASpellRiseCharacterBase::ApplyOrRefreshEffect(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!GetSpellRiseASC() || !HasAuthority() || !EffectClass)
	{
		return;
	}

	GetSpellRiseASC()->RemoveActiveGameplayEffectBySourceEffect(EffectClass, GetSpellRiseASC());

	FGameplayEffectContextHandle Context = GetSpellRiseASC()->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = GetSpellRiseASC()->MakeOutgoingSpec(EffectClass, 1.f, Context);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return;
	}

	GetSpellRiseASC()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void ASpellRiseCharacterBase::SetCharacterInputEnabled(bool bEnabled)
{
	if (!IsLocallyControlled())
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Verbose,
			TEXT("[Control][InputSkip] Character=%s Reason=not_locally_controlled Controller=%s PlayerState=%s WantedEnabled=%d Authority=%d"),
			*GetNameSafe(this),
			*GetNameSafe(GetController()),
			*GetNameSafe(GetPlayerState()),
			bEnabled ? 1 : 0,
			HasAuthority() ? 1 : 0);
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning,
			TEXT("[Control][InputSkip] Character=%s Reason=missing_player_controller Controller=%s PlayerState=%s WantedEnabled=%d Authority=%d"),
			*GetNameSafe(this),
			*GetNameSafe(GetController()),
			*GetNameSafe(GetPlayerState()),
			bEnabled ? 1 : 0,
			HasAuthority() ? 1 : 0);
		return;
	}

	UE_LOG(LogSpellRiseCharacterRuntime, Log,
		TEXT("[Control][InputState] Character=%s Controller=%s Enabled=%d Authority=%d"),
		*GetNameSafe(this),
		*GetNameSafe(PC),
		bEnabled ? 1 : 0,
		HasAuthority() ? 1 : 0);

	if (bEnabled)
	{
		ResetLocalDeathPresentation();
		EnableInput(PC);
		PC->SetIgnoreMoveInput(bInventoryWeightMovementBlocked);
		PC->SetIgnoreLookInput(false);
		return;
	}

	SR_ClearAbilityInput();
	if (GetSpellRiseASC())
	{
		GetSpellRiseASC()->SR_ClearAbilityInput();
	}

	DisableInput(PC);
	PC->SetIgnoreMoveInput(true);
	PC->SetIgnoreLookInput(true);
}

void ASpellRiseCharacterBase::ApplyRegenStartupEffects()
{
	if (!GetSpellRiseASC() || !HasAuthority())
	{
		return;
	}

	if (GE_RegenBase)
	{
		ApplyOrRefreshEffect(GE_RegenBase);
		return;
	}

	for (const TSubclassOf<UGameplayEffect>& RegenGE : GE_RegenEffects)
	{
		ApplyOrRefreshEffect(RegenGE);
	}
}

void ASpellRiseCharacterBase::StartResourceRegen_Server()
{
	if (!HasAuthority() || IsDead() || !GetSpellRiseASC())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float TickInterval = FMath::Max(0.1f, ResourceRegenTickIntervalSeconds);
	World->GetTimerManager().SetTimer(
		ResourceRegenTimerHandle,
		this,
		&ASpellRiseCharacterBase::ApplyResourceRegenTick_Server,
		TickInterval,
		true,
		TickInterval);
}

void ASpellRiseCharacterBase::StopResourceRegen_Server()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ResourceRegenTimerHandle);
	}
}

void ASpellRiseCharacterBase::ApplyResourceRegenTick_Server()
{
	USpellRiseAbilitySystemComponent* ASC = GetSpellRiseASC();
	if (!HasAuthority() || !ASC || IsDead())
	{
		StopResourceRegen_Server();
		return;
	}

	const float TickInterval = FMath::Max(0.1f, ResourceRegenTickIntervalSeconds);
	const bool bInCombat = IsCombatLockActive_Server();
	const bool bBleedingBlocksRegen = BleedingBlocksRegenTag.IsValid() && ASC->HasMatchingGameplayTag(BleedingBlocksRegenTag);
	const bool bManaRegenPenalized = ManaRegenPenaltyTag.IsValid() && ASC->HasMatchingGameplayTag(ManaRegenPenaltyTag);
	const bool bStaminaRegenPaused = (StaminaRegenPausedTag.IsValid() && ASC->HasMatchingGameplayTag(StaminaRegenPausedTag))
		|| (StaminaRegenPausedWhileTags.Num() > 0 && ASC->HasAnyMatchingGameplayTags(StaminaRegenPausedWhileTags));

	const auto ApplyRegen = [ASC, TickInterval](const FGameplayAttribute& ResourceAttribute, const FGameplayAttribute& MaxResourceAttribute, const FGameplayAttribute& RegenAttribute, float RegenMultiplier)
	{
		RegenMultiplier = FMath::Clamp(RegenMultiplier, 0.f, 1.f);
		if (RegenMultiplier <= 0.f)
		{
			return;
		}

		const float CurrentValue = ASC->GetNumericAttribute(ResourceAttribute);
		const float MaxValue = ASC->GetNumericAttribute(MaxResourceAttribute);
		const float RegenPerSecond = ASC->GetNumericAttribute(RegenAttribute) * RegenMultiplier;

		if (MaxValue <= 0.f || CurrentValue >= MaxValue || RegenPerSecond <= 0.f)
		{
			return;
		}

		const float RegenDelta = FMath::Min(RegenPerSecond * TickInterval, MaxValue - CurrentValue);
		if (RegenDelta > 0.f)
		{
			ASC->ApplyModToAttributeUnsafe(ResourceAttribute, EGameplayModOp::Additive, RegenDelta);
		}
	};

	if (!bBleedingBlocksRegen)
	{
		ApplyRegen(
			UResourceAttributeSet::GetHealthAttribute(),
			UResourceAttributeSet::GetMaxHealthAttribute(),
			UResourceAttributeSet::GetHealthRegenAttribute(),
			bInCombat ? CombatHealthRegenMultiplier : 1.f);
	}

	float ManaMultiplier = bInCombat ? CombatManaRegenMultiplier : 1.f;
	if (bManaRegenPenalized)
	{
		ManaMultiplier *= ManaRegenDebuffMultiplier;
	}

	ApplyRegen(
		UResourceAttributeSet::GetManaAttribute(),
		UResourceAttributeSet::GetMaxManaAttribute(),
		UResourceAttributeSet::GetManaRegenAttribute(),
		ManaMultiplier);

	if (!bStaminaRegenPaused)
	{
		ApplyRegen(
			UResourceAttributeSet::GetStaminaAttribute(),
			UResourceAttributeSet::GetMaxStaminaAttribute(),
			UResourceAttributeSet::GetStaminaRegenAttribute(),
			bInCombat ? CombatStaminaRegenMultiplier : 1.f);
	}
}

void ASpellRiseCharacterBase::RecalculateDerivedStats()
{
	if (!GetSpellRiseASC() || !HasAuthority())
	{
		return;
	}

	ApplyOrRefreshEffect(GE_RecalculateResources);
}

void ASpellRiseCharacterBase::ResetDeathStateAndResources_Server()
{
	if (!HasAuthority() || !GetSpellRiseASC())
	{
		return;
	}

	if (DeadStateTag.IsValid())
	{
		FGameplayTagContainer DeadTags;
		DeadTags.AddTag(DeadStateTag);
		GetSpellRiseASC()->RemoveActiveEffectsWithGrantedTags(DeadTags);
	}

	if (GE_Death)
	{
		GetSpellRiseASC()->RemoveActiveGameplayEffectBySourceEffect(GE_Death, GetSpellRiseASC());
	}

	if (DeadStateTag.IsValid() && GetSpellRiseASC()->HasMatchingGameplayTag(DeadStateTag))
	{
		GetSpellRiseASC()->RemoveLooseGameplayTag(DeadStateTag);
	}

	bIsDead = false;
	bFullLootProcessedForCurrentDeath = false;
	SyncDeadStateFromASC(TEXT("ResetDeathStateAndResources_Server"));
	RefreshRuntimeTickPolicy();

	const float MaxHealthValue = GetSpellRiseASC()->GetNumericAttribute(UResourceAttributeSet::GetMaxHealthAttribute());
	const float MaxManaValue = GetSpellRiseASC()->GetNumericAttribute(UResourceAttributeSet::GetMaxManaAttribute());
	const float MaxStaminaValue = GetSpellRiseASC()->GetNumericAttribute(UResourceAttributeSet::GetMaxStaminaAttribute());

	GetSpellRiseASC()->SetNumericAttributeBase(UResourceAttributeSet::GetHealthAttribute(), FMath::Max(1.f, MaxHealthValue));
	GetSpellRiseASC()->SetNumericAttributeBase(UResourceAttributeSet::GetManaAttribute(), FMath::Max(0.f, MaxManaValue));
	GetSpellRiseASC()->SetNumericAttributeBase(UResourceAttributeSet::GetStaminaAttribute(), FMath::Max(0.f, MaxStaminaValue));

}

void ASpellRiseCharacterBase::ServerSendGameplayEventToSelf_Implementation(const FGameplayEventData& EventData)
{
	if (!IsAllowedServerEventTag(EventData.EventTag))
	{
		AuditRejectedServerGameplayEvent(EventData.EventTag, TEXT("tag_not_allowlisted"));
		return;
	}

	FString RejectReason;
	if (!ValidateServerGameplayEventPayload(EventData, RejectReason))
	{
		AuditRejectedServerGameplayEvent(EventData.EventTag, RejectReason);
		return;
	}

	if (!CheckServerGameplayEventRateLimit(EventData.EventTag, RejectReason))
	{
		AuditRejectedServerGameplayEvent(EventData.EventTag, RejectReason);
		return;
	}

	FGameplayEventData SanitizedEventData = EventData;
	SanitizedEventData.Instigator = this;
	SanitizedEventData.Target = this;

	if (!FMath::IsFinite(SanitizedEventData.EventMagnitude))
	{
		SanitizedEventData.EventMagnitude = 0.f;
	}

	if (!GetSpellRiseASC())
	{
		InitASCActorInfo();
	}

	if (!GetSpellRiseASC())
	{
		AuditRejectedServerGameplayEvent(SanitizedEventData.EventTag, TEXT("missing_asc_context"));
		return;
	}

	AActor* EventReceiverActor = GetPlayerState() ? Cast<AActor>(GetPlayerState()) : Cast<AActor>(this);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(EventReceiverActor, SanitizedEventData.EventTag, SanitizedEventData);
}

void ASpellRiseCharacterBase::MultiSendGameplayEventToActor_Implementation(AActor* TargetActor, FGameplayEventData EventData)
{
	if (!TargetActor || !EventData.EventTag.IsValid())
	{
		return;
	}

	if (!FMath::IsFinite(EventData.EventMagnitude))
	{
		EventData.EventMagnitude = 0.f;
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, EventData.EventTag, EventData);
}

bool ASpellRiseCharacterBase::IsAllowedServerEventTag(const FGameplayTag& EventTag) const
{
	if (!EventTag.IsValid())
	{
		return false;
	}

	if (AllowedServerEventTags.HasTagExact(EventTag))
	{
		return true;
	}

	for (const FGameplayTag& AllowedTag : AllowedServerEventTags)
	{
		if (AllowedTag.IsValid() && EventTag.MatchesTag(AllowedTag))
		{
			return true;
		}
	}

	return false;
}

bool ASpellRiseCharacterBase::ValidateServerGameplayEventPayload(const FGameplayEventData& EventData, FString& OutRejectReason) const
{
	if (!HasAuthority())
	{
		OutRejectReason = TEXT("not_authority");
		return false;
	}

	if (!EventData.EventTag.IsValid())
	{
		OutRejectReason = TEXT("invalid_event_tag");
		return false;
	}

	if (!GetController())
	{
		OutRejectReason = TEXT("missing_controller_context");
		return false;
	}

	if (!FMath::IsFinite(EventData.EventMagnitude))
	{
		OutRejectReason = TEXT("invalid_magnitude_nan_inf");
		return false;
	}

	if (EventData.OptionalObject || EventData.OptionalObject2)
	{
		OutRejectReason = TEXT("optional_object_payload_blocked");
		return false;
	}

	if (EventData.ContextHandle.IsValid())
	{
		OutRejectReason = TEXT("client_context_handle_blocked");
		return false;
	}

	if (EventData.TargetData.Num() > 0)
	{
		OutRejectReason = TEXT("target_data_not_allowed_on_generic_event_rpc");
		return false;
	}

	if (EventData.InstigatorTags.Num() > 0 || EventData.TargetTags.Num() > 0)
	{
		OutRejectReason = TEXT("client_tag_container_payload_blocked");
		return false;
	}

	const float MaxAbsMagnitude = ResolveMaxAbsServerEventMagnitude(EventData.EventTag);
	if (MaxAbsMagnitude >= 0.f && FMath::Abs(EventData.EventMagnitude) > MaxAbsMagnitude)
	{
		OutRejectReason = FString::Printf(TEXT("magnitude_out_of_range(%.2f>%.2f)"), FMath::Abs(EventData.EventMagnitude), MaxAbsMagnitude);
		return false;
	}

	return true;
}

bool ASpellRiseCharacterBase::CheckServerGameplayEventRateLimit(const FGameplayTag& EventTag, FString& OutRejectReason)
{
	if (!EventTag.IsValid())
	{
		OutRejectReason = TEXT("invalid_event_tag_rate_limit");
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		OutRejectReason = TEXT("missing_world_context");
		return false;
	}

	const double Now = static_cast<double>(World->GetTimeSeconds());
	FSpellRiseServerEventRateLimitState& RateState = ServerEventRateLimitByTag.FindOrAdd(EventTag);

	const double WindowSeconds = static_cast<double>(FMath::Max(0.01f, ServerGameplayEventRateLimitWindowSeconds));
	if ((Now - RateState.WindowStartServerTimeSeconds) > WindowSeconds)
	{
		RateState.WindowStartServerTimeSeconds = Now;
		RateState.EventsInWindow = 0;
	}

	RateState.EventsInWindow += 1;
	if (RateState.EventsInWindow > FMath::Max(1, ServerGameplayEventRateLimitMaxCountPerWindow))
	{
		RateState.RejectCount += 1;
		OutRejectReason = FString::Printf(
			TEXT("rate_limit_exceeded(window=%.2fs count=%d max=%d)"),
			WindowSeconds,
			RateState.EventsInWindow,
			ServerGameplayEventRateLimitMaxCountPerWindow);
		return false;
	}

	return true;
}

bool ASpellRiseCharacterBase::ValidateServerRpcOwnerContext(FString& OutRejectReason) const
{
	if (!HasAuthority())
	{
		OutRejectReason = TEXT("not_authority");
		return false;
	}

	const AController* OwnerController = GetController();
	if (!OwnerController)
	{
		OutRejectReason = TEXT("missing_controller_context");
		return false;
	}

	if (OwnerController->GetPawn() != this)
	{
		OutRejectReason = TEXT("owner_pawn_context_mismatch");
		return false;
	}

	if (!GetPlayerState() || OwnerController->PlayerState != GetPlayerState())
	{
		OutRejectReason = TEXT("owner_playerstate_context_mismatch");
		return false;
	}

	return true;
}

bool ASpellRiseCharacterBase::CanIssueOwnerServerRpc(FString& OutRejectReason) const
{
	if (!IsLocallyControlled())
	{
		OutRejectReason = TEXT("character_not_locally_controlled");
		return false;
	}

	const AController* OwnerController = GetController();
	const APlayerController* OwnerPC = Cast<APlayerController>(OwnerController);
	if (!OwnerPC)
	{
		OutRejectReason = TEXT("missing_player_controller_context");
		return false;
	}

	if (!OwnerPC->IsLocalController())
	{
		OutRejectReason = TEXT("controller_not_local");
		return false;
	}

	if (OwnerController->GetPawn() != this)
	{
		OutRejectReason = TEXT("controller_pawn_mismatch");
		return false;
	}

	return true;
}

bool ASpellRiseCharacterBase::CheckServerRpcRateLimit(
	FSpellRiseServerEventRateLimitState& RateState,
	float WindowSeconds,
	int32 MaxCountPerWindow,
	const TCHAR* RpcName,
	FString& OutRejectReason)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		OutRejectReason = TEXT("missing_world_context");
		return false;
	}

	const double Now = static_cast<double>(World->GetTimeSeconds());
	const double EffectiveWindowSeconds = static_cast<double>(FMath::Max(0.01f, WindowSeconds));

	if ((Now - RateState.WindowStartServerTimeSeconds) > EffectiveWindowSeconds)
	{
		RateState.WindowStartServerTimeSeconds = Now;
		RateState.EventsInWindow = 0;
	}

	RateState.EventsInWindow += 1;
	const int32 EffectiveMaxCount = FMath::Max(1, MaxCountPerWindow);
	if (RateState.EventsInWindow > EffectiveMaxCount)
	{
		RateState.RejectCount += 1;
		OutRejectReason = FString::Printf(
			TEXT("rate_limit_exceeded(rpc=%s window=%.2fs count=%d max=%d)"),
			RpcName ? RpcName : TEXT("unknown"),
			EffectiveWindowSeconds,
			RateState.EventsInWindow,
			EffectiveMaxCount);
		return false;
	}

	return true;
}

void ASpellRiseCharacterBase::AuditRejectedServerRpc(const TCHAR* RpcName, const FString& RejectReason)
{
	ServerRejectedGenericRpcs += 1;

	const AController* OwnerController = GetController();
	const APlayerState* OwnerPlayerState = GetPlayerState();

	FSpellRiseAuditTrail::AppendEvent(
		TEXT("Security"),
		TEXT("RejectedServerRpc"),
		GetNameSafe(OwnerPlayerState),
		FString::Printf(TEXT("rpc=%s reason=%s character=%s"), RpcName ? RpcName : TEXT("unknown"), *RejectReason, *GetNameSafe(this)));
}

void ASpellRiseCharacterBase::AuditRejectedServerGameplayEvent(const FGameplayTag& EventTag, const FString& RejectReason)
{
	ServerRejectedGameplayEvents += 1;

	const AController* OwnerController = GetController();
	const APlayerState* OwnerPlayerState = GetPlayerState();

	FSpellRiseAuditTrail::AppendEvent(
		TEXT("Security"),
		TEXT("RejectedGameplayEvent"),
		GetNameSafe(OwnerPlayerState),
		FString::Printf(TEXT("tag=%s reason=%s character=%s"), *EventTag.ToString(), *RejectReason, *GetNameSafe(this)));
}

float ASpellRiseCharacterBase::ResolveMaxAbsServerEventMagnitude(const FGameplayTag& EventTag) const
{
	const FGameplayTag ComboStartTag = FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.Start"), false);
	const FGameplayTag ComboInputTag = FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.Input"), false);
	const FGameplayTag ComboEndTag = FGameplayTag::RequestGameplayTag(TEXT("Event.ContinueCombo.End"), false);
	const FGameplayTag HitScanStartTag = FGameplayTag::RequestGameplayTag(TEXT("Event.HitScan.Start"), false);
	const FGameplayTag HitScanEndTag = FGameplayTag::RequestGameplayTag(TEXT("Event.HitScan.End"), false);

	if (EventTag.MatchesTagExact(ComboStartTag) ||
		EventTag.MatchesTagExact(ComboInputTag) ||
		EventTag.MatchesTagExact(ComboEndTag) ||
		EventTag.MatchesTagExact(HitScanStartTag) ||
		EventTag.MatchesTagExact(HitScanEndTag))
	{
		return 1.0f;
	}

	return FMath::Max(0.f, ServerGameplayEventDefaultMaxAbsMagnitude);
}

void ASpellRiseCharacterBase::OnDeadTagChanged(FGameplayTag CallbackTag, int32 NewCount)
{
	if (!GetSpellRiseASC() || GetSpellRiseASC()->GetAvatarActor() != this)
	{
		return;
	}

	if (NewCount > 0)
	{
		if (!IsDead())
		{
			bIsDead = true;
		}

		if (HasAuthority())
		{
			StopResourceRegen_Server();
			MultiHandleDeath();
			ProcessFullLootDrop_Server(GetActorLocation());
			ScheduleCorpseDespawn_Server();
			ScheduleRespawn_Server();
		}
		else
		{
			HandleDeath();
		}

		SyncDeadStateFromASC(TEXT("OnDeadTagChanged"));
		RefreshRuntimeTickPolicy();
		return;
	}

	bIsDead = false;
	SyncDeadStateFromASC(TEXT("OnDeadTagChanged"));
	bFullLootProcessedForCurrentDeath = false;
	CombatLockExpireAtServerTimeSeconds = -1.0;
	ResetLocalDeathPresentation();
	SetCharacterInputEnabled(true);
	RefreshRuntimeTickPolicy();
	if (HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(RespawnTimerHandle);
			World->GetTimerManager().ClearTimer(CorpseDespawnTimerHandle);
		}
		ApplyRegenStartupEffects();
		StartResourceRegen_Server();
	}
}

void ASpellRiseCharacterBase::ScheduleCorpseDespawn_Server()
{
	if (!HasAuthority())
	{
		return;
	}

	if (CorpseDespawnDelaySeconds < 0.f)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (World->GetTimerManager().IsTimerActive(CorpseDespawnTimerHandle))
	{
		return;
	}

	World->GetTimerManager().SetTimer(
		CorpseDespawnTimerHandle,
		this,
		&ASpellRiseCharacterBase::ExecuteCorpseDespawn_Server,
		FMath::Max(20.f, CorpseDespawnDelaySeconds),
		false);
}

void ASpellRiseCharacterBase::ExecuteCorpseDespawn_Server()
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	USkeletalMeshComponent* VisualMesh = GetVisualMeshComponent();
	FVector CorpseLocation = GetActorLocation();
	if (VisualMesh)
	{
		CorpseLocation = VisualMesh->Bounds.Origin;
	}

	if (!bFullLootProcessedForCurrentDeath)
	{
		ProcessFullLootDrop_Server(CorpseLocation);
	}

	if (VisualMesh)
	{
		VisualMesh->SetSimulatePhysics(false);
		VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		VisualMesh->SetVisibility(false, true);
	}

	MultiHandleCorpseDespawn();
}

void ASpellRiseCharacterBase::ProcessFullLootDrop_Server(const FVector& LootOrigin)
{
	if (!HasAuthority() || bFullLootProcessedForCurrentDeath)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (USpellRiseFullLootSubsystem* FullLootSubsystem = World->GetSubsystem<USpellRiseFullLootSubsystem>())
	{
		FullLootSubsystem->HandleCharacterCorpseDespawn(this, FullLootBagClass, LootOrigin);
		bFullLootProcessedForCurrentDeath = true;
	}
}

void ASpellRiseCharacterBase::ScheduleRespawn_Server()
{
	if (!HasAuthority())
	{
		return;
	}

	if (RespawnDelaySeconds < 0.f)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (World->GetTimerManager().IsTimerActive(RespawnTimerHandle))
	{
		return;
	}

	const float BaseDelaySeconds = FMath::Max(0.f, RespawnDelaySeconds);
	float EffectiveDelaySeconds = BaseDelaySeconds;
	double CombatLockRemainingSeconds = 0.0;
	if (IsCombatLockActive_Server(&CombatLockRemainingSeconds))
	{
		EffectiveDelaySeconds = FMath::Max(EffectiveDelaySeconds, static_cast<float>(CombatLockRemainingSeconds));
	}
	else
	{
		CombatLockExpireAtServerTimeSeconds = -1.0;
	}

	World->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ASpellRiseCharacterBase::ExecuteRespawn_Server, EffectiveDelaySeconds, false);
}

void ASpellRiseCharacterBase::ExecuteRespawn_Server()
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	AController* ControllerToRespawn = GetController();
	if (!ControllerToRespawn)
	{
		if (APlayerState* PS = GetPlayerState())
		{
			ControllerToRespawn = Cast<AController>(PS->GetOwner());
		}
	}

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
	if (!GameMode || !ControllerToRespawn)
	{
		return;
	}

	StopAllCharacterAudio(true);
	ControllerToRespawn->UnPossess();
	Destroy();
	GameMode->RestartPlayer(ControllerToRespawn);
}

void ASpellRiseCharacterBase::RefreshCombatLockFromDamage_Server(float DamageDelta)
{
	if (!HasAuthority() || DamageDelta <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const double LockDurationSeconds = static_cast<double>(FMath::Max(0.f, CombatLockDurationSeconds));
	if (LockDurationSeconds <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const double NowSeconds = static_cast<double>(World->GetTimeSeconds());
	CombatLockExpireAtServerTimeSeconds = FMath::Max(CombatLockExpireAtServerTimeSeconds, NowSeconds + LockDurationSeconds);
}

bool ASpellRiseCharacterBase::IsCombatLockActive_Server(double* OutSecondsRemaining) const
{
	if (OutSecondsRemaining)
	{
		*OutSecondsRemaining = 0.0;
	}

	if (!HasAuthority())
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World || CombatLockExpireAtServerTimeSeconds <= 0.0)
	{
		return false;
	}

	const double RemainingSeconds = CombatLockExpireAtServerTimeSeconds - static_cast<double>(World->GetTimeSeconds());
	if (RemainingSeconds <= 0.0)
	{
		return false;
	}

	if (OutSecondsRemaining)
	{
		*OutSecondsRemaining = RemainingSeconds;
	}

	return true;
}

void ASpellRiseCharacterBase::StopAllCharacterAudio(bool bIncludeAttachedActors)
{
	TArray<UAudioComponent*> AudioComponents;
	GetComponents<UAudioComponent>(AudioComponents);

	for (UAudioComponent* AudioComp : AudioComponents)
	{
		if (!AudioComp)
		{
			continue;
		}

		AudioComp->Stop();
	}

	if (!bIncludeAttachedActors)
	{
		return;
	}

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for (AActor* AttachedActor : AttachedActors)
	{
		if (!AttachedActor)
		{
			continue;
		}

		TArray<UAudioComponent*> AttachedAudioComponents;
		AttachedActor->GetComponents<UAudioComponent>(AttachedAudioComponents);
		for (UAudioComponent* AudioComp : AttachedAudioComponents)
		{
			if (!AudioComp)
			{
				continue;
			}

			AudioComp->Stop();
		}
	}
}

void ASpellRiseCharacterBase::TriggerLocalDamageScreenEffect()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!IsLocallyControlled() || !bEnableLocalDamageScreenEffect)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !PC->PlayerCameraManager)
	{
		return;
	}

	const float ClampedAlpha = FMath::Clamp(DamageScreenEffectAlpha, 0.f, 1.f);
	if (ClampedAlpha <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	if (!LocalDamageEdgeWidget)
	{
		LocalDamageEdgeWidget = CreateWidget<USpellRiseDamageEdgeWidget>(PC, USpellRiseDamageEdgeWidget::StaticClass());
		if (LocalDamageEdgeWidget)
		{
			LocalDamageEdgeWidget->AddToViewport(1000);
		}
	}

	if (LocalDamageEdgeWidget)
	{
		LocalDamageEdgeWidget->PlayDamageFlash(
			ClampedAlpha,
			FMath::Max(0.f, DamageScreenEffectFadeInDuration),
			FMath::Max(0.f, DamageScreenEffectFadeOutDuration));
		return;
	}

	PC->PlayerCameraManager->StartCameraFade(
		0.f,
		ClampedAlpha * 0.15f,
		FMath::Max(0.f, DamageScreenEffectFadeInDuration),
		FLinearColor(0.75f, 0.0f, 0.0f, 1.0f),
		false,
		true);
}

void ASpellRiseCharacterBase::ShowLocalDeathScreenText()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!IsLocallyControlled())
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	if (!LocalDeathScreenWidget)
	{
		LocalDeathScreenWidget = CreateWidget<USpellRiseDeathScreenWidget>(PC, USpellRiseDeathScreenWidget::StaticClass());
	}

	if (LocalDeathScreenWidget)
	{
		LocalDeathScreenWidget->SetMessage(FText::FromString(TEXT("You died.")));
		if (!LocalDeathScreenWidget->IsInViewport())
		{
			LocalDeathScreenWidget->AddToViewport(2000);
		}
		LocalDeathScreenWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void ASpellRiseCharacterBase::HideLocalDeathScreenText()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LocalDeathScreenTimerHandle);
		World->GetTimerManager().ClearTimer(LocalDeathScreenHideTimerHandle);
	}

	if (LocalDeathScreenWidget)
	{
		LocalDeathScreenWidget->RemoveFromParent();
		LocalDeathScreenWidget = nullptr;
	}
}

void ASpellRiseCharacterBase::ResetLocalDeathPresentation()
{
	HideLocalDeathScreenText();

	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	if (!IsLocallyControlled())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	APlayerController* LocalPC = Cast<APlayerController>(GetController());
	if (!IsValid(LocalPC))
	{
		return;
	}

	TArray<UUserWidget*> ExistingDeathWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
		this,
		ExistingDeathWidgets,
		USpellRiseDeathScreenWidget::StaticClass(),
		false);

	for (UUserWidget* ExistingWidget : ExistingDeathWidgets)
	{
		if (ExistingWidget)
		{
			ExistingWidget->RemoveFromParent();
		}
	}

	if (!bEnableLocalDeathCameraEffect)
	{
		return;
	}

	if (APlayerCameraManager* CameraManager = LocalPC->PlayerCameraManager)
	{
		CameraManager->StartCameraFade(
			1.f,
			0.f,
			FMath::Max(0.f, DeathCameraFadeInDurationOnSpawn),
			FLinearColor::Black,
			false,
			true);
	}
}

void ASpellRiseCharacterBase::MultiHandleDeath_Implementation()
{
	HandleDeath();
}

void ASpellRiseCharacterBase::MultiHandleCorpseDespawn_Implementation()
{
	if (USkeletalMeshComponent* VisualMesh = GetVisualMeshComponent())
	{
		VisualMesh->SetSimulatePhysics(false);
		VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		VisualMesh->SetVisibility(false, true);
	}
}

void ASpellRiseCharacterBase::HandleDeath_Implementation()
{
	USkeletalMeshComponent* VisualMesh = GetVisualMeshComponent();
	if (!VisualMesh || !GetCapsuleComponent() || !GetCharacterMovement())
	{
		return;
	}

	const FVector PreDeathVelocity = GetCharacterMovement()->Velocity;

	StopAllCharacterAudio(true);
	SetCharacterInputEnabled(false);

	if (VisualMesh->IsSimulatingPhysics())
	{
		return;
	}

	if (HasAuthority())
	{
		RemoveRuntimeGrantedAbilitiesOnDeath_Server();
		DestroyDeathAttachments_Server(VisualMesh);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	ConfigureDeathRagdoll(VisualMesh, PreDeathVelocity);

	if (GetNetMode() != NM_DedicatedServer && bEnableLocalDeathCameraEffect && IsLocallyControlled())
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FRotator ControlRotation = PC->GetControlRotation();
			ControlRotation.Pitch = DeathCameraPitchDegrees;
			PC->SetControlRotation(ControlRotation);

			if (APlayerCameraManager* CameraManager = PC->PlayerCameraManager)
			{
				CameraManager->StartCameraFade(0.f, 1.f, FMath::Max(0.f, DeathCameraFadeOutDuration), FLinearColor::Black, false, true);
			}
		}
	}

	if (GetNetMode() != NM_DedicatedServer && IsLocallyControlled())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(LocalDeathScreenTimerHandle);
			World->GetTimerManager().ClearTimer(LocalDeathScreenHideTimerHandle);
			const float DeathTextDelay = bEnableLocalDeathCameraEffect ? FMath::Max(0.f, DeathCameraFadeOutDuration) : 0.f;
			World->GetTimerManager().SetTimer(LocalDeathScreenTimerHandle, this, &ASpellRiseCharacterBase::ShowLocalDeathScreenText, DeathTextDelay, false);

			const float HideDelay = FMath::Max(0.f, RespawnDelaySeconds - FMath::Max(0.f, DeathMessageHideLeadTimeSeconds));
			World->GetTimerManager().SetTimer(LocalDeathScreenHideTimerHandle, this, &ASpellRiseCharacterBase::HideLocalDeathScreenText, HideDelay, false);
		}
	}
}

void ASpellRiseCharacterBase::ConfigureDeathRagdoll(USkeletalMeshComponent* VisualMesh, const FVector& PreDeathVelocity)
{
	if (!VisualMesh)
	{
		return;
	}

	if (UAnimInstance* AnimInstance = VisualMesh->GetAnimInstance())
	{
		AnimInstance->StopAllMontages(0.08f);
	}

	VisualMesh->SetEnableGravity(true);

	if (!DeathRagdollCollisionProfileName.IsNone())
	{
		VisualMesh->SetCollisionProfileName(DeathRagdollCollisionProfileName, true);
	}
	else
	{
		VisualMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	VisualMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	VisualMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	VisualMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	VisualMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	VisualMesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	VisualMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	VisualMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	VisualMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	VisualMesh->SetGenerateOverlapEvents(false);
	VisualMesh->SetNotifyRigidBodyCollision(false);
	VisualMesh->bBlendPhysics = true;
	VisualMesh->SetAllBodiesSimulatePhysics(true);
	VisualMesh->SetSimulatePhysics(true);
	VisualMesh->SetAllBodiesPhysicsBlendWeight(1.0f);
	VisualMesh->WakeAllRigidBodies();

	const FVector Impulse = BuildDeathRagdollImpulse(PreDeathVelocity);
	if (Impulse.IsNearlyZero())
	{
		return;
	}

	FVector ImpulseLocation = VisualMesh->Bounds.Origin;
	if (!DeathRagdollImpulseBoneName.IsNone() && VisualMesh->GetBoneIndex(DeathRagdollImpulseBoneName) != INDEX_NONE)
	{
		ImpulseLocation = VisualMesh->GetBoneLocation(DeathRagdollImpulseBoneName);
	}

	VisualMesh->AddImpulseAtLocation(Impulse, ImpulseLocation);
}

FVector ASpellRiseCharacterBase::BuildDeathRagdollImpulse(const FVector& PreDeathVelocity) const
{
	FVector Impulse = (-GetActorForwardVector() * FMath::Max(0.f, DeathRagdollBackwardImpulse))
		+ (FVector::UpVector * FMath::Max(0.f, DeathRagdollUpwardImpulse))
		+ (PreDeathVelocity * FMath::Max(0.f, DeathRagdollInheritedVelocityScale));

	const float MaxImpulse = FMath::Max(0.f, DeathRagdollMaxImpulse);
	if (MaxImpulse > KINDA_SMALL_NUMBER)
	{
		Impulse = Impulse.GetClampedToMaxSize(MaxImpulse);
	}

	return Impulse;
}

void ASpellRiseCharacterBase::RemoveRuntimeGrantedAbilitiesOnDeath_Server()
{
	if (!HasAuthority() || !GetSpellRiseASC())
	{
		return;
	}

	const bool bHasPersistentTalentGrants = HasGrantedTalentStateForDeathCleanup(this);
	TArray<FGameplayAbilitySpecHandle> HandlesToRemove;
	for (const FGameplayAbilitySpec& Spec : GetSpellRiseASC()->GetActivatableAbilities())
	{
		if (!Spec.Handle.IsValid())
		{
			continue;
		}

		if (ASpellRisePlayerState* SRPlayerState = GetPlayerState<ASpellRisePlayerState>())
		{
			if (Spec.SourceObject.Get() == SRPlayerState)
			{
				continue;
			}
		}

		if (Spec.SourceObject.Get() != this)
		{
			continue;
		}

		if (bHasPersistentTalentGrants)
		{
			UE_LOG(LogSpellRiseCharacterRuntime, Verbose,
				TEXT("[Progression][DeathAbilityCleanupSkipped] Reason=preserve_talent_grants Character=%s Ability=%s"),
				*GetNameSafe(this),
				*GetNameSafe(Spec.Ability));
			continue;
		}

		HandlesToRemove.Add(Spec.Handle);
	}

	if (HandlesToRemove.IsEmpty())
	{
		return;
	}

	GetSpellRiseASC()->SR_ClearAbilityInput();
	GetSpellRiseASC()->SR_ClearSelectedSpellAbility();

	for (const FGameplayAbilitySpecHandle& AbilityHandle : HandlesToRemove)
	{
		GetSpellRiseASC()->ClearAbility(AbilityHandle);
	}

	GetSpellRiseASC()->RefreshAbilityActorInfo();
	if (ASpellRisePlayerState* SRPlayerState = GetPlayerState<ASpellRisePlayerState>())
	{
		SRPlayerState->SendAbilitiesChangedEvent();
	}

}

void ASpellRiseCharacterBase::DestroyDeathAttachments_Server(USkeletalMeshComponent* VisualMesh)
{
	if (!HasAuthority())
	{
		return;
	}

	TArray<UChildActorComponent*> ChildActorComps;
	GetComponents<UChildActorComponent>(ChildActorComps);
	for (UChildActorComponent* ChildActorComp : ChildActorComps)
	{
		if (!ChildActorComp)
		{
			continue;
		}

		if (AActor* ChildActor = ChildActorComp->GetChildActor())
		{
			ChildActor->Destroy();
		}
		ChildActorComp->DestroyChildActor();
	}

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors, true, true);
	for (AActor* AttachedActor : AttachedActors)
	{
		if (!AttachedActor || AttachedActor == this)
		{
			continue;
		}

		AttachedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		AttachedActor->Destroy();
	}

	if (VisualMesh)
	{
		TArray<UMeshComponent*> MeshComponents;
		GetComponents<UMeshComponent>(MeshComponents);
		for (UMeshComponent* MeshComponent : MeshComponents)
		{
			if (!MeshComponent || MeshComponent == VisualMesh)
			{
				continue;
			}

			if (USceneComponent* SceneComponent = Cast<USceneComponent>(MeshComponent))
			{
				if (SceneComponent->IsAttachedTo(VisualMesh))
				{
					MeshComponent->DestroyComponent();
				}
			}
		}
	}
}

void ASpellRiseCharacterBase::MultiOnCatalystProc_Implementation(int32 CatalystTier)
{
	BP_OnCatalystProc(CatalystTier);
}

void ASpellRiseCharacterBase::MultiShowDamagePop_Implementation(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit)
{
	if (Damage <= 0.f)
	{
		return;
	}

	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	ASpellRisePlayerController* PC = Cast<ASpellRisePlayerController>(LocalPlayerController);
	if (!PC)
	{
		return;
	}

	FGameplayTagContainer SourceTags;
	FGameplayTagContainer TargetTags;

	if (DamageTypeTag.IsValid())
	{
		TargetTags.AddTag(DamageTypeTag);
	}

	const FVector PopLocation = GetDamageNumberWorldLocation();

	PC->ShowDamageNumber(
		Damage,
		PopLocation,
		SourceTags,
		TargetTags,
		bIsCrit
	);

	BP_ShowDamagePop(Damage, InstigatorActor, DamageTypeTag, bIsCrit);

}

void ASpellRiseCharacterBase::MultiPlayHitReactionMontage_Implementation(float PlayRate)
{
	if (IsDead() || !HitReactionMontage)
	{
		return;
	}

	USkeletalMeshComponent* VisualMesh = GetVisualMeshComponent();
	UAnimInstance* AnimInstance = VisualMesh ? VisualMesh->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		return;
	}

	if (AnimInstance->Montage_IsPlaying(HitReactionMontage))
	{
		return;
	}

	AnimInstance->Montage_Play(HitReactionMontage, FMath::Max(0.1f, PlayRate));
}

void ASpellRiseCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (!FallDamageComponent)
	{
		return;
	}

	FallDamageComponent->OnMovementModeChanged();
}

void ASpellRiseCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (!FallDamageComponent)
	{
		return;
	}

	FallDamageComponent->OnLanded(Hit);
}
