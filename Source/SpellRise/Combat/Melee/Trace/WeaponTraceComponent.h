#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "WeaponTraceComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class USceneComponent;

USTRUCT(BlueprintType)
struct FWeaponTraceConfig
{
	GENERATED_BODY()

	// Componente que contém os sockets (geralmente o SkeletalMesh da arma)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponTrace")
	TObjectPtr<USceneComponent> WeaponComponent = nullptr;

	// Sockets usados para sweep (ex: BladeBase / BladeTip)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponTrace")
	FName SocketBase = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponTrace")
	FName SocketTip = NAME_None;

	// Raio do sweep (capsule/sphere)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponTrace", meta=(ClampMin="1.0"))
	float Radius = 12.f;

	// Canal de colisão
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponTrace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	// Desenhar debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponTrace")
	bool bDrawDebug = false;

	// Tempo do debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponTrace", meta=(ClampMin="0.0"))
	float DebugLifeTime = 0.1f;
};

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class SPELLRISE_API UWeaponTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponTraceComponent();

	// Abre janela e “arma” o trace. Ideal chamar no AnimNotifyState.
	UFUNCTION(BlueprintCallable, Category="WeaponTrace")
	void BeginTraceWindow(const FWeaponTraceConfig& InConfig, UAbilitySystemComponent* InSourceASC);

	// Fecha janela e limpa estado
	UFUNCTION(BlueprintCallable, Category="WeaponTrace")
	void EndTraceWindow();

	// Configura o “payload” do dano (GE + SetByCaller)
	UFUNCTION(BlueprintCallable, Category="WeaponTrace")
	void SetDamagePayload(TSubclassOf<UGameplayEffect> InDamageEffect, float InBaseDamage);

	// Para combos: permite limpar lista de alvos já atingidos
	UFUNCTION(BlueprintCallable, Category="WeaponTrace")
	void ResetHitActors();

	// Tag SetByCaller (editável em BP se quiser trocar no futuro)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponTrace|GAS")
	FGameplayTag SetByCallerDamageTag;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool GetSocketWorldLocation(const FName& Socket, FVector& OutLocation) const;
	void TraceStep();
	void ApplyDamageToActor(AActor* TargetActor);

private:
	bool bTracing = false;

	FWeaponTraceConfig Config;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffect;

	float BaseDamage = 0.f;

	// Para sweep contínuo (tunneling-free)
	FVector PrevBase = FVector::ZeroVector;
	FVector PrevTip  = FVector::ZeroVector;
	bool bHasPrev = false;

	// Evita hit múltiplo no mesmo swing
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> HitActors;
};
