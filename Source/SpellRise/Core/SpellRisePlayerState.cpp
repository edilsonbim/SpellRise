#include "SpellRisePlayerState.h"

#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "Engine/GameInstance.h"
#include "Net/UnrealNetwork.h"

#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/Persistence/SpellRisePersistenceSubsystem.h"

#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"
#include "SpellRise/Core/SpellRisePlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseRespawnSecurity, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseCombatLog, Log, All);

namespace
{
	constexpr int32 MaxRespawnBedActorNameLen = 128;
	constexpr int32 MaxRespawnBedClassPathLen = 512;
	constexpr float RespawnBedLocationTolerance = 300.0f;

	bool SRPS_IsFiniteVector(const FVector& Value)
	{
		return FMath::IsFinite(Value.X) && FMath::IsFinite(Value.Y) && FMath::IsFinite(Value.Z);
	}

	bool SRPS_IsLikelyBedClassPath(const FString& ClassPath)
	{
		return ClassPath.Contains(TEXT("Building_Bed"), ESearchCase::IgnoreCase)
			|| ClassPath.Contains(TEXT("_Bed"), ESearchCase::IgnoreCase)
			|| ClassPath.Contains(TEXT(".Bed"), ESearchCase::IgnoreCase);
	}

	bool SRPS_IsLikelyBedActor(const AActor* Actor)
	{
		if (!IsValid(Actor))
		{
			return false;
		}

		if (Actor->GetName().Contains(TEXT("Bed"), ESearchCase::IgnoreCase))
		{
			return true;
		}

		return SRPS_IsLikelyBedClassPath(Actor->GetClass()->GetPathName());
	}

	void PersistRespawnBedChange(ASpellRisePlayerState* PlayerState)
	{
		if (!PlayerState || !PlayerState->HasAuthority())
		{
			return;
		}

		AController* OwnerController = Cast<AController>(PlayerState->GetOwner());
		if (!OwnerController)
		{
			return;
		}

		if (UWorld* World = PlayerState->GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				if (USpellRisePersistenceSubsystem* Persistence = GameInstance->GetSubsystem<USpellRisePersistenceSubsystem>())
				{
					Persistence->SaveCharacterForController(OwnerController);
				}
			}
		}
	}
}

ASpellRisePlayerState::ASpellRisePlayerState()
{
	bReplicates = true;
	SetNetUpdateFrequency(30.0f);
	SetMinNetUpdateFrequency(10.0f);

	AbilitySystemComponent = CreateDefaultSubobject<USpellRiseAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	BasicAttributeSet = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("BasicAttributeSet"));
	CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));
	ResourceAttributeSet = CreateDefaultSubobject<UResourceAttributeSet>(TEXT("ResourceAttributeSet"));
	CatalystAttributeSet = CreateDefaultSubobject<UCatalystAttributeSet>(TEXT("CatalystAttributeSet"));
	DerivedStatsAttributeSet = CreateDefaultSubobject<UDerivedStatsAttributeSet>(TEXT("DerivedStatsAttributeSet"));
}

UAbilitySystemComponent* ASpellRisePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

USpellRiseAbilitySystemComponent* ASpellRisePlayerState::GetSpellRiseASC() const
{
	return AbilitySystemComponent;
}

void ASpellRisePlayerState::SetRespawnBedReference(AActor* BedActor)
{
	const FString ActorName = BedActor ? BedActor->GetName() : FString();
	const FString ClassPath = BedActor ? BedActor->GetClass()->GetPathName() : FString();
	const FVector Location = BedActor ? BedActor->GetActorLocation() : FVector::ZeroVector;

	SetRespawnBedReferenceData(ActorName, ClassPath, Location);
}

void ASpellRisePlayerState::SetRespawnBedReferenceData(const FString& InActorName, const FString& InClassPath, const FVector& InLocation)
{
	if (HasAuthority())
	{
		RespawnBedActorName = InActorName;
		RespawnBedClassPath = InClassPath;
		RespawnBedLocation = InLocation;
		OnRep_RespawnBedData();
		PersistRespawnBedChange(this);
		return;
	}

	ServerSetRespawnBedReferenceData(InActorName, InClassPath, FVector_NetQuantize(InLocation));
}

