#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_MeleeHitWindowBegin.generated.h"

class USpellRiseMeleeComponent;

UCLASS(meta = (DisplayName = "Melee Hit Window Begin"))
class SPELLRISE_API UAnimNotify_MeleeHitWindowBegin : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_MeleeHitWindowBegin();

	/** Índice do combo (0,1,2) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	int32 ComboIndex = 0;

	/** Ativar debug visual (linhas do trace) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee|Debug")
	bool bDebugVisual = false;

	/** Ativar logs de debug */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee|Debug")
	bool bDebug = false;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
						const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

private:
	USpellRiseMeleeComponent* GetMeleeComponent(AActor* Actor) const;
};