// Base C++ autoritativa para inimigos GAS.

#include "SpellRise/Characters/SpellRiseEnemyCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/SoftObjectPath.h"

#include "InventoryFunctionLibrary.h"
#include "SpellRise/Components/CatalystComponent.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/Inventory/SpellRiseLootBagActor.h"
#include "SpellRise/Inventory/SpellRiseLootBagNameComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseEnemyRuntime, Log, All);

namespace
{
	constexpr TCHAR DefaultNarrativeBagClassPath[] = TEXT("/Game/UI/InventorySystem/BP_Bag.BP_Bag_C");
	constexpr TCHAR LegacyBagClassPath[] = TEXT("/Game/UI/InventorySystem/BP_Storage_Bag.BP_Storage_Bag_C");

	static TSubclassOf<AActor> ResolveEnemyLootBagClass(TSubclassOf<AActor> OverrideClass)
	{
		if (OverrideClass)
		{
			return OverrideClass;
		}

		static TSoftClassPtr<AActor> DefaultBagClass{FSoftObjectPath(DefaultNarrativeBagClassPath)};
		if (UClass* LoadedDefaultClass = DefaultBagClass.LoadSynchronous())
		{
			return LoadedDefaultClass;
		}

		static TSoftClassPtr<AActor> LegacyBagClass{FSoftObjectPath(LegacyBagClassPath)};
		return LegacyBagClass.LoadSynchronous();
	}

	static void StabilizeEnemyLootBagActor(AActor* LootBagActor)
	{
		if (!LootBagActor)
		{
			return;
		}

		TArray<UPrimitiveComponent*> PrimitiveComponents;
		LootBagActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
		for (UPrimitiveComponent* Primitive : PrimitiveComponents)
		{
			if (!Primitive)
			{
				continue;
			}

			if (Primitive->IsSimulatingPhysics())
			{
				Primitive->SetPhysicsLinearVelocity(FVector::ZeroVector);
				Primitive->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
				Primitive->SetSimulatePhysics(false);
			}
		}
	}

	static void SetEnemyLootBagDisplayName(AActor* LootBagActor, const FString& DisplayName)
	{
		if (!LootBagActor || DisplayName.IsEmpty())
		{
			return;
		}

		if (ASpellRiseLootBagActor* SpellRiseBag = Cast<ASpellRiseLootBagActor>(LootBagActor))
		{
			SpellRiseBag->SetDeadPlayerDisplayName(DisplayName);
		}

		if (!LootBagActor->HasAuthority())
		{
			return;
		}

		USpellRiseLootBagNameComponent* LootBagNameComponent =
			LootBagActor->FindComponentByClass<USpellRiseLootBagNameComponent>();
		if (!LootBagNameComponent)
		{
			LootBagNameComponent = NewObject<USpellRiseLootBagNameComponent>(LootBagActor, TEXT("SpellRiseEnemyLootBagNameComponent"));
			if (LootBagNameComponent)
			{
				LootBagActor->AddInstanceComponent(LootBagNameComponent);
				LootBagNameComponent->RegisterComponent();
			}
		}

		if (LootBagNameComponent)
		{
			LootBagNameComponent->SetDeadPlayerDisplayName_Server(DisplayName);
		}
	}
}

ASpellRiseEnemyCharacterBase::ASpellRiseEnemyCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<USpellRiseAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetIsReplicated(true);
		AbilitySystemComponent->SetReplicationMode(AscReplicationMode);
	}

	BasicAttributeSet = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("BasicAttributeSet"));
	CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));
	ResourceAttributeSet = CreateDefaultSubobject<UResourceAttributeSet>(TEXT("ResourceAttributeSet"));
	CatalystAttributeSet = CreateDefaultSubobject<UCatalystAttributeSet>(TEXT("CatalystAttributeSet"));
	DerivedStatsAttributeSet = CreateDefaultSubobject<UDerivedStatsAttributeSet>(TEXT("DerivedStatsAttributeSet"));
	CatalystComponent = CreateDefaultSubobject<UCatalystComponent>(TEXT("CatalystComponent"));

	DeadStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Dead"), false);
	EnemyDisplayName = FText::FromString(TEXT("Enemy"));

	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.f, 500.f, 0.f);
		Movement->MaxWalkSpeed = 500.f;
	}
}

void ASpellRiseEnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeAbilitySystem();
	BindASCDelegates();

	if (HasAuthority() && !bStartupInitialized)
	{
		ApplyEnemyAttributeFallbacks_Server();
		ApplyEnemyStartupEffects_Server();
		GrantEnemyStartupAbilities_Server();
		bStartupInitialized = true;
	}
}

void ASpellRiseEnemyCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AbilitySystemComponent && bASCDelegatesBound)
	{
		if (DeadStateTag.IsValid())
		{
			AbilitySystemComponent
				->RegisterGameplayTagEvent(DeadStateTag, EGameplayTagEventType::NewOrRemoved)
				.RemoveAll(this);
		}

		AbilitySystemComponent
			->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetHealthAttribute())
			.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ASpellRiseEnemyCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitializeAbilitySystem();
	BindASCDelegates();

	if (HasAuthority() && !bStartupInitialized)
	{
		ApplyEnemyAttributeFallbacks_Server();
		ApplyEnemyStartupEffects_Server();
		GrantEnemyStartupAbilities_Server();
		bStartupInitialized = true;
	}
}

UAbilitySystemComponent* ASpellRiseEnemyCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASpellRiseEnemyCharacterBase::InitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->SetReplicationMode(AscReplicationMode);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->RefreshAbilityActorInfo();
}

void ASpellRiseEnemyCharacterBase::BindASCDelegates()
{
	if (!AbilitySystemComponent || bASCDelegatesBound)
	{
		return;
	}

	if (DeadStateTag.IsValid())
	{
		AbilitySystemComponent
			->RegisterGameplayTagEvent(DeadStateTag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &ASpellRiseEnemyCharacterBase::OnDeadTagChanged);
	}

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ASpellRiseEnemyCharacterBase::OnHealthChanged);

	bASCDelegatesBound = true;
}

void ASpellRiseEnemyCharacterBase::ApplyEnemyAttributeFallbacks_Server()
{
	if (!HasAuthority() || !AbilitySystemComponent)
	{
		return;
	}

	const float PrimaryValue = FMath::Clamp(InitialPrimaryValue, 20.0f, 120.0f);
	AbilitySystemComponent->SetNumericAttributeBase(UCombatAttributeSet::GetStrengthAttribute(), PrimaryValue);
	AbilitySystemComponent->SetNumericAttributeBase(UCombatAttributeSet::GetAgilityAttribute(), PrimaryValue);
	AbilitySystemComponent->SetNumericAttributeBase(UCombatAttributeSet::GetIntelligenceAttribute(), PrimaryValue);
	AbilitySystemComponent->SetNumericAttributeBase(UCombatAttributeSet::GetWisdomAttribute(), PrimaryValue);

	const float SafeMaxHealth = FMath::Max(1.0f, InitialMaxHealth);
	const float SafeMaxMana = FMath::Max(0.0f, InitialMaxMana);
	const float SafeMaxStamina = FMath::Max(0.0f, InitialMaxStamina);

	AbilitySystemComponent->SetNumericAttributeBase(UResourceAttributeSet::GetMaxHealthAttribute(), SafeMaxHealth);
	AbilitySystemComponent->SetNumericAttributeBase(UResourceAttributeSet::GetHealthAttribute(), SafeMaxHealth);
	AbilitySystemComponent->SetNumericAttributeBase(UResourceAttributeSet::GetMaxManaAttribute(), SafeMaxMana);
	AbilitySystemComponent->SetNumericAttributeBase(UResourceAttributeSet::GetManaAttribute(), SafeMaxMana);
	AbilitySystemComponent->SetNumericAttributeBase(UResourceAttributeSet::GetMaxStaminaAttribute(), SafeMaxStamina);
	AbilitySystemComponent->SetNumericAttributeBase(UResourceAttributeSet::GetStaminaAttribute(), SafeMaxStamina);
}

