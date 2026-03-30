#include "SpellRise/Components/SpellRiseBuildInputFacade.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Blueprint/UserWidget.h"
#include "UObject/UnrealType.h"

#include "SpellRise/Components/SpellRiseNarrativeBuildBridge.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseBuildInputFacade, Log, All);

namespace
{
	bool SetBoolCompatibleProperty(FProperty* Property, void* ValuePtr, bool bValue)
	{
		if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
		{
			BoolProperty->SetPropertyValue(ValuePtr, bValue);
			return true;
		}

		if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
		{
			ByteProperty->SetPropertyValue(ValuePtr, bValue ? 1 : 0);
			return true;
		}

		return false;
	}

	bool SetFloatCompatibleProperty(FProperty* Property, void* ValuePtr, float Value)
	{
		if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
		{
			FloatProperty->SetPropertyValue(ValuePtr, Value);
			return true;
		}

		if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(Property))
		{
			DoubleProperty->SetPropertyValue(ValuePtr, static_cast<double>(Value));
			return true;
		}

		if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
		{
			IntProperty->SetPropertyValue(ValuePtr, FMath::RoundToInt(Value));
			return true;
		}

		return false;
	}

	bool SetIncreaseEnumCompatibleProperty(FProperty* Property, void* ValuePtr, bool bIncrease)
	{
		const TCHAR* PreferredName = bIncrease ? TEXT("Increase") : TEXT("Decrease");

		if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
		{
			const UEnum* Enum = EnumProperty->GetEnum();
			if (Enum)
			{
				const int64 Value = Enum->GetValueByNameString(PreferredName);
				if (Value != INDEX_NONE)
				{
					if (FNumericProperty* Underlying = EnumProperty->GetUnderlyingProperty())
					{
						Underlying->SetIntPropertyValue(ValuePtr, Value);
						return true;
					}
				}
			}
		}

		if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
		{
			if (const UEnum* Enum = ByteProperty->Enum)
			{
				const int64 Value = Enum->GetValueByNameString(PreferredName);
				if (Value != INDEX_NONE)
				{
					ByteProperty->SetPropertyValue(ValuePtr, static_cast<uint8>(FMath::Clamp<int64>(Value, 0, 255)));
					return true;
				}
			}

			ByteProperty->SetPropertyValue(ValuePtr, bIncrease ? 0 : 1);
			return true;
		}

		if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
		{
			IntProperty->SetPropertyValue(ValuePtr, bIncrease ? 0 : 1);
			return true;
		}

		return false;
	}
}

USpellRiseBuildInputFacade::USpellRiseBuildInputFacade()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USpellRiseBuildInputFacade::BeginPlay()
{
	Super::BeginPlay();

	if (!NarrativeBuildBridge)
	{
		NarrativeBuildBridge = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseNarrativeBuildBridge>() : nullptr;
	}

	// Startup menu sync is intentionally deferred until pawn possession is stable.
	// Some EBS variants alter local view/input state when called too early in controller BeginPlay.
	bStartupMenuSyncDone = false;
}

void USpellRiseBuildInputFacade::HandleOwningPawnReady()
{
	if (bStartupMenuSyncDone)
	{
		return;
	}

	const APlayerController* OwnerController = Cast<APlayerController>(GetOwner());
	if (!OwnerController || !OwnerController->IsLocalController())
	{
		return;
	}

	if (!OwnerController->GetPawn())
	{
		return;
	}

	if (UObject* BuildingComponent = ResolveEBSBuildingComponent())
	{
		CallNoParamFunction(BuildingComponent, TEXT("HideBuildingMenu"));
	}

	bStartupMenuSyncDone = true;
}

void USpellRiseBuildInputFacade::SetConstructionModeActive(bool bInConstructionModeActive)
{
	bConstructionModeActive = bInConstructionModeActive;

	if (!bConstructionModeActive)
	{
		return;
	}

	// Prime the build component on mode enter to reduce cases where preview/widget doesn't show.
	if (UObject* BuildingComponent = ResolveEBSBuildingComponent())
	{
		CallNoParamFunction(BuildingComponent, TEXT("AutoInitComponent"));
	}
}

