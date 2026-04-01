#include "SpellRisePlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/ActorComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InteractionComponent.h"
#include "Misc/DateTime.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "UObject/UnrealType.h"

#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent.h"
#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent_NiagaraText.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/UI/SpellRiseCombatHUDWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRisePlayerControllerRuntime, Log, All);

namespace
{
	const FName NAME_SendChatToSERVER(TEXT("SendChatToSERVER"));
	const FName NAME_AddChatMessage(TEXT("AddChatMessage"));
	const FName NAME_AppendMessage(TEXT("AppendMessage"));
	const FString CombatChannelName(TEXT("Combat"));

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

	const FGameplayTag& InputTag_AbilitySlot(int32 SlotIndex)
	{
		switch (SlotIndex)
		{
		case 0:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill1"));
			return Tag;
		}
		case 1:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill2"));
			return Tag;
		}
		case 2:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill3"));
			return Tag;
		}
		case 3:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill4"));
			return Tag;
		}
		case 4:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill5"));
			return Tag;
		}
		case 5:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill6"));
			return Tag;
		}
		case 6:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill7"));
			return Tag;
		}
		case 7:
		{
			static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("InputTag.Ability.Skill8"));
			return Tag;
		}
		default:
		{
			static FGameplayTag InvalidTag;
			return InvalidTag;
		}
		}
	}

	FVector ResolveTraceOrigin(const APlayerController* PlayerController)
	{
		if (!PlayerController)
		{
			return FVector::ZeroVector;
		}

		if (const APawn* ControlledPawn = PlayerController->GetPawn())
		{
			return ControlledPawn->GetPawnViewLocation();
		}

		if (const APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
		{
			return CameraManager->GetCameraLocation();
		}

		return FVector::ZeroVector;
	}

	struct FCombatChatPayload
	{
		FName SenderName = FName(TEXT("Combat"));
		FText MessageText;
		FText TimeText;
	};

	FText BuildCombatTimeText()
	{
		const FDateTime Now = FDateTime::Now();
		return FText::FromString(Now.ToString(TEXT("%H:%M:%S")));
	}

	int64 ResolveCombatEnumValue(const UEnum* Enum)
	{
		if (!Enum)
		{
			return 3;
		}

		int64 Value = Enum->GetValueByNameString(TEXT("Combat"));
		if (Value != INDEX_NONE)
		{
			return Value;
		}

		for (int32 Index = 0; Index < Enum->NumEnums(); ++Index)
		{
			const FString EnumName = Enum->GetNameStringByIndex(Index);
			if (EnumName.Contains(TEXT("Combat"), ESearchCase::IgnoreCase))
			{
				return Enum->GetValueByIndex(Index);
			}
		}

		return 3;
	}

	bool SetNameCompatibleProperty(FProperty* Property, void* ValuePtr, const FName& Value)
	{
		if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
		{
			NameProperty->SetPropertyValue(ValuePtr, Value);
			return true;
		}

		if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
		{
			StrProperty->SetPropertyValue(ValuePtr, Value.ToString());
			return true;
		}

		if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
		{
			TextProperty->SetPropertyValue(ValuePtr, FText::FromName(Value));
			return true;
		}

		return false;
	}

	bool SetTextCompatibleProperty(FProperty* Property, void* ValuePtr, const FText& Value)
	{
		if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
		{
			TextProperty->SetPropertyValue(ValuePtr, Value);
			return true;
		}

		if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
		{
			StrProperty->SetPropertyValue(ValuePtr, Value.ToString());
			return true;
		}

		if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
		{
			NameProperty->SetPropertyValue(ValuePtr, FName(*Value.ToString()));
			return true;
		}

		return false;
	}

	bool SetCombatChannelProperty(FProperty* Property, void* ValuePtr)
	{
		if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
		{
			if (FNumericProperty* UnderlyingNumeric = EnumProperty->GetUnderlyingProperty())
			{
				UnderlyingNumeric->SetIntPropertyValue(ValuePtr, ResolveCombatEnumValue(EnumProperty->GetEnum()));
				return true;
			}
		}

		if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
		{
			const int64 Value = ResolveCombatEnumValue(ByteProperty->Enum);
			ByteProperty->SetPropertyValue(ValuePtr, static_cast<uint8>(FMath::Clamp<int64>(Value, 0, 255)));
			return true;
		}

		if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
		{
			NameProperty->SetPropertyValue(ValuePtr, FName(*CombatChannelName));
			return true;
		}

		if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
		{
			StrProperty->SetPropertyValue(ValuePtr, CombatChannelName);
			return true;
		}

		if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
		{
			TextProperty->SetPropertyValue(ValuePtr, FText::FromString(CombatChannelName));
			return true;
		}

		return false;
	}

	bool PopulateChatPayloadRecursive(
		FProperty* Property,
		void* ContainerPtr,
		const FCombatChatPayload& Payload,
		bool& bOutWroteText,
		bool& bOutWroteChannel,
		bool& bOutFunctionHasChannel)
	{
		if (!Property || !ContainerPtr)
		{
			return false;
		}

		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(ContainerPtr);
		const FString PropertyName = Property->GetName();

		if (PropertyName.Contains(TEXT("Channel"), ESearchCase::IgnoreCase))
		{
			bOutFunctionHasChannel = true;
			if (SetCombatChannelProperty(Property, ValuePtr))
			{
				bOutWroteChannel = true;
				return true;
			}
		}

		if (PropertyName.Contains(TEXT("TimeText"), ESearchCase::IgnoreCase))
		{
			return SetTextCompatibleProperty(Property, ValuePtr, Payload.TimeText);
		}

		if (PropertyName.Equals(TEXT("Text"), ESearchCase::IgnoreCase) ||
			PropertyName.Contains(TEXT("Message"), ESearchCase::IgnoreCase))
		{
			const bool bSet = SetTextCompatibleProperty(Property, ValuePtr, Payload.MessageText);
			bOutWroteText = bOutWroteText || bSet;
			return bSet;
		}

		if (PropertyName.Equals(TEXT("Name"), ESearchCase::IgnoreCase) ||
			PropertyName.Contains(TEXT("Sender"), ESearchCase::IgnoreCase))
		{
			return SetNameCompatibleProperty(Property, ValuePtr, Payload.SenderName);
		}

		if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			void* StructValuePtr = StructProperty->ContainerPtrToValuePtr<void>(ContainerPtr);
			bool bWroteAnyField = false;

			for (TFieldIterator<FProperty> It(StructProperty->Struct); It; ++It)
			{
				bWroteAnyField |= PopulateChatPayloadRecursive(
					*It,
					StructValuePtr,
					Payload,
					bOutWroteText,
					bOutWroteChannel,
					bOutFunctionHasChannel);
			}

			return bWroteAnyField;
		}

		return false;
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

	constexpr double HUDAbilitiesRefreshIntervalSeconds = 0.20;
	constexpr double HUDTargetRefreshIntervalSeconds = 0.05;

	bool SR_IsAbilityInputTag(const FGameplayTag& InputTag)
	{
		if (!InputTag.IsValid())
		{
			return false;
		}

		return InputTag.ToString().StartsWith(TEXT("InputTag.Ability."));
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
				UE_LOG(
					LogSpellRisePlayerControllerRuntime,
					Log,
					TEXT("[Input][IMC] Applied Context=%s Priority=%d"),
					ContextLabel,
					Priority);
			}

			return;
		}

		if (bHasContext)
		{
			Subsystem->RemoveMappingContext(Context);
			UE_LOG(
				LogSpellRisePlayerControllerRuntime,
				Log,
				TEXT("[Input][IMC] Removed Context=%s Priority=%d"),
				ContextLabel,
				Priority);
		}
	}
}

