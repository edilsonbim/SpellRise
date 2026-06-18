// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Persistence/SpellRisePersistenceSubsystem.h"

#include "EngineUtils.h"
#include "JsonObjectConverter.h"
#include "Components/ActorComponent.h"
#include "Components/ChildActorComponent.h"
#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
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
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Equipment/SpellRiseEquipmentManagerComponent.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityHotbarComponent.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/Persistence/SpellRiseBuildPersistenceAdapter.h"
#include "SpellRise/Persistence/SpellRiseFilePersistenceProvider.h"
#include "SpellRise/Persistence/SpellRisePostgresPersistenceProvider.h"
#include "SpellRise/Progression/SpellRiseProgressionComponent.h"
#include "EquippableItem.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRisePersistence, Log, All);

namespace
{
	constexpr int32 PersistenceCharacterSchemaVersion = 11;
	constexpr int32 PersistenceCharacterInventorySchemaVersion = 8;
	constexpr int32 PersistentCharacterLevelMin = 1;
	constexpr int32 PersistentCharacterLevelMax = 999;
	constexpr int32 PersistentExperienceMax = 2000000000;
	constexpr int32 PersistentProgressionCurrencyMax = 1000000;
	constexpr int32 PersistenceInventorySchemaVersion = 2;
	constexpr int32 PersistenceWorldSchemaVersion = 1;
	constexpr int32 LegacyPersistenceWorldSchemaVersion = 5;
	constexpr int32 MaxVisualConfigurationJsonLength = 64 * 1024;
	constexpr float PersistentPrimaryAttributeMin = 0.0f;
	constexpr float PersistentPrimaryAttributeMax = 140.0f;
	constexpr int32 PersistentTalentLevelMin = 1;
	constexpr int32 PersistentTalentLevelMax = 100;
	constexpr uint8 SlotEncodingLegacyRawBase64 = 0;
	constexpr uint8 SlotEncodingStructJson = 1;
	constexpr double RespawnBedLocationToleranceSq = 300.0 * 300.0;
	constexpr double SpawnActorLocationMatchToleranceSq = 25.0 * 25.0;
	constexpr double WorldEquipConflictRadius = 350.0;

	FString GetRequestedPersistenceProvider(bool& bOutWasExplicit)
	{
		bOutWasExplicit = false;
		FString CmdProvider;
		if (FParse::Value(FCommandLine::Get(), TEXT("SRPersistenceProvider="), CmdProvider))
		{
			CmdProvider.TrimStartAndEndInline();
			CmdProvider.ToLowerInline();
			if (!CmdProvider.IsEmpty())
			{
				bOutWasExplicit = true;
				return CmdProvider;
			}
		}
		return TEXT("postgres");
	}

	bool IsPostgresRequiredWithoutFallback()
	{
		return FParse::Param(FCommandLine::Get(), TEXT("SRPersistenceRequirePostgres"));
	}

	bool IsNoSteamPersistenceModeActive()
	{
		return FParse::Param(FCommandLine::Get(), TEXT("nosteam"));
	}

	bool IsPIEWorld(const UWorld* World)
	{
#if WITH_EDITOR
		return World && World->WorldType == EWorldType::PIE;
#else
		return false;
#endif
	}

	bool IsDevFallbackPersistenceReason(const TCHAR* Reason)
	{
		return Reason && FCString::Stricmp(Reason, TEXT("invalid_or_non_steam_persistent_id")) == 0;
	}

	bool IsFilePersistenceFallbackAllowed()
	{
		return FParse::Param(FCommandLine::Get(), TEXT("SRPersistenceAllowFileFallback"));
	}

	struct FPersistenceInventoryStats
	{
		int32 ComponentCount = 0;
		int32 ItemStackCount = 0;
		int32 ItemQuantity = 0;
	};

	bool IsValidOptionalJsonObjectString(const FString& JsonText)
	{
		if (JsonText.IsEmpty())
		{
			return true;
		}

		if (JsonText.Len() > MaxVisualConfigurationJsonLength)
		{
			return false;
		}

		TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
		return FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid();
	}

	UObject* ResolveGameInstanceObject(const AController* Controller)
	{
		const UWorld* World = Controller ? Controller->GetWorld() : nullptr;
		return World ? World->GetGameInstance() : nullptr;
	}

	bool ReadBoolPropertyByName(const UObject* Object, const FName PropertyName, bool& OutValue)
	{
		if (!Object || PropertyName.IsNone())
		{
			return false;
		}

		if (const FBoolProperty* BoolProperty = FindFProperty<FBoolProperty>(Object->GetClass(), PropertyName))
		{
			OutValue = BoolProperty->GetPropertyValue_InContainer(Object);
			return true;
		}

		return false;
	}

	void WriteBoolPropertyByName(UObject* Object, const FName PropertyName, const bool Value)
	{
		if (Object && !PropertyName.IsNone())
		{
			if (FBoolProperty* BoolProperty = FindFProperty<FBoolProperty>(Object->GetClass(), PropertyName))
			{
				BoolProperty->SetPropertyValue_InContainer(Object, Value);
			}
		}
	}

	bool ReadVisualConfigurationJson(const UObject* Object, FString& OutJson)
	{
		OutJson.Reset();
		if (!Object)
		{
			return false;
		}

		const FName PropertyName(TEXT("VisualConfiguration"));
		const FProperty* Property = Object->GetClass()->FindPropertyByName(PropertyName);
		if (!Property)
		{
			return false;
		}

		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);
		if (!ValuePtr)
		{
			return false;
		}

		if (const FStrProperty* StringProperty = CastField<FStrProperty>(Property))
		{
			OutJson = StringProperty->GetPropertyValue(ValuePtr);
			if (!OutJson.IsEmpty() && !IsValidOptionalJsonObjectString(OutJson))
			{
				OutJson = FString::Printf(TEXT("{\"value\":\"%s\"}"), *OutJson.ReplaceCharWithEscapedChar());
			}
			return !OutJson.IsEmpty();
		}

		if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
		{
			const FName NameValue = NameProperty->GetPropertyValue(ValuePtr);
			if (!NameValue.IsNone())
			{
				OutJson = FString::Printf(TEXT("{\"value\":\"%s\"}"), *NameValue.ToString());
				return true;
			}
			return false;
		}

		if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
		{
			const FString TextValue = TextProperty->GetPropertyValue(ValuePtr).ToString();
			if (!TextValue.IsEmpty())
			{
				OutJson = FString::Printf(TEXT("{\"value\":\"%s\"}"), *TextValue.ReplaceCharWithEscapedChar());
				return true;
			}
			return false;
		}