void USpellRiseBuildInputFacade::SetConstructionModifierHeld(bool bIsHeld)
{
	bConstructionModifierHeld = bIsHeld;
}

void USpellRiseBuildInputFacade::OnBuildMenuPressed()
{
	if (!CanProcessConstructionCommand())
	{
		return;
	}

	UObject* BuildingComponent = ResolveEBSBuildingComponent();
	if (!BuildingComponent)
	{
		return;
	}

	// Manual init mode requires explicit component init before opening the radial menu.
	CallNoParamFunction(BuildingComponent, TEXT("AutoInitComponent"));
	CallNoParamFunction(BuildingComponent, TEXT("ShowBuildingMenu"));
	bHasCachedTraceToMouseMode = GetBooleanProperty(BuildingComponent, TEXT("TraceToMouseMode"), bCachedTraceToMouseMode);
	SetBooleanProperty(BuildingComponent, TEXT("TraceToMouseMode"), true);

	// Allow explicit mouse click navigation/selection on radial menu.
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		bCachedClickEvents = PC->bEnableClickEvents;
		bCachedMouseOverEvents = PC->bEnableMouseOverEvents;

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;
	}
}

void USpellRiseBuildInputFacade::OnBuildMenuReleased()
{
	if (!CanProcessConstructionCommand())
	{
		return;
	}

	UObject* BuildingComponent = ResolveEBSBuildingComponent();
	if (!BuildingComponent)
	{
		return;
	}

	CallNoParamFunction(BuildingComponent, TEXT("HideBuildingMenu"));
	if (bHasCachedTraceToMouseMode)
	{
		SetBooleanProperty(BuildingComponent, TEXT("TraceToMouseMode"), bCachedTraceToMouseMode);
	}

	// Restore gameplay input after closing menu.
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
		PC->bEnableClickEvents = bCachedClickEvents;
		PC->bEnableMouseOverEvents = bCachedMouseOverEvents;
	}
}

void USpellRiseBuildInputFacade::OnBuildCancelPressed()
{
	if (!CanProcessConstructionCommand())
	{
		return;
	}

	CallNoParamFunction(ResolveEBSBuildingComponent(), TEXT("CancelBuild"));
}

void USpellRiseBuildInputFacade::OnBuildCycleMenuPressed()
{
	if (!CanProcessConstructionCommand() || !bEnableLegacyEBSMenuCalls)
	{
		return;
	}

	if (!CallNoParamFunction(ResolveEBSBuildingComponent(), TEXT("ChangeBuildingWidget")))
	{
		CallNoParamFunction(ResolveEBSBuildingComponent(), TEXT("ChangeBuildingMenuType"));
	}
}

void USpellRiseBuildInputFacade::OnBuildToggleNearSnappingPressed()
{
	if (!CanProcessConstructionCommand())
	{
		return;
	}

	ToggleBooleanPropertyAndSubmit(
		ResolveEBSBuildingComponent(),
		TEXT("NearSnappingMode"),
		TEXT("ChangeNearSnappingMode"),
		TEXT("NearSnappingMode"));
}

void USpellRiseBuildInputFacade::OnBuildToggleGridPressed()
{
	if (!CanProcessConstructionCommand())
	{
		return;
	}

	ToggleBooleanPropertyAndSubmit(
		ResolveEBSBuildingComponent(),
		TEXT("GridMode"),
		TEXT("ChangeGridMode"),
		TEXT("GridMode"));
}

