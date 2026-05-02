// Cabeçalho de implementação: executa a lógica runtime preservando autoridade do servidor e integração Unreal.
#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent_NiagaraText.h"

#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
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
			FVector(FMath::Max(0.01f, Style->WorldScale)),
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
	NiagaraComp->SetWorldScale3D(FVector(FMath::Max(0.01f, Style->WorldScale)));
	if (!Style->NumberColorParameterName.IsNone())
	{
		NiagaraComp->SetVariableLinearColor(
			Style->NumberColorParameterName,
			NewRequest.bIsCriticalDamage ? Style->CriticalColor : Style->NormalColor);
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
