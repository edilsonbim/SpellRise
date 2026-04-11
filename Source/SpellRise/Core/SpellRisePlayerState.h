#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Engine/NetSerialization.h"
#include "SpellRise/Core/SpellRiseCombatLogTypes.h"
#include "SpellRisePlayerState.generated.h"

class UAbilitySystemComponent;
class USpellRiseAbilitySystemComponent;
class UBasicAttributeSet;
class UCombatAttributeSet;
class UResourceAttributeSet;
class UCatalystAttributeSet;
class UDerivedStatsAttributeSet;
class AActor;
class AController;

UCLASS()
class SPELLRISE_API ASpellRisePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASpellRisePlayerState();


	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	USpellRiseAbilitySystemComponent* GetSpellRiseASC() const;

	UBasicAttributeSet* GetBasicAttributeSet() const { return BasicAttributeSet; }
	UCombatAttributeSet* GetCombatAttributeSet() const { return CombatAttributeSet; }
	UResourceAttributeSet* GetResourceAttributeSet() const { return ResourceAttributeSet; }
	UCatalystAttributeSet* GetCatalystAttributeSet() const { return CatalystAttributeSet; }
	UDerivedStatsAttributeSet* GetDerivedStatsAttributeSet() const { return DerivedStatsAttributeSet; }

	UFUNCTION(BlueprintCallable, Category="SpellRise|Respawn")
	void SetRespawnBedReference(AActor* BedActor);

	void SetRespawnBedReferenceData(const FString& InActorName, const FString& InClassPath, const FVector& InLocation);
	void SetPersistenceProfileApplied(bool bApplied);
	bool IsPersistenceProfileApplied() const { return bPersistenceProfileApplied; }

	const FString& GetRespawnBedActorName() const { return RespawnBedActorName; }
	const FString& GetRespawnBedClassPath() const { return RespawnBedClassPath; }
	const FVector& GetRespawnBedLocation() const { return RespawnBedLocation; }

	void AppendCombatLogEvent_Server(
		double ServerTimeSeconds,
		const FString& InstigatorName,
		const FString& TargetName,
		float Damage,
		FName DamageType,
		bool bIsCritical,
		ESpellRiseCombatLogFlags Flags);

	UFUNCTION(BlueprintPure, Category="SpellRise|CombatLog")
	const TArray<FSpellRiseCombatLogEntry>& GetCombatLogEntries() const { return CombatLog.Entries; }

protected:
	UFUNCTION(Server, Reliable)
	void ServerSetRespawnBedReferenceData(const FString& InActorName, const FString& InClassPath, const FVector_NetQuantize& InLocation);

	UFUNCTION()
	void OnRep_RespawnBedData();

	UFUNCTION()
	void OnRep_PersistenceProfileApplied();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS")
	TObjectPtr<USpellRiseAbilitySystemComponent> AbilitySystemComponent = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UBasicAttributeSet> BasicAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCombatAttributeSet> CombatAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UResourceAttributeSet> ResourceAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCatalystAttributeSet> CatalystAttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GAS", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UDerivedStatsAttributeSet> DerivedStatsAttributeSet = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_RespawnBedData, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Respawn")
	FString RespawnBedActorName;

	UPROPERTY(ReplicatedUsing=OnRep_RespawnBedData, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Respawn")
	FString RespawnBedClassPath;

	UPROPERTY(ReplicatedUsing=OnRep_RespawnBedData, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Respawn")
	FVector RespawnBedLocation = FVector::ZeroVector;

	UPROPERTY(ReplicatedUsing=OnRep_PersistenceProfileApplied, VisibleAnywhere, BlueprintReadOnly, Category="SpellRise|Persistence")
	bool bPersistenceProfileApplied = false;

	UPROPERTY(Replicated)
	FSpellRiseCombatLogArray CombatLog;

	UFUNCTION(Client, Reliable)
	void ClientReceiveCombatLogSnapshot(const TArray<FSpellRiseCombatLogEntry>& Snapshot, int64 SnapshotSequence);

private:
	struct FRespawnBedRpcRateState
	{
		double WindowStartSeconds = 0.0;
		int32 RequestCountInWindow = 0;
	};

	bool CheckRespawnBedServerRateLimit(FString& OutRejectReason);

	bool ValidateRespawnBedPayload(
		const AController* OwnerController,
		const FString& InActorName,
		const FString& InClassPath,
		const FVector& InLocation,
		FString& OutRejectReason) const;
	AActor* ResolveRespawnBedActorOnServer(
		const FString& InActorName,
		const FString& InClassPath,
		const FVector& InLocation) const;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Security|RPC", meta=(ClampMin="0.1", UIMin="0.1"))
	float RespawnBedRpcRateLimitWindowSeconds = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Security|RPC", meta=(ClampMin="1", UIMin="1"))
	int32 RespawnBedRpcRateLimitMaxCountPerWindow = 2;

	FRespawnBedRpcRateState RespawnBedRpcRateState;

	void MaybeSendCombatLogSnapshotToOwner_Server(const TCHAR* Reason);

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|CombatLog", meta=(ClampMin="1", UIMin="1"))
	int32 MaxCombatLogEntries = 150;

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|CombatLog", meta=(ClampMin="0.1", UIMin="0.1"))
	float CombatLogSnapshotRateLimitSeconds = 2.0f;

	UPROPERTY(Transient)
	int64 NextCombatLogSequence = 1;

	UPROPERTY(Transient)
	int64 CombatLogTruncatedCount = 0;

	UPROPERTY(Transient)
	double LastCombatLogSnapshotSentServerSeconds = -1.0;
};