void ASpellRiseEnemyCharacterBase::ApplyEnemyStartupEffects_Server()
{
	if (!HasAuthority() || !AbilitySystemComponent)
	{
		return;
	}

	ApplyGameplayEffectToSelf_Server(GE_RecalculateResources);
	ApplyGameplayEffectToSelf_Server(GE_DerivedStatsInfinite);

	for (const TSubclassOf<UGameplayEffect>& StartupEffect : StartupEffects)
	{
		ApplyGameplayEffectToSelf_Server(StartupEffect);
	}

	for (const TSubclassOf<UGameplayEffect>& RegenEffect : GE_RegenEffects)
	{
		ApplyGameplayEffectToSelf_Server(RegenEffect);
	}

	const float MaxHealthValue = AbilitySystemComponent->GetNumericAttribute(UResourceAttributeSet::GetMaxHealthAttribute());
	const float MaxManaValue = AbilitySystemComponent->GetNumericAttribute(UResourceAttributeSet::GetMaxManaAttribute());
	const float MaxStaminaValue = AbilitySystemComponent->GetNumericAttribute(UResourceAttributeSet::GetMaxStaminaAttribute());

	AbilitySystemComponent->SetNumericAttributeBase(UResourceAttributeSet::GetHealthAttribute(), FMath::Max(1.0f, MaxHealthValue));
	AbilitySystemComponent->SetNumericAttributeBase(UResourceAttributeSet::GetManaAttribute(), FMath::Max(0.0f, MaxManaValue));
	AbilitySystemComponent->SetNumericAttributeBase(UResourceAttributeSet::GetStaminaAttribute(), FMath::Max(0.0f, MaxStaminaValue));
}

void ASpellRiseEnemyCharacterBase::ApplyGameplayEffectToSelf_Server(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!HasAuthority() || !AbilitySystemComponent || !EffectClass)
	{
		return;
	}

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.0f, Context);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		UE_LOG(LogSpellRiseEnemyRuntime, Warning, TEXT("Enemy startup GE spec invalido. Enemy=%s Effect=%s"),
			*GetNameSafe(this),
			*GetNameSafe(EffectClass.Get()));
		return;
	}

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

TArray<FGameplayAbilitySpecHandle> ASpellRiseEnemyCharacterBase::GrantEnemyStartupAbilities_Server()
{
	if (!HasAuthority() || !AbilitySystemComponent)
	{
		return {};
	}

	TArray<FGameplayAbilitySpecHandle> GrantedHandles;
	GrantedHandles.Reserve(StartingAbilities.Num());

	for (const FSpellRiseEnemyGrantedAbility& Grant : StartingAbilities)
	{
		if (!Grant.Ability)
		{
			continue;
		}

		if (AbilitySystemComponent->FindAbilitySpecFromClass(Grant.Ability) != nullptr)
		{
			continue;
		}

		const int32 FinalLevel = FMath::Max(1, Grant.AbilityLevel);
		FGameplayAbilitySpec Spec(Grant.Ability, FinalLevel, INDEX_NONE, this);
		if (Grant.InputTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(Grant.InputTag);
		}

		const FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
		if (Handle.IsValid())
		{
			GrantedHandles.Add(Handle);
			StartupGrantedAbilityHandles.AddUnique(Handle);
		}

		if (Grant.bAutoActivateIfNoInputTag && !Grant.InputTag.IsValid())
		{
			AbilitySystemComponent->TryActivateAbility(Handle);
		}
	}

	AbilitySystemComponent->RefreshAbilityActorInfo();
	return GrantedHandles;
}

