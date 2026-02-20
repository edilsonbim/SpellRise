#include "SpellRiseAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"

static FGameplayTag TAG_State_Dead()
{
	return FGameplayTag::RequestGameplayTag(FName("State.Dead"), false);
}

static FGameplayTag TAG_State_Catalyst_Active()
{
	return FGameplayTag::RequestGameplayTag(FName("State.Catalyst.Active"), false);
}

USpellRiseAbilitySystemComponent::USpellRiseAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void USpellRiseAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// Não confie 100% em BeginPlay para bindar (ActorInfo pode não estar pronto ainda).
	if (IsOwnerActorAuthoritative())
	{
		EnsureCatalystChargeListenerBound_Server();
	}
}

// =========================================================
// Input bridge
// =========================================================
void USpellRiseAbilitySystemComponent::SR_AbilityInputPressed(int32 InputID)
{
	AbilityLocalInputPressed(InputID);
}

void USpellRiseAbilitySystemComponent::SR_AbilityInputReleased(int32 InputID)
{
	AbilityLocalInputReleased(InputID);
}

// =========================================================
// Input capture hooks
// =========================================================
void USpellRiseAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	InputPressedSpecHandles.AddUnique(Spec.Handle);
	InputHeldSpecHandles.AddUnique(Spec.Handle);
}

void USpellRiseAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	InputReleasedSpecHandles.AddUnique(Spec.Handle);
	InputHeldSpecHandles.Remove(Spec.Handle);
}

// =========================================================
// Input processing
// =========================================================
void USpellRiseAbilitySystemComponent::SR_ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

// ---------------------------------------------------------
// Ability Wheel helpers
// ---------------------------------------------------------
bool USpellRiseAbilitySystemComponent::SR_TryActivateAbilityByInputID(int32 InputID)
{
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.InputID == InputID)
		{
			return TryActivateAbility(Spec.Handle);
		}
	}
	return false;
}

USpellRiseGameplayAbility* USpellRiseAbilitySystemComponent::SR_GetSpellRiseAbilityForInputID(int32 InputID) const
{
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.InputID == InputID)
		{
			return Cast<USpellRiseGameplayAbility>(Spec.Ability);
		}
	}
	return nullptr;
}

void USpellRiseAbilitySystemComponent::SR_ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	AActor* Avatar = GetAvatarActor();
	APawn* AvatarPawn = Cast<APawn>(Avatar);

	// Só o pawn local processa input (evita lixo em proxies / server dedicado)
	if (!AvatarPawn || !AvatarPawn->IsLocallyControlled())
	{
		SR_ClearAbilityInput();
		return;
	}

	if (bGamePaused)
	{
		SR_ClearAbilityInput();
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reserve(InputPressedSpecHandles.Num());

	// Usado para evitar "Pressed" duplicado no mesmo frame (Pressed + Held)
	TSet<FGameplayAbilitySpecHandle> PressedThisFrame;
	PressedThisFrame.Reserve(InputPressedSpecHandles.Num());

	// 1) Pressed
	{
		const TArray<FGameplayAbilitySpecHandle> PressedSnapshot = InputPressedSpecHandles;

		for (const FGameplayAbilitySpecHandle& Handle : PressedSnapshot)
		{
			PressedThisFrame.Add(Handle);

			FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
			if (!Spec || !Spec->Ability)
			{
				continue;
			}

			if (Spec->IsActive())
			{
				// Alimenta o GAS local
				Super::AbilitySpecInputPressed(*Spec);

				// ✅ FIX: quando a GA já está ativa, o "pressed" precisa chegar no SERVER
				// para que o combo continue (WaitInputPress / ComboWindow no servidor).
				if (!IsOwnerActorAuthoritative() && Spec->Ability->bReplicateInputDirectly)
				{
					ServerSetInputPressed(Spec->Handle);
				}
			}
			else
			{
				AbilitiesToActivate.AddUnique(Handle);
			}
		}
	}

	// 2) Held
	{
		const TArray<FGameplayAbilitySpecHandle> HeldSnapshot = InputHeldSpecHandles;

		for (const FGameplayAbilitySpecHandle& Handle : HeldSnapshot)
		{
			// Evita duplicar o evento no mesmo frame (se já veio no Pressed)
			if (PressedThisFrame.Contains(Handle))
			{
				continue;
			}

			FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
			if (!Spec || !Spec->Ability)
			{
				continue;
			}

			if (Spec->IsActive())
			{
				// Held alimenta o GAS local (mantém comportamento de "segurar" se você tiver abilities assim)
				Super::AbilitySpecInputPressed(*Spec);
			}
		}
	}

	// 3) Activate
	for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToActivate)
	{
		const bool bAllowRemoteActivation = true;
		TryActivateAbility(Handle, bAllowRemoteActivation);
	}

	// 4) Released
	{
		const TArray<FGameplayAbilitySpecHandle> ReleasedSnapshot = InputReleasedSpecHandles;

		for (const FGameplayAbilitySpecHandle& Handle : ReleasedSnapshot)
		{
			FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
			if (!Spec || !Spec->Ability)
			{
				continue;
			}

			if (Spec->IsActive())
			{
				Super::AbilitySpecInputReleased(*Spec);

				// ✅ FIX: libera também no server quando a GA está ativa
				if (!IsOwnerActorAuthoritative() && Spec->Ability->bReplicateInputDirectly)
				{
					ServerSetInputReleased(Spec->Handle);
				}
			}
		}
	}

	// 5) Clear one-frame arrays
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

