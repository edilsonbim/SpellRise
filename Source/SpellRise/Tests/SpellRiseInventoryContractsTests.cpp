#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "SpellRise/Inventory/SpellRiseItemTypes.h"
#include "SpellRise/Persistence/SpellRiseInventoryPersistenceMigration.h"
#include "SpellRise/Persistence/SpellRisePersistenceTypes.h"

namespace SpellRiseInventoryTests
{
	constexpr EAutomationTestFlags TestFlags =
		EAutomationTestFlags::EditorContext
		| EAutomationTestFlags::EngineFilter;

	const FString SteamId(TEXT("76561198000000001"));
	const FString SwordClassPath(TEXT("/Game/Items/BPI_TestSword.BPI_TestSword_C"));
	const FString PotionClassPath(TEXT("/Game/Items/BPI_TestPotion.BPI_TestPotion_C"));

	FSpellRiseSavedInventoryComponent MakeLegacyContainer()
	{
		FSpellRiseSavedInventoryComponent Container;
		Container.OwnerScope = static_cast<uint8>(ESpellRiseSaveOwnerScope::PlayerState);
		Container.ContainerRole = static_cast<uint8>(ESpellRiseSaveContainerRole::Inventory);
		Container.ComponentName = TEXT("Inventory");
		Container.Capacity = 20;
		Container.WeightCapacity = 80.0f;
		Container.Currency = 125;

		FSpellRiseSavedNarrativeItem& Sword = Container.Items.AddDefaulted_GetRef();
		Sword.ItemClassPath = SwordClassPath;
		Sword.Quantity = 1;

		FSpellRiseSavedNarrativeItem& Potion = Container.Items.AddDefaulted_GetRef();
		Potion.ItemClassPath = PotionClassPath;
		Potion.Quantity = 7;
		return Container;
	}