void USpellRiseBuildInputFacade::OnBuildAdjustWheel(float AxisValue)
{
	if (!CanProcessConstructionCommand() || FMath::IsNearlyZero(AxisValue))
	{
		return;
	}

	UObject* BuildingComponent = ResolveEBSBuildingComponent();
	if (!BuildingComponent)
	{
		return;
	}

	const bool bIncrease = AxisValue > 0.0f;
	const float StepValue = bConstructionModifierHeld ? WheelHeightStep : WheelRotationStep;
	const float FinalValue = FMath::Max(0.01f, FMath::Abs(AxisValue) * StepValue);
	bool bShiftHeldRuntime = bConstructionModifierHeld;
	if (const APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		bShiftHeldRuntime = bShiftHeldRuntime
			|| PC->IsInputKeyDown(EKeys::LeftShift)
			|| PC->IsInputKeyDown(EKeys::RightShift);
	}

	bool bApplied = false;
	if (bShiftHeldRuntime)
	{
		bApplied |= CallOperationFunction(BuildingComponent, TEXT("ChangeBuildingOffsetZ"), bIncrease, FinalValue);
		bApplied |= CallOperationFunction(BuildingComponent, TEXT("ChangeBuildingHeight"), bIncrease, FinalValue);
		bApplied |= CallOperationFunction(BuildingComponent, TEXT("ChangeBuildingOffset"), bIncrease, FinalValue);
		bApplied |= CallOperationFunction(BuildingComponent, TEXT("AdjustBuildingHeight"), bIncrease, FinalValue);
		if (!bApplied)
		{
			bApplied |= bIncrease
				? CallFirstAvailableNoParamFunction(BuildingComponent, {TEXT("IncreaseBuildingHeight"), TEXT("RaiseBuilding"), TEXT("HeightUp")})
				: CallFirstAvailableNoParamFunction(BuildingComponent, {TEXT("DecreaseBuildingHeight"), TEXT("LowerBuilding"), TEXT("HeightDown")});
		}
	}
	else
	{
		bApplied |= CallOperationFunction(BuildingComponent, TEXT("ChangeBuildingRotationZ"), bIncrease, FinalValue);
		bApplied |= CallOperationFunction(BuildingComponent, TEXT("ChangeBuildingRotation"), bIncrease, FinalValue);
	}

	if (!bApplied && bEnableLegacyEBSAxisCalls)
	{
		UE_LOG(LogSpellRiseBuildInputFacade, Warning, TEXT("[Construction][Facade] Wheel axis sem função compatível no componente EBS (Rotation/Offset)."));
	}
}

void USpellRiseBuildInputFacade::OnBuildFloorUpPressed()
{
	if (!CanProcessConstructionCommand() || !bEnableDisplayedFloorCommands)
	{
		return;
	}

	CallDisplayedFloorFunction(true);
}

void USpellRiseBuildInputFacade::OnBuildFloorDownPressed()
{
	if (!CanProcessConstructionCommand() || !bEnableDisplayedFloorCommands)
	{
		return;
	}

	CallDisplayedFloorFunction(false);
}

void USpellRiseBuildInputFacade::OnBuildInteractPressed()
{
	if (!CanProcessConstructionCommand())
	{
		return;
	}

	const bool bHandledByNarrative = HandleMalletInteraction(true);
	if (!bHandledByNarrative)
	{
		UObject* BuildingComponent = ResolveEBSBuildingComponent();
		if (!BuildingComponent)
		{
			UE_LOG(
				LogSpellRiseBuildInputFacade,
				Warning,
				TEXT("[Construction][Facade] ConfirmBuild rejeitado: componente de construcao ausente. Owner=%s"),
				*GetNameSafe(GetOwner()));
			return;
		}

		if (bRequireBuildMaterialValidation)
		{
			FString RejectReason;
			if (!CanConfirmBuild(BuildingComponent, RejectReason))
			{
				UE_LOG(
					LogSpellRiseBuildInputFacade,
					Warning,
					TEXT("[Construction][Facade] ConfirmBuild rejeitado por validacao de material/contexto. Owner=%s Reason=%s"),
					*GetNameSafe(GetOwner()),
					*RejectReason);
				return;
			}
		}

		CallFirstAvailableNoParamFunction(
			BuildingComponent,
			{
				TEXT("ServerConfirmBuild"),
				TEXT("ServerPlaceBuilding"),
				TEXT("ConfirmBuild"),
				TEXT("PlaceBuilding"),
				TEXT("BuildInteractPressed"),
				TEXT("Build"),
				TEXT("TryBuild")
			});
	}
}

void USpellRiseBuildInputFacade::OnBuildInteractReleased()
{
	if (!CanProcessConstructionCommand())
	{
		return;
	}

	const bool bHandledByNarrative = HandleMalletInteraction(false);
	if (!bHandledByNarrative)
	{
		const AActor* OwnerActor = GetOwner();
		const bool bCanExecuteAuthorityFallback = OwnerActor && OwnerActor->HasAuthority();
		if (!bCanExecuteAuthorityFallback)
		{
			return;
		}

		UObject* BuildingComponent = ResolveEBSBuildingComponent();
		CallFirstAvailableNoParamFunction(
			BuildingComponent,
			{
				TEXT("BuildInteractReleased"),
				TEXT("ReleaseBuild"),
				TEXT("EndBuildInteraction")
			});
	}
}