		if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			return FJsonObjectConverter::UStructToJsonObjectString(StructProperty->Struct, ValuePtr, OutJson, 0, 0) && !OutJson.IsEmpty();
		}

		return false;
	}

	void WriteVisualConfigurationJson(UObject* Object, const FString& JsonText)
	{
		if (!Object || JsonText.IsEmpty())
		{
			return;
		}

		const FName PropertyName(TEXT("VisualConfiguration"));
		FProperty* Property = Object->GetClass()->FindPropertyByName(PropertyName);
		if (!Property)
		{
			return;
		}

		void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);
		if (!ValuePtr)
		{
			return;
		}

		if (FStrProperty* StringProperty = CastField<FStrProperty>(Property))
		{
			TSharedPtr<FJsonObject> JsonObject;
			const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
			FString Value;
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid() && JsonObject->TryGetStringField(TEXT("value"), Value))
			{
				StringProperty->SetPropertyValue(ValuePtr, Value);
			}
			else
			{
				StringProperty->SetPropertyValue(ValuePtr, JsonText);
			}
			return;
		}

		if (FNameProperty* NameProperty = CastField<FNameProperty>(Property))
		{
			TSharedPtr<FJsonObject> JsonObject;
			const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				FString Value;
				if (JsonObject->TryGetStringField(TEXT("value"), Value))
				{
					NameProperty->SetPropertyValue(ValuePtr, FName(*Value));
				}
			}
			return;
		}

		if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
		{
			TSharedPtr<FJsonObject> JsonObject;
			const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				FString Value;
				if (JsonObject->TryGetStringField(TEXT("value"), Value))
				{
					TextProperty->SetPropertyValue(ValuePtr, FText::FromString(Value));
				}
			}
			return;
		}

		if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			TSharedPtr<FJsonObject> JsonObject;
			const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructProperty->Struct, ValuePtr, 0, 0);
			}
		}
	}

	struct FInventoryComponentBinding
	{
		TObjectPtr<UActorComponent> Component = nullptr;
		FArrayProperty* SlotArrayProperty = nullptr;
		FStructProperty* SlotStructProperty = nullptr;
		ESpellRiseSaveOwnerScope OwnerScope = ESpellRiseSaveOwnerScope::Unknown;
		ESpellRiseSaveContainerRole ContainerRole = ESpellRiseSaveContainerRole::Unknown;
	};

	bool IsTalentTreeComponent(const UActorComponent* Component)
	{
		if (!Component || !Component->GetClass())
		{
			return false;
		}

		const FString ComponentName = Component->GetName();
		const FString ClassPath = Component->GetClass()->GetPathName();
		return ComponentName.Contains(TEXT("TalentTreeComponent"), ESearchCase::IgnoreCase)
			|| ClassPath.Contains(TEXT("TalentTreeComponent"), ESearchCase::IgnoreCase);
	}

	float ClampPersistentPrimaryAttribute(const float Value)
	{
		if (!FMath::IsFinite(Value))
		{
			return PersistentPrimaryAttributeMin;
		}

		return FMath::Clamp(Value, PersistentPrimaryAttributeMin, PersistentPrimaryAttributeMax);
	}

	int32 ClampPersistentTalentLevel(const int32 Level)
	{
		return FMath::Clamp(Level, PersistentTalentLevelMin, PersistentTalentLevelMax);
	}

	UActorComponent* FindTalentTreeComponent(AActor* Owner)
	{
		if (!Owner)
		{
			return nullptr;
		}

		TArray<UActorComponent*> Components;
		Owner->GetComponents(Components);
		for (UActorComponent* Component : Components)
		{
			if (IsTalentTreeComponent(Component))
			{
				return Component;
			}
		}

		return nullptr;
	}

	UActorComponent* ResolveTalentTreeComponent(AController* Controller)
	{
		if (!Controller)
		{
			return nullptr;
		}

		if (UActorComponent* CharacterComponent = FindTalentTreeComponent(Controller->GetPawn()))
		{
			return CharacterComponent;
		}

		return FindTalentTreeComponent(Controller->PlayerState);
	}

	const FProperty* FindFirstStructPropertyByNames(const UScriptStruct* StructType, const TArray<FName>& PreferredNames)
	{
		if (!StructType)
		{
			return nullptr;
		}

		for (const FName& PreferredName : PreferredNames)
		{
			if (const FProperty* Property = StructType->FindPropertyByName(PreferredName))
			{
				return Property;
			}
		}

		return nullptr;
	}

	int32 ReadIntLevelFromStruct(
		const UScriptStruct* StructType,
		const void* StructMemory,
		const TArray<FName>& PreferredNames,
		const bool bAllowFallbackContainsLevel)
	{
		if (!StructType || !StructMemory)
		{
			return 0;
		}

		const FProperty* LevelProperty = FindFirstStructPropertyByNames(StructType, PreferredNames);

		if (!LevelProperty && bAllowFallbackContainsLevel)
		{
			for (TFieldIterator<FIntProperty> It(StructType); It; ++It)
			{
				if (It->GetName().Contains(TEXT("Level"), ESearchCase::IgnoreCase))
				{
					LevelProperty = *It;
					break;
				}
			}
		}

		const void* ValuePtr = LevelProperty ? LevelProperty->ContainerPtrToValuePtr<void>(StructMemory) : nullptr;
		if (const FIntProperty* IntProperty = CastField<FIntProperty>(LevelProperty))
		{
			return FMath::Max(0, IntProperty->GetPropertyValue(ValuePtr));
		}

		if (const FByteProperty* ByteProperty = CastField<FByteProperty>(LevelProperty))
		{
			return FMath::Max(0, static_cast<int32>(ByteProperty->GetPropertyValue(ValuePtr)));
		}

		return 0;
	}

	int32 ReadTalentLevelFromStruct(const UScriptStruct* StructType, const void* StructMemory)
	{
		return ReadIntLevelFromStruct(StructType, StructMemory, {
			FName(TEXT("Level")),
			FName(TEXT("TalentLevel")),
			FName(TEXT("CurrentLevel")),
			FName(TEXT("GrantedLevel"))
		}, true);
	}

	int32 ReadAbilityLevelFromStruct(const UScriptStruct* StructType, const void* StructMemory)
	{
		return ReadIntLevelFromStruct(StructType, StructMemory, {
			FName(TEXT("AbilityLevel")),
			FName(TEXT("GrantedAbilityLevel"))
		}, false);
	}

	UObject* ReadTalentAssetFromStruct(const UScriptStruct* StructType, const void* StructMemory)
	{
		if (!StructType || !StructMemory)
		{
			return nullptr;
		}

		const FProperty* TalentProperty = FindFirstStructPropertyByNames(StructType, {
			FName(TEXT("Talent")),
			FName(TEXT("TalentData")),
			FName(TEXT("TalentAsset"))
		});

		if (!TalentProperty)
		{
			for (TFieldIterator<FObjectPropertyBase> It(StructType); It; ++It)
			{
				if (It->GetName().Contains(TEXT("Talent"), ESearchCase::IgnoreCase))
				{
					TalentProperty = *It;
					break;
				}
			}
		}

		if (const FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(TalentProperty))
		{
			return ObjectProperty->GetObjectPropertyValue_InContainer(StructMemory);
		}

		return nullptr;
	}

	bool CollectTalentTreeData(UActorComponent* TalentComponent, int32& OutTalentPoints, TArray<FSpellRiseSavedTalent>& OutTalents)
	{
		OutTalentPoints = 0;
		OutTalents.Reset();

		if (!TalentComponent || !TalentComponent->GetClass())
		{
			return false;
		}

		if (const FIntProperty* PointsProperty = FindFProperty<FIntProperty>(TalentComponent->GetClass(), TEXT("TalentPoints")))
		{
			OutTalentPoints = FMath::Max(0, PointsProperty->GetPropertyValue_InContainer(TalentComponent));
		}

		FArrayProperty* GrantedTalentsProperty = FindFProperty<FArrayProperty>(TalentComponent->GetClass(), TEXT("GrantedTalents"));
		const FStructProperty* GrantedTalentStructProperty = GrantedTalentsProperty ? CastField<FStructProperty>(GrantedTalentsProperty->Inner) : nullptr;
		if (!GrantedTalentsProperty || !GrantedTalentStructProperty || !GrantedTalentStructProperty->Struct)
		{
			return true;
		}

		FScriptArrayHelper GrantedTalentsHelper(GrantedTalentsProperty, GrantedTalentsProperty->ContainerPtrToValuePtr<void>(TalentComponent));
		for (int32 Index = 0; Index < GrantedTalentsHelper.Num(); ++Index)
		{
			const void* ElementMemory = GrantedTalentsHelper.GetRawPtr(Index);
			UObject* TalentAsset = ReadTalentAssetFromStruct(GrantedTalentStructProperty->Struct, ElementMemory);
			if (!TalentAsset)
			{
				continue;
			}

			FSpellRiseSavedTalent SavedTalent;
			SavedTalent.TalentAssetPath = TalentAsset->GetPathName();
			SavedTalent.Level = ClampPersistentTalentLevel(ReadTalentLevelFromStruct(GrantedTalentStructProperty->Struct, ElementMemory));
			const int32 ExplicitAbilityLevel = ReadAbilityLevelFromStruct(GrantedTalentStructProperty->Struct, ElementMemory);
			SavedTalent.AbilityLevel = ClampPersistentTalentLevel(ExplicitAbilityLevel > 0 ? ExplicitAbilityLevel : SavedTalent.Level);
			OutTalents.Add(MoveTemp(SavedTalent));
		}

		return true;
	}

	void CollectProgressionLevels(
		const TArray<FSpellRiseProgressionLevelEntry>& Entries,
		TArray<FSpellRiseSavedProgressionLevel>& OutEntries)
	{
		OutEntries.Reset();
		for (const FSpellRiseProgressionLevelEntry& Entry : Entries)
		{
			if (!Entry.ProgressionTag.IsValid())
			{
				continue;
			}

			FSpellRiseSavedProgressionLevel SavedEntry;
			SavedEntry.ProgressionTag = Entry.ProgressionTag;
			SavedEntry.Level = ClampPersistentTalentLevel(Entry.Level);
			OutEntries.Add(MoveTemp(SavedEntry));
		}
	}

	void CollectProgressionData(
		const ASpellRisePlayerState* PlayerState,
		int32& OutCharacterLevel,
		int32& OutExperience,
		int32& OutTalentPoints,
		int32& OutCraftPoints,
		int32& OutAttributePoints,
		TArray<FSpellRiseSavedProgressionLevel>& OutWeaponSkillLevels,
		TArray<FSpellRiseSavedProgressionLevel>& OutSchoolLevels)
	{
		OutCharacterLevel = 1;
		OutExperience = 0;
		OutTalentPoints = 100;
		OutCraftPoints = 100;
		OutAttributePoints = 0;
		OutWeaponSkillLevels.Reset();
		OutSchoolLevels.Reset();
		if (!PlayerState)
		{
			return;
		}

		const USpellRiseProgressionComponent* ProgressionComponent = PlayerState->GetProgressionComponent();
		if (!ProgressionComponent)
		{
			return;
		}

		OutCharacterLevel = FMath::Clamp(ProgressionComponent->GetCharacterLevel(), PersistentCharacterLevelMin, PersistentCharacterLevelMax);
		OutExperience = FMath::Clamp(ProgressionComponent->GetExperience(), 0, PersistentExperienceMax);
		OutTalentPoints = FMath::Clamp(ProgressionComponent->GetTalentPoints(), 0, PersistentProgressionCurrencyMax);
		OutCraftPoints = FMath::Clamp(ProgressionComponent->GetCraftPoints(), 0, PersistentProgressionCurrencyMax);
		OutAttributePoints = FMath::Clamp(ProgressionComponent->GetAttributePoints(), 0, PersistentProgressionCurrencyMax);
		CollectProgressionLevels(ProgressionComponent->GetWeaponSkillLevels(), OutWeaponSkillLevels);
		CollectProgressionLevels(ProgressionComponent->GetSchoolLevels(), OutSchoolLevels);
	}

	void CollectAbilityHotbarData(
		const ASpellRisePlayerState* PlayerState,
		TArray<FSpellRiseSavedAbilityHotbarSlot>& OutSlots)
	{
		OutSlots.Reset();
		if (!PlayerState)
		{
			return;
		}

		const USpellRiseAbilityHotbarComponent* HotbarComponent = PlayerState->GetAbilityHotbarComponent();
		if (!HotbarComponent)
		{
			return;
		}

		for (const FSpellRiseAbilityHotbarSlot& Slot : HotbarComponent->GetSlots())
		{
			if (Slot.SlotIndex == INDEX_NONE)
			{
				continue;
			}

			FSpellRiseSavedAbilityHotbarSlot SavedSlot;
			SavedSlot.SlotIndex = Slot.SlotIndex;
			SavedSlot.AbilityInputTag = Slot.AbilityInputTag;
			SavedSlot.AbilityClassPath = Slot.AbilityClass.ToSoftObjectPath().ToString();
			SavedSlot.AbilityDefinitionPath = Slot.AbilityDefinition.ToSoftObjectPath().ToString();
			OutSlots.Add(MoveTemp(SavedSlot));
		}
	}

	void ApplyAbilityHotbarData(ASpellRisePlayerState* PlayerState, const TArray<FSpellRiseSavedAbilityHotbarSlot>& SavedSlots)
	{
		if (!PlayerState || SavedSlots.Num() == 0)
		{
			return;
		}

		USpellRiseAbilityHotbarComponent* HotbarComponent = PlayerState->GetAbilityHotbarComponent();
		if (!HotbarComponent)
		{
			return;
		}

		TArray<FSpellRiseAbilityHotbarSlot> RuntimeSlots;
		RuntimeSlots.Reserve(SavedSlots.Num());
		for (const FSpellRiseSavedAbilityHotbarSlot& SavedSlot : SavedSlots)
		{
			if (SavedSlot.SlotIndex == INDEX_NONE)
			{
				continue;
			}

			FSpellRiseAbilityHotbarSlot RuntimeSlot;
			RuntimeSlot.SlotIndex = SavedSlot.SlotIndex;
			RuntimeSlot.AbilityInputTag = SavedSlot.AbilityInputTag;
			if (!SavedSlot.AbilityClassPath.IsEmpty())
			{
				RuntimeSlot.AbilityClass = TSoftClassPtr<UGameplayAbility>(FSoftObjectPath(SavedSlot.AbilityClassPath));
			}
			if (!SavedSlot.AbilityDefinitionPath.IsEmpty())
			{
				RuntimeSlot.AbilityDefinition = TSoftObjectPtr<UObject>(FSoftObjectPath(SavedSlot.AbilityDefinitionPath));
			}
			RuntimeSlots.Add(MoveTemp(RuntimeSlot));
		}

		HotbarComponent->ApplyPersistentSlots_Server(RuntimeSlots);
	}

	TSubclassOf<UEquippableItem> LoadEquippableItemClass(const FString& ClassPath)
	{
		if (ClassPath.IsEmpty())
		{
			return nullptr;
		}

		return LoadObject<UClass>(nullptr, *ClassPath);
	}

	void CollectEquipmentData(
		const ASpellRiseCharacterBase* Character,
		TArray<FSpellRiseSavedEquippedItem>& OutEquippedItems,
		TArray<FSpellRiseSavedWeaponLoadoutSlot>& OutWeaponLoadoutSlots,
		int32& OutActiveWeaponQuickSlotIndex,
		FString& OutOffHandItemClassPath)
	{
		OutEquippedItems.Reset();
		OutWeaponLoadoutSlots.Reset();
		OutActiveWeaponQuickSlotIndex = INDEX_NONE;
		OutOffHandItemClassPath.Reset();
		if (!Character)
		{
			return;
		}

		const USpellRiseEquipmentManagerComponent* EquipmentManager = Character->GetSpellRiseEquipmentManager();
		if (!EquipmentManager)
		{
			return;
		}

		for (const FSpellRiseHUDEquipmentSlotView& SlotView : EquipmentManager->GetHUDEquipmentSlotViews())
		{
			if (!SlotView.Item || !SlotView.Item->GetClass())
			{
				continue;
			}

			FSpellRiseSavedEquippedItem SavedItem;
			SavedItem.SlotName = SlotView.SlotName.ToString();
			SavedItem.ItemClassPath = SlotView.Item->GetClass()->GetPathName();
			OutEquippedItems.Add(MoveTemp(SavedItem));
		}

		OutActiveWeaponQuickSlotIndex = EquipmentManager->GetActiveQuickWeaponSlotIndex();
		for (int32 QuickSlotIndex = 0; QuickSlotIndex < 2; ++QuickSlotIndex)
		{
			const UEquippableItem* QuickSlotItem = EquipmentManager->GetQuickWeaponItemByIndex(QuickSlotIndex);
			if (!QuickSlotItem || !QuickSlotItem->GetClass())
			{
				continue;
			}

			FSpellRiseSavedWeaponLoadoutSlot SavedSlot;
			SavedSlot.QuickSlotIndex = QuickSlotIndex;
			SavedSlot.ItemClassPath = QuickSlotItem->GetClass()->GetPathName();
			OutWeaponLoadoutSlots.Add(MoveTemp(SavedSlot));
		}

		if (const UEquippableItem* OffHandItem = EquipmentManager->GetActiveOffHandItem())
		{
			if (OffHandItem->GetClass())
			{
				OutOffHandItemClassPath = OffHandItem->GetClass()->GetPathName();
			}
		}
	}

	void ApplyEquipmentData(ASpellRiseCharacterBase* Character, const FSpellRiseCharacterSaveData& Data)
	{
		if (!Character)
		{
			return;
		}

		USpellRiseEquipmentManagerComponent* EquipmentManager = Character->GetSpellRiseEquipmentManager();
		if (!EquipmentManager)
		{
			return;
		}

		TArray<TSubclassOf<UEquippableItem>> EquippedItemClasses;
		EquippedItemClasses.Reserve(Data.EquippedItems.Num());
		for (const FSpellRiseSavedEquippedItem& SavedItem : Data.EquippedItems)
		{
			if (TSubclassOf<UEquippableItem> ItemClass = LoadEquippableItemClass(SavedItem.ItemClassPath))
			{
				EquippedItemClasses.Add(ItemClass);
			}
		}

		TArray<TSubclassOf<UEquippableItem>> QuickWeaponSlotClasses;
		QuickWeaponSlotClasses.SetNum(2);
		for (const FSpellRiseSavedWeaponLoadoutSlot& SavedSlot : Data.WeaponLoadoutSlots)
		{
			if (!QuickWeaponSlotClasses.IsValidIndex(SavedSlot.QuickSlotIndex))
			{
				continue;
			}
			QuickWeaponSlotClasses[SavedSlot.QuickSlotIndex] = LoadEquippableItemClass(SavedSlot.ItemClassPath);
		}

		EquipmentManager->ApplyPersistentEquipment_Server(
			EquippedItemClasses,
			QuickWeaponSlotClasses,
			Data.ActiveWeaponQuickSlotIndex,
			LoadEquippableItemClass(Data.OffHandItemClassPath));
	}

	void SetTalentTreeTalentPoints(UActorComponent* TalentComponent, const int32 TalentPoints)
	{
		if (!TalentComponent || !TalentComponent->GetClass())
		{
			return;
		}

		if (FIntProperty* PointsProperty = FindFProperty<FIntProperty>(TalentComponent->GetClass(), TEXT("TalentPoints")))
		{
			PointsProperty->SetPropertyValue_InContainer(TalentComponent, FMath::Clamp(TalentPoints, 0, 1000000));
			if (UFunction* OnRepFunction = TalentComponent->FindFunction(TEXT("OnRep_TalentPoints")))
			{
				TalentComponent->ProcessEvent(OnRepFunction, nullptr);
			}
			if (UFunction* PointsChangedFunction = TalentComponent->FindFunction(TEXT("OnPointsChanged")))
			{
				TalentComponent->ProcessEvent(PointsChangedFunction, nullptr);
			}
		}
	}

	void ClearGrantedTalentArray(UActorComponent* TalentComponent)
	{
		if (!TalentComponent || !TalentComponent->GetClass())
		{
			return;
		}

		if (FArrayProperty* GrantedTalentsProperty = FindFProperty<FArrayProperty>(TalentComponent->GetClass(), TEXT("GrantedTalents")))
		{
			FScriptArrayHelper GrantedTalentsHelper(GrantedTalentsProperty, GrantedTalentsProperty->ContainerPtrToValuePtr<void>(TalentComponent));
			GrantedTalentsHelper.EmptyValues();
			if (UFunction* OnRepFunction = TalentComponent->FindFunction(TEXT("OnRep_GrantedTalents")))
			{
				TalentComponent->ProcessEvent(OnRepFunction, nullptr);
			}
		}
	}

	void ResetTalentTreeDataForMissingPersistence(AController* Controller, const TCHAR* Context)
	{
		UActorComponent* TalentComponent = ResolveTalentTreeComponent(Controller);
		if (!TalentComponent)
		{
			return;
		}

		ClearGrantedTalentArray(TalentComponent);
		UE_LOG(LogSpellRisePersistence, Log,
			TEXT("[Persistence][TalentsReset] Context=%s Component=%s"),
			Context ? Context : TEXT("unknown"),
			*GetNameSafe(TalentComponent));
	}

	void ApplyDefaultProgressionForMissingPersistence(AController* Controller, const TCHAR* Context)
	{
		if (!Controller)
		{
			return;
		}

		ASpellRisePlayerState* PlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState);
		if (!PlayerState)
		{
			return;
		}

		if (USpellRiseProgressionComponent* ProgressionComponent = PlayerState->GetProgressionComponent())
		{
			ProgressionComponent->InitializeCharacterProgressionDefaults_Server(true);
		}

		PlayerState->SetTalentPoints_Server(100);
		UE_LOG(LogSpellRisePersistence, Log,
			TEXT("[Persistence][ProgressionDefaultsApplied] Context=%s PlayerState=%s Level=1 Experience=0 TalentPoints=100 CraftPoints=100 AttributePoints=0"),
			Context ? Context : TEXT("unknown"),
			*GetNameSafe(PlayerState));
	}

	bool InvokeGrantTalent(UActorComponent* TalentComponent, UObject* TalentAsset, const int32 TalentLevel, const int32 AbilityLevel)
	{
		if (!TalentComponent || !TalentAsset)
		{
			return false;
		}

		UFunction* GrantTalentFunction = TalentComponent->FindFunction(TEXT("GrantTalent"));
		if (!GrantTalentFunction)
		{
			return false;
		}

		TArray<uint8> Params;
		Params.SetNumZeroed(GrantTalentFunction->ParmsSize);

		FBoolProperty* SuccessProperty = nullptr;
		void* SuccessValuePtr = nullptr;
		for (TFieldIterator<FProperty> It(GrantTalentFunction); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
		{
			FProperty* Property = *It;
			void* ValuePtr = Property->ContainerPtrToValuePtr<void>(Params.GetData());

			if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(Property))
			{
				if (Property->GetName().Contains(TEXT("Talent"), ESearchCase::IgnoreCase))
				{
					ObjectProperty->SetObjectPropertyValue(ValuePtr, TalentAsset);
				}
			}
			else if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
			{
				if (Property->GetName().Contains(TEXT("AbilityLevel"), ESearchCase::IgnoreCase))
				{
					IntProperty->SetPropertyValue(ValuePtr, ClampPersistentTalentLevel(AbilityLevel));
				}
				else if (Property->GetName().Contains(TEXT("Level"), ESearchCase::IgnoreCase))
				{
					IntProperty->SetPropertyValue(ValuePtr, ClampPersistentTalentLevel(TalentLevel));
				}
			}
			else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
			{
				if (Property->GetName().Equals(TEXT("SkipPointsCheck"), ESearchCase::IgnoreCase))
				{
					BoolProperty->SetPropertyValue(ValuePtr, true);
				}
				else if (Property->HasAnyPropertyFlags(CPF_OutParm))
				{
					SuccessProperty = BoolProperty;
					SuccessValuePtr = ValuePtr;
				}
			}
		}

		TalentComponent->ProcessEvent(GrantTalentFunction, Params.GetData());
		return !SuccessProperty || SuccessProperty->GetPropertyValue(SuccessValuePtr);
	}

	void ApplyTalentTreeData(AController* Controller, const FSpellRiseCharacterSaveData& Data)
	{
		UActorComponent* TalentComponent = ResolveTalentTreeComponent(Controller);
		if (!TalentComponent)
		{
			UE_LOG(LogSpellRisePersistence, Warning,
				TEXT("[Persistence][TalentsApplyRejected] Reason=missing_component Controller=%s"),
				*GetNameSafe(Controller));
			return;
		}

		int32 AppliedTalents = 0;
		int32 SkippedTalents = 0;
		ClearGrantedTalentArray(TalentComponent);
		for (const FSpellRiseSavedTalent& SavedTalent : Data.Talents)
		{
			if (SavedTalent.TalentAssetPath.IsEmpty() || SavedTalent.Level <= 0)
			{
				++SkippedTalents;
				continue;
			}

			UObject* TalentAsset = LoadObject<UObject>(nullptr, *SavedTalent.TalentAssetPath);
			const int32 SavedAbilityLevel = SavedTalent.AbilityLevel > 0 ? SavedTalent.AbilityLevel : SavedTalent.Level;
			if (!TalentAsset || !InvokeGrantTalent(TalentComponent, TalentAsset, SavedTalent.Level, SavedAbilityLevel))
			{
				++SkippedTalents;
				continue;
			}

			++AppliedTalents;
		}

		SetTalentTreeTalentPoints(TalentComponent, Data.TalentPoints);
		if (ASpellRisePlayerState* PlayerState = Cast<ASpellRisePlayerState>(Controller ? Controller->PlayerState : nullptr))
		{
			PlayerState->SetTalentPoints_Server(Data.TalentPoints);
		}
		UE_LOG(LogSpellRisePersistence, Log,
			TEXT("[Persistence][TalentsApplied] Component=%s Applied=%d Skipped=%d TalentPoints=%d"),
			*GetNameSafe(TalentComponent),
			AppliedTalents,
			SkippedTalents,
			FMath::Max(0, Data.TalentPoints));
	}

	void ApplyProgressionData(ASpellRisePlayerState* PlayerState, const FSpellRiseCharacterSaveData& Data)
	{
		if (!PlayerState)
		{
			return;
		}

		USpellRiseProgressionComponent* ProgressionComponent = PlayerState->GetProgressionComponent();
		if (!ProgressionComponent)
		{
			UE_LOG(LogSpellRisePersistence, Warning,
				TEXT("[Persistence][ProgressionApplyRejected] Reason=missing_component PlayerState=%s"),
				*GetNameSafe(PlayerState));
			return;
		}

		const int32 SavedCharacterLevel = Data.SchemaVersion >= 9
			? FMath::Clamp(Data.CharacterLevel, PersistentCharacterLevelMin, PersistentCharacterLevelMax)
			: 1;
		const int32 SavedExperience = Data.SchemaVersion >= 9
			? FMath::Clamp(Data.Experience, 0, PersistentExperienceMax)
			: 0;
		const int32 SavedTalentPoints = Data.SchemaVersion >= 10
			? FMath::Clamp(Data.TalentPoints, 0, PersistentProgressionCurrencyMax)
			: FMath::Clamp(Data.TalentPoints, 0, PersistentProgressionCurrencyMax);
		const int32 SavedCraftPoints = Data.SchemaVersion >= 9
			? FMath::Clamp(Data.CraftPoints, 0, PersistentProgressionCurrencyMax)
			: 100;
		const int32 SavedAttributePoints = Data.SchemaVersion >= 9
			? FMath::Clamp(Data.AttributePoints, 0, PersistentProgressionCurrencyMax)
			: 0;
		ProgressionComponent->SetCharacterProgression_Server(SavedCharacterLevel, SavedExperience, SavedTalentPoints, SavedCraftPoints, SavedAttributePoints);
		PlayerState->SetTalentPoints_Server(SavedTalentPoints);

		ProgressionComponent->ResetProgressionLevels_Server();

		int32 AppliedWeaponLevels = 0;
		int32 AppliedSchoolLevels = 0;
		for (const FSpellRiseSavedProgressionLevel& SavedLevel : Data.WeaponSkillLevels)
		{
			if (SavedLevel.ProgressionTag.IsValid()
				&& ProgressionComponent->SetWeaponSkillLevel_Server(SavedLevel.ProgressionTag, SavedLevel.Level))
			{
				++AppliedWeaponLevels;
			}
		}

		for (const FSpellRiseSavedProgressionLevel& SavedLevel : Data.SchoolLevels)
		{
			if (SavedLevel.ProgressionTag.IsValid()
				&& ProgressionComponent->SetSchoolLevel_Server(SavedLevel.ProgressionTag, SavedLevel.Level))
			{
				++AppliedSchoolLevels;
			}
		}

		UE_LOG(LogSpellRisePersistence, Log,
			TEXT("[Persistence][ProgressionApplied] PlayerState=%s Level=%d Experience=%d TalentPoints=%d CraftPoints=%d AttributePoints=%d WeaponLevels=%d SchoolLevels=%d"),
			*GetNameSafe(PlayerState),
			SavedCharacterLevel,
			SavedExperience,
			SavedTalentPoints,
			SavedCraftPoints,
			SavedAttributePoints,
			AppliedWeaponLevels,
			AppliedSchoolLevels);
	}

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

	bool IsValidSteamId64(const FString& PersistentId)
	{
		if (!IsValidPersistentId(PersistentId))
		{
			return false;
		}

		if (PersistentId.StartsWith(TEXT("DEV_"), ESearchCase::IgnoreCase) ||
			PersistentId.StartsWith(TEXT("NULL:"), ESearchCase::IgnoreCase))
		{
			return false;
		}

		return PersistentId.Len() == 17 && PersistentId.IsNumeric();
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

		if (!IsValidSteamId64(Data.SteamId64))
		{
			OutReason = TEXT("invalid_or_non_steam_persistent_id");
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

		if (!IsValidOptionalJsonObjectString(Data.VisualConfigurationJson))
		{
			OutReason = TEXT("invalid_visual_configuration");
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

		if (Data.TalentPoints < 0 || Data.TalentPoints > 1000000)
		{
			OutReason = TEXT("invalid_talent_points");
			return false;
		}

		if (Data.CharacterLevel < PersistentCharacterLevelMin || Data.CharacterLevel > PersistentCharacterLevelMax)
		{
			OutReason = TEXT("invalid_character_level");
			return false;
		}

		if (Data.Experience < 0 || Data.Experience > PersistentExperienceMax)
		{
			OutReason = TEXT("invalid_character_experience");
			return false;
		}

		if (Data.CraftPoints < 0 || Data.CraftPoints > PersistentProgressionCurrencyMax
			|| Data.AttributePoints < 0 || Data.AttributePoints > PersistentProgressionCurrencyMax)
		{
			OutReason = TEXT("invalid_character_progression");
			return false;
		}

		for (const FSpellRiseSavedTalent& Talent : Data.Talents)
		{
			const int32 EffectiveAbilityLevel = Talent.AbilityLevel > 0 ? Talent.AbilityLevel : Talent.Level;
			if (Talent.TalentAssetPath.IsEmpty()
				|| Talent.Level < PersistentTalentLevelMin
				|| Talent.Level > PersistentTalentLevelMax
				|| EffectiveAbilityLevel < PersistentTalentLevelMin
				|| EffectiveAbilityLevel > PersistentTalentLevelMax)
			{
				OutReason = TEXT("invalid_talent_state");
				return false;
			}
		}

		auto ValidateProgressionLevels = [&OutReason](const TArray<FSpellRiseSavedProgressionLevel>& Levels) -> bool
		{
			for (const FSpellRiseSavedProgressionLevel& Level : Levels)
			{
				if (!Level.ProgressionTag.IsValid()
					|| Level.Level < PersistentTalentLevelMin
					|| Level.Level > PersistentTalentLevelMax)
				{
					OutReason = TEXT("invalid_progression_state");
					return false;
				}
			}

			return true;
		};

		if (!ValidateProgressionLevels(Data.WeaponSkillLevels)
			|| !ValidateProgressionLevels(Data.SchoolLevels))
		{
			return false;
		}

		for (const FSpellRiseSavedAbilityHotbarSlot& Slot : Data.AbilityHotbarSlots)
		{
			if (Slot.SlotIndex < 0 || Slot.SlotIndex >= 16)
			{
				OutReason = TEXT("invalid_hotbar_slot");
				return false;
			}
			if ((!Slot.AbilityClassPath.IsEmpty() && Slot.AbilityClassPath.Len() > 512)
				|| (!Slot.AbilityDefinitionPath.IsEmpty() && Slot.AbilityDefinitionPath.Len() > 512))
			{
				OutReason = TEXT("invalid_hotbar_path");
				return false;
			}
		}

		for (const FSpellRiseSavedEquippedItem& Item : Data.EquippedItems)
		{
			if (Item.ItemClassPath.IsEmpty() || Item.ItemClassPath.Len() > 512 || Item.SlotName.Len() > 64)
			{
				OutReason = TEXT("invalid_equipped_item");
				return false;
			}
		}

		for (const FSpellRiseSavedWeaponLoadoutSlot& Slot : Data.WeaponLoadoutSlots)
		{
			if (Slot.QuickSlotIndex < 0 || Slot.QuickSlotIndex > 1 || Slot.ItemClassPath.IsEmpty() || Slot.ItemClassPath.Len() > 512)
			{
				OutReason = TEXT("invalid_weapon_loadout");
				return false;
			}
		}

		if ((!Data.OffHandItemClassPath.IsEmpty() && Data.OffHandItemClassPath.Len() > 512)
			|| (Data.ActiveWeaponQuickSlotIndex != INDEX_NONE && (Data.ActiveWeaponQuickSlotIndex < 0 || Data.ActiveWeaponQuickSlotIndex > 1)))
		{
			OutReason = TEXT("invalid_equipment_state");
			return false;
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

		if (!IsValidSteamId64(Data.SteamId64))
		{
			OutReason = TEXT("invalid_or_non_steam_persistent_id");
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

	bool bProviderWasExplicit = false;
	const FString RequestedProvider = GetRequestedPersistenceProvider(bProviderWasExplicit);
	const bool bRequirePostgresNoFallback = IsPostgresRequiredWithoutFallback();
	const bool bAllowFileFallback = IsFilePersistenceFallbackAllowed();
	const bool bNoSteamMode = IsNoSteamPersistenceModeActive();
	TUniquePtr<ISpellRisePersistenceProvider> SelectedProvider;
	ActiveProviderName.Reset();
	PersistenceOperationalMode.Reset();
	PersistenceBlockReason.Reset();

	if (bNoSteamMode)
	{
		PersistenceOperationalMode = TEXT("nosteam_disabled");
		PersistenceBlockReason = TEXT("nosteam_mode");
		UE_LOG(LogSpellRisePersistence, Warning,
			TEXT("[Persistence][ProviderDisabled] Mode=%s Reason=%s"),
			*PersistenceOperationalMode,
			*PersistenceBlockReason);
	}
	else if (RequestedProvider.Equals(TEXT("postgres"), ESearchCase::IgnoreCase))
	{
		SelectedProvider = MakeUnique<FSpellRisePostgresPersistenceProvider>();
		if (!SelectedProvider->IsReady())
		{
			if (bRequirePostgresNoFallback || !bAllowFileFallback)
			{
				SelectedProvider.Reset();
				PersistenceOperationalMode = TEXT("blocked");
				PersistenceBlockReason = TEXT("postgres_unavailable");
				UE_LOG(LogSpellRisePersistence, Error,
					TEXT("[Persistence][ProviderBlocked] Requested=postgres Reason=%s FallbackAllowed=%d RequirePostgres=%d"),
					*PersistenceBlockReason,
					bAllowFileFallback ? 1 : 0,
					bRequirePostgresNoFallback ? 1 : 0);
			}
			else
			{
				SelectedProvider.Reset();
				UE_LOG(LogSpellRisePersistence, Warning,
					TEXT("[Persistence][ProviderFallback] Requested=postgres Fallback=file Reason=postgres_unavailable"));
			}
		}
		else
		{
			PersistenceOperationalMode = TEXT("normal");
		}
	}
	else if (!RequestedProvider.Equals(TEXT("file"), ESearchCase::IgnoreCase))
	{
		PersistenceOperationalMode = TEXT("blocked");
		PersistenceBlockReason = FString::Printf(TEXT("unknown_provider_%s"), *RequestedProvider);
		UE_LOG(LogSpellRisePersistence, Error,
			TEXT("[Persistence][ProviderBlocked] Requested=%s Reason=%s"),
			*RequestedProvider,
			*PersistenceBlockReason);
	}

	if (!SelectedProvider.IsValid() &&
		!bNoSteamMode &&
		!PersistenceOperationalMode.Equals(TEXT("blocked"), ESearchCase::IgnoreCase))
	{
		SelectedProvider = MakeUnique<FSpellRiseFilePersistenceProvider>();
		PersistenceOperationalMode = bProviderWasExplicit ? TEXT("explicit_file") : TEXT("degraded_file_fallback");
		if (bProviderWasExplicit)
		{
			UE_LOG(LogSpellRisePersistence, Warning,
				TEXT("[Persistence][ProviderSelected] Provider=file Mode=%s Reason=explicit_request"),
				*PersistenceOperationalMode);
		}
	}

	Provider = MoveTemp(SelectedProvider);
	if (Provider)
	{
		ActiveProviderName = Provider->GetProviderName();
		if (PersistenceOperationalMode.IsEmpty())
		{
			PersistenceOperationalMode = TEXT("normal");
		}

		UE_LOG(LogSpellRisePersistence, Log,
			TEXT("[Persistence][ProviderReady] Provider=%s Mode=%s Requested=%s Explicit=%d"),
			*ActiveProviderName,
			*PersistenceOperationalMode,
			*RequestedProvider,
			bProviderWasExplicit ? 1 : 0);
	}
	else if (PersistenceOperationalMode.IsEmpty())
	{
		PersistenceOperationalMode = TEXT("blocked");
		PersistenceBlockReason = TEXT("provider_unavailable");
		UE_LOG(LogSpellRisePersistence, Error,
			TEXT("[Persistence][ProviderBlocked] Requested=%s Reason=%s"),
			*RequestedProvider,
			*PersistenceBlockReason);
	}

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
	ActiveProviderName.Reset();
	PersistenceOperationalMode.Reset();
	PersistenceBlockReason.Reset();
	bWorldDirty = false;

	Super::Deinitialize();
}

void USpellRisePersistenceSubsystem::RecordPersistenceTelemetry(const TCHAR* Operation, bool bSuccess, double LatencyMs, const TCHAR* Reason)
{
	const FString OperationName = Operation ? Operation : TEXT("Unknown");
	const FString ReasonText = Reason ? Reason : TEXT("unspecified");
	const FString ModeText = PersistenceOperationalMode.IsEmpty() ? TEXT("unknown") : PersistenceOperationalMode;
	const FString ProviderText = ActiveProviderName.IsEmpty() ? TEXT("none") : ActiveProviderName;

	LatencyByOperationMs.FindOrAdd(OperationName).Add(FMath::Max(0.0, LatencyMs));
	if (!bSuccess)
	{
		++FailureCountByOperation.FindOrAdd(OperationName);
	}

	if (!bSuccess && IsPIEWorld(GetWorld()) && IsDevFallbackPersistenceReason(Reason))
	{
		UE_LOG(LogSpellRisePersistence, Verbose,
			TEXT("[Persistence][%s] Result=%s Reason=%s Provider=%s Mode=%s LatencyMs=%.2f FailureCount=%d"),
			*OperationName,
			TEXT("Rejected"),
			*ReasonText,
			*ProviderText,
			*ModeText,
			FMath::Max(0.0, LatencyMs),
			FailureCountByOperation.FindRef(OperationName));
		return;
	}

	UE_LOG(LogSpellRisePersistence, Log,
		TEXT("[Persistence][%s] Result=%s Reason=%s Provider=%s Mode=%s LatencyMs=%.2f FailureCount=%d"),
		*OperationName,
		bSuccess ? TEXT("Succeeded") : TEXT("Rejected"),
		*ReasonText,
		*ProviderText,
		*ModeText,
		FMath::Max(0.0, LatencyMs),
		FailureCountByOperation.FindRef(OperationName));
}

bool USpellRisePersistenceSubsystem::PreloadCharacterForController(AController* Controller)
{
	const double StartSeconds = FPlatformTime::Seconds();
	if (IsNoSteamPersistenceModeActive())
	{
		RecordPersistenceTelemetry(TEXT("PreloadCharacter"), false, 0.0, TEXT("nosteam_mode"));
		return false;
	}

	if (!Provider || !Controller)
	{
		RecordPersistenceTelemetry(TEXT("PreloadCharacter"), false, (FPlatformTime::Seconds() - StartSeconds) * 1000.0, !Provider ? TEXT("provider_unavailable") : TEXT("missing_controller"));
		return false;
	}

	if (ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState))
	{
		SRPlayerState->SetPersistenceProfileApplied(false);
	}

	const FString SteamId64 = ResolveSteamIdFromController(Controller);
	if (!IsValidSteamId64(SteamId64))
	{
		RecordPersistenceTelemetry(TEXT("PreloadCharacter"), false, (FPlatformTime::Seconds() - StartSeconds) * 1000.0, TEXT("invalid_or_non_steam_persistent_id"));
		return false;
	}

	FSpellRiseCharacterSaveData LoadedData;
	int64 LoadedCharacterRevision = 0;
	if (!Provider->LoadCharacterState(SteamId64, LoadedData, LoadedCharacterRevision))
	{
		RecordPersistenceTelemetry(TEXT("PreloadCharacter"), false, (FPlatformTime::Seconds() - StartSeconds) * 1000.0, TEXT("character_load_miss_or_failed"));
		return false;
	}

	FString CharacterValidationReason;
	if (!ValidateCharacterSaveData(LoadedData, SteamId64, CharacterValidationReason))
	{
		CachedCharacterDataBySteamId.Remove(SteamId64);
		CachedInventoryDataBySteamId.Remove(SteamId64);
		CharacterRevisionBySteamId.Remove(SteamId64);
		InventoryRevisionBySteamId.Remove(SteamId64);
		RecordPersistenceTelemetry(TEXT("PreloadCharacter"), false, (FPlatformTime::Seconds() - StartSeconds) * 1000.0, *CharacterValidationReason);
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
		}
	}
	else
	{
		CachedInventoryDataBySteamId.Remove(SteamId64);
		InventoryRevisionBySteamId.Remove(SteamId64);
	}

	CachedCharacterDataBySteamId.Add(SteamId64, MoveTemp(LoadedData));
	CharacterRevisionBySteamId.Add(SteamId64, LoadedCharacterRevision);
	RecordPersistenceTelemetry(TEXT("PreloadCharacter"), true, (FPlatformTime::Seconds() - StartSeconds) * 1000.0, bHasInventoryData ? TEXT("loaded_character_and_inventory") : TEXT("loaded_character_only"));
	return true;
}

bool USpellRisePersistenceSubsystem::ApplyCachedCharacterToController(AController* Controller)
{
	const double StartSeconds = FPlatformTime::Seconds();
	if (IsNoSteamPersistenceModeActive())
	{
		if (Controller)
		{
			EnsureDefaultItemsForControllerIfNeeded(Controller, TEXT("nosteam_mode"));
			ApplyDefaultProgressionForMissingPersistence(Controller, TEXT("nosteam_mode"));
			if (ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState))
			{
				SRPlayerState->SetPersistenceProfileApplied(true);
			}
		}
		RecordPersistenceTelemetry(TEXT("ApplyCachedCharacter"), false, 0.0, TEXT("nosteam_mode"));
		return false;
	}

	if (!Controller)
	{
		RecordPersistenceTelemetry(TEXT("ApplyCachedCharacter"), false, 0.0, TEXT("missing_controller"));
		return false;
	}

	const FString SteamId64 = ResolveSteamIdFromController(Controller);
	if (!IsValidSteamId64(SteamId64))
	{
		EnsureDefaultItemsForControllerIfNeeded(Controller, TEXT("invalid_or_non_steam_persistent_id"));
		ResetTalentTreeDataForMissingPersistence(Controller, TEXT("invalid_or_non_steam_persistent_id"));
		ApplyDefaultProgressionForMissingPersistence(Controller, TEXT("invalid_or_non_steam_persistent_id"));
		if (ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState))
		{
			SRPlayerState->SetPersistenceProfileApplied(true);
		}
		RecordPersistenceTelemetry(TEXT("ApplyCachedCharacter"), false, (FPlatformTime::Seconds() - StartSeconds) * 1000.0, TEXT("invalid_or_non_steam_persistent_id"));
		return false;
	}

	const FSpellRiseCharacterSaveData* CachedData = CachedCharacterDataBySteamId.Find(SteamId64);
	if (!CachedData)
	{
		EnsureDefaultItemsForControllerIfNeeded(Controller, TEXT("no_cached_data"));
		ResetTalentTreeDataForMissingPersistence(Controller, TEXT("no_cached_data"));
		ApplyDefaultProgressionForMissingPersistence(Controller, TEXT("no_cached_data"));
		if (ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState))
		{
			SRPlayerState->SetPersistenceProfileApplied(true);
		}
		RecordPersistenceTelemetry(TEXT("ApplyCachedCharacter"), false, (FPlatformTime::Seconds() - StartSeconds) * 1000.0, TEXT("no_cached_data"));
		return false;
	}

	FString ValidationReason;
	if (!ValidateCharacterSaveData(*CachedData, SteamId64, ValidationReason))
	{
		RecordPersistenceTelemetry(TEXT("ApplyCachedCharacter"), false, (FPlatformTime::Seconds() - StartSeconds) * 1000.0, *ValidationReason);
		return false;
	}

	const bool bApplied = ApplyCharacterDataToController(Controller, *CachedData);
	if (ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState))
	{
		SRPlayerState->SetPersistenceProfileApplied(true);
	}
	RecordPersistenceTelemetry(TEXT("ApplyCachedCharacter"), bApplied, (FPlatformTime::Seconds() - StartSeconds) * 1000.0, bApplied ? TEXT("applied") : TEXT("apply_failed"));
	return bApplied;
}

