#include "SpellRise/UI/SpellRisePlayerHUDViewModelComponent.h"

#include "AbilitySystemComponent.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/Progression/SpellRiseProgressionComponent.h"

USpellRisePlayerHUDViewModelComponent::USpellRisePlayerHUDViewModelComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void USpellRisePlayerHUDViewModelComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ASpellRisePlayerState* PlayerState = Cast<ASpellRisePlayerState>(GetOwner()))
	{
		if (USpellRiseProgressionComponent* Progression = PlayerState->GetProgressionComponent())
		{
			Progression->OnCharacterProgressionChanged.AddUniqueDynamic(
				this,
				&USpellRisePlayerHUDViewModelComponent::HandleCharacterProgressionChanged);
		}
	}

	BindAttributeDelegates();
	RefreshSnapshot();
}

void USpellRisePlayerHUDViewModelComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASpellRisePlayerState* PlayerState = Cast<ASpellRisePlayerState>(GetOwner()))
	{
		if (USpellRiseProgressionComponent* Progression = PlayerState->GetProgressionComponent())
		{
			Progression->OnCharacterProgressionChanged.RemoveDynamic(
				this,
				&USpellRisePlayerHUDViewModelComponent::HandleCharacterProgressionChanged);
		}
	}

	UnbindAttributeDelegates();
	Super::EndPlay(EndPlayReason);
}

void USpellRisePlayerHUDViewModelComponent::RefreshSnapshot()
{
	CurrentSnapshot = BuildSnapshot();
	OnHUDSnapshotChanged.Broadcast(CurrentSnapshot);
}

FSpellRisePlayerHUDSnapshot USpellRisePlayerHUDViewModelComponent::BuildSnapshot() const
{
	FSpellRisePlayerHUDSnapshot Snapshot;
	const ASpellRisePlayerState* PlayerState = Cast<ASpellRisePlayerState>(GetOwner());
	if (!PlayerState)
	{
		return Snapshot;
	}

	Snapshot.DisplayName = PlayerState->GetPlayerName();

	if (const USpellRiseProgressionComponent* Progression = PlayerState->GetProgressionComponent())
	{
		Snapshot.Level = Progression->GetCharacterLevel();
		Snapshot.Experience = Progression->GetExperience();
		Snapshot.ExperienceRequiredForNextLevel = Progression->GetExperienceRequiredForNextLevel();

		const int32 CurrentLevelExperience = Progression->GetExperienceRequiredForLevel(Snapshot.Level);
		const int32 ExperienceWindow = Snapshot.ExperienceRequiredForNextLevel - CurrentLevelExperience;
		Snapshot.ExperienceProgress = ExperienceWindow > 0
			? FMath::Clamp(
				static_cast<float>(Snapshot.Experience - CurrentLevelExperience) / static_cast<float>(ExperienceWindow),
				0.0f,
				1.0f)
			: 1.0f;
	}

	if (const UBasicAttributeSet* BasicAttributes = PlayerState->GetBasicAttributeSet())
	{
		Snapshot.Strength = BasicAttributes->GetStrength();
		Snapshot.Agility = BasicAttributes->GetAgility();
		Snapshot.Intelligence = BasicAttributes->GetIntelligence();
		Snapshot.Wisdom = BasicAttributes->GetWisdom();
	}

	if (const UResourceAttributeSet* Resources = PlayerState->GetResourceAttributeSet())
	{
		Snapshot.Health = Resources->GetHealth();
		Snapshot.MaxHealth = Resources->GetMaxHealth();
		Snapshot.Mana = Resources->GetMana();
		Snapshot.MaxMana = Resources->GetMaxMana();
		Snapshot.Stamina = Resources->GetStamina();
		Snapshot.MaxStamina = Resources->GetMaxStamina();
	}

	return Snapshot;
}

void USpellRisePlayerHUDViewModelComponent::BindAttributeDelegates()
{
	ASpellRisePlayerState* PlayerState = Cast<ASpellRisePlayerState>(GetOwner());
	UAbilitySystemComponent* ASC = PlayerState ? PlayerState->GetAbilitySystemComponent() : nullptr;
	if (!ASC || AttributeDelegateHandles.Num() > 0)
	{
		return;
	}

	const FGameplayAttribute Attributes[] =
	{
		UBasicAttributeSet::GetStrengthAttribute(),
		UBasicAttributeSet::GetAgilityAttribute(),
		UBasicAttributeSet::GetIntelligenceAttribute(),
		UBasicAttributeSet::GetWisdomAttribute(),
		UResourceAttributeSet::GetHealthAttribute(),
		UResourceAttributeSet::GetMaxHealthAttribute(),
		UResourceAttributeSet::GetManaAttribute(),
		UResourceAttributeSet::GetMaxManaAttribute(),
		UResourceAttributeSet::GetStaminaAttribute(),
		UResourceAttributeSet::GetMaxStaminaAttribute()
	};

	AttributeDelegateHandles.Reserve(UE_ARRAY_COUNT(Attributes));
	for (const FGameplayAttribute& Attribute : Attributes)
	{
		AttributeDelegateHandles.Add(
			ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(
				this,
				&USpellRisePlayerHUDViewModelComponent::HandleAttributeChanged));
	}
}

void USpellRisePlayerHUDViewModelComponent::UnbindAttributeDelegates()
{
	ASpellRisePlayerState* PlayerState = Cast<ASpellRisePlayerState>(GetOwner());
	UAbilitySystemComponent* ASC = PlayerState ? PlayerState->GetAbilitySystemComponent() : nullptr;
	if (!ASC)
	{
		AttributeDelegateHandles.Reset();
		return;
	}

	const FGameplayAttribute Attributes[] =
	{
		UBasicAttributeSet::GetStrengthAttribute(),
		UBasicAttributeSet::GetAgilityAttribute(),
		UBasicAttributeSet::GetIntelligenceAttribute(),
		UBasicAttributeSet::GetWisdomAttribute(),
		UResourceAttributeSet::GetHealthAttribute(),
		UResourceAttributeSet::GetMaxHealthAttribute(),
		UResourceAttributeSet::GetManaAttribute(),
		UResourceAttributeSet::GetMaxManaAttribute(),
		UResourceAttributeSet::GetStaminaAttribute(),
		UResourceAttributeSet::GetMaxStaminaAttribute()
	};

	const int32 HandleCount = FMath::Min(AttributeDelegateHandles.Num(), static_cast<int32>(UE_ARRAY_COUNT(Attributes)));
	for (int32 Index = 0; Index < HandleCount; ++Index)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Attributes[Index]).Remove(AttributeDelegateHandles[Index]);
	}
	AttributeDelegateHandles.Reset();
}

void USpellRisePlayerHUDViewModelComponent::HandleCharacterProgressionChanged(
	const FSpellRiseCharacterProgressionSnapshot& /*Snapshot*/)
{
	RefreshSnapshot();
}

void USpellRisePlayerHUDViewModelComponent::HandleAttributeChanged(const FOnAttributeChangeData& /*ChangeData*/)
{
	RefreshSnapshot();
}