UActorComponent* USpellRiseBuildInputFacade::ResolveEBSBuildingComponent()
{
	if (CachedEBSBuildingComponent && IsValid(CachedEBSBuildingComponent))
	{
		return CachedEBSBuildingComponent;
	}

	APlayerController* OwnerController = Cast<APlayerController>(GetOwner());
	if (!OwnerController)
	{
		return nullptr;
	}

	TInlineComponentArray<UActorComponent*> Components(OwnerController);
	for (UActorComponent* Component : Components)
	{
		if (!Component || !Component->GetClass())
		{
			continue;
		}

		const FString ClassPath = Component->GetClass()->GetPathName();
		if (ClassPath.Contains(TEXT("EBS_Building"), ESearchCase::IgnoreCase))
		{
			CachedEBSBuildingComponent = Component;
			return CachedEBSBuildingComponent;
		}
	}

	return nullptr;
}

bool USpellRiseBuildInputFacade::CanProcessConstructionCommand() const
{
	return !bRequireConstructionModeForCommands || bConstructionModeActive;
}

bool USpellRiseBuildInputFacade::CallNoParamFunction(UObject* Target, const FName FunctionName) const
{
	if (!Target || FunctionName.IsNone())
	{
		return false;
	}

	UFunction* Function = Target->FindFunction(FunctionName);
	if (!Function)
	{
		return false;
	}

	const int32 ParamsSize = FMath::Max<int32>(1, static_cast<int32>(Function->ParmsSize));
	uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(ParamsSize));
	FMemory::Memzero(ParamsBuffer, ParamsSize);
	Function->InitializeStruct(ParamsBuffer);
	Target->ProcessEvent(Function, ParamsBuffer);
	Function->DestroyStruct(ParamsBuffer);
	return true;
}

bool USpellRiseBuildInputFacade::CallNoParamBoolFunction(UObject* Target, const FName FunctionName, bool& OutResult) const
{
	OutResult = false;

	if (!Target || FunctionName.IsNone())
	{
		return false;
	}

	UFunction* Function = Target->FindFunction(FunctionName);
	if (!Function || !Function->HasAnyFunctionFlags(FUNC_BlueprintCallable))
	{
		return false;
	}

	const int32 ParamsSize = FMath::Max<int32>(1, static_cast<int32>(Function->ParmsSize));
	uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(ParamsSize));
	FMemory::Memzero(ParamsBuffer, ParamsSize);
	Function->InitializeStruct(ParamsBuffer);
	Target->ProcessEvent(Function, ParamsBuffer);

	bool bHasReturnValue = false;
	for (TFieldIterator<FProperty> It(Function); It; ++It)
	{
		FProperty* Property = *It;
		if (!Property || !Property->HasAnyPropertyFlags(CPF_ReturnParm))
		{
			continue;
		}

		if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
		{
			const void* ValuePtr = BoolProperty->ContainerPtrToValuePtr<void>(ParamsBuffer);
			OutResult = BoolProperty->GetPropertyValue(ValuePtr);
			bHasReturnValue = true;
			break;
		}
	}

	Function->DestroyStruct(ParamsBuffer);
	return bHasReturnValue;
}

bool USpellRiseBuildInputFacade::CallFirstAvailableNoParamFunction(UObject* Target, std::initializer_list<FName> FunctionNames) const
{
	if (!Target)
	{
		return false;
	}

	for (const FName FunctionName : FunctionNames)
	{
		if (CallNoParamFunction(Target, FunctionName))
		{
			return true;
		}
	}

	return false;
}

bool USpellRiseBuildInputFacade::GetBooleanProperty(UObject* Target, const TCHAR* PropertyName, bool& OutValue) const
{
	if (!Target)
	{
		return false;
	}

	FProperty* Property = Target->GetClass()->FindPropertyByName(FName(PropertyName));
	if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		const void* ValuePtr = BoolProperty->ContainerPtrToValuePtr<void>(Target);
		OutValue = BoolProperty->GetPropertyValue(ValuePtr);
		return true;
	}

	return false;
}

