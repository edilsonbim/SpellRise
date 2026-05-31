// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRisePlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InteractionComponent.h"
#include "Misc/DateTime.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UObject/UnrealType.h"

#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent.h"
#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent_NiagaraText.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRisePlayerControllerRuntime, Log, All);

namespace
{
	const FName NAME_SendChatToSERVER(TEXT("SendChatToSERVER"));
	constexpr int32 MaxChatNameChars = 32;
	constexpr int32 MaxChatTextChars = 256;
	constexpr int32 MaxCombatActorNameChars = 64;
	constexpr int32 MaxCombatLogMessageChars = 192;

	const FGameplayTag& InputTag_Primary()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Primary"));
		return Tag;
	}

	const FGameplayTag& InputTag_Secondary()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Secondary"));
		return Tag;
	}

	const FGameplayTag& InputTag_Interact()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Utility.Interact"));
		return Tag;
	}

	const FGameplayTag& InputTag_Sprint()
	{
		static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Utility.Sprint"));
		return Tag;
	}

	FText BuildCombatTimeText()
	{
		const FDateTime Now = FDateTime::Now();
		return FText::FromString(Now.ToString(TEXT("%H:%M:%S")));
	}

	FString SanitizeBoundedString(FString Value, const int32 MaxChars, const TCHAR* Fallback = TEXT(""))
	{
		Value.TrimStartAndEndInline();
		Value.ReplaceInline(TEXT("\r"), TEXT(" "));
		Value.ReplaceInline(TEXT("\n"), TEXT(" "));
		Value.ReplaceInline(TEXT("\t"), TEXT(" "));

		const int32 SafeMaxChars = FMath::Max(1, MaxChars);
		if (Value.Len() > SafeMaxChars)
		{
			Value.LeftInline(SafeMaxChars, EAllowShrinking::No);
		}

		if (Value.IsEmpty() && Fallback)
		{
			Value = Fallback;
		}

		return Value;
	}

	FSpellRiseChatMessage SanitizeChatMessageForLocalDelivery(const FSpellRiseChatMessage& Message)
	{
		FSpellRiseChatMessage Sanitized = Message;
		Sanitized.Name = FName(*SanitizeBoundedString(Message.Name.ToString(), MaxChatNameChars, TEXT("System")));
		Sanitized.Text = FText::FromString(SanitizeBoundedString(Message.Text.ToString(), MaxChatTextChars));
		Sanitized.TimeText = FText::FromString(SanitizeBoundedString(Message.TimeText.ToString(), 16));
		return Sanitized;
	}

	bool TryReadChannelPropertyValue(const FProperty* Property, const void* ValuePtr, FString& OutChannelName, int64& OutChannelValue)
	{
		if (!Property || !ValuePtr)
		{
			return false;
		}

		if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
		{
			if (const FNumericProperty* UnderlyingNumeric = EnumProperty->GetUnderlyingProperty())
			{
				OutChannelValue = UnderlyingNumeric->GetSignedIntPropertyValue(ValuePtr);
				if (const UEnum* Enum = EnumProperty->GetEnum())
				{
					OutChannelName = Enum->GetNameStringByValue(OutChannelValue);
				}
				return true;
			}
		}

		if (const FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
		{
			OutChannelValue = ByteProperty->GetPropertyValue(ValuePtr);
			if (ByteProperty->Enum)
			{
				OutChannelName = ByteProperty->Enum->GetNameStringByValue(OutChannelValue);
			}
			return true;
		}

		if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
		{
			OutChannelName = NameProperty->GetPropertyValue(ValuePtr).ToString();
			return true;
		}

		if (const FStrProperty* StrProperty = CastField<FStrProperty>(Property))
		{
			OutChannelName = StrProperty->GetPropertyValue(ValuePtr);
			return true;
		}

		if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
		{
			OutChannelName = TextProperty->GetPropertyValue(ValuePtr).ToString();
			return true;
		}

		return false;
	}

	bool ExtractChannelFromPropertyRecursive(const FProperty* Property, const void* ContainerPtr, FString& OutChannelName, int64& OutChannelValue)
	{
		if (!Property || !ContainerPtr)
		{
			return false;
		}

		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(const_cast<void*>(ContainerPtr));

		if (Property->GetName().Contains(TEXT("Channel"), ESearchCase::IgnoreCase) &&
			TryReadChannelPropertyValue(Property, ValuePtr, OutChannelName, OutChannelValue))
		{
			return true;
		}

		if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			const void* StructValuePtr = StructProperty->ContainerPtrToValuePtr<void>(const_cast<void*>(ContainerPtr));

			for (TFieldIterator<FProperty> It(StructProperty->Struct); It; ++It)
			{
				if (ExtractChannelFromPropertyRecursive(*It, StructValuePtr, OutChannelName, OutChannelValue))
				{
					return true;
				}
			}
		}

		return false;
	}

	bool IsCombatChannelDescriptor(const FString& ChannelName, int64 ChannelValue)
	{
		if (ChannelName.Contains(TEXT("Combat"), ESearchCase::IgnoreCase))
		{
			return true;
		}

		return ChannelValue == 3;
	}

	bool SR_IsAbilityInputTag(const FGameplayTag& InputTag)
	{
		if (!InputTag.IsValid())
		{
			return false;
		}

		return InputTag.ToString().StartsWith(TEXT("InputTag.Ability."));
	}

	bool SR_CanSendPawnOwnedInputRpc(const APlayerController* PlayerController, const APawn* Pawn, FString* OutReason = nullptr)
	{
		if (!PlayerController)
		{
			if (OutReason)
			{
				*OutReason = TEXT("missing_player_controller");
			}
			return false;
		}

		if (!PlayerController->IsLocalController())
		{
			if (OutReason)
			{
				*OutReason = TEXT("controller_not_local");
			}
			return false;
		}

		if (!Pawn)
		{
			if (OutReason)
			{
				*OutReason = TEXT("missing_pawn");
			}
			return false;
		}

		if (Pawn->GetController() != PlayerController)
		{
			if (OutReason)
			{
				*OutReason = TEXT("pawn_controller_mismatch");
			}
			return false;
		}

		if (!Pawn->IsLocallyControlled())
		{
			if (OutReason)
			{
				*OutReason = TEXT("pawn_not_locally_controlled");
			}
			return false;
		}

		return true;
	}

	void UpdateEnhancedInputContext(
		UEnhancedInputLocalPlayerSubsystem* Subsystem,
		UInputMappingContext* Context,
		int32 Priority,
		bool bShouldBeActive,
		const TCHAR* ContextLabel)
	{
		if (!Subsystem || !Context || !ContextLabel)
		{
			return;
		}

		const bool bHasContext = Subsystem->HasMappingContext(Context);
		if (bShouldBeActive)
		{
			if (!bHasContext)
			{
				Subsystem->AddMappingContext(Context, Priority);
			}

			return;
		}

		if (bHasContext)
		{
			Subsystem->RemoveMappingContext(Context);
		}
	}
}

