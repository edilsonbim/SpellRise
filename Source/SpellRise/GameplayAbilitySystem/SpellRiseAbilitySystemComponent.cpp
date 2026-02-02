#include "SpellRiseAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/SpellRiseCharacterBase.h"
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
}

void USpellRiseAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ✅ O proc automático deve ser decidido NO SERVER
	if (IsOwnerActorAuthoritative())
	{
		BindCatalystChargeListener_Server();
	}
}

void USpellRiseAbilitySystemComponent::BindCatalystChargeListener_Server()
{
	// Se o AttributeSet não existe ainda por algum motivo, o delegate ainda funciona,
	// mas o valor pode vir 0 até o ASC estar inicializado. Mesmo assim ok.

	GetGameplayAttributeValueChangeDelegate(
		UCatalystAttributeSet::GetCatalystChargeAttribute()
	).AddUObject(this, &USpellRiseAbilitySystemComponent::OnCatalystChargeChanged);
}

void USpellRiseAbilitySystemComponent::OnCatalystChargeChanged(const FOnAttributeChangeData& Data)
{
	// Server only
	if (!IsOwnerActorAuthoritative())
	{
		return;
	}

	// Só dispara quando cruza o limiar (ex: 95 -> 100)
	const bool bCrossedToReady = (Data.OldValue < 100.f && Data.NewValue >= 100.f);
	if (!bCrossedToReady)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[CATALYST] Charge crossed 100: Old=%.2f New=%.2f"), Data.OldValue, Data.NewValue);

	// ✅ tenta procar imediatamente (antes de qualquer reset)
	TryProcCatalystIfReady();
}

void USpellRiseAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	AActor* Avatar = GetAvatarActor();
	ASpellRiseCharacterBase* Character = Cast<ASpellRiseCharacterBase>(Avatar);
	if (!Character) return;

	bool bAbilitiesChanged = (LastActivatableAbilities.Num() != ActivatableAbilities.Items.Num());

	if (!bAbilitiesChanged)
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

bool USpellRiseAbilitySystemComponent::TryAddCatalystCharge_OnValidHit(AActor* TargetActor)
{
	if (!IsOwnerActorAuthoritative())
	{
		return false;
	}

	AActor* SourceActor = GetOwnerActor();
	if (!SourceActor || !TargetActor || TargetActor == SourceActor)
	{
		return false;
	}

	// não conta durante o proc ativo
	if (HasMatchingGameplayTag(TAG_State_Catalyst_Active()))
	{
		return false;
	}

	// não conta se alvo morto
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		if (TargetASC->HasMatchingGameplayTag(TAG_State_Dead()))
		{
			return false;
		}
	}

	if (!GE_CatalystAddCharge)
	{
		return false;
	}

	FGameplayEffectContextHandle Ctx = MakeEffectContext();
	Ctx.AddSourceObject(SourceActor);

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(GE_CatalystAddCharge, 1.f, Ctx);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return false;
	}

	ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	// ✅ NÃO chama TryProc aqui.
	// O delegate OnCatalystChargeChanged vai disparar quando cruzar 100.
	return true;
}

void USpellRiseAbilitySystemComponent::TryProcCatalystIfReady()
{
	// Server only
	if (!IsOwnerActorAuthoritative())
	{
		return;
	}

	// Não ativa se já está ativo
	if (HasMatchingGameplayTag(TAG_State_Catalyst_Active()))
	{
		UE_LOG(LogTemp, Warning, TEXT("[CATALYST] Skip proc: already active tag."));
		return;
	}

	if (!CatalystProcAbilityClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CATALYST] Skip proc: CatalystProcAbilityClass not set."));
		return;
	}

	const float CurrentCharge = GetNumericAttribute(UCatalystAttributeSet::GetCatalystChargeAttribute());
	if (CurrentCharge < 100.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CATALYST] Skip proc: Charge=%.2f < 100"), CurrentCharge);
		return;
	}

	// ✅ A habilidade precisa estar GRANTED no server (via Character/GrantAbilities/Startup)
	// Se manual funciona, ela provavelmente está.
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(CatalystProcAbilityClass);
	if (!Spec)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CATALYST] Proc ability not granted on ASC. Grant it on spawn/startup."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[CATALYST] Activating proc ability. Charge=%.2f"), CurrentCharge);

	// Ativa usando handle (mais confiável)
	const bool bActivated = TryActivateAbility(Spec->Handle, /*bAllowRemoteActivation*/ false);
	UE_LOG(LogTemp, Warning, TEXT("[CATALYST] TryActivateAbility result=%d"), bActivated ? 1 : 0);
}