void ASpellRisePlayerState::SetPersistenceProfileApplied(bool bApplied)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bPersistenceProfileApplied == bApplied)
	{
		return;
	}

	bPersistenceProfileApplied = bApplied;
	OnRep_PersistenceProfileApplied();
	ForceNetUpdate();

	if (bPersistenceProfileApplied && HasAuthority())
	{
		MaybeSendCombatLogSnapshotToOwner_Server(TEXT("profile_applied"));
	}
}

void ASpellRisePlayerState::ServerSetRespawnBedReferenceData_Implementation(const FString& InActorName, const FString& InClassPath, const FVector_NetQuantize& InLocation)
{
	const AController* OwnerController = Cast<AController>(GetOwner());
	const FVector RequestLocation = FVector(InLocation);

	FString RejectReason;
	if (!CheckRespawnBedServerRateLimit(RejectReason))
	{
		UE_LOG(
			LogSpellRiseRespawnSecurity,
			Warning,
			TEXT("[SEC][RespawnBed] Rejected by rate-limit. PlayerState=%s Controller=%s Reason=%s ActorName=%s ClassPath=%s Location=%s"),
			*GetNameSafe(this),
			*GetNameSafe(OwnerController),
			*RejectReason,
			*InActorName,
			*InClassPath,
			*RequestLocation.ToCompactString());
		return;
	}

	if (!ValidateRespawnBedPayload(OwnerController, InActorName, InClassPath, RequestLocation, RejectReason))
	{
		UE_LOG(
			LogSpellRiseRespawnSecurity,
			Warning,
			TEXT("[SEC][RespawnBed] Rejected payload. PlayerState=%s Controller=%s Reason=%s ActorName=%s ClassPath=%s Location=%s"),
			*GetNameSafe(this),
			*GetNameSafe(OwnerController),
			*RejectReason,
			*InActorName,
			*InClassPath,
			*RequestLocation.ToCompactString());
		return;
	}

	const FString SanitizedActorName = InActorName.TrimStartAndEnd();
	const FString SanitizedClassPath = InClassPath.TrimStartAndEnd();
	const bool bClearRequest = SanitizedActorName.IsEmpty() && SanitizedClassPath.IsEmpty() && RequestLocation.IsNearlyZero();

	if (bClearRequest)
	{
		RespawnBedActorName.Reset();
		RespawnBedClassPath.Reset();
		RespawnBedLocation = FVector::ZeroVector;
		OnRep_RespawnBedData();
		PersistRespawnBedChange(this);
		ForceNetUpdate();
		return;
	}

	AActor* ResolvedBedActor = ResolveRespawnBedActorOnServer(SanitizedActorName, SanitizedClassPath, RequestLocation);
	if (!ResolvedBedActor)
	{
		UE_LOG(
			LogSpellRiseRespawnSecurity,
			Warning,
			TEXT("[SEC][RespawnBed] Rejected unresolved bed reference. PlayerState=%s Controller=%s ActorName=%s ClassPath=%s Location=%s"),
			*GetNameSafe(this),
			*GetNameSafe(OwnerController),
			*SanitizedActorName,
			*SanitizedClassPath,
			*RequestLocation.ToCompactString());
		return;
	}

	RespawnBedActorName = ResolvedBedActor->GetName();
	RespawnBedClassPath = ResolvedBedActor->GetClass()->GetPathName();
	RespawnBedLocation = ResolvedBedActor->GetActorLocation();
	OnRep_RespawnBedData();
	PersistRespawnBedChange(this);
	ForceNetUpdate();
}

bool ASpellRisePlayerState::CheckRespawnBedServerRateLimit(FString& OutRejectReason)
{
	if (!HasAuthority())
	{
		OutRejectReason = TEXT("not_authority");
		return false;
	}

	UWorld* World = GetWorld();
	const double NowSeconds = World ? World->GetTimeSeconds() : 0.0;
	const double WindowSeconds = FMath::Max(0.1, static_cast<double>(RespawnBedRpcRateLimitWindowSeconds));
	const int32 MaxCount = FMath::Max(1, RespawnBedRpcRateLimitMaxCountPerWindow);

	if ((NowSeconds - RespawnBedRpcRateState.WindowStartSeconds) > WindowSeconds)
	{
		RespawnBedRpcRateState.WindowStartSeconds = NowSeconds;
		RespawnBedRpcRateState.RequestCountInWindow = 0;
	}

	++RespawnBedRpcRateState.RequestCountInWindow;
	if (RespawnBedRpcRateState.RequestCountInWindow > MaxCount)
	{
		OutRejectReason = FString::Printf(
			TEXT("rate_limited(window=%.2fs,max=%d,count=%d)"),
			WindowSeconds,
			MaxCount,
			RespawnBedRpcRateState.RequestCountInWindow);
		return false;
	}

	return true;
}

