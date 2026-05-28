#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffectTypes.h"
#include "SpellRiseEnemyEquipmentComponent.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class USkeletalMeshComponent;
class USpellRiseAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FSpellRiseEnemyEquipmentGrantedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|Equipment|GAS", meta=(AllowedClasses="/Script/GameplayAbilities.GameplayAbility", DisplayName="Ability"))
	TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SpellRise|Enemy|Equipment|GAS")
	bool bAutoActivateIfNoInputTag = false;
};

UCLASS(ClassGroup=(SpellRise), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class SPELLRISE_API USpellRiseEnemyEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellRiseEnemyEquipmentComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Enemy|Equipment")
	bool ApplyLoadout_Server();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SpellRise|Enemy|Equipment")
	void ClearLoadout_Server();

	UFUNCTION(BlueprintCallable, Category="SpellRise|Enemy|Equipment")
	bool RefreshWeaponAttachment();

	UFUNCTION(BlueprintPure, Category="SpellRise|Enemy|Equipment")
	AActor* GetEquippedWeaponActor() const { return EquippedWeaponActor; }

protected:
	UPROPERTY(Transient)
	TSoftClassPtr<AActor> WeaponActorClassRef;

	UPROPERTY(Transient)
	FName EquippedSocket = NAME_None;

	UPROPERTY(Transient)
	FName WeaponAttachComponentName = TEXT("Weapon");

	UPROPERTY(Transient)
	float WeaponNetUpdateFrequency = 10.0f;

	UPROPERTY(Transient)
	TArray<FSpellRiseEnemyEquipmentGrantedAbility> GrantedAbilities;

	UPROPERTY(Transient)
	TArray<TSoftClassPtr<UGameplayEffect>> GrantedEffectClasses;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeaponActor, BlueprintReadOnly, Category="SpellRise|Enemy|Equipment")
	TObjectPtr<AActor> EquippedWeaponActor = nullptr;

private:
	UFUNCTION()
	void OnRep_EquippedWeaponActor();

	USpellRiseAbilitySystemComponent* ResolveEnemyASC() const;
	USkeletalMeshComponent* ResolveAttachMesh(FName TargetSocket = NAME_None) const;
	USceneComponent* ResolveWeaponAttachComponent(AActor* WeaponActor) const;
	bool SpawnWeaponActor_Server();
	void DestroyWeaponActor_Server();
	bool AttachWeaponActor();
	void GrantAbilities_Server();
	void ApplyEffects_Server();
	void ClearGrantedAbilities_Server();
	void ClearGrantedEffects_Server();
	void PrepareWeaponActorForAttachment(AActor* WeaponActor) const;

	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	UPROPERTY(Transient)
	TArray<FActiveGameplayEffectHandle> GrantedEffectHandles;

	UPROPERTY(Transient)
	bool bLoadoutApplied = false;
};
