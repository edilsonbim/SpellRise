#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SpellRiseCombatLogTypes.generated.h"

UENUM(BlueprintType)
enum class ESpellRiseCombatLogFlags : uint8
{
	None = 0,
	Outgoing = 1 << 0,
	TargetDied = 1 << 1
};

ENUM_CLASS_FLAGS(ESpellRiseCombatLogFlags);

USTRUCT(BlueprintType)
struct FSpellRiseCombatLogEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|CombatLog")
	double ServerTimeSeconds = 0.0;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|CombatLog")
	FString InstigatorName;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|CombatLog")
	FString TargetName;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|CombatLog")
	float Damage = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|CombatLog")
	FName DamageType = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|CombatLog")
	bool bIsCritical = false;

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|CombatLog")
	uint8 Flags = static_cast<uint8>(ESpellRiseCombatLogFlags::None);

	UPROPERTY(BlueprintReadOnly, Category="SpellRise|CombatLog")
	int64 Sequence = 0;
};

USTRUCT()
struct FSpellRiseCombatLogArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSpellRiseCombatLogEntry> Entries;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSpellRiseCombatLogEntry, FSpellRiseCombatLogArray>(Entries, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FSpellRiseCombatLogArray> : public TStructOpsTypeTraitsBase2<FSpellRiseCombatLogArray>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