void ASpellRisePlayerState::OnRep_RespawnBedData()
{
}

void ASpellRisePlayerState::OnRep_PersistenceProfileApplied()
{
}

void ASpellRisePlayerState::AppendCombatLogEvent_Server(
	double ServerTimeSeconds,
	const FString& InstigatorName,
	const FString& TargetName,
	float Damage,
	FName DamageType,
	bool bIsCritical,
	ESpellRiseCombatLogFlags Flags)
{
	if (!HasAuthority())
	{
		return;
	}

	if (Damage <= 0.0f)
	{
		return;
	}

	FSpellRiseCombatLogEntry& NewEntry = CombatLog.Entries.Emplace_GetRef();
	NewEntry.ServerTimeSeconds = ServerTimeSeconds;
	NewEntry.InstigatorName = InstigatorName;
	NewEntry.TargetName = TargetName;
	NewEntry.Damage = Damage;
	NewEntry.DamageType = DamageType;
	NewEntry.bIsCritical = bIsCritical;
	NewEntry.Flags = static_cast<uint8>(Flags);
	NewEntry.Sequence = NextCombatLogSequence++;

	CombatLog.MarkItemDirty(NewEntry);

	const int32 MaxEntries = FMath::Max(1, MaxCombatLogEntries);
	while (CombatLog.Entries.Num() > MaxEntries)
	{
		CombatLog.Entries.RemoveAt(0);
		++CombatLogTruncatedCount;
		CombatLog.MarkArrayDirty();
	}

	if (CombatLogTruncatedCount > 0 && (CombatLogTruncatedCount % 10) == 0)
	{
		UE_LOG(
			LogSpellRiseCombatLog,
			Warning,
			TEXT("[CombatLog] TruncatedCount=%lld PlayerState=%s MaxEntries=%d"),
			CombatLogTruncatedCount,
			*GetNameSafe(this),
			MaxEntries);
	}

	ForceNetUpdate();
}

void ASpellRisePlayerState::MaybeSendCombatLogSnapshotToOwner_Server(const TCHAR* Reason)
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const double NowSeconds = static_cast<double>(World->GetTimeSeconds());
	const double RateLimit = static_cast<double>(FMath::Max(0.1f, CombatLogSnapshotRateLimitSeconds));
	if (LastCombatLogSnapshotSentServerSeconds > 0.0 && (NowSeconds - LastCombatLogSnapshotSentServerSeconds) < RateLimit)
	{
		return;
	}

	LastCombatLogSnapshotSentServerSeconds = NowSeconds;
	int64 SnapshotSequence = 0;
	if (CombatLog.Entries.Num() > 0)
	{
		SnapshotSequence = CombatLog.Entries.Last().Sequence;
	}

	ClientReceiveCombatLogSnapshot(CombatLog.Entries, SnapshotSequence);
	UE_LOG(LogSpellRiseCombatLog, Log, TEXT("[CombatLog][Snapshot] Reason=%s PlayerState=%s Entries=%d Seq=%lld"),
		Reason ? Reason : TEXT("unknown"),
		*GetNameSafe(this),
		CombatLog.Entries.Num(),
		SnapshotSequence);
}

void ASpellRisePlayerState::ClientReceiveCombatLogSnapshot_Implementation(const TArray<FSpellRiseCombatLogEntry>& Snapshot, int64 SnapshotSequence)
{
	CombatLog.Entries = Snapshot;
	CombatLog.MarkArrayDirty();

	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (ASpellRisePlayerController* SRPC = Cast<ASpellRisePlayerController>(OwnerController))
		{
			for (const FSpellRiseCombatLogEntry& Entry : Snapshot)
			{
				const bool bIsOutgoing = EnumHasAnyFlags(static_cast<ESpellRiseCombatLogFlags>(Entry.Flags), ESpellRiseCombatLogFlags::Outgoing);
				const FString OtherActor = bIsOutgoing ? Entry.TargetName : Entry.InstigatorName;
				SRPC->ClientReceiveCombatLogEntry(Entry.Damage, OtherActor, bIsOutgoing);
			}
		}
	}

	UE_LOG(LogSpellRiseCombatLog, Log, TEXT("[CombatLog][SnapshotClient] PlayerState=%s Entries=%d Seq=%lld"),
		*GetNameSafe(this),
		Snapshot.Num(),
		SnapshotSequence);
}

void ASpellRisePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASpellRisePlayerState, RespawnBedActorName, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASpellRisePlayerState, RespawnBedClassPath, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASpellRisePlayerState, RespawnBedLocation, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASpellRisePlayerState, bPersistenceProfileApplied, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASpellRisePlayerState, CombatLog, COND_OwnerOnly);
}

bool ASpellRisePlayerState::ValidateRespawnBedPayload(
	const AController* OwnerController,
	const FString& InActorName,
	const FString& InClassPath,
	const FVector& InLocation,
	FString& OutRejectReason) const
{
	if (!HasAuthority())
	{
		OutRejectReason = TEXT("not_authority");
		return false;
	}

	if (!OwnerController || OwnerController->PlayerState != this)
	{
		OutRejectReason = TEXT("owner_context_mismatch");
		return false;
	}

	const FString SanitizedActorName = InActorName.TrimStartAndEnd();
	const FString SanitizedClassPath = InClassPath.TrimStartAndEnd();

	if (SanitizedActorName.Len() > MaxRespawnBedActorNameLen)
	{
		OutRejectReason = TEXT("actor_name_too_long");
		return false;
	}

	if (SanitizedClassPath.Len() > MaxRespawnBedClassPathLen)
	{
		OutRejectReason = TEXT("class_path_too_long");
		return false;
	}

	if (!SRPS_IsFiniteVector(InLocation))
	{
		OutRejectReason = TEXT("location_non_finite");
		return false;
	}

	const bool bClearRequest = SanitizedActorName.IsEmpty() && SanitizedClassPath.IsEmpty() && InLocation.IsNearlyZero();
	if (bClearRequest)
	{
		return true;
	}

	const bool bLooksLikeBedActorName = SanitizedActorName.IsEmpty() || SanitizedActorName.Contains(TEXT("Bed"), ESearchCase::IgnoreCase);
	const bool bLooksLikeBedClassPath = SanitizedClassPath.IsEmpty() || SRPS_IsLikelyBedClassPath(SanitizedClassPath);
	if (!bLooksLikeBedActorName || !bLooksLikeBedClassPath)
	{
		OutRejectReason = TEXT("payload_not_bed_like");
		return false;
	}

	return true;
}

AActor* ASpellRisePlayerState::ResolveRespawnBedActorOnServer(
	const FString& InActorName,
	const FString& InClassPath,
	const FVector& InLocation) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UClass* RequestedClass = nullptr;
	if (!InClassPath.IsEmpty())
	{
		RequestedClass = LoadObject<UClass>(nullptr, *InClassPath);
	}

	const auto IsMatchingBedActor = [&](const AActor* Actor) -> bool
	{
		if (!SRPS_IsLikelyBedActor(Actor))
		{
			return false;
		}

		if (RequestedClass && !Actor->IsA(RequestedClass))
		{
			return false;
		}

		if (!InActorName.IsEmpty() && !Actor->GetName().Equals(InActorName, ESearchCase::IgnoreCase))
		{
			return false;
		}

		return true;
	};

	AActor* BestMatch = nullptr;
	double BestDistanceSq = TNumericLimits<double>::Max();
	const double MaxDistanceSq = FMath::Square(static_cast<double>(RespawnBedLocationTolerance));
	const bool bUseLocation = !InLocation.IsNearlyZero();

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Candidate = *It;
		if (!IsMatchingBedActor(Candidate))
		{
			continue;
		}

		if (!bUseLocation)
		{
			return Candidate;
		}

		const double DistanceSq = FVector::DistSquared(Candidate->GetActorLocation(), InLocation);
		if (DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestMatch = Candidate;
		}
	}

	if (BestMatch && BestDistanceSq <= MaxDistanceSq)
	{
		return BestMatch;
	}

	return nullptr;
}
