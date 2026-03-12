#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityTagRelationshipMapping.h"
#include "SpellRiseAbilitySystemComponent.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class USpellRiseGameplayAbility;
class USpellRiseAbilityTagRelationshipMapping;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:
	// Cache para detectar mudanças em abilities replicadas e disparar evento no Character
	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpec> LastActivatableAbilities;

	// =========================================================
	// Input pipeline por Gameplay Tag (custom, estilo Lyra)
	// =========================================================
	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

public:
	USpellRiseAbilitySystemComponent();

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_ActivateAbilities() override;

	// Hooks chamados internamente
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

public:
	// =========================================================
	// Input public API
	// =========================================================
	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_AbilityInputTagPressed(FGameplayTag InputTag);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_AbilityInputTagReleased(FGameplayTag InputTag);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_ClearAbilityInput();

	// ---------------------------------------------------------
	// Ability Wheel helpers
	// ---------------------------------------------------------
	/** Activate a primeira ability spec que combinar com o InputTag. */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	bool SR_TryActivateAbilityByInputTag(FGameplayTag InputTag);

	/** Retorna a SpellRiseGameplayAbility CDO da primeira spec que combinar com o InputTag. */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	USpellRiseGameplayAbility* SR_GetSpellRiseAbilityForInputTag(FGameplayTag InputTag) const;

	// =========================================================
	// Tag relationship mapping
	// =========================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|GAS|Tags")
	TObjectPtr<USpellRiseAbilityTagRelationshipMapping> TagRelationshipMapping;

	/** Sets the current tag relationship mapping. Passing nullptr clears it. */
	void SetTagRelationshipMapping(USpellRiseAbilityTagRelationshipMapping* NewMapping);

	/**
	 * Given a set of ability tags, append additional required and blocked activation tags
	 * based on the tag relationship mapping.
	 */
	void GetAdditionalActivationTagRequirements(
		const FGameplayTagContainer& AbilityTags,
		FGameplayTagContainer& OutActivationRequired,
		FGameplayTagContainer& OutActivationBlocked) const;

protected:
	/**
	 * Override of UAbilitySystemComponent::ApplyAbilityBlockAndCancelTags to apply tag relationship
	 * rules before applying the incoming block and cancel tags.
	 */
	virtual void ApplyAbilityBlockAndCancelTags(
		const FGameplayTagContainer& AbilityTags,
		UGameplayAbility* RequestingAbility,
		bool bEnableBlockTags,
		const FGameplayTagContainer& BlockTags,
		bool bExecuteCancelTags,
		const FGameplayTagContainer& CancelTags) override;

	bool AbilitySpecMatchesInputTag(const FGameplayAbilitySpec& Spec, const FGameplayTag& InputTag) const;
	void GetAbilitySpecsFromInputTag(const FGameplayTag& InputTag, TArray<FGameplayAbilitySpecHandle>& OutSpecHandles) const;

	void MarkSpecInputPressed(FGameplayAbilitySpec& Spec);
	void MarkSpecInputReleased(FGameplayAbilitySpec& Spec);
};