bool USpellRiseBuildInputFacade::SetBooleanProperty(UObject* Target, const TCHAR* PropertyName, bool bValue) const
{
	if (!Target)
	{
		return false;
	}

	FProperty* Property = Target->GetClass()->FindPropertyByName(FName(PropertyName));
	if (!Property)
	{
		return false;
	}

	void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Target);
	return SetBoolCompatibleProperty(Property, ValuePtr, bValue);
}

bool USpellRiseBuildInputFacade::ToggleBooleanPropertyAndSubmit(UObject* Target, const TCHAR* PropertyName, const FName FunctionName, const TCHAR* ParamName) const
{
	if (!Target || !PropertyName || FunctionName.IsNone() || !ParamName)
	{
		return false;
	}

	const FBoolProperty* BoolProperty = FindFProperty<FBoolProperty>(Target->GetClass(), PropertyName);
	if (!BoolProperty)
	{
		return false;
	}

	const bool bCurrentValue = BoolProperty->GetPropertyValue_InContainer(Target);
	const bool bNewValue = !bCurrentValue;

	UFunction* Function = Target->FindFunction(FunctionName);
	if (!Function)
	{
		return false;
	}

	const int32 ParamsSize = FMath::Max<int32>(1, static_cast<int32>(Function->ParmsSize));
	uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(ParamsSize));
	FMemory::Memzero(ParamsBuffer, ParamsSize);
	Function->InitializeStruct(ParamsBuffer);

	bool bParamSet = false;
	for (TFieldIterator<FProperty> It(Function); It; ++It)
	{
		FProperty* Property = *It;
		if (!Property || !Property->HasAnyPropertyFlags(CPF_Parm) || Property->HasAnyPropertyFlags(CPF_ReturnParm))
		{
			continue;
		}

		if (Property->GetName().Equals(ParamName, ESearchCase::IgnoreCase))
		{
			void* ValuePtr = Property->ContainerPtrToValuePtr<void>(ParamsBuffer);
			bParamSet = SetBoolCompatibleProperty(Property, ValuePtr, bNewValue);
			break;
		}
	}

	if (!bParamSet)
	{
		Function->DestroyStruct(ParamsBuffer);
		return false;
	}

	Target->ProcessEvent(Function, ParamsBuffer);
	Function->DestroyStruct(ParamsBuffer);
	return true;
}

bool USpellRiseBuildInputFacade::CallOperationFunction(UObject* Target, const FName FunctionName, bool bIncrease, float Value) const
{
	if (!Target || FunctionName.IsNone())
	{
		return false;
	}

	UFunction* Function = Target->FindFunction(FunctionName);
	if (!Function)
	{
		return false;
	}

	const int32 ParamsSize = FMath::Max<int32>(1, static_cast<int32>(Function->ParmsSize));
	uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(ParamsSize));
	FMemory::Memzero(ParamsBuffer, ParamsSize);
	Function->InitializeStruct(ParamsBuffer);

	bool bOperationSet = false;
	bool bValueSet = false;

	for (TFieldIterator<FProperty> It(Function); It; ++It)
	{
		FProperty* Property = *It;
		if (!Property || !Property->HasAnyPropertyFlags(CPF_Parm) || Property->HasAnyPropertyFlags(CPF_ReturnParm))
		{
			continue;
		}

		// Out/ref params in reflective calls are too risky for this generic bridge.
		if (Property->HasAnyPropertyFlags(CPF_OutParm) || Property->HasAnyPropertyFlags(CPF_ReferenceParm))
		{
			Function->DestroyStruct(ParamsBuffer);
			return false;
		}

		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(ParamsBuffer);
		const FString PropertyName = Property->GetName();
		if (PropertyName.Contains(TEXT("Operation"), ESearchCase::IgnoreCase))
		{
			bOperationSet = SetIncreaseEnumCompatibleProperty(Property, ValuePtr, bIncrease);
		}
		else if (PropertyName.Contains(TEXT("Value"), ESearchCase::IgnoreCase))
		{
			bValueSet = SetFloatCompatibleProperty(Property, ValuePtr, Value);
		}
		else if (PropertyName.Contains(TEXT("UpdateVisibility"), ESearchCase::IgnoreCase))
		{
			// Optional EBS param used by some variants.
			SetBoolCompatibleProperty(Property, ValuePtr, true);
		}
		else
		{
			// Accept only primitive optional params to avoid unsafe ProcessEvent payloads.
			const bool bIsPrimitiveOptional =
				CastField<FBoolProperty>(Property) ||
				CastField<FByteProperty>(Property) ||
				CastField<FIntProperty>(Property) ||
				CastField<FFloatProperty>(Property) ||
				CastField<FDoubleProperty>(Property);

			if (!bIsPrimitiveOptional)
			{
				Function->DestroyStruct(ParamsBuffer);
				return false;
			}
		}
	}

	// Must at least match operation/value contract.
	if (!bOperationSet || !bValueSet)
	{
		Function->DestroyStruct(ParamsBuffer);
		return false;
	}

	Target->ProcessEvent(Function, ParamsBuffer);
	Function->DestroyStruct(ParamsBuffer);
	return true;
}

