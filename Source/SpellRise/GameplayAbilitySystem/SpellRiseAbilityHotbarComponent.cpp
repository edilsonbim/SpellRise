#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityHotbarComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UnrealType.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRise/GameplayAbilitySystem/Data/SpellRiseAbilityDefinition.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseAbilityHotbar, Log, All);

namespace SpellRiseAbilityHotbarPrivate
{
	constexpr int32 WeaponSlotCount = 8;
	constexpr int32 CommonSlotCount = 8;
	constexpr int32 TotalSlotCount = WeaponSlotCount + CommonSlotCount;

	bool IsValidSlotIndex(const int32 SlotIndex)
	{
		return SlotIndex >= 0 && SlotIndex < TotalSlotCount;
	}

	bool IsAllowedInputTag(const FGameplayTag& Tag)
	{
		return !Tag.IsValid() || Tag.ToString().StartsWith(TEXT("InputTag.Ability."));
	}

	ESpellRiseAbilityHotbarGroup ResolveGroup(const int32 SlotIndex)
	{
		return SlotIndex < WeaponSlotCount
			? ESpellRiseAbilityHotbarGroup::Weapon
			: ESpellRiseAbilityHotbarGroup::Common;
	}

	int32 ResolveGroupSlotIndex(const int32 SlotIndex)
	{
		return ResolveGroup(SlotIndex) == ESpellRiseAbilityHotbarGroup::Common
			? SlotIndex - WeaponSlotCount
			: SlotIndex;
	}

	FGameplayTag DefaultInputTagForSlot(const int32 SlotIndex)
	{
		static const TCHAR* Names[TotalSlotCount] =
		{
			TEXT("InputTag.Ability.Skill1"),
			TEXT("InputTag.Ability.Skill2"),
			TEXT("InputTag.Ability.Skill3"),
			TEXT("InputTag.Ability.Skill4"),
			TEXT("InputTag.Ability.Skill5"),
			TEXT("InputTag.Ability.Skill6"),
			TEXT("InputTag.Ability.Skill7"),
			TEXT("InputTag.Ability.Skill8"),
			TEXT("InputTag.Ability.Common1"),
			TEXT("InputTag.Ability.Common2"),
			TEXT("InputTag.Ability.Common3"),
			TEXT("InputTag.Ability.Common4"),
			TEXT("InputTag.Ability.Common5"),
			TEXT("InputTag.Ability.Common6"),
			TEXT("InputTag.Ability.Common7"),
			TEXT("InputTag.Ability.Common8")
		};

		return IsValidSlotIndex(SlotIndex)
			? FGameplayTag::RequestGameplayTag(Names[SlotIndex], false)
			: FGameplayTag();
	}

	UObject* ReadFirstObjectPropertyByNames(UObject* SourceObject, const TArray<FName>& PropertyNames)
	{
		if (!SourceObject || !SourceObject->GetClass())
		{
			return nullptr;
		}

		for (const FName PropertyName : PropertyNames)
		{
			if (const FObjectPropertyBase* ObjectProperty = FindFProperty<FObjectPropertyBase>(SourceObject->GetClass(), PropertyName))
			{
				return ObjectProperty->GetObjectPropertyValue_InContainer(SourceObject);
			}
		}

		return nullptr;
	}

	UObject* ResolveAbilityDefinitionObject(TSoftObjectPtr<UObject> AbilityDefinitionOrSource)
	{
		UObject* LoadedObject = AbilityDefinitionOrSource.LoadSynchronous();
		if (!LoadedObject)
		{
			return nullptr;
		}

		if (UObject* NestedAbilityDefinition = ReadFirstObjectPropertyByNames(LoadedObject, { TEXT("AbilityDefinition") }))
		{
			return NestedAbilityDefinition;
		}

		return LoadedObject;
	}

	UTexture2D* ResolveAbilityDefinitionIcon(UObject* AbilityDefinition)
	{
		if (const USpellRiseAbilityDefinition* TypedDefinition = Cast<USpellRiseAbilityDefinition>(AbilityDefinition))
		{
			return TypedDefinition->Icon;
		}

		return Cast<UTexture2D>(ReadFirstObjectPropertyByNames(AbilityDefinition, {
			TEXT("Icon"),
		}));
	}

	UTexture2D* ResolveAbilityDefinitionActiveIcon(UObject* AbilityDefinition)
	{
		if (const USpellRiseAbilityDefinition* TypedDefinition = Cast<USpellRiseAbilityDefinition>(AbilityDefinition))
		{
			return TypedDefinition->ActiveIcon;
		}

		return Cast<UTexture2D>(ReadFirstObjectPropertyByNames(AbilityDefinition, {
			TEXT("ActiveIcon"),
		}));
	}

