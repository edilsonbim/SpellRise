// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRisePlayerState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/ActorComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Engine/GameInstance.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UnrealType.h"

#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilityHotbarComponent.h"
#include "SpellRise/Persistence/SpellRisePersistenceSubsystem.h"

#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"
#include "SpellRise/Core/SpellRisePlayerController.h"
#include "SpellRise/Progression/SpellRiseProgressionComponent.h"
#include "InventoryComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseRespawnSecurity, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseCombatLog, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseProgression, Log, All);

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

	bool SRPS_IsTalentTreeComponent(const UActorComponent* Component)
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

	UActorComponent* SRPS_FindTalentTreeComponent(AActor* Owner)
	{
		if (!Owner)
		{
			return nullptr;
		}

		TArray<UActorComponent*> Components;
		Owner->GetComponents(Components);
		for (UActorComponent* Component : Components)
		{
			if (SRPS_IsTalentTreeComponent(Component))
			{
				return Component;
			}
		}

		return nullptr;
	}

	AController* SRPS_ResolveControllerForPlayerState(ASpellRisePlayerState* PlayerState)
	{
		if (!PlayerState)
		{
			return nullptr;
		}

		if (APawn* Pawn = Cast<APawn>(PlayerState->GetPawn()))
		{
			if (AController* Controller = Pawn->GetController())
			{
				return Controller;
			}
		}

		UWorld* World = PlayerState->GetWorld();
		if (!World)
		{
			return nullptr;
		}

		for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
		{
			AController* Controller = It->Get();
			if (Controller && Controller->PlayerState == PlayerState)
			{
				return Controller;
			}
		}

		return nullptr;
	}

	UActorComponent* SRPS_ResolveTalentTreeComponent(ASpellRisePlayerState* PlayerState)
	{
		if (!PlayerState)
		{
			return nullptr;
		}

		if (UActorComponent* PlayerStateComponent = SRPS_FindTalentTreeComponent(PlayerState))
		{
			return PlayerStateComponent;
		}

		if (AController* Controller = SRPS_ResolveControllerForPlayerState(PlayerState))
		{
			if (UActorComponent* CharacterComponent = SRPS_FindTalentTreeComponent(Controller->GetPawn()))
			{
				return CharacterComponent;
			}
		}

		return nullptr;
	}

	void SRPS_NotifyTalentTreeTalentPointsChanged(UActorComponent* TalentComponent)
	{
		if (!TalentComponent)
		{
			return;
		}

		if (UFunction* OnRepFunction = TalentComponent->FindFunction(TEXT("OnRep_TalentPoints")))
		{
			TalentComponent->ProcessEvent(OnRepFunction, nullptr);
		}

		if (UFunction* PointsChangedFunction = TalentComponent->FindFunction(TEXT("OnPointsChanged")))
		{
			TalentComponent->ProcessEvent(PointsChangedFunction, nullptr);
		}
	}

	FNumericProperty* SRPS_FindTalentTreeTalentPointsProperty(UClass* ComponentClass)
	{
		if (!ComponentClass)
		{
			return nullptr;
		}

		return FindFProperty<FNumericProperty>(ComponentClass, TEXT("TalentPoints"));
	}

	bool SRPS_AddTalentTreeTalentPoints(ASpellRisePlayerState* PlayerState, const int32 PointsToAdd, int32& OutNewPoints)
	{
		OutNewPoints = 0;
		if (!PlayerState || PointsToAdd <= 0)
		{
			return false;
		}

		UActorComponent* TalentComponent = SRPS_ResolveTalentTreeComponent(PlayerState);
		if (!TalentComponent || !TalentComponent->GetClass())
		{
			UE_LOG(LogSpellRiseProgression, Warning,
				TEXT("[Progression][TalentPointsGrantRejected] Reason=missing_talent_component PlayerState=%s Amount=%d"),
				*GetNameSafe(PlayerState),
				PointsToAdd);
			return false;
		}

		FNumericProperty* PointsProperty = SRPS_FindTalentTreeTalentPointsProperty(TalentComponent->GetClass());
		if (!PointsProperty)
		{
			FString NumericProperties;
			for (TFieldIterator<FProperty> It(TalentComponent->GetClass(), EFieldIterationFlags::IncludeSuper); It; ++It)
			{
				if (CastField<FNumericProperty>(*It))
				{
					if (!NumericProperties.IsEmpty())
					{
						NumericProperties += TEXT(",");
					}
					NumericProperties += It->GetName();
				}
			}

			UE_LOG(LogSpellRiseProgression, Warning,
				TEXT("[Progression][TalentPointsGrantRejected] Reason=missing_talent_points Component=%s Class=%s Amount=%d NumericProperties=%s"),
				*GetNameSafe(TalentComponent),
				*GetNameSafe(TalentComponent->GetClass()),
				PointsToAdd,
				*NumericProperties.Left(512));
			return false;
		}

		const int32 CurrentPoints = PointsProperty->IsInteger()
			? FMath::Max(0, static_cast<int32>(PointsProperty->GetSignedIntPropertyValue(PointsProperty->ContainerPtrToValuePtr<void>(TalentComponent))))
			: FMath::Max(0, FMath::RoundToInt(PointsProperty->GetFloatingPointPropertyValue(PointsProperty->ContainerPtrToValuePtr<void>(TalentComponent))));
		OutNewPoints = FMath::Clamp(CurrentPoints + PointsToAdd, 0, 1000000);
		if (PointsProperty->IsInteger())
		{
			PointsProperty->SetIntPropertyValue(PointsProperty->ContainerPtrToValuePtr<void>(TalentComponent), static_cast<int64>(OutNewPoints));
		}
		else
		{
			PointsProperty->SetFloatingPointPropertyValue(PointsProperty->ContainerPtrToValuePtr<void>(TalentComponent), static_cast<double>(OutNewPoints));
		}

		SRPS_NotifyTalentTreeTalentPointsChanged(TalentComponent);

		if (AActor* TalentOwner = TalentComponent->GetOwner())
		{
			TalentOwner->ForceNetUpdate();
		}

		UE_LOG(LogSpellRiseProgression, Log,
			TEXT("[Progression][TalentPointsGranted] Component=%s Property=%s Added=%d Total=%d"),
			*GetNameSafe(TalentComponent),
			*PointsProperty->GetName(),
			PointsToAdd,
			OutNewPoints);
		return true;
	}

	const FGameplayTag& SRPS_EventAbilitiesChanged()
	{
		static const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TEXT("Event.Abilities.Changed"), false);
		return Tag;
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
	AbilityHotbarComponent = CreateDefaultSubobject<USpellRiseAbilityHotbarComponent>(TEXT("AbilityHotbarComponent"));
	ProgressionComponent = CreateDefaultSubobject<USpellRiseProgressionComponent>(TEXT("ProgressionComponent"));
	NarrativeInventoryComponent = CreateDefaultSubobject<UNarrativeInventoryComponent>(TEXT("NarrativeInventoryComponent"));

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

