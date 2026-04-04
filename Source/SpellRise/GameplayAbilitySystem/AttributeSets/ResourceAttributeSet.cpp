#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "GameplayEffectExtension.h"
#include "Misc/DateTime.h"
#include "Net/UnrealNetwork.h"

#include "SpellRise/Characters/SpellRisePawnBase.h"
#include "SpellRise/Components/CatalystComponent.h"
#include "SpellRise/Components/SpellRiseChatComponent.h"
#include "SpellRise/Core/SpellRiseGameState.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseResourceRuntime, Log, All);

namespace SpellRiseTags
{
	inline const FGameplayTag& Cue_DamageNumber()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Cue.DamageNumber"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_StabilityScalar()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.StabilityScalar"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_CatalystChargeDelta()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.CatalystChargeDelta"), false);
		return Tag;
	}

	inline const FGameplayTag& Data_CatalystChargeLegacy()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Data.CatalystCharge"), false);
		return Tag;
	}

	inline const FGameplayTag& Event_Combat_HitTaken()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combat.HitTaken"), false);
		return Tag;
	}

	inline const FGameplayTag& Event_Combat_HitGiven()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Combat.HitGiven"), false);
		return Tag;
	}
}

static void AdjustForMaxChange(
	UAbilitySystemComponent* ASC,
	const FGameplayAttributeData& Affected,
	const FGameplayAttributeData& MaxAttr,
	float NewMaxValue,
	const FGameplayAttribute& AffectedProperty)
{
	if (!ASC) return;

	const float CurrentMax = MaxAttr.GetCurrentValue();
	if (CurrentMax > 0.f && !FMath::IsNearlyEqual(CurrentMax, NewMaxValue))
	{
		const float CurrentValue = Affected.GetCurrentValue();
		const float NewDelta = (CurrentValue * NewMaxValue / CurrentMax) - CurrentValue;
		ASC->ApplyModToAttributeUnsafe(AffectedProperty, EGameplayModOp::Additive, NewDelta);
	}
}

static void ApplyCatalystChargeIfConfigured(
	UAbilitySystemComponent* ASC,
	const FGameplayEffectContextHandle& Context,
	TSubclassOf<UGameplayEffect> GE_Catalyst_AddCharge,
	float ChargeAmount)
{
	if (!ASC || !ASC->IsOwnerActorAuthoritative() || !GE_Catalyst_AddCharge)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE_Catalyst_AddCharge, 1.f, Context.IsValid() ? Context : ASC->MakeEffectContext());
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return;
	}

	// Prefer canonical tag and keep legacy fallback for older assets.
	if (SpellRiseTags::Data_CatalystChargeDelta().IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(SpellRiseTags::Data_CatalystChargeDelta(), FMath::Max(0.f, ChargeAmount));
	}

	if (SpellRiseTags::Data_CatalystChargeLegacy().IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(SpellRiseTags::Data_CatalystChargeLegacy(), FMath::Max(0.f, ChargeAmount));
	}

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

static ASpellRisePlayerController* ResolvePlayerControllerFromPawn(ASpellRisePawnBase* Pawn)
{
	if (!Pawn)
	{
		return nullptr;
	}

	return Cast<ASpellRisePlayerController>(Pawn->GetController());
}

static FString ResolveCombatDisplayName(const AActor* Actor)
{
	if (!Actor)
	{
		return TEXT("desconhecido");
	}

	const APawn* Pawn = Cast<APawn>(Actor);
	if (Pawn && Pawn->GetPlayerState())
	{
		const FString PlayerName = Pawn->GetPlayerState()->GetPlayerName();
		if (!PlayerName.IsEmpty())
		{
			return PlayerName;
		}
	}

	return Actor->GetName();
}

static bool IsFallDamageTypeTag(const FGameplayTag& DamageTypeTag)
{
	static const FGameplayTag DataDamageTypeFall = FGameplayTag::RequestGameplayTag(TEXT("Data.DamageType.Fall"), false);
	return DataDamageTypeFall.IsValid() && DamageTypeTag == DataDamageTypeFall;
}

static FString ResolveCombatSourceName(const AActor* SourceActor, const AActor* TargetActor, const FGameplayTag& DamageTypeTag)
{
	if (IsFallDamageTypeTag(DamageTypeTag) && (!SourceActor || SourceActor == TargetActor))
	{
		return TEXT("queda");
	}

	return ResolveCombatDisplayName(SourceActor);
}

