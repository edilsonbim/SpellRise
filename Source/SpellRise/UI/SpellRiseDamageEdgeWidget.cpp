// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/UI/SpellRiseDamageEdgeWidget.h"

#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

void USpellRiseDamageEdgeWidget::PlayDamageFlash(float InPeakAlpha, float InFadeInDuration, float InFadeOutDuration)
{
	PeakAlpha = FMath::Clamp(InPeakAlpha, 0.f, 1.f);
	FadeInDuration = FMath::Max(0.f, InFadeInDuration);
	FadeOutDuration = FMath::Max(0.f, InFadeOutDuration);
	Elapsed = 0.f;
	CurrentAlpha = 0.f;
	bFlashActive = PeakAlpha > KINDA_SMALL_NUMBER;
}

void USpellRiseDamageEdgeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetIsEnabled(true);
}

void USpellRiseDamageEdgeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bFlashActive)
	{
		CurrentAlpha = 0.f;
		return;
	}

	Elapsed += FMath::Max(0.f, InDeltaTime);
	const float InDuration = FMath::Max(KINDA_SMALL_NUMBER, FadeInDuration);
	const float OutDuration = FMath::Max(KINDA_SMALL_NUMBER, FadeOutDuration);

	if (Elapsed <= InDuration)
	{
		CurrentAlpha = PeakAlpha * (Elapsed / InDuration);
	}
	else
	{
		const float FadeOutElapsed = Elapsed - InDuration;
		const float FadeOutT = FMath::Clamp(FadeOutElapsed / OutDuration, 0.f, 1.f);
		CurrentAlpha = PeakAlpha * (1.f - FadeOutT);
		if (FadeOutT >= 1.f)
		{
			CurrentAlpha = 0.f;
			bFlashActive = false;
		}
	}

	Invalidate(EInvalidateWidget::Paint);
}

int32 USpellRiseDamageEdgeWidget::NativePaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	const int32 ResultLayerId = Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);

	if (CurrentAlpha <= KINDA_SMALL_NUMBER)
	{
		return ResultLayerId;
	}

	const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
	const float MinDimension = FMath::Min(LocalSize.X, LocalSize.Y);
	const float MaxThickness = FMath::Clamp(EdgeThicknessFraction, 0.01f, 0.45f) * MinDimension;
	const int32 Steps = FMath::Clamp(GradientSteps, 1, 32);
	const float BandThickness = MaxThickness / static_cast<float>(Steps);
	const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush(TEXT("WhiteBrush"));

	int32 MaxLayer = ResultLayerId;
	for (int32 Step = 0; Step < Steps; ++Step)
	{
		const float StepT = 1.f - (static_cast<float>(Step) / static_cast<float>(Steps));
		const float StepAlpha = CurrentAlpha * StepT * StepT;
		if (StepAlpha <= KINDA_SMALL_NUMBER)
		{
			continue;
		}

		const float Inset = BandThickness * static_cast<float>(Step);
		const FLinearColor StepColor(EdgeColor.R, EdgeColor.G, EdgeColor.B, StepAlpha);
		const int32 DrawLayer = ResultLayerId + 1 + Step;

		const FVector2D TopPos(Inset, Inset);
		const FVector2D TopSize(FMath::Max(0.f, LocalSize.X - (2.f * Inset)), BandThickness);
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			DrawLayer,
			AllottedGeometry.ToPaintGeometry(
				FVector2f(TopSize),
				FSlateLayoutTransform(FVector2f(TopPos))),
			WhiteBrush,
			ESlateDrawEffect::None,
			StepColor);

		const FVector2D BottomPos(Inset, FMath::Max(0.f, LocalSize.Y - Inset - BandThickness));
		const FVector2D BottomSize(FMath::Max(0.f, LocalSize.X - (2.f * Inset)), BandThickness);
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			DrawLayer,
			AllottedGeometry.ToPaintGeometry(
				FVector2f(BottomSize),
				FSlateLayoutTransform(FVector2f(BottomPos))),
			WhiteBrush,
			ESlateDrawEffect::None,
			StepColor);

		const FVector2D LeftPos(Inset, Inset + BandThickness);
		const FVector2D LeftSize(BandThickness, FMath::Max(0.f, LocalSize.Y - (2.f * Inset) - (2.f * BandThickness)));
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			DrawLayer,
			AllottedGeometry.ToPaintGeometry(
				FVector2f(LeftSize),
				FSlateLayoutTransform(FVector2f(LeftPos))),
			WhiteBrush,
			ESlateDrawEffect::None,
			StepColor);

		const FVector2D RightPos(FMath::Max(0.f, LocalSize.X - Inset - BandThickness), Inset + BandThickness);
		const FVector2D RightSize(BandThickness, FMath::Max(0.f, LocalSize.Y - (2.f * Inset) - (2.f * BandThickness)));
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			DrawLayer,
			AllottedGeometry.ToPaintGeometry(
				FVector2f(RightSize),
				FSlateLayoutTransform(FVector2f(RightPos))),
			WhiteBrush,
			ESlateDrawEffect::None,
			StepColor);

		MaxLayer = FMath::Max(MaxLayer, DrawLayer);
	}

	return MaxLayer;
}
