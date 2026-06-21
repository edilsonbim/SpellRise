#include "CodexBlueprintBridgePolicy.h"

#include "Engine/Blueprint.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
bool IsAuthoritySensitiveClass(const FString& ClassName)
{
    static const TCHAR* SensitiveClasses[] = {
        TEXT("PlayerController"),
        TEXT("PlayerState"),
        TEXT("GameMode"),
        TEXT("GameState"),
        TEXT("Character"),
        TEXT("Pawn"),
        TEXT("AbilitySystemComponent"),
        TEXT("GameplayAbility"),
        TEXT("AttributeSet")
    };

    for (const TCHAR* SensitiveClass : SensitiveClasses)
    {
        if (ClassName.Contains(SensitiveClass, ESearchCase::IgnoreCase))
        {
            return true;
        }
    }

    return false;
}

bool ContainsRpcMutation(const TSharedRef<FJsonObject>& Request)
{
    FString FunctionFlags;
    Request->TryGetStringField(TEXT("functionFlags"), FunctionFlags);

    FString Replication;
    Request->TryGetStringField(TEXT("replication"), Replication);

    bool bRpc = false;
    Request->TryGetBoolField(TEXT("rpc"), bRpc);

    return bRpc
        || FunctionFlags.Contains(TEXT("Server"), ESearchCase::IgnoreCase)
        || FunctionFlags.Contains(TEXT("Client"), ESearchCase::IgnoreCase)
        || FunctionFlags.Contains(TEXT("NetMulticast"), ESearchCase::IgnoreCase)
        || Replication.Contains(TEXT("Server"), ESearchCase::IgnoreCase)
        || Replication.Contains(TEXT("Client"), ESearchCase::IgnoreCase)
        || Replication.Contains(TEXT("Multicast"), ESearchCase::IgnoreCase);
}
}

bool FCodexBlueprintBridgePolicy::ValidateBaseRequest(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const
{
    int32 Version = 0;
    if (!Request->TryGetNumberField(TEXT("version"), Version) || Version != 1)
    {
        Result.Errors.Add({TEXT("UnsupportedVersion"), TEXT("Only request version 1 is supported."), TEXT("version")});
        return false;
    }

    FString Operation;
    if (!Request->TryGetStringField(TEXT("operation"), Operation) || Operation.IsEmpty())
    {
        Result.Errors.Add({TEXT("MissingOperation"), TEXT("Request must include operation."), TEXT("operation")});
        return false;
    }

    Result.Operation = Operation;
    return true;
}

bool FCodexBlueprintBridgePolicy::ValidateAssetPath(const FString& AssetPath, FCodexBridgeResult& Result) const
{
    if (AssetPath.IsEmpty())
    {
        Result.Errors.Add({TEXT("MissingAsset"), TEXT("Request must include asset."), TEXT("asset")});
        return false;
    }

    if (!AssetPath.StartsWith(TEXT("/Game/")) || AssetPath.Contains(TEXT("..")) || AssetPath.Contains(TEXT("\\")))
    {
        Result.Errors.Add({TEXT("InvalidAssetPath"), TEXT("Asset path must be a package path under /Game."), TEXT("asset")});
        return false;
    }

    return true;
}

bool FCodexBlueprintBridgePolicy::ValidateMutationRequest(
    const TSharedRef<FJsonObject>& Request,
    ECodexBridgeMutationKind MutationKind,
    FCodexBridgeResult& Result,
    FCodexBridgeRequestPolicy* OutPolicy) const
{
    FCodexBridgeRequestPolicy Policy;
    Request->TryGetBoolField(TEXT("dryRun"), Policy.bDryRun);
    Request->TryGetBoolField(TEXT("allowGraphMutation"), Policy.bAllowGraphMutation);
    Request->TryGetBoolField(TEXT("allowWidgetMutation"), Policy.bAllowWidgetMutation);
    Request->TryGetBoolField(TEXT("allowBindingMutation"), Policy.bAllowBindingMutation);
    Request->TryGetBoolField(TEXT("allowAuthoritySensitiveMutation"), Policy.bAllowAuthoritySensitiveMutation);

    const TArray<TSharedPtr<FJsonValue>>* Mutations = nullptr;
    Policy.MutationCount = Request->TryGetArrayField(TEXT("mutations"), Mutations) ? Mutations->Num() : 1;

    FString SerializedRequest;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&SerializedRequest);
    FJsonSerializer::Serialize(Request, Writer);
    Policy.PayloadBytes = FTCHARToUTF8(*SerializedRequest).Length();

    if (OutPolicy != nullptr)
    {
        *OutPolicy = Policy;
    }

    bool bAllowed = ValidateRequestLimits(Request, Policy.MutationCount, Result);
    bAllowed = ValidateAuthoritySafety(Request, Result) && bAllowed;

    switch (MutationKind)
    {
    case ECodexBridgeMutationKind::Graph:
        bAllowed = RequireExplicitPermission(
            Request,
            TEXT("allowGraphMutation"),
            TEXT("GraphMutationPermissionRequired"),
            TEXT("Graph mutation requires allowGraphMutation=true."),
            Result) && bAllowed;
        break;
    case ECodexBridgeMutationKind::Widget:
        bAllowed = RequireExplicitPermission(
            Request,
            TEXT("allowWidgetMutation"),
            TEXT("WidgetMutationPermissionRequired"),
            TEXT("Widget Tree mutation requires allowWidgetMutation=true."),
            Result) && bAllowed;
        break;
    case ECodexBridgeMutationKind::Binding:
        bAllowed = RequireExplicitPermission(
            Request,
            TEXT("allowBindingMutation"),
            TEXT("BindingMutationPermissionRequired"),
            TEXT("Binding or delegate mutation requires allowBindingMutation=true."),
            Result) && bAllowed;
        break;
    case ECodexBridgeMutationKind::None:
    default:
        break;
    }

    if (Policy.bDryRun)
    {
        Result.Warnings.Add(TEXT("Dry run enabled: validate and describe the mutation without modifying or saving the asset."));
    }

    return bAllowed;
}