static FGameplayTag ResolveDamageTypeTag(const FGameplayEffectSpec& Spec)
{
	const FGameplayTag DataDamageTypeFall = FGameplayTag::RequestGameplayTag(TEXT("Data.DamageType.Fall"), false);
	if (DataDamageTypeFall.IsValid() && Spec.GetSetByCallerMagnitude(DataDamageTypeFall, false, 0.f) > 0.f)
	{
		return DataDamageTypeFall;
	}

	FGameplayTagContainer CombinedTags;
	Spec.GetAllAssetTags(CombinedTags);
	CombinedTags.AppendTags(Spec.GetDynamicAssetTags());

	if (const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags())
	{
		CombinedTags.AppendTags(*SourceTags);
	}

	if (DataDamageTypeFall.IsValid() && CombinedTags.HasTagExact(DataDamageTypeFall))
	{
		return DataDamageTypeFall;
	}

	const FGameplayTag DamageTypeRoot = FGameplayTag::RequestGameplayTag(TEXT("DamageType"), false);
	FGameplayTag BestMatch;
	int32 BestDepth = -1;

	for (const FGameplayTag& Tag : CombinedTags)
	{
		if (!Tag.IsValid())
		{
			continue;
		}

		const bool bMatchesDamageType = DamageTypeRoot.IsValid()
			? Tag.MatchesTag(DamageTypeRoot)
			: Tag.ToString().StartsWith(TEXT("DamageType."));

		if (!bMatchesDamageType)
		{
			continue;
		}

		const int32 Depth = Tag.ToString().Len();
		if (Depth > BestDepth)
		{
			BestDepth = Depth;
			BestMatch = Tag;
		}
	}

	if (BestMatch.IsValid())
	{
		return BestMatch;
	}

	return FGameplayTag::RequestGameplayTag(TEXT("DamageType.Generic"), false);
}

static FString ResolveDamageTypeLabel(const FGameplayTag& DamageTypeTag)
{
	const FString TagName = DamageTypeTag.ToString();
	if (TagName.Contains(TEXT("Data.DamageType.Fall"), ESearchCase::IgnoreCase))
	{
		return TEXT("queda");
	}
	if (TagName.Contains(TEXT("Spell.Fire"), ESearchCase::IgnoreCase))
	{
		return TEXT("fogo");
	}
	if (TagName.Contains(TEXT("Spell.Cold"), ESearchCase::IgnoreCase))
	{
		return TEXT("gelo");
	}
	if (TagName.Contains(TEXT("Spell.Shock"), ESearchCase::IgnoreCase))
	{
		return TEXT("choque");
	}
	if (TagName.Contains(TEXT("Spell.Acid"), ESearchCase::IgnoreCase))
	{
		return TEXT("acido");
	}
	if (TagName.Contains(TEXT("Physical.Slashing"), ESearchCase::IgnoreCase))
	{
		return TEXT("cortante");
	}
	if (TagName.Contains(TEXT("Physical.Piercing"), ESearchCase::IgnoreCase))
	{
		return TEXT("perfurante");
	}
	if (TagName.Contains(TEXT("Physical.Bashing"), ESearchCase::IgnoreCase) ||
		TagName.Contains(TEXT("Physical.Impact"), ESearchCase::IgnoreCase))
	{
		return TEXT("impacto");
	}
	if (TagName.Contains(TEXT("Poison"), ESearchCase::IgnoreCase))
	{
		return TEXT("veneno");
	}
	if (TagName.Contains(TEXT("Bleed"), ESearchCase::IgnoreCase))
	{
		return TEXT("sangramento");
	}
	if (TagName.Contains(TEXT("Curse"), ESearchCase::IgnoreCase))
	{
		return TEXT("maldicao");
	}
	if (TagName.Contains(TEXT("Divine"), ESearchCase::IgnoreCase))
	{
		return TEXT("divino");
	}
	if (TagName.Contains(TEXT("Almighty"), ESearchCase::IgnoreCase))
	{
		return TEXT("supremo");
	}

	return TEXT("generico");
}