ASpellRisePlayerController::ASpellRisePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bShowMouseCursor = false;
	NumberPopComponent = CreateDefaultSubobject<USpellRiseNumberPopComponent_NiagaraText>(TEXT("NumberPopComponent"));
}

void ASpellRisePlayerController::ProcessEvent(UFunction* Function, void* Parameters)
{
	if (Function && IsControlledCharacterDead())
	{
		const FString FunctionName = Function->GetName();
		if (FunctionName.Contains(TEXT("InpActEvt_"), ESearchCase::CaseSensitive) ||
			FunctionName.Contains(TEXT("EnhancedInputAction"), ESearchCase::IgnoreCase))
		{
			return;
		}
	}

	if (HasAuthority() &&
		Function &&
		Function->GetFName() == NAME_SendChatToSERVER &&
		Function->HasAnyFunctionFlags(FUNC_NetServer))
	{
		FString ChannelName;
		int64 ChannelValue = INDEX_NONE;
		bool bFoundChannel = false;

		for (TFieldIterator<FProperty> It(Function); It; ++It)
		{
			FProperty* Property = *It;
			if (!Property->HasAnyPropertyFlags(CPF_Parm) || Property->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				continue;
			}

			if (ExtractChannelFromPropertyRecursive(Property, Parameters, ChannelName, ChannelValue))
			{
				bFoundChannel = true;
				break;
			}
		}

		if (bFoundChannel && IsCombatChannelDescriptor(ChannelName, ChannelValue))
		{
			ClientMessage(TEXT("Canal Combat e somente leitura."));
			return;
		}
	}

	Super::ProcessEvent(Function, Parameters);
}

void ASpellRisePlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(GetNetMode() != NM_DedicatedServer && IsLocalController());

	if (IsLocalController())
	{
		FInputModeGameOnly Mode;
		SetInputMode(Mode);
		LogInputFocusSnapshot(TEXT("BeginPlay.SetInputModeGameOnly"));
	}

	SetupEnhancedInput();

	if (APawn* InitialPawn = GetPawn())
	{
		HandlePawnChangedRuntime(InitialPawn, TEXT("BeginPlay"));
	}

}

bool ASpellRisePlayerController::CanRunLocalHUDFlow(FString* OutSkipReason) const
{
	auto SetSkipReason = [&](const TCHAR* Reason)
	{
		if (OutSkipReason)
		{
			*OutSkipReason = Reason;
		}
		return false;
	};

	if (GetNetMode() == NM_DedicatedServer)
	{
		return SetSkipReason(TEXT("dedicated_server"));
	}

	if (!IsLocalController())
	{
		return SetSkipReason(TEXT("non_local_controller"));
	}

	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return SetSkipReason(TEXT("local_player_missing"));
	}

	if (!LocalPlayer->ViewportClient)
	{
		return SetSkipReason(TEXT("viewport_client_missing"));
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return SetSkipReason(TEXT("world_missing"));
	}

	return true;
}

void ASpellRisePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const TCHAR* EndPlayReasonText = TEXT("Unknown");
	switch (EndPlayReason)
	{
	case EEndPlayReason::Destroyed:
		EndPlayReasonText = TEXT("Destroyed");
		break;
	case EEndPlayReason::LevelTransition:
		EndPlayReasonText = TEXT("LevelTransition");
		break;
	case EEndPlayReason::EndPlayInEditor:
		EndPlayReasonText = TEXT("EndPlayInEditor");
		break;
	case EEndPlayReason::RemovedFromWorld:
		EndPlayReasonText = TEXT("RemovedFromWorld");
		break;
	case EEndPlayReason::Quit:
		EndPlayReasonText = TEXT("Quit");
		break;
	default:
		break;
	}

	Super::EndPlay(EndPlayReason);
}

void ASpellRisePlayerController::SetupEnhancedInput()
{
	if (!IsLocalController())
	{
		return;
	}

	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	const bool bHasEnhancedInputContexts =
		IMC_CoreMovement.Get() ||
		IMC_CoreCamera.Get() ||
		IMC_Combat.Get() ||
		IMC_Interaction.Get() ||
		IMC_UI.Get() ||
		IMC_System.Get();

	if (bHasEnhancedInputContexts)
	{
		RefreshEnhancedInputContexts();
		return;
	}

	UInputMappingContext* MappingContextToApply = DefaultMappingContext.Get();
	int32 MappingPriority = DefaultMappingPriority;
	if (!MappingContextToApply)
	{
		if (const ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn()))
		{
			MappingContextToApply = const_cast<UInputMappingContext*>(ControlledCharacter->GetDefaultInputMappingContext());
			MappingPriority = 0;
		}
	}

	if (!MappingContextToApply)
	{
		return;
	}

	if (!Subsystem->HasMappingContext(MappingContextToApply))
	{
		Subsystem->AddMappingContext(MappingContextToApply, MappingPriority);
	}
}

void ASpellRisePlayerController::RefreshEnhancedInputContexts()
{
	if (!IsLocalController())
	{
		return;
	}

	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	const bool bUIInputContextActive = ShouldEnableUIInputContext();

	UpdateEnhancedInputContext(Subsystem, IMC_CoreMovement.Get(), IMC_CoreMovementPriority, true, TEXT("CoreMovement"));
	UpdateEnhancedInputContext(Subsystem, IMC_CoreCamera.Get(), IMC_CoreCameraPriority, true, TEXT("CoreCamera"));
	UpdateEnhancedInputContext(Subsystem, IMC_Combat.Get(), IMC_CombatPriority, true, TEXT("Combat"));
	UpdateEnhancedInputContext(Subsystem, IMC_Interaction.Get(), IMC_InteractionPriority, true, TEXT("Interaction"));
	UpdateEnhancedInputContext(Subsystem, IMC_UI.Get(), IMC_UIPriority, bUIInputContextActive, TEXT("UI"));
	UpdateEnhancedInputContext(Subsystem, IMC_System.Get(), IMC_SystemPriority, true, TEXT("System"));
}

