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
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "GameplayEffect.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationMarkerComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "SpellRise/Components/CatalystComponent.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h"
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

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseDeathLoot, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseSecurity, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseCharacterRuntime, Log, All);

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

		for (int32 SlotIndex = 0; SlotIndex < 8; ++SlotIndex)
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

	CatalystComponent = CreateDefaultSubobject<UCatalystComponent>(TEXT("CatalystComponent"));
	if (CatalystComponent)
	{
		CatalystComponent->bCatalystEnabled = bEnableCatalyst;
	}

	FallDamageComponent = CreateDefaultSubobject<UFallDamageComponent>(TEXT("FallDamageComponent"));
	EquipmentManager = CreateDefaultSubobject<USpellRiseEquipmentManagerComponent>(TEXT("EquipmentManager"));

	DeadStateTag = SpellRiseTags::State_Dead();

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

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();
}

void ASpellRiseCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	ResetLocalDeathPresentation();
	SyncDeadStateFromASC(TEXT("BeginPlay"));

	if (CatalystComponent)
	{
		CatalystComponent->bCatalystEnabled = bEnableCatalyst;
	}

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();

	InitASCActorInfo();
	BindASCDelegates();
	SetCharacterInputEnabled(!IsDead());

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

	if (HasAuthority())
	{
		USkeletalMeshComponent* VisualMesh = GetVisualMeshComponent();
		UCameraComponent* AimCamera = GetActiveAimCameraComponent();

		UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[Anim][Server] Char=%s Mesh=%s AnimClass=%s AnimInstance=%s TickOption=%d AimCamera=%s"),
			*GetNameSafe(this),
			*GetNameSafe(VisualMesh),
			VisualMesh ? *GetNameSafe(VisualMesh->AnimClass) : TEXT("NULL"),
			VisualMesh ? *GetNameSafe(VisualMesh->GetAnimInstance()) : TEXT("NULL"),
			VisualMesh ? static_cast<int32>(VisualMesh->VisibilityBasedAnimTickOption) : -1,
			*GetNameSafe(AimCamera));
	}
}

void ASpellRiseCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ASCInitializationRetryTimerHandle);
	}
	ResetLocalDeathPresentation();
	Super::EndPlay(EndPlayReason);
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

	if (const ASpellRisePlayerController* SRPC = Cast<ASpellRisePlayerController>(GetController()))
	{
		if (SRPC->IsConstructionModeActive())
		{
			if (GetSpellRiseASC())
			{
				GetSpellRiseASC()->SR_ClearAbilityInput();
			}
			return;
		}
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

	// Ability input bindings now live in PlayerController.
	// Character keeps movement/locomotion responsibilities.
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

		MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		MeshComp->bEnableUpdateRateOptimizations = false;
		MeshComp->SetComponentTickEnabled(true);
	}
}

void ASpellRiseCharacterBase::EnsureAnimInstanceInitialized()
{
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
	if (USkeletalMeshComponent* VisualMesh = FindCharacterSkeletalMeshComponentByName(VisualMeshComponentName))
	{
		return VisualMesh;
	}

	return GetMesh();
}

USkeletalMeshComponent* ASpellRiseCharacterBase::GetEquipmentAttachMeshComponent() const
{
	if (USkeletalMeshComponent* AttachMesh = FindCharacterSkeletalMeshComponentByName(EquipmentAttachMeshComponentName))
	{
		return AttachMesh;
	}

	return GetVisualMeshComponent();
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

void ASpellRiseCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ASpellRiseCharacterBase, SelectedAbilityInputTag, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ASpellRiseCharacterBase, Archetype, COND_None, REPNOTIFY_Always);
}

void ASpellRiseCharacterBase::MultiRefreshEquipmentVisuals_Implementation()
{
	UE_LOG(
		LogSpellRiseCharacterRuntime,
		Warning,
		TEXT("[Equipment][VisualRefreshDeprecated] Character=%s Local=%d Authority=%d"),
		*GetNameSafe(this),
		IsLocallyControlled() ? 1 : 0,
		HasAuthority() ? 1 : 0);
}