bool ASpellRiseEnemyCharacterBase::IsEnemyDead() const
{
	if (AbilitySystemComponent && DeadStateTag.IsValid() && AbilitySystemComponent->HasMatchingGameplayTag(DeadStateTag))
	{
		return true;
	}

	return bIsDead;
}

void ASpellRiseEnemyCharacterBase::OnDeadTagChanged(FGameplayTag CallbackTag, int32 NewCount)
{
	const bool bNowDead = NewCount > 0;
	bIsDead = bNowDead;

	if (bNowDead)
	{
		if (HasAuthority())
		{
			MultiHandleDeath();
		}
		else
		{
			HandleDeath();
		}
	}
}

void ASpellRiseEnemyCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!HasAuthority() || !AbilitySystemComponent || IsEnemyDead())
	{
		return;
	}

	if (Data.NewValue > 0.0f)
	{
		return;
	}

	ApplyEnemyDeathState_Server();
}

void ASpellRiseEnemyCharacterBase::ApplyEnemyDeathState_Server()
{
	if (!HasAuthority() || !AbilitySystemComponent || IsEnemyDead())
	{
		return;
	}

	bIsDead = true;

	if (GE_Death)
	{
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GE_Death, 1.0f, Context);
		if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
		else if (DeadStateTag.IsValid())
		{
			AbilitySystemComponent->AddLooseGameplayTag(DeadStateTag);
		}
	}

	if (DeadStateTag.IsValid() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadStateTag))
	{
		AbilitySystemComponent->AddLooseGameplayTag(DeadStateTag);
	}

	AbilitySystemComponent->CancelAllAbilities();
	SpawnEnemyLootBag_Server();

	if (!DeadStateTag.IsValid())
	{
		MultiHandleDeath();
	}
}

void ASpellRiseEnemyCharacterBase::SpawnEnemyLootBag_Server()
{
	if (!HasAuthority() || bEnemyLootProcessedForCurrentDeath)
	{
		return;
	}

	bEnemyLootProcessedForCurrentDeath = true;

	if (EnemyLootTables.IsEmpty())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const TSubclassOf<AActor> LootBagClass = ResolveEnemyLootBagClass(EnemyLootBagClass);
	if (!LootBagClass)
	{
		UE_LOG(LogSpellRiseEnemyRuntime, Warning, TEXT("Enemy loot bag class nao resolvida. Enemy=%s"),
			*GetNameSafe(this));
		return;
	}

	const FVector SpawnLocation = ResolveEnemyLootBagSpawnLocation_Server();
	const FTransform SpawnTransform(GetActorRotation(), SpawnLocation, FVector::OneVector);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetController() ? Cast<AActor>(GetController()) : this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* LootBagActor = World->SpawnActor<AActor>(LootBagClass, SpawnTransform, SpawnParams);
	if (!LootBagActor)
	{
		UE_LOG(LogSpellRiseEnemyRuntime, Warning, TEXT("Falha ao spawnar loot bag. Enemy=%s BagClass=%s"),
			*GetNameSafe(this),
			*GetNameSafe(LootBagClass.Get()));
		return;
	}

	LootBagActor->SetReplicates(true);
	LootBagActor->SetReplicateMovement(true);
	LootBagActor->SetNetUpdateFrequency(FMath::Clamp(EnemyLootBagNetUpdateFrequency, 1.0f, 30.0f));
	StabilizeEnemyLootBagActor(LootBagActor);

	SetEnemyLootBagDisplayName(LootBagActor, GetEnemyDisplayName().ToString());

	UNarrativeInventoryComponent* LootBagInventory = UInventoryFunctionLibrary::GetInventoryComponentFromTarget(LootBagActor);
	if (!LootBagInventory)
	{
		LootBagInventory = LootBagActor->FindComponentByClass<UNarrativeInventoryComponent>();
	}

	if (!LootBagInventory)
	{
		UE_LOG(LogSpellRiseEnemyRuntime, Warning, TEXT("Loot bag sem NarrativeInventoryComponent. Enemy=%s Bag=%s"),
			*GetNameSafe(this),
			*GetNameSafe(LootBagActor));
		LootBagActor->Destroy();
		return;
	}

	TArray<FItemAddResult> AddResults;
	for (const FLootTableRoll& LootTable : EnemyLootTables)
	{
		LootBagInventory->TryAddFromLootTable(LootTable, AddResults);
	}

	int32 TotalItemsGiven = 0;
	for (const FItemAddResult& AddResult : AddResults)
	{
		TotalItemsGiven += FMath::Max(0, AddResult.AmountGiven);
	}

	if (TotalItemsGiven <= 0)
	{
		LootBagActor->Destroy();
		return;
	}

	LootBagActor->ForceNetUpdate();
	UE_LOG(LogSpellRiseEnemyRuntime, Verbose, TEXT("Enemy loot bag criada. Enemy=%s Bag=%s Items=%d"),
		*GetNameSafe(this),
		*GetNameSafe(LootBagActor),
		TotalItemsGiven);
}

