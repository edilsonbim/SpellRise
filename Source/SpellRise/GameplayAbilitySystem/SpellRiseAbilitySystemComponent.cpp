// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellRiseAbilitySystemComponent.h"

#include "Characters/SpellRiseCharacterBase.h"


// Sets default values for this component's properties
USpellRiseAbilitySystemComponent::USpellRiseAbilitySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USpellRiseAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void USpellRiseAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	
	AActor* Avatar = GetAvatarActor();
	ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(Avatar);

	if (!Character) return;
	
	bool bAbilitiesChanged = false;
	if (LastActivatableAbilities.Num() != ActivatableAbilities.Items.Num())
	{
		bAbilitiesChanged = true;
	} else
	{
		for (int32 i = 0; i < ActivatableAbilities.Items.Num(); i++)
		{
			if (LastActivatableAbilities[i].Ability != ActivatableAbilities.Items[i].Ability)
				{
				bAbilitiesChanged = true;
				break;
			}
		}
	
		
	}
	if (bAbilitiesChanged)
	{
		Character->SendAbilitiesChangedEvent();
		LastActivatableAbilities = ActivatableAbilities.Items;
	}
	

}


// Called every frame
void USpellRiseAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

