#include "SpellRise/Persistence/SpellRisePersistenceSubsystem.h"

#include "EngineUtils.h"
#include "JsonObjectConverter.h"
#include "Components/ActorComponent.h"
#include "Components/ChildActorComponent.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Base64.h"
#include "Misc/CommandLine.h"
#include "OnlineSubsystem.h"
#include "Misc/Parse.h"
#include "Misc/ScopeExit.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/UnrealType.h"
#include "InventoryComponent.h"
#include "InventoryFunctionLibrary.h"
#include "NarrativeItem.h"

#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/Persistence/SpellRiseBuildPersistenceAdapter.h"
#include "SpellRise/Persistence/SpellRiseFilePersistenceProvider.h"
#include "SpellRise/Persistence/SpellRisePostgresPersistenceProvider.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRisePersistence, Log, All);

namespace
{
	constexpr int32 PersistenceCharacterSchemaVersion = 5;
	constexpr int32 PersistenceInventorySchemaVersion = 2;
	constexpr int32 PersistenceWorldSchemaVersion = 1;
	constexpr int32 LegacyPersistenceWorldSchemaVersion = 5;
	constexpr uint8 SlotEncodingLegacyRawBase64 = 0;
	constexpr uint8 SlotEncodingStructJson = 1;
	constexpr double RespawnBedLocationToleranceSq = 300.0 * 300.0;
	constexpr double SpawnActorLocationMatchToleranceSq = 25.0 * 25.0;
	constexpr double WorldEquipConflictRadius = 350.0;

	FString GetRequestedPersistenceProvider()
	{
		FString CmdProvider;
		if (FParse::Value(FCommandLine::Get(), TEXT("SRPersistenceProvider="), CmdProvider))
		{
			CmdProvider.TrimStartAndEndInline();
			CmdProvider.ToLowerInline();
			if (!CmdProvider.IsEmpty())
			{
				return CmdProvider;
			}
		}
		return TEXT("file");
	}

	struct FPersistenceInventoryStats
	{
		int32 ComponentCount = 0;
		int32 ItemStackCount = 0;
		int32 ItemQuantity = 0;
	};

	struct FInventoryComponentBinding
	{
		TObjectPtr<UActorComponent> Component = nullptr;
		FArrayProperty* SlotArrayProperty = nullptr;
		FStructProperty* SlotStructProperty = nullptr;
		ESpellRiseSaveOwnerScope OwnerScope = ESpellRiseSaveOwnerScope::Unknown;
		ESpellRiseSaveContainerRole ContainerRole = ESpellRiseSaveContainerRole::Unknown;
	};

	bool IsValidPersistentId(const FString& PersistentId)
	{
		if (PersistentId.IsEmpty())
		{
			return false;
		}

		FString TrimmedPersistentId = PersistentId;
		TrimmedPersistentId.TrimStartAndEndInline();
		return !TrimmedPersistentId.IsEmpty() && TrimmedPersistentId == PersistentId;
	}

	bool IsFiniteTransform(const FTransform& Transform)
	{
		return !Transform.GetLocation().ContainsNaN()
			&& !Transform.GetRotation().ContainsNaN()
			&& !Transform.GetScale3D().ContainsNaN();
	}

