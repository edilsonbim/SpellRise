#include "AnimNotify_ComboWindowBegin.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

UAnimNotify_ComboWindowBegin::UAnimNotify_ComboWindowBegin()
{
    EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.Begin"), false);
    ComboIndex = 0;
    bDebug = false;
}

static UAbilitySystemComponent* SR_FindASCFromOwnerChain(AActor* Owner)
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

void UAnimNotify_ComboWindowBegin::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    // Apenas o servidor envia o evento (autoritativo)
    if (!Owner->HasAuthority()) return;

    UAbilitySystemComponent* ASC = SR_FindASCFromOwnerChain(Owner);
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
        UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] 🟢 BEGIN - Hit: %d, Owner: %s, Tag: %s"),
               ComboIndex, *GetNameSafe(Owner), *EventTag.ToString());
    }
}

FString UAnimNotify_ComboWindowBegin::GetNotifyName_Implementation() const
{
    return FString::Printf(TEXT("Combo Begin [%d]"), ComboIndex);
}