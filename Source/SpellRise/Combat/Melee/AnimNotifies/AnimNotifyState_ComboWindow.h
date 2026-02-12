#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"

#include "AnimNotifyState_ComboWindow.generated.h"

UCLASS(meta=(DisplayName="SpellRise Combo Window"))
class SPELLRISE_API UAnimNotifyState_ComboWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_ComboWindow();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ComboWindow")
	FGameplayTag EventBeginTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ComboWindow")
	FGameplayTag EventEndTag;

	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference
	) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;

private:
	// Dedup per-mesh (begin/end) per-frame
	mutable TWeakObjectPtr<const USkeletalMeshComponent> LastMesh_Begin;
	mutable uint64 LastFrame_Begin = 0;

	mutable TWeakObjectPtr<const USkeletalMeshComponent> LastMesh_End;
	mutable uint64 LastFrame_End = 0;

	void SendGameplayEventIfASC(AActor* Owner, const FGameplayTag& Tag) const;

	bool ShouldSendBegin(const USkeletalMeshComponent* MeshComp) const;
	bool ShouldSendEnd(const USkeletalMeshComponent* MeshComp) const;
};