bool USpellRiseBuildInputFacade::CallDisplayedFloorFunction(bool bIncrease)
{
	UObject* BuildingComponent = ResolveEBSBuildingComponent();
	if (!BuildingComponent)
	{
		return false;
	}

	UFunction* Function = BuildingComponent->FindFunction(TEXT("ChangeDisplayedFloor"));
	if (!Function)
	{
		return false;
	}

	const int32 ParamsSize = FMath::Max<int32>(1, static_cast<int32>(Function->ParmsSize));
	uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(ParamsSize));
	FMemory::Memzero(ParamsBuffer, ParamsSize);
	Function->InitializeStruct(ParamsBuffer);

	bool bOperationSet = false;
	bool bValueSet = false;
	bool bVisibilitySet = false;

	for (TFieldIterator<FProperty> It(Function); It; ++It)
	{
		FProperty* Property = *It;
		if (!Property || !Property->HasAnyPropertyFlags(CPF_Parm) || Property->HasAnyPropertyFlags(CPF_ReturnParm))
		{
			continue;
		}

		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(ParamsBuffer);
		const FString PropertyName = Property->GetName();

		if (PropertyName.Contains(TEXT("Operation"), ESearchCase::IgnoreCase))
		{
			bOperationSet = SetIncreaseEnumCompatibleProperty(Property, ValuePtr, bIncrease);
		}
		else if (PropertyName.Contains(TEXT("Value"), ESearchCase::IgnoreCase))
		{
			bValueSet = SetFloatCompatibleProperty(Property, ValuePtr, static_cast<float>(FMath::Max(1, FloorStep)));
		}
		else if (PropertyName.Contains(TEXT("UpdateVisibility"), ESearchCase::IgnoreCase))
		{
			bVisibilitySet = SetBoolCompatibleProperty(Property, ValuePtr, true);
		}
	}

	if (!bOperationSet || !bValueSet)
	{
		Function->DestroyStruct(ParamsBuffer);
		return false;
	}

	if (!bVisibilitySet)
	{
		UE_LOG(LogSpellRiseBuildInputFacade, Verbose, TEXT("[Construction][Facade] ChangeDisplayedFloor sem parametro UpdateVisibility."));
	}

	BuildingComponent->ProcessEvent(Function, ParamsBuffer);
	Function->DestroyStruct(ParamsBuffer);
	return true;
}

bool USpellRiseBuildInputFacade::HandleMalletInteraction(bool bPressed)
{
	APlayerController* OwnerController = Cast<APlayerController>(GetOwner());
	APawn* Pawn = OwnerController ? OwnerController->GetPawn() : nullptr;
	if (!Pawn)
	{
		return false;
	}

	if (!NarrativeBuildBridge)
	{
		NarrativeBuildBridge = GetOwner() ? GetOwner()->FindComponentByClass<USpellRiseNarrativeBuildBridge>() : nullptr;
	}

	if (!NarrativeBuildBridge)
	{
		return false;
	}

	AActor* TargetActor = nullptr;
	const bool bInteracted = NarrativeBuildBridge->TryBuildInteract(Pawn, bPressed, TargetActor);
	if (!bInteracted)
	{
		return false;
	}

	if (bPressed)
	{
		return NarrativeBuildBridge->TryOpenMalletMenu(ResolveEBSBuildingComponent(), TargetActor);
	}

	return NarrativeBuildBridge->TryCloseMalletMenu(ResolveEBSBuildingComponent());
}

