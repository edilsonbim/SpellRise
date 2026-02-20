#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

class USpellRiseGameplayAbility;
#include "SpellRiseAbilitySystemComponent.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:
	// Cache para detectar mudanças em abilities replicadas e disparar evento no Character
	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpec> LastActivatableAbilities;

	// =========================================================
	// Input pipeline (custom, like Lyra)
	// =========================================================
	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

public:
	USpellRiseAbilitySystemComponent();

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_ActivateAbilities() override;

	// Hooks chamados por AbilityLocalInputPressed/Released internamente
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

public:
	// =========================================================
	// Input public API
	// =========================================================
	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_AbilityInputPressed(int32 InputID);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_AbilityInputReleased(int32 InputID);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	UFUNCTION(BlueprintCallable, Category="SpellRise|GAS|Input")
	void SR_ClearAbilityInput();

	// ---------------------------------------------------------
	// Ability Wheel helpers
	// ---------------------------------------------------------
	/** Activate the first ability spec that matches the given InputID. */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	bool SR_TryActivateAbilityByInputID(int32 InputID);

	/** Returns the SpellRiseGameplayAbility CDO for the first spec matching InputID (may be null). */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Abilities")
	USpellRiseGameplayAbility* SR_GetSpellRiseAbilityForInputID(int32 InputID) const;

	// =========================================================
	// Catalyst (único dono da mecânica)
	// =========================================================
	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Catalyst")
	TSubclassOf<UGameplayEffect> GE_CatalystAddCharge;

	/** Habilidade que faz o proc (deve aplicar State.Catalyst.Active e consumir charge). */
	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Catalyst")
	TSubclassOf<UGameplayAbility> CatalystProcAbilityClass;

	FORCEINLINE TSubclassOf<UGameplayAbility> GetCatalystProcAbilityClass() const
	{
		return CatalystProcAbilityClass;
	}

	/** Adiciona charge quando o hit for válido. (SERVER ONLY) (para o agressor) */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Catalyst")
	bool TryAddCatalystCharge_OnValidHit(AActor* TargetActor);

	/** Adiciona charge quando toma dano real. (SERVER ONLY) (para a vítima) */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Catalyst")
	bool TryAddCatalystCharge_OnDamageTaken(AActor* InstigatorActor);

	/** Tenta proc se estiver pronto. (SERVER ONLY) */
	UFUNCTION(BlueprintCallable, Category="SpellRise|Catalyst")
	void TryProcCatalystIfReady();

private:
	// Garantia de bind do listener, mesmo quando BeginPlay ocorrer antes do ActorInfo estar pronto.
	void EnsureCatalystChargeListenerBound_Server();

	void BindCatalystChargeListener_Server();
	void OnCatalystChargeChanged(const struct FOnAttributeChangeData& Data);

private:
	bool bCatalystListenerBound = false;
};
