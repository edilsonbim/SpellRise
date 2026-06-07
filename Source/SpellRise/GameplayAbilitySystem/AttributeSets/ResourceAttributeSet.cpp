// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "GameplayEffectExtension.h"
#include "Engine/GameInstance.h"
#include "Misc/DateTime.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Net/UnrealNetwork.h"

#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/Components/CatalystComponent.h"
#include "SpellRise/Components/SpellRiseChatComponent.h"
#include "SpellRise/Core/SpellRiseGameState.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/Core/SpellRisePlayerState.h"
#include "SpellRise/Characters/SpellRiseEnemyCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/Persistence/SpellRisePersistenceSubsystem.h"
#include "SpellRise/Persistence/SpellRisePersistenceTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseResourceRuntime, Log, All);

namespace SpellRiseTags
{
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

static ASpellRisePlayerController* ResolvePlayerControllerFromCharacter(ASpellRiseCharacterBase* Character)
{
	if (!Character)
	{
		return nullptr;
	}

	return Cast<ASpellRisePlayerController>(Character->GetController());
}

static FString ResolveCombatDisplayName(const AActor* Actor)
{
	if (!Actor)
	{
		return TEXT("desconhecido");
	}

	if (const ASpellRiseEnemyCharacterBase* Enemy = Cast<ASpellRiseEnemyCharacterBase>(Actor))
	{
		const FString EnemyName = Enemy->GetEnemyDisplayName().ToString().TrimStartAndEnd();
		if (!EnemyName.IsEmpty())
		{
			return EnemyName;
		}
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

static bool IsFallDamageSelfOrUnknownSource(const FGameplayTag& DamageTypeTag, const ASpellRiseCharacterBase* SourceCharacter, const ASpellRiseCharacterBase* TargetCharacter)
{
	return IsFallDamageTypeTag(DamageTypeTag) && (!SourceCharacter || SourceCharacter == TargetCharacter);
}

static FString ResolveCombatSourceName(const AActor* SourceActor, const AActor* TargetActor, const FGameplayTag& DamageTypeTag)
{
	if (IsFallDamageTypeTag(DamageTypeTag) && (!SourceActor || SourceActor == TargetActor))
	{
		return TEXT("fall");
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
		return TEXT("fall");
	}
	if (TagName.Contains(TEXT("Spell.Fire"), ESearchCase::IgnoreCase))
	{
		return TEXT("fire");
	}
	if (TagName.Contains(TEXT("Spell.Cold"), ESearchCase::IgnoreCase))
	{
		return TEXT("cold");
	}
	if (TagName.Contains(TEXT("Spell.Shock"), ESearchCase::IgnoreCase))
	{
		return TEXT("shock");
	}
	if (TagName.Contains(TEXT("Spell.Acid"), ESearchCase::IgnoreCase))
	{
		return TEXT("acid");
	}
	if (TagName.Contains(TEXT("Physical.Slashing"), ESearchCase::IgnoreCase))
	{
		return TEXT("slashing");
	}
	if (TagName.Contains(TEXT("Physical.Piercing"), ESearchCase::IgnoreCase))
	{
		return TEXT("piercing");
	}
	if (TagName.Contains(TEXT("Physical.Bashing"), ESearchCase::IgnoreCase) ||
		TagName.Contains(TEXT("Physical.Impact"), ESearchCase::IgnoreCase))
	{
		return TEXT("impact");
	}
	if (TagName.Contains(TEXT("Poison"), ESearchCase::IgnoreCase))
	{
		return TEXT("poison");
	}
	if (TagName.Contains(TEXT("Bleed"), ESearchCase::IgnoreCase))
	{
		return TEXT("bleed");
	}
	if (TagName.Contains(TEXT("Curse"), ESearchCase::IgnoreCase))
	{
		return TEXT("curse");
	}
	if (TagName.Contains(TEXT("Divine"), ESearchCase::IgnoreCase))
	{
		return TEXT("divine");
	}
	if (TagName.Contains(TEXT("Almighty"), ESearchCase::IgnoreCase))
	{
		return TEXT("almighty");
	}

	return TEXT("generic");
}

static void SendCombatLogMessages(
	ASpellRiseCharacterBase* SourceCharacter,
	AActor* SourceActor,
	ASpellRiseCharacterBase* TargetCharacter,
	AActor* TargetActor,
	float Damage,
	const FGameplayTag& DamageTypeTag,
	bool bTargetDied)
{
	if (Damage <= 0.f)
	{
		return;
	}

	const bool bIsFallDamage = IsFallDamageTypeTag(DamageTypeTag);
	const bool bFallSourceIsSelfOrUnknown = IsFallDamageSelfOrUnknownSource(DamageTypeTag, SourceCharacter, TargetCharacter);
	ASpellRisePlayerController* SourceController = ResolvePlayerControllerFromCharacter(SourceCharacter);
	ASpellRisePlayerController* TargetController = ResolvePlayerControllerFromCharacter(TargetCharacter);
	ASpellRisePlayerState* SourcePlayerState = SourceCharacter ? Cast<ASpellRisePlayerState>(SourceCharacter->GetPlayerState()) : nullptr;
	ASpellRisePlayerState* TargetPlayerState = TargetCharacter ? Cast<ASpellRisePlayerState>(TargetCharacter->GetPlayerState()) : nullptr;

	if (!SourceController && !TargetController)
	{
		return;
	}

	const FString SourceName = ResolveCombatSourceName(SourceActor ? SourceActor : SourceCharacter, TargetActor ? TargetActor : TargetCharacter, DamageTypeTag);
	const FString TargetName = ResolveCombatDisplayName(TargetActor ? TargetActor : TargetCharacter);
	const FString DamageTypeLabel = ResolveDamageTypeLabel(DamageTypeTag);
	const FText TimeText = FText::FromString(FDateTime::Now().ToString(TEXT("%H:%M:%S")));
	UWorld* World = TargetCharacter ? TargetCharacter->GetWorld() : nullptr;
	if (!World && SourceCharacter)
	{
		World = SourceCharacter->GetWorld();
	}

	if (SourcePlayerState && !bFallSourceIsSelfOrUnknown)
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
					? FText::FromString(TEXT("You died from fall damage."))
					: FText::FromString(FString::Printf(TEXT("You died. Killed by %s."), *SourceName));
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
				KillMessage.Text = FText::FromString(FString::Printf(TEXT("You killed %s."), *TargetName));
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
					? FText::FromString(TEXT("You died from fall damage."))
					: FText::FromString(FString::Printf(TEXT("You died. Killed by %s."), *SourceName));
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
			? FString::Printf(TEXT("You took %d fall damage."), FMath::RoundToInt(Damage))
			: FString::Printf(TEXT("You took %d %s damage from %s."), FMath::RoundToInt(Damage), *DamageTypeLabel, *SourceName);
		ChatComponent->SendCombatToPlayer(TargetController, FText::FromString(MessageText), TimeText);
		if (bTargetDied)
		{
			const FString DeathText = bFallSourceIsSelfOrUnknown
				? TEXT("You died from fall damage.")
				: FString::Printf(TEXT("You died. Killed by %s."), *SourceName);
			ChatComponent->SendCombatToPlayer(TargetController, FText::FromString(DeathText), TimeText);
		}
		return;
	}

	if (SourceController)
	{
		const FString MessageText = FString::Printf(TEXT("You dealt %d %s damage to %s."), FMath::RoundToInt(Damage), *DamageTypeLabel, *TargetName);
		ChatComponent->SendCombatToPlayer(SourceController, FText::FromString(MessageText), TimeText);
		if (bTargetDied)
		{
			const FString KillText = FString::Printf(TEXT("You killed %s."), *TargetName);
			ChatComponent->SendCombatToPlayer(SourceController, FText::FromString(KillText), TimeText);
		}
	}

	if (TargetController)
	{
		const FString MessageText = bFallSourceIsSelfOrUnknown
			? FString::Printf(TEXT("You took %d fall damage."), FMath::RoundToInt(Damage))
			: FString::Printf(TEXT("You took %d %s damage from %s."), FMath::RoundToInt(Damage), *DamageTypeLabel, *SourceName);
		ChatComponent->SendCombatToPlayer(TargetController, FText::FromString(MessageText), TimeText);
		if (bTargetDied)
		{
			const FString DeathText = bFallSourceIsSelfOrUnknown
				? TEXT("You died from fall damage.")
				: FString::Printf(TEXT("You died. Killed by %s."), *SourceName);
			ChatComponent->SendCombatToPlayer(TargetController, FText::FromString(DeathText), TimeText);
		}
	}

	if (bTargetDied && World)
	{
		const FString PublicDeathText = bFallSourceIsSelfOrUnknown
			? FString::Printf(TEXT("%s died from fall damage."), *TargetName)
			: FString::Printf(TEXT("%s killed %s."), *SourceName, *TargetName);
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

	struct FSpellRiseDeathDamageContributor
	{
		TWeakObjectPtr<AActor> Actor;
		FSpellRiseDeathParticipantData Participant;
		double LastDamageWorldSeconds = 0.0;
	};

	constexpr double DeathDamageContributionWindowSeconds = 120.0;
	TMap<TObjectKey<ASpellRiseCharacterBase>, TArray<FSpellRiseDeathDamageContributor>> GDeathDamageContributorsByVictim;

	FString ResolveDeathWorldId(const UWorld* World)
	{
		if (!World)
		{
			return TEXT("UnknownWorld");
		}

		FString ServerInstanceId;
		if (!FParse::Value(FCommandLine::Get(), TEXT("ServerInstanceId="), ServerInstanceId))
		{
			ServerInstanceId = World->URL.Port > 0
				? FString::Printf(TEXT("Port%d"), World->URL.Port)
				: TEXT("Local");
		}

		const FString MapId = UWorld::RemovePIEPrefix(World->GetMapName());
		return FString::Printf(TEXT("%s@%s"), *MapId, *ServerInstanceId);
	}

	FString ResolvePersistentIdForDeathActor(const AActor* Actor, const UWorld* World)
	{
		const APawn* Pawn = Cast<APawn>(Actor);
		const APlayerState* PlayerState = Pawn ? Pawn->GetPlayerState() : nullptr;
		if (!PlayerState)
		{
			return FString();
		}

		const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
		const USpellRisePersistenceSubsystem* Persistence = GameInstance ? GameInstance->GetSubsystem<USpellRisePersistenceSubsystem>() : nullptr;
		FString PersistentId;
		if (Persistence && Persistence->GetSteamIdFromPlayerState(PlayerState, PersistentId))
		{
			return PersistentId;
		}

		const FUniqueNetIdRepl UniqueIdRepl = PlayerState->GetUniqueId();
		return UniqueIdRepl.IsValid() && UniqueIdRepl.GetUniqueNetId().IsValid()
			? UniqueIdRepl.GetUniqueNetId()->ToString()
			: FString();
	}

	FString ResolveDeathCauseType(const AActor* Actor, const FGameplayTag& DamageTypeTag)
	{
		if (IsFallDamageTypeTag(DamageTypeTag))
		{
			return TEXT("environment");
		}
		if (Cast<ASpellRiseCharacterBase>(Actor))
		{
			return TEXT("player");
		}
		if (Cast<ASpellRiseEnemyCharacterBase>(Actor))
		{
			return TEXT("enemy");
		}
		return Actor ? TEXT("actor") : TEXT("unknown");
	}

	FSpellRiseDeathParticipantData BuildDeathParticipant(AActor* Actor, const FString& FallbackName, const FGameplayTag& DamageTypeTag, float Damage, UWorld* World)
	{
		FSpellRiseDeathParticipantData Participant;
		const bool bFallDamage = IsFallDamageTypeTag(DamageTypeTag);
		Participant.PlayerId = bFallDamage ? FString() : ResolvePersistentIdForDeathActor(Actor, World);
		Participant.DisplayName = bFallDamage ? TEXT("fall") : ResolveCombatSourceName(Actor, nullptr, DamageTypeTag);
		if (Participant.DisplayName.IsEmpty() || Participant.DisplayName.Equals(TEXT("desconhecido"), ESearchCase::IgnoreCase))
		{
			Participant.DisplayName = FallbackName;
		}
		if (Participant.DisplayName.IsEmpty())
		{
			Participant.DisplayName = bFallDamage ? TEXT("fall") : TEXT("unknown");
		}
		Participant.CauseType = ResolveDeathCauseType(Actor, DamageTypeTag);
		Participant.DamageAmount = FMath::Max(0.0f, Damage);
		return Participant;
	}

	bool IsSameDeathContributor(const FSpellRiseDeathDamageContributor& Existing, AActor* Actor, const FSpellRiseDeathParticipantData& Participant)
	{
		if (!Existing.Participant.PlayerId.IsEmpty() && !Participant.PlayerId.IsEmpty())
		{
			return Existing.Participant.PlayerId == Participant.PlayerId;
		}
		if (Existing.Actor.IsValid() && Actor)
		{
			return Existing.Actor.Get() == Actor;
		}
		return Existing.Participant.DisplayName.Equals(Participant.DisplayName, ESearchCase::IgnoreCase)
			&& Existing.Participant.CauseType.Equals(Participant.CauseType, ESearchCase::IgnoreCase);
	}

	void TrackDeathDamageContribution(ASpellRiseCharacterBase* Victim, AActor* Actor, const FSpellRiseDeathParticipantData& Participant, double WorldSeconds)
	{
		if (!Victim || Participant.DamageAmount <= 0.0f)
		{
			return;
		}

		TArray<FSpellRiseDeathDamageContributor>& Contributors = GDeathDamageContributorsByVictim.FindOrAdd(TObjectKey<ASpellRiseCharacterBase>(Victim));
		for (int32 Index = Contributors.Num() - 1; Index >= 0; --Index)
		{
			if ((WorldSeconds - Contributors[Index].LastDamageWorldSeconds) > DeathDamageContributionWindowSeconds)
			{
				Contributors.RemoveAtSwap(Index);
			}
		}

		for (FSpellRiseDeathDamageContributor& Existing : Contributors)
		{
			if (IsSameDeathContributor(Existing, Actor, Participant))
			{
				Existing.Participant.DamageAmount += Participant.DamageAmount;
				Existing.LastDamageWorldSeconds = WorldSeconds;
				return;
			}
		}

		FSpellRiseDeathDamageContributor NewContributor;
		NewContributor.Actor = Actor;
		NewContributor.Participant = Participant;
		NewContributor.LastDamageWorldSeconds = WorldSeconds;
		Contributors.Add(MoveTemp(NewContributor));
	}

	FSpellRiseDeathParticipantData ResolveTopDamageParticipant(ASpellRiseCharacterBase* Victim, const FSpellRiseDeathParticipantData& FatalParticipant)
	{
		FSpellRiseDeathParticipantData TopDamage = FatalParticipant;
		if (!Victim)
		{
			return TopDamage;
		}

		if (const TArray<FSpellRiseDeathDamageContributor>* Contributors = GDeathDamageContributorsByVictim.Find(TObjectKey<ASpellRiseCharacterBase>(Victim)))
		{
			for (const FSpellRiseDeathDamageContributor& Contributor : *Contributors)
			{
				if (Contributor.Participant.DamageAmount > TopDamage.DamageAmount)
				{
					TopDamage = Contributor.Participant;
				}
			}
		}

		return TopDamage;
	}

	bool AreSameDeathParticipants(const FSpellRiseDeathParticipantData& A, const FSpellRiseDeathParticipantData& B)
	{
		if (!A.PlayerId.IsEmpty() && !B.PlayerId.IsEmpty())
		{
			return A.PlayerId == B.PlayerId;
		}
		return A.DisplayName.Equals(B.DisplayName, ESearchCase::IgnoreCase)
			&& A.CauseType.Equals(B.CauseType, ESearchCase::IgnoreCase);
	}

	FString BuildDeathMessage(const FSpellRiseDeathParticipantData& TopDamage, const FSpellRiseDeathParticipantData& Fatal)
	{
		if (Fatal.CauseType.Equals(TEXT("environment"), ESearchCase::IgnoreCase)
			&& Fatal.DisplayName.Equals(TEXT("fall"), ESearchCase::IgnoreCase))
		{
			if (!TopDamage.DisplayName.IsEmpty()
				&& !TopDamage.DisplayName.Equals(TEXT("fall"), ESearchCase::IgnoreCase)
				&& !TopDamage.CauseType.Equals(TEXT("environment"), ESearchCase::IgnoreCase))
			{
				return FString::Printf(TEXT("Died from fall damage after taking damage from %s."), *TopDamage.DisplayName);
			}

			return TEXT("Died from fall damage.");
		}

		if (TopDamage.DisplayName.IsEmpty() && Fatal.DisplayName.IsEmpty())
		{
			return TEXT("Died.");
		}

		if (AreSameDeathParticipants(TopDamage, Fatal) || Fatal.DisplayName.IsEmpty())
		{
			return FString::Printf(TEXT("Killed by %s."), *TopDamage.DisplayName);
		}

		return FString::Printf(TEXT("Killed by %s and %s."), *TopDamage.DisplayName, *Fatal.DisplayName);
	}

	void PersistDeathEventIfNeeded(
		ASpellRiseCharacterBase* TargetCharacter,
		const FSpellRiseDeathParticipantData& TopDamage,
		const FSpellRiseDeathParticipantData& Fatal,
		const FGameplayTag& DamageTypeTag)
	{
		if (!TargetCharacter)
		{
			return;
		}

		UWorld* World = TargetCharacter->GetWorld();
		UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
		USpellRisePersistenceSubsystem* Persistence = GameInstance ? GameInstance->GetSubsystem<USpellRisePersistenceSubsystem>() : nullptr;
		if (!Persistence)
		{
			UE_LOG(LogSpellRiseResourceRuntime, Warning,
				TEXT("[Persistence][DeathEventRejected] Reason=persistence_unavailable Victim=%s"),
				*GetNameSafe(TargetCharacter));
			return;
		}

		FSpellRiseDeathEventData EventData;
		EventData.OccurredAtUtcIso8601 = FDateTime::UtcNow().ToIso8601();
		EventData.WorldId = ResolveDeathWorldId(World);
		EventData.VictimPlayerId = ResolvePersistentIdForDeathActor(TargetCharacter, World);
		EventData.VictimName = ResolveCombatDisplayName(TargetCharacter);
		EventData.VictimLevel = 0;
		EventData.TopDamage = TopDamage;
		EventData.Fatal = Fatal;
		EventData.DamageType = DamageTypeTag.IsValid() ? DamageTypeTag.ToString() : FString();
		EventData.DeathLocation = TargetCharacter->GetActorLocation();
		EventData.Message = BuildDeathMessage(EventData.TopDamage, EventData.Fatal);

		Persistence->SaveDeathEvent(EventData);
	}
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
		else if (Data.EvaluatedData.Attribute == GetDamageWasCriticalAttribute())
		{
			SetDamageWasCritical(0.f);
		}
		return;
	}

	const FGameplayAttribute& Attr = Data.EvaluatedData.Attribute;

	if (Attr == GetDamageWasCriticalAttribute())
	{
		SetDamageWasCritical(FMath::Max(0.f, GetDamageWasCritical()));
		return;
	}

	if (Attr == GetDamageAttribute())
	{
		++GResourceCombatRuntimeCounters.DamageExecAttempts;
		const float TotalDamage = GetDamage();
		const bool bWasCritical = GetDamageWasCritical() > 0.f;
		SetDamage(0.f);
		SetDamageWasCritical(0.f);

		if (TotalDamage <= 0.f)
		{
			++GResourceCombatRuntimeCounters.DamageRejectedNonPositive;
			return;
		}

		const FGameplayEffectContextHandle Ctx = Data.EffectSpec.GetEffectContext();

		float CatalystScalar = 1.f;
		if (SpellRiseTags::Data_StabilityScalar().IsValid())
		{
			CatalystScalar = Data.EffectSpec.GetSetByCallerMagnitude(SpellRiseTags::Data_StabilityScalar(), false, 1.f);
			CatalystScalar = FMath::Clamp(CatalystScalar, 0.f, 10.f);
		}

		const float PreviousHealth = GetHealth();
		ASpellRiseEnemyCharacterBase* PreDamageTargetEnemy = Cast<ASpellRiseEnemyCharacterBase>(TargetASC->GetAvatarActor());
		if (PreDamageTargetEnemy)
		{
			ASpellRisePlayerState* ContributorPlayerState = nullptr;
			if (UAbilitySystemComponent* SourceASC = Ctx.GetOriginalInstigatorAbilitySystemComponent())
			{
				if (AActor* SourceOwner = SourceASC->GetOwnerActor())
				{
					ContributorPlayerState = Cast<ASpellRisePlayerState>(SourceOwner);
					if (!ContributorPlayerState)
					{
						if (const APawn* SourcePawn = Cast<APawn>(SourceOwner))
						{
							ContributorPlayerState = Cast<ASpellRisePlayerState>(SourcePawn->GetPlayerState());
						}
					}
				}

				if (!ContributorPlayerState)
				{
					if (const APawn* SourcePawn = Cast<APawn>(SourceASC->GetAvatarActor()))
					{
						ContributorPlayerState = Cast<ASpellRisePlayerState>(SourcePawn->GetPlayerState());
					}
				}
			}

			if (!ContributorPlayerState)
			{
				if (const APawn* SourcePawn = Cast<APawn>(Ctx.GetOriginalInstigator()))
				{
					ContributorPlayerState = Cast<ASpellRisePlayerState>(SourcePawn->GetPlayerState());
				}
			}

			if (!ContributorPlayerState)
			{
				if (const APawn* SourcePawn = Cast<APawn>(Ctx.GetEffectCauser() ? Ctx.GetEffectCauser()->GetInstigator() : nullptr))
				{
					ContributorPlayerState = Cast<ASpellRisePlayerState>(SourcePawn->GetPlayerState());
				}
			}

			PreDamageTargetEnemy->RecordTalentDamageContribution_Server(ContributorPlayerState, TotalDamage);
		}

		SetHealth(FMath::Clamp(PreviousHealth - TotalDamage, 0.f, GetMaxHealth()));

		AActor* InstigatorActor = nullptr;
		if (UAbilitySystemComponent* SourceASC = Ctx.GetOriginalInstigatorAbilitySystemComponent())
		{
			InstigatorActor = SourceASC->GetAvatarActor();
		}

		if (!InstigatorActor)
		{
			InstigatorActor = Ctx.GetEffectCauser();
		}

		ASpellRiseCharacterBase* SourceCharacter = Cast<ASpellRiseCharacterBase>(InstigatorActor);

		if (!SourceCharacter && Ctx.GetEffectCauser())
		{
			SourceCharacter = Cast<ASpellRiseCharacterBase>(Ctx.GetEffectCauser()->GetInstigator());
		}

		if (!SourceCharacter && Ctx.GetOriginalInstigator())
		{
			SourceCharacter = Cast<ASpellRiseCharacterBase>(Ctx.GetOriginalInstigator());
		}

		ASpellRiseEnemyCharacterBase* SourceEnemy = nullptr;
		if (!SourceCharacter)
		{
			SourceEnemy = Cast<ASpellRiseEnemyCharacterBase>(InstigatorActor);
			if (!SourceEnemy && Ctx.GetEffectCauser())
			{
				SourceEnemy = Cast<ASpellRiseEnemyCharacterBase>(Ctx.GetEffectCauser()->GetInstigator());
			}
			if (!SourceEnemy && Ctx.GetOriginalInstigator())
			{
				SourceEnemy = Cast<ASpellRiseEnemyCharacterBase>(Ctx.GetOriginalInstigator());
			}
		}

		ASpellRiseCharacterBase* TargetCharacter = Cast<ASpellRiseCharacterBase>(TargetASC->GetAvatarActor());
		ASpellRiseEnemyCharacterBase* TargetEnemy = TargetCharacter ? nullptr : Cast<ASpellRiseEnemyCharacterBase>(TargetASC->GetAvatarActor());
		const FGameplayTag DamageTypeTag = ResolveDamageTypeTag(Data.EffectSpec);
		const bool bTargetDied = PreviousHealth > KINDA_SMALL_NUMBER && GetHealth() <= KINDA_SMALL_NUMBER;
		UWorld* World = TargetASC->GetAvatarActor() ? TargetASC->GetAvatarActor()->GetWorld() : nullptr;
		const double WorldSeconds = World ? World->GetTimeSeconds() : 0.0;
		AActor* DeathCauserActor = SourceCharacter ? Cast<AActor>(SourceCharacter) : (SourceEnemy ? Cast<AActor>(SourceEnemy) : InstigatorActor);
		if (IsFallDamageSelfOrUnknownSource(DamageTypeTag, SourceCharacter, TargetCharacter))
		{
			DeathCauserActor = nullptr;
			SourceCharacter = nullptr;
			SourceEnemy = nullptr;
			InstigatorActor = nullptr;
		}
		FSpellRiseDeathParticipantData FatalParticipant;
		if (TargetCharacter)
		{
			const FString FatalName = ResolveCombatSourceName(DeathCauserActor, TargetCharacter, DamageTypeTag);
			FatalParticipant = BuildDeathParticipant(DeathCauserActor, FatalName, DamageTypeTag, TotalDamage, World);
			TrackDeathDamageContribution(TargetCharacter, DeathCauserActor, FatalParticipant, WorldSeconds);
		}
		++GResourceCombatRuntimeCounters.DamageApplied;
		if (bTargetDied)
		{
			++GResourceCombatRuntimeCounters.DamageTargetKilled;
		}
		SendCombatHitGameplayEvents(Ctx, SourceCharacter, TargetCharacter, TotalDamage);
		if (TargetCharacter && !bTargetDied)
		{

			TargetCharacter->MultiPlayHitReactionMontage(1.0f);
		}
		else if (TargetEnemy && !bTargetDied)
		{
			TargetEnemy->MultiPlayHitReactionMontage(1.0f);
		}
		SendCombatLogMessages(SourceCharacter, InstigatorActor, TargetCharacter, TargetASC->GetAvatarActor(), TotalDamage, DamageTypeTag, bTargetDied);
		if (TargetCharacter && bTargetDied)
		{
			const FSpellRiseDeathParticipantData TopDamageParticipant = ResolveTopDamageParticipant(TargetCharacter, FatalParticipant);
			PersistDeathEventIfNeeded(TargetCharacter, TopDamageParticipant, FatalParticipant, DamageTypeTag);
			GDeathDamageContributorsByVictim.Remove(TObjectKey<ASpellRiseCharacterBase>(TargetCharacter));
		}

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
			if (TargetCharacter)
			{
				if (UCatalystComponent* TargetCatalyst = TargetCharacter->GetCatalystComponent())
				{
					TargetCatalyst->TryAddCatalystCharge_OnDamageTaken(SourceCharacter);
				}
			}
			else if (TargetEnemy)
			{
				if (UCatalystComponent* TargetCatalyst = TargetEnemy->GetCatalystComponent())
				{
					TargetCatalyst->TryAddCatalystCharge_OnDamageTaken(SourceCharacter ? Cast<AActor>(SourceCharacter) : Cast<AActor>(SourceEnemy));
				}
			}

			if (SourceCharacter && SourceCharacter != TargetCharacter)
			{
				if (UCatalystComponent* SourceCatalyst = SourceCharacter->GetCatalystComponent())
				{
					SourceCatalyst->TryAddCatalystCharge_OnValidHit(TargetCharacter);
				}
			}
			else if (SourceEnemy && SourceEnemy != TargetEnemy)
			{
				if (UCatalystComponent* SourceCatalyst = SourceEnemy->GetCatalystComponent())
				{
					SourceCatalyst->TryAddCatalystCharge_OnValidHit(TargetCharacter ? Cast<AActor>(TargetCharacter) : Cast<AActor>(TargetEnemy));
				}
			}
		}

		if (TargetCharacter)
		{
			TargetCharacter->MultiShowDamagePop(
				TotalDamage,
				TargetCharacter,
				DamageTypeTag,
				bWasCritical
			);

		}
		else if (TargetEnemy)
		{
			TargetEnemy->MultiShowDamagePop(
				TotalDamage,
				TargetEnemy,
				DamageTypeTag,
				bWasCritical
			);
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