ASpellRisePlayerController::ASpellRisePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
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
			UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose, TEXT("[Chat] Blocked SendChatToSERVER to combat channel from %s"), *GetNameSafe(this));
			return;
		}
	}

	Super::ProcessEvent(Function, Parameters);
}

void ASpellRisePlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogSpellRisePlayerControllerRuntime, Log, TEXT("[PC.BeginPlayStep] Step=BeginPlayStart Controller=%s Class=%s Local=%d"),
		*GetNameSafe(this),
		*GetClass()->GetName(),
		IsLocalController() ? 1 : 0);

	if (IsLocalController())
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Log, TEXT("[PC.BeginPlayStep] Step=SetInputModeGameOnly"));
		FInputModeGameOnly Mode;
		SetInputMode(Mode);
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Log, TEXT("[PC.BeginPlayStep] Step=SetupEnhancedInput"));
	SetupEnhancedInput();

	FString LocalFlowSkipReason;
	if (CanRunLocalPawnRuntime(GetPawn(), &LocalFlowSkipReason))
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Log, TEXT("[PC.BeginPlayStep] Step=HUDFlowReady"));
		EnsureCombatHUDCreated();
		UpdateCombatHUDVisibility();
		TryBindHUDToCurrentASC();
	}
	else
	{
		UE_LOG(
			LogSpellRisePlayerControllerRuntime,
			Log,
			TEXT("[PC.BeginPlayStep] Step=HUDFlowSkipped Reason=%s"),
			*LocalFlowSkipReason);
	}

	if (APawn* InitialPawn = GetPawn())
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Log, TEXT("[PC.BeginPlayStep] Step=HandlePawnChangedRuntime PawnReady=1"));
		HandlePawnChangedRuntime(InitialPawn, TEXT("BeginPlay"));
	}
	else
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Log, TEXT("[PC.BeginPlayStep] Step=HandlePawnChangedRuntimeDeferred PawnReady=0"));
	}
	bLastKnownPersistenceProfileReady = IsPersistenceProfileReady();
	if (bLastKnownPersistenceProfileReady)
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Log, TEXT("[PC.BeginPlayStep] Step=InitialHUDBroadcast"));
		BroadcastResourcesToHUD();
		BroadcastAbilitySlotsToHUD();
	}

#if !UE_BUILD_SHIPPING
	bAutoForceDeathFromCommandLine = FParse::Param(FCommandLine::Get(), TEXT("SRForceDeathOnBeginPlay"));
	bAutoForceDeathTriggered = false;
	if (bAutoForceDeathFromCommandLine)
	{
		AutoForceDeathTriggerAtServerTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() + 5.0 : 5.0;
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[Debug][ForceDeath] Flag de cmdline detectada para %s. Disparo em %.2fs"), *GetNameSafe(this), AutoForceDeathTriggerAtServerTimeSeconds);
	}
#endif
}

void ASpellRisePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FString LocalPawnRuntimeSkipReason;
	const bool bCanRunHUDFlow = CanRunLocalPawnRuntime(GetPawn(), &LocalPawnRuntimeSkipReason);
	if (bCanRunHUDFlow)
	{
		EnsureCombatHUDCreated();
		UpdateCombatHUDVisibility();
		TryBindHUDToCurrentASC();

		const bool bPersistenceReady = IsPersistenceProfileReady();
		if (bPersistenceReady)
		{
			if (!bLastKnownPersistenceProfileReady)
			{
				BroadcastResourcesToHUD();
				BroadcastAbilitySlotsToHUD();
			}
		}
		bLastKnownPersistenceProfileReady = bPersistenceReady;

		if (const double NowSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0; NowSeconds >= NextHUDAbilitiesRefreshTimeSeconds)
		{
			NextHUDAbilitiesRefreshTimeSeconds = NowSeconds + HUDAbilitiesRefreshIntervalSeconds;
			if (bPersistenceReady)
			{
				BroadcastAbilitySlotsToHUD();
			}
		}

		if (const double NowSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0; NowSeconds >= NextHUDTargetRefreshTimeSeconds)
		{
			NextHUDTargetRefreshTimeSeconds = NowSeconds + HUDTargetRefreshIntervalSeconds;

			FSpellRiseAimTraceResult AimResult;
			if (GetCurrentAimTraceResult(AimResult))
			{
				BroadcastTargetToHUD(AimResult);
			}
			else if (CombatHUDWidget)
			{
				CombatHUDWidget->BP_OnTargetUpdated(false, nullptr, 0.f);
			}
		}
	}
	else
	{
		LogASCBindSkipReason(LocalPawnRuntimeSkipReason);
		bLastKnownPersistenceProfileReady = false;
	}

	if (bEnableLegacyReflectionChatRouting)
	{
		FlushPendingChatMessages();
	}

#if !UE_BUILD_SHIPPING
	if (bAutoForceDeathFromCommandLine && !bAutoForceDeathTriggered)
	{
		UWorld* World = GetWorld();
		const double NowSeconds = World ? World->GetTimeSeconds() : 0.0;
		if (NowSeconds >= AutoForceDeathTriggerAtServerTimeSeconds && IsValid(GetPawn()))
		{
			bAutoForceDeathTriggered = true;
			SR_ForceDeath();
		}
	}
#endif
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

bool ASpellRisePlayerController::CanRunLocalPawnRuntime(APawn* CandidatePawn, FString* OutSkipReason) const
{
	auto SetSkipReason = [&](const TCHAR* Reason)
	{
		if (OutSkipReason)
		{
			*OutSkipReason = Reason;
		}
		return false;
	};

	FString LocalHUDSkipReason;
	if (!CanRunLocalHUDFlow(&LocalHUDSkipReason))
	{
		return SetSkipReason(*LocalHUDSkipReason);
	}

	APawn* PawnToValidate = CandidatePawn;
	if (!IsValid(PawnToValidate))
	{
		PawnToValidate = GetPawn();
	}
	if (!IsValid(PawnToValidate))
	{
		return SetSkipReason(TEXT("pawn_missing"));
	}

	APlayerState* ControlledPlayerState = PawnToValidate->GetPlayerState();
	if (!IsValid(ControlledPlayerState))
	{
		return SetSkipReason(TEXT("playerstate_missing"));
	}

	UAbilitySystemComponent* PawnASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PawnToValidate);
	if (!IsValid(PawnASC))
	{
		return SetSkipReason(TEXT("asc_missing"));
	}

	return true;
}