TArray<FGameplayAbilitySpecHandle> ASpellRisePlayerState::GrantAbilities(
	const TArray<FSpellRiseGrantedAbility>& AbilitiesToGrant,
	const int32 AbilityLevel)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return {};
	}

	TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	AbilityHandles.Reserve(AbilitiesToGrant.Num());

	const int32 FinalLevel = FMath::Max(1, AbilityLevel);
	for (const FSpellRiseGrantedAbility& Grant : AbilitiesToGrant)
	{
		UClass* AbilityClass = Grant.AbilityClass.LoadSynchronous();
		if (!AbilityClass)
		{
			continue;
		}

		if (AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass) != nullptr)
		{
			continue;
		}

		FGameplayAbilitySpec Spec(AbilityClass, FinalLevel, INDEX_NONE, this);
		if (Grant.InputTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(Grant.InputTag);
		}

		const FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
		AbilityHandles.Add(Handle);

		if (Grant.bAutoActivateIfNoInputTag && !Grant.InputTag.IsValid())
		{
			AbilitySystemComponent->TryActivateAbility(Handle);
		}
	}

	AbilitySystemComponent->RefreshAbilityActorInfo();
	SendAbilitiesChangedEvent();
	return AbilityHandles;
}

FGameplayAbilitySpecHandle ASpellRisePlayerState::GrantAbility(
	TSoftClassPtr<UGameplayAbility> AbilityClass,
	const int32 AbilityLevel,
	FGameplayTag InputTag,
	const bool bAutoActivateIfNoInputTag)
{
	FSpellRiseGrantedAbility Grant;
	Grant.AbilityClass = AbilityClass;
	Grant.InputTag = InputTag;
	Grant.bAutoActivateIfNoInputTag = bAutoActivateIfNoInputTag;

	const TArray<FGameplayAbilitySpecHandle> Handles = GrantAbilities({ Grant }, AbilityLevel);
	return Handles.Num() > 0 ? Handles[0] : FGameplayAbilitySpecHandle();
}

