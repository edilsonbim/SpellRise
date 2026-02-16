// ============================================================================
// AnimNotify_SpellCastPause.h
// Path: Source/SpellRise/Combat/Spells/Cosmetic/AnimNotifies/AnimNotify_SpellCastPause.h
// ============================================================================

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_SpellCastPause.generated.h"

UCLASS(meta = (DisplayName = "Spell Cast Pause"))
class SPELLRISE_API UAnimNotify_SpellCastPause : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_SpellCastPause();

	/** Tag disparado quando o cast chega no ponto de pause/hold. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellCast")
	FGameplayTag EventTag;

	/** Ativar logs de debug */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpellCast|Debug")
	bool bDebug = false;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
						const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
};
