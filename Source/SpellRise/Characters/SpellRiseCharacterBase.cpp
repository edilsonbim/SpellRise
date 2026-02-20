// SpellRiseCharacterBase.cpp

#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "InputCoreTypes.h"

#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"

#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"

#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"

namespace SpellRiseTags
{
	static FGameplayTag State_Dead()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Dead"));
		return Tag;
	}

	static FGameplayTag State_DerivedStats_Active()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("State.DerivedStats.Active"));
		return Tag;
	}

	static FGameplayTag Event_Abilities_Changed()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Abilities.Changed"));
		return Tag;
	}

	static FGameplayTag Data_MaxHealth()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.MaxHealth"));
		return Tag;
	}

	static FGameplayTag Data_MaxMana()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.MaxMana"));
		return Tag;
	}

	static FGameplayTag Data_MaxStamina()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.MaxStamina"));
		return Tag;
	}

	static FGameplayTag Data_MaxShield()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.MaxShield"));
		return Tag;
	}

	static FGameplayTag Data_CarryWeight()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.CarryWeight"));
		return Tag;
	}
}

ASpellRiseCharacterBase::ASpellRiseCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<USpellRiseAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	BasicAttributeSet    = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("BasicAttributeSet"));
	CombatAttributeSet   = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));
	ResourceAttributeSet = CreateDefaultSubobject<UResourceAttributeSet>(TEXT("ResourceAttributeSet"));
	CatalystAttributeSet = CreateDefaultSubobject<UCatalystAttributeSet>(TEXT("CatalystAttributeSet"));
	DerivedStatsAttributeSet = CreateDefaultSubobject<UDerivedStatsAttributeSet>(TEXT("DerivedStatsAttributeSet"));

	DeadStateTag = SpellRiseTags::State_Dead();

	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	ForceServerAnimTick();
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

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC)
	{
		return;
	}

	// Universal inputs
	if (IA_Primary)
	{
		EIC->BindAction(IA_Primary, ETriggerEvent::Started,   this, &ASpellRiseCharacterBase::Input_Primary_Pressed);
		EIC->BindAction(IA_Primary, ETriggerEvent::Completed, this, &ASpellRiseCharacterBase::Input_Primary_Released);
		EIC->BindAction(IA_Primary, ETriggerEvent::Canceled,  this, &ASpellRiseCharacterBase::Input_Primary_Released);
	}

	if (IA_Secondary)
	{
		EIC->BindAction(IA_Secondary, ETriggerEvent::Started,   this, &ASpellRiseCharacterBase::Input_Secondary_Pressed);
		EIC->BindAction(IA_Secondary, ETriggerEvent::Completed, this, &ASpellRiseCharacterBase::Input_Secondary_Released);
		EIC->BindAction(IA_Secondary, ETriggerEvent::Canceled,  this, &ASpellRiseCharacterBase::Input_Secondary_Released);
	}

	if (IA_Interact)
	{
		EIC->BindAction(IA_Interact, ETriggerEvent::Started,   this, &ASpellRiseCharacterBase::Input_Interact_Pressed);
		EIC->BindAction(IA_Interact, ETriggerEvent::Completed, this, &ASpellRiseCharacterBase::Input_Interact_Released);
		EIC->BindAction(IA_Interact, ETriggerEvent::Canceled,  this, &ASpellRiseCharacterBase::Input_Interact_Released);
	}

	// Utility
	if (IA_ClearSelection)
	{
		EIC->BindAction(IA_ClearSelection, ETriggerEvent::Started, this, &ASpellRiseCharacterBase::Input_ClearSelection);
	}

	// Slots 1..8
	if (IA_Ability1)
	{
		EIC->BindAction(IA_Ability1, ETriggerEvent::Started,   this, &ASpellRiseCharacterBase::AbilityInputPressed,  EAbilityInputID::Ability1);
		EIC->BindAction(IA_Ability1, ETriggerEvent::Completed, this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability1);
		EIC->BindAction(IA_Ability1, ETriggerEvent::Canceled,  this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability1);
	}

	if (IA_Ability2)
	{
		EIC->BindAction(IA_Ability2, ETriggerEvent::Started,   this, &ASpellRiseCharacterBase::AbilityInputPressed,  EAbilityInputID::Ability2);
		EIC->BindAction(IA_Ability2, ETriggerEvent::Completed, this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability2);
		EIC->BindAction(IA_Ability2, ETriggerEvent::Canceled,  this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability2);
	}

	if (IA_Ability3)
	{
		EIC->BindAction(IA_Ability3, ETriggerEvent::Started,   this, &ASpellRiseCharacterBase::AbilityInputPressed,  EAbilityInputID::Ability3);
		EIC->BindAction(IA_Ability3, ETriggerEvent::Completed, this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability3);
		EIC->BindAction(IA_Ability3, ETriggerEvent::Canceled,  this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability3);
	}

	if (IA_Ability4)
	{
		EIC->BindAction(IA_Ability4, ETriggerEvent::Started,   this, &ASpellRiseCharacterBase::AbilityInputPressed,  EAbilityInputID::Ability4);
		EIC->BindAction(IA_Ability4, ETriggerEvent::Completed, this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability4);
		EIC->BindAction(IA_Ability4, ETriggerEvent::Canceled,  this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability4);
	}

	if (IA_Ability5)
	{
		EIC->BindAction(IA_Ability5, ETriggerEvent::Started,   this, &ASpellRiseCharacterBase::AbilityInputPressed,  EAbilityInputID::Ability5);
		EIC->BindAction(IA_Ability5, ETriggerEvent::Completed, this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability5);
		EIC->BindAction(IA_Ability5, ETriggerEvent::Canceled,  this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability5);
	}

	if (IA_Ability6)
	{
		EIC->BindAction(IA_Ability6, ETriggerEvent::Started,   this, &ASpellRiseCharacterBase::AbilityInputPressed,  EAbilityInputID::Ability6);
		EIC->BindAction(IA_Ability6, ETriggerEvent::Completed, this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability6);
		EIC->BindAction(IA_Ability6, ETriggerEvent::Canceled,  this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability6);
	}

	if (IA_Ability7)
	{
		EIC->BindAction(IA_Ability7, ETriggerEvent::Started,   this, &ASpellRiseCharacterBase::AbilityInputPressed,  EAbilityInputID::Ability7);
		EIC->BindAction(IA_Ability7, ETriggerEvent::Completed, this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability7);
		EIC->BindAction(IA_Ability7, ETriggerEvent::Canceled,  this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability7);
	}

	if (IA_Ability8)
	{
		EIC->BindAction(IA_Ability8, ETriggerEvent::Started,   this, &ASpellRiseCharacterBase::AbilityInputPressed,  EAbilityInputID::Ability8);
		EIC->BindAction(IA_Ability8, ETriggerEvent::Completed, this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability8);
		EIC->BindAction(IA_Ability8, ETriggerEvent::Canceled,  this, &ASpellRiseCharacterBase::AbilityInputReleased, EAbilityInputID::Ability8);
	}
}