bool USpellRisePersistenceSubsystem::SaveCharacterForController(AController* Controller)
{
	const double SaveStartSeconds = FPlatformTime::Seconds();
	if (IsNoSteamPersistenceModeActive())
	{
		RecordPersistenceTelemetry(TEXT("SaveCharacter"), false, 0.0, TEXT("nosteam_mode"));
		return false;
	}

	if (!Provider || !Controller)
	{
		RecordPersistenceTelemetry(TEXT("SaveCharacter"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, !Provider ? TEXT("provider_unavailable") : TEXT("missing_controller"));
		return false;
	}

	const FString SteamId64 = ResolveSteamIdFromController(Controller);
	if (!IsValidSteamId64(SteamId64))
	{
		RecordPersistenceTelemetry(TEXT("SaveCharacter"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, TEXT("invalid_or_non_steam_persistent_id"));
		return false;
	}

	if (SaveInProgressPersistentIds.Contains(SteamId64))
	{
		RecordPersistenceTelemetry(TEXT("SaveCharacter"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, TEXT("save_in_progress"));
		return false;
	}

	SaveInProgressPersistentIds.Add(SteamId64);
	ON_SCOPE_EXIT
	{
		SaveInProgressPersistentIds.Remove(SteamId64);
	};

	FSpellRiseCharacterSaveData SaveData;
	if (!CollectCharacterData(Controller, SteamId64, SaveData))
	{
		RecordPersistenceTelemetry(TEXT("SaveCharacter"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, TEXT("collect_failed"));
		return false;
	}

	FSpellRiseInventorySaveData InventoryData;
	BuildInventorySnapshotFromCharacterData(SaveData, SteamId64, InventoryData);

	FString CharacterValidationReason;
	if (!ValidateCharacterSaveData(SaveData, SteamId64, CharacterValidationReason))
	{
		RecordPersistenceTelemetry(TEXT("SaveCharacter"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, *CharacterValidationReason);
		return false;
	}

	FString InventoryValidationReason;
	if (!ValidateInventorySaveData(InventoryData, SteamId64, InventoryValidationReason))
	{
		RecordPersistenceTelemetry(TEXT("SaveCharacter"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, *InventoryValidationReason);
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
		RecordPersistenceTelemetry(TEXT("SaveCharacter"), false, SaveLatencyMs, TEXT("character_save_failed"));
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
		RecordPersistenceTelemetry(TEXT("SaveCharacter"), false, SaveLatencyMs, bRollbackCharacterSaved ? TEXT("inventory_save_failed_rollback_ok") : TEXT("inventory_save_failed_rollback_failed"));
		return false;
	}

	CachedCharacterDataBySteamId.Add(SteamId64, SaveData);
	CachedInventoryDataBySteamId.Add(SteamId64, InventoryData);
	CharacterRevisionBySteamId.Add(SteamId64, TargetRevision);
	InventoryRevisionBySteamId.Add(SteamId64, TargetRevision);
	DirtyCharacterIds.Remove(SteamId64);

	if (!bHadPreviousInventory)
	{
	}

	const double SaveLatencyMs = (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0;
	if (!bHadPreviousCharacter)
	{
	}

	RecordPersistenceTelemetry(TEXT("SaveCharacter"), true, SaveLatencyMs, TEXT("saved"));
	return true;
}

bool USpellRisePersistenceSubsystem::SaveDirtyCharacters(UWorld* World)
{
	if (!World || DirtyCharacterIds.IsEmpty())
	{
		return false;
	}

	bool bSavedAny = false;
	for (APlayerController* PlayerController : TActorRange<APlayerController>(World))
	{
		if (!PlayerController || !PlayerController->PlayerState)
		{
			continue;
		}

		const FString SteamId64 = ResolveSteamIdFromController(PlayerController);
		if (!DirtyCharacterIds.Contains(SteamId64))
		{
			continue;
		}

		bSavedAny |= SaveCharacterForController(PlayerController);
	}

	return bSavedAny;
}

bool USpellRisePersistenceSubsystem::HasCachedCharacterCreatedForController(AController* Controller) const
{
	bool bCharacterCreated = false;
	FString IgnoredVisualConfigurationJson;
	return TryGetCachedCharacterCreationState(Controller, bCharacterCreated, IgnoredVisualConfigurationJson) && bCharacterCreated;
}

bool USpellRisePersistenceSubsystem::TryGetCachedCharacterCreationState(AController* Controller, bool& bOutCharacterCreated, FString& OutVisualConfigurationJson) const
{
	bOutCharacterCreated = false;
	OutVisualConfigurationJson.Reset();

	if (!Controller)
	{
		return false;
	}

	const FString SteamId64 = ResolveSteamIdFromController(Controller);
	if (!IsValidSteamId64(SteamId64))
	{
		return false;
	}

	const FSpellRiseCharacterSaveData* CachedData = CachedCharacterDataBySteamId.Find(SteamId64);
	if (!CachedData)
	{
		return false;
	}

	bOutCharacterCreated = CachedData->bCharacterCreated;
	OutVisualConfigurationJson = CachedData->VisualConfigurationJson;
	return true;
}

bool USpellRisePersistenceSubsystem::SaveWorld(UWorld* World)
{
	const double SaveStartSeconds = FPlatformTime::Seconds();
	if (IsNoSteamPersistenceModeActive())
	{
		RecordPersistenceTelemetry(TEXT("SaveWorld"), false, 0.0, TEXT("nosteam_mode"));
		return false;
	}

	if (!Provider || !World)
	{
		RecordPersistenceTelemetry(TEXT("SaveWorld"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, !Provider ? TEXT("provider_unavailable") : TEXT("missing_world"));
		return false;
	}

	if (!IsServerWorld(World))
	{
		RecordPersistenceTelemetry(TEXT("SaveWorld"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, TEXT("not_server_world"));
		return false;
	}

	if (!bWorldDirty)
	{
		RecordPersistenceTelemetry(TEXT("SaveWorld"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, TEXT("not_dirty"));
		return false;
	}

	FSpellRiseWorldSaveData WorldSaveData;
	WorldSaveData.SchemaVersion = PersistenceWorldSchemaVersion;
	WorldSaveData.WorldId = ResolveWorldId(World);
	FSpellRiseBuildPersistenceAdapter::GatherWorldBuildingActors(World, WorldSaveData.BuildingActors);

	FString ValidationReason;
	if (!ValidateWorldSaveData(WorldSaveData, WorldSaveData.WorldId, ValidationReason))
	{
		RecordPersistenceTelemetry(TEXT("SaveWorld"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, *ValidationReason);
		return false;
	}

	const bool bSaved = Provider->SaveWorld(WorldSaveData.WorldId, WorldSaveData);
	const double SaveLatencyMs = (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0;
	if (bSaved)
	{
		bWorldDirty = false;
	}
	else
	{
	}

	RecordPersistenceTelemetry(TEXT("SaveWorld"), bSaved, SaveLatencyMs, bSaved ? TEXT("saved") : TEXT("provider_save_failed"));
	return bSaved;
}

bool USpellRisePersistenceSubsystem::LoadWorld(UWorld* World)
{
	const double LoadStartSeconds = FPlatformTime::Seconds();
	if (IsNoSteamPersistenceModeActive())
	{
		RecordPersistenceTelemetry(TEXT("LoadWorld"), false, 0.0, TEXT("nosteam_mode"));
		return false;
	}

	if (!Provider || !World)
	{
		RecordPersistenceTelemetry(TEXT("LoadWorld"), false, (FPlatformTime::Seconds() - LoadStartSeconds) * 1000.0, !Provider ? TEXT("provider_unavailable") : TEXT("missing_world"));
		return false;
	}

	if (!IsServerWorld(World))
	{
		RecordPersistenceTelemetry(TEXT("LoadWorld"), false, (FPlatformTime::Seconds() - LoadStartSeconds) * 1000.0, TEXT("not_server_world"));
		return false;
	}

	const FString WorldId = ResolveWorldId(World);
	FSpellRiseWorldSaveData LoadedWorldData;
	if (!Provider->LoadWorld(WorldId, LoadedWorldData))
	{
		RecordPersistenceTelemetry(TEXT("LoadWorld"), false, (FPlatformTime::Seconds() - LoadStartSeconds) * 1000.0, TEXT("provider_load_failed_or_missing"));
		return false;
	}

	FString ValidationReason;
	if (!ValidateWorldSaveData(LoadedWorldData, WorldId, ValidationReason))
	{
		RecordPersistenceTelemetry(TEXT("LoadWorld"), false, (FPlatformTime::Seconds() - LoadStartSeconds) * 1000.0, *ValidationReason);
		return false;
	}

	if (LoadedWorldData.SchemaVersion == LegacyPersistenceWorldSchemaVersion)
	{
	}

	FSpellRiseBuildPersistenceAdapter::SpawnMissingBuildingActors(World, LoadedWorldData.BuildingActors, SpawnActorLocationMatchToleranceSq);

	bWorldDirty = false;
	RecordPersistenceTelemetry(TEXT("LoadWorld"), true, (FPlatformTime::Seconds() - LoadStartSeconds) * 1000.0, TEXT("loaded"));
	return true;
}

bool USpellRisePersistenceSubsystem::SaveDeathEvent(const FSpellRiseDeathEventData& Data)
{
	const double SaveStartSeconds = FPlatformTime::Seconds();
	if (IsNoSteamPersistenceModeActive())
	{
		RecordPersistenceTelemetry(TEXT("SaveDeathEvent"), false, 0.0, TEXT("nosteam_mode"));
		return false;
	}

	if (!Provider)
	{
		RecordPersistenceTelemetry(TEXT("SaveDeathEvent"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, TEXT("provider_unavailable"));
		return false;
	}

	if (Data.VictimPlayerId.IsEmpty() && Data.VictimName.IsEmpty())
	{
		RecordPersistenceTelemetry(TEXT("SaveDeathEvent"), false, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, TEXT("missing_victim"));
		return false;
	}

	const bool bSaved = Provider->SaveDeathEvent(Data);
	RecordPersistenceTelemetry(TEXT("SaveDeathEvent"), bSaved, (FPlatformTime::Seconds() - SaveStartSeconds) * 1000.0, bSaved ? TEXT("saved") : TEXT("provider_save_failed"));
	return bSaved;
}

bool USpellRisePersistenceSubsystem::BuildRespawnTransformForController(AController* Controller, FTransform& OutSpawnTransform) const
{
	if (IsNoSteamPersistenceModeActive())
	{
		return false;
	}

	if (!Controller || !Controller->PlayerState)
	{
		return false;
	}

	const FString SteamId64 = ResolveSteamIdFromController(Controller);
	if (!IsValidSteamId64(SteamId64))
	{
		return false;
	}

	const FSpellRiseCharacterSaveData* Data = CachedCharacterDataBySteamId.Find(SteamId64);
	if (!Data)
	{
		return false;
	}

	FString ValidationReason;
	if (!ValidateCharacterSaveData(*Data, SteamId64, ValidationReason))
	{
		return false;
	}

	if (AActor* BedActor = ResolveSavedBedActor(Controller->GetWorld(), *Data))
	{
		OutSpawnTransform = BedActor->GetActorTransform();
		OutSpawnTransform.AddToTranslation(FVector(0.0f, 0.0f, 120.0f));
		FString TransformValidationReason;
		if (!ValidateWorldSpawnTransform(Controller->GetWorld(), OutSpawnTransform, TransformValidationReason))
		{
			return false;
		}

		return true;
	}

	return false;
}

void USpellRisePersistenceSubsystem::MarkWorldDirty()
{
	bWorldDirty = true;
}

void USpellRisePersistenceSubsystem::MarkPlayerDirtyBySteamId(const FString& SteamId64)
{
	if (IsValidSteamId64(SteamId64))
	{
		DirtyCharacterIds.Add(SteamId64);
		return;
	}

}

void USpellRisePersistenceSubsystem::SetControllerPersistentId(const AController* Controller, const FString& PersistentId)
{
	if (!Controller || !IsValidSteamId64(PersistentId))
	{
		if (Controller && IsPIEWorld(Controller->GetWorld()))
		{
			UE_LOG(LogSpellRisePersistence, Verbose, TEXT("[Persistence][ControllerPersistentIdRejected] Reason=invalid_or_non_steam_persistent_id Controller=%s PersistentId=%s"),
				*GetNameSafe(Controller),
				*PersistentId);
			return;
		}

		UE_LOG(LogSpellRisePersistence, Warning, TEXT("[Persistence][ControllerPersistentIdRejected] Reason=invalid_or_non_steam_persistent_id Controller=%s PersistentId=%s"),
			*GetNameSafe(Controller),
			*PersistentId);
		return;
	}

	PersistentIdByController.Add(Controller, PersistentId);
	UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][ControllerPersistentIdSet] Controller=%s PersistentId=%s"),
		*GetNameSafe(Controller),
		*PersistentId);
}

void USpellRisePersistenceSubsystem::ClearControllerPersistentId(const AController* Controller)
{
	if (!Controller)
	{
		return;
	}

	PersistentIdByController.Remove(Controller);
	UE_LOG(LogSpellRisePersistence, Log, TEXT("[Persistence][ControllerPersistentIdCleared] Controller=%s"),
		*GetNameSafe(Controller));
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
	return IsValidSteamId64(OutSteamId64);
}

FString USpellRisePersistenceSubsystem::ResolveSteamIdFromController(const AController* Controller) const
{
	if (!Controller)
	{
		return FString();
	}

	if (const FString* RegisteredId = PersistentIdByController.Find(TWeakObjectPtr<const AController>(Controller)))
	{
		if (IsValidSteamId64(*RegisteredId))
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
		return false;
	}

	ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(Controller->GetPawn());
	if (!Character)
	{
		if (const ASpellRisePlayerController* SpellRisePC = Cast<ASpellRisePlayerController>(Controller))
		{
			Character = Cast<ASpellRiseCharacterBase>(SpellRisePC->GetLastSpellRiseControlledPawn());
		}
	}
	ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState);
	if (!SRPlayerState)
	{
		return false;
	}

	USpellRiseAbilitySystemComponent* ASC = SRPlayerState->GetSpellRiseASC();
	if (!ASC)
	{
		return false;
	}

	OutData = FSpellRiseCharacterSaveData();
	OutData.SchemaVersion = PersistenceCharacterSchemaVersion;
	OutData.SteamId64 = SteamId64;
	OutData.PlayerDisplayName = Controller->PlayerState->GetPlayerName();
	OutData.bCharacterCreated = true;
	if (Character)
	{
		OutData.CharacterTransform = Character->GetActorTransform();
		OutData.ArchetypeValue = static_cast<uint8>(Character->Archetype);
	}
	else if (const FSpellRiseCharacterSaveData* CachedData = CachedCharacterDataBySteamId.Find(SteamId64))
	{
		OutData.CharacterTransform = CachedData->CharacterTransform;
		OutData.ArchetypeValue = CachedData->ArchetypeValue;
		OutData.RespawnBedActorName = CachedData->RespawnBedActorName;
		OutData.RespawnBedClassPath = CachedData->RespawnBedClassPath;
		OutData.RespawnBedLocation = CachedData->RespawnBedLocation;
		OutData.EquippedItems = CachedData->EquippedItems;
		OutData.WeaponLoadoutSlots = CachedData->WeaponLoadoutSlots;
		OutData.ActiveWeaponQuickSlotIndex = CachedData->ActiveWeaponQuickSlotIndex;
		OutData.OffHandItemClassPath = CachedData->OffHandItemClassPath;

		UE_LOG(LogSpellRisePersistence, Warning,
			TEXT("[Persistence][CollectCharacterDataFallback] Reason=missing_pawn Controller=%s PlayerState=%s SteamId64=%s"),
			*GetNameSafe(Controller),
			*GetNameSafe(SRPlayerState),
			*SteamId64);
	}
	else
	{
		UE_LOG(LogSpellRisePersistence, Warning,
			TEXT("[Persistence][CollectCharacterDataRejected] Reason=missing_pawn_no_cached_transform Controller=%s PlayerState=%s SteamId64=%s"),
			*GetNameSafe(Controller),
			*GetNameSafe(SRPlayerState),
			*SteamId64);
		return false;
	}

	if (const UObject* GameInstanceObject = ResolveGameInstanceObject(Controller))
	{
		bool bCharacterCreatedFromGameInstance = false;
		if (ReadBoolPropertyByName(GameInstanceObject, TEXT("CharacterCreated"), bCharacterCreatedFromGameInstance))
		{
			OutData.bCharacterCreated = bCharacterCreatedFromGameInstance || OutData.bCharacterCreated;
		}

		FString VisualConfigurationJson;
		if (ReadVisualConfigurationJson(GameInstanceObject, VisualConfigurationJson) && IsValidOptionalJsonObjectString(VisualConfigurationJson))
		{
			OutData.VisualConfigurationJson = MoveTemp(VisualConfigurationJson);
		}
	}

	if (!IsFiniteTransform(OutData.CharacterTransform))
	{
		return false;
	}

	if (!OutData.PlayerDisplayName.IsEmpty())
	{
	}

	OutData.Strength = ClampPersistentPrimaryAttribute(ASC->GetNumericAttributeBase(UCombatAttributeSet::GetStrengthAttribute()));
	OutData.Agility = ClampPersistentPrimaryAttribute(ASC->GetNumericAttributeBase(UCombatAttributeSet::GetAgilityAttribute()));
	OutData.Intelligence = ClampPersistentPrimaryAttribute(ASC->GetNumericAttributeBase(UCombatAttributeSet::GetIntelligenceAttribute()));
	OutData.Wisdom = ClampPersistentPrimaryAttribute(ASC->GetNumericAttributeBase(UCombatAttributeSet::GetWisdomAttribute()));

	OutData.Health = ASC->GetNumericAttribute(UResourceAttributeSet::GetHealthAttribute());
	OutData.Mana = ASC->GetNumericAttribute(UResourceAttributeSet::GetManaAttribute());
	OutData.Stamina = ASC->GetNumericAttribute(UResourceAttributeSet::GetStaminaAttribute());

	OutData.CatalystCharge = ASC->GetNumericAttribute(UCatalystAttributeSet::GetCatalystChargeAttribute());
	OutData.CatalystXP = ASC->GetNumericAttribute(UCatalystAttributeSet::GetCatalystXPAttribute());
	OutData.CatalystLevel = ASC->GetNumericAttribute(UCatalystAttributeSet::GetCatalystLevelAttribute());

	if (UActorComponent* TalentComponent = ResolveTalentTreeComponent(Controller))
	{
		CollectTalentTreeData(TalentComponent, OutData.TalentPoints, OutData.Talents);
	}

	CollectProgressionData(
		SRPlayerState,
		OutData.CharacterLevel,
		OutData.Experience,
		OutData.TalentPoints,
		OutData.CraftPoints,
		OutData.AttributePoints,
		OutData.WeaponSkillLevels,
		OutData.SchoolLevels);

	CollectAbilityHotbarData(SRPlayerState, OutData.AbilityHotbarSlots);
	if (Character)
	{
		CollectEquipmentData(
			Character,
			OutData.EquippedItems,
			OutData.WeaponLoadoutSlots,
			OutData.ActiveWeaponQuickSlotIndex,
			OutData.OffHandItemClassPath);
	}

	if (!SRPlayerState->GetRespawnBedActorName().IsEmpty())
	{
		OutData.RespawnBedActorName = SRPlayerState->GetRespawnBedActorName();
		OutData.RespawnBedClassPath = SRPlayerState->GetRespawnBedClassPath();
		OutData.RespawnBedLocation = SRPlayerState->GetRespawnBedLocation();
	}

	TArray<FNarrativeInventoryBinding> Bindings;
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
		return false;
	}

	const FString SteamId64 = ResolveSteamIdFromController(Controller);
	FString ValidationReason;
	if (!ValidateCharacterSaveData(Data, SteamId64, ValidationReason))
	{
		return false;
	}

	ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(Controller->GetPawn());
	ASpellRisePlayerState* SRPlayerState = Cast<ASpellRisePlayerState>(Controller->PlayerState);
	if (!Character || !SRPlayerState)
	{
		return false;
	}

	USpellRiseAbilitySystemComponent* ASC = SRPlayerState->GetSpellRiseASC();
	if (!ASC)
	{
		return false;
	}

	if (!Data.PlayerDisplayName.IsEmpty())
	{
		Controller->PlayerState->SetPlayerName(Data.PlayerDisplayName);
	}

	if (!IsRunningDedicatedServer())
	{
		if (UObject* GameInstanceObject = ResolveGameInstanceObject(Controller))
		{
			WriteBoolPropertyByName(GameInstanceObject, TEXT("CharacterCreated"), Data.bCharacterCreated);
			WriteVisualConfigurationJson(GameInstanceObject, Data.VisualConfigurationJson);
		}
	}

	Character->Archetype = static_cast<ESpellRiseArchetype>(Data.ArchetypeValue);

	ASC->SetNumericAttributeBase(UCombatAttributeSet::GetStrengthAttribute(), ClampPersistentPrimaryAttribute(Data.Strength));
	ASC->SetNumericAttributeBase(UCombatAttributeSet::GetAgilityAttribute(), ClampPersistentPrimaryAttribute(Data.Agility));
	ASC->SetNumericAttributeBase(UCombatAttributeSet::GetIntelligenceAttribute(), ClampPersistentPrimaryAttribute(Data.Intelligence));
	ASC->SetNumericAttributeBase(UCombatAttributeSet::GetWisdomAttribute(), ClampPersistentPrimaryAttribute(Data.Wisdom));

	ASC->SetNumericAttributeBase(UResourceAttributeSet::GetHealthAttribute(), Data.Health);
	ASC->SetNumericAttributeBase(UResourceAttributeSet::GetManaAttribute(), Data.Mana);
	ASC->SetNumericAttributeBase(UResourceAttributeSet::GetStaminaAttribute(), Data.Stamina);

	ASC->SetNumericAttributeBase(UCatalystAttributeSet::GetCatalystChargeAttribute(), Data.CatalystCharge);
	ASC->SetNumericAttributeBase(UCatalystAttributeSet::GetCatalystXPAttribute(), Data.CatalystXP);
	ASC->SetNumericAttributeBase(UCatalystAttributeSet::GetCatalystLevelAttribute(), Data.CatalystLevel);

	ApplyTalentTreeData(Controller, Data);
	ApplyProgressionData(SRPlayerState, Data);

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
		}
		else
		{
		}
	}

	DestroyOwnedEquipmentVisualActors(Character);
	TArray<FNarrativeInventoryBinding> AvailableBindings;
	GatherNarrativeInventoryBindings(SRPlayerState, ESpellRiseSaveOwnerScope::PlayerState, AvailableBindings);
	Character->SyncNarrativeInventoryWeightCapacityFromCarryWeight(TEXT("BeforePersistenceInventoryApply"));

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

	if (Data.SchemaVersion < PersistenceCharacterInventorySchemaVersion)
	{
		SkippedLegacyComponents = Data.InventoryComponents.Num();
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

	Character->SyncNarrativeInventoryWeightCapacityFromCarryWeight(TEXT("AfterPersistenceInventoryApply"));
	ApplyAbilityHotbarData(SRPlayerState, Data.AbilityHotbarSlots);
	ApplyEquipmentData(Character, Data);

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
		return;
	}

	TArray<FNarrativeInventoryBinding> Bindings;
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
			continue;
		}

		Inventory->GiveDefaultItems();

		int32 ResultingStackCount = 0;
		for (UNarrativeItem* Item : Inventory->GetItems())
		{
			if (Item && Item->GetQuantity() > 0)
			{
				++ResultingStackCount;
			}
		}

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
