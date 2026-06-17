#include "CodexBlueprintBridgeBlueprintOps.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "FileHelpers.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "ObjectTools.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

namespace
{
FString CompileStatusToString(const UBlueprint* Blueprint)
{
    if (!Blueprint)
    {
        return TEXT("Unknown");
    }

    switch (Blueprint->Status)
    {
    case BS_UpToDate:
        return TEXT("Success");
    case BS_Dirty:
        return TEXT("Dirty");
    case BS_Error:
        return TEXT("Error");
    case BS_UpToDateWithWarnings:
        return TEXT("SuccessWithWarnings");
    default:
        return TEXT("Unknown");
    }
}
}

bool FCodexBlueprintBridgeBlueprintOps::ListBlueprints(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const
{
    FString Path = TEXT("/Game");
    Request->TryGetStringField(TEXT("path"), Path);

    if (!Path.StartsWith(TEXT("/Game")))
    {
        Result.Errors.Add({TEXT("InvalidPath"), TEXT("ListBlueprints path must be under /Game."), TEXT("path")});
        return false;
    }

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    FARFilter Filter;
    Filter.PackagePaths.Add(*Path);
    Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
    Filter.bRecursivePaths = true;

    TArray<FAssetData> Assets;
    AssetRegistryModule.Get().GetAssets(Filter, Assets);

    TArray<TSharedPtr<FJsonValue>> Blueprints;
    for (const FAssetData& Asset : Assets)
    {
        const TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
        Item->SetStringField(TEXT("asset"), Asset.GetSoftObjectPath().GetLongPackageName());
        Item->SetStringField(TEXT("name"), Asset.AssetName.ToString());

        FString ParentClass;
        Asset.GetTagValue(TEXT("ParentClass"), ParentClass);
        Item->SetStringField(TEXT("parentClass"), ParentClass);

        Blueprints.Add(MakeShared<FJsonValueObject>(Item));
    }

    Result.Data->SetArrayField(TEXT("blueprints"), Blueprints);
    return true;
}

bool FCodexBlueprintBridgeBlueprintOps::DescribeBlueprint(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const
{
    UBlueprint* Blueprint = LoadBlueprint(GetAssetPath(Request), Result);
    if (!Blueprint)
    {
        return false;
    }

    Result.Data->SetStringField(TEXT("asset"), GetAssetPath(Request));
    Result.Data->SetStringField(TEXT("generatedClass"), Blueprint->GeneratedClass ? Blueprint->GeneratedClass->GetName() : TEXT(""));
    Result.Data->SetStringField(TEXT("parentClass"), Blueprint->ParentClass ? Blueprint->ParentClass->GetName() : TEXT(""));
    Result.Data->SetStringField(TEXT("compileStatus"), CompileStatusToString(Blueprint));

    TArray<TSharedPtr<FJsonValue>> Variables;
    for (const FBPVariableDescription& Variable : Blueprint->NewVariables)
    {
        const TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
        Item->SetStringField(TEXT("name"), Variable.VarName.ToString());
        Item->SetStringField(TEXT("type"), Variable.VarType.PinCategory.ToString());
        Item->SetStringField(TEXT("default"), Variable.DefaultValue);
        Item->SetBoolField(TEXT("replicated"), (Variable.PropertyFlags & CPF_Net) != 0);
        Item->SetStringField(TEXT("category"), Variable.Category.ToString());
        Variables.Add(MakeShared<FJsonValueObject>(Item));
    }
    Result.Data->SetArrayField(TEXT("variables"), Variables);

    TArray<TSharedPtr<FJsonValue>> Functions;
    for (const UEdGraph* FunctionGraph : Blueprint->FunctionGraphs)
    {
        if (!FunctionGraph)
        {
            continue;
        }

        const TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
        Item->SetStringField(TEXT("name"), FunctionGraph->GetName());
        Item->SetArrayField(TEXT("inputs"), {});
        Item->SetArrayField(TEXT("outputs"), {});
        Functions.Add(MakeShared<FJsonValueObject>(Item));
    }
    Result.Data->SetArrayField(TEXT("functions"), Functions);

    Result.Data->SetArrayField(TEXT("components"), {});
    return true;
}

bool FCodexBlueprintBridgeBlueprintOps::AddVariable(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const
{
    UBlueprint* Blueprint = LoadBlueprint(GetAssetPath(Request), Result);
    if (!Blueprint)
    {
        return false;
    }

    FString Name;
    FString TypeName;
    Request->TryGetStringField(TEXT("name"), Name);
    Request->TryGetStringField(TEXT("type"), TypeName);

    if (!IsValidVariableName(Name))
    {
        Result.Errors.Add({TEXT("InvalidVariableName"), TEXT("Variable name must be a valid identifier."), TEXT("name")});
        return false;
    }

    if (FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, FName(*Name)) != INDEX_NONE)
    {
        Result.Errors.Add({TEXT("VariableAlreadyExists"), TEXT("Blueprint already has a variable with this name."), TEXT("name")});
        return false;
    }

    FEdGraphPinType PinType;
    if (!BuildPinType(TypeName, PinType, Result))
    {
        return false;
    }

    FString DefaultValue;
    Request->TryGetStringField(TEXT("default"), DefaultValue);

    if (!FBlueprintEditorUtils::AddMemberVariable(Blueprint, FName(*Name), PinType, DefaultValue))
    {
        Result.Errors.Add({TEXT("AddVariableFailed"), TEXT("FBlueprintEditorUtils::AddMemberVariable returned false."), TEXT("name")});
        return false;
    }

    FString Category;
    if (Request->TryGetStringField(TEXT("category"), Category) && !Category.IsEmpty())
    {
        FBlueprintEditorUtils::SetBlueprintVariableCategory(Blueprint, FName(*Name), nullptr, FText::FromString(Category));
    }

    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

    Result.Warnings.Add(TEXT("Blueprint modified but not compiled or saved. Run CompileBlueprint and SaveBlueprint explicitly."));
    Result.Data->SetStringField(TEXT("asset"), GetAssetPath(Request));
    Result.Data->SetStringField(TEXT("variable"), Name);
    return true;
}

bool FCodexBlueprintBridgeBlueprintOps::SetVariableDefault(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const
{
    UBlueprint* Blueprint = LoadBlueprint(GetAssetPath(Request), Result);
    if (!Blueprint)
    {
        return false;
    }

    FString Name;
    FString DefaultValue;
    Request->TryGetStringField(TEXT("name"), Name);
    Request->TryGetStringField(TEXT("default"), DefaultValue);

    if (FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, FName(*Name)) == INDEX_NONE)
    {
        Result.Errors.Add({TEXT("VariableNotFound"), TEXT("Blueprint variable was not found."), TEXT("name")});
        return false;
    }

    for (FBPVariableDescription& Variable : Blueprint->NewVariables)
    {
        if (Variable.VarName == FName(*Name))
        {
            Variable.DefaultValue = DefaultValue;
            break;
        }
    }

    FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

    Result.Warnings.Add(TEXT("Blueprint modified but not compiled or saved. Run CompileBlueprint and SaveBlueprint explicitly."));
    Result.Data->SetStringField(TEXT("asset"), GetAssetPath(Request));
    Result.Data->SetStringField(TEXT("variable"), Name);
    return true;
}

bool FCodexBlueprintBridgeBlueprintOps::CompileBlueprint(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const
{
    UBlueprint* Blueprint = LoadBlueprint(GetAssetPath(Request), Result);
    if (!Blueprint)
    {
        return false;
    }

    FKismetEditorUtilities::CompileBlueprint(Blueprint);

    Result.Data->SetStringField(TEXT("asset"), GetAssetPath(Request));
    Result.Data->SetStringField(TEXT("compileStatus"), CompileStatusToString(Blueprint));

    if (Blueprint->Status == BS_Error)
    {
        Result.Errors.Add({TEXT("CompileFailed"), TEXT("Blueprint compile failed. Check Unreal log for compiler details."), TEXT("asset")});
        return false;
    }

    return true;
}

bool FCodexBlueprintBridgeBlueprintOps::SaveBlueprint(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const
{
    UBlueprint* Blueprint = LoadBlueprint(GetAssetPath(Request), Result);
    if (!Blueprint || !Blueprint->GetOutermost())
    {
        return false;
    }

    UPackage* Package = Blueprint->GetOutermost();
    const FString PackageName = Package->GetName();
    FString Filename;
    if (!FPackageName::DoesPackageExist(PackageName, &Filename))
    {
        Filename = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
    }

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SaveArgs.SaveFlags = SAVE_NoError;

    const bool bSaved = UPackage::SavePackage(Package, Blueprint, *Filename, SaveArgs);
    if (!bSaved)
    {
        Result.Errors.Add({TEXT("SaveFailed"), TEXT("UPackage::SavePackage returned false."), TEXT("asset")});
        return false;
    }

    Result.Data->SetStringField(TEXT("asset"), GetAssetPath(Request));
    Result.Data->SetStringField(TEXT("file"), Filename);
    return true;
}

UBlueprint* FCodexBlueprintBridgeBlueprintOps::LoadBlueprint(const FString& AssetPath, FCodexBridgeResult& Result)
{
    UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *AssetPath);
    if (!Blueprint)
    {
        Result.Errors.Add({TEXT("AssetNotFound"), TEXT("Blueprint asset not found or is not a UBlueprint."), TEXT("asset")});
        return nullptr;
    }

    return Blueprint;
}

bool FCodexBlueprintBridgeBlueprintOps::BuildPinType(const FString& TypeName, FEdGraphPinType& OutPinType, FCodexBridgeResult& Result)
{
    const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
    OutPinType = FEdGraphPinType();

    if (TypeName == TEXT("bool"))
    {
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
    }
    else if (TypeName == TEXT("int"))
    {
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Int;
    }
    else if (TypeName == TEXT("float"))
    {
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Real;
        OutPinType.PinSubCategory = UEdGraphSchema_K2::PC_Float;
    }
    else if (TypeName == TEXT("name"))
    {
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Name;
    }
    else if (TypeName == TEXT("string"))
    {
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_String;
    }
    else if (TypeName == TEXT("text"))
    {
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Text;
    }
    else if (TypeName == TEXT("vector"))
    {
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
        OutPinType.PinSubCategoryObject = TBaseStructure<FVector>::Get();
    }
    else if (TypeName == TEXT("rotator"))
    {
        OutPinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
        OutPinType.PinSubCategoryObject = TBaseStructure<FRotator>::Get();
    }
    else
    {
        Result.Errors.Add({TEXT("UnsupportedType"), TEXT("Variable type is not supported by MVP whitelist."), TEXT("type")});
        return false;
    }

    return K2Schema != nullptr;
}

bool FCodexBlueprintBridgeBlueprintOps::IsValidVariableName(const FString& Name)
{
    if (Name.IsEmpty() || !FChar::IsAlpha(Name[0]))
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

FString FCodexBlueprintBridgeBlueprintOps::GetAssetPath(const TSharedRef<FJsonObject>& Request)
{
    FString AssetPath;
    Request->TryGetStringField(TEXT("asset"), AssetPath);
    return AssetPath;
}