void ASpellRisePlayerController::InventorySplitSlotSERVER_Implementation(UObject* FromContainer, int32 Slot, int32 Amount)
{
	if (!FromContainer || Slot < 0 || Amount <= 0)
	{
		AuditRejectedInventoryRpc(TEXT("InventorySplitSlotSERVER"), TEXT("invalid_payload"));
		return;
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose,
		TEXT("InventorySplitSlotSERVER ainda nao implementado no PlayerController. Controller=%s Container=%s"),
		*GetNameSafe(this),
		*GetNameSafe(FromContainer));
}

void ASpellRisePlayerController::OnInventorySlotDropSERVER_Implementation(UObject* FromContainer, UObject* ToInventoryComponent, int32 FromIndex, int32 ToIndex)
{
	if (!FromContainer || FromIndex < 0 || ToIndex < 0)
	{
		AuditRejectedInventoryRpc(TEXT("OnInventorySlotDropSERVER"), TEXT("invalid_payload"));
		return;
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose,
		TEXT("OnInventorySlotDropSERVER ainda nao implementado no PlayerController. Controller=%s From=%s To=%s"),
		*GetNameSafe(this),
		*GetNameSafe(FromContainer),
		*GetNameSafe(ToInventoryComponent));
}

void ASpellRisePlayerController::Route_InventorySortBy_SERVER_Implementation(UObject* InventoryRef, int32 SortBy)
{
	constexpr int32 MinSupportedSortMode = 0;
	constexpr int32 MaxSupportedSortMode = 16;
	if (!InventoryRef || SortBy < MinSupportedSortMode || SortBy > MaxSupportedSortMode)
	{
		AuditRejectedInventoryRpc(TEXT("Route_InventorySortBy_SERVER"), TEXT("invalid_payload"));
		return;
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose,
		TEXT("Route_InventorySortBy_SERVER ainda nao implementado no PlayerController. Controller=%s Inventory=%s SortBy=%d"),
		*GetNameSafe(this),
		*GetNameSafe(InventoryRef),
		SortBy);
}

void ASpellRisePlayerController::AuditRejectedInventoryRpc(const TCHAR* RpcName, const FString& RejectReason)
{
	const FString CounterKey = FString::Printf(TEXT("%s|%s"), RpcName ? RpcName : TEXT("unknown"), *RejectReason);
	const int32 RejectCount = ++RejectedInventoryRpcCountByReason.FindOrAdd(CounterKey);
	UE_LOG(LogSpellRisePlayerControllerRuntime, Warning,
		TEXT("[RPC][Rejected] Rpc=%s Reason=%s Count=%d Controller=%s PlayerState=%s Pawn=%s"),
		RpcName ? RpcName : TEXT("unknown"),
		*RejectReason,
		RejectCount,
		*GetNameSafe(this),
		*GetNameSafe(PlayerState.Get()),
		*GetNameSafe(GetPawn()));
}


void ASpellRisePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalController())
	{
		return;
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EIC)
	{
		return;
	}

	if (!IA_Attack)
	{

	}
	else
	{
		EIC->BindAction(IA_Attack, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAttackPressed);
		EIC->BindAction(IA_Attack, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAttackReleased);
		EIC->BindAction(IA_Attack, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAttackReleased);
	}

	if (IA_Primary)
	{
		EIC->BindAction(IA_Primary, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnPrimaryPressed);
		EIC->BindAction(IA_Primary, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnPrimaryReleased);
		EIC->BindAction(IA_Primary, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnPrimaryReleased);
	}

	if (IA_Secondary)
	{
		EIC->BindAction(IA_Secondary, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnSecondaryPressed);
		EIC->BindAction(IA_Secondary, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnSecondaryReleased);
		EIC->BindAction(IA_Secondary, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnSecondaryReleased);
	}

	if (IA_Interact)
	{
		EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnInteractPressed);
		EIC->BindAction(IA_Interact, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnInteractReleased);
		EIC->BindAction(IA_Interact, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnInteractReleased);
	}

	if (IA_ClearSelection)
	{
		EIC->BindAction(IA_ClearSelection, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnClearSelectionPressed);
	}

	if (IA_Sprint)
	{
		EIC->BindAction(IA_Sprint, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnSprintPressed);
		EIC->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnSprintReleased);
		EIC->BindAction(IA_Sprint, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnSprintReleased);
	}

	if (IA_Ability1)
	{
		EIC->BindAction(IA_Ability1, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility1Pressed);
		EIC->BindAction(IA_Ability1, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility1Released);
		EIC->BindAction(IA_Ability1, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility1Released);
	}

	if (IA_Ability2)
	{
		EIC->BindAction(IA_Ability2, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility2Pressed);
		EIC->BindAction(IA_Ability2, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility2Released);
		EIC->BindAction(IA_Ability2, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility2Released);
	}

	if (IA_Ability3)
	{
		EIC->BindAction(IA_Ability3, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility3Pressed);
		EIC->BindAction(IA_Ability3, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility3Released);
		EIC->BindAction(IA_Ability3, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility3Released);
	}

	if (IA_Ability4)
	{
		EIC->BindAction(IA_Ability4, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility4Pressed);
		EIC->BindAction(IA_Ability4, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility4Released);
		EIC->BindAction(IA_Ability4, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility4Released);
	}

	if (IA_Ability5)
	{
		EIC->BindAction(IA_Ability5, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility5Pressed);
		EIC->BindAction(IA_Ability5, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility5Released);
		EIC->BindAction(IA_Ability5, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility5Released);
	}

	if (IA_Ability6)
	{
		EIC->BindAction(IA_Ability6, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility6Pressed);
		EIC->BindAction(IA_Ability6, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility6Released);
		EIC->BindAction(IA_Ability6, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility6Released);
	}

	if (IA_Ability7)
	{
		EIC->BindAction(IA_Ability7, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility7Pressed);
		EIC->BindAction(IA_Ability7, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility7Released);
		EIC->BindAction(IA_Ability7, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility7Released);
	}

	if (IA_Ability8)
	{
		EIC->BindAction(IA_Ability8, ETriggerEvent::Started, this, &ASpellRisePlayerController::OnAbility8Pressed);
		EIC->BindAction(IA_Ability8, ETriggerEvent::Completed, this, &ASpellRisePlayerController::OnAbility8Released);
		EIC->BindAction(IA_Ability8, ETriggerEvent::Canceled, this, &ASpellRisePlayerController::OnAbility8Released);
	}

}

void ASpellRisePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	LastSpellRiseControlledPawn = InPawn;
	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[Control][PCOnPossess] Controller=%s Pawn=%s PlayerState=%s Local=%d Authority=%d"),
		*GetNameSafe(this),
		*GetNameSafe(InPawn),
		*GetNameSafe(PlayerState.Get()),
		IsLocalController() ? 1 : 0,
		HasAuthority() ? 1 : 0);
	HandlePawnChangedRuntime(InPawn, TEXT("OnPossess"));
}

APawn* ASpellRisePlayerController::GetLastSpellRiseControlledPawn() const
{
	return LastSpellRiseControlledPawn.Get();
}

void ASpellRisePlayerController::OnUnPossess()
{
	APawn* PreviousPawn = GetPawn();
	Super::OnUnPossess();
	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[Control][PCOnUnPossess] Controller=%s PreviousPawn=%s PlayerState=%s Local=%d Authority=%d"),
		*GetNameSafe(this),
		*GetNameSafe(PreviousPawn),
		*GetNameSafe(PlayerState.Get()),
		IsLocalController() ? 1 : 0,
		HasAuthority() ? 1 : 0);
	HandlePawnChangedRuntime(nullptr, TEXT("OnUnPossess"));

}

void ASpellRisePlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);
	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[Control][PCAcknowledgePossession] Controller=%s Pawn=%s PlayerState=%s Local=%d Authority=%d"),
		*GetNameSafe(this),
		*GetNameSafe(InPawn),
		*GetNameSafe(PlayerState.Get()),
		IsLocalController() ? 1 : 0,
		HasAuthority() ? 1 : 0);
	HandlePawnChangedRuntime(InPawn, TEXT("AcknowledgePossession"));
}

