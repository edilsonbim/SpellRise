#include "AnimNotify_MeleeHitWindowBegin.h"
#include "SpellRise/Combat/Melee/Components/SpellRiseMeleeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

UAnimNotify_MeleeHitWindowBegin::UAnimNotify_MeleeHitWindowBegin()
{
	ComboIndex = 0;
	bDebugVisual = false;
	bDebug = false;
}

USpellRiseMeleeComponent* UAnimNotify_MeleeHitWindowBegin::GetMeleeComponent(AActor* Actor) const
{
	if (!Actor) return nullptr;
	return Actor->FindComponentByClass<USpellRiseMeleeComponent>();
}

void UAnimNotify_MeleeHitWindowBegin::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
											 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	// O Hit Window deve ser ativado em **todos** os lugares (servidor e clientes)
	// porque o MeleeComponent é replicado e o trace é contínuo.
	// Mas o dano só será aplicado no servidor (dentro do MeleeComponent).
	if (USpellRiseMeleeComponent* MeleeComp = GetMeleeComponent(Owner))
	{
		MeleeComp->SetDebugVisual(bDebugVisual);
		MeleeComp->BeginHitWindow(ComboIndex);

		if (bDebug)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MeleeHit] 🟢 BEGIN - Combo: %d, Owner: %s"),
				   ComboIndex, *GetNameSafe(Owner));
		}
	}
}

FString UAnimNotify_MeleeHitWindowBegin::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Melee Hit Begin [%d]"), ComboIndex);
}