static void SendCombatLogMessages(
	ASpellRisePawnBase* SourcePawn,
	ASpellRisePawnBase* TargetPawn,
	float Damage,
	const FGameplayTag& DamageTypeTag,
	bool bTargetDied)
{
	if (Damage <= 0.f)
	{
		return;
	}

	const bool bIsFallDamage = IsFallDamageTypeTag(DamageTypeTag);
	const bool bFallSourceIsSelfOrUnknown = bIsFallDamage && (!SourcePawn || SourcePawn == TargetPawn);
	ASpellRisePlayerController* SourceController = ResolvePlayerControllerFromPawn(SourcePawn);
	ASpellRisePlayerController* TargetController = ResolvePlayerControllerFromPawn(TargetPawn);
	ASpellRisePlayerState* SourcePlayerState = SourcePawn ? Cast<ASpellRisePlayerState>(SourcePawn->GetPlayerState()) : nullptr;
	ASpellRisePlayerState* TargetPlayerState = TargetPawn ? Cast<ASpellRisePlayerState>(TargetPawn->GetPlayerState()) : nullptr;

	if (!SourceController && !TargetController)
	{
		return;
	}

	const FString SourceName = ResolveCombatSourceName(SourcePawn, TargetPawn, DamageTypeTag);
	const FString TargetName = ResolveCombatDisplayName(TargetPawn);
	const FString DamageTypeLabel = ResolveDamageTypeLabel(DamageTypeTag);
	const FText TimeText = FText::FromString(FDateTime::Now().ToString(TEXT("%H:%M:%S")));
	UWorld* World = TargetPawn ? TargetPawn->GetWorld() : nullptr;
	if (!World && SourcePawn)
	{
		World = SourcePawn->GetWorld();
	}

	if (SourcePlayerState)
	{
		ESpellRiseCombatLogFlags SourceFlags = ESpellRiseCombatLogFlags::Outgoing;
		if (bTargetDied)
		{
			SourceFlags |= ESpellRiseCombatLogFlags::TargetDied;
		}

		SourcePlayerState->AppendCombatLogEvent_Server(
			World ? World->GetTimeSeconds() : 0.0,
			SourceName,
			TargetName,
			Damage,
			DamageTypeTag.GetTagName(),
			false,
			SourceFlags);
	}

	if (TargetPlayerState)
	{
		ESpellRiseCombatLogFlags TargetFlags = ESpellRiseCombatLogFlags::None;
		if (bTargetDied)
		{
			TargetFlags |= ESpellRiseCombatLogFlags::TargetDied;
		}

		TargetPlayerState->AppendCombatLogEvent_Server(
			World ? World->GetTimeSeconds() : 0.0,
			SourceName,
			TargetName,
			Damage,
			DamageTypeTag.GetTagName(),
			false,
			TargetFlags);
	}

	ASpellRiseGameState* SRGameState = World ? World->GetGameState<ASpellRiseGameState>() : nullptr;
	USpellRiseChatComponent* ChatComponent = SRGameState ? SRGameState->GetChatComponent() : nullptr;

	const auto SendViaControllerFallback = [&]()
	{
		if (SourceController && SourceController == TargetController)
		{
			TargetController->ClientReceiveCombatLogEntry(Damage, SourceName, false);
			if (bTargetDied)
			{
				FSpellRiseChatMessage DeathMessage;
				DeathMessage.Name = FName(TEXT("Combat"));
				DeathMessage.Text = bFallSourceIsSelfOrUnknown
					? FText::FromString(TEXT("Você morreu por queda."))
					: FText::FromString(FString::Printf(TEXT("Você morreu. Morto por %s."), *SourceName));
				DeathMessage.TimeText = TimeText;
				DeathMessage.Channel = SpellRiseChatChannel::Combat;
				TargetController->ClientReceiveChatMessage(DeathMessage);
			}
			return;
		}

		if (SourceController)
		{
			SourceController->ClientReceiveCombatLogEntry(Damage, TargetName, true);
			if (bTargetDied)
			{
				FSpellRiseChatMessage KillMessage;
				KillMessage.Name = FName(TEXT("Combat"));
				KillMessage.Text = FText::FromString(FString::Printf(TEXT("Você matou %s."), *TargetName));
				KillMessage.TimeText = TimeText;
				KillMessage.Channel = SpellRiseChatChannel::Combat;
				SourceController->ClientReceiveChatMessage(KillMessage);
			}
		}

		if (TargetController)
		{
			TargetController->ClientReceiveCombatLogEntry(Damage, SourceName, false);
			if (bTargetDied)
			{
				FSpellRiseChatMessage DeathMessage;
				DeathMessage.Name = FName(TEXT("Combat"));
				DeathMessage.Text = bFallSourceIsSelfOrUnknown
					? FText::FromString(TEXT("Você morreu por queda."))
					: FText::FromString(FString::Printf(TEXT("Você morreu. Morto por %s."), *SourceName));
				DeathMessage.TimeText = TimeText;
				DeathMessage.Channel = SpellRiseChatChannel::Combat;
				TargetController->ClientReceiveChatMessage(DeathMessage);
			}
		}
	};

	if (!ChatComponent)
	{
		SendViaControllerFallback();
		return;
	}

	if (SourceController && SourceController == TargetController)
	{
		const FString MessageText = bFallSourceIsSelfOrUnknown
			? FString::Printf(TEXT("Voce recebeu %d de dano por queda."), FMath::RoundToInt(Damage))
			: FString::Printf(TEXT("Voce recebeu %d de dano (%s) de %s."), FMath::RoundToInt(Damage), *DamageTypeLabel, *SourceName);
		ChatComponent->SendCombatToPlayer(TargetController, FText::FromString(MessageText), TimeText);
		if (bTargetDied)
		{
			const FString DeathText = bFallSourceIsSelfOrUnknown
				? TEXT("Você morreu por queda.")
				: FString::Printf(TEXT("Você morreu. Morto por %s."), *SourceName);
			ChatComponent->SendCombatToPlayer(TargetController, FText::FromString(DeathText), TimeText);
		}
		return;
	}

	if (SourceController)
	{
		const FString MessageText = FString::Printf(TEXT("Voce causou %d de dano (%s) em %s."), FMath::RoundToInt(Damage), *DamageTypeLabel, *TargetName);
		ChatComponent->SendCombatToPlayer(SourceController, FText::FromString(MessageText), TimeText);
		if (bTargetDied)
		{
			const FString KillText = FString::Printf(TEXT("Você matou %s."), *TargetName);
			ChatComponent->SendCombatToPlayer(SourceController, FText::FromString(KillText), TimeText);
		}
	}

	if (TargetController)
	{
		const FString MessageText = bFallSourceIsSelfOrUnknown
			? FString::Printf(TEXT("Voce recebeu %d de dano por queda."), FMath::RoundToInt(Damage))
			: FString::Printf(TEXT("Voce recebeu %d de dano (%s) de %s."), FMath::RoundToInt(Damage), *DamageTypeLabel, *SourceName);
		ChatComponent->SendCombatToPlayer(TargetController, FText::FromString(MessageText), TimeText);
		if (bTargetDied)
		{
			const FString DeathText = bFallSourceIsSelfOrUnknown
				? TEXT("Você morreu por queda.")
				: FString::Printf(TEXT("Você morreu. Morto por %s."), *SourceName);
			ChatComponent->SendCombatToPlayer(TargetController, FText::FromString(DeathText), TimeText);
		}
	}

	if (bTargetDied && World)
	{
		const FString PublicDeathText = bFallSourceIsSelfOrUnknown
			? FString::Printf(TEXT("%s morreu por queda."), *TargetName)
			: FString::Printf(TEXT("%s matou %s."), *SourceName, *TargetName);
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			ASpellRisePlayerController* OtherController = Cast<ASpellRisePlayerController>(It->Get());
			if (!OtherController)
			{
				continue;
			}

			if (OtherController == SourceController || OtherController == TargetController)
			{
				continue;
			}

			ChatComponent->SendCombatToPlayer(OtherController, FText::FromString(PublicDeathText), TimeText);
		}
	}
}