	bool ValidateWorldSpawnTransform(const UWorld* World, const FTransform& Transform, FString& OutReason)
	{
		OutReason.Reset();

		if (!World)
		{
			OutReason = TEXT("missing_world");
			return false;
		}

		if (!IsFiniteTransform(Transform))
		{
			OutReason = TEXT("non_finite_transform");
			return false;
		}

		const FVector Location = Transform.GetLocation();
		constexpr float MaxWorldCoordAbs = 200000.0f;
		if (FMath::Abs(Location.X) > MaxWorldCoordAbs || FMath::Abs(Location.Y) > MaxWorldCoordAbs || FMath::Abs(Location.Z) > MaxWorldCoordAbs)
		{
			OutReason = TEXT("out_of_bounds");
			return false;
		}

		const FVector TraceStart = Location + FVector(0.0f, 0.0f, 4000.0f);
		const FVector TraceEnd = Location - FVector(0.0f, 0.0f, 12000.0f);
		FHitResult GroundHit;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpellRisePersistSpawnValidation), false);
		const bool bHasGround = World->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
		if (!bHasGround)
		{
			OutReason = TEXT("no_ground_trace_hit");
			return false;
		}

		const float VerticalOffsetToGround = FMath::Abs(Location.Z - GroundHit.ImpactPoint.Z);
		constexpr float MaxVerticalOffsetToGround = 1500.0f;
		if (VerticalOffsetToGround > MaxVerticalOffsetToGround)
		{
			OutReason = TEXT("ground_offset_too_large");
			return false;
		}

		constexpr float MinCharacterCenterClearanceFromGround = 60.0f;
		if (Location.Z < (GroundHit.ImpactPoint.Z + MinCharacterCenterClearanceFromGround))
		{
			OutReason = TEXT("below_ground_clearance");
			return false;
		}

		FCollisionQueryParams OverlapParams(SCENE_QUERY_STAT(SpellRisePersistSpawnOverlapValidation), false);
		const FCollisionShape CharacterCapsule = FCollisionShape::MakeCapsule(34.0f, 88.0f);
		const bool bCapsuleBlocked = World->OverlapBlockingTestByChannel(
			Location,
			FQuat::Identity,
			ECC_Pawn,
			CharacterCapsule,
			OverlapParams);
		if (bCapsuleBlocked)
		{
			OutReason = TEXT("blocked_capsule_overlap");
			return false;
		}

		return true;
	}

	FPersistenceInventoryStats ComputeInventoryStats(const TArray<FSpellRiseSavedInventoryComponent>& Components)
	{
		FPersistenceInventoryStats Stats;
		Stats.ComponentCount = Components.Num();

		for (const FSpellRiseSavedInventoryComponent& Component : Components)
		{
			for (const FSpellRiseSavedNarrativeItem& Item : Component.Items)
			{
				if (Item.ItemClassPath.IsEmpty() || Item.Quantity <= 0)
				{
					continue;
				}

				++Stats.ItemStackCount;
				Stats.ItemQuantity += Item.Quantity;
			}
		}

		return Stats;
	}

	bool ValidateCharacterSaveData(const FSpellRiseCharacterSaveData& Data, const FString& ExpectedSteamId64, FString& OutReason)
	{
		OutReason.Reset();

		if (Data.SchemaVersion <= 0)
		{
			OutReason = TEXT("invalid_schema");
			return false;
		}

		if (Data.SchemaVersion > PersistenceCharacterSchemaVersion)
		{
			OutReason = TEXT("future_schema");
			return false;
		}

		if (!IsValidPersistentId(Data.SteamId64))
		{
			OutReason = TEXT("invalid_persistent_id");
			return false;
		}

		if (Data.SteamId64 != ExpectedSteamId64)
		{
			OutReason = TEXT("steam_id_mismatch");
			return false;
		}

		if (!IsFiniteTransform(Data.CharacterTransform))
		{
			OutReason = TEXT("invalid_transform");
			return false;
		}

		const float NumericValues[] = {
			Data.Strength,
			Data.Agility,
			Data.Intelligence,
			Data.Wisdom,
			Data.Health,
			Data.Mana,
			Data.Stamina,
			Data.CatalystCharge,
			Data.CatalystXP,
			Data.CatalystLevel
		};
		for (const float NumericValue : NumericValues)
		{
			if (!FMath::IsFinite(NumericValue))
			{
				OutReason = TEXT("non_finite_attribute");
				return false;
			}
		}

		return true;
	}

	bool ValidateInventorySaveData(const FSpellRiseInventorySaveData& Data, const FString& ExpectedSteamId64, FString& OutReason)
	{
		OutReason.Reset();

		if (Data.SchemaVersion <= 0)
		{
			OutReason = TEXT("invalid_schema");
			return false;
		}

		if (Data.SchemaVersion > PersistenceInventorySchemaVersion)
		{
			OutReason = TEXT("future_schema");
			return false;
		}

		if (!IsValidPersistentId(Data.SteamId64))
		{
			OutReason = TEXT("invalid_persistent_id");
			return false;
		}

		if (Data.SteamId64 != ExpectedSteamId64)
		{
			OutReason = TEXT("steam_id_mismatch");
			return false;
		}

		return true;
	}

	bool ValidateWorldSaveData(const FSpellRiseWorldSaveData& Data, const FString& ExpectedWorldId, FString& OutReason)
	{
		OutReason.Reset();

		if (Data.SchemaVersion <= 0)
		{
			OutReason = TEXT("invalid_schema");
			return false;
		}

		if (Data.SchemaVersion != PersistenceWorldSchemaVersion && Data.SchemaVersion != LegacyPersistenceWorldSchemaVersion)
		{
			OutReason = TEXT("schema_mismatch");
			return false;
		}

		if (!IsValidPersistentId(ExpectedWorldId) || !IsValidPersistentId(Data.WorldId))
		{
			OutReason = TEXT("invalid_world_id");
			return false;
		}

		if (Data.WorldId != ExpectedWorldId)
		{
			OutReason = TEXT("world_id_mismatch");
			return false;
		}

		return true;
	}

	bool IsServerWorld(const UWorld* World)
	{
		if (!World)
		{
			return false;
		}

		const ENetMode NetMode = World->GetNetMode();
		return NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer;
	}

	FArrayProperty* PersistenceFindItemSlotsProperty(const UActorComponent* Component)
	{
		if (!Component || !Component->GetClass())
		{
			return nullptr;
		}

		return FindFProperty<FArrayProperty>(Component->GetClass(), TEXT("ItemSlots"));
	}

	FArrayProperty* FindArrayPropertyByName(const UActorComponent* Component, const FName PropertyName)
	{
		if (!Component || !Component->GetClass() || PropertyName.IsNone())
		{
			return nullptr;
		}

		return FindFProperty<FArrayProperty>(Component->GetClass(), PropertyName);
	}

	bool IsSerializableSlotArrayName(const FName PropertyName)
	{
		if (PropertyName.IsNone())
		{
			return false;
		}

		static const TSet<FName> PreferredNames = {
			FName(TEXT("ItemSlots")),
			FName(TEXT("HotbarSlots")),
			FName(TEXT("QuickbarSlots")),
			FName(TEXT("EquipmentSlots")),
			FName(TEXT("EquipSlots")),
			FName(TEXT("ActionBarSlots")),
			FName(TEXT("WeaponSlots")),
			FName(TEXT("CatalystSlots"))
		};

		if (PreferredNames.Contains(PropertyName))
		{
			return true;
		}

		return PropertyName.ToString().Contains(TEXT("Slot"), ESearchCase::IgnoreCase);
	}

	ESpellRiseSaveContainerRole ClassifyContainerRole(const UActorComponent* Component, const FArrayProperty* ArrayProperty)
	{
		if (!Component || !ArrayProperty)
		{
			return ESpellRiseSaveContainerRole::Unknown;
		}

		const FString ComponentName = Component->GetName().ToLower();
		const FString ClassPath = Component->GetClass() ? Component->GetClass()->GetPathName().ToLower() : FString();
		const FString ArrayName = ArrayProperty->GetName().ToLower();

		const bool bStorage = ComponentName.Contains(TEXT("storage")) || ClassPath.Contains(TEXT("storage"));
		if (bStorage)
		{
			return ESpellRiseSaveContainerRole::Storage;
		}

		if (ArrayName.Contains(TEXT("hotbar")) || ArrayName.Contains(TEXT("quickbar")) || ArrayName.Contains(TEXT("actionbar")) || ComponentName.Contains(TEXT("hotbar")))
		{
			return ESpellRiseSaveContainerRole::Hotbar;
		}

		if (ArrayName.Contains(TEXT("equip")) || ArrayName.Contains(TEXT("weapon")) || ComponentName.Contains(TEXT("equip")) || ComponentName.Contains(TEXT("weapon")))
		{
			return ESpellRiseSaveContainerRole::Equipment;
		}

		if (ComponentName.Contains(TEXT("inventory")) || ArrayName.Equals(TEXT("itemslots")))
		{
			return ESpellRiseSaveContainerRole::Inventory;
		}

		return ESpellRiseSaveContainerRole::Unknown;
	}

	bool IsSerializableSlotArrayProperty(const FArrayProperty* ArrayProperty)
	{
		if (!ArrayProperty)
		{
			return false;
		}

		if (!IsSerializableSlotArrayName(ArrayProperty->GetFName()))
		{
			return false;
		}

		const FStructProperty* SlotStructProperty = CastField<FStructProperty>(ArrayProperty->Inner);
		return SlotStructProperty && SlotStructProperty->Struct;
	}

	bool StructContainsUnsafeObjectReference(const UStruct* StructType)
	{
		if (!StructType)
		{
			return true;
		}

		for (TFieldIterator<FProperty> It(StructType, EFieldIterationFlags::IncludeSuper); It; ++It)
		{
			const FProperty* Property = *It;
			if (!Property)
			{
				continue;
			}

			TArray<const FStructProperty*> EncounteredStructProps;
			if (Property->ContainsObjectReference(EncounteredStructProps, EPropertyObjectReferenceType::Strong | EPropertyObjectReferenceType::Weak))
			{
				return true;
			}

			if (const FStructProperty* NestedStructProperty = CastField<FStructProperty>(Property))
			{
				if (StructContainsUnsafeObjectReference(NestedStructProperty->Struct))
				{
					return true;
				}
			}
		}

		return false;
	}

	bool IsSafeSlotStructForRawPersistence(const UScriptStruct* SlotStruct)
	{
		return SlotStruct && !StructContainsUnsafeObjectReference(SlotStruct);
	}

	void GatherSerializableSlotArrayProperties(const UActorComponent* Component, TArray<FArrayProperty*>& OutProperties)
	{
		if (!Component || !Component->GetClass())
		{
			return;
		}

		for (TFieldIterator<FArrayProperty> It(Component->GetClass(), EFieldIterationFlags::IncludeSuper); It; ++It)
		{
			FArrayProperty* ArrayProperty = *It;
			if (!IsSerializableSlotArrayProperty(ArrayProperty))
			{
				continue;
			}

			OutProperties.AddUnique(ArrayProperty);
		}
	}

	bool PersistenceIsInventoryComponent(const UActorComponent* Component)
	{
		if (!Component)
		{
			return false;
		}

		TArray<FArrayProperty*> SlotArrays;
		GatherSerializableSlotArrayProperties(Component, SlotArrays);
		return SlotArrays.Num() > 0;
	}

	void PersistenceGatherInventoryComponents(AActor* Owner, TArray<UActorComponent*>& OutComponents)
	{
		if (!Owner)
		{
			return;
		}

		TArray<UActorComponent*> Components;
		Owner->GetComponents(Components);

		for (UActorComponent* Component : Components)
		{
			if (PersistenceIsInventoryComponent(Component))
			{
				OutComponents.AddUnique(Component);
			}
		}
	}

	FString EncodeSlotToBase64(UScriptStruct* SlotStruct, const void* SlotMemory)
	{
		if (!SlotStruct || !SlotMemory)
		{
			return FString();
		}

		TArray<uint8> Bytes;
		Bytes.SetNumUninitialized(SlotStruct->GetStructureSize());
		SlotStruct->CopyScriptStruct(Bytes.GetData(), SlotMemory);
		const FString Encoded = FBase64::Encode(Bytes);
		SlotStruct->DestroyStruct(Bytes.GetData());
		return Encoded;
	}

	bool DecodeSlotFromBase64(UScriptStruct* SlotStruct, const FString& DataBase64, TArray<uint8>& OutBytes)
	{
		if (!SlotStruct || DataBase64.IsEmpty())
		{
			return false;
		}

		TArray<uint8> Decoded;
		if (!FBase64::Decode(DataBase64, Decoded))
		{
			return false;
		}

		if (Decoded.Num() != SlotStruct->GetStructureSize())
		{
			return false;
		}

		OutBytes = MoveTemp(Decoded);
		return true;
	}

	bool TrySerializeSlotToJson(UScriptStruct* SlotStruct, const void* SlotMemory, FString& OutJson)
	{
		OutJson.Reset();
		if (!SlotStruct || !SlotMemory)
		{
			return false;
		}

		return FJsonObjectConverter::UStructToJsonObjectString(SlotStruct, SlotMemory, OutJson, 0, 0);
	}

	bool TryDeserializeSlotFromJson(UScriptStruct* SlotStruct, const FString& Json, void* OutSlotMemory)
	{
		if (!SlotStruct || !OutSlotMemory || Json.IsEmpty())
		{
			return false;
		}

		TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
		if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
		{
			return false;
		}

		return FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), SlotStruct, OutSlotMemory, 0, 0);
	}

	bool PersistenceIsLikelyBedClassPath(const FString& ClassPath)
	{
		return ClassPath.Contains(TEXT("Building_Bed"), ESearchCase::IgnoreCase)
			|| ClassPath.Contains(TEXT("_Bed"), ESearchCase::IgnoreCase);
	}

	void DestroyOwnedEquipmentVisualActors(ASpellRiseCharacterBase* Character)
	{
		if (!Character)
		{
			return;
		}

		TArray<UChildActorComponent*> ChildActorComps;
		Character->GetComponents<UChildActorComponent>(ChildActorComps);
		for (UChildActorComponent* ChildActorComp : ChildActorComps)
		{
			if (ChildActorComp)
			{
				ChildActorComp->DestroyChildActor();
			}
		}

		TArray<AActor*> AttachedActors;
		Character->GetAttachedActors(AttachedActors);
		for (AActor* AttachedActor : AttachedActors)
		{
			if (AttachedActor && AttachedActor->GetOwner() == Character)
			{
				AttachedActor->Destroy();
			}
		}
	}

	void InvokeNoParamFunctionIfExists(UObject* Target, std::initializer_list<const TCHAR*> CandidateNames)
	{
		if (!Target)
		{
			return;
		}

		for (const TCHAR* CandidateName : CandidateNames)
		{
			if (!CandidateName)
			{
				continue;
			}

			UFunction* Function = Target->FindFunction(FName(CandidateName));
			if (!Function || Function->NumParms != 0)
			{
				continue;
			}

			Target->ProcessEvent(Function, nullptr);
		}
	}
}

void USpellRisePersistenceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const FString RequestedProvider = GetRequestedPersistenceProvider();
	UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][ProviderInit] Requested=%s"), *RequestedProvider);

	TUniquePtr<ISpellRisePersistenceProvider> SelectedProvider;
	if (RequestedProvider.Equals(TEXT("postgres"), ESearchCase::IgnoreCase))
	{
		SelectedProvider = MakeUnique<FSpellRisePostgresPersistenceProvider>();
		if (!SelectedProvider->IsReady())
		{
			UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][ProviderInit] Postgres provider not ready, falling back to file provider."));
			SelectedProvider.Reset();
		}
	}

	if (!SelectedProvider.IsValid())
	{
		SelectedProvider = MakeUnique<FSpellRiseFilePersistenceProvider>();
	}

	UE_LOG(
		LogSpellRisePersistence,
		Log,
		TEXT("[Persistence][ProviderInit] Active=%s Ready=%d"),
		*SelectedProvider->GetProviderName(),
		SelectedProvider->IsReady() ? 1 : 0);

	Provider = MoveTemp(SelectedProvider);
	CachedCharacterDataBySteamId.Reset();
	CachedInventoryDataBySteamId.Reset();
	CharacterRevisionBySteamId.Reset();
	InventoryRevisionBySteamId.Reset();
	PersistentIdByController.Reset();
	DirtyCharacterIds.Reset();
	SaveInProgressPersistentIds.Reset();
	bWorldDirty = true;
}

void USpellRisePersistenceSubsystem::Deinitialize()
{
	Provider.Reset();
	CachedCharacterDataBySteamId.Reset();
	CachedInventoryDataBySteamId.Reset();
	CharacterRevisionBySteamId.Reset();
	InventoryRevisionBySteamId.Reset();
	PersistentIdByController.Reset();
	DirtyCharacterIds.Reset();
	SaveInProgressPersistentIds.Reset();
	bWorldDirty = false;

	Super::Deinitialize();
}

bool USpellRisePersistenceSubsystem::PreloadCharacterForController(AController* Controller)
{
	if (!Provider || !Controller)
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][PreloadRejected] Controller=%s Reason=%s"),
			*GetNameSafe(Controller),
			!Provider ? TEXT("missing_provider") : TEXT("missing_controller"));
		return false;
	}

	if (ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState))
	{
		SRPlayerState->SetPersistenceProfileApplied(false);
	}

	const FString SteamId64 = ResolveSteamIdFromController(Controller);
	if (SteamId64.IsEmpty())
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][PreloadRejected] Controller=%s Reason=missing_persistent_id"), *GetNameSafe(Controller));
		return false;
	}

	FSpellRiseCharacterSaveData LoadedData;
	int64 LoadedCharacterRevision = 0;
	if (!Provider->LoadCharacterState(SteamId64, LoadedData, LoadedCharacterRevision))
	{
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][PreloadMiss] PersistentId=%s Controller=%s"), *SteamId64, *GetNameSafe(Controller));
		return false;
	}

	FString CharacterValidationReason;
	if (!ValidateCharacterSaveData(LoadedData, SteamId64, CharacterValidationReason))
	{
		CachedCharacterDataBySteamId.Remove(SteamId64);
		CachedInventoryDataBySteamId.Remove(SteamId64);
		CharacterRevisionBySteamId.Remove(SteamId64);
		InventoryRevisionBySteamId.Remove(SteamId64);
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][PreloadRejected] PersistentId=%s Controller=%s CharacterRevision=%lld Reason=%s"),
			*SteamId64,
			*GetNameSafe(Controller),
			LoadedCharacterRevision,
			*CharacterValidationReason);
		return false;
	}

	FSpellRiseInventorySaveData LoadedInventoryData;
	int64 LoadedInventoryRevision = 0;
	bool bHasInventoryData = Provider->LoadInventoryState(SteamId64, LoadedInventoryData, LoadedInventoryRevision);
	if (bHasInventoryData)
	{
		FString InventoryValidationReason;
		if (ValidateInventorySaveData(LoadedInventoryData, SteamId64, InventoryValidationReason))
		{
			MergeInventorySnapshotIntoCharacterData(LoadedInventoryData, LoadedData);
			CachedInventoryDataBySteamId.Add(SteamId64, MoveTemp(LoadedInventoryData));
			InventoryRevisionBySteamId.Add(SteamId64, LoadedInventoryRevision);
		}
		else
		{
			bHasInventoryData = false;
			LoadedInventoryRevision = 0;
			CachedInventoryDataBySteamId.Remove(SteamId64);
			InventoryRevisionBySteamId.Remove(SteamId64);
			UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][InventoryLoadRejected] PersistentId=%s Controller=%s InventoryRevision=%lld Reason=%s"),
				*SteamId64,
				*GetNameSafe(Controller),
				LoadedInventoryRevision,
				*InventoryValidationReason);
		}
	}
	else
	{
		CachedInventoryDataBySteamId.Remove(SteamId64);
		InventoryRevisionBySteamId.Remove(SteamId64);
	}

	CachedCharacterDataBySteamId.Add(SteamId64, MoveTemp(LoadedData));
	CharacterRevisionBySteamId.Add(SteamId64, LoadedCharacterRevision);
	const FPersistenceInventoryStats CharacterStats = ComputeInventoryStats(CachedCharacterDataBySteamId.FindChecked(SteamId64).InventoryComponents);
	const FPersistenceInventoryStats InventoryStats = bHasInventoryData ? ComputeInventoryStats(CachedInventoryDataBySteamId.FindChecked(SteamId64).InventoryComponents) : FPersistenceInventoryStats{};
	UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][PreloadOk] PersistentId=%s Controller=%s CharacterRevision=%lld InventoryRevision=%lld CharacterComponents=%d CharacterStacks=%d CharacterQuantity=%d InventoryLoaded=%d InventoryComponents=%d InventoryStacks=%d InventoryQuantity=%d"),
		*SteamId64,
		*GetNameSafe(Controller),
		LoadedCharacterRevision,
		LoadedInventoryRevision,
		CharacterStats.ComponentCount,
		CharacterStats.ItemStackCount,
		CharacterStats.ItemQuantity,
		bHasInventoryData ? 1 : 0,
		InventoryStats.ComponentCount,
		InventoryStats.ItemStackCount,
		InventoryStats.ItemQuantity);
	return true;
}

bool USpellRisePersistenceSubsystem::ApplyCachedCharacterToController(AController* Controller)
{
	if (!Controller)
	{
		return false;
	}

	const FString SteamId64 = ResolveSteamIdFromController(Controller);
	if (SteamId64.IsEmpty())
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][ApplyRejected] Controller=%s Reason=missing_persistent_id"), *GetNameSafe(Controller));
		return false;
	}

	const FSpellRiseCharacterSaveData* CachedData = CachedCharacterDataBySteamId.Find(SteamId64);
	if (!CachedData)
	{
		EnsureDefaultItemsForControllerIfNeeded(Controller, TEXT("no_cached_data"));
		if (ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState))
		{
			SRPlayerState->SetPersistenceProfileApplied(true);
		}
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][ApplySkipped] PersistentId=%s Controller=%s Reason=no_cached_data"), *SteamId64, *GetNameSafe(Controller));
		return false;
	}

	FString ValidationReason;
	if (!ValidateCharacterSaveData(*CachedData, SteamId64, ValidationReason))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][ApplyRejected] PersistentId=%s Controller=%s Reason=%s"),
			*SteamId64,
			*GetNameSafe(Controller),
			*ValidationReason);
		return false;
	}

	const bool bApplied = ApplyCharacterDataToController(Controller, *CachedData);
	if (ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState))
	{
		SRPlayerState->SetPersistenceProfileApplied(true);
	}
	const FPersistenceInventoryStats CachedStats = ComputeInventoryStats(CachedData->InventoryComponents);
	UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][ApplyResult] PersistentId=%s Controller=%s Applied=%d CharacterComponents=%d CharacterStacks=%d CharacterQuantity=%d"),
		*SteamId64,
		*GetNameSafe(Controller),
		bApplied ? 1 : 0,
		CachedStats.ComponentCount,
		CachedStats.ItemStackCount,
		CachedStats.ItemQuantity);
	return bApplied;
}

