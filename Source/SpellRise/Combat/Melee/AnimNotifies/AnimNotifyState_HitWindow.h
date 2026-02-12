#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Engine/EngineTypes.h"
#include "AnimNotifyState_HitWindow.generated.h"

// Forward declarations
class USceneComponent;
class USkeletalMeshComponent;
class UAbilitySystemComponent;
class UWeaponTraceComponent;

// Estrutura de configuração do trace (deve estar definida no WeaponTraceComponent.h)
struct FWeaponTraceConfig;

UCLASS(meta = (DisplayName = "Hit Window"))
class SPELLRISE_API UAnimNotifyState_HitWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_HitWindow();

	// ---------------------------------------------------------
	// Configuração do Trace
	// ---------------------------------------------------------
	
	/** Componente da arma (opcional - se não setado, tenta resolver automaticamente) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	TObjectPtr<USceneComponent> WeaponComponent = nullptr;

	/** Socket base da arma (início do trace) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FName SocketBase = NAME_None;

	/** Socket tip da arma (fim do trace) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FName SocketTip = NAME_None;

	/** Raio do trace */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float Radius = 10.f;

	/** Canal de colisão para o trace */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	TEnumAsByte<ETraceTypeQuery> TraceChannel = TraceTypeQuery1;

	/** Desenhar debug? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebug = false;

	/** Tempo de vida do debug (segundos) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (ClampMin = "0", EditCondition = "bDrawDebug"))
	float DebugLifeTime = 2.f;

protected:
	/**
	 * Resolve qual SceneComponent usar para o trace.
	 * Ordem: 1) WeaponComponent setado manualmente, 2) WeaponActor->Mesh, 3) MeshComp (fallback)
	 */
	UFUNCTION(BlueprintCallable, Category = "Trace")
	USceneComponent* ResolveWeaponComponent(AActor* Owner, USkeletalMeshComponent* MeshComp) const;

	// ---------------------------------------------------------
	// Notify overrides
	// ---------------------------------------------------------
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference
	) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
};