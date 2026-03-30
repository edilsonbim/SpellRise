#include "SpellRise/UI/SpellRiseDeathScreenWidget.h"

#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "UObject/SoftObjectPath.h"

void USpellRiseDeathScreenWidget::SetMessage(const FText& InMessage)
{
	Message = InMessage;
	Invalidate(EInvalidateWidget::Paint);
}

void USpellRiseDeathScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetIsEnabled(true);

	if (Message.IsEmpty())
	{
		Message = FText::FromString(TEXT("You are dead."));
	}
}

int32 USpellRiseDeathScreenWidget::NativePaint(
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

	if (Message.IsEmpty())
	{
		return ResultLayerId;
	}

	FSlateFontInfo FontInfo = FCoreStyle::GetDefaultFontStyle(TEXT("Bold"), FontSize);
	if (PreferredFontObjectPath.IsValid())
	{
		if (UObject* FontObject = PreferredFontObjectPath.TryLoad())
		{
			FontInfo.FontObject = FontObject;
			FontInfo.TypefaceFontName = TEXT("Regular");
		}
	}
	FVector2D TextSize(320.f, 60.f);
	if (FSlateApplication::IsInitialized())
	{
		if (FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer())
		{
			const TSharedRef<FSlateFontMeasure> FontMeasure = Renderer->GetFontMeasureService();
			TextSize = FontMeasure->Measure(Message, FontInfo);
		}
	}

	const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
	const FVector2D DrawPosition(
		FMath::Max(0.f, (LocalSize.X - TextSize.X) * 0.5f),
		FMath::Max(0.f, (LocalSize.Y - TextSize.Y) * 0.5f));

	const FLinearColor ShadowColor(0.f, 0.f, 0.f, 0.8f);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		ResultLayerId + 1,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(LocalSize),
			FSlateLayoutTransform(FVector2f(DrawPosition + FVector2D(2.f, 2.f)))),
		Message,
		FontInfo,
		ESlateDrawEffect::None,
		ShadowColor);

	FSlateDrawElement::MakeText(
		OutDrawElements,
		ResultLayerId + 2,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(LocalSize),
			FSlateLayoutTransform(FVector2f(DrawPosition))),
		Message,
		FontInfo,
		ESlateDrawEffect::None,
		TextColor);

	return ResultLayerId + 2;
}
