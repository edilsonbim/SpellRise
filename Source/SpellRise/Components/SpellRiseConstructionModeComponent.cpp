#include "SpellRise/Components/SpellRiseConstructionModeComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseConstructionMode, Log, All);

namespace
{
	constexpr double MinConstructionToggleIntervalSeconds = 0.15;
}

USpellRiseConstructionModeComponent::USpellRiseConstructionModeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	BlockedEntryStateTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Dead"), false));
	BlockedEntryStateTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.CC.Stunned"), false));
	BlockedEntryStateTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Casting"), false));
	BlockedEntryStateTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Combat.BuildBlocked"), false));
}

void USpellRiseConstructionModeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USpellRiseConstructionModeComponent, bConstructionModeEnabled);
}

void USpellRiseConstructionModeComponent::RequestSetConstructionMode(bool bEnableConstructionMode)
{
	if (APlayerController* OwnerPC = Cast<APlayerController>(GetOwner()))
	{
		if (!OwnerPC->IsLocalController() && !OwnerPC->HasAuthority())
		{
			return;
		}
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ApplyConstructionModeInternal(bEnableConstructionMode, true);
		return;
	}

	ApplyConstructionModeInternal(bEnableConstructionMode, false);
	ServerSetConstructionMode(bEnableConstructionMode);
}

void USpellRiseConstructionModeComponent::RequestToggleConstructionMode()
{
	RequestSetConstructionMode(!bConstructionModeEnabled);
}

void USpellRiseConstructionModeComponent::OnRep_ConstructionModeEnabled()
{
	ConstructionModeChangedNative.Broadcast(bConstructionModeEnabled);
}

void USpellRiseConstructionModeComponent::ServerSetConstructionMode_Implementation(bool bEnableConstructionMode)
{
	FString RejectReason;
	if (!CanAcceptConstructionToggle(bEnableConstructionMode, RejectReason))
	{
		UE_LOG(
			LogSpellRiseConstructionMode,
			Warning,
			TEXT("[ConstructionMode][Reject] Owner=%s Requested=%d Reason=%s"),
			*GetNameSafe(GetOwner()),
			bEnableConstructionMode ? 1 : 0,
			*RejectReason);
		return;
	}

	ApplyConstructionModeInternal(bEnableConstructionMode, true);
}

void USpellRiseConstructionModeComponent::ApplyConstructionModeInternal(bool bEnableConstructionMode, bool bLog)
{
	if (bConstructionModeEnabled == bEnableConstructionMode)
	{
		return;
	}

	bConstructionModeEnabled = bEnableConstructionMode;
	ConstructionModeChangedNative.Broadcast(bConstructionModeEnabled);

	if (bLog)
	{
		UE_LOG(
			LogSpellRiseConstructionMode,
			Log,
			TEXT("[ConstructionMode][StateChanged] Owner=%s Enabled=%d"),
			*GetNameSafe(GetOwner()),
			bConstructionModeEnabled ? 1 : 0);
	}
}

bool USpellRiseConstructionModeComponent::CanAcceptConstructionToggle(bool bEnableConstructionMode, FString& OutRejectReason) const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		OutRejectReason = TEXT("NoWorld");
		return false;
	}

	const double Now = World->GetTimeSeconds();
	if ((Now - LastConstructionToggleServerTimeSeconds) < MinConstructionToggleIntervalSeconds)
	{
		OutRejectReason = TEXT("RateLimited");
		return false;
	}

	if (bEnableConstructionMode && IsBlockedForConstructionEntry(OutRejectReason))
	{
		return false;
	}

	USpellRiseConstructionModeComponent* MutableThis = const_cast<USpellRiseConstructionModeComponent*>(this);
	MutableThis->LastConstructionToggleServerTimeSeconds = Now;
	OutRejectReason = TEXT("Accepted");
	return true;
}

bool USpellRiseConstructionModeComponent::IsBlockedForConstructionEntry(FString& OutRejectReason) const
{
	const APlayerController* OwnerPC = Cast<APlayerController>(GetOwner());
	const APawn* Pawn = OwnerPC ? OwnerPC->GetPawn() : nullptr;
	if (!IsValid(Pawn))
	{
		OutRejectReason = TEXT("NoPawn");
		return true;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn);
	if (!ASC)
	{
		return false;
	}

	for (const FGameplayTag& BlockedTag : BlockedEntryStateTags)
	{
		if (BlockedTag.IsValid() && ASC->HasMatchingGameplayTag(BlockedTag))
		{
			OutRejectReason = FString::Printf(TEXT("BlockedTag:%s"), *BlockedTag.ToString());
			return true;
		}
	}

	return false;
}