	TMap<FString, FPrimaryAssetId> MakeRegistry()
	{
		TMap<FString, FPrimaryAssetId> Registry;
		Registry.Add(SwordClassPath, FPrimaryAssetId(TEXT("SpellRiseItem"), TEXT("TestSword")));
		Registry.Add(PotionClassPath, FPrimaryAssetId(TEXT("SpellRiseItem"), TEXT("TestPotion")));
		return Registry;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSpellRiseInventoryMigrationDeterminismTest,
	"SpellRise.Inventory.Persistence.LegacyMigrationIsDeterministic",
	SpellRiseInventoryTests::TestFlags)

bool FSpellRiseInventoryMigrationDeterminismTest::RunTest(const FString& Parameters)
{
	FSpellRiseInventorySaveData LegacyInventory;
	LegacyInventory.SchemaVersion = SpellRisePersistenceSchema::InventoryLegacy;
	LegacyInventory.SteamId64 = SpellRiseInventoryTests::SteamId;
	LegacyInventory.InventoryComponents.Add(SpellRiseInventoryTests::MakeLegacyContainer());

	FSpellRiseCharacterSaveData LegacyCharacter;
	LegacyCharacter.SchemaVersion = SpellRisePersistenceSchema::CharacterLegacy;
	LegacyCharacter.SteamId64 = SpellRiseInventoryTests::SteamId;
	LegacyCharacter.ActiveWeaponQuickSlotIndex = 2;

	FSpellRiseSavedEquippedItem& Equipped = LegacyCharacter.EquippedItems.AddDefaulted_GetRef();
	Equipped.SlotName = TEXT("MainHand");
	Equipped.ItemClassPath = SpellRiseInventoryTests::SwordClassPath;

	const TMap<FString, FPrimaryAssetId> Registry = SpellRiseInventoryTests::MakeRegistry();
	const FSpellRiseInventoryMigrationResult First =
		FSpellRiseInventoryPersistenceMigration::ConvertLegacyToV3(LegacyInventory, LegacyCharacter, Registry);
	const FSpellRiseInventoryMigrationResult Second =
		FSpellRiseInventoryPersistenceMigration::ConvertLegacyToV3(LegacyInventory, LegacyCharacter, Registry);

	TestTrue(TEXT("Conversão deve concluir"), First.bSucceeded);
	TestTrue(TEXT("Repetição deve concluir"), Second.bSucceeded);
	TestEqual(TEXT("Schema V3"), First.Data.SchemaVersion, SpellRisePersistenceSchema::InventoryItemInstances);
	TestEqual(TEXT("SteamId preservado"), First.Data.SteamId64, SpellRiseInventoryTests::SteamId);
	TestEqual(TEXT("Quick slot preservado"), First.Data.ActiveWeaponQuickSlotIndex, 2);
	TestEqual(TEXT("Quantidade de containers"), First.Data.Containers.Num(), 1);
	TestEqual(TEXT("Quantidade de itens"), First.Data.Containers[0].Items.Num(), 2);
	TestEqual(TEXT("Quantidade equipada"), First.Data.EquippedItems.Num(), 1);

	if (First.Data.Containers.Num() == 1 && Second.Data.Containers.Num() == 1)
	{
		const FSpellRiseSavedInventoryContainerV3& FirstContainer = First.Data.Containers[0];
		const FSpellRiseSavedInventoryContainerV3& SecondContainer = Second.Data.Containers[0];
		TestTrue(TEXT("ContainerId deve ser determinístico"), FirstContainer.ContainerId == SecondContainer.ContainerId);
		TestTrue(TEXT("ContainerId deve ser válido"), FirstContainer.ContainerId.IsValid());

		if (FirstContainer.Items.Num() == 2 && SecondContainer.Items.Num() == 2)
		{
			for (int32 Index = 0; Index < FirstContainer.Items.Num(); ++Index)
			{
				const FSpellRiseSavedItemInstanceV3& FirstItem = FirstContainer.Items[Index];
				const FSpellRiseSavedItemInstanceV3& SecondItem = SecondContainer.Items[Index];
				TestTrue(
					*FString::Printf(TEXT("ItemInstanceId[%d] determinístico"), Index),
					FirstItem.ItemInstanceId == SecondItem.ItemInstanceId);
				TestTrue(
					*FString::Printf(TEXT("ItemInstanceId[%d] válido"), Index),
					FirstItem.ItemInstanceId.IsValid());
				TestTrue(
					*FString::Printf(TEXT("ContainerId[%d] consistente"), Index),
					FirstItem.ContainerId == FirstContainer.ContainerId);
				TestEqual(
					*FString::Printf(TEXT("Slot[%d] determinístico"), Index),
					FirstItem.SlotIndex,
					Index);
			}

			TestTrue(
				TEXT("Equipamento deve referenciar a instância migrada"),
				First.Data.EquippedItems[0].ItemInstanceId == FirstContainer.Items[0].ItemInstanceId);
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSpellRiseInventoryMigrationValidationTest,
	"SpellRise.Inventory.Persistence.LegacyMigrationValidatesRegistryAndAmbiguity",
	SpellRiseInventoryTests::TestFlags)

bool FSpellRiseInventoryMigrationValidationTest::RunTest(const FString& Parameters)
{
	FSpellRiseInventorySaveData AmbiguousInventory;
	AmbiguousInventory.SteamId64 = SpellRiseInventoryTests::SteamId;
	FSpellRiseSavedInventoryComponent Container = SpellRiseInventoryTests::MakeLegacyContainer();
	Container.Items.Add(Container.Items[0]);
	AmbiguousInventory.InventoryComponents.Add(Container);

	FSpellRiseCharacterSaveData Character;
	Character.SteamId64 = SpellRiseInventoryTests::SteamId;
	FSpellRiseSavedEquippedItem& MainHand = Character.EquippedItems.AddDefaulted_GetRef();
	MainHand.SlotName = TEXT("MainHand");
	MainHand.ItemClassPath = SpellRiseInventoryTests::SwordClassPath.ToUpper();

	const FSpellRiseInventoryMigrationResult Ambiguous =
		FSpellRiseInventoryPersistenceMigration::ConvertLegacyToV3(
			AmbiguousInventory,
			Character,
			SpellRiseInventoryTests::MakeRegistry());

	TestTrue(TEXT("Ambiguidade resolvível deve concluir"), Ambiguous.bSucceeded);
	TestEqual(TEXT("Equipamento deve escolher uma instância"), Ambiguous.Data.EquippedItems.Num(), 1);
	TestEqual(TEXT("Deve registrar aviso de ambiguidade"), Ambiguous.Warnings.Num(), 1);
	if (Ambiguous.Warnings.Num() == 1)
	{
		TestTrue(
			TEXT("Aviso deve identificar escolha determinística"),
			Ambiguous.Warnings[0].StartsWith(TEXT("equipment_item_ambiguous_deterministic_choice:")));
	}

	FSpellRiseInventorySaveData UnmappedInventory;
	UnmappedInventory.SteamId64 = SpellRiseInventoryTests::SteamId;
	FSpellRiseSavedInventoryComponent UnmappedContainer;
	FSpellRiseSavedNarrativeItem& UnmappedItem = UnmappedContainer.Items.AddDefaulted_GetRef();
	UnmappedItem.ItemClassPath = TEXT("/Game/Items/BPI_Unmapped.BPI_Unmapped_C");
	UnmappedItem.Quantity = 1;
	UnmappedInventory.InventoryComponents.Add(UnmappedContainer);

	const FSpellRiseInventoryMigrationResult Unmapped =
		FSpellRiseInventoryPersistenceMigration::ConvertLegacyToV3(
			UnmappedInventory,
			Character,
			SpellRiseInventoryTests::MakeRegistry());

	TestFalse(TEXT("Item sem mapping deve bloquear migração"), Unmapped.bSucceeded);
	TestEqual(TEXT("Deve retornar um erro de mapping"), Unmapped.Errors.Num(), 2);
	TestTrue(
		TEXT("Primeiro erro deve identificar classe não mapeada"),
		Unmapped.Errors.Num() > 0
			&& Unmapped.Errors[0].StartsWith(TEXT("unmapped_item_class:")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSpellRiseInventoryPureContractsTest,
	"SpellRise.Inventory.Contracts.PureTypesHaveSafeDefaults",
	SpellRiseInventoryTests::TestFlags)

bool FSpellRiseInventoryPureContractsTest::RunTest(const FString& Parameters)
{
	const FSpellRiseItemInstance Item;
	TestFalse(TEXT("Item default não possui identidade"), Item.ItemInstanceId.IsValid());
	TestFalse(TEXT("Item default não possui definição"), Item.DefinitionId.IsValid());
	TestEqual(TEXT("Slot default inválido"), Item.SlotIndex, INDEX_NONE);
	TestEqual(TEXT("Quantidade default zero"), Item.Quantity, 0);
	TestEqual(TEXT("Durabilidade default zero"), Item.Durability, 0);
	TestEqual(TEXT("Flags default zero"), Item.Flags, static_cast<uint8>(0));
	TestEqual(TEXT("Revisão default zero"), Item.Revision, 0);

	const FSpellRiseInventoryList List;
	TestEqual(TEXT("FastArray default vazio"), List.Entries.Num(), 0);

	const FSpellRiseSavedItemInstanceV3 SavedItem;
	TestFalse(TEXT("Persistência default sem GUID"), SavedItem.ItemInstanceId.IsValid());
	TestFalse(TEXT("Persistência default sem DefinitionId"), SavedItem.DefinitionId.IsValid());
	TestEqual(TEXT("Persistência default sem slot"), SavedItem.SlotIndex, INDEX_NONE);

	const FSpellRiseInventorySaveDataV3 SaveData;
	TestEqual(TEXT("SaveData usa schema V3"), SaveData.SchemaVersion, SpellRisePersistenceSchema::InventoryItemInstances);
	TestEqual(TEXT("SaveData inicia sem containers"), SaveData.Containers.Num(), 0);
	TestEqual(TEXT("SaveData inicia sem equipamento"), SaveData.EquippedItems.Num(), 0);
	return true;
}

#endif