void ASpellRisePlayerController::HandlePawnChangedRuntime(APawn* NewPawn, const TCHAR* SourceLabel)
{
	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[Control][PawnChanged] Source=%s Controller=%s Pawn=%s CurrentPawn=%s ViewTarget=%s Local=%d Authority=%d"),
		SourceLabel ? SourceLabel : TEXT("Unknown"),
		*GetNameSafe(this),
		*GetNameSafe(NewPawn),
		*GetNameSafe(GetPawn()),
		*GetNameSafe(GetViewTarget()),
		IsLocalController() ? 1 : 0,
		HasAuthority() ? 1 : 0);

	if (NewPawn && IsLocalController())
	{
		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);
		SetupEnhancedInput();
		RefreshEnhancedInputContexts();
		LogInputFocusSnapshot(SourceLabel ? SourceLabel : TEXT("HandlePawnChangedRuntime"));
	}

	if (NewPawn && IsLocalController() && GetViewTarget() != NewPawn)
	{
		FString CameraFlowSkipReason;
		if (CanRunLocalHUDFlow(&CameraFlowSkipReason))
		{
			SetViewTarget(NewPawn);
		}
		else
		{
		}
	}
}

void ASpellRisePlayerController::OnAttackPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	// Evita press/release duplicado quando IA_Attack e IA_Primary mapeiam o mesmo botão.
	if (IA_Primary != nullptr)
	{
		return;
	}

	SendAbilityInputTagPressed(InputTag_Primary());
}

void ASpellRisePlayerController::OnAttackReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (IA_Primary != nullptr)
	{
		return;
	}

	SendAbilityInputTagReleased(InputTag_Primary());
}

USpellRiseAbilitySystemComponent* ASpellRisePlayerController::GetSpellRiseASCFromPawn() const
{
	APawn* ControlledPawn = GetPawn();
	FString SkipReason;
	if (!SR_CanSendPawnOwnedInputRpc(this, ControlledPawn, &SkipReason))
	{
		return nullptr;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledPawn);
	return Cast<USpellRiseAbilitySystemComponent>(ASC);
}

bool ASpellRisePlayerController::IsControlledCharacterDead() const
{
	const ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn());
	return ControlledCharacter && ControlledCharacter->IsDead();
}

bool ASpellRisePlayerController::IsGameplayInputBlocked() const
{
	return IsControlledCharacterDead() || ShouldEnableUIInputContext();
}

void ASpellRisePlayerController::SendAbilityInputTagPressed(FGameplayTag InputTag)
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (!InputTag.IsValid())
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	FString SkipReason;
	if (!SR_CanSendPawnOwnedInputRpc(this, ControlledPawn, &SkipReason))
	{
		return;
	}

	if (USpellRiseAbilitySystemComponent* SRASC = GetSpellRiseASCFromPawn())
	{
		SRASC->SR_AbilityInputTagPressed(InputTag);
	}
}

void ASpellRisePlayerController::LogInputFocusSnapshot(const TCHAR* SourceLabel)
{
	const double NowSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	if (NowSeconds > 0.0 && (NowSeconds - LastInputFocusSnapshotTimeSeconds) < 0.35)
	{
		return;
	}

	FString FocusWidgetName(TEXT("NoSlateFocus"));
	FString FocusWidgetClass(TEXT("None"));

	if (FSlateApplication::IsInitialized())
	{
		if (TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0))
		{
			FocusWidgetName = FocusedWidget->ToString();
			FocusWidgetClass = FocusedWidget->GetTypeAsString();
		}
	}

	const FString Signature = FString::Printf(
		TEXT("%d|%d|%d|%s|%s|%s|%s"),
		bShowMouseCursor ? 1 : 0,
		bEnableClickEvents ? 1 : 0,
		bEnableMouseOverEvents ? 1 : 0,
		*FocusWidgetName,
		*FocusWidgetClass,
		*GetNameSafe(GetPawn()),
		*GetNameSafe(this));

	if (Signature == LastInputFocusSnapshotSignature)
	{
		return;
	}

	LastInputFocusSnapshotSignature = Signature;
	LastInputFocusSnapshotTimeSeconds = NowSeconds;

	const ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn());
	const UCharacterMovementComponent* Movement = ControlledCharacter ? ControlledCharacter->GetCharacterMovement() : nullptr;

	UE_LOG(LogSpellRisePlayerControllerRuntime, Log,
		TEXT("[InputFocus] Source=%s Controller=%s Pawn=%s Cursor=%d Click=%d MouseOver=%d UIContext=%d MoveIgnored=%d LookIgnored=%d Focus=%s|%s MovementMode=%d MaxWalkSpeed=%.2f"),
		SourceLabel ? SourceLabel : TEXT("Unknown"),
		*GetNameSafe(this),
		*GetNameSafe(GetPawn()),
		bShowMouseCursor ? 1 : 0,
		bEnableClickEvents ? 1 : 0,
		bEnableMouseOverEvents ? 1 : 0,
		ShouldEnableUIInputContext() ? 1 : 0,
		IsMoveInputIgnored() ? 1 : 0,
		IsLookInputIgnored() ? 1 : 0,
		*FocusWidgetName,
		*FocusWidgetClass,
		Movement ? static_cast<int32>(Movement->MovementMode) : INDEX_NONE,
		Movement ? Movement->MaxWalkSpeed : 0.0f);
}