bool USpellRisePersistenceSubsystem::SaveCharacterForController(AController* Controller)
{
	if (!Provider || !Controller)
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveRejected] Controller=%s Reason=%s"),
			*GetNameSafe(Controller),
			!Provider ? TEXT("missing_provider") : TEXT("missing_controller"));
		return false;
	}

	const FString SteamId64 = ResolveSteamIdFromController(Controller);
	if (SteamId64.IsEmpty())
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveRejected] Controller=%s Reason=missing_persistent_id"), *GetNameSafe(Controller));
		return false;
	}

	if (SaveInProgressPersistentIds.Contains(SteamId64))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveRejected] PersistentId=%s Controller=%s Reason=save_already_in_progress"), *SteamId64, *GetNameSafe(Controller));
		return false;
	}

	SaveInProgressPersistentIds.Add(SteamId64);
	ON_SCOPE_EXIT
	{
		SaveInProgressPersistentIds.Remove(SteamId64);
	};

	const double SaveStartSeconds = FPlatformTime::Seconds();
	FSpellRiseCharacterSaveData SaveData;
	if (!CollectCharacterData(Controller, SteamId64, SaveData))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveRejected] PersistentId=%s Controller=%s Reason=collect_failed"), *SteamId64, *GetNameSafe(Controller));
		return false;
	}

	FSpellRiseInventorySaveData InventoryData;
	BuildInventorySnapshotFromCharacterData(SaveData, SteamId64, InventoryData);

	FString CharacterValidationReason;
	if (!ValidateCharacterSaveData(SaveData, SteamId64, CharacterValidationReason))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveRejected] PersistentId=%s Controller=%s Reason=%s"), *SteamId64, *GetNameSafe(Controller), *CharacterValidationReason);
		return false;
	}

	FString InventoryValidationReason;
	if (!ValidateInventorySaveData(InventoryData, SteamId64, InventoryValidationReason))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveRejected] PersistentId=%s Controller=%s Reason=%s"), *SteamId64, *GetNameSafe(Controller), *InventoryValidationReason);
		return false;
	}

	const int64 ExpectedCharacterRevision = CharacterRevisionBySteamId.FindRef(SteamId64);
	const int64 ExpectedInventoryRevision = InventoryRevisionBySteamId.FindRef(SteamId64);
	const int64 TargetRevision = FMath::Max(ExpectedCharacterRevision, ExpectedInventoryRevision) + 1;

	const FSpellRiseCharacterSaveData PreviousCharacterData = CachedCharacterDataBySteamId.FindRef(SteamId64);
	const bool bHadPreviousCharacter = CachedCharacterDataBySteamId.Contains(SteamId64);
	const bool bHadPreviousInventory = CachedInventoryDataBySteamId.Contains(SteamId64);

	const bool bCharacterSaved = Provider->SaveCharacterState(SteamId64, SaveData, ExpectedCharacterRevision, TargetRevision);
	if (!bCharacterSaved)
	{
		const double SaveLatencyMs = (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0;
		const FPersistenceInventoryStats SaveStats = ComputeInventoryStats(SaveData.InventoryComponents);
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveCharacterFailed] PersistentId=%s Controller=%s RevisionExpected=%lld RevisionTarget=%lld LatencyMs=%.2f CharacterComponents=%d CharacterStacks=%d CharacterQuantity=%d Reason=character_write_rejected"),
			*SteamId64,
			*GetNameSafe(Controller),
			ExpectedCharacterRevision,
			TargetRevision,
			SaveLatencyMs,
			SaveStats.ComponentCount,
			SaveStats.ItemStackCount,
			SaveStats.ItemQuantity);
		return false;
	}

	const bool bInventorySaved = Provider->SaveInventoryState(SteamId64, InventoryData, ExpectedInventoryRevision, TargetRevision);
	if (!bInventorySaved)
	{
		bool bRollbackCharacterSaved = false;
		if (bHadPreviousCharacter)
		{
			bRollbackCharacterSaved = Provider->SaveCharacterState(SteamId64, PreviousCharacterData, TargetRevision, ExpectedCharacterRevision);
		}

		const double SaveLatencyMs = (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0;
		const FPersistenceInventoryStats SaveStats = ComputeInventoryStats(SaveData.InventoryComponents);
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveCharacterFailed] PersistentId=%s Controller=%s RevisionExpectedCharacter=%lld RevisionExpectedInventory=%lld RevisionTarget=%lld LatencyMs=%.2f CharacterComponents=%d CharacterStacks=%d CharacterQuantity=%d Reason=inventory_write_rejected RollbackCharacterAttempted=%d RollbackCharacterSucceeded=%d"),
			*SteamId64,
			*GetNameSafe(Controller),
			ExpectedCharacterRevision,
			ExpectedInventoryRevision,
			TargetRevision,
			SaveLatencyMs,
			SaveStats.ComponentCount,
			SaveStats.ItemStackCount,
			SaveStats.ItemQuantity,
			bHadPreviousCharacter ? 1 : 0,
			bRollbackCharacterSaved ? 1 : 0);
		return false;
	}

	CachedCharacterDataBySteamId.Add(SteamId64, SaveData);
	CachedInventoryDataBySteamId.Add(SteamId64, InventoryData);
	CharacterRevisionBySteamId.Add(SteamId64, TargetRevision);
	InventoryRevisionBySteamId.Add(SteamId64, TargetRevision);
	DirtyCharacterIds.Remove(SteamId64);

	if (!bHadPreviousInventory)
	{
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][InventoryBootstrap] PersistentId=%s Revision=%lld"), *SteamId64, TargetRevision);
	}

	const double SaveLatencyMs = (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0;
	const FPersistenceInventoryStats SaveStats = ComputeInventoryStats(SaveData.InventoryComponents);
	UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][SaveCharacterOk] PersistentId=%s Controller=%s Revision=%lld LatencyMs=%.2f CharacterComponents=%d CharacterStacks=%d CharacterQuantity=%d"),
		*SteamId64,
		*GetNameSafe(Controller),
		TargetRevision,
		SaveLatencyMs,
		SaveStats.ComponentCount,
		SaveStats.ItemStackCount,
		SaveStats.ItemQuantity);

	if (!bHadPreviousCharacter)
	{
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][CharacterBootstrap] PersistentId=%s Revision=%lld"), *SteamId64, TargetRevision);
	}

	return true;
}

bool USpellRisePersistenceSubsystem::SaveWorld(UWorld* World)
{
	if (!Provider || !World)
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveWorldRejected] World=%s Reason=%s"),
			*GetNameSafe(World),
			!Provider ? TEXT("missing_provider") : TEXT("missing_world"));
		return false;
	}

	if (!IsServerWorld(World))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveWorldRejected] World=%s Reason=non_server_world"), *GetNameSafe(World));
		return false;
	}

	if (!bWorldDirty)
	{
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][SaveWorldSkipped] World=%s Reason=not_dirty"), *GetNameSafe(World));
		return false;
	}

	FSpellRiseWorldSaveData WorldSaveData;
	WorldSaveData.SchemaVersion = PersistenceWorldSchemaVersion;
	WorldSaveData.WorldId = ResolveWorldId(World);
	FSpellRiseBuildPersistenceAdapter::GatherWorldBuildingActors(World, WorldSaveData.BuildingActors);

	FString ValidationReason;
	if (!ValidateWorldSaveData(WorldSaveData, WorldSaveData.WorldId, ValidationReason))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveWorldRejected] WorldId=%s Reason=%s"), *WorldSaveData.WorldId, *ValidationReason);
		return false;
	}

	const double SaveStartSeconds = FPlatformTime::Seconds();
	const bool bSaved = Provider->SaveWorld(WorldSaveData.WorldId, WorldSaveData);
	const double SaveLatencyMs = (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0;
	if (bSaved)
	{
		bWorldDirty = false;
		UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][SaveWorldOk] WorldId=%s Actors=%d LatencyMs=%.2f"), *WorldSaveData.WorldId, WorldSaveData.BuildingActors.Num(), SaveLatencyMs);
	}
	else
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveWorldFailed] WorldId=%s Actors=%d LatencyMs=%.2f"), *WorldSaveData.WorldId, WorldSaveData.BuildingActors.Num(), SaveLatencyMs);
	}

	return bSaved;
}

bool USpellRisePersistenceSubsystem::LoadWorld(UWorld* World)
{
	if (!Provider || !World)
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][LoadWorldRejected] World=%s Reason=%s"),
			*GetNameSafe(World),
			!Provider ? TEXT("missing_provider") : TEXT("missing_world"));
		return false;
	}

	if (!IsServerWorld(World))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][LoadWorldRejected] World=%s Reason=non_server_world"), *GetNameSafe(World));
		return false;
	}

	const FString WorldId = ResolveWorldId(World);
	FSpellRiseWorldSaveData LoadedWorldData;
	const double LoadStartSeconds = FPlatformTime::Seconds();
	if (!Provider->LoadWorld(WorldId, LoadedWorldData))
	{
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][LoadWorldMiss] WorldId=%s"), *WorldId);
		return false;
	}

	FString ValidationReason;
	if (!ValidateWorldSaveData(LoadedWorldData, WorldId, ValidationReason))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][LoadWorldRejected] WorldId=%s Reason=%s"), *WorldId, *ValidationReason);
		return false;
	}

	if (LoadedWorldData.SchemaVersion == LegacyPersistenceWorldSchemaVersion)
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][LoadWorldLegacySchema] WorldId=%s Schema=%d TargetSchema=%d"),
			*WorldId,
			LoadedWorldData.SchemaVersion,
			PersistenceWorldSchemaVersion);
	}

	FSpellRiseBuildPersistenceAdapter::SpawnMissingBuildingActors(World, LoadedWorldData.BuildingActors, SpawnActorLocationMatchToleranceSq);

	bWorldDirty = false;
	UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][LoadWorldOk] WorldId=%s Actors=%d LatencyMs=%.2f"), *WorldId, LoadedWorldData.BuildingActors.Num(), (FPlatformTime::Seconds() - LoadStartSeconds) * 1000.0);
	return true;
}

