#include "SpellRiseCharacterBase.h"

// Engine - Ordem organizada
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// GAS Core
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"

// SpellRise GAS - Includes explícitos
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"

// SpellRise Weapons
#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"
#include "SpellRise/Weapons/Data/DA_WeaponDefinition.h"
#include "Animation/AnimInstance.h"  // ESSENCIAL - Resolve o erro UAnimInstance incomplete
// ---------------------------------------------------------
// Gameplay Tags (Versão otimizada)
// ---------------------------------------------------------
namespace SpellRiseTags
{
	FGameplayTag State_Dead()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("State.Dead"));
		return Tag;
	}

	FGameplayTag State_DerivedStats_Active()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("State.DerivedStats.Active"));
		return Tag;
	}

	FGameplayTag Event_Abilities_Changed()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Abilities.Changed"));
		return Tag;
	}

	FGameplayTag Data_MaxHealth()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.MaxHealth"));
		return Tag;
	}

	FGameplayTag Data_MaxMana()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.MaxMana"));
		return Tag;
	}

	FGameplayTag Data_MaxStamina()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.MaxStamina"));
		return Tag;
	}

	FGameplayTag Data_MaxShield()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.MaxShield"));
		return Tag;
	}

	FGameplayTag Data_CarryWeight()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.CarryWeight"));
		return Tag;
	}
}

// ---------------------------------------------------------
// Movement
// ---------------------------------------------------------
void ASpellRiseCharacterBase::ApplyMovementSpeedFromAttributes()
{
	USpellRiseAbilitySystemComponent* ASC = Cast<USpellRiseAbilitySystemComponent>(GetAbilitySystemComponent());
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	
	if (!ASC || !MoveComp)
	{
		return;
	}

	const float Bonus = ASC->GetNumericAttribute(UCombatAttributeSet::GetMoveSpeedAttribute());
	const float Mult  = ASC->GetNumericAttribute(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute());

	const float Desired = (BaseWalkSpeed + Bonus) * Mult;
	const float Clamped = FMath::Clamp(Desired, 10.f, 2000.f);

	if (!FMath::IsNearlyEqual(MoveComp->MaxWalkSpeed, Clamped, 0.1f))
	{
		MoveComp->MaxWalkSpeed = Clamped;
	}
}

void ASpellRiseCharacterBase::OnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	ApplyMovementSpeedFromAttributes();
}

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------
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

	WeaponComponent = CreateDefaultSubobject<USpellRiseWeaponComponent>(TEXT("WeaponComponent"));

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

// ---------------------------------------------------------
// Animation Tick / AnimInstance
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// Component Initialization
// ---------------------------------------------------------
void ASpellRiseCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!WeaponComponent)
	{
		WeaponComponent = FindComponentByClass<USpellRiseWeaponComponent>();

		UE_LOG(LogTemp, Warning, TEXT("[WeaponStartup] PostInitializeComponents: WeaponComponent was NULL. Resolved=%s Owner=%s"),
			*GetNameSafe(WeaponComponent), *GetNameSafe(this));
	}

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();
}

// ---------------------------------------------------------
// Engine Lifecycle
// ---------------------------------------------------------
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
	if (WeaponComponent)
	{
		WeaponComponent->HandleOwnerDeath();
	}

	Super::EndPlay(EndPlayReason);
}

void ASpellRiseCharacterBase::Destroyed()
{
	if (WeaponComponent)
	{
		WeaponComponent->HandleOwnerDeath();
	}

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

// ---------------------------------------------------------
// ASC Input Wrappers
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// Input Bindings
// ---------------------------------------------------------
void ASpellRiseCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!PlayerInputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Input] PlayerInputComponent is null on %s"), *GetName());
		return;
	}

	if (Action_PrimaryAttack != NAME_None)
	{
		PlayerInputComponent->BindAction(Action_PrimaryAttack, IE_Pressed,  this, &ASpellRiseCharacterBase::LI_PrimaryAttackPressed);
		PlayerInputComponent->BindAction(Action_PrimaryAttack, IE_Released, this, &ASpellRiseCharacterBase::LI_PrimaryAttackReleased);
	}

	if (Action_SecondaryAttack != NAME_None)
	{
		PlayerInputComponent->BindAction(Action_SecondaryAttack, IE_Pressed,  this, &ASpellRiseCharacterBase::LI_SecondaryAttackPressed);
		PlayerInputComponent->BindAction(Action_SecondaryAttack, IE_Released, this, &ASpellRiseCharacterBase::LI_SecondaryAttackReleased);
	}

	if (Action_Cancel != NAME_None)
	{
		PlayerInputComponent->BindAction(Action_Cancel, IE_Pressed,  this, &ASpellRiseCharacterBase::LI_CancelPressed);
		PlayerInputComponent->BindAction(Action_Cancel, IE_Released, this, &ASpellRiseCharacterBase::LI_CancelReleased);
	}
}