TArray<FGameplayAbilitySpecHandle> ASpellRisePlayerState::GrantAbilitiesFromSource(
	const TArray<FSpellRiseGrantedAbility>& AbilitiesToGrant,
	UObject* SourceObject,
	const int32 AbilityLevel,
	const bool bAllowDuplicateAbilityClassesForDifferentSources)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return {};
	}

	UObject* EffectiveSourceObject = SourceObject ? SourceObject : static_cast<UObject*>(this);
	const int32 FinalLevel = FMath::Max(1, AbilityLevel);
	TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	AbilityHandles.Reserve(AbilitiesToGrant.Num());

	for (const FSpellRiseGrantedAbility& Grant : AbilitiesToGrant)
	{
		UClass* AbilityClass = Grant.AbilityClass.LoadSynchronous();
		if (!AbilityClass)
		{
			continue;
		}

		if (!bAllowDuplicateAbilityClassesForDifferentSources && AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass) != nullptr)
		{
			continue;
		}

		if (bAllowDuplicateAbilityClassesForDifferentSources)
		{
			bool bAlreadyGrantedForSource = false;
			for (const FGameplayAbilitySpec& ExistingSpec : AbilitySystemComponent->GetActivatableAbilities())
			{
				if (ExistingSpec.Ability && ExistingSpec.Ability->GetClass() == AbilityClass && ExistingSpec.SourceObject == EffectiveSourceObject)
				{
					bAlreadyGrantedForSource = true;
					break;
				}
			}

			if (bAlreadyGrantedForSource)
			{
				continue;
			}
		}

		FGameplayAbilitySpec Spec(AbilityClass, FinalLevel, INDEX_NONE, EffectiveSourceObject);
		if (Grant.InputTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(Grant.InputTag);
		}

		const FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
		AbilityHandles.Add(Handle);

		if (Grant.bAutoActivateIfNoInputTag && !Grant.InputTag.IsValid())
		{
			AbilitySystemComponent->TryActivateAbility(Handle);
		}
	}

	AbilitySystemComponent->RefreshAbilityActorInfo();
	SendAbilitiesChangedEvent();
	return AbilityHandles;
}

void ASpellRisePlayerState::RemoveAbilities(const TArray<FGameplayAbilitySpecHandle>& AbilityHandlesToRemove)
{
	if (!AbilitySystemComponent || !HasAuthority())
	{
		return;
	}

	AbilitySystemComponent->SR_ClearAbilityInput();
	AbilitySystemComponent->SR_ClearSelectedSpellAbility();

	for (const FGameplayAbilitySpecHandle& AbilityHandle : AbilityHandlesToRemove)
	{
		AbilitySystemComponent->ClearAbility(AbilityHandle);
	}

	SendAbilitiesChangedEvent();
}

void ASpellRisePlayerState::SendAbilitiesChangedEvent()
{
	if (!SRPS_EventAbilitiesChanged().IsValid())
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.EventTag = SRPS_EventAbilitiesChanged();
	EventData.Instigator = this;
	EventData.Target = this;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
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
		AuditRejectedRespawnBedRpc(RejectReason, InActorName, InClassPath, RequestLocation);
		return;
	}

	if (!ValidateRespawnBedPayload(OwnerController, InActorName, InClassPath, RequestLocation, RejectReason))
	{
		AuditRejectedRespawnBedRpc(RejectReason, InActorName, InClassPath, RequestLocation);
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
		AuditRejectedRespawnBedRpc(TEXT("bed_actor_not_resolved"), InActorName, InClassPath, RequestLocation);
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
	RecordOnRepTelemetry(TEXT("RespawnBedData"));
}

void ASpellRisePlayerState::OnRep_PersistenceProfileApplied()
{
	RecordOnRepTelemetry(TEXT("PersistenceProfileApplied"));
}

void ASpellRisePlayerState::OnRep_TalentPoints()
{
	RecordOnRepTelemetry(TEXT("TalentPoints"));
	UE_LOG(LogSpellRiseProgression, Verbose, TEXT("TalentPoints replicado para o cliente. Novo valor: %f"), TalentPoints);
}

