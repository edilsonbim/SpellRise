#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpellRiseDamageEdgeWidget.generated.h"

UCLASS()
class SPELLRISE_API USpellRiseDamageEdgeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="SpellRise|UI|Damage")
	void PlayDamageFlash(float InPeakAlpha, float InFadeInDuration, float InFadeOutDuration);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

private:
	UPROPERTY(EditAnywhere, Category="SpellRise|UI|Damage", meta=(ClampMin="0.01", ClampMax="0.45"))
	float EdgeThicknessFraction = 0.14f;

	UPROPERTY(EditAnywhere, Category="SpellRise|UI|Damage", meta=(ClampMin="1", ClampMax="32"))
	int32 GradientSteps = 10;

	UPROPERTY(EditAnywhere, Category="SpellRise|UI|Damage")
	FLinearColor EdgeColor = FLinearColor(0.85f, 0.05f, 0.05f, 1.f);

	UPROPERTY(Transient)
	float PeakAlpha = 0.30f;

	UPROPERTY(Transient)
	float FadeInDuration = 0.06f;

	UPROPERTY(Transient)
	float FadeOutDuration = 0.28f;

	UPROPERTY(Transient)
	float CurrentAlpha = 0.f;

	UPROPERTY(Transient)
	float Elapsed = 0.f;

	UPROPERTY(Transient)
	bool bFlashActive = false;
};