void ASpellRiseCharacterBase::ServerHandleNarrativeItemActivationForEquipment_Implementation(UObject* ItemObject, bool bShouldEquip)
{
	if (!HasAuthority())
	{
		return;
	}

	UEquippableItem* EquippableItem = Cast<UEquippableItem>(ItemObject);
	if (!EquippableItem || !EquipmentManager)
	{
		return;
	}

	if (EquippableItem->GetOwningPawn() != this)
	{
		UE_LOG(
			LogSpellRiseCharacterRuntime,
			Warning,
			TEXT("[Equipment][EquipRejected] Character=%s Item=%s Reason=item_not_owned_by_character"),
			*GetNameSafe(this),
			*GetNameSafe(EquippableItem));
		return;
	}

	if (bShouldEquip)
	{
		EquipmentManager->RequestEquipItem(EquippableItem);
	}
	else
	{
		EquipmentManager->RequestUnequipItem(EquippableItem);
	}
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
		UE_LOG(
			LogSpellRiseCharacterRuntime,
			Verbose,
			TEXT("[RPC][ClientGuard] SetArchetype skipped reason=%s Char=%s Controller=%s Owner=%s"),
			*RejectReason,
			*GetNameSafe(this),
			*GetNameSafe(GetController()),
			*GetNameSafe(GetOwner()));
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
	LogDerivedDebug();
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

	if (SlotIndex >= 0 && SlotIndex < 8)
	{
		NewTag = SpellRiseTags::Input_AbilitySlot(SlotIndex);
	}

	if (!HasAuthority())
	{
		FString RejectReason;
		if (!CanIssueOwnerServerRpc(RejectReason))
		{
			UE_LOG(
				LogSpellRiseCharacterRuntime,
				Verbose,
				TEXT("[RPC][ClientGuard] SelectAbilitySlot skipped reason=%s Char=%s Controller=%s Owner=%s Slot=%d"),
				*RejectReason,
				*GetNameSafe(this),
				*GetNameSafe(GetController()),
				*GetNameSafe(GetOwner()),
				SlotIndex);
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

	if (SlotIndex < 0 || SlotIndex >= 8)
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

	if (SlotIndex < 0 || SlotIndex >= 8)
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
			UE_LOG(
				LogSpellRiseCharacterRuntime,
				Verbose,
				TEXT("[RPC][ClientGuard] ClearSelectedAbility skipped reason=%s Char=%s Controller=%s Owner=%s"),
				*RejectReason,
				*GetNameSafe(this),
				*GetNameSafe(GetController()),
				*GetNameSafe(GetOwner()));
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
		UE_LOG(
			LogSpellRiseCharacterRuntime,
			Verbose,
			TEXT("[GAS][DeadStateSync] Char=%s Context=%s MirrorOld=%d MirrorNew=%d"),
			*GetNameSafe(this),
			Context ? Context : TEXT("Unknown"),
			bOldMirror ? 1 : 0,
			bIsDead ? 1 : 0);
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
	if (GetSpellRiseASC())
	{
		GetSpellRiseASC()->SR_ProcessAbilityInput(DeltaSeconds, bGamePaused);
	}
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

	if (!InitializeAbilitySystemFromPlayerState())
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[GAS] InitASCActorInfo falhou em %s: PlayerState/ASC ainda nao disponivel."), *GetNameSafe(this));
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

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();

	InitASCActorInfo();
	BindASCDelegates();
	SyncDeadStateFromASC(TEXT("PossessedBy"));
	ResetLocalDeathPresentation();
	SetCharacterInputEnabled(!IsDead());

	if (!GetSpellRiseASC())
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[GAS] PossessedBy sem ASC cacheado em %s (OwnerController=%s)."), *GetNameSafe(this), *GetNameSafe(NewController));
		return;
	}

	if (!HasAuthority())
	{
		return;
	}

	ResetDeathStateAndResources_Server();
	ApplyArchetypeToPrimaries_Server();

	static const FName StartupGrantedTagName(TEXT("SpellRise.StartupAbilitiesGranted"));
	const bool bAlreadyGranted = Tags.Contains(StartupGrantedTagName);

	if (!bAlreadyGranted)
	{
		const TArray<FGameplayAbilitySpecHandle> GrantedHandles = GrantAbilities(StartingAbilities);
		for (const FGameplayAbilitySpecHandle& Handle : GrantedHandles)
		{
			if (Handle.IsValid())
			{
				StartupGrantedAbilityHandles.AddUnique(Handle);
			}
		}
		Tags.AddUnique(StartupGrantedTagName);
	}
	else
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[GAS] Startup abilities already granted for %s (skipping)"), *GetNameSafe(this));
	}

	ApplyRegenStartupEffects();
	ApplyStartupEffects();
}

void ASpellRiseCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();

	InitASCActorInfo();
	BindASCDelegates();
	SyncDeadStateFromASC(TEXT("OnRep_PlayerState"));
	ResetLocalDeathPresentation();
	SetCharacterInputEnabled(!IsDead());

	if (!GetSpellRiseASC())
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[GAS] OnRep_PlayerState sem ASC cacheado em %s."), *GetNameSafe(this));
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

void ASpellRiseCharacterBase::OnPrimaryChanged(const FOnAttributeChangeData& Data)
{
	if (!HasAuthority() || !GetSpellRiseASC())
	{
		return;
	}

	RecalculateDerivedStats();
	ApplyDerivedStatsInfinite();
	LogDerivedDebug();
}

void ASpellRiseCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!GetSpellRiseASC() || GetSpellRiseASC()->GetAvatarActor() != this)
	{
		UE_LOG(
			LogSpellRiseCharacterRuntime,
			Warning,
			TEXT("[FullLoot][Death] OnHealthChanged ignorado por AvatarContext mismatch. Char=%s ASC=%s Avatar=%s"),
			*GetNameSafe(this),
			*GetNameSafe(GetSpellRiseASC()),
			*GetNameSafe(GetSpellRiseASC() ? GetSpellRiseASC()->GetAvatarActor() : nullptr));
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
		UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Death] OnHealthChanged ignorado (ja morto). Char=%s"), *GetNameSafe(this));
		return;
	}

	const float NewHealth = Data.NewValue;
	if (NewHealth > 0.f)
	{
		return;
	}

	if (!HasAuthority() || !GetSpellRiseASC())
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Death] OnHealthChanged sem authority/ASC. Char=%s Health=%.2f"), *GetNameSafe(this), NewHealth);
		return;
	}

	UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Death] OnHealthChanged matou personagem. Char=%s Health=%.2f LootProcessed=%d"), *GetNameSafe(this), NewHealth, bFullLootProcessedForCurrentDeath ? 1 : 0);
	bIsDead = true;

	if (!bFullLootProcessedForCurrentDeath)
	{
		if (UWorld* World = GetWorld())
		{
			if (USpellRiseFullLootSubsystem* FullLootSubsystem = World->GetSubsystem<USpellRiseFullLootSubsystem>())
			{
				UE_LOG(LogSpellRiseDeathLoot, Log, TEXT("[DeathLoot] Disparando FullLoot via OnHealthChanged. Char=%s"), *GetNameSafe(this));
				UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Death] Disparando FullLoot via OnHealthChanged. Char=%s"), *GetNameSafe(this));
				FullLootSubsystem->HandleCharacterDeath(this, FullLootBagClass);
				bFullLootProcessedForCurrentDeath = true;
			}
			else
			{
				UE_LOG(LogSpellRiseCharacterRuntime, Error, TEXT("[FullLoot][Death] Subsystem FullLoot ausente em OnHealthChanged. Char=%s"), *GetNameSafe(this));
			}
		}
		else
		{
			UE_LOG(LogSpellRiseCharacterRuntime, Error, TEXT("[FullLoot][Death] World nulo em OnHealthChanged. Char=%s"), *GetNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Death] FullLoot ja processado em OnHealthChanged. Char=%s"), *GetNameSafe(this));
	}

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
	SyncDeadStateFromASC(TEXT("OnHealthChanged"));
	ScheduleRespawn_Server();
}

void ASpellRiseCharacterBase::ApplyStartupEffects()
{
	if (!GetSpellRiseASC() || !HasAuthority())
	{
		return;
	}

	if (!GE_RecalculateResources)
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[GAS] GE_RecalculateResources is not set on %s"), *GetName());
		return;
	}

	RecalculateDerivedStats();
	ApplyDerivedStatsInfinite();
	LogDerivedDebug();
}

