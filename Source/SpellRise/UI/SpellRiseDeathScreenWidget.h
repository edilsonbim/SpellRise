#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpellRise/Characters/SpellRiseLifeStateComponent.h"
#include "SpellRiseDeathScreenWidget.generated.h"

class UTextBlock;

UCLASS()
class SPELLRISE_API USpellRiseDeathScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="SpellRise|UI|Death")
	void SetMessage(const FText& InMessage);

	UFUNCTION(BlueprintCallable, Category="SpellRise|UI|Death")
	void RequestAcceptDeath();

	UFUNCTION(BlueprintCallable, Category="SpellRise|UI|Death")
	void ConfigureDownedAction(class ASpellRiseCharacterBase* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category="SpellRise|UI|Death")
	void RequestDownedAction(bool bRevive);

	UFUNCTION()
	void HandleLifeStateChanged(ESpellRiseLifeState NewState, ESpellRiseLifeState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|UI|Death|Presentation")
	void OnShowDownedActions(ASpellRiseCharacterBase* TargetCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|UI|Death|Presentation")
	void OnHideDownedActions();

	UFUNCTION(BlueprintImplementableEvent, Category="SpellRise|UI|Death|Presentation")
	void OnLifeStateChanged(ESpellRiseLifeState NewState, ESpellRiseLifeState OldState);

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void HandleReviveAction();
	void HandleGankAction();

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_InteractableName = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_YesAction = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_NoAction = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<class ASpellRiseCharacterBase> DownedActionTarget = nullptr;

	UPROPERTY(Transient)
	FText Message;
};