	FText ReadFirstTextPropertyByNames(UObject* SourceObject, const TArray<FName>& PropertyNames)
	{
		if (!SourceObject || !SourceObject->GetClass())
		{
			return FText::GetEmpty();
		}

		for (const FName PropertyName : PropertyNames)
		{
			if (const FTextProperty* TextProperty = FindFProperty<FTextProperty>(SourceObject->GetClass(), PropertyName))
			{
				return TextProperty->GetPropertyValue_InContainer(SourceObject);
			}

			if (const FStrProperty* StringProperty = FindFProperty<FStrProperty>(SourceObject->GetClass(), PropertyName))
			{
				const FString Value = StringProperty->GetPropertyValue_InContainer(SourceObject).TrimStartAndEnd();
				if (!Value.IsEmpty())
				{
					return FText::FromString(Value);
				}
			}

			if (const FNameProperty* NameProperty = FindFProperty<FNameProperty>(SourceObject->GetClass(), PropertyName))
			{
				const FName Value = NameProperty->GetPropertyValue_InContainer(SourceObject);
				if (!Value.IsNone())
				{
					return FText::FromName(Value);
				}
			}
		}

		return FText::GetEmpty();
	}

	FText ResolveAbilityDefinitionDisplayName(UObject* AbilityDefinition)
	{
		if (const USpellRiseAbilityDefinition* TypedDefinition = Cast<USpellRiseAbilityDefinition>(AbilityDefinition))
		{
			if (!TypedDefinition->DisplayName.IsEmpty())
			{
				return TypedDefinition->DisplayName;
			}

			if (!TypedDefinition->DefinitionType.IsNone())
			{
				return FText::FromName(TypedDefinition->DefinitionType);
			}
		}

		return ReadFirstTextPropertyByNames(AbilityDefinition, {
			TEXT("DisplayName"),
			TEXT("AbilityName"),
			TEXT("TalentName"),
			TEXT("Name")
		});
	}

	bool ExtractFirstAbilityClassFromDefinition(UObject* AbilityDefinition, TSubclassOf<UGameplayAbility>& OutAbilityClass)
	{
		OutAbilityClass = nullptr;
		if (!AbilityDefinition || !AbilityDefinition->GetClass())
		{
			return false;
		}

		if (const USpellRiseAbilityDefinition* TypedDefinition = Cast<USpellRiseAbilityDefinition>(AbilityDefinition))
		{
			if (TypedDefinition->AbilitiesToGrant.Num() <= 0)
			{
				return false;
			}

			OutAbilityClass = TypedDefinition->AbilitiesToGrant[0].AbilityClass.LoadSynchronous();
			return OutAbilityClass != nullptr;
		}

		const FArrayProperty* AbilitiesProperty = FindFProperty<FArrayProperty>(AbilityDefinition->GetClass(), TEXT("AbilitiesToGrant"));
		const FStructProperty* InnerStructProperty = AbilitiesProperty ? CastField<FStructProperty>(AbilitiesProperty->Inner) : nullptr;
		if (!AbilitiesProperty || !InnerStructProperty || InnerStructProperty->Struct != FSpellRiseGrantedAbility::StaticStruct())
		{
			return false;
		}

		FScriptArrayHelper ArrayHelper(AbilitiesProperty, AbilitiesProperty->ContainerPtrToValuePtr<void>(AbilityDefinition));
		if (ArrayHelper.Num() <= 0)
		{
			return false;
		}

		const FSpellRiseGrantedAbility* AbilityGrant = reinterpret_cast<const FSpellRiseGrantedAbility*>(ArrayHelper.GetRawPtr(0));
		if (!AbilityGrant)
		{
			return false;
		}

		OutAbilityClass = AbilityGrant->AbilityClass.LoadSynchronous();
		return OutAbilityClass != nullptr;
	}

