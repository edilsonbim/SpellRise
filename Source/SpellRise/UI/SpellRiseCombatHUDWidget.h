#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SpellRise/Core/SpellRiseChatTypes.h"
#include "SpellRiseCombatHUDWidget.generated.h"

USTRUCT(BlueprintType)
struct FSpellRiseAbilitySlotView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD")
	FGameplayTag InputTag;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD")
	FName AbilityName = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD")
	float CooldownRemaining = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD")
	float CooldownDuration = 0.f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD")
	bool bIsActive = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD")
	bool bIsOnCooldown = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|HUD")
	bool bIsSelected = false;
};

UCLASS(Blueprintable, Abstract)
class SPELLRISE_API USpellRiseCombatHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|HUD")
	void BP_OnResourcesUpdated(
		float NewHealth,
		float MaxHealth,
		float NewMana,
		float MaxMana,
		float NewStamina,
		float MaxStamina);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|HUD")
	void BP_OnAbilitySlotsUpdated(const TArray<FSpellRiseAbilitySlotView>& Slots);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|HUD")
	void BP_OnTargetUpdated(bool bHasTarget, AActor* TargetActor, float Distance);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|HUD")
	void BP_OnCombatFeedMessage(const FSpellRiseChatMessage& Message);
};