FVector ASpellRiseEnemyCharacterBase::ResolveEnemyLootBagSpawnLocation_Server() const
{
	UWorld* World = GetWorld();
	const FVector FallbackLocation = GetActorLocation() + FVector(0.0f, 0.0f, 12.0f);
	if (!World)
	{
		return FallbackLocation;
	}

	const FVector TraceStart = GetActorLocation() + FVector(0.0f, 0.0f, 120.0f);
	const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, 5000.0f);

	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EnemyLootBagGroundTrace), false, this);
	if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams)
		|| World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams)
		|| World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldDynamic, QueryParams))
	{
		return Hit.ImpactPoint + FVector(0.0f, 0.0f, 8.0f);
	}

	return FallbackLocation;
}

void ASpellRiseEnemyCharacterBase::MultiHandleDeath_Implementation()
{
	HandleDeath();
}

void ASpellRiseEnemyCharacterBase::HandleDeath_Implementation()
{
	USkeletalMeshComponent* VisualMesh = GetMesh();
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	UCapsuleComponent* Capsule = GetCapsuleComponent();

	if (!VisualMesh || !Movement || !Capsule)
	{
		return;
	}

	if (HasAuthority())
	{
		for (const FGameplayAbilitySpecHandle& Handle : StartupGrantedAbilityHandles)
		{
			if (Handle.IsValid())
			{
				AbilitySystemComponent->ClearAbility(Handle);
			}
		}
		StartupGrantedAbilityHandles.Reset();
	}

	Movement->DisableMovement();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (!VisualMesh->IsSimulatingPhysics())
	{
		VisualMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		VisualMesh->SetSimulatePhysics(true);
	}
}

void ASpellRiseEnemyCharacterBase::MultiPlayHitReactionMontage_Implementation(float PlayRate)
{
	if (!HitReactionMontage)
	{
		return;
	}

	if (USkeletalMeshComponent* VisualMesh = GetMesh())
	{
		if (UAnimInstance* AnimInstance = VisualMesh->GetAnimInstance())
		{
			AnimInstance->Montage_Play(HitReactionMontage, FMath::Max(0.01f, PlayRate));
		}
	}
}

void ASpellRiseEnemyCharacterBase::MultiShowDamagePop_Implementation(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit)
{
	if (Damage <= 0.0f)
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

	PC->ShowDamageNumber(
		Damage,
		GetDamageNumberWorldLocation(),
		SourceTags,
		TargetTags,
		bIsCrit);

	BP_ShowDamagePop(Damage, InstigatorActor, DamageTypeTag, bIsCrit);
}

FVector ASpellRiseEnemyCharacterBase::GetDamageNumberWorldLocation() const
{
	const FVector Origin = GetActorLocation();
	if (const UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		return Origin + FVector(0.0f, 0.0f, Capsule->GetScaledCapsuleHalfHeight() * 0.65f);
	}

	return Origin;
}