void ASpellRisePlayerController::LogASCBindSkipReason(const FString& SkipReason)
{
	if (SkipReason.Equals(LastASCBindSkipReason, ESearchCase::CaseSensitive))
	{
		return;
	}

	LastASCBindSkipReason = SkipReason;
	UE_LOG(
		LogSpellRisePlayerControllerRuntime,
		Log,
		TEXT("[PC.ASCBind.SkipReason] Controller=%s Reason=%s"),
		*GetNameSafe(this),
		*SkipReason);
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

	UE_LOG(
		LogSpellRisePlayerControllerRuntime,
		Log,
		TEXT("[Net][ClientEndPlay] Controller=%s Local=%d Reason=%s Pawn=%s"),
		*GetNameSafe(this),
		IsLocalController() ? 1 : 0,
		EndPlayReasonText,
		*GetNameSafe(GetPawn()));

	UnbindHUDFromASC();
	CleanupCombatHUD();

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
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[SpellRise] PlayerController: nenhum MappingContext disponivel (Core/Combat/Interaction/UI/Default)."));
		return;
	}

	if (!Subsystem->HasMappingContext(MappingContextToApply))
	{
		Subsystem->AddMappingContext(MappingContextToApply, MappingPriority);
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[Input][IMC] Fallback MappingContext aplicado (%s)"), *GetNameSafe(MappingContextToApply));
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

void ASpellRisePlayerController::EnsureCombatHUDCreated()
{
	if (CombatHUDWidget)
	{
		return;
	}

	FString LocalFlowSkipReason;
	if (!CanRunLocalPawnRuntime(GetPawn(), &LocalFlowSkipReason))
	{
		if (!LocalFlowSkipReason.Equals(LastHUDCreateFailureReason, ESearchCase::CaseSensitive))
		{
			LastHUDCreateFailureReason = LocalFlowSkipReason;
			UE_LOG(
				LogSpellRisePlayerControllerRuntime,
				Log,
				TEXT("[PC.HUDCreate.Fail] Controller=%s Reason=%s"),
				*GetNameSafe(this),
				*LocalFlowSkipReason);
		}
		return;
	}

	if (!CombatHUDWidgetClass || !IsValid(CombatHUDWidgetClass.Get()))
	{
		const FString InvalidClassReason = TEXT("invalid_widget_class");
		if (!InvalidClassReason.Equals(LastHUDCreateFailureReason, ESearchCase::CaseSensitive))
		{
			LastHUDCreateFailureReason = InvalidClassReason;
			UE_LOG(
				LogSpellRisePlayerControllerRuntime,
				Log,
				TEXT("[PC.HUDCreate.Fail] Controller=%s Reason=%s Class=%s"),
				*GetNameSafe(this),
				*InvalidClassReason,
				*GetNameSafe(CombatHUDWidgetClass.Get()));
		}
		return;
	}

	if (!bLoggedHUDCreateAttempt)
	{
		bLoggedHUDCreateAttempt = true;
		UE_LOG(
			LogSpellRisePlayerControllerRuntime,
			Log,
			TEXT("[PC.HUDCreate.Attempt] Controller=%s WidgetClass=%s"),
			*GetNameSafe(this),
			*GetNameSafe(CombatHUDWidgetClass.Get()));
	}

	CombatHUDWidget = CreateWidget<USpellRiseCombatHUDWidget>(this, CombatHUDWidgetClass);
	if (!CombatHUDWidget)
	{
		const FString CreationFailReason = TEXT("create_widget_returned_null");
		if (!CreationFailReason.Equals(LastHUDCreateFailureReason, ESearchCase::CaseSensitive))
		{
			LastHUDCreateFailureReason = CreationFailReason;
			UE_LOG(
				LogSpellRisePlayerControllerRuntime,
				Log,
				TEXT("[PC.HUDCreate.Fail] Controller=%s Reason=%s WidgetClass=%s"),
				*GetNameSafe(this),
				*CreationFailReason,
				*GetNameSafe(CombatHUDWidgetClass.Get()));
		}
		return;
	}

	CombatHUDWidget->AddToViewport();
	LastHUDCreateFailureReason.Reset();
	if (!bLoggedHUDCreateSuccess)
	{
		bLoggedHUDCreateSuccess = true;
		UE_LOG(
			LogSpellRisePlayerControllerRuntime,
			Log,
			TEXT("[PC.HUDCreate.Success] Controller=%s Widget=%s"),
			*GetNameSafe(this),
			*GetNameSafe(CombatHUDWidget));
	}
}

void ASpellRisePlayerController::CleanupCombatHUD()
{
	if (CombatHUDWidget)
	{
		CombatHUDWidget->RemoveFromParent();
		CombatHUDWidget = nullptr;
	}

	bCombatHUDHiddenByDeath = false;
	bCombatHUDSuppressedByDeath = false;
	DeathSuppressedHUDWidgets.Reset();
}

void ASpellRisePlayerController::SetCombatHUDSuppressedByDeath(bool bSuppressed)
{
	bCombatHUDSuppressedByDeath = bSuppressed;
	SetPlayerHUDWidgetsSuppressed(bSuppressed);
	UpdateCombatHUDVisibility();
}

void ASpellRisePlayerController::SR_ForceDeath()
{
#if UE_BUILD_SHIPPING
	UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[Debug][ForceDeath] Bloqueado em build shipping. Controller=%s"), *GetNameSafe(this));
	return;
#else
	if (HasAuthority())
	{
		ExecuteForceDeath_Server();
		return;
	}

	ServerSR_ForceDeath();
#endif
}

void ASpellRisePlayerController::ServerSR_ForceDeath_Implementation()
{
#if UE_BUILD_SHIPPING
	return;
#else
	ExecuteForceDeath_Server();
#endif
}

void ASpellRisePlayerController::InventorySplitSlotSERVER_Implementation(UObject* FromContainer, int32 Slot, int32 Amount)
{
	UE_LOG(
		LogSpellRisePlayerControllerRuntime,
		Warning,
		TEXT("[Inventory][Server] InventorySplitSlotSERVER chamado sem implementacao ativa. Controller=%s FromContainer=%s Slot=%d Amount=%d"),
		*GetNameSafe(this),
		*GetNameSafe(FromContainer),
		Slot,
		Amount);
}

void ASpellRisePlayerController::OnInventorySlotDropSERVER_Implementation(UObject* FromContainer, UObject* ToInventoryComponent, int32 FromIndex, int32 ToIndex)
{
	UE_LOG(
		LogSpellRisePlayerControllerRuntime,
		Warning,
		TEXT("[Inventory][Server] OnInventorySlotDropSERVER chamado sem implementacao ativa. Controller=%s FromContainer=%s ToInventoryComponent=%s FromIndex=%d ToIndex=%d"),
		*GetNameSafe(this),
		*GetNameSafe(FromContainer),
		*GetNameSafe(ToInventoryComponent),
		FromIndex,
		ToIndex);
}

void ASpellRisePlayerController::Route_InventorySortBy_SERVER_Implementation(UObject* InventoryRef, int32 SortBy)
{
	UE_LOG(
		LogSpellRisePlayerControllerRuntime,
		Warning,
		TEXT("[Inventory][Server] Route_InventorySortBy_SERVER chamado sem implementacao ativa. Controller=%s InventoryRef=%s SortBy=%d"),
		*GetNameSafe(this),
		*GetNameSafe(InventoryRef),
		SortBy);
}

void ASpellRisePlayerController::ExecuteForceDeath_Server()
{
	if (!HasAuthority())
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[Debug][ForceDeath] Rejeitado sem authority. Controller=%s"), *GetNameSafe(this));
		return;
	}

	ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn());
	if (!IsValid(ControlledCharacter))
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[Debug][ForceDeath] Pawn invalido para %s"), *GetNameSafe(this));
		return;
	}

	if (ControlledCharacter->IsDead())
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[Debug][ForceDeath] Ignorado (personagem ja morto). Char=%s"), *GetNameSafe(ControlledCharacter));
		return;
	}

	UAbilitySystemComponent* ASC = ControlledCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Error, TEXT("[Debug][ForceDeath] ASC ausente. Char=%s"), *GetNameSafe(ControlledCharacter));
		return;
	}

	UE_LOG(
		LogSpellRisePlayerControllerRuntime,
		Warning,
		TEXT("[Debug][ForceDeath] Forcando Health=0 no servidor. Controller=%s Char=%s"),
		*GetNameSafe(this),
		*GetNameSafe(ControlledCharacter));

	ASC->SetNumericAttributeBase(UResourceAttributeSet::GetHealthAttribute(), 0.f);
}

