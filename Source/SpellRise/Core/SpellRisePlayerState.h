#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "SpellRisePlayerState.generated.h"

class UAbilitySystemComponent;
class USpellRiseAbilitySystemComponent;
class UBasicAttributeSet;
class UCombatAttributeSet;
class UResourceAttributeSet;
class UCatalystAttributeSet;
class UDerivedStatsAttributeSet;

UCLASS()
class SPELLRISE_API ASpellRisePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASpellRisePlayerState();

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	USpellRiseAbilitySystemComponent* GetSpellRiseASC() const;

	UBasicAttributeSet* GetBasicAttributeSet() const { return BasicAttributeSet; }
	UCombatAttributeSet* GetCombatAttributeSet() const { return CombatAttributeSet; }
	UResourceAttributeSet* GetResourceAttributeSet() const { return ResourceAttributeSet; }
	UCatalystAttributeSet* GetCatalystAttributeSet() const { return CatalystAttributeSet; }
	UDerivedStatsAttributeSet* GetDerivedStatsAttributeSet() const { return DerivedStatsAttributeSet; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<USpellRiseAbilitySystemComponent> AbilitySystemComponent = nullptr;

	// Attribute sets live on PlayerState (authoritative GAS owner)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBasicAttributeSet> BasicAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCombatAttributeSet> CombatAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UResourceAttributeSet> ResourceAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCatalystAttributeSet> CatalystAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UDerivedStatsAttributeSet> DerivedStatsAttributeSet = nullptr;
};