	bool ResolveAbilityDefinitionSlotGroup(UObject* AbilityDefinition, ESpellRiseAbilityHotbarGroup& OutGroup, bool& bOutExplicitlyNotHotbar)
	{
		bOutExplicitlyNotHotbar = false;
		if (!AbilityDefinition || !AbilityDefinition->GetClass())
		{
			return false;
		}

		if (const USpellRiseAbilityDefinition* TypedDefinition = Cast<USpellRiseAbilityDefinition>(AbilityDefinition))
		{
			if (TypedDefinition->SlotGroup == ESpellRiseAbilityDefinitionSlotGroup::None)
			{
				bOutExplicitlyNotHotbar = true;
				return false;
			}

			OutGroup = TypedDefinition->SlotGroup == ESpellRiseAbilityDefinitionSlotGroup::Weapon
				? ESpellRiseAbilityHotbarGroup::Weapon
				: ESpellRiseAbilityHotbarGroup::Common;
			return true;
		}

		const FProperty* SlotGroupProperty = FindFProperty<FProperty>(AbilityDefinition->GetClass(), TEXT("SlotGroup"));
		if (!SlotGroupProperty)
		{
			return false;
		}

		FString Value;
		if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(SlotGroupProperty))
		{
			const int64 IntValue = EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(
				EnumProperty->ContainerPtrToValuePtr<void>(AbilityDefinition));
			Value = EnumProperty->GetEnum()->GetNameStringByValue(IntValue);
		}
		else if (const FByteProperty* ByteProperty = CastField<FByteProperty>(SlotGroupProperty))
		{
			const uint8 IntValue = ByteProperty->GetPropertyValue_InContainer(AbilityDefinition);
			Value = ByteProperty->Enum
				? ByteProperty->Enum->GetNameStringByValue(IntValue)
				: FString::FromInt(IntValue);
		}

		if (Value.Equals(TEXT("Weapon"), ESearchCase::IgnoreCase)
			|| Value.EndsWith(TEXT("::Weapon"), ESearchCase::IgnoreCase))
		{
			OutGroup = ESpellRiseAbilityHotbarGroup::Weapon;
			return true;
		}

		if (Value.Equals(TEXT("Common"), ESearchCase::IgnoreCase)
			|| Value.EndsWith(TEXT("::Common"), ESearchCase::IgnoreCase))
		{
			OutGroup = ESpellRiseAbilityHotbarGroup::Common;
			return true;
		}

		return false;
	}

	void CaptureAbilityRuntimeState(
		const UObject* WorldContextObject,
		const USpellRiseAbilitySystemComponent* ASC,
		TSubclassOf<UGameplayAbility> AbilityClass,
		bool& bOutIsGranted,
		bool& bOutIsActive,
		float& OutCooldownTimeRemaining,
		float& OutCooldownDuration,
		float& OutCooldownPercent)
	{
		bOutIsGranted = false;
		bOutIsActive = false;
		OutCooldownTimeRemaining = 0.f;
		OutCooldownDuration = 0.f;
		OutCooldownPercent = 0.f;

		if (!ASC || !AbilityClass)
		{
			return;
		}

		const FGameplayAbilityActorInfo* ActorInfo = ASC->AbilityActorInfo.Get();
		const UGameplayAbility* CooldownAbility = Cast<UGameplayAbility>(AbilityClass->GetDefaultObject());
		FGameplayAbilitySpecHandle AbilityHandle;

		for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (!Spec.Ability || Spec.Ability->GetClass() != AbilityClass)
			{
				continue;
			}

			bOutIsGranted = true;
			bOutIsActive = Spec.IsActive();
			CooldownAbility = Spec.Ability;
			AbilityHandle = Spec.Handle;
			break;
		}

		if (!CooldownAbility)
		{
			return;
		}

		FGameplayTagContainer SpecificCooldownTags;
		const FGameplayTag CooldownRootTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown"), false);
		const FGameplayTagContainer* CooldownTags = CooldownAbility->GetCooldownTags();
		if (CooldownTags && CooldownTags->Num() > 0)
		{
			for (const FGameplayTag& CooldownTag : *CooldownTags)
			{
				if (CooldownRootTag.IsValid() && CooldownTag == CooldownRootTag)
				{
					continue;
				}

				SpecificCooldownTags.AddTag(CooldownTag);
			}
		}

		float TimeRemaining = 0.f;
		float Duration = 0.f;
		if (SpecificCooldownTags.Num() > 0)
		{
			const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(SpecificCooldownTags);
			const TArray<TPair<float, float>> DurationAndTimeRemaining = ASC->GetActiveEffectsTimeRemainingAndDuration(Query);
			for (const TPair<float, float>& CooldownPair : DurationAndTimeRemaining)
			{
				if (CooldownPair.Key > TimeRemaining)
				{
					TimeRemaining = CooldownPair.Key;
					Duration = CooldownPair.Value;
				}
			}
		}
		else if (ActorInfo && AbilityHandle.IsValid())
		{
			CooldownAbility->GetCooldownTimeRemainingAndDuration(AbilityHandle, ActorInfo, TimeRemaining, Duration);
		}

		OutCooldownTimeRemaining = FMath::Max(0.f, TimeRemaining);
		OutCooldownDuration = FMath::Max(0.f, Duration);
		if (OutCooldownDuration > KINDA_SMALL_NUMBER && OutCooldownTimeRemaining > KINDA_SMALL_NUMBER)
		{
			OutCooldownPercent = FMath::Clamp(OutCooldownTimeRemaining / OutCooldownDuration, 0.f, 1.f);
		}
	}
}