void ASpellRiseCharacterBase::AbilityInputPressed(EAbilityInputID InputID)
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GAS][Input] ASC is null on %s"), *GetName());
		return;
	}

	AbilitySystemComponent->SR_AbilityInputPressed(static_cast<int32>(InputID));
}

void ASpellRiseCharacterBase::AbilityInputReleased(EAbilityInputID InputID)
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GAS][Input] ASC is null on %s"), *GetName());
		return;
	}

	AbilitySystemComponent->SR_AbilityInputReleased(static_cast<int32>(InputID));
}

void ASpellRiseCharacterBase::LI_PrimaryAttackPressed()    { AbilityInputPressed(EAbilityInputID::PrimaryAttack); }
void ASpellRiseCharacterBase::LI_PrimaryAttackReleased()   { AbilityInputReleased(EAbilityInputID::PrimaryAttack); }
void ASpellRiseCharacterBase::LI_SecondaryAttackPressed()  { AbilityInputPressed(EAbilityInputID::SecondaryAttack); }
void ASpellRiseCharacterBase::LI_SecondaryAttackReleased() { AbilityInputReleased(EAbilityInputID::SecondaryAttack); }
void ASpellRiseCharacterBase::LI_CancelPressed()           { AbilityInputPressed(EAbilityInputID::Cancel); }
void ASpellRiseCharacterBase::LI_CancelReleased()          { AbilityInputReleased(EAbilityInputID::Cancel); }

// ---------------------------------------------------------
// AbilitySystemInterface
// ---------------------------------------------------------
UAbilitySystemComponent* ASpellRiseCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// ---------------------------------------------------------
// ASC ActorInfo
// ---------------------------------------------------------
void ASpellRiseCharacterBase::InitASCActorInfo()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->RefreshAbilityActorInfo();
}

// ---------------------------------------------------------
// Possession / Replication
// ---------------------------------------------------------
void ASpellRiseCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ForceServerAnimTick();
	EnsureAnimInstanceInitialized();

	if (!AbilitySystemComponent)
	{
		return;
	}

	if (!WeaponComponent)
	{
		WeaponComponent = FindComponentByClass<USpellRiseWeaponComponent>();
	}

	InitASCActorInfo();
	BindASCDelegates();

	UE_LOG(LogTemp, Warning, TEXT("[WeaponStartup] PossessedBy: Auth=%d Char=%s WeaponComp=%s DefaultWeapon=%s CompEquippedWeapon=%s"),
		HasAuthority() ? 1 : 0,
		*GetNameSafe(this),
		*GetNameSafe(WeaponComponent),
		*GetNameSafe(DefaultWeapon),
		WeaponComponent ? *GetNameSafe(WeaponComponent->GetEquippedWeapon()) : TEXT("NULL"));

	if (HasAuthority())
	{
		if (!WeaponComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("[WeaponStartup] WeaponComponent STILL NULL on server for %s. Fix BP: remove duplicate weapon components and keep the native WeaponComponent."), *GetNameSafe(this));
		}
		else
		{
			if (DefaultWeapon)
			{
				UE_LOG(LogTemp, Warning, TEXT("[WeaponStartup] Equipping DefaultWeapon on server: %s -> %s"),
					*GetNameSafe(this), *GetNameSafe(DefaultWeapon));

				WeaponComponent->EquipWeapon(DefaultWeapon);
			}
			else if (UDA_WeaponDefinition* Already = WeaponComponent->GetEquippedWeapon())
			{
				UE_LOG(LogTemp, Warning, TEXT("[WeaponStartup] DefaultWeapon NULL, using component EquippedWeapon: %s"), *GetNameSafe(Already));
				WeaponComponent->EquipWeapon(Already);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[WeaponStartup] NO WEAPON on server for %s. Set DefaultWeapon or WeaponComponent.EquippedWeapon."), *GetNameSafe(this));
			}
		}

		GrantAbilities(StartingAbilities);
		ApplyRegenStartupEffects();
		ApplyStartupEffects();
	}
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

// ---------------------------------------------------------
// Delegates / Attribute Listeners
// ---------------------------------------------------------
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

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMoveSpeedAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnMoveSpeedChanged);

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnMoveSpeedChanged);

	ApplyMovementSpeedFromAttributes();

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetVigorAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetFocusAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetAgilityAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetWillpowerAttribute())
		.AddUObject(this, &ASpellRiseCharacterBase::OnPrimaryChanged);

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetAttunementAttribute())
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

