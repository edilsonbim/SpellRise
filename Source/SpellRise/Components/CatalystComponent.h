// CatalystComponent.h

/*
 * UCatalystComponent encapsulates all of the catalyst-related functionality that used to live on
 * the ability system component.  By moving this logic into its own actor component, designers can
 * enable or disable the catalyst system on any character instance directly from the editor.  The
 * component communicates with the owning character's ability system component to add and consume
 * catalyst charges and to trigger the proc ability when charged.
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "CatalystComponent.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class USpellRiseAbilitySystemComponent;
class UCatalystAttributeSet;
class ASpellRisePawnBase;

/**
 * Component that implements SpellRise's catalyst mechanic.  It handles adding catalyst charges
 * when the owner lands valid hits or takes damage, listening for charge reaching 100%, and
 * triggering the configured proc ability when ready.  The component can be toggled on or off via
 * the bCatalystEnabled property in the editor.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API UCatalystComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCatalystComponent();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Gameplay effect used to add catalyst charge.  Should increment the CatalystCharge attribute
     * defined on UCatalystAttributeSet.  Assign this in the Blueprint defaults of the pawn.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Catalyst")
    TSubclassOf<UGameplayEffect> GE_CatalystAddCharge;

    /**
     * Ability that is activated automatically when catalyst charge reaches 100%.  Assign this in
     * the Blueprint defaults of the character.  The ability should apply State.Catalyst.Active
     * and consume the catalyst charge.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Catalyst")
    TSubclassOf<UGameplayAbility> CatalystProcAbilityClass;

    /**
     * Whether the catalyst system is currently enabled.  When disabled, no charges are gained
     * and the proc will not trigger.  Exposed to the editor so designers can toggle on/off per
     * instance.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Catalyst")
    bool bCatalystEnabled = true;

    /**
     * Adds a catalyst charge when the owner hits a valid target.  Should be called by gameplay
     * abilities or weapons when damage is successfully applied to another actor.
     *
     * @param TargetActor Actor that was hit by the owner.
     * @return True if a charge was successfully added.
     */
    UFUNCTION(BlueprintCallable, Category="Catalyst")
    bool TryAddCatalystCharge_OnValidHit(AActor* TargetActor);

    /**
     * Adds a catalyst charge when the owner takes real damage.  Should be called by health/damage
     * systems when the owner suffers damage.
     *
     * @param InstigatorActor Actor that inflicted the damage on the owner.
     * @return True if a charge was successfully added.
     */
    UFUNCTION(BlueprintCallable, Category="Catalyst")
    bool TryAddCatalystCharge_OnDamageTaken(AActor* InstigatorActor);

    /**
     * Attempts to trigger the catalyst proc ability if the charge is ready.  This should be
     * automatically called when the charge crosses the threshold, but can also be called manually.
     */
    UFUNCTION(BlueprintCallable, Category="Catalyst")
    void TryProcCatalystIfReady();

private:
    bool ResolveAbilitySystemComponent();
    bool HasServerAuthority() const;
    /** Ensure the OnCatalystChargeChanged delegate is bound on the server. */
    void EnsureCatalystChargeListenerBound_Server();
    /** Bind the catalyst charge change delegate on the ability system component. */
    void BindCatalystChargeListener_Server();
    /** Called when the catalyst charge attribute changes.  Triggers the proc when it reaches 100. */
    void OnCatalystChargeChanged(const struct FOnAttributeChangeData& Data);

    /** Whether we've bound the charge change delegate. */
    bool bCatalystListenerBound = false;

    /** Ability system component of our owning pawn. */
    UPROPERTY(Transient)
    TObjectPtr<USpellRiseAbilitySystemComponent> AbilitySystemComponent;

    /** Owning SpellRise pawn. */
    UPROPERTY(Transient)
    TObjectPtr<ASpellRisePawnBase> OwnerPawn;
};
