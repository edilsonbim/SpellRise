#include "CodexBlueprintBridgeBindingOps.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphSchema_K2_Actions.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ComponentBoundEvent.h"
#include "K2Node_FunctionResult.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "ScopedTransaction.h"
#include "UObject/UnrealType.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "WidgetBlueprint.h"

namespace
{
void AddUnsupportedMutationError(FCodexBridgeResult& Result, const FString& Message)
{
    Result.Errors.Add({
        TEXT("UnsupportedGraphMutation"),
        Message,
        TEXT("reply")
    });
}
}

bool FCodexBlueprintBridgeBindingOps::DescribeBindings(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UWidgetBlueprint* Blueprint = LoadWidgetBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    TArray<TSharedPtr<FJsonValue>> Items;
    for (const FDelegateEditorBinding& Binding : Blueprint->Bindings)
    {
        const TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
        Item->SetStringField(TEXT("widget"), Binding.ObjectName);
        Item->SetStringField(TEXT("delegate"), Binding.PropertyName.ToString());
        Item->SetStringField(TEXT("function"), Binding.FunctionName.ToString());
        Item->SetStringField(
            TEXT("kind"),
            Binding.Kind == EBindingKind::Function ? TEXT("Function") : TEXT("Property"));
        Item->SetStringField(TEXT("memberGuid"), Binding.MemberGuid.ToString(EGuidFormats::DigitsWithHyphens));
        Item->SetBoolField(TEXT("targetExists"), Binding.DoesBindingTargetExist(Blueprint));
        Items.Add(MakeShared<FJsonValueObject>(Item));
    }

    TArray<UK2Node_ComponentBoundEvent*> BoundEvents;
    for (UEdGraph* Graph : Blueprint->UbergraphPages)
    {
        if (Graph)
        {
            Graph->GetNodesOfClass(BoundEvents);
        }
    }
    for (const UK2Node_ComponentBoundEvent* BoundEvent : BoundEvents)
    {
        if (!BoundEvent)
        {
            continue;
        }

        const TSharedRef<FJsonObject> Item = MakeShared<FJsonObject>();
        Item->SetStringField(TEXT("widget"), BoundEvent->ComponentPropertyName.ToString());
        Item->SetStringField(TEXT("delegate"), BoundEvent->DelegatePropertyName.ToString());
        Item->SetStringField(TEXT("function"), BoundEvent->CustomFunctionName.ToString());
        Item->SetStringField(TEXT("kind"), TEXT("Event"));
        Item->SetStringField(TEXT("nodeId"), BoundEvent->NodeGuid.ToString(EGuidFormats::DigitsWithHyphens));
        Item->SetBoolField(TEXT("targetExists"), BoundEvent->GetTargetDelegateProperty() != nullptr);
        Items.Add(MakeShared<FJsonValueObject>(Item));
    }

    Result.Data->SetArrayField(TEXT("bindings"), Items);
    Result.Data->SetNumberField(TEXT("count"), Items.Num());
    return true;
}