void ASpellRiseCharacterBase::ApplyDerivedStatsInfinite()
{
	if (!GetSpellRiseASC() || !HasAuthority())
	{
		return;
	}

	if (!GE_DerivedStatsInfinite)
	{
		UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[DERIVED] GE_DerivedStatsInfinite is NULL on %s."), *GetNameSafe(this));
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
}

void ASpellRiseCharacterBase::LogDerivedDebug()
{
	if (!GetSpellRiseASC())
	{
		return;
	}

	const bool bTagActive = GetSpellRiseASC()->HasMatchingGameplayTag(SpellRiseTags::State_DerivedStats_Active());

	UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[DERIVED] GE=%s TagActive=%d"),
		*GetNameSafe(GE_DerivedStatsInfinite),
		bTagActive ? 1 : 0);

	const float STR = GetSpellRiseASC()->GetNumericAttribute(UCombatAttributeSet::GetStrengthAttribute());
	const float AGI = GetSpellRiseASC()->GetNumericAttribute(UCombatAttributeSet::GetAgilityAttribute());
	const float INT = GetSpellRiseASC()->GetNumericAttribute(UCombatAttributeSet::GetIntelligenceAttribute());
	const float WIS = GetSpellRiseASC()->GetNumericAttribute(UCombatAttributeSet::GetWisdomAttribute());

	UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[PRIM] STR=%.1f AGI=%.1f INT=%.1f WIS=%.1f"), STR, AGI, INT, WIS);

	const float MeleeMult = GetSpellRiseASC()->GetNumericAttribute(UDerivedStatsAttributeSet::GetMeleeDamageMultiplierAttribute());
	const float BowMult = GetSpellRiseASC()->GetNumericAttribute(UDerivedStatsAttributeSet::GetBowDamageMultiplierAttribute());
	const float SpellMult = GetSpellRiseASC()->GetNumericAttribute(UDerivedStatsAttributeSet::GetSpellDamageMultiplierAttribute());
	const float HealMult = GetSpellRiseASC()->GetNumericAttribute(UDerivedStatsAttributeSet::GetHealingMultiplierAttribute());
	const float CTR = GetSpellRiseASC()->GetNumericAttribute(UDerivedStatsAttributeSet::GetCastTimeReductionAttribute());
	const float CC = GetSpellRiseASC()->GetNumericAttribute(UDerivedStatsAttributeSet::GetCritChanceAttribute());
	const float CD = GetSpellRiseASC()->GetNumericAttribute(UDerivedStatsAttributeSet::GetCritDamageAttribute());
	const float AP = GetSpellRiseASC()->GetNumericAttribute(UDerivedStatsAttributeSet::GetArmorPenetrationAttribute());
	const float MCR = GetSpellRiseASC()->GetNumericAttribute(UDerivedStatsAttributeSet::GetManaCostReductionAttribute());

	UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[DER]  Melee=%.3f Bow=%.3f Spell=%.3f Heal=%.3f CTR=%.3f CC=%.3f CD=%.3f AP=%.3f MCR=%.3f"),
		MeleeMult, BowMult, SpellMult, HealMult, CTR, CC, CD, AP, MCR);
}

