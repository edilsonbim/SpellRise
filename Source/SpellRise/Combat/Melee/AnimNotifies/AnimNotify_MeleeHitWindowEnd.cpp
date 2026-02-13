#include "AnimNotify_MeleeHitWindowEnd.h"
#include "SpellRise/Combat/Melee/Components/SpellRiseMeleeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

UAnimNotify_MeleeHitWindowEnd::UAnimNotify_MeleeHitWindowEnd()
{
	ComboIndex = 0;
	bDebug = false;
}

USpellRiseMeleeComponent* UAnimNotify_MeleeHitWindowEnd::GetMeleeComponent(AActor* Actor) const
{
	if (!Actor) return nullptr;
	return Actor->FindComponentByClass<USpellRiseMeleeComponent>();
}

void UAnimNotify_MeleeHitWindowEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
										   const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (USpellRiseMeleeComponent* MeleeComp = GetMeleeComponent(Owner))
	{
		MeleeComp->EndHitWindow();

		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MeleeHit] 🔴 END - Combo: %d, Owner: %s"),
				   ComboIndex, *GetNameSafe(Owner));
		}
	}
}

FString UAnimNotify_MeleeHitWindowEnd::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Melee Hit End [%d]"), ComboIndex);
}