bool USpellRisePersistenceSubsystem::BuildRespawnTransformForController(AController* Controller, FTransform& OutSpawnTransform) const
{
	if (!Controller || !Controller->PlayerState)
	{
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][RespawnTransformMiss] Controller=%s Reason=%s"),
			*GetNameSafe(Controller),
			!Controller ? TEXT("missing_controller") : TEXT("missing_playerstate"));
		return false;
	}

	const FString SteamId64 = ResolveSteamIdFromController(Controller);
	if (!IsValidPersistentId(SteamId64))
	{
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][RespawnTransformMiss] Controller=%s Reason=missing_persistent_id"), *GetNameSafe(Controller));
		return false;
	}

	const FSpellRiseCharacterSaveData* Data = CachedCharacterDataBySteamId.Find(SteamId64);
	if (!Data)
	{
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][RespawnTransformMiss] PersistentId=%s Controller=%s Reason=no_cached_data"), *SteamId64, *GetNameSafe(Controller));
		return false;
	}

	FString ValidationReason;
	if (!ValidateCharacterSaveData(*Data, SteamId64, ValidationReason))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][RespawnTransformRejected] PersistentId=%s Controller=%s Reason=%s"), *SteamId64, *GetNameSafe(Controller), *ValidationReason);
		return false;
	}

	if (AActor* BedActor = ResolveSavedBedActor(Controller->GetWorld(), *Data))
	{
		OutSpawnTransform = BedActor->GetActorTransform();
		OutSpawnTransform.AddToTranslation(FVector(0.0f, 0.0f, 120.0f));
		FString TransformValidationReason;
		if (!ValidateWorldSpawnTransform(Controller->GetWorld(), OutSpawnTransform, TransformValidationReason))
		{
			UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][RespawnTransformRejected] PersistentId=%s Controller=%s BedActor=%s Reason=%s Location=%s"),
				*SteamId64,
				*GetNameSafe(Controller),
				*GetNameSafe(BedActor),
				*TransformValidationReason,
				*OutSpawnTransform.GetLocation().ToCompactString());
			return false;
		}

		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][RespawnTransformOk] PersistentId=%s Controller=%s BedActor=%s Location=%s"),
			*SteamId64,
			*GetNameSafe(Controller),
			*GetNameSafe(BedActor),
			*OutSpawnTransform.GetLocation().ToCompactString());
		return true;
	}

	UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][RespawnTransformMiss] PersistentId=%s Controller=%s Reason=no_bed_actor"), *SteamId64, *GetNameSafe(Controller));
	return false;
}

void USpellRisePersistenceSubsystem::MarkWorldDirty()
{
	bWorldDirty = true;
}

void USpellRisePersistenceSubsystem::MarkPlayerDirtyBySteamId(const FString& SteamId64)
{
	if (IsValidPersistentId(SteamId64))
	{
		DirtyCharacterIds.Add(SteamId64);
		return;
	}

	UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][MarkDirtyRejected] PersistentId=%s Reason=invalid_persistent_id"), *SteamId64);
}

void USpellRisePersistenceSubsystem::SetControllerPersistentId(const AController* Controller, const FString& PersistentId)
{
	if (!Controller || !IsValidPersistentId(PersistentId))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][ControllerPersistentIdRejected] Controller=%s Reason=%s"),
			*GetNameSafe(Controller),
			!Controller ? TEXT("missing_controller") : TEXT("invalid_persistent_id"));
		return;
	}

	PersistentIdByController.Add(Controller, PersistentId);
}

void USpellRisePersistenceSubsystem::ClearControllerPersistentId(const AController* Controller)
{
	if (!Controller)
	{
		return;
	}

	PersistentIdByController.Remove(Controller);
}

bool USpellRisePersistenceSubsystem::GetSteamIdFromPlayerState(const APlayerState* PlayerState, FString& OutSteamId64) const
{
	OutSteamId64.Reset();

	if (!PlayerState)
	{
		return false;
	}

	const FUniqueNetIdRepl UniqueIdRepl = PlayerState->GetUniqueId();
	if (!UniqueIdRepl.IsValid())
	{
		return false;
	}

	const TSharedPtr<const FUniqueNetId> UniqueId = UniqueIdRepl.GetUniqueNetId();
	if (!UniqueId.IsValid())
	{
		return false;
	}

	OutSteamId64 = UniqueId->ToString();
	return IsValidPersistentId(OutSteamId64);
}

FString USpellRisePersistenceSubsystem::ResolveSteamIdFromController(const AController* Controller) const
{
	if (!Controller)
	{
		return FString();
	}

	if (const FString* RegisteredId = PersistentIdByController.Find(TWeakObjectPtr<const AController>(Controller)))
	{
		if (IsValidPersistentId(*RegisteredId))
		{
			return *RegisteredId;
		}
	}

	if (!Controller->PlayerState)
	{
		return FString();
	}

	FString SteamId64;
	if (GetSteamIdFromPlayerState(Controller->PlayerState, SteamId64))
	{
		return SteamId64;
	}

	return FString();
}

FString USpellRisePersistenceSubsystem::ResolveWorldId(const UWorld* World) const
{
	if (!World)
	{
		return TEXT("UnknownWorld");
	}

	FString ServerInstanceId;
	if (!FParse::Value(FCommandLine::Get(), TEXT("ServerInstanceId="), ServerInstanceId))
	{
		if (World->URL.Port > 0)
		{
			ServerInstanceId = FString::Printf(TEXT("Port%d"), World->URL.Port);
		}
		else
		{
			ServerInstanceId = TEXT("Local");
		}
	}

	const FString MapId = UWorld::RemovePIEPrefix(World->GetMapName());
	return FString::Printf(TEXT("%s@%s"), *MapId, *ServerInstanceId);
}

	void GatherInventoryBindings(AActor* Owner, ESpellRiseSaveOwnerScope OwnerScope, TArray<FInventoryComponentBinding>& OutBindings)
	{
		if (!Owner)
		{
			return;
		}

		TArray<UActorComponent*> Components;
		Owner->GetComponents(Components);

		for (UActorComponent* Component : Components)
		{
			if (!Component)
			{
				continue;
			}

			TArray<FArrayProperty*> SlotArrays;
			GatherSerializableSlotArrayProperties(Component, SlotArrays);
			for (FArrayProperty* SlotArrayProperty : SlotArrays)
			{
				FStructProperty* SlotStructProperty = SlotArrayProperty ? CastField<FStructProperty>(SlotArrayProperty->Inner) : nullptr;
				if (!SlotArrayProperty || !SlotStructProperty || !SlotStructProperty->Struct)
				{
					continue;
				}

				FInventoryComponentBinding Binding;
				Binding.Component = Component;
				Binding.SlotArrayProperty = SlotArrayProperty;
				Binding.SlotStructProperty = SlotStructProperty;
				Binding.OwnerScope = OwnerScope;
				Binding.ContainerRole = ClassifyContainerRole(Component, SlotArrayProperty);
				OutBindings.Add(MoveTemp(Binding));
			}
		}
	}

	bool ParseOwnerScope(uint8 RawValue, ESpellRiseSaveOwnerScope& OutScope)
	{
		switch (static_cast<ESpellRiseSaveOwnerScope>(RawValue))
		{
		case ESpellRiseSaveOwnerScope::Character:
		case ESpellRiseSaveOwnerScope::PlayerState:
			OutScope = static_cast<ESpellRiseSaveOwnerScope>(RawValue);
			return true;
		default:
			OutScope = ESpellRiseSaveOwnerScope::Unknown;
			return false;
		}
	}

	bool ParseContainerRole(uint8 RawValue, ESpellRiseSaveContainerRole& OutRole)
	{
		switch (static_cast<ESpellRiseSaveContainerRole>(RawValue))
		{
		case ESpellRiseSaveContainerRole::Inventory:
		case ESpellRiseSaveContainerRole::Hotbar:
		case ESpellRiseSaveContainerRole::Equipment:
		case ESpellRiseSaveContainerRole::Storage:
			OutRole = static_cast<ESpellRiseSaveContainerRole>(RawValue);
			return true;
		default:
			OutRole = ESpellRiseSaveContainerRole::Unknown;
			return false;
		}
	}

	FString ReadSlotItemId(UScriptStruct* SlotStruct, const void* SlotMemory)
	{
		if (!SlotStruct || !SlotMemory)
		{
			return FString();
		}

		if (const FNameProperty* NameProperty = FindFProperty<FNameProperty>(SlotStruct, TEXT("ItemID")))
		{
			return NameProperty->GetPropertyValue_InContainer(SlotMemory).ToString();
		}
		if (const FStrProperty* StrProperty = FindFProperty<FStrProperty>(SlotStruct, TEXT("ItemID")))
		{
			return StrProperty->GetPropertyValue_InContainer(SlotMemory);
		}
		if (const FTextProperty* TextProperty = FindFProperty<FTextProperty>(SlotStruct, TEXT("ItemID")))
		{
			return TextProperty->GetPropertyValue_InContainer(SlotMemory).ToString();
		}

		for (TFieldIterator<FProperty> It(SlotStruct, EFieldIterationFlags::IncludeSuper); It; ++It)
		{
			const FProperty* Property = *It;
			if (!Property || !Property->GetName().Contains(TEXT("ItemID"), ESearchCase::IgnoreCase))
			{
				continue;
			}

			if (const FNameProperty* P = CastField<FNameProperty>(Property))
			{
				return P->GetPropertyValue_InContainer(SlotMemory).ToString();
			}
			if (const FStrProperty* P = CastField<FStrProperty>(Property))
			{
				return P->GetPropertyValue_InContainer(SlotMemory);
			}
			if (const FTextProperty* P = CastField<FTextProperty>(Property))
			{
				return P->GetPropertyValue_InContainer(SlotMemory).ToString();
			}
		}

		return FString();
	}

	bool TryReadWorldItemIdFromObject(const UObject* Object, FString& OutItemId)
	{
		OutItemId.Reset();
		if (!Object)
		{
			return false;
		}

		for (TFieldIterator<FProperty> It(Object->GetClass(), EFieldIterationFlags::IncludeSuper); It; ++It)
		{
			const FProperty* Property = *It;
			if (!Property || !Property->GetName().Contains(TEXT("ItemID"), ESearchCase::IgnoreCase))
			{
				continue;
			}

			if (const FNameProperty* P = CastField<FNameProperty>(Property))
			{
				OutItemId = P->GetPropertyValue_InContainer(Object).ToString();
				return !OutItemId.IsEmpty();
			}
			if (const FStrProperty* P = CastField<FStrProperty>(Property))
			{
				OutItemId = P->GetPropertyValue_InContainer(Object);
				return !OutItemId.IsEmpty();
			}
			if (const FTextProperty* P = CastField<FTextProperty>(Property))
			{
				OutItemId = P->GetPropertyValue_InContainer(Object).ToString();
				return !OutItemId.IsEmpty();
			}
		}

		return false;
	}

