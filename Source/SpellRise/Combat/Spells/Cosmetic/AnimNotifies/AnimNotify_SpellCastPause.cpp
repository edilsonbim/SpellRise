// ============================================================================
// AnimNotify_SpellCastPause.cpp
// Path: Source/SpellRise/Combat/Spells/Cosmetic/AnimNotifies/AnimNotify_SpellCastPause.cpp
// ============================================================================

#include "SpellRise/Combat/Spells/Cosmetic/AnimNotifies/AnimNotify_SpellCastPause.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"

UAnimNotify_SpellCastPause::UAnimNotify_SpellCastPause()
{
	EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Spell.CastPause"), false);
	bDebug = false;
}

static UAbilitySystemComponent* SR_FindASCFromOwnerChain_Spell(AActor* Owner)
{
	if (!Owner) return nullptr;

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
		return ASC;

	if (AActor* Outer = Owner->GetOwner())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Outer))
			return ASC;

		if (AActor* Outer2 = Outer->GetOwner())
			return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Outer2);
	}

	return nullptr;
}

static bool SR_ShouldSendSpellEvent(AActor* Owner)
{
	if (!Owner) return false;

	// Servidor sempre envia (autoritativo)
	if (Owner->HasAuthority())
		return true;

	// Owning client também envia localmente para WaitGameplayEvent funcionar no client
	if (const APawn* Pawn = Cast<APawn>(Owner))
	{
		return Pawn->IsLocallyControlled();
	}

	return false;
}

void UAnimNotify_SpellCastPause::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (!SR_ShouldSendSpellEvent(Owner))
		return;

	UAbilitySystemComponent* ASC = SR_FindASCFromOwnerChain_Spell(Owner);
	if (!ASC) return;

	FGameplayEventData Data;
	Data.EventTag = EventTag;
	Data.Instigator = Owner;
	Data.Target = Owner;
	Data.ContextHandle = ASC->MakeEffectContext();
	Data.EventMagnitude = 1.0f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, Data);

	if (bDebug)
	{
		const bool bLocal = (Cast<APawn>(Owner) && Cast<APawn>(Owner)->IsLocallyControlled());
		UE_LOG(LogTemp, Warning, TEXT("[SpellCast] PAUSE - Owner=%s Tag=%s Authority=%d Local=%d"),
			*GetNameSafe(Owner),
			*EventTag.ToString(),
			Owner->HasAuthority() ? 1 : 0,
			bLocal ? 1 : 0);
	}
}

FString UAnimNotify_SpellCastPause::GetNotifyName_Implementation() const
{
	return FString(TEXT("Spell Cast Pause"));
}
