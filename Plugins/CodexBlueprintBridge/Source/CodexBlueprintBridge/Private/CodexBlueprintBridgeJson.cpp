#include "CodexBlueprintBridgeJson.h"

#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

bool FCodexBlueprintBridgeJson::LoadJsonFile(const FString& Path, TSharedPtr<FJsonObject>& OutJson, FString& OutError)
{
    FString Text;
    if (!FFileHelper::LoadFileToString(Text, *Path))
    {
        OutError = FString::Printf(TEXT("Could not read file: %s"), *Path);
        return false;
    }

    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
    if (!FJsonSerializer::Deserialize(Reader, OutJson) || !OutJson.IsValid())
    {
        OutError = FString::Printf(TEXT("Invalid JSON: %s"), *Reader->GetErrorMessage());
        return false;
    }

    return true;
}

bool FCodexBlueprintBridgeJson::SaveResult(const FString& Path, const FCodexBridgeResult& Result, FString* OutError)
{
    const TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    Root->SetNumberField(TEXT("version"), 1);
    Root->SetBoolField(TEXT("success"), Result.bSuccess);
    Root->SetStringField(TEXT("operation"), Result.Operation);

    TArray<TSharedPtr<FJsonValue>> Warnings;
    for (const FString& Warning : Result.Warnings)
    {
        Warnings.Add(MakeShared<FJsonValueString>(Warning));
    }
    Root->SetArrayField(TEXT("warnings"), Warnings);

    TArray<TSharedPtr<FJsonValue>> Errors;
    for (const FCodexBridgeError& Error : Result.Errors)
    {
        const TSharedRef<FJsonObject> ErrorObject = MakeShared<FJsonObject>();
        ErrorObject->SetStringField(TEXT("code"), Error.Code);
        ErrorObject->SetStringField(TEXT("message"), Error.Message);
        ErrorObject->SetStringField(TEXT("field"), Error.Field);
        Errors.Add(MakeShared<FJsonValueObject>(ErrorObject));
    }
    Root->SetArrayField(TEXT("errors"), Errors);

    Root->SetObjectField(TEXT("data"), Result.Data.IsValid() ? Result.Data.ToSharedRef() : MakeShared<FJsonObject>());

    FString Text;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Text);
    if (!FJsonSerializer::Serialize(Root, Writer))
    {
        if (OutError)
        {
            *OutError = TEXT("Could not serialize result JSON.");
        }
        return false;
    }

    if (!FFileHelper::SaveStringToFile(Text, *Path))
    {
        if (OutError)
        {
            *OutError = FString::Printf(TEXT("Could not write file: %s"), *Path);
        }
        return false;
    }

    return true;
}