void ASpellRiseCharacterBase::Input_Primary_Pressed(const FInputActionValue& Value)
{
	if (!IsLocallyControlled() || !AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(EAbilityInputID::Primary));
}

void ASpellRiseCharacterBase::Input_Primary_Released(const FInputActionValue& Value)
{
	if (!IsLocallyControlled() || !AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(EAbilityInputID::Primary));
}

void ASpellRiseCharacterBase::Input_Secondary_Pressed(const FInputActionValue& Value)
{
	if (!IsLocallyControlled() || !AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(EAbilityInputID::Secondary));
}

void ASpellRiseCharacterBase::Input_Secondary_Released(const FInputActionValue& Value)
{
	if (!IsLocallyControlled() || !AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(EAbilityInputID::Secondary));
}

void ASpellRiseCharacterBase::Input_Interact_Pressed(const FInputActionValue& Value)
{
	if (!IsLocallyControlled() || !AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(EAbilityInputID::Interact));
}

void ASpellRiseCharacterBase::Input_Interact_Released(const FInputActionValue& Value)
{
	if (!IsLocallyControlled() || !AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(EAbilityInputID::Interact));
}

void ASpellRiseCharacterBase::Input_ClearSelection(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	ClearSelectedAbility();
}

void ASpellRiseCharacterBase::AbilityInputPressed(EAbilityInputID InputID)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (!AbilitySystemComponent)
	{
		return;
	}

	if (InputID >= EAbilityInputID::Ability1 && InputID <= EAbilityInputID::Ability8)
	{
		const int32 SlotIndex = static_cast<int32>(InputID) - static_cast<int32>(EAbilityInputID::Ability1);
		AbilityWheelInputPressed(SlotIndex);
		return;
	}

	AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(InputID));
}