static void SendCombatHitGameplayEvents(
	const FGameplayEffectContextHandle& ContextHandle,
	AActor* SourceActor,
	AActor* TargetActor,
	float Damage)
{
	if (Damage <= 0.f)
	{
		return;
	}

	AActor* EventInstigator = SourceActor ? SourceActor : ContextHandle.GetEffectCauser();
	UObject* SourceObject = ContextHandle.GetSourceObject();

	if (TargetActor && SpellRiseTags::Event_Combat_HitTaken().IsValid())
	{
		FGameplayEventData HitTakenData;
		HitTakenData.EventTag = SpellRiseTags::Event_Combat_HitTaken();
		HitTakenData.EventMagnitude = Damage;
		HitTakenData.Instigator = EventInstigator;
		HitTakenData.Target = TargetActor;
		HitTakenData.ContextHandle = ContextHandle;
		HitTakenData.OptionalObject = SourceObject;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, HitTakenData.EventTag, HitTakenData);
	}

	if (SourceActor && SpellRiseTags::Event_Combat_HitGiven().IsValid())
	{
		FGameplayEventData HitGivenData;
		HitGivenData.EventTag = SpellRiseTags::Event_Combat_HitGiven();
		HitGivenData.EventMagnitude = Damage;
		HitGivenData.Instigator = EventInstigator;
		HitGivenData.Target = TargetActor;
		HitGivenData.ContextHandle = ContextHandle;
		HitGivenData.OptionalObject = SourceObject;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SourceActor, HitGivenData.EventTag, HitGivenData);
	}
}

