#pragma once

// Cabeçalho de interface: declara contratos, propriedades e pontos de integração Unreal.

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "GameplayTagContainer.h"
#include "SpellRiseNumberPopComponent.generated.h"

USTRUCT(BlueprintType)
struct FSpellRiseNumberPopRequest
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Number Pops")
	FVector WorldLocation = FVector::ZeroVector;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Number Pops")
	FGameplayTagContainer SourceTags;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Number Pops")
	FGameplayTagContainer TargetTags;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellRise|Number Pops")
	int32 NumberToDisplay = 0;


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