void ASpellRiseCharacterBase::AbilityInputReleased(EAbilityInputID InputID)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (!AbilitySystemComponent)
	{
		return;
	}

	if (InputID >= EAbilityInputID::Ability1 && InputID <= EAbilityInputID::Ability8)
	{
		const int32 SlotIndex = static_cast<int32>(InputID) - static_cast<int32>(EAbilityInputID::Ability1);
		AbilityWheelInputReleased(SlotIndex);
		return;
	}

	AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(InputID));
}

void ASpellRiseCharacterBase::ForceServerAnimTick()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		MeshComp->bEnableUpdateRateOptimizations = false;
		MeshComp->SetComponentTickEnabled(true);
	}
}

void ASpellRiseCharacterBase::EnsureAnimInstanceInitialized()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (MeshComp->AnimClass && !MeshComp->GetAnimInstance())
		{
			MeshComp->InitAnim(true);
		}
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

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetReplicationMode(AscReplicationMode);
	}

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();

	InitASCActorInfo();
	BindASCDelegates();

	if (HasAuthority())
	{
		USkeletalMeshComponent* MeshComp = GetMesh();
		UE_LOG(LogTemp, Warning, TEXT("[Anim][Server] Char=%s Mesh=%s AnimClass=%s AnimInstance=%s TickOption=%d"),
			*GetNameSafe(this),
			*GetNameSafe(MeshComp),
			MeshComp ? *GetNameSafe(MeshComp->AnimClass) : TEXT("NULL"),
			MeshComp ? *GetNameSafe(MeshComp->GetAnimInstance()) : TEXT("NULL"),
			MeshComp ? (int32)MeshComp->VisibilityBasedAnimTickOption : -1);
	}
}

void ASpellRiseCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASpellRiseCharacterBase::Destroyed()
{
	Super::Destroyed();
}

void ASpellRiseCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled())
	{
		if (!HasAuthority() && AbilitySystemComponent)
		{
			AbilitySystemComponent->SR_ClearAbilityInput();
		}
		return;
	}

	SR_ProcessAbilityInput(DeltaTime, false);
}

void ASpellRiseCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASpellRiseCharacterBase, SelectedAbilityInputID, COND_OwnerOnly);
	DOREPLIFETIME(ASpellRiseCharacterBase, Archetype);
}

void ASpellRiseCharacterBase::OnRep_SelectedAbilityInputID()
{
}

void ASpellRiseCharacterBase::OnRep_Archetype()
{
}

void ASpellRiseCharacterBase::SetArchetype(ESpellRiseArchetype NewArchetype)
{
	if (HasAuthority())
	{
		Archetype = NewArchetype;
		ApplyArchetypeToPrimaries_Server();
		return;
	}

	ServerSetArchetype(NewArchetype);
	Archetype = NewArchetype;
}