USpellRiseAbilityHotbarComponent::USpellRiseAbilityHotbarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void USpellRiseAbilityHotbarComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeDefaultSlots();
}

void USpellRiseAbilityHotbarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(
		USpellRiseAbilityHotbarComponent,
		Slots,
		COND_OwnerOnly,
		REPNOTIFY_Always);
}

bool USpellRiseAbilityHotbarComponent::GetSlot(const int32 SlotIndex, FSpellRiseAbilityHotbarSlot& OutSlot) const
{
	if (!SpellRiseAbilityHotbarPrivate::IsValidSlotIndex(SlotIndex) || !Slots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	OutSlot = Slots[SlotIndex];
	return true;
}

bool USpellRiseAbilityHotbarComponent::GetSlotViewData(
	const int32 SlotIndex,
	FSpellRiseAbilityHotbarSlotViewData& OutViewData) const
{
	FSpellRiseAbilityHotbarSlot Slot;
	if (!GetSlot(SlotIndex, Slot))
	{
		return false;
	}

	OutViewData = FSpellRiseAbilityHotbarSlotViewData();
	OutViewData.SlotIndex = Slot.SlotIndex;
	OutViewData.GroupSlotIndex = SpellRiseAbilityHotbarPrivate::ResolveGroupSlotIndex(Slot.SlotIndex);
	OutViewData.Group = Slot.Group;
	OutViewData.AbilityInputTag = Slot.AbilityInputTag;
	OutViewData.AbilityClass = Slot.AbilityClass;
	OutViewData.bLockedByServer = Slot.bLockedByServer;

	UObject* LoadedAbilityDefinition = Slot.AbilityDefinition.LoadSynchronous();
	if (LoadedAbilityDefinition)
	{
		OutViewData.DisplayName = SpellRiseAbilityHotbarPrivate::ResolveAbilityDefinitionDisplayName(LoadedAbilityDefinition);
		OutViewData.Icon = SpellRiseAbilityHotbarPrivate::ResolveAbilityDefinitionIcon(LoadedAbilityDefinition);
		OutViewData.ActiveIcon = SpellRiseAbilityHotbarPrivate::ResolveAbilityDefinitionActiveIcon(LoadedAbilityDefinition);
	}

	UClass* LoadedAbilityClass = Slot.AbilityClass.LoadSynchronous();
	OutViewData.bHasAbility = LoadedAbilityClass != nullptr;
	if (!LoadedAbilityClass)
	{
		return true;
	}

	const USpellRiseGameplayAbility* AbilityCDO = Cast<USpellRiseGameplayAbility>(LoadedAbilityClass->GetDefaultObject());
	if (AbilityCDO)
	{
		if (OutViewData.DisplayName.IsEmpty())
		{
			OutViewData.DisplayName = AbilityCDO->AbilityDisplayName;
		}
		if (!OutViewData.Icon)
		{
			OutViewData.Icon = AbilityCDO->AbilityIcon;
		}
	}

	if (OutViewData.DisplayName.IsEmpty())
	{
		FString AbilityClassName = LoadedAbilityClass->GetName();
		AbilityClassName.RemoveFromEnd(TEXT("_C"));
		OutViewData.DisplayName = FText::FromString(FName::NameToDisplayString(AbilityClassName, false));
	}

	SpellRiseAbilityHotbarPrivate::CaptureAbilityRuntimeState(
		this,
		ResolveASC(),
		LoadedAbilityClass,
		OutViewData.bIsGranted,
		OutViewData.bIsActive,
		OutViewData.CooldownTimeRemaining,
		OutViewData.CooldownDuration,
		OutViewData.CooldownPercent);

	return true;
}

TArray<FSpellRiseAbilityHotbarSlotViewData> USpellRiseAbilityHotbarComponent::GetSlotViewDataForGroup(
	const ESpellRiseAbilityHotbarGroup Group) const
{
	TArray<FSpellRiseAbilityHotbarSlotViewData> ViewData;
	ViewData.Reserve(SpellRiseAbilityHotbarPrivate::WeaponSlotCount);

	const int32 FirstSlotIndex = Group == ESpellRiseAbilityHotbarGroup::Common
		? SpellRiseAbilityHotbarPrivate::WeaponSlotCount
		: 0;
	const int32 LastSlotIndexExclusive = FirstSlotIndex + SpellRiseAbilityHotbarPrivate::WeaponSlotCount;

	for (int32 SlotIndex = FirstSlotIndex; SlotIndex < LastSlotIndexExclusive; ++SlotIndex)
	{
		FSpellRiseAbilityHotbarSlotViewData SlotViewData;
		if (GetSlotViewData(SlotIndex, SlotViewData))
		{
			ViewData.Add(SlotViewData);
		}
	}

	return ViewData;
}

bool USpellRiseAbilityHotbarComponent::RequestSetSlot(
	const int32 SlotIndex,
	const FGameplayTag AbilityInputTag,
	const TSubclassOf<UGameplayAbility> AbilityClass)
{
	FString RejectReason;
	if (!ValidateSlotPayload(SlotIndex, AbilityInputTag, AbilityClass, RejectReason))
	{
		AuditRejectedHotbarRpc(TEXT("RequestSetSlot"), RejectReason);
		return false;
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		const bool bChanged = SetSlot_Server(SlotIndex, AbilityInputTag, AbilityClass, RejectReason);
		if (!bChanged && !RejectReason.IsEmpty())
		{
			AuditRejectedHotbarRpc(TEXT("RequestSetSlot"), RejectReason);
		}
		return bChanged;
	}

	ServerSetSlot(SlotIndex, AbilityInputTag, AbilityClass);
	return true;
}

bool USpellRiseAbilityHotbarComponent::RequestSetSlotFromAbilityDefinition(
	const int32 SlotIndex,
	UObject* AbilityDefinition)
{
	return RequestSetSlotFromAbilityDefinitionSoft(SlotIndex, AbilityDefinition);
}

bool USpellRiseAbilityHotbarComponent::RequestSetSlotFromAbilityDefinitionSoft(
	const int32 SlotIndex,
	TSoftObjectPtr<UObject> AbilityDefinition)
{
	TSubclassOf<UGameplayAbility> AbilityClass = nullptr;
	FString RejectReason;
	if (!ValidateAbilityDefinitionPayload(SlotIndex, AbilityDefinition, AbilityClass, RejectReason))
	{
		AuditRejectedHotbarRpc(TEXT("RequestSetSlotFromAbilityDefinition"), RejectReason);
		return false;
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		const bool bChanged = SetSlotFromAbilityDefinition_Server(SlotIndex, AbilityDefinition, RejectReason);
		if (!bChanged && !RejectReason.IsEmpty())
		{
			AuditRejectedHotbarRpc(TEXT("RequestSetSlotFromAbilityDefinition"), RejectReason);
		}
		return bChanged;
	}

	ServerSetSlotFromAbilityDefinition(SlotIndex, AbilityDefinition);
	return true;
}

bool USpellRiseAbilityHotbarComponent::RequestClearSlot(const int32 SlotIndex)
{
	if (!SpellRiseAbilityHotbarPrivate::IsValidSlotIndex(SlotIndex))
	{
		AuditRejectedHotbarRpc(TEXT("RequestClearSlot"), TEXT("invalid_slot_index"));
		return false;
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FString RejectReason;
		const bool bChanged = ClearSlot_Server(SlotIndex, RejectReason);
		if (!bChanged && !RejectReason.IsEmpty())
		{
			AuditRejectedHotbarRpc(TEXT("RequestClearSlot"), RejectReason);
		}
		return bChanged;
	}

	ServerClearSlot(SlotIndex);
	return true;
}

void USpellRiseAbilityHotbarComponent::AbilitySlotPressed(const int32 SlotIndex)
{
	FSpellRiseAbilityHotbarSlot Slot;
	if (!GetSlot(SlotIndex, Slot))
	{
		return;
	}

	if (USpellRiseAbilitySystemComponent* ASC = ResolveASC())
	{
		if (UClass* AbilityClass = Slot.AbilityClass.LoadSynchronous())
		{
			ASC->SR_AbilityInputClassPressed(AbilityClass);
			return;
		}

		if (Slot.AbilityInputTag.IsValid())
		{
			ASC->SR_AbilityInputTagPressed(Slot.AbilityInputTag);
		}
	}
}

void USpellRiseAbilityHotbarComponent::AbilitySlotReleased(const int32 SlotIndex)
{
	FSpellRiseAbilityHotbarSlot Slot;
	if (!GetSlot(SlotIndex, Slot))
	{
		return;
	}

	if (USpellRiseAbilitySystemComponent* ASC = ResolveASC())
	{
		if (UClass* AbilityClass = Slot.AbilityClass.LoadSynchronous())
		{
			ASC->SR_AbilityInputClassReleased(AbilityClass);
			return;
		}

		if (Slot.AbilityInputTag.IsValid())
		{
			ASC->SR_AbilityInputTagReleased(Slot.AbilityInputTag);
		}
	}
}

int32 USpellRiseAbilityHotbarComponent::MakeSlotIndex(
	const ESpellRiseAbilityHotbarGroup Group,
	const int32 GroupSlotIndex)
{
	if (GroupSlotIndex < 0 || GroupSlotIndex >= SpellRiseAbilityHotbarPrivate::WeaponSlotCount)
	{
		return INDEX_NONE;
	}

	return Group == ESpellRiseAbilityHotbarGroup::Common
		? SpellRiseAbilityHotbarPrivate::WeaponSlotCount + GroupSlotIndex
		: GroupSlotIndex;
}

void USpellRiseAbilityHotbarComponent::ServerSetSlot_Implementation(
	const int32 SlotIndex,
	const FGameplayTag AbilityInputTag,
	const TSubclassOf<UGameplayAbility> AbilityClass)
{
	FString RejectReason;
	if (!CheckServerRateLimit(RejectReason)
		|| !SetSlot_Server(SlotIndex, AbilityInputTag, AbilityClass, RejectReason))
	{
		AuditRejectedHotbarRpc(TEXT("ServerSetSlot"), RejectReason);
	}
}

void USpellRiseAbilityHotbarComponent::ServerSetSlotFromAbilityDefinition_Implementation(
	const int32 SlotIndex,
	const TSoftObjectPtr<UObject>& AbilityDefinition)
{
	FString RejectReason;
	if (!CheckServerRateLimit(RejectReason)
		|| !SetSlotFromAbilityDefinition_Server(SlotIndex, AbilityDefinition, RejectReason))
	{
		AuditRejectedHotbarRpc(TEXT("ServerSetSlotFromAbilityDefinition"), RejectReason);
	}
}

void USpellRiseAbilityHotbarComponent::ServerClearSlot_Implementation(const int32 SlotIndex)
{
	FString RejectReason;
	if (!CheckServerRateLimit(RejectReason)
		|| !ClearSlot_Server(SlotIndex, RejectReason))
	{
		AuditRejectedHotbarRpc(TEXT("ServerClearSlot"), RejectReason);
	}
}

void USpellRiseAbilityHotbarComponent::OnRep_Slots()
{
	OnHotbarChanged.Broadcast();
}

void USpellRiseAbilityHotbarComponent::InitializeDefaultSlots()
{
	if (Slots.Num() == SpellRiseAbilityHotbarPrivate::TotalSlotCount)
	{
		return;
	}

	Slots.Reset(SpellRiseAbilityHotbarPrivate::TotalSlotCount);
	for (int32 SlotIndex = 0; SlotIndex < SpellRiseAbilityHotbarPrivate::TotalSlotCount; ++SlotIndex)
	{
		FSpellRiseAbilityHotbarSlot Slot;
		Slot.SlotIndex = SlotIndex;
		Slot.Group = SpellRiseAbilityHotbarPrivate::ResolveGroup(SlotIndex);
		Slot.AbilityInputTag = SpellRiseAbilityHotbarPrivate::DefaultInputTagForSlot(SlotIndex);
		Slots.Add(Slot);
	}

	OnHotbarChanged.Broadcast();
}

bool USpellRiseAbilityHotbarComponent::SetSlot_Server(
	const int32 SlotIndex,
	const FGameplayTag AbilityInputTag,
	const TSubclassOf<UGameplayAbility> AbilityClass,
	FString& OutRejectReason)
{
	InitializeDefaultSlots();

	if (!ValidateSlotPayload(SlotIndex, AbilityInputTag, AbilityClass, OutRejectReason))
	{
		return false;
	}

	FSpellRiseAbilityHotbarSlot& Slot = Slots[SlotIndex];
	if (Slot.bLockedByServer)
	{
		OutRejectReason = TEXT("slot_locked_by_server");
		return false;
	}

	Slot.AbilityInputTag = AbilityInputTag;
	Slot.AbilityClass = AbilityClass;
	Slot.AbilityDefinition.Reset();
	OnHotbarChanged.Broadcast();

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}

	return true;
}