bool FCodexBlueprintBridgeBindingOps::BindWidgetEvent(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UWidgetBlueprint* Blueprint = LoadWidgetBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    FString WidgetName;
    FString DelegateName;
    FString FunctionName;
    if (!ReadRequiredString(Request, TEXT("widget"), WidgetName, Result)
        || !ReadRequiredString(Request, TEXT("delegate"), DelegateName, Result)
        || !ReadRequiredString(Request, TEXT("function"), FunctionName, Result))
    {
        return false;
    }

    UWidget* Widget = Blueprint->WidgetTree
        ? Blueprint->WidgetTree->FindWidget(FName(*WidgetName))
        : nullptr;
    if (!Widget)
    {
        Result.Errors.Add({TEXT("WidgetNotFound"), TEXT("Widget was not found in the Widget Tree."), TEXT("widget")});
        return false;
    }

    const FMulticastDelegateProperty* DelegateProperty =
        FindFProperty<FMulticastDelegateProperty>(Widget->GetClass(), FName(*DelegateName));
    if (!DelegateProperty || !DelegateProperty->SignatureFunction)
    {
        Result.Errors.Add({
            TEXT("DelegateNotFound"),
            TEXT("The widget property is not a multicast delegate with a valid signature."),
            TEXT("delegate")
        });
        return false;
    }

    UFunction* TargetFunction = Blueprint->SkeletonGeneratedClass
        ? Blueprint->SkeletonGeneratedClass->FindFunctionByName(FName(*FunctionName))
        : nullptr;
    if (!TargetFunction)
    {
        Result.Errors.Add({
            TEXT("FunctionNotFound"),
            TEXT("Target function/custom event does not exist in the Blueprint skeleton class."),
            TEXT("function")
        });
        return false;
    }

    if (!TargetFunction->IsSignatureCompatibleWith(DelegateProperty->SignatureFunction))
    {
        Result.Errors.Add({
            TEXT("IncompatibleDelegateSignature"),
            TEXT("Target function signature is incompatible with the widget delegate."),
            TEXT("function")
        });
        return false;
    }

    const FObjectProperty* WidgetProperty = Blueprint->SkeletonGeneratedClass
        ? FindFProperty<FObjectProperty>(Blueprint->SkeletonGeneratedClass, FName(*WidgetName))
        : nullptr;
    if (!WidgetProperty)
    {
        Result.Errors.Add({
            TEXT("WidgetPropertyNotFound"),
            TEXT("Widget must be exposed as a Blueprint variable before binding an event."),
            TEXT("widget")
        });
        return false;
    }

    if (FKismetEditorUtilities::FindBoundEventForComponent(
        Blueprint,
        DelegateProperty->GetFName(),
        WidgetProperty->GetFName()))
    {
        Result.Errors.Add({
            TEXT("BindingAlreadyExists"),
            TEXT("A bound event node already exists for this widget delegate."),
            TEXT("delegate")
        });
        return false;
    }

    UEdGraph* EventGraph = Blueprint->UbergraphPages.Num() > 0
        ? Blueprint->UbergraphPages[0]
        : nullptr;
    if (!EventGraph)
    {
        Result.Errors.Add({
            TEXT("EventGraphNotFound"),
            TEXT("Widget Blueprint has no event graph."),
            TEXT("asset")
        });
        return false;
    }

    const FScopedTransaction Transaction(
        NSLOCTEXT("CodexBlueprintBridge", "BindWidgetEvent", "Bind Widget Event"));
    Blueprint->Modify();
    EventGraph->Modify();

    UK2Node_ComponentBoundEvent* EventNode =
        FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_ComponentBoundEvent>(
            EventGraph,
            EventGraph->GetGoodPlaceForNewNode(),
            EK2NewNodeFlags::None,
            [WidgetProperty, DelegateProperty](UK2Node_ComponentBoundEvent* NewNode)
            {
                NewNode->InitializeComponentBoundEventParams(WidgetProperty, DelegateProperty);
            });
    UK2Node_CallFunction* CallNode =
        FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_CallFunction>(
            EventGraph,
            EventNode
                ? FVector2D(EventNode->NodePosX + 360, EventNode->NodePosY)
                : EventGraph->GetGoodPlaceForNewNode(),
            EK2NewNodeFlags::None,
            [TargetFunction](UK2Node_CallFunction* NewNode)
            {
                NewNode->SetFromFunction(TargetFunction);
            });
    if (!EventNode || !CallNode)
    {
        if (EventNode)
        {
            EventNode->DestroyNode();
        }
        if (CallNode)
        {
            CallNode->DestroyNode();
        }
        Result.Errors.Add({
            TEXT("BindingNodeCreationFailed"),
            TEXT("Failed to create event binding nodes."),
            TEXT("delegate")
        });
        return false;
    }

    const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
    UEdGraphPin* EventExec = EventNode->FindPin(UEdGraphSchema_K2::PN_Then, EGPD_Output);
    UEdGraphPin* CallExec = CallNode->GetExecPin();
    if (!Schema || !EventExec || !CallExec || !Schema->TryCreateConnection(EventExec, CallExec))
    {
        EventNode->DestroyNode();
        CallNode->DestroyNode();
        Result.Errors.Add({
            TEXT("BindingConnectionFailed"),
            TEXT("Failed to connect the event to its target function."),
            TEXT("function")
        });
        return false;
    }

    for (UEdGraphPin* EventPin : EventNode->Pins)
    {
        if (!EventPin || EventPin->Direction != EGPD_Output || EventPin == EventExec)
        {
            continue;
        }
        if (UEdGraphPin* FunctionPin = CallNode->FindPin(EventPin->PinName, EGPD_Input))
        {
            Schema->TryCreateConnection(EventPin, FunctionPin);
        }
    }

    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

    Result.Data->SetStringField(TEXT("widget"), WidgetName);
    Result.Data->SetStringField(TEXT("delegate"), DelegateName);
    Result.Data->SetStringField(TEXT("function"), FunctionName);
    Result.Data->SetStringField(TEXT("eventNodeId"), EventNode->NodeGuid.ToString(EGuidFormats::DigitsWithHyphens));
    Result.Data->SetStringField(TEXT("callNodeId"), CallNode->NodeGuid.ToString(EGuidFormats::DigitsWithHyphens));
    Result.Data->SetBoolField(TEXT("modified"), true);
    return true;
}