void ASpellRiseCharacterBase::ServerSetArchetype_Implementation(ESpellRiseArchetype NewArchetype)
{
	Archetype = NewArchetype;
	ApplyArchetypeToPrimaries_Server();
}

void ASpellRiseCharacterBase::ApplyArchetypeToPrimaries_Server()
{
	if (!HasAuthority() || !AbilitySystemComponent)
	{
		return;
	}

	// New primaries baseline:
	// - Base 20..100 (100 distribuíveis)
	// - Catalyst pode empurrar até 120
	constexpr float Baseline = 20.f;

	float dSTR = 0.f;
	float dAGI = 0.f;
	float dINT = 0.f;
	float dWIS = 0.f;

	switch (Archetype)
	{
	case ESpellRiseArchetype::Warrior:
		dSTR = 20.f; dAGI = 20.f;
		break;
	case ESpellRiseArchetype::Assassin:
		dAGI = 20.f; dSTR = 10.f; dWIS = 10.f;
		break;
	case ESpellRiseArchetype::Mage:
		dINT = 20.f; dWIS = 20.f;
		break;
	case ESpellRiseArchetype::Battlemage:
		dSTR = 10.f; dAGI = 10.f; dINT = 10.f; dWIS = 10.f;
		break;
	case ESpellRiseArchetype::Cleric:
		dWIS = 30.f; dSTR = 10.f;
		break;
	case ESpellRiseArchetype::None:
	default:
		break;
	}

	AbilitySystemComponent->SetNumericAttributeBase(UCombatAttributeSet::GetStrengthAttribute(),     Baseline + dSTR);
	AbilitySystemComponent->SetNumericAttributeBase(UCombatAttributeSet::GetAgilityAttribute(),      Baseline + dAGI);
	AbilitySystemComponent->SetNumericAttributeBase(UCombatAttributeSet::GetIntelligenceAttribute(), Baseline + dINT);
	AbilitySystemComponent->SetNumericAttributeBase(UCombatAttributeSet::GetWisdomAttribute(),       Baseline + dWIS);

	RecalculateDerivedStats();
	ApplyDerivedStatsInfinite();
	LogDerivedDebug();
}

void ASpellRiseCharacterBase::ServerSetSelectedAbilityInputID_Implementation(EAbilityInputID NewID)
{
	SelectedAbilityInputID = NewID;
}

void ASpellRiseCharacterBase::SelectAbilitySlot(int32 SlotIndex)
{
	EAbilityInputID NewID = EAbilityInputID::None;
	if (SlotIndex >= 0 && SlotIndex < 8)
	{
		NewID = static_cast<EAbilityInputID>(static_cast<uint8>(EAbilityInputID::Ability1) + SlotIndex);
	}

	if (!HasAuthority())
	{
		ServerSetSelectedAbilityInputID(NewID);
	}

	SelectedAbilityInputID = NewID;
}

void ASpellRiseCharacterBase::AbilityWheelInputPressed(int32 SlotIndex)
{
	SelectAbilitySlot(SlotIndex);

	if (!IsLocallyControlled() || !AbilitySystemComponent)
	{
		return;
	}

	if (SlotIndex < 0 || SlotIndex >= 8)
	{
		return;
	}

	const int32 InputID = static_cast<int32>(EAbilityInputID::Ability1) + SlotIndex;
	USpellRiseAbilitySystemComponent* SRASC = Cast<USpellRiseAbilitySystemComponent>(AbilitySystemComponent);
	if (!SRASC)
	{
		return;
	}

	SRASC->SR_TryActivateAbilityByInputID(InputID);

	if (USpellRiseGameplayAbility* SRGA = SRASC->SR_GetSpellRiseAbilityForInputID(InputID))
	{
		if (SRGA->bActivateOnSelection)
		{
			AbilitySystemComponent->AbilityLocalInputPressed(InputID);
		}
	}
}

