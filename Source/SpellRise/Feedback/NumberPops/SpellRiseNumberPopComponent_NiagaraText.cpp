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
	UE_LOG(
		LogSpellRiseNumberPopNiagaraRuntime,
		VeryVerbose,
		TEXT("[POP][Niagara] Num=%d Crit=%d Owner=%s Style=%s Niagara=%s Array=%s"),
		NewRequest.NumberToDisplay,
		NewRequest.bIsCriticalDamage ? 1 : 0,
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Style),
		Style && Style->TextNiagara ? *GetNameSafe(Style->TextNiagara) : TEXT("NULL"),
		Style ? *Style->NiagaraArrayName.ToString() : TEXT("NULL"));

	// Em listen server evita duplicar number pop em controllers remotos.
	if (APlayerController* PC = GetController<APlayerController>())
	{
		if (!PC->IsLocalController())
		{
			return;
		}
	}

	if (!Style)
	{
		UE_LOG(LogSpellRiseNumberPopNiagaraRuntime, Warning, TEXT("[POP][Niagara] Style null on %s"), *GetNameSafe(this));
		return;
	}

	if (!Style->TextNiagara)
	{
		UE_LOG(LogSpellRiseNumberPopNiagaraRuntime, Warning, TEXT("[POP][Niagara] TextNiagara null on Style=%s"), *GetNameSafe(Style));
		return;
	}

	if (Style->NiagaraArrayName.IsNone())
	{
		UE_LOG(LogSpellRiseNumberPopNiagaraRuntime, Warning, TEXT("[POP][Niagara] NiagaraArrayName is None on Style=%s"), *GetNameSafe(Style));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogSpellRiseNumberPopNiagaraRuntime, Warning, TEXT("[POP][Niagara] World null on %s"), *GetNameSafe(this));
		return;
	}

	int32 LocalDamage = NewRequest.NumberToDisplay;
	if (NewRequest.bIsCriticalDamage)
	{
		LocalDamage *= -1;
	}

	// Garante componente valido no mundo com o sistema correto.
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
			UE_LOG(
				LogSpellRiseNumberPopNiagaraRuntime,
				Warning,
				TEXT("[POP][Niagara] Failed to spawn NiagaraComp. Style=%s Niagara=%s"),
				*GetNameSafe(Style),
				*GetNameSafe(Style->TextNiagara));
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