bool USpellRiseAbilityHotbarComponent::SetSlotFromAbilityDefinition_Server(
	const int32 SlotIndex,
	TSoftObjectPtr<UObject> AbilityDefinition,
	FString& OutRejectReason)
{
	InitializeDefaultSlots();

	TSubclassOf<UGameplayAbility> AbilityClass = nullptr;
	if (!ValidateAbilityDefinitionPayload(SlotIndex, AbilityDefinition, AbilityClass, OutRejectReason))
	{
		return false;
	}

	FSpellRiseAbilityHotbarSlot& Slot = Slots[SlotIndex];
	if (Slot.bLockedByServer)
	{
		OutRejectReason = TEXT("slot_locked_by_server");
		return false;
	}

	UObject* LoadedAbilityDefinition = SpellRiseAbilityHotbarPrivate::ResolveAbilityDefinitionObject(AbilityDefinition);
	if (!LoadedAbilityDefinition)
	{
		OutRejectReason = TEXT("definition_load_failed_after_validation");
		return false;
	}

	Slot.AbilityClass = AbilityClass;
	Slot.AbilityDefinition = LoadedAbilityDefinition;
	OnHotbarChanged.Broadcast();

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}

	return true;
}

bool USpellRiseAbilityHotbarComponent::ClearSlot_Server(const int32 SlotIndex, FString& OutRejectReason)
{
	InitializeDefaultSlots();

	if (!SpellRiseAbilityHotbarPrivate::IsValidSlotIndex(SlotIndex) || !Slots.IsValidIndex(SlotIndex))
	{
		OutRejectReason = TEXT("invalid_slot_index");
		return false;
	}

	FSpellRiseAbilityHotbarSlot& Slot = Slots[SlotIndex];
	if (Slot.bLockedByServer)
	{
		OutRejectReason = TEXT("slot_locked_by_server");
		return false;
	}

	Slot.AbilityInputTag = FGameplayTag();
	Slot.AbilityClass.Reset();
	Slot.AbilityDefinition.Reset();
	OnHotbarChanged.Broadcast();

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}

	return true;
}

