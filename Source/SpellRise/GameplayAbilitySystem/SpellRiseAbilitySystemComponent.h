#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SpellRiseAbilitySystemComponent.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

protected:
	TArray<FGameplayAbilitySpec> LastActivatableAbilities;

public:
	USpellRiseAbilitySystemComponent();

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_ActivateAbilities() override;

public:
	// -------------------------
	// Catalyst
	// -------------------------

	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Catalyst")
	TSubclassOf<UGameplayEffect> GE_CatalystAddCharge;

	// GA que faz o proc (aplica buff e reseta charge)
	UPROPERTY(EditDefaultsOnly, Category="SpellRise|Catalyst")
	TSubclassOf<UGameplayAbility> CatalystProcAbilityClass;

	FORCEINLINE TSubclassOf<UGameplayAbility> GetCatalystProcAbilityClass() const
	{
		return CatalystProcAbilityClass;
	}

	UFUNCTION(BlueprintCallable, Category="SpellRise|Catalyst")
	bool TryAddCatalystCharge_OnValidHit(AActor* TargetActor);

	// Pode ser chamado pelo AttributeSet ou via BP pra teste
	UFUNCTION(BlueprintCallable, Category="SpellRise|Catalyst")
	void TryProcCatalystIfReady();

private:
	// Bind do atributo CatalystCharge no server
	void BindCatalystChargeListener_Server();

	// Callback quando CatalystCharge muda
	void OnCatalystChargeChanged(const struct FOnAttributeChangeData& Data);
};
