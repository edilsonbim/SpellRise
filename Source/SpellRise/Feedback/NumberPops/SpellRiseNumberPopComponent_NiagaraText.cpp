#include "SpellRise/Feedback/NumberPops/SpellRiseNumberPopComponent_NiagaraText.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "SpellRise/Feedback/NumberPops/SpellRiseDamagePopStyleNiagara.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SpellRiseNumberPopComponent_NiagaraText)

USpellRiseNumberPopComponent_NiagaraText::USpellRiseNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USpellRiseNumberPopComponent_NiagaraText::AddNumberPop(const FSpellRiseNumberPopRequest& NewRequest)
{
	UE_LOG(LogTemp, Warning, TEXT("[POP][Niagara] Num=%d Crit=%d Owner=%s Style=%s Niagara=%s Array=%s"),
		NewRequest.NumberToDisplay,
		NewRequest.bIsCriticalDamage ? 1 : 0,
		*GetNameSafe(GetOwner()),
		*GetNameSafe(Style),
		Style && Style->TextNiagara ? *GetNameSafe(Style->TextNiagara) : TEXT("NULL"),
		Style ? *Style->NiagaraArrayName.ToString() : TEXT("NULL"));

	// Em listen server evita duplicar number pop em controllers remotos
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

	int32 LocalDamage = NewRequest.NumberToDisplay;

	// Mesmo padrão do Lyra:
	// crítico vira negativo para o Niagara diferenciar visualmente
	if (NewRequest.bIsCriticalDamage)
	{
		LocalDamage *= -1;
	}

	// Cria um Niagara component persistente se ainda não existir
	if (!NiagaraComp)
	{
		NiagaraComp = NewObject<UNiagaraComponent>(GetOwner());
		if (!NiagaraComp)
		{
			return;
		}

		NiagaraComp->SetAsset(Style->TextNiagara);
		NiagaraComp->bAutoActivate = false;
		NiagaraComp->SetupAttachment(nullptr);
		NiagaraComp->RegisterComponent();
	}

	NiagaraComp->Activate(false);
	NiagaraComp->SetWorldLocation(NewRequest.WorldLocation);

	// Cada hit é empacotado em Vector4:
	// XYZ = WorldLocation
	// W   = Damage (negativo = crítico)
	TArray<FVector4> DamageList =
		UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(
			NiagaraComp,
			Style->NiagaraArrayName
		);

	DamageList.Add(FVector4(
		NewRequest.WorldLocation.X,
		NewRequest.WorldLocation.Y,
		NewRequest.WorldLocation.Z,
		static_cast<float>(LocalDamage)
	));

	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(
		NiagaraComp,
		Style->NiagaraArrayName,
		DamageList
	);

	// DEBUG: spawn temporário isolado para validar runtime do NS_DamageNumbers
	UNiagaraComponent* DebugComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		Style->TextNiagara,
		NewRequest.WorldLocation,
		FRotator::ZeroRotator,
		FVector(1.f),
		true,
		true,
		ENCPoolMethod::None,
		true
	);

	if (DebugComp)
	{
		TArray<FVector4> DebugDamageList;
		DebugDamageList.Add(FVector4(
			NewRequest.WorldLocation.X,
			NewRequest.WorldLocation.Y,
			NewRequest.WorldLocation.Z,
			static_cast<float>(LocalDamage)
		));

		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(
			DebugComp,
			Style->NiagaraArrayName,
			DebugDamageList
		);

		UE_LOG(LogTemp, Warning, TEXT("[POP][DebugSpawn] Spawned debug Niagara with 1 entry"));
	}
}