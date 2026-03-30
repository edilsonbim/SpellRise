#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "GameplayTagContainer.h"
#include "SpellRiseNumberPopComponent.generated.h"

USTRUCT(BlueprintType)
struct FSpellRiseNumberPopRequest
{
	GENERATED_BODY()

	// Local no mundo onde o número deve aparecer
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Number Pops")
	FVector WorldLocation = FVector::ZeroVector;

	// Tags da origem/causa
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Number Pops")
	FGameplayTagContainer SourceTags;

	// Tags do alvo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Number Pops")
	FGameplayTagContainer TargetTags;

	// Valor a exibir
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Number Pops")
	int32 NumberToDisplay = 0;

	// Crítico ou não
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Number Pops")
	bool bIsCriticalDamage = false;
};

UCLASS(Abstract, Blueprintable)
class SPELLRISE_API USpellRiseNumberPopComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	USpellRiseNumberPopComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="SpellRise|Number Pops")
	virtual void AddNumberPop(const FSpellRiseNumberPopRequest& NewRequest);

protected:
	template<typename T>
	T* GetController() const
	{
		return Cast<T>(GetOwner());
	}
};