// =========================================================
// Catalyst (SINGLE SOURCE OF TRUTH)
// =========================================================
void USpellRiseAbilitySystemComponent::EnsureCatalystChargeListenerBound_Server()
{
	if (!IsOwnerActorAuthoritative())
	{
		return;
	}

	if (bCatalystListenerBound)
	{
		return;
	}

	// Exigimos Avatar para evitar bind prematuro demais
	if (!GetAvatarActor())
	{
		return;
	}

	BindCatalystChargeListener_Server();
	bCatalystListenerBound = true;
}

void USpellRiseAbilitySystemComponent::BindCatalystChargeListener_Server()
{
	GetGameplayAttributeValueChangeDelegate(
		UCatalystAttributeSet::GetCatalystChargeAttribute()
	).AddUObject(this, &USpellRiseAbilitySystemComponent::OnCatalystChargeChanged);
}

void USpellRiseAbilitySystemComponent::OnCatalystChargeChanged(const FOnAttributeChangeData& Data)
{
	if (!IsOwnerActorAuthoritative())
	{
		return;
	}

	// Detecta cruzamento para "ready" (100)
	const bool bCrossedToReady = (Data.OldValue < 100.f && Data.NewValue >= 100.f);
	if (!bCrossedToReady)
	{
		return;
	}

	TryProcCatalystIfReady();
}

// ---------------------------------------------------------
// Agressor ganha charge quando causa dano real
// ---------------------------------------------------------
bool USpellRiseAbilitySystemComponent::TryAddCatalystCharge_OnValidHit(AActor* TargetActor)
{
	if (!IsOwnerActorAuthoritative())
	{
		return false;
	}

	EnsureCatalystChargeListenerBound_Server();

	AActor* SourceActor = GetAvatarActor();
	if (!SourceActor || !TargetActor || TargetActor == SourceActor)
	{
		return false;
	}

	// Se o próprio source está morto, não acumula
	if (HasMatchingGameplayTag(TAG_State_Dead()))
	{
		return false;
	}

	// Se já está ativo, não acumula (um proc por vez)
	if (HasMatchingGameplayTag(TAG_State_Catalyst_Active()))
	{
		return false;
	}

	// Não ganha charge batendo em alvo morto
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		if (TargetASC->HasMatchingGameplayTag(TAG_State_Dead()))
		{
			return false;
		}
	}

	if (!GE_CatalystAddCharge)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[CAT] GE_CatalystAddCharge is NULL on %s (Owner=%s Avatar=%s). Set it on the ASC component defaults in the Character BP."),
			*GetNameSafe(this),
			*GetNameSafe(GetOwnerActor()),
			*GetNameSafe(GetAvatarActor()));
		return false;
	}

	FGameplayEffectContextHandle Ctx = MakeEffectContext();
	Ctx.AddSourceObject(SourceActor);

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(GE_CatalystAddCharge, 1.f, Ctx);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[CAT] MakeOutgoingSpec failed for GE_CatalystAddCharge on %s"), *GetNameSafe(SourceActor));
		return false;
	}

	ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return true;
}

