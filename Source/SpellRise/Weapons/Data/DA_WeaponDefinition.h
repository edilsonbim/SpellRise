#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DA_WeaponDefinition.generated.h"

class UAnimMontage;
class AActor;

UENUM(BlueprintType)
enum class EWeaponAttackShape : uint8
{
    Sphere UMETA(DisplayName="Sphere"),
    Capsule UMETA(DisplayName="Capsule"),
    Box    UMETA(DisplayName="Box")
};

USTRUCT(BlueprintType)
struct SPELLRISE_API FWeaponAttackTuning
{
    GENERATED_BODY()

    /** Base weapon damage */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Damage", meta=(ClampMin="0.0"))
    float BaseDamage = 25.0f;

    /** Damage scalar (rarity/quality) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Damage", meta=(ClampMin="0.0"))
    float DamageScalar = 1.0f;

    /** Damage scaling per combo hit */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Damage", meta=(ClampMin="0.0"))
    TArray<float> DamageScalingPerHit = { 1.00f, 1.10f, 1.25f };

    /** Trace range */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Trace", meta=(ClampMin="0.0"))
    float Range = 180.0f;

    /** Attack shape */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Trace")
    EWeaponAttackShape AttackShape = EWeaponAttackShape::Sphere;

    /** Sphere radius */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Trace",
        meta=(ClampMin="0.0", EditCondition="AttackShape==EWeaponAttackShape::Sphere"))
    float SphereRadius = 60.0f;

    /** Capsule radius */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Trace",
        meta=(ClampMin="0.0", EditCondition="AttackShape==EWeaponAttackShape::Capsule"))
    float CapsuleRadius = 45.0f;

    /** Capsule half height */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Trace",
        meta=(ClampMin="0.0", EditCondition="AttackShape==EWeaponAttackShape::Capsule"))
    float CapsuleHalfHeight = 75.0f;

    /** Box extent */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Trace",
        meta=(EditCondition="AttackShape==EWeaponAttackShape::Box"))
    FVector BoxExtent = FVector(60.f, 40.f, 40.f);

    /** Damage type tag */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Tags")
    FGameplayTag DamageTypeTag;

    /** Attack montage */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Animation")
    TObjectPtr<UAnimMontage> AttackMontage = nullptr;
};

UCLASS(BlueprintType)
class SPELLRISE_API UDA_WeaponDefinition : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
    FName WeaponId = NAME_None;

    /** Light attack tuning */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Attacks")
    FWeaponAttackTuning LightAttack;

    /** Weapon actor class to spawn */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Visual")
    TSubclassOf<AActor> WeaponActorClass;

    /** Socket to attach weapon */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Visual")
    FName AttachSocketName = TEXT("Socket_Weapon_R");

    /** Attachment offset */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Visual")
    FTransform AttachOffset;

    UFUNCTION(BlueprintPure, Category="Weapon")
    FORCEINLINE float GetLightAttackFinalBaseDamage() const
    {
        return LightAttack.BaseDamage * LightAttack.DamageScalar;
    }
};