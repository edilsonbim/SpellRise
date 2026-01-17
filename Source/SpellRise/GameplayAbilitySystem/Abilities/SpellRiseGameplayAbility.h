// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SpellRiseGameplayAbility.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"),
	LMB	UMETA(DisplayName = "LMB"),
	RMB UMETA(DisplayName = "RMB"),
	Ability1 UMETA(DisplayName = "Ability1"),
	Ability2 UMETA(DisplayName = "Ability2"),
	Ability3 UMETA(DisplayName = "Ability3"),
	Ability4 UMETA(DisplayName = "Ability4"),
	Ability5 UMETA(DisplayName = "Ability5"),
	Ability6 UMETA(DisplayName = "Ability6"),
	Ability7 UMETA(DisplayName = "Ability7"),
	Ability8 UMETA(DisplayName = "Ability8")
};

UCLASS()
class SPELLRISE_API USpellRiseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	USpellRiseGameplayAbility();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool ShouldShowInAbilityBar = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;
	
private:
	UFUNCTION(BlueprintCallable, Category = "Helpers")
	bool HasPC() const;
};
