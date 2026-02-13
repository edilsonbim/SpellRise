#include "AnimNotify_ComboWindowEnd.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

UAnimNotify_ComboWindowEnd::UAnimNotify_ComboWindowEnd()
{
    EventTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.End"), false);
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

void UAnimNotify_ComboWindowEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

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
        UE_LOG(LogTemp, Warning, TEXT("[Combo][SERVER] 🔴 END - Hit: %d, Owner: %s, Tag: %s"),
               ComboIndex, *GetNameSafe(Owner), *EventTag.ToString());
    }
}

FString UAnimNotify_ComboWindowEnd::GetNotifyName_Implementation() const
{
    return FString::Printf(TEXT("Combo End [%d]"), ComboIndex);
}