// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent_NiagaraText.h"

#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "SpellRise/Feedback/NumberPops/SpellRiseDamagePopStyleNiagara.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SpellRiseNumberPopComponent_NiagaraText)

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseNumberPopNiagaraRuntime, Log, All);

USpellRiseNumberPopComponent_NiagaraText::USpellRiseNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USpellRiseNumberPopComponent_NiagaraText::AddNumberPop(const FSpellRiseNumberPopRequest& NewRequest)
{

	if (APlayerController* PC = GetController<APlayerController>())
	{
		if (!PC->IsLocalController())
		{
			return;
		}
	}

	if (!Style)
	{
		return;
	}

	if (!Style->TextNiagara)
	{
		return;
	}

	if (Style->NiagaraArrayName.IsNone())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	int32 LocalDamage = NewRequest.NumberToDisplay;
	if (NewRequest.bIsCriticalDamage)
	{
		LocalDamage *= -1;
	}


	if (!IsValid(NiagaraComp) || NiagaraComp->GetAsset() != Style->TextNiagara)
	{
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			Style->TextNiagara,
			NewRequest.WorldLocation,
			FRotator::ZeroRotator,
			FVector::OneVector,
			false,
			false,
			ENCPoolMethod::None,
			true);

		if (!NiagaraComp)
		{
			return;
		}
	}

	NiagaraComp->SetWorldLocation(NewRequest.WorldLocation);

	if (bCompensateScaleByDistance)
	{
		float DistanceToView = ReferenceDistance;
		float FovScale = 1.0f;
		if (APlayerController* PC = GetController<APlayerController>())
		{
			if (const APlayerCameraManager* CameraManager = PC->PlayerCameraManager)
			{
				DistanceToView = FVector::Distance(CameraManager->GetCameraLocation(), NewRequest.WorldLocation);

				if (bCompensateScaleByFOV)
				{
					const float SafeCurrentFOV = FMath::Max(1.0f, CameraManager->GetFOVAngle());
					const float SafeReferenceFOV = FMath::Clamp(ReferenceFOV, 1.0f, 170.0f);
					const float CurrentHalfFOVRad = FMath::DegreesToRadians(SafeCurrentFOV * 0.5f);
					const float ReferenceHalfFOVRad = FMath::DegreesToRadians(SafeReferenceFOV * 0.5f);
					const float CurrentFovTan = FMath::Max(0.001f, FMath::Tan(CurrentHalfFOVRad));
					const float ReferenceFovTan = FMath::Max(0.001f, FMath::Tan(ReferenceHalfFOVRad));
					FovScale = CurrentFovTan / ReferenceFovTan;
				}
			}
			else if (const APawn* ControlledPawn = PC->GetPawn())
			{
				DistanceToView = FVector::Distance(ControlledPawn->GetPawnViewLocation(), NewRequest.WorldLocation);
			}
		}

		const float SafeReferenceDistance = FMath::Max(1.0f, ReferenceDistance);
		const float DistanceScale = (DistanceToView / SafeReferenceDistance) * FovScale;
		const float FinalScale = bExactScreenSizeCompensation
			? DistanceScale
			: FMath::Clamp(DistanceScale, MinWorldScale, MaxWorldScale);
		NiagaraComp->SetWorldScale3D(FVector(FinalScale));
	}

	NiagaraComp->Activate(false);

	TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(
		NiagaraComp,
		Style->NiagaraArrayName);

	DamageList.Add(FVector4(
		NewRequest.WorldLocation.X,
		NewRequest.WorldLocation.Y,
		NewRequest.WorldLocation.Z,
		static_cast<float>(LocalDamage)));

	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(
		NiagaraComp,
		Style->NiagaraArrayName,
		DamageList);
}