void ASpellRiseCharacterBase::AbilityWheelInputReleased(int32 SlotIndex)
{
	if (!IsLocallyControlled() || !AbilitySystemComponent)
	{
		return;
	}

	if (SlotIndex < 0 || SlotIndex >= 8)
	{
		return;
	}

	const int32 InputID = static_cast<int32>(EAbilityInputID::Ability1) + SlotIndex;
	USpellRiseAbilitySystemComponent* SRASC = Cast<USpellRiseAbilitySystemComponent>(AbilitySystemComponent);
	if (!SRASC)
	{
		return;
	}

	if (USpellRiseGameplayAbility* SRGA = SRASC->SR_GetSpellRiseAbilityForInputID(InputID))
	{
		if (SRGA->bActivateOnSelection)
		{
			AbilitySystemComponent->AbilityLocalInputReleased(InputID);
			ClearSelectedAbility();
		}
	}
}

void ASpellRiseCharacterBase::ClearSelectedAbility()
{
	EAbilityInputID NewID = EAbilityInputID::None;

	if (!HasAuthority())
	{
		ServerSetSelectedAbilityInputID(NewID);
	}

	SelectedAbilityInputID = NewID;
}

void ASpellRiseCharacterBase::SR_ProcessAbilityInput(float DeltaSeconds, bool bGamePaused)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SR_ProcessAbilityInput(DeltaSeconds, bGamePaused);
	}
}

void ASpellRiseCharacterBase::SR_ClearAbilityInput()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SR_ClearAbilityInput();
	}
}

UAbilitySystemComponent* ASpellRiseCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASpellRiseCharacterBase::InitASCActorInfo()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->RefreshAbilityActorInfo();
}

void ASpellRiseCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();

	if (!AbilitySystemComponent)
	{
		return;
	}

	InitASCActorInfo();
	BindASCDelegates();

	if (!HasAuthority())
	{
		return;
	}

	ApplyArchetypeToPrimaries_Server();

	static const FName StartupGrantedTagName(TEXT("SpellRise.StartupAbilitiesGranted"));
	const bool bAlreadyGranted = Tags.Contains(StartupGrantedTagName);

	if (!bAlreadyGranted)
	{
		GrantAbilities(StartingAbilities);
		Tags.AddUnique(StartupGrantedTagName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[GAS] Startup abilities already granted for %s (skipping)"), *GetNameSafe(this));
	}

	ApplyRegenStartupEffects();
	ApplyStartupEffects();
}

void ASpellRiseCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();

	if (!AbilitySystemComponent)
	{
		return;
	}

	InitASCActorInfo();
	BindASCDelegates();
}

void ASpellRiseCharacterBase::BindASCDelegates()
{
	if (!AbilitySystemComponent || bASCDelegatesBound)
	{
		return;
	}
	bASCDelegatesBound = true;

	if (DeadStateTag.IsValid())
	{
		AbilitySystemComponent
			->RegisterGameplayTagEvent(DeadStateTag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &ASpellRiseCharacterBase::OnDeadTagChanged);
	}

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnHealthChanged);

	// Primaries (new)
	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetStrengthAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetAgilityAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetIntelligenceAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetWisdomAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);
}

void ASpellRiseCharacterBase::OnPrimaryChanged(const FOnAttributeChangeData& Data)
{
	if (!HasAuthority() || !AbilitySystemComponent)
	{
		return;
	}

	RecalculateDerivedStats();
	ApplyDerivedStatsInfinite();
	LogDerivedDebug();
}

void ASpellRiseCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (bIsDead)
	{
		return;
	}

	const float NewHealth = Data.NewValue;
	if (NewHealth > 0.f)
	{
		return;
	}

	if (!HasAuthority() || !AbilitySystemComponent)
	{
		return;
	}

	bIsDead = true;

	if (GE_Death)
	{
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GE_Death, 1.f, Context);
		if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		else if (DeadStateTag.IsValid())
		{
			AbilitySystemComponent->AddLooseGameplayTag(DeadStateTag);
		}
	}
	else if (DeadStateTag.IsValid())
	{
		AbilitySystemComponent->AddLooseGameplayTag(DeadStateTag);
	}

	AbilitySystemComponent->CancelAllAbilities();
}