void ASpellRisePlayerController::SendAbilityInputTagReleased(FGameplayTag InputTag)
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (!InputTag.IsValid())
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	FString SkipReason;
	if (!SR_CanSendPawnOwnedInputRpc(this, ControlledPawn, &SkipReason))
	{
		return;
	}

	if (USpellRiseAbilitySystemComponent* SRASC = GetSpellRiseASCFromPawn())
	{
		SRASC->SR_AbilityInputTagReleased(InputTag);
	}
}

void ASpellRisePlayerController::HandleAbilitySlotPressed(int32 SlotIndex)
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn()))
	{
		ControlledCharacter->AbilityWheelInputPressed(SlotIndex);
	}
}

void ASpellRisePlayerController::HandleAbilitySlotReleased(int32 SlotIndex)
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn()))
	{
		ControlledCharacter->AbilityWheelInputReleased(SlotIndex);
	}
}

void ASpellRisePlayerController::OnPrimaryPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagPressed(InputTag_Primary());
}

void ASpellRisePlayerController::OnPrimaryReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagReleased(InputTag_Primary());
}

void ASpellRisePlayerController::OnSecondaryPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagPressed(InputTag_Secondary());
}

void ASpellRisePlayerController::OnSecondaryReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagReleased(InputTag_Secondary());
}

void ASpellRisePlayerController::OnInteractPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (TryExecuteNarrativeInteract(true))
	{
		return;
	}

	SendAbilityInputTagPressed(InputTag_Interact());
}

void ASpellRisePlayerController::OnInteractReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (TryExecuteNarrativeInteract(false))
	{
		return;
	}

	SendAbilityInputTagReleased(InputTag_Interact());
}

bool ASpellRisePlayerController::TryExecuteNarrativeInteract(bool bPressed) const
{
	if (!IsLocalController())
	{
		return false;
	}

	UNarrativeInteractionComponent* InteractionComponent = ResolveNarrativeInteractionComponent();
	if (!InteractionComponent)
	{
		return false;
	}

	const FName FunctionName = bPressed ? TEXT("BeginInteract") : TEXT("EndInteract");
	UFunction* Function = InteractionComponent->FindFunction(FunctionName);
	if (!Function)
	{
		return false;
	}

	InteractionComponent->ProcessEvent(Function, nullptr);
	return true;
}

UNarrativeInteractionComponent* ASpellRisePlayerController::ResolveNarrativeInteractionComponent() const
{
	if (UNarrativeInteractionComponent* ControllerInteraction = FindComponentByClass<UNarrativeInteractionComponent>())
	{
		return ControllerInteraction;
	}

	if (APawn* ControlledPawn = GetPawn())
	{
		if (UNarrativeInteractionComponent* PawnInteraction = ControlledPawn->FindComponentByClass<UNarrativeInteractionComponent>())
		{
			return PawnInteraction;
		}
	}

	return nullptr;
}

void ASpellRisePlayerController::OnClearSelectionPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	if (ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn()))
	{
		ControlledCharacter->ClearSelectedAbility();
	}
}

void ASpellRisePlayerController::OnSprintPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagPressed(InputTag_Sprint());
}

void ASpellRisePlayerController::OnSprintReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	SendAbilityInputTagReleased(InputTag_Sprint());
}