bool USpellRiseAbilityHotbarComponent::ValidateSlotPayload(
	const int32 SlotIndex,
	const FGameplayTag AbilityInputTag,
	const TSubclassOf<UGameplayAbility> AbilityClass,
	FString& OutRejectReason) const
{
	if (!SpellRiseAbilityHotbarPrivate::IsValidSlotIndex(SlotIndex))
	{
		OutRejectReason = TEXT("invalid_slot_index");
		return false;
	}

	if (!SpellRiseAbilityHotbarPrivate::IsAllowedInputTag(AbilityInputTag))
	{
		OutRejectReason = FString::Printf(TEXT("invalid_input_tag(%s)"), *AbilityInputTag.ToString());
		return false;
	}

	if (AbilityClass)
	{
		const USpellRiseGameplayAbility* AbilityCDO = Cast<USpellRiseGameplayAbility>(AbilityClass->GetDefaultObject());
		const ESpellRiseAbilityHotbarGroup SlotGroup = SpellRiseAbilityHotbarPrivate::ResolveGroup(SlotIndex);
		if (AbilityCDO)
		{
			const bool bAbilityIsWeapon = AbilityCDO->AbilitySlotGroup == ESpellRiseAbilitySlotGroup::Weapon;
			if (SlotGroup == ESpellRiseAbilityHotbarGroup::Weapon && !bAbilityIsWeapon)
			{
				OutRejectReason = TEXT("common_ability_in_weapon_slot");
				return false;
			}

			if (SlotGroup == ESpellRiseAbilityHotbarGroup::Common && bAbilityIsWeapon)
			{
				OutRejectReason = TEXT("weapon_ability_in_common_slot");
				return false;
			}
		}
	}

	return true;
}

