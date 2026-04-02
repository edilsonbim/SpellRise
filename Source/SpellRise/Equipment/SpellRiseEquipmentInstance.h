#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SpellRiseEquipmentInstance.generated.h"

class UEquippableItem;
class USpellRiseEquipmentManagerComponent;

UCLASS(BlueprintType)
class SPELLRISE_API USpellRiseEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	virtual bool IsSupportedForNetworking() const override { return true; }

	void Initialize(USpellRiseEquipmentManagerComponent* InOwnerComponent, UEquippableItem* InSourceItem);

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	UEquippableItem* GetSourceItem() const { return SourceItem; }

	UFUNCTION(BlueprintPure, Category="SpellRise|Equipment")
	USpellRiseEquipmentManagerComponent* GetOwnerComponent() const { return OwnerComponent.Get(); }

	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	virtual void OnEquipped();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Equipment")
	virtual void OnUnequipped();

private:
	UPROPERTY()
	TObjectPtr<UEquippableItem> SourceItem = nullptr;

	UPROPERTY()
	TWeakObjectPtr<USpellRiseEquipmentManagerComponent> OwnerComponent;
};
