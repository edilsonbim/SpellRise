#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "SpellRisePlayerHUDViewModelComponent.generated.h"

struct FOnAttributeChangeData;
struct FSpellRiseCharacterProgressionSnapshot;

USTRUCT(BlueprintType)
struct SPELLRISE_API FSpellRisePlayerHUDSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD")
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Progression")
	int32 Level = 1;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Progression")
	int32 Experience = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Progression")
	int32 ExperienceRequiredForNextLevel = 0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Progression")
	float ExperienceProgress = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Attributes")
	float Strength = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Attributes")
	float Agility = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Attributes")
	float Intelligence = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Attributes")
	float Wisdom = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Resources")
	float Health = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Resources")
	float MaxHealth = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Resources")
	float Mana = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Resources")
	float MaxMana = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Resources")
	float Stamina = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD|Resources")
	float MaxStamina = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FSpellRisePlayerHUDSnapshotChangedSignature,
	const FSpellRisePlayerHUDSnapshot&,
	Snapshot);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FSpellRiseHUDSelectedAbilityChangedSignature,
	FGameplayTag,
	NewTag,
	FGameplayTag,
	OldTag);

UCLASS(ClassGroup=(SpellRise), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRisePlayerHUDViewModelComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRisePlayerHUDViewModelComponent();

	UFUNCTION(BlueprintPure, Category="SpellRise|HUD")
	FSpellRisePlayerHUDSnapshot GetCurrentSnapshot() const { return CurrentSnapshot; }

	UFUNCTION(BlueprintCallable, Category="SpellRise|HUD")
	void RefreshSnapshot();

	UPROPERTY(BlueprintAssignable, Category="SpellRise|HUD|Events")
	FSpellRisePlayerHUDSnapshotChangedSignature OnHUDSnapshotChanged;

	UFUNCTION(BlueprintPure, Category="SpellRise|HUD|Abilities")
	FGameplayTag GetSelectedAbilityInputTag() const;

	UPROPERTY(BlueprintAssignable, Category="SpellRise|HUD|Abilities|Events")
	FSpellRiseHUDSelectedAbilityChangedSignature OnSelectedAbilityChanged;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void BindAttributeDelegates();
	void UnbindAttributeDelegates();
	FSpellRisePlayerHUDSnapshot BuildSnapshot() const;

	UFUNCTION()
	void HandleCharacterProgressionChanged(const FSpellRiseCharacterProgressionSnapshot& Snapshot);

	UFUNCTION()
	void HandleSelectedAbilityChanged(FGameplayTag NewTag, FGameplayTag OldTag);

	void HandleAttributeChanged(const FOnAttributeChangeData& ChangeData);

	UPROPERTY(Transient)
	FSpellRisePlayerHUDSnapshot CurrentSnapshot;

	TArray<FDelegateHandle> AttributeDelegateHandles;
};