void ASpellRisePlayerController::OnAbility1Pressed() { HandleAbilitySlotPressed(0); }
void ASpellRisePlayerController::OnAbility1Released() { HandleAbilitySlotReleased(0); }
void ASpellRisePlayerController::OnAbility2Pressed() { HandleAbilitySlotPressed(1); }
void ASpellRisePlayerController::OnAbility2Released() { HandleAbilitySlotReleased(1); }
void ASpellRisePlayerController::OnAbility3Pressed() { HandleAbilitySlotPressed(2); }
void ASpellRisePlayerController::OnAbility3Released() { HandleAbilitySlotReleased(2); }
void ASpellRisePlayerController::OnAbility4Pressed() { HandleAbilitySlotPressed(3); }
void ASpellRisePlayerController::OnAbility4Released() { HandleAbilitySlotReleased(3); }
void ASpellRisePlayerController::OnAbility5Pressed() { HandleAbilitySlotPressed(4); }
void ASpellRisePlayerController::OnAbility5Released() { HandleAbilitySlotReleased(4); }
void ASpellRisePlayerController::OnAbility6Pressed() { HandleAbilitySlotPressed(5); }
void ASpellRisePlayerController::OnAbility6Released() { HandleAbilitySlotReleased(5); }
void ASpellRisePlayerController::OnAbility7Pressed() { HandleAbilitySlotPressed(6); }
void ASpellRisePlayerController::OnAbility7Released() { HandleAbilitySlotReleased(6); }
void ASpellRisePlayerController::OnAbility8Pressed() { HandleAbilitySlotPressed(7); }
void ASpellRisePlayerController::OnAbility8Released() { HandleAbilitySlotReleased(7); }

bool ASpellRisePlayerController::ShouldEnableUIInputContext() const
{
	return bShowMouseCursor || bEnableClickEvents || bEnableMouseOverEvents;
}

void ASpellRisePlayerController::ShowDamageNumber(
	float Damage,
	const FVector& WorldLocation,
	const FGameplayTagContainer& SourceTags,
	const FGameplayTagContainer& TargetTags,
	bool bIsCritical)
{
	if (!IsLocalController())
	{
		return;
	}

	if (!NumberPopComponent)
	{
		return;
	}

	FSpellRiseNumberPopRequest Request;
	Request.WorldLocation = WorldLocation;
	Request.SourceTags = SourceTags;
	Request.TargetTags = TargetTags;
	Request.NumberToDisplay = FMath::RoundToInt(Damage);
	Request.bIsCriticalDamage = bIsCritical;

	NumberPopComponent->AddNumberPop(Request);
}

void ASpellRisePlayerController::ClientReceiveCombatLogEntry_Implementation(float Damage, const FString& OtherActorName, bool bIsOutgoing)
{
	if (!IsLocalController() || Damage <= 0.f)
	{
		return;
	}

	const int32 RoundedDamage = FMath::RoundToInt(Damage);
	const FString SafeOtherName = SanitizeBoundedString(OtherActorName, MaxCombatActorNameChars, TEXT("unknown target"));
	const FString MessageText = bIsOutgoing
		? FString::Printf(TEXT("You dealt %d damage to %s."), RoundedDamage, *SafeOtherName)
		: FString::Printf(TEXT("You took %d damage from %s."), RoundedDamage, *SafeOtherName);

	PushCombatLogMessage(SanitizeBoundedString(MessageText, MaxCombatLogMessageChars));
}

void ASpellRisePlayerController::ClientReceiveChatMessage_Implementation(const FSpellRiseChatMessage& Message)
{
	ReceiveChatMessageLocal(SanitizeChatMessageForLocalDelivery(Message));
}

void ASpellRisePlayerController::ReceiveChatMessageLocal(const FSpellRiseChatMessage& Message)
{
	const FSpellRiseChatMessage SanitizedMessage = SanitizeChatMessageForLocalDelivery(Message);
	if (!IsLocalController() || SanitizedMessage.Text.IsEmpty())
	{
		return;
	}

	BP_OnChatMessageReceived(SanitizedMessage);

}

void ASpellRisePlayerController::PushCombatLogMessage(const FString& MessageText)
{
	if (!IsLocalController() || MessageText.IsEmpty())
	{
		return;
	}

	const FString SafeMessageText = SanitizeBoundedString(MessageText, MaxCombatLogMessageChars);
	BP_OnCombatLogMessage(SafeMessageText);

	FSpellRiseChatMessage CombatMessage;
	CombatMessage.Name = FName(TEXT("Combat"));
	CombatMessage.Text = FText::FromString(SafeMessageText);
	CombatMessage.TimeText = BuildCombatTimeText();
	CombatMessage.Channel = SpellRiseChatChannel::Combat;

	ReceiveChatMessageLocal(CombatMessage);
}