// ---------------------------------------------------------
// Startup Effects / Regen
// ---------------------------------------------------------
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

	const float VIG = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetVigorAttribute());
	const float AGI = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetAgilityAttribute());
	const float FOC = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetFocusAttribute());
	const float WIL = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetWillpowerAttribute());
	const float ATT = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetAttunementAttribute());

	UE_LOG(LogTemp, Warning, TEXT("[PRIM] VIG=%.1f AGI=%.1f FOC=%.1f WIL=%.1f ATT=%.1f"),
		VIG, AGI, FOC, WIL, ATT);

	const float WDM = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetWeaponDamageMultiplierAttribute());
	const float ASM = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetAttackSpeedMultiplierAttribute());
	const float CC  = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetCritChanceAttribute());
	const float CD  = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetCritDamageAttribute());
	const float SPM = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetSpellPowerMultiplierAttribute());
	const float CSM = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetCastSpeedMultiplierAttribute());
	const float MCM = AbilitySystemComponent->GetNumericAttribute(UDerivedStatsAttributeSet::GetManaCostMultiplierAttribute());

	UE_LOG(LogTemp, Warning, TEXT("[DER]  WDM=%.3f ASM=%.3f CC=%.3f CD=%.3f SPM=%.3f CSM=%.3f MCM=%.3f"),
		WDM, ASM, CC, CD, SPM, CSM, MCM);
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

// ---------------------------------------------------------
// Derived Resources
// ---------------------------------------------------------
void ASpellRiseCharacterBase::RecalculateDerivedStats()
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}

	const float VIG = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetVigorAttribute());
	const float FOC = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetFocusAttribute());
	const float AGI = AbilitySystemComponent->GetNumericAttribute(UCombatAttributeSet::GetAgilityAttribute());

	if (GE_RecalculateResources)
	{
		const float NewMaxHealth  = 100.f + (VIG * 12.f);
		const float NewMaxMana    = 80.f  + (FOC * 14.f);
		const float NewMaxStamina = 100.f + (AGI * 10.f) + (VIG * 4.f);
		const float NewMaxShield  = 0.f;
		const float NewCarryWeight = 30.f;

		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GE_RecalculateResources, 1.f, Context);
		if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
		{
			FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

			if (SpellRiseTags::Data_MaxHealth().IsValid())
				Spec->SetSetByCallerMagnitude(SpellRiseTags::Data_MaxHealth(), NewMaxHealth);

			if (SpellRiseTags::Data_MaxMana().IsValid())
				Spec->SetSetByCallerMagnitude(SpellRiseTags::Data_MaxMana(), NewMaxMana);

			if (SpellRiseTags::Data_MaxStamina().IsValid())
				Spec->SetSetByCallerMagnitude(SpellRiseTags::Data_MaxStamina(), NewMaxStamina);

			if (SpellRiseTags::Data_MaxShield().IsValid() && NewMaxShield > 0.f)
				Spec->SetSetByCallerMagnitude(SpellRiseTags::Data_MaxShield(), NewMaxShield);

			if (SpellRiseTags::Data_CarryWeight().IsValid())
				Spec->SetSetByCallerMagnitude(SpellRiseTags::Data_CarryWeight(), NewCarryWeight);

			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
		}
	}
}

// ---------------------------------------------------------
// Abilities
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// Death
// ---------------------------------------------------------
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

	if (WeaponComponent)
	{
		WeaponComponent->HandleOwnerDeath();
	}

	{
		TArray<UChildActorComponent*> ChildActorComps;
		GetComponents<UChildActorComponent>(ChildActorComps);
		for (UChildActorComponent* CAC : ChildActorComps)
		{
			if (!CAC)
			{
				continue;
			}
			CAC->DestroyChildActor();
		}
	}

	{
		TArray<AActor*> AttachedActors;
		GetAttachedActors(AttachedActors);
		for (AActor* Attached : AttachedActors)
		{
			if (!Attached)
			{
				continue;
			}

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

// ---------------------------------------------------------
// Catalyst UI/VFX
// ---------------------------------------------------------
void ASpellRiseCharacterBase::MultiOnCatalystProc_Implementation(int32 CatalystTier)
{
	BP_OnCatalystProc(CatalystTier);
}

// ---------------------------------------------------------
// Damage Pop
// ---------------------------------------------------------
void ASpellRiseCharacterBase::MultiShowDamagePop_Implementation(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit)
{
	if (Damage <= 0.f)
	{
		return;
	}

	BP_ShowDamagePop(Damage, InstigatorActor, DamageTypeTag, bIsCrit);
}