#pragma once

#include "CodexBlueprintBridgeTypes.h"

class FCodexBlueprintBridgePolicy
{
public:
    static constexpr int32 MaxMutationCount = 128;
    static constexpr int32 MaxPayloadBytes = 512 * 1024;
    static constexpr int32 MaxStringFieldCharacters = 16 * 1024;
    static constexpr int32 MaxArrayEntries = 256;

    bool ValidateBaseRequest(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;
    bool ValidateAssetPath(const FString& AssetPath, FCodexBridgeResult& Result) const;
    bool ValidateMutationRequest(
        const TSharedRef<FJsonObject>& Request,
        ECodexBridgeMutationKind MutationKind,
        FCodexBridgeResult& Result,
        FCodexBridgeRequestPolicy* OutPolicy = nullptr) const;
    bool ValidateRequestLimits(
        const TSharedRef<FJsonObject>& Request,
        int32 MutationCount,
        FCodexBridgeResult& Result) const;
    bool ValidateAuthoritySafety(
        const TSharedRef<FJsonObject>& Request,
        FCodexBridgeResult& Result) const;
    bool AllowsReplicatedVariable(const TSharedRef<FJsonObject>& Request, FCodexBridgeResult& Result) const;

private:
    bool RequireExplicitPermission(
        const TSharedRef<FJsonObject>& Request,
        const TCHAR* Field,
        const TCHAR* ErrorCode,
        const TCHAR* Message,
        FCodexBridgeResult& Result) const;
};