// ---------------------------------------------------------
// Vítima ganha charge quando toma dano real (anti-zerg)
// ---------------------------------------------------------
bool USpellRiseAbilitySystemComponent::TryAddCatalystCharge_OnDamageTaken(AActor* InstigatorActor)
{
	if (!IsOwnerActorAuthoritative())
	{
		return false;
	}

	EnsureCatalystChargeListenerBound_Server();

	AActor* VictimActor = GetAvatarActor();
	if (!VictimActor)
	{
		return false;
	}

	// Se a vítima está morta, não acumula
	if (HasMatchingGameplayTag(TAG_State_Dead()))
	{
		return false;
	}

	// Se já está ativo, não acumula (um proc por vez)
	if (HasMatchingGameplayTag(TAG_State_Catalyst_Active()))
	{
		return false;
	}

	// Evita auto-hit
	if (InstigatorActor && InstigatorActor == VictimActor)
	{
		return false;
	}

	if (!GE_CatalystAddCharge)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[CAT] GE_CatalystAddCharge is NULL on %s (Owner=%s Avatar=%s). Set it on the ASC component defaults in the Character BP."),
			*GetNameSafe(this),
			*GetNameSafe(GetOwnerActor()),
			*GetNameSafe(GetAvatarActor()));
		return false;
	}

	FGameplayEffectContextHandle Ctx = MakeEffectContext();
	// Para dano tomado, SourceObject útil é o instigator (se existir), senão a própria vítima
	Ctx.AddSourceObject(InstigatorActor ? InstigatorActor : VictimActor);

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(GE_CatalystAddCharge, 1.f, Ctx);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[CAT] MakeOutgoingSpec failed for GE_CatalystAddCharge on victim %s"), *GetNameSafe(VictimActor));
		return false;
	}

	ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return true;
}

void USpellRiseAbilitySystemComponent::TryProcCatalystIfReady()
{
	if (!IsOwnerActorAuthoritative())
	{
		return;
	}

	EnsureCatalystChargeListenerBound_Server();

	// Evita proc duplicado por tag
	if (HasMatchingGameplayTag(TAG_State_Catalyst_Active()))
	{
		return;
	}

	if (!CatalystProcAbilityClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CATALYST] CatalystProcAbilityClass not set."));
		return;
	}

	const float CurrentCharge = GetNumericAttribute(UCatalystAttributeSet::GetCatalystChargeAttribute());
	if (CurrentCharge < 100.f)
	{
		return;
	}

	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(CatalystProcAbilityClass);
	if (!Spec)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CATALYST] Proc ability not granted on ASC."));
		return;
	}

	const bool bAllowRemoteActivation = true;
	const bool bActivated = TryActivateAbility(Spec->Handle, bAllowRemoteActivation);

	if (bActivated)
	{
		if (ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(GetAvatarActor()))
		{
			int32 Tier = 1;
			if (const UCatalystAttributeSet* CatAS = Character->GetCatalystAttributeSet())
			{
				Tier = FMath::Clamp(FMath::RoundToInt(CatAS->GetCatalystLevel()), 1, 3);
			}
			Character->MultiOnCatalystProc(Tier);
		}
	}
}

// =========================================================
// Ability replication change tracking
// =========================================================
void USpellRiseAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	AActor* Avatar = GetAvatarActor();
	ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(Avatar);
	if (!Character)
	{
		return;
	}

	bool bAbilitiesChanged = (LastActivatableAbilities.Num() != ActivatableAbilities.Items.Num());

	if (!bAbilitiesChanged)
	{
		for (int32 i = 0; i < ActivatableAbilities.Items.Num(); i++)
		{
			if (LastActivatableAbilities.IsValidIndex(i) &&
				LastActivatableAbilities[i].Ability != ActivatableAbilities.Items[i].Ability)
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