bool USpellRiseAbilityHotbarComponent::ValidateAbilityDefinitionPayload(
	const int32 SlotIndex,
	TSoftObjectPtr<UObject> AbilityDefinition,
	TSubclassOf<UGameplayAbility>& OutAbilityClass,
	FString& OutRejectReason) const
{
	OutAbilityClass = nullptr;
	if (!SpellRiseAbilityHotbarPrivate::IsValidSlotIndex(SlotIndex))
	{
		OutRejectReason = TEXT("invalid_slot_index");
		return false;
	}

	if (AbilityDefinition.IsNull())
	{
		OutRejectReason = TEXT("missing_ability_definition");
		return false;
	}

	UObject* LoadedAbilityDefinition = SpellRiseAbilityHotbarPrivate::ResolveAbilityDefinitionObject(AbilityDefinition);
	if (!LoadedAbilityDefinition)
	{
		OutRejectReason = FString::Printf(
			TEXT("definition_load_failed(%s)"),
			*AbilityDefinition.ToSoftObjectPath().ToString());
		return false;
	}

	const FString DefinitionPath = LoadedAbilityDefinition->GetPathName();
	if (!DefinitionPath.StartsWith(TEXT("/Game/")))
	{
		OutRejectReason = FString::Printf(TEXT("invalid_definition_path(%s)"), *DefinitionPath);
		return false;
	}

	if (!SpellRiseAbilityHotbarPrivate::ExtractFirstAbilityClassFromDefinition(LoadedAbilityDefinition, OutAbilityClass))
	{
		OutRejectReason = TEXT("definition_missing_granted_ability");
		return false;
	}

	if (!ValidateSlotPayload(SlotIndex, SpellRiseAbilityHotbarPrivate::DefaultInputTagForSlot(SlotIndex), OutAbilityClass, OutRejectReason))
	{
		OutRejectReason = FString::Printf(TEXT("definition_ability_slot_invalid(%s)"), *OutRejectReason);
		return false;
	}

	ESpellRiseAbilityHotbarGroup DefinitionGroup = ESpellRiseAbilityHotbarGroup::Common;
	bool bDefinitionNotHotbar = false;
	if (!SpellRiseAbilityHotbarPrivate::ResolveAbilityDefinitionSlotGroup(LoadedAbilityDefinition, DefinitionGroup, bDefinitionNotHotbar))
	{
		OutRejectReason = bDefinitionNotHotbar ? TEXT("definition_not_hotbar_assignable") : TEXT("definition_missing_slot_group");
		return false;
	}

	const ESpellRiseAbilityHotbarGroup SlotGroup = SpellRiseAbilityHotbarPrivate::ResolveGroup(SlotIndex);
	if (DefinitionGroup != SlotGroup)
	{
		OutRejectReason = FString::Printf(
			TEXT("definition_slot_group_mismatch(slot=%s,definition=%s)"),
			SlotGroup == ESpellRiseAbilityHotbarGroup::Weapon ? TEXT("Weapon") : TEXT("Common"),
			DefinitionGroup == ESpellRiseAbilityHotbarGroup::Weapon ? TEXT("Weapon") : TEXT("Common"));
		return false;
	}

	return true;
}