bool FCodexBlueprintBridgeBindingOps::UnbindWidgetEvent(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UWidgetBlueprint* Blueprint = LoadWidgetBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    FString WidgetName;
    FString DelegateName;
    if (!ReadRequiredString(Request, TEXT("widget"), WidgetName, Result)
        || !ReadRequiredString(Request, TEXT("delegate"), DelegateName, Result))
    {
        return false;
    }

    const UK2Node_ComponentBoundEvent* Existing =
        FKismetEditorUtilities::FindBoundEventForComponent(
            Blueprint,
            FName(*DelegateName),
            FName(*WidgetName));
    if (!Existing)
    {
        Result.Errors.Add({
            TEXT("BindingNotFound"),
            TEXT("No matching widget delegate binding exists."),
            TEXT("delegate")
        });
        return false;
    }

    const FScopedTransaction Transaction(
        NSLOCTEXT("CodexBlueprintBridge", "UnbindWidgetEvent", "Unbind Widget Event"));
    Blueprint->Modify();
    UEdGraphNode* MutableEvent = const_cast<UK2Node_ComponentBoundEvent*>(Existing);
    MutableEvent->Modify();
    FBlueprintEditorUtils::RemoveNode(Blueprint, MutableEvent, true);
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    Result.Data->SetNumberField(TEXT("removed"), 1);
    Result.Data->SetBoolField(TEXT("modified"), true);
    return true;
}

bool FCodexBlueprintBridgeBindingOps::GetOrCreateFunctionGraph(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UWidgetBlueprint* Blueprint = LoadWidgetBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    FString FunctionName;
    if (!ReadRequiredString(Request, TEXT("function"), FunctionName, Result))
    {
        return false;
    }

    UEdGraph* Graph = FindFunctionGraph(Blueprint, FName(*FunctionName));
    if (!Graph)
    {
        Result.Errors.Add({
            TEXT("FunctionGraphNotFound"),
            TEXT("Automatic function graph creation is intentionally rejected unless a complete, verified signature contract is supplied."),
            TEXT("function")
        });
        return false;
    }

    Result.Data->SetStringField(TEXT("function"), FunctionName);
    Result.Data->SetStringField(TEXT("graph"), Graph->GetName());
    Result.Data->SetBoolField(TEXT("created"), false);
    return true;
}

