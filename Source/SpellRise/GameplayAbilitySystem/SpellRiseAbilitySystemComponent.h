#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "SpellRiseAbilitySystemComponent.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class USpellRiseGameplayAbility;
struct FGameplayEffectSpec;
struct FActiveGameplayEffect;
struct FActiveGameplayEffectHandle;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpec> LastActivatableAbilities;

	UPROPERTY(Transient)
	FGameplayAbilitySpecHandle SelectedSpellSpecHandle;




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
	virtual void NotifyAbilityCommit(UGameplayAbility* Ability) override;
	virtual void NotifyAbilityActivated(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	void OnGameplayEffectAppliedToSelf(UAbilitySystemComponent* SourceASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle ActiveHandle);
	void OnActiveGameplayEffectAddedToSelf(UAbilitySystemComponent* SourceASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle ActiveHandle);
	void OnGameplayEffectRemovedFromSelf(const FActiveGameplayEffect& ActiveEffect);
	void BroadcastEquipmentAbilityStateChanged() const;

public:



	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_AbilityInputTagPressed(FGameplayTag InputTag);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_AbilityInputTagReleased(FGameplayTag InputTag);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_AbilityInputClassPressed(TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_AbilityInputClassReleased(TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_ClearAbilityInput();





	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	bool SR_TryActivateAbilityByInputTag(FGameplayTag InputTag);


	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	USpellRiseGameplayAbility* SR_GetSpellRiseAbilityForInputTag(FGameplayTag InputTag) const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	USpellRiseGameplayAbility* SR_GetSelectedSpellAbility() const;

	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	void SR_ClearSelectedSpellAbility();


	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	void SR_SetSelectedSpellAbilityByInputTag(FGameplayTag InputTag);

	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	bool SR_IsSelectedSpellAbilityHandle(FGameplayAbilitySpecHandle AbilityHandle) const;


protected:

	bool AbilitySpecMatchesInputTag(const FGameplayAbilitySpec& Spec, const FGameplayTag& InputTag) const;
	void GetAbilitySpecsFromInputTag(const FGameplayTag& InputTag, TArray<FGameplayAbilitySpecHandle>& OutSpecHandles) const;
	void GetAbilitySpecsFromAbilityClass(TSubclassOf<UGameplayAbility> AbilityClass, TArray<FGameplayAbilitySpecHandle>& OutSpecHandles) const;
	void RecordAbilityActivationFailure(const FGameplayAbilitySpec& Spec, const FGameplayTagContainer& FailureTags, const TCHAR* Context);

	void MarkSpecInputPressed(FGameplayAbilitySpec& Spec);
	void MarkSpecInputReleased(FGameplayAbilitySpec& Spec);

	UPROPERTY(Transient)
	TMap<FString, int32> AbilityActivationFailureCountByReason;
};