void ASpellRiseCharacterBase::ApplyStartupEffects()
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}

	if (!GE_RecalculateResources)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GAS] GE_RecalculateResources is not set on %s"), *GetName());
		return;
	}

	RecalculateDerivedStats();
	ApplyDerivedStatsInfinite();
	LogDerivedDebug();
}

void ASpellRiseCharacterBase::ApplyDerivedStatsInfinite()
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}

	if (!GE_DerivedStatsInfinite)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DERIVED] GE_DerivedStatsInfinite is NULL on %s."), *GetNameSafe(this));
		return;
	}

	AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(GE_DerivedStatsInfinite, AbilitySystemComponent);

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GE_DerivedStatsInfinite, 1.f, Context);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return;
	}

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void ASpellRiseCharacterBase::LogDerivedDebug()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	const bool bTagActive =
		AbilitySystemComponent->HasMatchingGameplayTag(SpellRiseTags::State_DerivedStats_Active());

	UE_LOG(LogTemp, Warning, TEXT("[DERIVED] GE=%s TagActive=%d"),
		*GetNameSafe(GE_DerivedStatsInfinite),
		bTagActive ? 1 : 0);

	const float STR = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetStrengthAttribute());
	const float AGI = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetAgilityAttribute());
	const float INT = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetIntelligenceAttribute());
	const float WIS = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetWisdomAttribute());

	UE_LOG(LogTemp, Warning, TEXT("[PRIM] STR=%.1f AGI=%.1f INT=%.1f WIS=%.1f"),
		STR, AGI, INT, WIS);

	const float MeleeMult = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetMeleeDamageMultiplierAttribute());
	const float BowMult   = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetBowDamageMultiplierAttribute());
	const float SpellMult = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetSpellDamageMultiplierAttribute());
	const float HealMult  = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetHealingMultiplierAttribute());

	const float CTR = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetCastTimeReductionAttribute());
	const float CC  = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetCritChanceAttribute());
	const float CD  = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetCritDamageAttribute());
	const float AP  = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetArmorPenetrationAttribute());
	const float MCR = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetManaCostReductionAttribute());

	UE_LOG(LogTemp, Warning, TEXT("[DER]  Melee=%.3f Bow=%.3f Spell=%.3f Heal=%.3f CTR=%.3f CC=%.3f CD=%.3f AP=%.3f MCR=%.3f"),
		MeleeMult, BowMult, SpellMult, HealMult, CTR, CC, CD, AP, MCR);
}

void ASpellRiseCharacterBase::ApplyOrRefreshEffect(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!AbilitySystemComponent || !HasAuthority() || !EffectClass)
	{
		return;
	}

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.f, Context);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return;
	}

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void ASpellRiseCharacterBase::ApplyRegenStartupEffects()
{
	if (!AbilitySystemComponent || !HasAuthority())
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
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}

	// Agora o cálculo de MaxHealth/MaxMana/MaxStamina/CarryWeight deve vir do próprio GE_RecalculateResources
	// via MMCs (MaxHealthFromPrimaries/MaxManaFromPrimaries/MaxStaminaFromPrimaries/CarryWeightFromPrimaries).
	ApplyOrRefreshEffect(GE_RecalculateResources);
}