bool USpellRiseBuildInputFacade::CanConfirmBuild(UObject* BuildingComponent, FString& OutRejectReason) const
{
	OutRejectReason.Reset();

	if (!BuildingComponent)
	{
		OutRejectReason = TEXT("missing_build_component");
		return false;
	}

	auto CheckBoolPropertyFalse = [&](const TCHAR* PropertyName, const TCHAR* Reason) -> bool
	{
		bool bValue = true;
		if (GetBooleanProperty(BuildingComponent, PropertyName, bValue) && !bValue)
		{
			OutRejectReason = Reason;
			return true;
		}
		return false;
	};

	auto CheckObjectPropertyNull = [&](const TCHAR* PropertyName, const TCHAR* Reason) -> bool
	{
		FProperty* Property = BuildingComponent->GetClass()->FindPropertyByName(FName(PropertyName));
		if (!Property)
		{
			return false;
		}

		if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
		{
			const void* ValuePtr = ObjectProperty->ContainerPtrToValuePtr<void>(BuildingComponent);
			if (!ObjectProperty->GetObjectPropertyValue(ValuePtr))
			{
				OutRejectReason = Reason;
				return true;
			}
		}

		return false;
	};

	if (CheckObjectPropertyNull(TEXT("BuildingObject"), TEXT("BuildingObject=null")) ||
		CheckObjectPropertyNull(TEXT("CurrentBuildingObject"), TEXT("CurrentBuildingObject=null")) ||
		CheckObjectPropertyNull(TEXT("SelectedBuilding"), TEXT("SelectedBuilding=null")))
	{
		return false;
	}

	if (CheckBoolPropertyFalse(TEXT("CanBuild"), TEXT("CanBuild=false")) ||
		CheckBoolPropertyFalse(TEXT("bCanBuild"), TEXT("bCanBuild=false")) ||
		CheckBoolPropertyFalse(TEXT("CanPlace"), TEXT("CanPlace=false")) ||
		CheckBoolPropertyFalse(TEXT("bCanPlace"), TEXT("bCanPlace=false")) ||
		CheckBoolPropertyFalse(TEXT("HasRequiredResources"), TEXT("HasRequiredResources=false")) ||
		CheckBoolPropertyFalse(TEXT("bHasRequiredResources"), TEXT("bHasRequiredResources=false")) ||
		CheckBoolPropertyFalse(TEXT("HasEnoughResources"), TEXT("HasEnoughResources=false")) ||
		CheckBoolPropertyFalse(TEXT("bHasEnoughResources"), TEXT("bHasEnoughResources=false")) ||
		CheckBoolPropertyFalse(TEXT("HasMaterials"), TEXT("HasMaterials=false")) ||
		CheckBoolPropertyFalse(TEXT("bHasMaterials"), TEXT("bHasMaterials=false")))
	{
		return false;
	}

	bool bBoolResult = true;
	if (CallNoParamBoolFunction(BuildingComponent, TEXT("CanBuild"), bBoolResult) && !bBoolResult)
	{
		OutRejectReason = TEXT("CanBuild()=false");
		return false;
	}
	if (CallNoParamBoolFunction(BuildingComponent, TEXT("CanPlace"), bBoolResult) && !bBoolResult)
	{
		OutRejectReason = TEXT("CanPlace()=false");
		return false;
	}
	if (CallNoParamBoolFunction(BuildingComponent, TEXT("HasRequiredResources"), bBoolResult) && !bBoolResult)
	{
		OutRejectReason = TEXT("HasRequiredResources()=false");
		return false;
	}
	if (CallNoParamBoolFunction(BuildingComponent, TEXT("HasEnoughResources"), bBoolResult) && !bBoolResult)
	{
		OutRejectReason = TEXT("HasEnoughResources()=false");
		return false;
	}

	return true;
}