bool USpellRiseAbilityHotbarComponent::CheckServerRateLimit(FString& OutRejectReason)
{
	UWorld* World = GetWorld();
	const double NowSeconds = World ? World->GetTimeSeconds() : 0.0;
	if (NowSeconds <= 0.0)
	{
		return true;
	}

	if ((NowSeconds - RateLimitWindowStartServerTimeSeconds) > HotbarRpcRateLimitWindowSeconds)
	{
		RateLimitWindowStartServerTimeSeconds = NowSeconds;
		RateLimitRequestsInWindow = 0;
	}

	++RateLimitRequestsInWindow;
	if (RateLimitRequestsInWindow > HotbarRpcRateLimitMaxRequestsPerWindow)
	{
		OutRejectReason = TEXT("rate_limited");
		return false;
	}

	return true;
}

void USpellRiseAbilityHotbarComponent::AuditRejectedHotbarRpc(const TCHAR* RpcName, const FString& RejectReason) const
{
	const FString Reason = RejectReason.IsEmpty() ? TEXT("unknown") : RejectReason;
	const int32 Count = ++RejectedRpcCountByReason.FindOrAdd(Reason);

	UE_LOG(LogSpellRiseAbilityHotbar, Warning,
		TEXT("[Hotbar][RpcRejected] Rpc=%s Reason=%s Count=%d Owner=%s"),
		RpcName ? RpcName : TEXT("unknown"),
		*Reason,
		Count,
		*GetNameSafe(GetOwner()));
}

USpellRiseAbilitySystemComponent* USpellRiseAbilityHotbarComponent::ResolveASC() const
{
	return Cast<USpellRiseAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()));
}