namespace
{
	struct FNarrativeInventoryBinding
	{
		TObjectPtr<UNarrativeInventoryComponent> Inventory = nullptr;
		ESpellRiseSaveOwnerScope OwnerScope = ESpellRiseSaveOwnerScope::Unknown;
		ESpellRiseSaveContainerRole ContainerRole = ESpellRiseSaveContainerRole::Unknown;
	};

	ESpellRiseSaveContainerRole ResolveNarrativeContainerRole(const UNarrativeInventoryComponent* InventoryComponent)
	{
		if (!InventoryComponent)
		{
			return ESpellRiseSaveContainerRole::Unknown;
		}

		const FString NameLower = InventoryComponent->GetFName().ToString().ToLower();
		if (NameLower.Contains(TEXT("hotbar")) || NameLower.Contains(TEXT("quickbar")) || NameLower.Contains(TEXT("actionbar")))
		{
			return ESpellRiseSaveContainerRole::Hotbar;
		}
		if (NameLower.Contains(TEXT("equip")) || NameLower.Contains(TEXT("weapon")))
		{
			return ESpellRiseSaveContainerRole::Equipment;
		}
		if (NameLower.Contains(TEXT("storage")))
		{
			return ESpellRiseSaveContainerRole::Storage;
		}
		return ESpellRiseSaveContainerRole::Inventory;
	}

	void GatherNarrativeInventoryBindings(AActor* Owner, const ESpellRiseSaveOwnerScope OwnerScope, TArray<FNarrativeInventoryBinding>& OutBindings)
	{
		if (!Owner)
		{
			return;
		}

		TArray<UNarrativeInventoryComponent*> Components;
		Owner->GetComponents<UNarrativeInventoryComponent>(Components);
		if (UNarrativeInventoryComponent* Primary = UInventoryFunctionLibrary::GetInventoryComponentFromTarget(Owner))
		{
			Components.AddUnique(Primary);
		}

		for (UNarrativeInventoryComponent* Inventory : Components)
		{
			if (!Inventory)
			{
				continue;
			}

			FNarrativeInventoryBinding Binding;
			Binding.Inventory = Inventory;
			Binding.OwnerScope = OwnerScope;
			Binding.ContainerRole = ResolveNarrativeContainerRole(Inventory);
			OutBindings.Add(MoveTemp(Binding));
		}
	}
}

bool USpellRisePersistenceSubsystem::CollectCharacterData(AController* Controller, const FString& SteamId64, FSpellRiseCharacterSaveData& OutData) const
{
	if (!Controller || !Controller->PlayerState)
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveCollectRejected] PersistentId=%s Controller=%s Reason=%s"),
			*SteamId64,
			*GetNameSafe(Controller),
			!Controller ? TEXT("missing_controller_or_playerstate") : TEXT("missing_playerstate"));
		return false;
	}

	ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(Controller->GetPawn());
	ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState);
	if (!Character || !SRPlayerState)
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveCollectRejected] PersistentId=%s Controller=%s Reason=missing_character_or_playerstate"), *SteamId64, *GetNameSafe(Controller));
		return false;
	}

	USpellRiseAbilitySystemComponent* ASC = SRPlayerState->GetSpellRiseASC();
	if (!ASC)
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveCollectRejected] PersistentId=%s Controller=%s Reason=missing_asc"), *SteamId64, *GetNameSafe(Controller));
		return false;
	}

	OutData = FSpellRiseCharacterSaveData();
	OutData.SchemaVersion = PersistenceCharacterSchemaVersion;
	OutData.SteamId64 = SteamId64;
	OutData.PlayerDisplayName = Controller->PlayerState->GetPlayerName();
	OutData.CharacterTransform = Character->GetActorTransform();
	OutData.ArchetypeValue = static_cast<uint8>(Character->Archetype);

	if (!IsFiniteTransform(OutData.CharacterTransform))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveCollectRejected] PersistentId=%s Controller=%s Reason=invalid_character_transform"), *SteamId64, *GetNameSafe(Controller));
		return false;
	}

	if (!OutData.PlayerDisplayName.IsEmpty())
	{
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][SaveName] PersistentId=%s Name=%s"), *SteamId64, *OutData.PlayerDisplayName);
	}

	OutData.Strength = ASC->GetNumericAttribute(UCombatAttributeSet::GetStrengthAttribute());
	OutData.Agility = ASC->GetNumericAttribute(UCombatAttributeSet::GetAgilityAttribute());
	OutData.Intelligence = ASC->GetNumericAttribute(UCombatAttributeSet::GetIntelligenceAttribute());
	OutData.Wisdom = ASC->GetNumericAttribute(UCombatAttributeSet::GetWisdomAttribute());

	OutData.Health = ASC->GetNumericAttribute(UResourceAttributeSet::GetHealthAttribute());
	OutData.Mana = ASC->GetNumericAttribute(UResourceAttributeSet::GetManaAttribute());
	OutData.Stamina = ASC->GetNumericAttribute(UResourceAttributeSet::GetStaminaAttribute());

	OutData.CatalystCharge = ASC->GetNumericAttribute(UCatalystAttributeSet::GetCatalystChargeAttribute());
	OutData.CatalystXP = ASC->GetNumericAttribute(UCatalystAttributeSet::GetCatalystXPAttribute());
	OutData.CatalystLevel = ASC->GetNumericAttribute(UCatalystAttributeSet::GetCatalystLevelAttribute());

	if (!SRPlayerState->GetRespawnBedActorName().IsEmpty())
	{
		OutData.RespawnBedActorName = SRPlayerState->GetRespawnBedActorName();
		OutData.RespawnBedClassPath = SRPlayerState->GetRespawnBedClassPath();
		OutData.RespawnBedLocation = SRPlayerState->GetRespawnBedLocation();
	}

	TArray<FNarrativeInventoryBinding> Bindings;
	GatherNarrativeInventoryBindings(Character, ESpellRiseSaveOwnerScope::Character, Bindings);
	GatherNarrativeInventoryBindings(SRPlayerState, ESpellRiseSaveOwnerScope::PlayerState, Bindings);

	for (const FNarrativeInventoryBinding& Binding : Bindings)
	{
		UNarrativeInventoryComponent* Inventory = Binding.Inventory.Get();
		if (!Inventory)
		{
			continue;
		}

		FSpellRiseSavedInventoryComponent SavedComponent;
		SavedComponent.OwnerScope = static_cast<uint8>(Binding.OwnerScope);
		SavedComponent.ContainerRole = static_cast<uint8>(Binding.ContainerRole);
		SavedComponent.ComponentName = Inventory->GetName();
		SavedComponent.ComponentClassPath = Inventory->GetClass()->GetPathName();
		SavedComponent.InventoryFriendlyName = FString();
		SavedComponent.WeightCapacity = Inventory->GetWeightCapacity();
		SavedComponent.Capacity = Inventory->GetCapacity();
		SavedComponent.Currency = Inventory->GetCurrency();

		for (UNarrativeItem* Item : Inventory->GetItems())
		{
			if (!Item || !Item->GetClass())
			{
				continue;
			}

			const int32 Quantity = FMath::Max(0, Item->GetQuantity());
			if (Quantity <= 0)
			{
				continue;
			}

			FSpellRiseSavedNarrativeItem SavedItem;
			SavedItem.ItemClassPath = Item->GetClass()->GetPathName();
			SavedItem.Quantity = Quantity;
			SavedComponent.Items.Add(MoveTemp(SavedItem));
		}

		if (SavedComponent.ComponentName.IsEmpty() || SavedComponent.ComponentClassPath.IsEmpty())
		{
			UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][SaveCollectRejected] PersistentId=%s Controller=%s Reason=invalid_inventory_component"), *SteamId64, *GetNameSafe(Controller));
			continue;
		}

		OutData.InventoryComponents.Add(MoveTemp(SavedComponent));
	}

	return true;
}