void ASpellRiseCharacterBase::ApplyOrRefreshEffect(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!GetSpellRiseASC() || !HasAuthority() || !EffectClass)
	{
		return;
	}

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
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	if (bEnabled)
	{
		ResetLocalDeathPresentation();
		if (ASpellRisePlayerController* SRPC = Cast<ASpellRisePlayerController>(PC))
		{
			SRPC->SetCombatHUDSuppressedByDeath(false);
		}
		EnableInput(PC);
		PC->SetIgnoreMoveInput(false);
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
	if (ASpellRisePlayerController* SRPC = Cast<ASpellRisePlayerController>(PC))
	{
		SRPC->SetCombatHUDSuppressedByDeath(true);
	}
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
		GetSpellRiseASC()->RemoveLooseGameplayTag(DeadStateTag);

		FGameplayTagContainer DeadTags;
		DeadTags.AddTag(DeadStateTag);
		GetSpellRiseASC()->RemoveActiveEffectsWithGrantedTags(DeadTags);
	}

	if (GE_Death)
	{
		GetSpellRiseASC()->RemoveActiveGameplayEffectBySourceEffect(GE_Death, GetSpellRiseASC());
	}

	bIsDead = false;
	bFullLootProcessedForCurrentDeath = false;
	SyncDeadStateFromASC(TEXT("ResetDeathStateAndResources_Server"));

	const float MaxHealthValue = GetSpellRiseASC()->GetNumericAttribute(UResourceAttributeSet::GetMaxHealthAttribute());
	const float MaxManaValue = GetSpellRiseASC()->GetNumericAttribute(UResourceAttributeSet::GetMaxManaAttribute());
	const float MaxStaminaValue = GetSpellRiseASC()->GetNumericAttribute(UResourceAttributeSet::GetMaxStaminaAttribute());

	GetSpellRiseASC()->SetNumericAttributeBase(UResourceAttributeSet::GetHealthAttribute(), FMath::Max(1.f, MaxHealthValue));
	GetSpellRiseASC()->SetNumericAttributeBase(UResourceAttributeSet::GetManaAttribute(), FMath::Max(0.f, MaxManaValue));
	GetSpellRiseASC()->SetNumericAttributeBase(UResourceAttributeSet::GetStaminaAttribute(), FMath::Max(0.f, MaxStaminaValue));

	UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Respawn] Reset de morte aplicado. Char=%s DeadTag=%s Health=%.1f/%.1f"),
		*GetNameSafe(this),
		DeadStateTag.IsValid() ? TEXT("valid") : TEXT("invalid"),
		GetSpellRiseASC()->GetNumericAttribute(UResourceAttributeSet::GetHealthAttribute()),
		GetSpellRiseASC()->GetNumericAttribute(UResourceAttributeSet::GetMaxHealthAttribute()));
}

TArray<FGameplayAbilitySpecHandle> ASpellRiseCharacterBase::GrantAbilities(const TArray<FSpellRiseGrantedAbility>& AbilitiesToGrant)
{
	if (!GetSpellRiseASC() || !HasAuthority())
	{
		return {};
	}

	TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	AbilityHandles.Reserve(AbilitiesToGrant.Num());

	for (const FSpellRiseGrantedAbility& Grant : AbilitiesToGrant)
	{
		if (!Grant.Ability)
		{
			continue;
		}

		if (GetSpellRiseASC()->FindAbilitySpecFromClass(Grant.Ability) != nullptr)
		{
			UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[GAS] Ability ja concedida no ASC. Pulando duplicata: %s"), *GetNameSafe(Grant.Ability));
			continue;
		}

		const int32 FinalLevel = FMath::Max(1, Grant.AbilityLevel);
		FGameplayAbilitySpec Spec(Grant.Ability, FinalLevel, INDEX_NONE, this);

		if (Grant.InputTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(Grant.InputTag);
		}

		const FGameplayAbilitySpecHandle Handle = GetSpellRiseASC()->GiveAbility(Spec);
		AbilityHandles.Add(Handle);

		if (Grant.bAutoActivateIfNoInputTag && !Grant.InputTag.IsValid())
		{
			const bool bActivated = GetSpellRiseASC()->TryActivateAbility(Handle);
			if (!bActivated)
			{
				UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[GAS] AutoActivate FAILED: %s"), *GetNameSafe(Grant.Ability));
			}
		}
	}

	GetSpellRiseASC()->RefreshAbilityActorInfo();
	SendAbilitiesChangedEvent();

	return AbilityHandles;
}

void ASpellRiseCharacterBase::RemoveAbilities(const TArray<FGameplayAbilitySpecHandle>& AbilityHandlesToRemove)
{
	if (!GetSpellRiseASC() || !HasAuthority())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& AbilityHandle : AbilityHandlesToRemove)
	{
		GetSpellRiseASC()->ClearAbility(AbilityHandle);
	}

	SendAbilitiesChangedEvent();
}