namespace
{
	struct FResourceCombatRuntimeCounters
	{
		int64 DamageExecAttempts = 0;
		int64 DamageRejectedNonPositive = 0;
		int64 DamageApplied = 0;
		int64 DamageTargetKilled = 0;
	};

	FResourceCombatRuntimeCounters GResourceCombatRuntimeCounters;
}

UResourceAttributeSet::UResourceAttributeSet()
{
	Health = 180.f;
	MaxHealth = 180.f;

	Mana = 180.f;
	MaxMana = 180.f;

	Stamina = 180.f;
	MaxStamina = 180.f;

	HealthRegen = 1.f;
	ManaRegen = 1.f;
	StaminaRegen = 1.f;

	Damage = 0.f;
}

void UResourceAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, CarryWeight, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, HealthRegen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, ManaRegen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UResourceAttributeSet, StaminaRegen, COND_None, REPNOTIFY_Always);
}

void UResourceAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
		AdjustForMaxChange(GetOwningAbilitySystemComponent(), Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
		AdjustForMaxChange(GetOwningAbilitySystemComponent(), Mana, MaxMana, NewValue, GetManaAttribute());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
		AdjustForMaxChange(GetOwningAbilitySystemComponent(), Stamina, MaxStamina, NewValue, GetStaminaAttribute());
	}
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	else if (Attribute == GetHealthRegenAttribute() ||
	         Attribute == GetManaRegenAttribute() ||
	         Attribute == GetStaminaRegenAttribute())
	{
		NewValue = FMath::Max(0.f, NewValue);
	}
}

void UResourceAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	auto SetIfChanged = [](float Current, float Desired, TFunctionRef<void(float)> Setter)
	{
		if (!FMath::IsNearlyEqual(Current, Desired))
		{
			Setter(Desired);
		}
	};

	if (Attribute == GetCarryWeightAttribute())
	{
		const float Clamped = FMath::Max(0.f, GetCarryWeight());
		SetIfChanged(GetCarryWeight(), Clamped, [this](float V) { SetCarryWeight(V); });
		return;
	}

	if (Attribute == GetMaxHealthAttribute())
	{
		const float ClampedMax = FMath::Max(0.f, GetMaxHealth());
		SetIfChanged(GetMaxHealth(), ClampedMax, [this](float V) { SetMaxHealth(V); });
		const float ClampedValue = FMath::Clamp(GetHealth(), 0.f, ClampedMax);
		SetIfChanged(GetHealth(), ClampedValue, [this](float V) { SetHealth(V); });
		return;
	}

	if (Attribute == GetMaxManaAttribute())
	{
		const float ClampedMax = FMath::Max(0.f, GetMaxMana());
		SetIfChanged(GetMaxMana(), ClampedMax, [this](float V) { SetMaxMana(V); });
		const float ClampedValue = FMath::Clamp(GetMana(), 0.f, ClampedMax);
		SetIfChanged(GetMana(), ClampedValue, [this](float V) { SetMana(V); });
		return;
	}

	if (Attribute == GetMaxStaminaAttribute())
	{
		const float ClampedMax = FMath::Max(0.f, GetMaxStamina());
		SetIfChanged(GetMaxStamina(), ClampedMax, [this](float V) { SetMaxStamina(V); });
		const float ClampedValue = FMath::Clamp(GetStamina(), 0.f, ClampedMax);
		SetIfChanged(GetStamina(), ClampedValue, [this](float V) { SetStamina(V); });
		return;
	}

	if (Attribute == GetHealthAttribute())
	{
		const float Clamped = FMath::Clamp(GetHealth(), 0.f, GetMaxHealth());
		SetIfChanged(GetHealth(), Clamped, [this](float V) { SetHealth(V); });
	}
	else if (Attribute == GetManaAttribute())
	{
		const float Clamped = FMath::Clamp(GetMana(), 0.f, GetMaxMana());
		SetIfChanged(GetMana(), Clamped, [this](float V) { SetMana(V); });
	}
	else if (Attribute == GetStaminaAttribute())
	{
		const float Clamped = FMath::Clamp(GetStamina(), 0.f, GetMaxStamina());
		SetIfChanged(GetStamina(), Clamped, [this](float V) { SetStamina(V); });
	}
	else if (Attribute == GetHealthRegenAttribute())
	{
		const float Clamped = FMath::Max(0.f, GetHealthRegen());
		SetIfChanged(GetHealthRegen(), Clamped, [this](float V) { SetHealthRegen(V); });
	}
	else if (Attribute == GetManaRegenAttribute())
	{
		const float Clamped = FMath::Max(0.f, GetManaRegen());
		SetIfChanged(GetManaRegen(), Clamped, [this](float V) { SetManaRegen(V); });
	}
	else if (Attribute == GetStaminaRegenAttribute())
	{
		const float Clamped = FMath::Max(0.f, GetStaminaRegen());
		SetIfChanged(GetStaminaRegen(), Clamped, [this](float V) { SetStaminaRegen(V); });
	}
}

void UResourceAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* TargetASC = GetOwningAbilitySystemComponent();
	if (!TargetASC)
	{
		return;
	}

	if (!TargetASC->IsOwnerActorAuthoritative())
	{
		if (Data.EvaluatedData.Attribute == GetDamageAttribute())
		{
			SetDamage(0.f);
		}
		return;
	}

	const FGameplayAttribute& Attr = Data.EvaluatedData.Attribute;

	if (Attr == GetDamageAttribute())
	{
		++GResourceCombatRuntimeCounters.DamageExecAttempts;
		const float TotalDamage = GetDamage();
		SetDamage(0.f);

		if (TotalDamage <= 0.f)
		{
			++GResourceCombatRuntimeCounters.DamageRejectedNonPositive;
			UE_LOG(
				LogSpellRiseResourceRuntime,
				Verbose,
				TEXT("[COMBAT][DAMAGE_DENIED] Reason=NonPositive Damage=%.2f Attempts=%lld Denied=%lld"),
				TotalDamage,
				GResourceCombatRuntimeCounters.DamageExecAttempts,
				GResourceCombatRuntimeCounters.DamageRejectedNonPositive);
			return;
		}

		const FGameplayEffectContextHandle Ctx = Data.EffectSpec.GetEffectContext();

		float CatalystScalar = 1.f;
		if (SpellRiseTags::Data_StabilityScalar().IsValid())
		{
			CatalystScalar = Data.EffectSpec.GetSetByCallerMagnitude(SpellRiseTags::Data_StabilityScalar(), false, 1.f);
			CatalystScalar = FMath::Clamp(CatalystScalar, 0.f, 10.f);
		}

		SetHealth(FMath::Clamp(GetHealth() - TotalDamage, 0.f, GetMaxHealth()));

		AActor* InstigatorActor = nullptr;
		if (UAbilitySystemComponent* SourceASC = Ctx.GetOriginalInstigatorAbilitySystemComponent())
		{
			InstigatorActor = SourceASC->GetAvatarActor();
		}

		if (!InstigatorActor)
		{
			InstigatorActor = Ctx.GetEffectCauser();
		}

		ASpellRisePawnBase* SourcePawn = Cast<ASpellRisePawnBase>(InstigatorActor);

		if (!SourcePawn && Ctx.GetEffectCauser())
		{
			SourcePawn = Cast<ASpellRisePawnBase>(Ctx.GetEffectCauser()->GetInstigator());
		}

		if (!SourcePawn && Ctx.GetOriginalInstigator())
		{
			SourcePawn = Cast<ASpellRisePawnBase>(Ctx.GetOriginalInstigator());
		}

		ASpellRisePawnBase* TargetPawn = Cast<ASpellRisePawnBase>(TargetASC->GetAvatarActor());
		const FGameplayTag DamageTypeTag = ResolveDamageTypeTag(Data.EffectSpec);
		const bool bTargetDied = (GetHealth() <= KINDA_SMALL_NUMBER);
		++GResourceCombatRuntimeCounters.DamageApplied;
		if (bTargetDied)
		{
			++GResourceCombatRuntimeCounters.DamageTargetKilled;
		}
		UE_LOG(
			LogSpellRiseResourceRuntime,
			Verbose,
			TEXT("[COMBAT][DAMAGE_APPLIED] Damage=%.2f Type=%s Source=%s Target=%s Died=%d Applied=%lld Kills=%lld"),
			TotalDamage,
			*DamageTypeTag.ToString(),
			*GetNameSafe(SourcePawn),
			*GetNameSafe(TargetPawn),
			bTargetDied ? 1 : 0,
			GResourceCombatRuntimeCounters.DamageApplied,
			GResourceCombatRuntimeCounters.DamageTargetKilled);
		SendCombatHitGameplayEvents(Ctx, SourcePawn, TargetPawn, TotalDamage);
		if (TargetPawn && !bTargetDied)
		{
			// Cosmetic fallback AAA: server decides hit and replicates only presentation.
			TargetPawn->MultiPlayHitReactionMontage(1.0f);
		}
		SendCombatLogMessages(SourcePawn, TargetPawn, TotalDamage, DamageTypeTag, bTargetDied);

		const float CatalystChargeAmount = TotalDamage * CatalystScalar;
		if (GE_Catalyst_AddCharge)
		{
			ApplyCatalystChargeIfConfigured(TargetASC, Ctx, GE_Catalyst_AddCharge, CatalystChargeAmount);

			if (UAbilitySystemComponent* SourceASC = Ctx.GetOriginalInstigatorAbilitySystemComponent())
			{
				if (SourceASC != TargetASC)
				{
					ApplyCatalystChargeIfConfigured(SourceASC, Ctx, GE_Catalyst_AddCharge, CatalystChargeAmount);
				}
			}
		}
		else
		{
			if (TargetPawn)
			{
				if (UCatalystComponent* TargetCatalyst = TargetPawn->GetCatalystComponent())
				{
					TargetCatalyst->TryAddCatalystCharge_OnDamageTaken(SourcePawn);
				}
			}

			if (SourcePawn && SourcePawn != TargetPawn)
			{
				if (UCatalystComponent* SourceCatalyst = SourcePawn->GetCatalystComponent())
				{
					SourceCatalyst->TryAddCatalystCharge_OnValidHit(TargetPawn);
				}
			}
		}

		if (TargetPawn)
		{
			TargetPawn->MultiShowDamagePop(
				TotalDamage,
				TargetPawn,
				DamageTypeTag,
				false
			);

			UE_LOG(LogSpellRiseResourceRuntime, Verbose, TEXT("[POP][ResourceSet] Damage=%.1f Source=%s Target=%s Causer=%s"),
				TotalDamage,
				*GetNameSafe(SourcePawn),
				*GetNameSafe(TargetPawn),
				*GetNameSafe(Ctx.GetEffectCauser()));
		}

		if (SpellRiseTags::Cue_DamageNumber().IsValid())
		{
			FGameplayCueParameters CueParams;
			CueParams.RawMagnitude = TotalDamage;
			TargetASC->ExecuteGameplayCue(SpellRiseTags::Cue_DamageNumber(), CueParams);
		}

		return;
	}

	if (Attr == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		return;
	}

	if (Attr == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
		return;
	}

	if (Attr == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
		return;
	}
}

void UResourceAttributeSet::OnRep_CarryWeight(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, CarryWeight, OldValue);
}

void UResourceAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, Health, OldValue);
}

void UResourceAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, MaxHealth, OldValue);
}

void UResourceAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, Mana, OldValue);
}

void UResourceAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, MaxMana, OldValue);
}

void UResourceAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, Stamina, OldValue);
}

void UResourceAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, MaxStamina, OldValue);
}

void UResourceAttributeSet::OnRep_HealthRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, HealthRegen, OldValue);
}

void UResourceAttributeSet::OnRep_ManaRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, ManaRegen, OldValue);
}

void UResourceAttributeSet::OnRep_StaminaRegen(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UResourceAttributeSet, StaminaRegen, OldValue);
}