bool FCodexBlueprintBridgeBindingOps::SetOnMouseButtonDownReply(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result) const
{
    UWidgetBlueprint* Blueprint = LoadWidgetBlueprint(Request, Result);
    if (!Blueprint)
    {
        return false;
    }

    FString Reply;
    if (!ReadRequiredString(Request, TEXT("reply"), Reply, Result))
    {
        return false;
    }
    if (!Reply.Equals(TEXT("Handled"), ESearchCase::IgnoreCase)
        && !Reply.Equals(TEXT("Unhandled"), ESearchCase::IgnoreCase))
    {
        Result.Errors.Add({
            TEXT("InvalidReply"),
            TEXT("reply must be Handled or Unhandled."),
            TEXT("reply")
        });
        return false;
    }

    UEdGraph* Graph = FindFunctionGraph(Blueprint, TEXT("OnMouseButtonDown"));
    if (!Graph)
    {
        AddUnsupportedMutationError(
            Result,
            TEXT("OnMouseButtonDown function graph does not exist. Create the override in the Widget Blueprint first."));
        return false;
    }

    UK2Node_FunctionResult* ResultNode = nullptr;
    for (UEdGraphNode* Node : Graph->Nodes)
    {
        if (UK2Node_FunctionResult* Candidate = Cast<UK2Node_FunctionResult>(Node))
        {
            if (ResultNode)
            {
                Result.Errors.Add({
                    TEXT("AmbiguousFunctionResult"),
                    TEXT("OnMouseButtonDown has multiple return nodes; operation refused."),
                    TEXT("asset")
                });
                return false;
            }
            ResultNode = Candidate;
        }
    }

    UEdGraphPin* ReturnPin = ResultNode ? ResultNode->FindPin(TEXT("ReturnValue"), EGPD_Input) : nullptr;
    if (!ReturnPin)
    {
        Result.Errors.Add({
            TEXT("ReturnPinNotFound"),
            TEXT("OnMouseButtonDown ReturnValue pin was not found."),
            TEXT("asset")
        });
        return false;
    }

    UFunction* ReplyFunction = Reply.Equals(TEXT("Handled"), ESearchCase::IgnoreCase)
        ? UWidgetBlueprintLibrary::StaticClass()->FindFunctionByName(
            GET_FUNCTION_NAME_CHECKED(UWidgetBlueprintLibrary, Handled))
        : UWidgetBlueprintLibrary::StaticClass()->FindFunctionByName(
            GET_FUNCTION_NAME_CHECKED(UWidgetBlueprintLibrary, Unhandled));
    if (!ReplyFunction)
    {
        Result.Errors.Add({
            TEXT("ReplyFunctionNotFound"),
            TEXT("WidgetBlueprintLibrary reply function was not found."),
            TEXT("reply")
        });
        return false;
    }

    const FScopedTransaction Transaction(
        NSLOCTEXT("CodexBlueprintBridge", "SetMouseReply", "Set OnMouseButtonDown Reply"));
    Blueprint->Modify();
    Graph->Modify();
    ResultNode->Modify();

    UK2Node_CallFunction* ReplyNode =
        FEdGraphSchemaAction_K2NewNode::SpawnNode<UK2Node_CallFunction>(
            Graph,
            FVector2D(ResultNode->NodePosX - 320, ResultNode->NodePosY + 160),
            EK2NewNodeFlags::None,
            [ReplyFunction](UK2Node_CallFunction* NewNode)
            {
                NewNode->SetFromFunction(ReplyFunction);
            });
    if (!ReplyNode)
    {
        Result.Errors.Add({
            TEXT("ReplyNodeCreationFailed"),
            TEXT("Failed to create the constant reply node."),
            TEXT("reply")
        });
        return false;
    }

    UEdGraphPin* ReplyOutput = ReplyNode->GetReturnValuePin();
    const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
    if (!ReplyOutput || !Schema)
    {
        Result.Errors.Add({
            TEXT("ReplyOutputNotFound"),
            TEXT("Reply function output pin was not created."),
            TEXT("reply")
        });
        ReplyNode->DestroyNode();
        return false;
    }

    Schema->BreakPinLinks(*ReturnPin, true);
    if (!Schema->TryCreateConnection(ReplyOutput, ReturnPin))
    {
        Result.Errors.Add({
            TEXT("ReplyConnectionRejected"),
            TEXT("K2 schema rejected the reply connection."),
            TEXT("reply")
        });
        ReplyNode->DestroyNode();
        return false;
    }

    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    Result.Data->SetBoolField(TEXT("overrideExists"), true);
    Result.Data->SetStringField(TEXT("requestedReply"), Reply);
    Result.Data->SetStringField(
        TEXT("replyNodeId"),
        ReplyNode->NodeGuid.ToString(EGuidFormats::DigitsWithHyphens));
    Result.Data->SetBoolField(TEXT("modified"), true);
    Result.Warnings.Add(TEXT("Blueprint modified but not compiled or saved."));
    return true;
}

UWidgetBlueprint* FCodexBlueprintBridgeBindingOps::LoadWidgetBlueprint(
    const TSharedRef<FJsonObject>& Request,
    FCodexBridgeResult& Result)
{
    FString AssetPath;
    if (!ReadRequiredString(Request, TEXT("asset"), AssetPath, Result))
    {
        return nullptr;
    }

    UWidgetBlueprint* Blueprint = LoadObject<UWidgetBlueprint>(nullptr, *AssetPath);
    if (!Blueprint)
    {
        Result.Errors.Add({
            TEXT("WidgetBlueprintNotFound"),
            TEXT("Asset was not found or is not a Widget Blueprint."),
            TEXT("asset")
        });
    }
    return Blueprint;
}

UEdGraph* FCodexBlueprintBridgeBindingOps::FindFunctionGraph(
    const UWidgetBlueprint* Blueprint,
    FName FunctionName)
{
    if (!Blueprint)
    {
        return nullptr;
    }
    for (UEdGraph* Graph : Blueprint->FunctionGraphs)
    {
        if (Graph && Graph->GetFName() == FunctionName)
        {
            return Graph;
        }
    }
    return nullptr;
}

bool FCodexBlueprintBridgeBindingOps::ReadRequiredString(
    const TSharedRef<FJsonObject>& Request,
    const TCHAR* Field,
    FString& OutValue,
    FCodexBridgeResult& Result)
{
    if (!Request->TryGetStringField(Field, OutValue) || OutValue.IsEmpty())
    {
        Result.Errors.Add({
            TEXT("MissingField"),
            FString::Printf(TEXT("Request must include non-empty %s."), Field),
            Field
        });
        return false;
    }
    return true;
}
