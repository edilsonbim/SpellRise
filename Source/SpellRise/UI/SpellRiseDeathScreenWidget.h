#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpellRiseDeathScreenWidget.generated.h"

UCLASS()
class SPELLRISE_API USpellRiseDeathScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="SpellRise|UI|Death")
	void SetMessage(const FText& InMessage);

protected:
	virtual void NativeConstruct() override;
	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

private:
	UPROPERTY(EditAnywhere, Category="SpellRise|UI|Death")
	FLinearColor TextColor = FLinearColor(0.45f, 0.02f, 0.02f, 1.f);

	UPROPERTY(EditAnywhere, Category="SpellRise|UI|Death", meta=(ClampMin="16", ClampMax="120"))
	int32 FontSize = 60;

	UPROPERTY(EditAnywhere, Category="SpellRise|UI|Death")
	FSoftObjectPath PreferredFontObjectPath = FSoftObjectPath(TEXT("/NarrativeCommonUI/Fonts/RobotoCondensed-Bold.RobotoCondensed-Bold"));

	UPROPERTY(Transient)
	FText Message;
};

