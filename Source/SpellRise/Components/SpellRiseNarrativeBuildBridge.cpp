#include "SpellRise/Components/SpellRiseNarrativeBuildBridge.h"

#include "GameFramework/Pawn.h"
#include "InteractionComponent.h"
#include "InteractableComponent.h"
#include "UObject/UnrealType.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseNarrativeBuildBridge, Log, All);

USpellRiseNarrativeBuildBridge::USpellRiseNarrativeBuildBridge()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool USpellRiseNarrativeBuildBridge::TryBuildInteract(APawn* InteractingPawn, bool bPressed, AActor*& OutTargetActor)
{
	OutTargetActor = nullptr;

	UNarrativeInteractionComponent* InteractionComponent = ResolveNarrativeInteractionComponent(InteractingPawn);
	if (!InteractionComponent)
	{
		return false;
	}

	OutTargetActor = ResolveCurrentNarrativeTargetActor(InteractionComponent);

	bool bDispatchedInteraction = false;
	if (bPressed)
	{
		if (UFunction* BeginInteractFunction = InteractionComponent->FindFunction(TEXT("BeginInteract")))
		{
			InteractionComponent->ProcessEvent(BeginInteractFunction, nullptr);
			bDispatchedInteraction = true;
		}
	}
	else
	{
		if (UFunction* EndInteractFunction = InteractionComponent->FindFunction(TEXT("EndInteract")))
		{
			InteractionComponent->ProcessEvent(EndInteractFunction, nullptr);
			bDispatchedInteraction = true;
		}
	}

	return bDispatchedInteraction;
}

bool USpellRiseNarrativeBuildBridge::TryOpenMalletMenu(UActorComponent* BuildingComponent, AActor* TargetActor)
{
	if (!BuildingComponent || !TargetActor)
	{
		return false;
	}

	UFunction* ShowMalletMenuFunction = BuildingComponent->FindFunction(TEXT("ShowMalletMenu"));
	if (!ShowMalletMenuFunction)
	{
		return false;
	}

	const int32 ParamsSize = FMath::Max<int32>(1, static_cast<int32>(ShowMalletMenuFunction->ParmsSize));
	uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(ParamsSize));
	FMemory::Memzero(ParamsBuffer, ParamsSize);
	ShowMalletMenuFunction->InitializeStruct(ParamsBuffer);

	bool bTargetSet = false;
	for (TFieldIterator<FProperty> It(ShowMalletMenuFunction); It; ++It)
	{
		FProperty* Property = *It;
		if (!Property || !Property->HasAnyPropertyFlags(CPF_Parm) || Property->HasAnyPropertyFlags(CPF_ReturnParm))
		{
			continue;
		}

		if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
		{
			const FString PropertyName = ObjectProperty->GetName();
			if (PropertyName.Contains(TEXT("TargetActor"), ESearchCase::IgnoreCase) ||
				PropertyName.Equals(TEXT("Target"), ESearchCase::IgnoreCase))
			{
				ObjectProperty->SetObjectPropertyValue_InContainer(ParamsBuffer, TargetActor);
				bTargetSet = true;
			}
		}
	}

	if (!bTargetSet)
	{
		ShowMalletMenuFunction->DestroyStruct(ParamsBuffer);
		UE_LOG(LogSpellRiseNarrativeBuildBridge, Warning, TEXT("[Construction][NarrativeBridge] ShowMalletMenu sem parametro de alvo em %s"), *GetNameSafe(BuildingComponent));
		return false;
	}

	BuildingComponent->ProcessEvent(ShowMalletMenuFunction, ParamsBuffer);
	ShowMalletMenuFunction->DestroyStruct(ParamsBuffer);
	return true;
}

bool USpellRiseNarrativeBuildBridge::TryCloseMalletMenu(UActorComponent* BuildingComponent)
{
	if (!BuildingComponent)
	{
		return false;
	}

	UFunction* HideMalletMenuFunction = BuildingComponent->FindFunction(TEXT("HideMalletMenu"));
	if (!HideMalletMenuFunction)
	{
		return false;
	}

	const int32 ParamsSize = FMath::Max<int32>(1, static_cast<int32>(HideMalletMenuFunction->ParmsSize));
	uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(ParamsSize));
	FMemory::Memzero(ParamsBuffer, ParamsSize);
	HideMalletMenuFunction->InitializeStruct(ParamsBuffer);
	BuildingComponent->ProcessEvent(HideMalletMenuFunction, ParamsBuffer);
	HideMalletMenuFunction->DestroyStruct(ParamsBuffer);
	return true;
}

UNarrativeInteractionComponent* USpellRiseNarrativeBuildBridge::ResolveNarrativeInteractionComponent(APawn* InteractingPawn) const
{
	if (!InteractingPawn)
	{
		return nullptr;
	}

	return InteractingPawn->FindComponentByClass<UNarrativeInteractionComponent>();
}

AActor* USpellRiseNarrativeBuildBridge::ResolveCurrentNarrativeTargetActor(const UNarrativeInteractionComponent* InteractionComponent) const
{
	if (!InteractionComponent)
	{
		return nullptr;
	}

	const FObjectProperty* CurrentInteractableProperty = FindFProperty<FObjectProperty>(
		InteractionComponent->GetClass(),
		TEXT("CurrentInteractable"));

	if (!CurrentInteractableProperty)
	{
		return nullptr;
	}

	UObject* InteractableObject = CurrentInteractableProperty->GetObjectPropertyValue_InContainer(InteractionComponent);
	const UNarrativeInteractableComponent* Interactable = Cast<UNarrativeInteractableComponent>(InteractableObject);
	return Interactable ? Interactable->GetOwner() : nullptr;
}