void ASpellRiseCharacterBase::SendAbilitiesChangedEvent()
{
	if (!SpellRiseTags::Event_Abilities_Changed().IsValid())
	{
		return;
	}

	if (!GetSpellRiseASC())
	{
		InitASCActorInfo();
	}

	if (!GetSpellRiseASC())
	{
		UE_LOG(
			LogSpellRiseCharacterRuntime,
			Warning,
			TEXT("[GAS] SendAbilitiesChangedEvent skipped: missing ASC. Character=%s PlayerState=%s Controller=%s"),
			*GetNameSafe(this),
			*GetNameSafe(GetPlayerState()),
			*GetNameSafe(GetController()));
		return;
	}

	FGameplayEventData EventData;
	EventData.EventTag = SpellRiseTags::Event_Abilities_Changed();
	EventData.Instigator = this;
	EventData.Target = this;
	AActor* EventReceiverActor = GetPlayerState() ? Cast<AActor>(GetPlayerState()) : Cast<AActor>(this);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(EventReceiverActor, EventData.EventTag, EventData);
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

void ASpellRiseCharacterBase::MultiSendGameplayEventToActor_Implementation(AActor* TargetActor, const FGameplayEventData& EventData)
{
	if (!TargetActor || !EventData.EventTag.IsValid())
	{
		return;
	}

	FGameplayEventData SanitizedEventData = EventData;
	if (!FMath::IsFinite(SanitizedEventData.EventMagnitude))
	{
		SanitizedEventData.EventMagnitude = 0.f;
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, SanitizedEventData.EventTag, SanitizedEventData);
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

	UE_LOG(
		LogSpellRiseSecurity,
		Warning,
		TEXT("[SEC][RPC] Rejected Rpc=%s Reason=%s Character=%s Controller=%s PlayerState=%s TotalRejected=%d"),
		RpcName ? RpcName : TEXT("unknown"),
		*RejectReason,
		*GetNameSafe(this),
		*GetNameSafe(OwnerController),
		*GetNameSafe(OwnerPlayerState),
		ServerRejectedGenericRpcs);

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

	UE_LOG(
		LogSpellRiseSecurity,
		Warning,
		TEXT("[SEC][RPC][GameplayEvent] Rejected Tag=%s Reason=%s Character=%s Controller=%s PlayerState=%s TotalRejected=%d"),
		*EventTag.ToString(),
		*RejectReason,
		*GetNameSafe(this),
		*GetNameSafe(OwnerController),
		*GetNameSafe(OwnerPlayerState),
		ServerRejectedGameplayEvents);

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
		UE_LOG(
			LogSpellRiseCharacterRuntime,
			Warning,
			TEXT("[FullLoot][Death] OnDeadTagChanged ignorado por AvatarContext mismatch. Char=%s ASC=%s Avatar=%s Tag=%s NewCount=%d"),
			*GetNameSafe(this),
			*GetNameSafe(GetSpellRiseASC()),
			*GetNameSafe(GetSpellRiseASC() ? GetSpellRiseASC()->GetAvatarActor() : nullptr),
			*CallbackTag.ToString(),
			NewCount);
		return;
	}

	UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Death] OnDeadTagChanged Char=%s Tag=%s NewCount=%d IsDead=%d LootProcessed=%d"),
		*GetNameSafe(this),
		*CallbackTag.ToString(),
		NewCount,
		IsDead() ? 1 : 0,
		bFullLootProcessedForCurrentDeath ? 1 : 0);

	if (NewCount > 0)
	{
		if (!IsDead())
		{
			bIsDead = true;
		}

		if (HasAuthority())
		{
			if (!bFullLootProcessedForCurrentDeath)
			{
				if (UWorld* World = GetWorld())
				{
					if (USpellRiseFullLootSubsystem* FullLootSubsystem = World->GetSubsystem<USpellRiseFullLootSubsystem>())
					{
						UE_LOG(LogSpellRiseDeathLoot, Log, TEXT("[DeathLoot] Disparando FullLoot via OnDeadTagChanged. Char=%s"), *GetNameSafe(this));
						UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Death] Disparando FullLoot via OnDeadTagChanged. Char=%s"), *GetNameSafe(this));
						FullLootSubsystem->HandleCharacterDeath(this, FullLootBagClass);
						bFullLootProcessedForCurrentDeath = true;
					}
					else
					{
						UE_LOG(LogSpellRiseCharacterRuntime, Error, TEXT("[FullLoot][Death] Subsystem FullLoot ausente em OnDeadTagChanged. Char=%s"), *GetNameSafe(this));
					}
				}
			}

			MultiHandleDeath();
			ScheduleRespawn_Server();
		}
		else
		{
			HandleDeath();
		}

		SyncDeadStateFromASC(TEXT("OnDeadTagChanged"));
		return;
	}

	bIsDead = false;
	SyncDeadStateFromASC(TEXT("OnDeadTagChanged"));
	bFullLootProcessedForCurrentDeath = false;
	CombatLockExpireAtServerTimeSeconds = -1.0;
	ResetLocalDeathPresentation();
	SetCharacterInputEnabled(true);
	UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Death] Estado de morte resetado. Char=%s"), *GetNameSafe(this));

	if (HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(RespawnTimerHandle);
		}
		ApplyRegenStartupEffects();
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
		UE_LOG(LogSpellRiseCharacterRuntime, Error, TEXT("[FullLoot][Respawn] World nulo para %s"), *GetNameSafe(this));
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
		UE_LOG(
			LogSpellRiseCharacterRuntime,
			Warning,
			TEXT("[FullLoot][Respawn] CombatLock ativo. Delay base=%.1fs delay efetivo=%.1fs lock restante=%.2fs Char=%s"),
			BaseDelaySeconds,
			EffectiveDelaySeconds,
			CombatLockRemainingSeconds,
			*GetNameSafe(this));
	}
	else
	{
		CombatLockExpireAtServerTimeSeconds = -1.0;
	}

	UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Respawn] Agendado respawn em %.1fs para %s"), EffectiveDelaySeconds, *GetNameSafe(this));
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
		UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Respawn] Abortado: GameMode=%s Controller=%s Pawn=%s"),
			*GetNameSafe(GameMode),
			*GetNameSafe(ControllerToRespawn),
			*GetNameSafe(this));
		return;
	}

	UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[FullLoot][Respawn] Reiniciando jogador. Controller=%s PawnAntigo=%s"),
		*GetNameSafe(ControllerToRespawn),
		*GetNameSafe(this));

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
		LocalDeathScreenWidget->SetMessage(FText::FromString(TEXT("Você morreu.")));
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

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (APlayerCameraManager* CameraManager = PC->PlayerCameraManager)
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
}