void ASpellRisePlayerController::SetPlayerHUDWidgetsSuppressed(bool bSuppressed)
{
	if (!IsLocalController())
	{
		return;
	}

	if (!bSuppressed)
	{
		for (const TWeakObjectPtr<UUserWidget>& WidgetPtr : DeathSuppressedHUDWidgets)
		{
			if (UUserWidget* Widget = WidgetPtr.Get())
			{
				Widget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
		DeathSuppressedHUDWidgets.Reset();
		return;
	}

	DeathSuppressedHUDWidgets.Reset();

	TArray<UUserWidget*> TopLevelWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, TopLevelWidgets, UUserWidget::StaticClass(), false);
	for (UUserWidget* Widget : TopLevelWidgets)
	{
		if (!Widget || Widget->GetOwningPlayer() != this)
		{
			continue;
		}

		if (!IsKnownPlayerHUDWidget(Widget))
		{
			continue;
		}

		Widget->SetVisibility(ESlateVisibility::Collapsed);
		DeathSuppressedHUDWidgets.Add(Widget);
	}
}

bool ASpellRisePlayerController::IsKnownPlayerHUDWidget(const UUserWidget* Widget) const
{
	if (!Widget)
	{
		return false;
	}

	const FString ClassName = Widget->GetClass()->GetName();
	return ClassName.Contains(TEXT("W_PlayerHUD"), ESearchCase::IgnoreCase)
		|| ClassName.Contains(TEXT("W_PlayerVitals"), ESearchCase::IgnoreCase)
		|| ClassName.Contains(TEXT("WBP_SR_HUDRoot"), ESearchCase::IgnoreCase)
		|| ClassName.Contains(TEXT("WBP_HUD"), ESearchCase::IgnoreCase);
}

bool ASpellRisePlayerController::IsPersistenceProfileReady() const
{
	const APawn* ControlledPawn = GetPawn();
	const ASpellRisePlayerState* SRPlayerState = ControlledPawn ? Cast<ASpellRisePlayerState>(ControlledPawn->GetPlayerState()) : nullptr;
	if (!SRPlayerState)
	{
		// Fallback permissivo para cenarios sem PlayerState especializado (ex.: editor/fluxos legados).
		return true;
	}

	return SRPlayerState->IsPersistenceProfileApplied();
}

void ASpellRisePlayerController::UpdateCombatHUDVisibility()
{
	if (!CombatHUDWidget)
	{
		bCombatHUDHiddenByDeath = false;
		return;
	}

	const bool bShouldHide = bCombatHUDSuppressedByDeath || IsGameplayInputBlocked() || !IsPersistenceProfileReady();
	if (bCombatHUDHiddenByDeath == bShouldHide)
	{
		return;
	}

	bCombatHUDHiddenByDeath = bShouldHide;
	CombatHUDWidget->SetVisibility(bShouldHide ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);

	if (bShouldHide)
	{
		CombatHUDWidget->BP_OnTargetUpdated(false, nullptr, 0.f);
	}
}

void ASpellRisePlayerController::TryBindHUDToCurrentASC()
{
	FString LocalFlowSkipReason;
	if (!CanRunLocalPawnRuntime(GetPawn(), &LocalFlowSkipReason))
	{
		LogASCBindSkipReason(LocalFlowSkipReason);
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn))
	{
		LogASCBindSkipReason(TEXT("pawn_missing"));
		return;
	}

	const APlayerState* ControlledPlayerState = ControlledPawn->GetPlayerState();
	if (!IsValid(ControlledPlayerState))
	{
		LogASCBindSkipReason(TEXT("playerstate_missing"));
		return;
	}

	USpellRiseAbilitySystemComponent* CurrentASC = GetSpellRiseASCFromPawn();
	if (!CurrentASC)
	{
		LogASCBindSkipReason(TEXT("asc_missing"));
		return;
	}

	if (HUDObservedASC.Get() == CurrentASC)
	{
		return;
	}

	UE_LOG(
		LogSpellRisePlayerControllerRuntime,
		Log,
		TEXT("[PC.ASCBind.Attempt] Controller=%s Pawn=%s ASC=%s"),
		*GetNameSafe(this),
		*GetNameSafe(ControlledPawn),
		*GetNameSafe(CurrentASC));

	UnbindHUDFromASC();

	HUDObservedASC = CurrentASC;
	HUDHealthChangedHandle = CurrentASC->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ASpellRisePlayerController::OnResourceAttributeChanged);
	HUDManaChangedHandle = CurrentASC->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetManaAttribute())
		.AddUObject(this, &ASpellRisePlayerController::OnResourceAttributeChanged);
	HUDStaminaChangedHandle = CurrentASC->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetStaminaAttribute())
		.AddUObject(this, &ASpellRisePlayerController::OnResourceAttributeChanged);

	LastASCBindSkipReason.Reset();
	UE_LOG(
		LogSpellRisePlayerControllerRuntime,
		Log,
		TEXT("[PC.ASCBind.Success] Controller=%s ASC=%s"),
		*GetNameSafe(this),
		*GetNameSafe(CurrentASC));

	BroadcastResourcesToHUD();
	BroadcastAbilitySlotsToHUD();
}

