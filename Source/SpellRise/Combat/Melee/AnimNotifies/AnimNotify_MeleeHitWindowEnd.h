#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_MeleeHitWindowEnd.generated.h"

class USpellRiseMeleeComponent;

UCLASS(meta = (DisplayName = "Melee Hit Window End"))
class SPELLRISE_API UAnimNotify_MeleeHitWindowEnd : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_MeleeHitWindowEnd();

	/** Índice do combo (0,1,2) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	int32 ComboIndex = 0;

	/** Ativar logs de debug */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee|Debug")
	bool bDebug = false;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
						const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

private:
	USpellRiseMeleeComponent* GetMeleeComponent(AActor* Actor) const;
};