bool USpellRisePersistenceSubsystem::ApplyCharacterDataToController(AController* Controller, const FSpellRiseCharacterSaveData& Data)
{
	if (!Controller || !Controller->PlayerState)
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][ApplyRejected] Controller=%s Reason=%s"),
			*GetNameSafe(Controller),
			!Controller ? TEXT("missing_controller") : TEXT("missing_playerstate"));
		return false;
	}

	const FString SteamId64 = ResolveSteamIdFromController(Controller);
	FString ValidationReason;
	if (!ValidateCharacterSaveData(Data, SteamId64, ValidationReason))
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][ApplyRejected] PersistentId=%s Controller=%s Reason=%s"),
			*SteamId64,
			*GetNameSafe(Controller),
			*ValidationReason);
		return false;
	}

	ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(Controller->GetPawn());
	ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState);
	if (!Character || !SRPlayerState)
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][ApplyRejected] PersistentId=%s Controller=%s Reason=missing_character_or_playerstate"), *SteamId64, *GetNameSafe(Controller));
		return false;
	}

	USpellRiseAbilitySystemComponent* ASC = SRPlayerState->GetSpellRiseASC();
	if (!ASC)
	{
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][ApplyRejected] PersistentId=%s Controller=%s Reason=missing_asc"), *SteamId64, *GetNameSafe(Controller));
		return false;
	}

	if (!Data.PlayerDisplayName.IsEmpty())
	{
		Controller->PlayerState->SetPlayerName(Data.PlayerDisplayName);
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][LoadName] PersistentId=%s Name=%s"), *SteamId64, *Data.PlayerDisplayName);
	}

	Character->Archetype = static_cast<ESpellRiseArchetype>(Data.ArchetypeValue);

	ASC->SetNumericAttributeBase(UCombatAttributeSet::GetStrengthAttribute(), Data.Strength);
	ASC->SetNumericAttributeBase(UCombatAttributeSet::GetAgilityAttribute(), Data.Agility);
	ASC->SetNumericAttributeBase(UCombatAttributeSet::GetIntelligenceAttribute(), Data.Intelligence);
	ASC->SetNumericAttributeBase(UCombatAttributeSet::GetWisdomAttribute(), Data.Wisdom);

	ASC->SetNumericAttributeBase(UResourceAttributeSet::GetHealthAttribute(), Data.Health);
	ASC->SetNumericAttributeBase(UResourceAttributeSet::GetManaAttribute(), Data.Mana);
	ASC->SetNumericAttributeBase(UResourceAttributeSet::GetStaminaAttribute(), Data.Stamina);

	ASC->SetNumericAttributeBase(UCatalystAttributeSet::GetCatalystChargeAttribute(), Data.CatalystCharge);
	ASC->SetNumericAttributeBase(UCatalystAttributeSet::GetCatalystXPAttribute(), Data.CatalystXP);
	ASC->SetNumericAttributeBase(UCatalystAttributeSet::GetCatalystLevelAttribute(), Data.CatalystLevel);

	if (!Data.RespawnBedActorName.IsEmpty() || !Data.RespawnBedClassPath.IsEmpty())
	{
		SRPlayerState->SetRespawnBedReferenceData(Data.RespawnBedActorName, Data.RespawnBedClassPath, Data.RespawnBedLocation);
	}

	if (!Data.CharacterTransform.GetLocation().ContainsNaN())
	{
		FString TransformValidationReason;
		if (ValidateWorldSpawnTransform(Controller->GetWorld(), Data.CharacterTransform, TransformValidationReason))
		{
			Character->SetActorTransform(Data.CharacterTransform, false, nullptr, ETeleportType::TeleportPhysics);
			UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][ApplyCharacterTransform] PersistentId=%s Controller=%s Location=%s"),
				*SteamId64,
				*GetNameSafe(Controller),
				*Data.CharacterTransform.GetLocation().ToCompactString());
		}
		else
		{
			UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][ApplyCharacterTransformSkipped] PersistentId=%s Controller=%s Reason=%s Location=%s"),
				*SteamId64,
				*GetNameSafe(Controller),
				*TransformValidationReason,
				*Data.CharacterTransform.GetLocation().ToCompactString());
		}
	}

	DestroyOwnedEquipmentVisualActors(Character);
	TArray<FNarrativeInventoryBinding> AvailableBindings;
	GatherNarrativeInventoryBindings(Character, ESpellRiseSaveOwnerScope::Character, AvailableBindings);
	GatherNarrativeInventoryBindings(SRPlayerState, ESpellRiseSaveOwnerScope::PlayerState, AvailableBindings);

	for (const FNarrativeInventoryBinding& Binding : AvailableBindings)
	{
		if (UNarrativeInventoryComponent* Inventory = Binding.Inventory.Get())
		{
			Inventory->SetLootSource(nullptr);
			const TArray<UNarrativeItem*> ExistingItems = Inventory->GetItems();
			for (UNarrativeItem* ExistingItem : ExistingItems)
			{
				if (ExistingItem && ExistingItem->CanBeRemoved())
				{
					Inventory->RemoveItem(ExistingItem);
				}
			}
		}
	}

	int32 AppliedComponents = 0;
	int32 AppliedItemStacks = 0;
	int32 AppliedItemQuantity = 0;
	int32 SkippedLegacyComponents = 0;
	int32 SkippedInvalidComponents = 0;
	int32 SkippedInvalidStacks = 0;
	int32 SkippedUnmatchedComponents = 0;

	if (Data.SchemaVersion < PersistenceCharacterSchemaVersion)
	{
		SkippedLegacyComponents = Data.InventoryComponents.Num();
		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][InventoryResetLegacy] Controller=%s SchemaVersion=%d Components=%d"),
			*GetNameSafe(Controller),
			Data.SchemaVersion,
			Data.InventoryComponents.Num());
	}
	else
	{
		for (const FSpellRiseSavedInventoryComponent& SavedComponent : Data.InventoryComponents)
		{
			if (SavedComponent.ComponentName.IsEmpty() || SavedComponent.ComponentClassPath.IsEmpty())
			{
				++SkippedInvalidComponents;
				continue;
			}

			FNarrativeInventoryBinding* TargetBinding = nullptr;
			for (FNarrativeInventoryBinding& Binding : AvailableBindings)
			{
				UNarrativeInventoryComponent* Inventory = Binding.Inventory.Get();
				if (!Inventory)
				{
					continue;
				}

				if (Binding.OwnerScope == static_cast<ESpellRiseSaveOwnerScope>(SavedComponent.OwnerScope) &&
					Inventory->GetName().Equals(SavedComponent.ComponentName, ESearchCase::IgnoreCase))
				{
					TargetBinding = &Binding;
					break;
				}
			}

			if (!TargetBinding || !TargetBinding->Inventory.Get())
			{
				++SkippedUnmatchedComponents;
				++SkippedLegacyComponents;
				continue;
			}

			UNarrativeInventoryComponent* Inventory = TargetBinding->Inventory.Get();
			Inventory->SetCapacity(FMath::Max(1, SavedComponent.Capacity));
			Inventory->SetWeightCapacity(FMath::Max(0.0f, SavedComponent.WeightCapacity));
			Inventory->SetCurrency(FMath::Max(0, SavedComponent.Currency));
			if (!SavedComponent.InventoryFriendlyName.IsEmpty())
			{
				Inventory->SetInventoryFriendlyName(FText::FromString(SavedComponent.InventoryFriendlyName));
			}

			for (const FSpellRiseSavedNarrativeItem& SavedItem : SavedComponent.Items)
			{
				if (SavedItem.ItemClassPath.IsEmpty() || SavedItem.Quantity <= 0)
				{
					++SkippedInvalidStacks;
					continue;
				}

				UClass* LoadedClass = LoadObject<UClass>(nullptr, *SavedItem.ItemClassPath);
				TSubclassOf<UNarrativeItem> ItemClass = LoadedClass;
				if (!ItemClass)
				{
					++SkippedInvalidStacks;
					continue;
				}

				const FItemAddResult AddResult = Inventory->TryAddItemFromClass(ItemClass, SavedItem.Quantity, false);
				if (AddResult.AmountGiven > 0)
				{
					++AppliedItemStacks;
					AppliedItemQuantity += AddResult.AmountGiven;
				}
				else
				{
					++SkippedInvalidStacks;
				}
			}

			++AppliedComponents;
		}
	}

	UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][InventoryApplySummary] Controller=%s AppliedComponents=%d AppliedStacks=%d AppliedQuantity=%d SkippedLegacy=%d SkippedInvalidComponents=%d SkippedInvalidStacks=%d SkippedUnmatchedComponents=%d"),
		*GetNameSafe(Controller),
		AppliedComponents,
		AppliedItemStacks,
		AppliedItemQuantity,
		SkippedLegacyComponents,
		SkippedInvalidComponents,
		SkippedInvalidStacks,
		SkippedUnmatchedComponents);

	const bool bShouldEnsureDefaults =
		(Data.InventoryComponents.Num() == 0) ||
		(AppliedItemQuantity <= 0 && (SkippedLegacyComponents > 0 || SkippedInvalidStacks > 0 || SkippedUnmatchedComponents > 0));
	if (bShouldEnsureDefaults)
	{
		EnsureDefaultItemsForControllerIfNeeded(Controller, TEXT("apply_profile_fallback"));
	}

	ReconcileCharacterVisualEquipment(Character, SRPlayerState);

	return true;
}