void ASpellRisePlayerController::UnbindHUDFromASC()
{
	if (USpellRiseAbilitySystemComponent* OldASC = HUDObservedASC.Get())
	{
		if (HUDHealthChangedHandle.IsValid())
		{
			OldASC->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetHealthAttribute()).Remove(HUDHealthChangedHandle);
			HUDHealthChangedHandle.Reset();
		}

		if (HUDManaChangedHandle.IsValid())
		{
			OldASC->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetManaAttribute()).Remove(HUDManaChangedHandle);
			HUDManaChangedHandle.Reset();
		}

		if (HUDStaminaChangedHandle.IsValid())
		{
			OldASC->GetGameplayAttributeValueChangeDelegate(UResourceAttributeSet::GetStaminaAttribute()).Remove(HUDStaminaChangedHandle);
			HUDStaminaChangedHandle.Reset();
		}
	}

	HUDObservedASC = nullptr;
}

void ASpellRisePlayerController::BroadcastResourcesToHUD()
{
	if (!CombatHUDWidget || !IsPersistenceProfileReady())
	{
		return;
	}

	USpellRiseAbilitySystemComponent* ASC = GetSpellRiseASCFromPawn();
	if (!ASC)
	{
		return;
	}

	const float NewHealth = ASC->GetNumericAttribute(UResourceAttributeSet::GetHealthAttribute());
	const float MaxHealth = ASC->GetNumericAttribute(UResourceAttributeSet::GetMaxHealthAttribute());
	const float NewMana = ASC->GetNumericAttribute(UResourceAttributeSet::GetManaAttribute());
	const float MaxMana = ASC->GetNumericAttribute(UResourceAttributeSet::GetMaxManaAttribute());
	const float NewStamina = ASC->GetNumericAttribute(UResourceAttributeSet::GetStaminaAttribute());
	const float MaxStamina = ASC->GetNumericAttribute(UResourceAttributeSet::GetMaxStaminaAttribute());

	CombatHUDWidget->BP_OnResourcesUpdated(NewHealth, MaxHealth, NewMana, MaxMana, NewStamina, MaxStamina);
}

void ASpellRisePlayerController::BroadcastAbilitySlotsToHUD()
{
	if (!CombatHUDWidget || !IsPersistenceProfileReady())
	{
		return;
	}

	USpellRiseAbilitySystemComponent* ASC = GetSpellRiseASCFromPawn();
	if (!ASC)
	{
		return;
	}

	const FGameplayAbilityActorInfo* ActorInfo = ASC->AbilityActorInfo.Get();
	FGameplayTag SelectedTag;
	if (const ASpellRiseCharacterBase* ControlledCharacter = Cast<ASpellRiseCharacterBase>(GetPawn()))
	{
		SelectedTag = ControlledCharacter->GetSelectedAbilityInputTag();
	}

	TArray<FSpellRiseAbilitySlotView> Slots;
	Slots.Reserve(8);

	for (int32 SlotIndex = 0; SlotIndex < 8; ++SlotIndex)
	{
		const FGameplayTag SlotTag = InputTag_AbilitySlot(SlotIndex);
		FSpellRiseAbilitySlotView SlotView;
		SlotView.InputTag = SlotTag;
		SlotView.bIsSelected = SelectedTag.IsValid() && SlotTag.MatchesTagExact(SelectedTag);

		const USpellRiseGameplayAbility* AbilityCDO = ASC->SR_GetSpellRiseAbilityForInputTag(SlotTag);
		if (!AbilityCDO)
		{
			Slots.Add(SlotView);
			continue;
		}

		SlotView.AbilityName = FName(*AbilityCDO->GetName());

		for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (Spec.Ability != AbilityCDO)
			{
				continue;
			}

			float TimeRemaining = 0.f;
			float CooldownDuration = 0.f;
			AbilityCDO->GetCooldownTimeRemainingAndDuration(Spec.Handle, ActorInfo, TimeRemaining, CooldownDuration);
			SlotView.CooldownRemaining = FMath::Max(0.f, TimeRemaining);
			SlotView.CooldownDuration = FMath::Max(0.f, CooldownDuration);
			SlotView.bIsOnCooldown = SlotView.CooldownRemaining > KINDA_SMALL_NUMBER;
			SlotView.bIsActive = Spec.IsActive();
			break;
		}

		Slots.Add(SlotView);
	}

	CombatHUDWidget->BP_OnAbilitySlotsUpdated(Slots);
}

void ASpellRisePlayerController::BroadcastTargetToHUD(const FSpellRiseAimTraceResult& AimResult)
{
	if (!CombatHUDWidget)
	{
		return;
	}

	const bool bHasTarget = AimResult.bHit && IsValid(AimResult.HitResult.GetActor());
	AActor* TargetActor = bHasTarget ? AimResult.HitResult.GetActor() : nullptr;
	const float Distance = FVector::Dist(AimResult.TraceStart, AimResult.TargetPoint);
	CombatHUDWidget->BP_OnTargetUpdated(bHasTarget, TargetActor, Distance);
}

void ASpellRisePlayerController::OnResourceAttributeChanged(const FOnAttributeChangeData& Data)
{
	BroadcastResourcesToHUD();
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
		UE_LOG(LogSpellRisePlayerControllerRuntime, Error, TEXT("[SpellRise] PlayerController: InputComponent is not EnhancedInputComponent."));
		return;
	}

	if (!IA_Attack)
	{
		// Optional action; many setups drive melee via IA_Primary only.
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
	HandlePawnChangedRuntime(InPawn, TEXT("OnPossess"));
}

void ASpellRisePlayerController::OnUnPossess()
{
	APawn* PreviousPawn = GetPawn();
	Super::OnUnPossess();
	HandlePawnChangedRuntime(nullptr, TEXT("OnUnPossess"));

	UE_LOG(
		LogSpellRisePlayerControllerRuntime,
		Log,
		TEXT("[Net][PawnChanged] Source=OnUnPossess Controller=%s PreviousPawn=%s"),
		*GetNameSafe(this),
		*GetNameSafe(PreviousPawn));
}

void ASpellRisePlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);
	HandlePawnChangedRuntime(InPawn, TEXT("AcknowledgePossession"));
}

