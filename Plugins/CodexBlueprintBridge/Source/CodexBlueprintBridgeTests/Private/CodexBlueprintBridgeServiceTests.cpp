#include "CodexBlueprintBridgeService.h"

#include "Dom/JsonObject.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
TSharedRef<FJsonObject> MakeRequest(const FString& Operation)
{
    TSharedRef<FJsonObject> Request = MakeShared<FJsonObject>();
    Request->SetNumberField(TEXT("version"), 1);
    Request->SetStringField(TEXT("operation"), Operation);
    return Request;
}

}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FCodexBridgeRejectsInvalidVersionTest,
    "Codex.BlueprintBridge.Policy.RejectsInvalidVersion",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexBridgeRejectsInvalidVersionTest::RunTest(const FString& Parameters)
{
    TSharedRef<FJsonObject> Request = MakeRequest(TEXT("ListBlueprints"));
    Request->SetNumberField(TEXT("version"), 999);

    const FCodexBridgeResult Result = FCodexBlueprintBridgeService().Execute(Request);

    TestFalse(TEXT("Invalid protocol version must fail"), Result.bSuccess);
    TestTrue(TEXT("Result must contain a validation error"), Result.Errors.Num() > 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FCodexBridgeRejectsMissingAssetTest,
    "Codex.BlueprintBridge.Policy.RejectsMissingAsset",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexBridgeRejectsMissingAssetTest::RunTest(const FString& Parameters)
{
    const FCodexBridgeResult Result =
        FCodexBlueprintBridgeService().Execute(MakeRequest(TEXT("DescribeGraphs")));

    TestFalse(TEXT("Read-only operation without asset must fail"), Result.bSuccess);
    TestTrue(TEXT("Result must contain an asset validation error"), Result.Errors.Num() > 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FCodexBridgeGraphDryRunTest,
    "Codex.BlueprintBridge.Mutation.GraphDryRunDoesNotMutate",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexBridgeGraphDryRunTest::RunTest(const FString& Parameters)
{
    TSharedRef<FJsonObject> Request = MakeRequest(TEXT("AddNode"));
    Request->SetStringField(TEXT("asset"), TEXT("/Game/Tests/DoesNotNeedToExist"));
    Request->SetBoolField(TEXT("dryRun"), true);
    Request->SetBoolField(TEXT("allowGraphMutation"), true);

    const FCodexBridgeResult Result = FCodexBlueprintBridgeService().Execute(Request);

    TestTrue(TEXT("Authorized dry-run must complete without loading or mutating an asset"), Result.bSuccess);
    TestTrue(TEXT("Response must report dryRun"), Result.Data->GetBoolField(TEXT("dryRun")));
    TestEqual(TEXT("Response must classify the mutation"), Result.Data->GetStringField(TEXT("mutationKind")), FString(TEXT("Graph")));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FCodexBridgeReadOnlyMissingAssetTest,
    "Codex.BlueprintBridge.ReadOnly.MissingAssetFailsSafely",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexBridgeReadOnlyMissingAssetTest::RunTest(const FString& Parameters)
{
    TSharedRef<FJsonObject> Request = MakeRequest(TEXT("DescribeWidgetTree"));
    Request->SetStringField(TEXT("asset"), TEXT("/Game/Tests/CodexBridgeAssetThatDoesNotExist"));

    const FCodexBridgeResult Result = FCodexBlueprintBridgeService().Execute(Request);

    TestFalse(TEXT("Missing read-only asset must fail"), Result.bSuccess);
    TestTrue(TEXT("Failure must be structured"), Result.Errors.Num() > 0);
    return true;
}

#endif