void USpellRisePersistenceSubsystem::EnsureDefaultItemsForControllerIfNeeded(AController* Controller, const TCHAR* ContextTag)
{
	if (!Controller || !Controller->HasAuthority())
	{
		return;
	}

	ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(Controller->GetPawn());
	ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState);
	if (!Character || !SRPlayerState)
	{
		UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][DefaultItemsSkipped] Controller=%s Context=%s Reason=missing_character_or_playerstate"),
			*GetNameSafe(Controller),
			ContextTag ? ContextTag : TEXT("unknown"));
		return;
	}

	TArray<FNarrativeInventoryBinding> Bindings;
	GatherNarrativeInventoryBindings(Character, ESpellRiseSaveOwnerScope::Character, Bindings);
	GatherNarrativeInventoryBindings(SRPlayerState, ESpellRiseSaveOwnerScope::PlayerState, Bindings);

	for (const FNarrativeInventoryBinding& Binding : Bindings)
	{
		UNarrativeInventoryComponent* Inventory = Binding.Inventory.Get();
		if (!Inventory)
		{
			continue;
		}

		int32 ExistingStackCount = 0;
		for (UNarrativeItem* ExistingItem : Inventory->GetItems())
		{
			if (ExistingItem && ExistingItem->GetQuantity() > 0)
			{
				++ExistingStackCount;
			}
		}

		if (ExistingStackCount > 0)
		{
			UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][DefaultItemsSkipComponent] Controller=%s Context=%s Component=%s Reason=already_has_items Stacks=%d"),
				*GetNameSafe(Controller),
				ContextTag ? ContextTag : TEXT("unknown"),
				*Inventory->GetName(),
				ExistingStackCount);
			continue;
		}

		UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][DefaultItemsAttempt] Controller=%s Context=%s Component=%s"),
			*GetNameSafe(Controller),
			ContextTag ? ContextTag : TEXT("unknown"),
			*Inventory->GetName());

		Inventory->GiveDefaultItems();

		int32 ResultingStackCount = 0;
		for (UNarrativeItem* Item : Inventory->GetItems())
		{
			if (Item && Item->GetQuantity() > 0)
			{
				++ResultingStackCount;
			}
		}

		UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][DefaultItemsResult] Controller=%s Context=%s Component=%s Stacks=%d"),
			*GetNameSafe(Controller),
			ContextTag ? ContextTag : TEXT("unknown"),
			*Inventory->GetName(),
			ResultingStackCount);
	}
}

void USpellRisePersistenceSubsystem::BuildInventorySnapshotFromCharacterData(const FSpellRiseCharacterSaveData& CharacterData, const FString& SteamId64, FSpellRiseInventorySaveData& OutInventoryData) const
{
	OutInventoryData.SchemaVersion = PersistenceInventorySchemaVersion;
	OutInventoryData.SteamId64 = SteamId64;
	OutInventoryData.InventoryComponents = CharacterData.InventoryComponents;
}

void USpellRisePersistenceSubsystem::MergeInventorySnapshotIntoCharacterData(const FSpellRiseInventorySaveData& InventoryData, FSpellRiseCharacterSaveData& InOutCharacterData) const
{
	if (InventoryData.SchemaVersion >= PersistenceInventorySchemaVersion)
	{
		InOutCharacterData.InventoryComponents = InventoryData.InventoryComponents;
	}
	else
	{
		InOutCharacterData.InventoryComponents.Reset();
	}
}

void USpellRisePersistenceSubsystem::ReconcileCharacterVisualEquipment(ASpellRiseCharacterBase* Character, ASpellRisePlayerState* PlayerState) const
{
	if (!Character)
	{
		return;
	}

	TArray<FNarrativeInventoryBinding> CandidateBindings;
	GatherNarrativeInventoryBindings(Character, ESpellRiseSaveOwnerScope::Character, CandidateBindings);
	GatherNarrativeInventoryBindings(PlayerState, ESpellRiseSaveOwnerScope::PlayerState, CandidateBindings);

	for (const FNarrativeInventoryBinding& Binding : CandidateBindings)
	{
		UActorComponent* Component = Binding.Inventory.Get();
		if (!Component)
		{
			continue;
		}

		InvokeNoParamFunctionIfExists(
			Component,
			{
				TEXT("ClientRefreshInventory"),
				TEXT("RefreshInventory"),
				TEXT("RefreshInventorySERVER"),
				TEXT("RefreshHotbar"),
				TEXT("RefreshHotbarSERVER"),
				TEXT("RefreshEquipment"),
				TEXT("RefreshEquipmentSERVER"),
				TEXT("RefreshEquippedItems"),
				TEXT("RefreshEquippedItemsSERVER"),
				TEXT("UpdateEquipment"),
				TEXT("UpdateEquipmentSERVER"),
				TEXT("UpdateHotbar"),
				TEXT("UpdateHotbarSERVER"),
				TEXT("SyncEquipmentVisuals"),
				TEXT("SyncEquipmentVisualsSERVER")
			});
	}

	InvokeNoParamFunctionIfExists(
		Character,
		{
			TEXT("RefreshEquipment"),
			TEXT("RefreshEquipmentSERVER"),
			TEXT("RefreshEquippedItems"),
			TEXT("RefreshEquippedItemsSERVER"),
			TEXT("UpdateEquipment"),
			TEXT("UpdateEquipmentSERVER"),
			TEXT("SyncEquipmentVisuals"),
			TEXT("SyncEquipmentVisualsSERVER")
		});

	Character->ForceNetUpdate();
	if (PlayerState)
	{
		PlayerState->ForceNetUpdate();
	}

	if (Character->HasAuthority())
	{
		Character->MultiRefreshEquipmentVisuals();
	}
}

AActor* USpellRisePersistenceSubsystem::ResolveSavedBedActor(UWorld* World, const FSpellRiseCharacterSaveData& Data) const
{
	if (!World)
	{
		return nullptr;
	}

	UClass* SavedBedClass = nullptr;
	if (!Data.RespawnBedClassPath.IsEmpty())
	{
		SavedBedClass = LoadObject<UClass>(nullptr, *Data.RespawnBedClassPath);
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}

		const FString ClassPath = Actor->GetClass()->GetPathName();
		const bool bClassLooksLikeBed = SavedBedClass ? Actor->IsA(SavedBedClass) : PersistenceIsLikelyBedClassPath(ClassPath);
		if (!bClassLooksLikeBed)
		{
			continue;
		}

		if (!Data.RespawnBedActorName.IsEmpty() && Actor->GetName().Equals(Data.RespawnBedActorName, ESearchCase::IgnoreCase))
		{
			return Actor;
		}

		if (!Data.RespawnBedLocation.IsNearlyZero() &&
			FVector::DistSquared(Actor->GetActorLocation(), Data.RespawnBedLocation) <= RespawnBedLocationToleranceSq)
		{
			return Actor;
		}
	}

	return nullptr;
}