void ASpellRisePlayerState::AddTalentPoints_Server(float Amount)
{
	if (!HasAuthority())
	{
		UE_LOG(LogSpellRiseProgression, Warning, TEXT("Tentativa de adicionar Talent Points sem autoridade. PlayerState=%s"), *GetNameSafe(this));
		return;
	}

	if (Amount <= 0.0f)
	{
		return;
	}

	const int32 PointsToAdd = FMath::Max(1, FMath::RoundToInt(Amount));
	int32 TalentTreePoints = 0;
	const bool bUpdatedTalentTree = SRPS_AddTalentTreeTalentPoints(this, PointsToAdd, TalentTreePoints);
	TalentPoints = bUpdatedTalentTree ? static_cast<float>(TalentTreePoints) : TalentPoints + Amount;
	ForceNetUpdate();
	UE_LOG(LogSpellRiseProgression, Log, TEXT("Adicionado %f Talent Points para %s. Total: %f"), Amount, *GetNameSafe(this), TalentPoints);
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

	UWorld* World = GetWorld();
	const double NowSeconds = World ? static_cast<double>(World->GetTimeSeconds()) : 0.0;
	constexpr double CombatLogOverflowWarningIntervalSeconds = 60.0;
	if (CombatLogTruncatedCount > 0
		&& (LastCombatLogOverflowWarningServerSeconds < 0.0
			|| (NowSeconds - LastCombatLogOverflowWarningServerSeconds) >= CombatLogOverflowWarningIntervalSeconds))
	{
		LastCombatLogOverflowWarningServerSeconds = NowSeconds;
		UE_LOG(LogSpellRiseCombatLog, Warning,
			TEXT("[Net][CombatLogOverflow] PlayerState=%s TruncatedCount=%lld MaxEntries=%d CurrentEntries=%d"),
			*GetNameSafe(this),
			CombatLogTruncatedCount,
			MaxEntries,
			CombatLog.Entries.Num());
	}

	const bool bTargetDied = EnumHasAnyFlags(Flags, ESpellRiseCombatLogFlags::TargetDied);
	const double NetUpdateRateLimit = static_cast<double>(FMath::Max(0.02f, CombatLogNetUpdateRateLimitSeconds));
	if (bTargetDied
		|| LastCombatLogNetUpdateServerSeconds < 0.0
		|| (NowSeconds - LastCombatLogNetUpdateServerSeconds) >= NetUpdateRateLimit)
	{
		LastCombatLogNetUpdateServerSeconds = NowSeconds;
		ForceNetUpdate();
	}
}

void ASpellRisePlayerState::AuditRejectedRespawnBedRpc(
	const FString& RejectReason,
	const FString& InActorName,
	const FString& InClassPath,
	const FVector& InLocation)
{
	const int32 RejectCount = ++RejectedRpcCountByReason.FindOrAdd(RejectReason);
	UE_LOG(LogSpellRiseRespawnSecurity, Warning,
		TEXT("[RPC][Rejected] Rpc=ServerSetRespawnBedReferenceData Reason=%s Count=%d PlayerState=%s Owner=%s ActorNameLen=%d ClassPathLen=%d Location=%s"),
		*RejectReason,
		RejectCount,
		*GetNameSafe(this),
		*GetNameSafe(GetOwner()),
		InActorName.Len(),
		InClassPath.Len(),
		*InLocation.ToCompactString());
}

void ASpellRisePlayerState::RecordOnRepTelemetry(const TCHAR* RepName)
{
	const FString RepKey = RepName ? RepName : TEXT("unknown");
	UWorld* World = GetWorld();
	const double NowSeconds = World ? World->GetTimeSeconds() : 0.0;
	double& WindowStart = OnRepWindowStartByName.FindOrAdd(RepKey);
	int32& Count = OnRepCountByName.FindOrAdd(RepKey);

	if (WindowStart <= 0.0 || (NowSeconds - WindowStart) >= 60.0)
	{
		WindowStart = NowSeconds;
		Count = 0;
	}

	++Count;
	if (Count == 1 || (Count % 30) == 0)
	{
		UE_LOG(LogSpellRiseCombatLog, Verbose,
			TEXT("[Net][OnRepRate] PlayerState=%s Rep=%s CountIn60s=%d"),
			*GetNameSafe(this),
			*RepKey,
			Count);
	}
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
	TArray<FSpellRiseCombatLogEntry> Snapshot;
	int64 SnapshotSequence = 0;
	const int32 EntryCount = CombatLog.Entries.Num();
	if (EntryCount > 0)
	{
		const int32 MaxSnapshotEntries = FMath::Clamp(MaxCombatLogSnapshotEntries, 1, FMath::Max(1, MaxCombatLogEntries));
		const int32 SnapshotCount = FMath::Min(EntryCount, MaxSnapshotEntries);
		const int32 StartIndex = EntryCount - SnapshotCount;
		Snapshot.Reserve(SnapshotCount);
		for (int32 Index = StartIndex; Index < EntryCount; ++Index)
		{
			Snapshot.Add(CombatLog.Entries[Index]);
		}

		SnapshotSequence = CombatLog.Entries.Last().Sequence;
	}

	ClientReceiveCombatLogSnapshot(Snapshot, SnapshotSequence);
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

}

void ASpellRisePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASpellRisePlayerState, RespawnBedActorName, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASpellRisePlayerState, RespawnBedClassPath, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASpellRisePlayerState, RespawnBedLocation, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASpellRisePlayerState, bPersistenceProfileApplied, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASpellRisePlayerState, CombatLog, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASpellRisePlayerState, TalentPoints, COND_OwnerOnly);
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