void ASpellRisePlayerController::HandlePawnChangedRuntime(APawn* NewPawn, const TCHAR* SourceLabel)
{
	const TCHAR* SafeSource = SourceLabel ? SourceLabel : TEXT("Unknown");

	UE_LOG(
		LogSpellRisePlayerControllerRuntime,
		Log,
		TEXT("[Net][PawnChanged] Source=%s Controller=%s Pawn=%s Local=%d HasAuthority=%d"),
		SafeSource,
		*GetNameSafe(this),
		*GetNameSafe(NewPawn),
		IsLocalController() ? 1 : 0,
		HasAuthority() ? 1 : 0);

	FString LocalFlowSkipReason;
	if (CanRunLocalPawnRuntime(NewPawn, &LocalFlowSkipReason))
	{
		SetupEnhancedInput();
		RefreshEnhancedInputContexts();
		EnsureCombatHUDCreated();
		UpdateCombatHUDVisibility();
		TryBindHUDToCurrentASC();
	}
	else
	{
		UE_LOG(
			LogSpellRisePlayerControllerRuntime,
			Verbose,
			TEXT("[Net][PawnChanged] Source=%s Controller=%s local flow skipped Reason=%s"),
			SafeSource,
			*GetNameSafe(this),
			*LocalFlowSkipReason);
	}

	if (NewPawn && IsLocalController() && GetViewTarget() != NewPawn)
	{
		FString CameraFlowSkipReason;
		if (CanRunLocalHUDFlow(&CameraFlowSkipReason))
		{
			SetViewTarget(NewPawn);
			UE_LOG(
				LogSpellRisePlayerControllerRuntime,
				Verbose,
				TEXT("[Net][PawnChanged] Source=%s Controller=%s forced ViewTarget=%s"),
				SafeSource,
				*GetNameSafe(this),
				*GetNameSafe(NewPawn));
		}
		else
		{
			UE_LOG(
				LogSpellRisePlayerControllerRuntime,
				Verbose,
				TEXT("[Net][PawnChanged] Source=%s Controller=%s camera flow skipped Reason=%s"),
				SafeSource,
				*GetNameSafe(this),
				*CameraFlowSkipReason);
		}
	}
}

void ASpellRisePlayerController::OnAttackPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[GAS][Input] OnAttackPressed"));

	// Unified pipeline (AAA/Lyra-style): always route attack as gameplay-input-tag first.
	// This keeps combo continuation (WaitInputPress) on the same active spec flow.
	SendAbilityInputTagPressed(InputTag_Primary());

	USpellRiseAbilitySystemComponent* SRASC = GetSpellRiseASCFromPawn();
	if (SRASC && SRASC->SR_GetSpellRiseAbilityForInputTag(InputTag_Primary()))
	{
		return;
	}

	// Legacy fallback for old setups that still rely on direct class activation.
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledPawn);
	if (!ASC)
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[SpellRise] AttackPressed: ASC is null on Pawn=%s"), *GetNameSafe(ControlledPawn));
		return;
	}

	if (!AttackAbilityClass)
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[SpellRise] AttackPressed: AttackAbilityClass is null and no InputTag.Ability.Primary mapping found."));
		return;
	}

	const bool bActivated = ASC->TryActivateAbilityByClass(AttackAbilityClass);
	UE_LOG(LogSpellRisePlayerControllerRuntime, VeryVerbose, TEXT("[SpellRise] AttackPressed(FallbackClass): TryActivate=%d Ability=%s"),
		bActivated ? 1 : 0,
		*GetNameSafe(AttackAbilityClass.Get()));
}

void ASpellRisePlayerController::OnAttackReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[GAS][Input] OnAttackReleased"));
	SendAbilityInputTagReleased(InputTag_Primary());
}

USpellRiseAbilitySystemComponent* ASpellRisePlayerController::GetSpellRiseASCFromPawn() const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
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

	if (USpellRiseAbilitySystemComponent* SRASC = GetSpellRiseASCFromPawn())
	{
		SRASC->SR_AbilityInputTagPressed(InputTag);
	}
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
		return;
	}

	SendAbilityInputTagPressed(InputTag_AbilitySlot(SlotIndex));
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
		return;
	}

	SendAbilityInputTagReleased(InputTag_AbilitySlot(SlotIndex));
}

void ASpellRisePlayerController::OnPrimaryPressed()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[GAS][Input] OnPrimaryPressed"));
	SendAbilityInputTagPressed(InputTag_Primary());
}

void ASpellRisePlayerController::OnPrimaryReleased()
{
	if (IsGameplayInputBlocked())
	{
		return;
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[GAS][Input] OnPrimaryReleased"));
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
		UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose, TEXT("[Input][Interact] Rejeitado por bloqueio de gameplay. Controller=%s Pawn=%s"), *GetNameSafe(this), *GetNameSafe(GetPawn()));
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
		UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose, TEXT("[Input][Interact] Release rejeitado por bloqueio de gameplay. Controller=%s Pawn=%s"), *GetNameSafe(this), *GetNameSafe(GetPawn()));
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
		UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose, TEXT("[Input][Interact] Ignorado em controller nao-local. Controller=%s"), *GetNameSafe(this));
		return false;
	}

	UNarrativeInteractionComponent* InteractionComponent = ResolveNarrativeInteractionComponent();
	if (!InteractionComponent)
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[Input][Interact] Rejeitado: UNarrativeInteractionComponent ausente. Controller=%s Pawn=%s"), *GetNameSafe(this), *GetNameSafe(GetPawn()));
		return false;
	}

	const FName FunctionName = bPressed ? TEXT("BeginInteract") : TEXT("EndInteract");
	UFunction* Function = InteractionComponent->FindFunction(FunctionName);
	if (!Function)
	{
		UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[Input][Interact] Rejeitado: funcao %s ausente em %s. Controller=%s"), *FunctionName.ToString(), *GetNameSafe(InteractionComponent), *GetNameSafe(this));
		return false;
	}

	UE_LOG(LogSpellRisePlayerControllerRuntime, Verbose, TEXT("[Input][Interact] Dispatch %s via %s. Controller=%s"), *FunctionName.ToString(), *GetNameSafe(InteractionComponent), *GetNameSafe(this));
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

bool ASpellRisePlayerController::IsConstructionModeActive() const
{
	// Compatibilidade temporaria para chamadas legadas: build mode removido deste controller.
	return false;
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

	UE_LOG(LogSpellRisePlayerControllerRuntime, Warning, TEXT("[POP][PC] Damage=%.1f Loc=%s Local=%d Controller=%s"),
		Damage,
		*WorldLocation.ToString(),
		IsLocalController() ? 1 : 0,
		*GetNameSafe(this));

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
	const FString SafeOtherName = OtherActorName.IsEmpty() ? TEXT("alvo desconhecido") : OtherActorName;
	const FString MessageText = bIsOutgoing
		? FString::Printf(TEXT("Voce causou %d de dano em %s."), RoundedDamage, *SafeOtherName)
		: FString::Printf(TEXT("Voce recebeu %d de dano de %s."), RoundedDamage, *SafeOtherName);

	PushCombatLogMessage(MessageText);
}

