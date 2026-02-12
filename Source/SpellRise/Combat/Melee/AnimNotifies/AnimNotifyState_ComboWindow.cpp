#include "AnimNotifyState_ComboWindow.h"

// ESSENCIAL - FALTANDO ESTE INCLUDE!
#include "Components/SkeletalMeshComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"

UAnimNotifyState_ComboWindow::UAnimNotifyState_ComboWindow()
{
	EventBeginTag = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.Begin"), /*ErrorIfNotFound=*/false);
	EventEndTag   = FGameplayTag::RequestGameplayTag(TEXT("Event.Melee.ComboWindow.End"),   /*ErrorIfNotFound=*/false);
}

static UAbilitySystemComponent* SR_FindASCFromOwnerChain(AActor* Owner)
{
	if (!Owner)
	{
		return nullptr;
	}

	// 1) Direct owner
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
	{
		return ASC;
	}

	// 2) Try Owner->GetOwner() (common when mesh lives in equipment/child actor)
	if (AActor* Outer = Owner->GetOwner())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Outer))
		{
			return ASC;
		}

		// 3) One more level (rare but helps with nested ownership)
		if (AActor* Outer2 = Outer->GetOwner())
		{
			return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Outer2);
		}
	}

	return nullptr;
}

void UAnimNotifyState_ComboWindow::SendGameplayEventIfASC(AActor* Owner, const FGameplayTag& Tag) const
{
	if (!Owner || !Tag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = SR_FindASCFromOwnerChain(Owner);
	if (!ASC)
	{
		return;
	}

	FGameplayEventData Data;
	Data.EventTag = Tag;
	Data.Instigator = Owner;
	Data.Target = Owner;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, Tag, Data);
}

bool UAnimNotifyState_ComboWindow::ShouldSendBegin(const USkeletalMeshComponent* MeshComp) const
{
	const uint64 Frame = GFrameCounter;

	if (LastMesh_Begin.Get() == MeshComp && LastFrame_Begin == Frame)
	{
		return false;
	}

	LastMesh_Begin = MeshComp;
	LastFrame_Begin = Frame;
	return true;
}

bool UAnimNotifyState_ComboWindow::ShouldSendEnd(const USkeletalMeshComponent* MeshComp) const
{
	const uint64 Frame = GFrameCounter;

	if (LastMesh_End.Get() == MeshComp && LastFrame_End == Frame)
	{
		return false;
	}

	LastMesh_End = MeshComp;
	LastFrame_End = Frame;
	return true;
}

void UAnimNotifyState_ComboWindow::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		return;
	}

	if (!ShouldSendBegin(MeshComp))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	SendGameplayEventIfASC(Owner, EventBeginTag);
}

void UAnimNotifyState_ComboWindow::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	if (!ShouldSendEnd(MeshComp))
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	SendGameplayEventIfASC(Owner, EventEndTag);
}