bool FCodexBlueprintBridgePolicy::ValidateRequestLimits(
    const TSharedRef<FJsonObject>& Request,
    int32 MutationCount,
    FCodexBridgeResult& Result) const
{
    bool bValid = true;

    FString SerializedRequest;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&SerializedRequest);
    FJsonSerializer::Serialize(Request, Writer);
    if (FTCHARToUTF8(*SerializedRequest).Length() > MaxPayloadBytes)
    {
        Result.Errors.Add({
            TEXT("PayloadTooLarge"),
            FString::Printf(TEXT("Request payload exceeds the %d byte limit."), MaxPayloadBytes),
            TEXT("")
        });
        bValid = false;
    }

    if (MutationCount < 0 || MutationCount > MaxMutationCount)
    {
        Result.Errors.Add({
            TEXT("TooManyMutations"),
            FString::Printf(TEXT("Mutation count exceeds the %d operation limit."), MaxMutationCount),
            TEXT("mutations")
        });
        bValid = false;
    }

    for (const TPair<FString, TSharedPtr<FJsonValue>>& Field : Request->Values)
    {
        FString StringValue;
        if (Field.Value.IsValid()
            && Field.Value->TryGetString(StringValue)
            && StringValue.Len() > MaxStringFieldCharacters)
        {
            Result.Errors.Add({
                TEXT("StringFieldTooLarge"),
                FString::Printf(TEXT("String field exceeds the %d character limit."), MaxStringFieldCharacters),
                Field.Key
            });
            bValid = false;
        }

        const TArray<TSharedPtr<FJsonValue>>* ArrayValue = nullptr;
        if (Field.Value.IsValid()
            && Field.Value->TryGetArray(ArrayValue)
            && ArrayValue != nullptr
            && ArrayValue->Num() > MaxArrayEntries)
        {
            Result.Errors.Add({
                TEXT("ArrayFieldTooLarge"),
                FString::Printf(TEXT("Array field exceeds the %d entry limit."), MaxArrayEntries),
                Field.Key
            });
            bValid = false;
        }
    }

    return bValid;
}

bool FCodexBlueprintBridgePolicy::ValidateAuthoritySafety(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    bool bReplicated = false;
    Request->TryGetBoolField(TEXT("replicated"), bReplicated);

    FString AssetClass;
    Request->TryGetStringField(TEXT("assetClass"), AssetClass);

    FString ParentClass;
    Request->TryGetStringField(TEXT("parentClass"), ParentClass);

    FString AssetPath;
    Request->TryGetStringField(TEXT("asset"), AssetPath);
    if (!AssetPath.IsEmpty())
    {
        if (const UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *AssetPath))
        {
            AssetClass = Blueprint->GetClass()->GetName();
            ParentClass = Blueprint->ParentClass ? Blueprint->ParentClass->GetName() : FString();
        }
    }

    const bool bSensitive = bReplicated
        || ContainsRpcMutation(Request)
        || IsAuthoritySensitiveClass(AssetClass)
        || IsAuthoritySensitiveClass(ParentClass);
    if (!bSensitive)
    {
        return true;
    }

    bool bExplicitlyAllowed = false;
    Request->TryGetBoolField(TEXT("allowAuthoritySensitiveMutation"), bExplicitlyAllowed);
    if (!bExplicitlyAllowed)
    {
        Result.Errors.Add({
            TEXT("AuthoritySensitiveMutationBlocked"),
            TEXT("Replicated, RPC, or authority-sensitive Blueprint mutation is blocked by default."),
            TEXT("allowAuthoritySensitiveMutation")
        });
        return false;
    }

    Result.Warnings.Add(TEXT("Authority-sensitive mutation explicitly enabled. Manual network and server-authority review is required."));
    return true;
}

bool FCodexBlueprintBridgePolicy::AllowsReplicatedVariable(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const
{
    bool bReplicated = false;
    Request->TryGetBoolField(TEXT("replicated"), bReplicated);
    if (!bReplicated)
    {
        return true;
    }

    Result.Errors.Add({
        TEXT("ReplicatedVariableBlocked"),
        TEXT("Adding replicated Blueprint variables is blocked in the MVP until replication flags, lifetime conditions, OnRep policy and network review output are implemented."),
        TEXT("replicated")
    });
    return false;
}

bool FCodexBlueprintBridgePolicy::RequireExplicitPermission(
    const TSharedRef<FJsonObject>& Request,
    const TCHAR* Field,
    const TCHAR* ErrorCode,
    const TCHAR* Message,
    FCodexBridgeResult& Result) const
{
    bool bAllowed = false;
    if (Request->TryGetBoolField(Field, bAllowed) && bAllowed)
    {
        return true;
    }

    Result.Errors.Add({ErrorCode, Message, Field});
    return false;
}
