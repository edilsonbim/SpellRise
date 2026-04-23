// Cabeçalho de implementação: subclasses concretas para spells self-cast autoritativas.
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGA_AuthoritativeSelfEffects.h"

#include "UObject/SoftObjectPath.h"

namespace
{
	TSubclassOf<UGameplayEffect> LoadGameplayEffectClass(const TCHAR* AssetClassPath)
	{
		return FSoftClassPath(AssetClassPath).TryLoadClass<UGameplayEffect>();
	}

	void AddSelfEffect(
		TArray<FSpellRiseAuthoritativeSelfEffectEntry>& EffectEntries,
		const TCHAR* AssetClassPath,
		float LevelMultiplier = 1.0f)
	{
		FSpellRiseAuthoritativeSelfEffectEntry Entry;
		Entry.GameplayEffectClass = LoadGameplayEffectClass(AssetClassPath);
		Entry.LevelMultiplier = LevelMultiplier;
		EffectEntries.Add(Entry);
	}
}

USpellRiseGA_HealSelf::USpellRiseGA_HealSelf()
{
	AddSelfEffect(SelfAuthorityEffects, TEXT("/Game/GamePlayAbilitySystem/Effects/Transfers/GE_HealSelf.GE_HealSelf_C"));
}

USpellRiseGA_TransferToHealth::USpellRiseGA_TransferToHealth()
{
	AddSelfEffect(SelfAuthorityEffects, TEXT("/Game/GamePlayAbilitySystem/Effects/Transfers/GE_Transfer_toHealth.GE_Transfer_toHealth_C"));
}

USpellRiseGA_TransferToMana::USpellRiseGA_TransferToMana()
{
	AddSelfEffect(SelfAuthorityEffects, TEXT("/Game/GamePlayAbilitySystem/Effects/Transfers/GE_Transfer_toMana.GE_Transfer_toMana_C"));
}

USpellRiseGA_TransferToStamina::USpellRiseGA_TransferToStamina()
{
	AddSelfEffect(SelfAuthorityEffects, TEXT("/Game/GamePlayAbilitySystem/Effects/Transfers/GE_Transfer_toStamina.GE_Transfer_toStamina_C"));
}

USpellRiseGA_ShieldSelf::USpellRiseGA_ShieldSelf()
{
	AddSelfEffect(SelfAuthorityEffects, TEXT("/Game/GamePlayAbilitySystem/Effects/GE_GiveShield.GE_GiveShield_C"));
}
