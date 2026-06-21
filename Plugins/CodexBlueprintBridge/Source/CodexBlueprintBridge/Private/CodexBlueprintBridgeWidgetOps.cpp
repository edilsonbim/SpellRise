#include "CodexBlueprintBridgeWidgetOps.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Widget.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ScopedTransaction.h"
#include "WidgetBlueprint.h"

namespace
{
FString GetStringField(const TSharedRef<FJsonObject>& Request, const TCHAR* Field)
{
    FString Value;
    Request->TryGetStringField(Field, Value);
    return Value;
}

UClass* ResolveWidgetClass(const FString& ClassName)
{
    if (ClassName == TEXT("Button"))
    {
        return UButton::StaticClass();
    }
    if (ClassName == TEXT("TextBlock"))
    {
        return UTextBlock::StaticClass();
    }
    if (ClassName == TEXT("Image"))
    {
        return UImage::StaticClass();
    }
    if (ClassName == TEXT("Border"))
    {
        return UBorder::StaticClass();
    }
    if (ClassName == TEXT("Overlay"))
    {
        return UOverlay::StaticClass();
    }
    if (ClassName == TEXT("VerticalBox"))
    {
        return UVerticalBox::StaticClass();
    }
    if (ClassName == TEXT("HorizontalBox"))
    {
        return UHorizontalBox::StaticClass();
    }
    if (ClassName == TEXT("CanvasPanel"))
    {
        return UCanvasPanel::StaticClass();
    }
    return nullptr;
}

FString VisibilityToString(const ESlateVisibility Visibility)
{
    if (const UEnum* Enum = StaticEnum<ESlateVisibility>())
    {
        return Enum->GetNameStringByValue(static_cast<int64>(Visibility));
    }
    return TEXT("Unknown");
}

bool ParseVisibility(const FString& Value, ESlateVisibility& OutVisibility)
{
    const UEnum* Enum = StaticEnum<ESlateVisibility>();
    if (!Enum)
    {
        return false;
    }

    const int64 EnumValue = Enum->GetValueByNameString(Value);
    if (EnumValue == INDEX_NONE)
    {
        return false;
    }

    OutVisibility = static_cast<ESlateVisibility>(EnumValue);
    return true;
}

TSharedRef<FJsonObject> DescribeWidget(const UWidget* Widget, const FString& ParentName)
{
    const TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
    Item->SetStringField(TEXT("name"), Widget->GetName());
    Item->SetStringField(TEXT("class"), Widget->GetClass()->GetName());
    Item->SetStringField(TEXT("parent"), ParentName);
    Item->SetBoolField(TEXT("isVariable"), Widget->bIsVariable);
    Item->SetBoolField(TEXT("isEnabled"), Widget->GetIsEnabled());
    Item->SetStringField(TEXT("visibility"), VisibilityToString(Widget->GetVisibility()));

    if (const UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
    {
        Item->SetStringField(TEXT("text"), TextBlock->GetText().ToString());
    }

    TArray<TSharedPtr<FJsonValue>> Children;
    if (const UPanelWidget* Panel = Cast<UPanelWidget>(Widget))
    {
        for (int32 ChildIndex = 0; ChildIndex < Panel->GetChildrenCount(); ++ChildIndex)
        {
            if (const UWidget* Child = Panel->GetChildAt(ChildIndex))
            {
                Children.Add(MakeShared<FJsonValueObject>(DescribeWidget(Child, Widget->GetName())));
            }
        }
    }
    Item->SetArrayField(TEXT("children"), Children);
    return Item;
}

bool ReadLinearColor(const TSharedRef<FJsonObject>& Request, FLinearColor& OutColor)
{
    const TSharedPtr<FJsonObject>* Value = nullptr;
    if (!Request->TryGetObjectField(TEXT("value"), Value) || !Value || !Value->IsValid())
    {
        return false;
    }

    double R = 0.0;
    double G = 0.0;
    double B = 0.0;
    double A = 1.0;
    if (!(*Value)->TryGetNumberField(TEXT("r"), R)
        || !(*Value)->TryGetNumberField(TEXT("g"), G)
        || !(*Value)->TryGetNumberField(TEXT("b"), B))
    {
        return false;
    }
    (*Value)->TryGetNumberField(TEXT("a"), A);

    OutColor = FLinearColor(
        static_cast<float>(R),
        static_cast<float>(G),
        static_cast<float>(B),
        static_cast<float>(A));
    return true;
}
}

bool FCodexBlueprintBridgeWidgetOps::DescribeWidgetTree(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UWidgetBlueprint* Blueprint = LoadWidgetBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    Result.Data->SetStringField(TEXT("asset"), GetStringField(Request, TEXT("asset")));
    Result.Data->SetStringField(
        TEXT("root"),
        Blueprint->WidgetTree && Blueprint->WidgetTree->RootWidget
            ? Blueprint->WidgetTree->RootWidget->GetName()
            : TEXT(""));

    TArray<TSharedPtr<FJsonValue>> Widgets;
    if (Blueprint->WidgetTree && Blueprint->WidgetTree->RootWidget)
    {
        Widgets.Add(MakeShared<FJsonValueObject>(
            DescribeWidget(Blueprint->WidgetTree->RootWidget, TEXT(""))));
    }
    Result.Data->SetArrayField(TEXT("widgets"), Widgets);
    return true;
}

bool FCodexBlueprintBridgeWidgetOps::AddWidget(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UWidgetBlueprint* Blueprint = LoadWidgetBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    const FString Name = GetStringField(Request, TEXT("name"));
    const FString ClassName = GetStringField(Request, TEXT("class"));
    const FString ParentName = GetStringField(Request, TEXT("parent"));
    if (!IsValidWidgetName(Name))
    {
        Result.Errors.Add({TEXT("InvalidWidgetName"), TEXT("Widget name must be a valid identifier."), TEXT("name")});
        return false;
    }
    if (FindWidget(Blueprint, Name))
    {
        Result.Errors.Add({TEXT("WidgetAlreadyExists"), TEXT("Widget tree already contains this name."), TEXT("name")});
        return false;
    }

    UClass* WidgetClass = ResolveWidgetClass(ClassName);
    if (!WidgetClass)
    {
        Result.Errors.Add({TEXT("UnsupportedWidgetClass"), TEXT("Widget class is not in the whitelist."), TEXT("class")});
        return false;
    }

    UPanelWidget* Parent = nullptr;
    if (!ParentName.IsEmpty())
    {
        Parent = Cast<UPanelWidget>(FindWidget(Blueprint, ParentName));
        if (!Parent)
        {
            Result.Errors.Add({TEXT("InvalidWidgetParent"), TEXT("Parent was not found or cannot contain children."), TEXT("parent")});
            return false;
        }
    }
    else if (Blueprint->WidgetTree->RootWidget)
    {
        Result.Errors.Add({TEXT("MissingWidgetParent"), TEXT("A parent is required when the tree already has a root."), TEXT("parent")});
        return false;
    }

    const FScopedTransaction Transaction(
        NSLOCTEXT("CodexBlueprintBridge", "AddWidget", "Add Widget"));
    Blueprint->Modify();
    Blueprint->WidgetTree->Modify();
    UWidget* Widget = Blueprint->WidgetTree->ConstructWidget<UWidget>(WidgetClass, FName(*Name));
    if (!Widget)
    {
        Result.Errors.Add({TEXT("AddWidgetFailed"), TEXT("WidgetTree failed to construct the widget."), TEXT("class")});
        return false;
    }

    if (Parent)
    {
        Parent->Modify();
        if (!Parent->AddChild(Widget))
        {
            Blueprint->WidgetTree->RemoveWidget(Widget);
            Result.Errors.Add({TEXT("AddWidgetFailed"), TEXT("Parent rejected the child widget."), TEXT("parent")});
            return false;
        }
    }
    else
    {
        Blueprint->WidgetTree->RootWidget = Widget;
    }

    bool bIsVariable = true;
    Request->TryGetBoolField(TEXT("isVariable"), bIsVariable);
    Widget->bIsVariable = bIsVariable;
    MarkModified(Blueprint, true);

    Result.Data->SetStringField(TEXT("asset"), GetStringField(Request, TEXT("asset")));
    Result.Data->SetStringField(TEXT("widget"), Name);
    Result.Data->SetStringField(TEXT("class"), ClassName);
    Result.Warnings.Add(TEXT("Widget Blueprint modified but not compiled or saved."));
    return true;
}

bool FCodexBlueprintBridgeWidgetOps::RemoveWidget(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UWidgetBlueprint* Blueprint = LoadWidgetBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    const FString Name = GetStringField(Request, TEXT("name"));
    UWidget* Widget = FindWidget(Blueprint, Name);
    if (!Widget)
    {
        Result.Errors.Add({TEXT("WidgetNotFound"), TEXT("Widget was not found in the tree."), TEXT("name")});
        return false;
    }

    const FScopedTransaction Transaction(
        NSLOCTEXT("CodexBlueprintBridge", "RemoveWidget", "Remove Widget"));
    Blueprint->Modify();
    Blueprint->WidgetTree->Modify();
    if (!Blueprint->WidgetTree->RemoveWidget(Widget))
    {
        Result.Errors.Add({TEXT("RemoveWidgetFailed"), TEXT("WidgetTree failed to remove the widget."), TEXT("name")});
        return false;
    }

    MarkModified(Blueprint, true);
    Result.Data->SetStringField(TEXT("asset"), GetStringField(Request, TEXT("asset")));
    Result.Data->SetStringField(TEXT("widget"), Name);
    Result.Warnings.Add(TEXT("Widget Blueprint modified but not compiled or saved."));
    return true;
}

bool FCodexBlueprintBridgeWidgetOps::RenameWidget(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UWidgetBlueprint* Blueprint = LoadWidgetBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    const FString Name = GetStringField(Request, TEXT("name"));
    const FString NewName = GetStringField(Request, TEXT("newName"));
    UWidget* Widget = FindWidget(Blueprint, Name);
    if (!Widget)
    {
        Result.Errors.Add({TEXT("WidgetNotFound"), TEXT("Widget was not found in the tree."), TEXT("name")});
        return false;
    }
    if (!IsValidWidgetName(NewName))
    {
        Result.Errors.Add({TEXT("InvalidWidgetName"), TEXT("New widget name must be a valid identifier."), TEXT("newName")});
        return false;
    }
    if (FindWidget(Blueprint, NewName))
    {
        Result.Errors.Add({TEXT("WidgetAlreadyExists"), TEXT("Widget tree already contains the new name."), TEXT("newName")});
        return false;
    }
    for (const FDelegateEditorBinding& Binding : Blueprint->Bindings)
    {
        if (Binding.ObjectName == Name)
        {
            Result.Errors.Add({
                TEXT("RenameWidgetHasBindings"),
                TEXT("Rename is blocked because the widget has an editor binding."),
                TEXT("name")});
            return false;
        }
    }
    if (!Blueprint->Animations.IsEmpty())
    {
        Result.Errors.Add({
            TEXT("RenameWidgetHasAnimations"),
            TEXT("Rename is blocked while the Widget Blueprint contains animations."),
            TEXT("name")});
        return false;
    }

    const FScopedTransaction Transaction(
        NSLOCTEXT("CodexBlueprintBridge", "RenameWidget", "Rename Widget"));
    Blueprint->Modify();
    Blueprint->WidgetTree->Modify();
    Widget->Modify();
    Blueprint->OnVariableRenamed(FName(*Name), FName(*NewName));
    if (!Widget->Rename(*NewName, Blueprint->WidgetTree, REN_DontCreateRedirectors))
    {
        Result.Errors.Add({TEXT("RenameWidgetFailed"), TEXT("UObject rename failed."), TEXT("newName")});
        return false;
    }

    MarkModified(Blueprint, true);
    Result.Data->SetStringField(TEXT("asset"), GetStringField(Request, TEXT("asset")));
    Result.Data->SetStringField(TEXT("oldName"), Name);
    Result.Data->SetStringField(TEXT("widget"), NewName);
    Result.Warnings.Add(TEXT("Widget Blueprint modified but not compiled or saved."));
    return true;
}

bool FCodexBlueprintBridgeWidgetOps::SetWidgetProperty(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UWidgetBlueprint* Blueprint = LoadWidgetBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    const FString Name = GetStringField(Request, TEXT("name"));
    const FString Property = GetStringField(Request, TEXT("property"));
    UWidget* Widget = FindWidget(Blueprint, Name);
    if (!Widget)
    {
        Result.Errors.Add({TEXT("WidgetNotFound"), TEXT("Widget was not found in the tree."), TEXT("name")});
        return false;
    }

    Widget->Modify();
    if (Property == TEXT("isEnabled"))
    {
        bool Value = false;
        if (!Request->TryGetBoolField(TEXT("value"), Value))
        {
            Result.Errors.Add({TEXT("InvalidPropertyValue"), TEXT("isEnabled requires a boolean value."), TEXT("value")});
            return false;
        }
        Widget->SetIsEnabled(Value);
    }
    else if (Property == TEXT("visibility"))
    {
        ESlateVisibility Visibility;
        if (!ParseVisibility(GetStringField(Request, TEXT("value")), Visibility))
        {
            Result.Errors.Add({TEXT("InvalidPropertyValue"), TEXT("visibility value is invalid."), TEXT("value")});
            return false;
        }
        Widget->SetVisibility(Visibility);
    }
    else if (Property == TEXT("toolTipText"))
    {
        Widget->SetToolTipText(FText::FromString(GetStringField(Request, TEXT("value"))));
    }
    else if (Property == TEXT("text"))
    {
        UTextBlock* TextBlock = Cast<UTextBlock>(Widget);
        if (!TextBlock)
        {
            Result.Errors.Add({TEXT("UnsupportedWidgetProperty"), TEXT("text is only allowed for TextBlock."), TEXT("property")});
            return false;
        }
        TextBlock->SetText(FText::FromString(GetStringField(Request, TEXT("value"))));
    }
    else if (Property == TEXT("colorAndOpacity"))
    {
        FLinearColor Color;
        if (!ReadLinearColor(Request, Color))
        {
            Result.Errors.Add({TEXT("InvalidPropertyValue"), TEXT("colorAndOpacity requires {r,g,b,a}."), TEXT("value")});
            return false;
        }

        if (UTextBlock* TextBlock = Cast<UTextBlock>(Widget))
        {
            TextBlock->SetColorAndOpacity(FSlateColor(Color));
        }
        else if (UImage* Image = Cast<UImage>(Widget))
        {
            Image->SetColorAndOpacity(Color);
        }
        else
        {
            Result.Errors.Add({TEXT("UnsupportedWidgetProperty"), TEXT("colorAndOpacity is only allowed for TextBlock or Image."), TEXT("property")});
            return false;
        }
    }
    else if (Property == TEXT("brushColor"))
    {
        UBorder* Border = Cast<UBorder>(Widget);
        FLinearColor Color;
        if (!Border)
        {
            Result.Errors.Add({TEXT("UnsupportedWidgetProperty"), TEXT("brushColor is only allowed for Border."), TEXT("property")});
            return false;
        }
        if (!ReadLinearColor(Request, Color))
        {
            Result.Errors.Add({TEXT("InvalidPropertyValue"), TEXT("brushColor requires {r,g,b,a}."), TEXT("value")});
            return false;
        }
        Border->SetBrushColor(Color);
    }
    else
    {
        Result.Errors.Add({TEXT("UnsupportedWidgetProperty"), TEXT("Property is not in the conservative whitelist."), TEXT("property")});
        return false;
    }

    MarkModified(Blueprint, false);
    Result.Data->SetStringField(TEXT("asset"), GetStringField(Request, TEXT("asset")));
    Result.Data->SetStringField(TEXT("widget"), Name);
    Result.Data->SetStringField(TEXT("property"), Property);
    Result.Warnings.Add(TEXT("Widget Blueprint modified but not compiled or saved."));
    return true;
}

UWidgetBlueprint* FCodexBlueprintBridgeWidgetOps::LoadWidgetBlueprint(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result)
{
    const FString AssetPath = GetStringField(Request, TEXT("asset"));
    UWidgetBlueprint* Blueprint = LoadObject<UWidgetBlueprint>(nullptr, *AssetPath);
    if (!Blueprint || !Blueprint->WidgetTree)
    {
        Result.Errors.Add({TEXT("WidgetBlueprintNotFound"), TEXT("Asset was not found or is not a Widget Blueprint."), TEXT("asset")});
        return nullptr;
    }
    return Blueprint;
}

UWidget* FCodexBlueprintBridgeWidgetOps::FindWidget(UWidgetBlueprint* Blueprint, const FString& Name)
{
    return Blueprint && Blueprint->WidgetTree && !Name.IsEmpty()
        ? Blueprint->WidgetTree->FindWidget(FName(*Name))
        : nullptr;
}

bool FCodexBlueprintBridgeWidgetOps::IsValidWidgetName(const FString& Name)
{
    if (Name.IsEmpty() || (!FChar::IsAlpha(Name[0]) && Name[0] != TEXT('_')))
    {
        return false;
    }

    for (const TCHAR Character : Name)
    {
        if (!FChar::IsAlnum(Character) && Character != TEXT('_'))
        {
            return false;
        }
    }
    return true;
}

void FCodexBlueprintBridgeWidgetOps::MarkModified(UWidgetBlueprint* Blueprint, const bool bStructural)
{
    if (bStructural)
    {
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    }
    else
    {
        FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
    }
}