TArray<FGameplayAbilitySpecHandle> ASpellRiseCharacterBase::GrantAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant,
	const int32 Level)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return {};
	}

	TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	AbilityHandles.Reserve(AbilitiesToGrant.Num());

	for (TSubclassOf<UGameplayAbility> AbilityClass : AbilitiesToGrant)
	{
		if (!AbilityClass)
		{
			continue;
		}

		int32 InputID = INDEX_NONE;
		bool bAutoActivate = false;

		if (const USpellRiseGameplayAbility* CDO = Cast<USpellRiseGameplayAbility>(AbilityClass->GetDefaultObject()))
		{
			InputID = static_cast<int32>(CDO->AbilityInputID);
			bAutoActivate = CDO->AutoActivateWhenGranted;
		}

		FGameplayAbilitySpec Spec(AbilityClass, Level, InputID, this);
		const FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
		AbilityHandles.Add(Handle);

		const bool bHasInputBinding =
			(InputID != INDEX_NONE) &&
			(InputID != static_cast<int32>(EAbilityInputID::None));

		if (bAutoActivate && !bHasInputBinding)
		{
			const bool bActivated = AbilitySystemComponent->TryActivateAbility(Handle);
			if (!bActivated)
			{
				UE_LOG(LogTemp, Warning, TEXT("[GAS] AutoActivate FAILED: %s"), *GetNameSafe(AbilityClass));
			}
		}
	}

	AbilitySystemComponent->RefreshAbilityActorInfo();
	SendAbilitiesChangedEvent();
	return AbilityHandles;
}

void ASpellRiseCharacterBase::RemoveAbilities(const TArray<FGameplayAbilitySpecHandle>& AbilityHandlesToRemove)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& AbilityHandle : AbilityHandlesToRemove)
	{
		AbilitySystemComponent->ClearAbility(AbilityHandle);
	}

	SendAbilitiesChangedEvent();
}

void ASpellRiseCharacterBase::SendAbilitiesChangedEvent()
{
	if (!SpellRiseTags::Event_Abilities_Changed().IsValid())
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.EventTag = SpellRiseTags::Event_Abilities_Changed();
	EventData.Instigator = this;
	EventData.Target = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

void ASpellRiseCharacterBase::MultiSendGameplayEventToActor_Implementation(AActor* TargetActor, const FGameplayEventData& EventData)
{
	if (!TargetActor || !EventData.EventTag.IsValid())
	{
		return;
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, EventData.EventTag, EventData);
}

void ASpellRiseCharacterBase::ServerSendGameplayEventToSelf_Implementation(const FGameplayEventData& EventData)
{
	if (!EventData.EventTag.IsValid())
	{
		return;
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

void ASpellRiseCharacterBase::OnDeadTagChanged(FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		if (!bIsDead)
		{
			bIsDead = true;
		}

		if (HasAuthority())
		{
			MultiHandleDeath();
		}
		else
		{
			HandleDeath();
		}
		return;
	}

	bIsDead = false;

	if (HasAuthority())
	{
		ApplyRegenStartupEffects();
	}
}

void ASpellRiseCharacterBase::MultiHandleDeath_Implementation()
{
	HandleDeath();
}

void ASpellRiseCharacterBase::HandleDeath_Implementation()
{
	if (!GetMesh() || !GetCapsuleComponent() || !GetCharacterMovement())
	{
		return;
	}

	if (GetMesh()->IsSimulatingPhysics())
	{
		return;
	}

	{
		TArray<UChildActorComponent*> ChildActorComps;
		GetComponents<UChildActorComponent>(ChildActorComps);
		for (UChildActorComponent* CAC : ChildActorComps)
		{
			if (!CAC) continue;
			CAC->DestroyChildActor();
		}
	}

	{
		TArray<AActor*> AttachedActors;
		GetAttachedActors(AttachedActors);
		for (AActor* Attached : AttachedActors)
		{
			if (!Attached) continue;

			if (Attached->GetOwner() == this)
			{
				Attached->Destroy();
			}
		}
	}

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	FVector Impulse = GetActorForwardVector() * -20000.f;
	Impulse.Z = 15000.f;
	GetMesh()->AddImpulseAtLocation(Impulse, GetActorLocation());
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

	BP_ShowDamagePop(Damage, InstigatorActor, DamageTypeTag, bIsCrit);
}
