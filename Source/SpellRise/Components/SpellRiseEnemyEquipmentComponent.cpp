// Componente server-authoritative para loadout visual/GAS de inimigos sem inventario.
#include "SpellRise/Components/SpellRiseEnemyEquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpellRiseEnemyEquipment, Log, All);

USpellRiseEnemyEquipmentComponent::USpellRiseEnemyEquipmentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void USpellRiseEnemyEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		RefreshWeaponAttachment();
	}
}

void USpellRiseEnemyEquipmentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ClearLoadout_Server();
	}

	Super::EndPlay(EndPlayReason);
}

void USpellRiseEnemyEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquippedWeaponActor);
}

bool USpellRiseEnemyEquipmentComponent::ApplyLoadout_Server()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return false;
	}

	if (bLoadoutApplied)
	{
		return true;
	}

	const bool bWeaponReady = SpawnWeaponActor_Server();
	GrantAbilities_Server();
	ApplyEffects_Server();

	bLoadoutApplied = true;
	OwnerActor->ForceNetUpdate();
	return bWeaponReady || !GrantedAbilities.IsEmpty() || !GrantedEffectClasses.IsEmpty();
}

void USpellRiseEnemyEquipmentComponent::ClearLoadout_Server()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	ClearGrantedAbilities_Server();
	ClearGrantedEffects_Server();
	DestroyWeaponActor_Server();
	bLoadoutApplied = false;
	OwnerActor->ForceNetUpdate();
}

bool USpellRiseEnemyEquipmentComponent::RefreshWeaponAttachment()
{
	return AttachWeaponActor();
}

void USpellRiseEnemyEquipmentComponent::OnRep_EquippedWeaponActor()
{
	RefreshWeaponAttachment();
}

USpellRiseAbilitySystemComponent* USpellRiseEnemyEquipmentComponent::ResolveEnemyASC() const
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	return Cast<USpellRiseAbilitySystemComponent>(
		OwnerActor->FindComponentByClass<UAbilitySystemComponent>());
}

USkeletalMeshComponent* USpellRiseEnemyEquipmentComponent::ResolveAttachMesh(FName TargetSocket) const
{
	if (const ASpellRiseCharacterBase* SpellRiseCharacterOwner = Cast<ASpellRiseCharacterBase>(GetOwner()))
	{
		return SpellRiseCharacterOwner->GetEquipmentAttachMeshComponentForSocket(TargetSocket);
	}

	if (AActor* OwnerActor = GetOwner())
	{
		TArray<USkeletalMeshComponent*> SkeletalMeshes;
		OwnerActor->GetComponents<USkeletalMeshComponent>(SkeletalMeshes);
		for (USkeletalMeshComponent* MeshComp : SkeletalMeshes)
		{
			if (IsValid(MeshComp) && (TargetSocket == NAME_None || MeshComp->DoesSocketExist(TargetSocket)))
			{
				return MeshComp;
			}
		}
	}

	return nullptr;
}

USceneComponent* USpellRiseEnemyEquipmentComponent::ResolveWeaponAttachComponent(AActor* WeaponActor) const
{
	if (!IsValid(WeaponActor))
	{
		return nullptr;
	}

	USceneComponent* NamedWeaponAttachComponent = nullptr;
	if (!WeaponAttachComponentName.IsNone())
	{
		TArray<USceneComponent*> SceneComponents;
		WeaponActor->GetComponents<USceneComponent>(SceneComponents);
		for (USceneComponent* SceneComponent : SceneComponents)
		{
			if (IsValid(SceneComponent) && SceneComponent->GetFName() == WeaponAttachComponentName)
			{
				NamedWeaponAttachComponent = SceneComponent;
				break;
			}
		}
	}

	USceneComponent* RootComponent = WeaponActor->GetRootComponent();
	USceneComponent* SpawnPointComponent = nullptr;
	TArray<USceneComponent*> SceneComponents;
	WeaponActor->GetComponents<USceneComponent>(SceneComponents);
	for (USceneComponent* SceneComponent : SceneComponents)
	{
		if (IsValid(SceneComponent) && SceneComponent->GetFName() == TEXT("SpawnPoint"))
		{
			SpawnPointComponent = SceneComponent;
			break;
		}
	}

	if (IsValid(NamedWeaponAttachComponent))
	{
		if (IsValid(SpawnPointComponent) && !SpawnPointComponent->IsAttachedTo(NamedWeaponAttachComponent) && IsValid(RootComponent))
		{
			return RootComponent;
		}

		return NamedWeaponAttachComponent;
	}

	return WeaponActor->GetRootComponent();
}