void ASpellRisePlayerController::ClientReceiveChatMessage_Implementation(const FSpellRiseChatMessage& Message)
{
	ReceiveChatMessageLocal(Message);
}

void ASpellRisePlayerController::ReceiveChatMessageLocal(const FSpellRiseChatMessage& Message)
{
	if (!IsLocalController() || Message.Text.IsEmpty())
	{
		return;
	}

	BP_OnChatMessageReceived(Message);

	if (CombatHUDWidget && Message.Channel == SpellRiseChatChannel::Combat)
	{
		CombatHUDWidget->BP_OnCombatFeedMessage(Message);
	}

	if (bEnableLegacyReflectionChatRouting)
	{
		PendingChatMessages.Add(Message);
		FlushPendingChatMessages();
	}
}

void ASpellRisePlayerController::PushCombatLogMessage(const FString& MessageText)
{
	if (!IsLocalController() || MessageText.IsEmpty())
	{
		return;
	}

	BP_OnCombatLogMessage(MessageText);

	FSpellRiseChatMessage CombatMessage;
	CombatMessage.Name = FName(TEXT("Combat"));
	CombatMessage.Text = FText::FromString(MessageText);
	CombatMessage.TimeText = BuildCombatTimeText();
	CombatMessage.Channel = SpellRiseChatChannel::Combat;

	ReceiveChatMessageLocal(CombatMessage);
}

void ASpellRisePlayerController::FlushPendingChatMessages()
{
	if (!IsLocalController() || PendingChatMessages.Num() == 0)
	{
		return;
	}

	for (int32 Index = 0; Index < PendingChatMessages.Num();)
	{
		if (!TryAppendPendingChatMessage(PendingChatMessages[Index]))
		{
			break;
		}

		PendingChatMessages.RemoveAt(Index, 1, EAllowShrinking::No);
	}
}

bool ASpellRisePlayerController::IsChatWidgetReadyForAppend() const
{
	for (TFieldIterator<FObjectProperty> It(GetClass(), EFieldIterationFlags::IncludeSuper); It; ++It)
	{
		const FObjectProperty* ObjectProperty = *It;
		if (!ObjectProperty->GetName().Contains(TEXT("ChatWidgetRef"), ESearchCase::IgnoreCase))
		{
			continue;
		}

		UObject* PropertyValue = ObjectProperty->GetObjectPropertyValue_InContainer(this);
		UUserWidget* ChatWidget = Cast<UUserWidget>(PropertyValue);
		return IsValid(ChatWidget) && ChatWidget->WidgetTree != nullptr && ChatWidget->IsInViewport();
	}

	return false;
}

bool ASpellRisePlayerController::TryAppendPendingChatMessage(const FSpellRiseChatMessage& Message) const
{
	if (!IsChatWidgetReadyForAppend())
	{
		return false;
	}

	UObject* ControllerObject = const_cast<ASpellRisePlayerController*>(this);
	if (!ControllerObject)
	{
		return false;
	}

	UFunction* AddChatMessageFunction = ControllerObject->FindFunction(NAME_AddChatMessage);
	if (!AddChatMessageFunction)
	{
		return false;
	}

	const int32 ParamsSize = FMath::Max<int32>(1, static_cast<int32>(AddChatMessageFunction->ParmsSize));
	uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(ParamsSize));
	FMemory::Memzero(ParamsBuffer, ParamsSize);
	AddChatMessageFunction->InitializeStruct(ParamsBuffer);

	bool bWroteText = false;
	bool bWroteChannel = false;
	bool bFunctionHasChannel = false;

	for (TFieldIterator<FProperty> It(AddChatMessageFunction); It; ++It)
	{
		FProperty* Property = *It;
		if (!Property->HasAnyPropertyFlags(CPF_Parm) || Property->HasAnyPropertyFlags(CPF_ReturnParm))
		{
			continue;
		}

		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(ParamsBuffer);
		const FString PropertyName = Property->GetName();

		if (PropertyName.Contains(TEXT("Channel"), ESearchCase::IgnoreCase))
		{
			bFunctionHasChannel = true;

			if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
			{
				if (FNumericProperty* UnderlyingNumeric = EnumProperty->GetUnderlyingProperty())
				{
					UnderlyingNumeric->SetIntPropertyValue(ValuePtr, static_cast<uint64>(Message.Channel));
					bWroteChannel = true;
				}
			}
			else if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
			{
				ByteProperty->SetPropertyValue(ValuePtr, Message.Channel);
				bWroteChannel = true;
			}
		}
		else if (PropertyName.Contains(TEXT("TimeText"), ESearchCase::IgnoreCase))
		{
			if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
			{
				TextProperty->SetPropertyValue(ValuePtr, Message.TimeText);
			}
		}
		else if (PropertyName.Equals(TEXT("Text"), ESearchCase::IgnoreCase))
		{
			if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
			{
				TextProperty->SetPropertyValue(ValuePtr, Message.Text);
				bWroteText = true;
			}
		}
		else if (PropertyName.Equals(TEXT("Name"), ESearchCase::IgnoreCase))
		{
			if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
			{
				NameProperty->SetPropertyValue(ValuePtr, Message.Name);
			}
			else if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
			{
				StrProperty->SetPropertyValue(ValuePtr, Message.Name.ToString());
			}
		}
	}

	if (!bWroteText || (bFunctionHasChannel && !bWroteChannel))
	{
		AddChatMessageFunction->DestroyStruct(ParamsBuffer);
		return false;
	}

	ControllerObject->ProcessEvent(AddChatMessageFunction, ParamsBuffer);
	AddChatMessageFunction->DestroyStruct(ParamsBuffer);
	return true;
}