void ASpellRiseCharacterBase::MultiHandleDeath_Implementation()
{
	HandleDeath();
}

void ASpellRiseCharacterBase::HandleDeath_Implementation()
{
	USkeletalMeshComponent* VisualMesh = GetVisualMeshComponent();
	if (!VisualMesh || !GetCapsuleComponent() || !GetCharacterMovement())
	{
		return;
	}

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

	VisualMesh->SetSimulatePhysics(true);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

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

	FVector Impulse = GetActorForwardVector() * -20000.f;
	Impulse.Z = 15000.f;
	VisualMesh->AddImpulseAtLocation(Impulse, GetActorLocation());
}

void ASpellRiseCharacterBase::RemoveRuntimeGrantedAbilitiesOnDeath_Server()
{
	if (!HasAuthority() || !GetSpellRiseASC())
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> HandlesToRemove;
	for (const FGameplayAbilitySpec& Spec : GetSpellRiseASC()->GetActivatableAbilities())
	{
		if (!Spec.Handle.IsValid())
		{
			continue;
		}

		if (StartupGrantedAbilityHandles.Contains(Spec.Handle))
		{
			continue;
		}

		if (Spec.SourceObject.Get() != this)
		{
			continue;
		}

		HandlesToRemove.Add(Spec.Handle);
	}

	if (HandlesToRemove.IsEmpty())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& AbilityHandle : HandlesToRemove)
	{
		GetSpellRiseASC()->ClearAbility(AbilityHandle);
	}

	GetSpellRiseASC()->RefreshAbilityActorInfo();
	SendAbilitiesChangedEvent();

	UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[Death] Removidas %d abilities runtime no servidor para %s"),
		HandlesToRemove.Num(),
		*GetNameSafe(this));
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

	const FVector PopLocation =
	(InstigatorActor ? InstigatorActor->GetActorLocation() : GetActorLocation()) + FVector(0.f, 0.f, 90.f);

	PC->ShowDamageNumber(
		Damage,
		PopLocation,
		SourceTags,
		TargetTags,
		bIsCrit
	);

	BP_ShowDamagePop(Damage, InstigatorActor, DamageTypeTag, bIsCrit);

	UE_LOG(LogSpellRiseCharacterRuntime, Warning, TEXT("[POP][Character] Damage=%.1f Crit=%d Type=%s Target=%s ViewerPC=%s"),
		Damage,
		bIsCrit ? 1 : 0,
		*DamageTypeTag.ToString(),
		*GetNameSafe(this),
		*GetNameSafe(PC));
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