bool USpellRiseEnemyEquipmentComponent::SpawnWeaponActor_Server()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return false;
	}

	UClass* ResolvedWeaponActorClass = WeaponActorClassRef.LoadSynchronous();
	if (!ResolvedWeaponActorClass)
	{
		return false;
	}

	if (IsValid(EquippedWeaponActor))
	{
		return AttachWeaponActor();
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerActor;
	SpawnParams.Instigator = Cast<APawn>(OwnerActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	EquippedWeaponActor = World->SpawnActor<AActor>(ResolvedWeaponActorClass, FTransform::Identity, SpawnParams);
	if (!EquippedWeaponActor)
	{
		UE_LOG(LogSpellRiseEnemyEquipment, Warning,
			TEXT("Falha ao spawnar arma de enemy. Owner=%s WeaponClass=%s"),
			*GetNameSafe(OwnerActor),
			*GetNameSafe(ResolvedWeaponActorClass));
		return false;
	}

	EquippedWeaponActor->SetReplicates(true);
	EquippedWeaponActor->SetReplicateMovement(false);
	EquippedWeaponActor->SetNetUpdateFrequency(FMath::Clamp(WeaponNetUpdateFrequency, 1.0f, 30.0f));

	const bool bAttached = AttachWeaponActor();
	EquippedWeaponActor->ForceNetUpdate();
	return bAttached;
}

void USpellRiseEnemyEquipmentComponent::DestroyWeaponActor_Server()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	AActor* WeaponActor = EquippedWeaponActor;
	EquippedWeaponActor = nullptr;

	if (IsValid(WeaponActor))
	{
		WeaponActor->Destroy();
	}
}

bool USpellRiseEnemyEquipmentComponent::AttachWeaponActor()
{
	AActor* WeaponActor = EquippedWeaponActor;
	if (!IsValid(WeaponActor))
	{
		return false;
	}

	USkeletalMeshComponent* AttachMesh = ResolveAttachMesh(EquippedSocket);
	USceneComponent* WeaponAttachComponent = ResolveWeaponAttachComponent(WeaponActor);
	if (!AttachMesh || !WeaponAttachComponent || EquippedSocket == NAME_None)
	{
		return false;
	}

	PrepareWeaponActorForAttachment(WeaponActor);

	const FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
	const bool bAttached = WeaponAttachComponent->AttachToComponent(AttachMesh, AttachRules, EquippedSocket);
	if (!bAttached)
	{
		UE_LOG(LogSpellRiseEnemyEquipment, Warning,
			TEXT("Attach de arma enemy falhou. Owner=%s Weapon=%s Mesh=%s Socket=%s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(WeaponActor),
			*GetNameSafe(AttachMesh),
			*EquippedSocket.ToString());
	}

	return bAttached;
}

void USpellRiseEnemyEquipmentComponent::GrantAbilities_Server()
{
	USpellRiseAbilitySystemComponent* ASC = ResolveEnemyASC();
	if (!GetOwner() || !GetOwner()->HasAuthority() || !ASC)
	{
		return;
	}

	for (const FSpellRiseEnemyEquipmentGrantedAbility& Grant : GrantedAbilities)
	{
		UClass* AbilityClass = Grant.AbilityClass.LoadSynchronous();
		if (!AbilityClass)
		{
			continue;
		}

		if (ASC->FindAbilitySpecFromClass(AbilityClass) != nullptr)
		{
			continue;
		}

		FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);

		const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
		if (Handle.IsValid())
		{
			GrantedAbilityHandles.AddUnique(Handle);
		}

		if (Handle.IsValid() && Grant.bAutoActivateIfNoInputTag)
		{
			ASC->TryActivateAbility(Handle);
		}
	}

	ASC->RefreshAbilityActorInfo();
}

void USpellRiseEnemyEquipmentComponent::ApplyEffects_Server()
{
	USpellRiseAbilitySystemComponent* ASC = ResolveEnemyASC();
	if (!GetOwner() || !GetOwner()->HasAuthority() || !ASC)
	{
		return;
	}

	for (const TSoftClassPtr<UGameplayEffect>& EffectClassRef : GrantedEffectClasses)
	{
		UClass* EffectClass = EffectClassRef.LoadSynchronous();
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, 1.0f, Context);
		if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
		{
			UE_LOG(LogSpellRiseEnemyEquipment, Warning,
				TEXT("Spec de GE de equipamento enemy invalido. Owner=%s Effect=%s"),
				*GetNameSafe(GetOwner()),
				*GetNameSafe(EffectClass));
			continue;
		}

		const FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		if (ActiveHandle.IsValid())
		{
			GrantedEffectHandles.Add(ActiveHandle);
		}
	}
}

void USpellRiseEnemyEquipmentComponent::ClearGrantedAbilities_Server()
{
	USpellRiseAbilitySystemComponent* ASC = ResolveEnemyASC();
	if (!GetOwner() || !GetOwner()->HasAuthority() || !ASC)
	{
		GrantedAbilityHandles.Reset();
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilityHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	GrantedAbilityHandles.Reset();
	ASC->RefreshAbilityActorInfo();
}

void USpellRiseEnemyEquipmentComponent::ClearGrantedEffects_Server()
{
	USpellRiseAbilitySystemComponent* ASC = ResolveEnemyASC();
	if (!GetOwner() || !GetOwner()->HasAuthority() || !ASC)
	{
		GrantedEffectHandles.Reset();
		return;
	}

	for (const FActiveGameplayEffectHandle& EffectHandle : GrantedEffectHandles)
	{
		if (EffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(EffectHandle);
		}
	}

	GrantedEffectHandles.Reset();
}

void USpellRiseEnemyEquipmentComponent::PrepareWeaponActorForAttachment(AActor* WeaponActor) const
{
	if (!IsValid(WeaponActor))
	{
		return;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	WeaponActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!IsValid(PrimitiveComponent))
		{
			continue;
		}

		if (PrimitiveComponent->IsSimulatingPhysics())
		{
			PrimitiveComponent->SetSimulatePhysics(false);
		}
	}

	WeaponActor->SetReplicateMovement(false);
}