bool ASpellRisePlayerController::TryAppendCombatLogToChatChannel(const FString& MessageText) const
{
	const FCombatChatPayload Payload{
		FName(TEXT("Combat")),
		FText::FromString(MessageText),
		BuildCombatTimeText()
	};

	auto TryInvokeFunctionOnTarget = [&](UObject* TargetObject, const FName& FunctionName) -> bool
	{
		if (!TargetObject)
		{
			return false;
		}

		UFunction* ChatFunction = TargetObject->FindFunction(FunctionName);
		if (!ChatFunction)
		{
			return false;
		}

		const int32 ParamsSize = FMath::Max<int32>(1, static_cast<int32>(ChatFunction->ParmsSize));
		uint8* ParamsBuffer = static_cast<uint8*>(FMemory_Alloca(ParamsSize));
		FMemory::Memzero(ParamsBuffer, ParamsSize);
		ChatFunction->InitializeStruct(ParamsBuffer);

		bool bWroteText = false;
		bool bWroteChannel = false;
		bool bFunctionHasChannel = false;

		for (TFieldIterator<FProperty> It(ChatFunction); It; ++It)
		{
			FProperty* Property = *It;
			if (!Property->HasAnyPropertyFlags(CPF_Parm) || Property->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				continue;
			}

			PopulateChatPayloadRecursive(
				Property,
				ParamsBuffer,
				Payload,
				bWroteText,
				bWroteChannel,
				bFunctionHasChannel);
		}

		if (!bWroteText || (bFunctionHasChannel && !bWroteChannel))
		{
			ChatFunction->DestroyStruct(ParamsBuffer);
			return false;
		}

		TargetObject->ProcessEvent(ChatFunction, ParamsBuffer);
		ChatFunction->DestroyStruct(ParamsBuffer);
		return true;
	};

	UObject* ControllerObject = const_cast<ASpellRisePlayerController*>(this);
	if (TryInvokeFunctionOnTarget(ControllerObject, NAME_AddChatMessage) ||
		TryInvokeFunctionOnTarget(ControllerObject, NAME_AppendMessage))
	{
		return true;
	}

	UObject* ChatComponentObject = FindChatComponentObject();
	return TryInvokeFunctionOnTarget(ChatComponentObject, NAME_AddChatMessage) ||
		TryInvokeFunctionOnTarget(ChatComponentObject, NAME_AppendMessage);
}

UObject* ASpellRisePlayerController::FindChatComponentObject() const
{
	for (TFieldIterator<FObjectProperty> It(GetClass(), EFieldIterationFlags::IncludeSuper); It; ++It)
	{
		const FObjectProperty* ObjectProperty = *It;
		if (!ObjectProperty->GetName().Contains(TEXT("ChatComponent"), ESearchCase::IgnoreCase))
		{
			continue;
		}

		UObject* PropertyValue = ObjectProperty->GetObjectPropertyValue_InContainer(this);
		if (PropertyValue)
		{
			return PropertyValue;
		}
	}

	TInlineComponentArray<UActorComponent*> ActorComponents(this);
	for (UActorComponent* Component : ActorComponents)
	{
		if (!Component)
		{
			continue;
		}

		const bool bLooksLikeChatComponent =
			Component->GetName().Contains(TEXT("ChatComponent"), ESearchCase::IgnoreCase) ||
			Component->FindFunction(NAME_AddChatMessage) != nullptr;

		if (bLooksLikeChatComponent)
		{
			return Component;
		}
	}

	return nullptr;
}

bool ASpellRisePlayerController::GetCurrentAimTraceResult(FSpellRiseAimTraceResult& OutResult) const
{
	return BuildAimTraceResult(
		OutResult,
		DefaultAimTraceDistance,
		DefaultAimTraceRadius,
		DefaultAimTraceChannel,
		bDefaultAimTraceComplex);
}

bool ASpellRisePlayerController::BuildAimTraceResult(
	FSpellRiseAimTraceResult& OutResult,
	float TraceDistance,
	float TraceRadius,
	ECollisionChannel TraceChannel,
	bool bTraceComplex) const
{
	OutResult = FSpellRiseAimTraceResult();

	if (!IsLocalController())
	{
		return false;
	}

	if (!PlayerCameraManager)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
	const FRotator CameraRotation = PlayerCameraManager->GetCameraRotation();
	const FVector CameraDirection = CameraRotation.Vector().GetSafeNormal();

	if (CameraDirection.IsNearlyZero())
	{
		return false;
	}

	const float SafeTraceDistance = FMath::Max(1.f, TraceDistance);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpellRise_CurrentAimTrace), bTraceComplex);
	QueryParams.bReturnPhysicalMaterial = false;

	TArray<AActor*> ActorsToIgnore;
	CollectAimIgnoredActors(ActorsToIgnore);

	for (AActor* IgnoredActor : ActorsToIgnore)
	{
		if (IsValid(IgnoredActor))
		{
			QueryParams.AddIgnoredActor(IgnoredActor);
		}
	}

	// Stage 1: camera trace to acquire the desired aim point.
	const FVector CameraTraceStart = CameraLocation;
	const FVector CameraTraceEnd = CameraTraceStart + (CameraDirection * SafeTraceDistance);

	FHitResult CameraHitResult;
	const bool bCameraHit = World->LineTraceSingleByChannel(
		CameraHitResult,
		CameraTraceStart,
		CameraTraceEnd,
		TraceChannel,
		QueryParams);

	const FVector DesiredTargetPoint = bCameraHit ? CameraHitResult.ImpactPoint : CameraTraceEnd;

	// Stage 2: validate from the controlled pawn view location.
	// This avoids camera/muzzle parallax and prevents aiming through near walls.
	const FVector SourceTraceStart = ResolveTraceOrigin(this);
	if (SourceTraceStart.IsNearlyZero())
	{
		return false;
	}

	FVector SourceToTarget = DesiredTargetPoint - SourceTraceStart;
	if (SourceToTarget.IsNearlyZero())
	{
		SourceToTarget = CameraDirection * SafeTraceDistance;
	}

	const FVector SourceDirection = SourceToTarget.GetSafeNormal();
	if (SourceDirection.IsNearlyZero())
	{
		return false;
	}

	const FVector SourceTraceEnd = SourceTraceStart + (SourceDirection * SafeTraceDistance);

	FHitResult SourceHitResult;
	const bool bSourceHit = World->LineTraceSingleByChannel(
		SourceHitResult,
		SourceTraceStart,
		SourceTraceEnd,
		TraceChannel,
		QueryParams);

	OutResult.bHit = bSourceHit;
	OutResult.HitResult = SourceHitResult;
	OutResult.TraceStart = SourceTraceStart;
	OutResult.TraceEnd = SourceTraceEnd;
	OutResult.AimDirection = SourceDirection;
	OutResult.TargetPoint = bSourceHit ? SourceHitResult.ImpactPoint : DesiredTargetPoint;

	// Mantém compatibilidade com quem usa HitResult.TraceStart/TraceEnd.
	if (!bSourceHit)
	{
		OutResult.HitResult = FHitResult();
	}

	OutResult.HitResult.TraceStart = SourceTraceStart;
	OutResult.HitResult.TraceEnd = SourceTraceEnd;

	return true;
}

void ASpellRisePlayerController::CollectAimIgnoredActors(TArray<AActor*>& OutActorsToIgnore) const
{
	if (APawn* ControlledPawn = GetPawn())
	{
		OutActorsToIgnore.AddUnique(ControlledPawn);

		TArray<AActor*> AttachedActors;
		ControlledPawn->GetAttachedActors(AttachedActors);

		for (AActor* AttachedActor : AttachedActors)
		{
			if (IsValid(AttachedActor))
			{
				OutActorsToIgnore.AddUnique(AttachedActor);
			}
		}
	}

	if (AActor* ViewTargetActor = GetViewTarget())
	{
		OutActorsToIgnore.AddUnique(ViewTargetActor);
	}
}
