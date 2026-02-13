#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_ComboWindowEnd.generated.h"

UCLASS(meta = (DisplayName = "Combo Window End"))
class SPELLRISE_API UAnimNotify_ComboWindowEnd : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_ComboWindowEnd();

	/** Tag de fim da janela de combo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboWindow")
	FGameplayTag EventTag;

	/** Índice do combo (0,1,2) para debug */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboWindow")
	int32 ComboIndex = 0;

	/** Ativar logs de debug */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboWindow|Debug")
	bool bDebug = false;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
						const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
};