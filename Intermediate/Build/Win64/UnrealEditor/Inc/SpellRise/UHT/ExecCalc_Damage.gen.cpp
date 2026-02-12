// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/Calculations/ExecCalc_Damage.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeExecCalc_Damage() {}

// ********** Begin Cross Module References ********************************************************
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffectExecutionCalculation();
SPELLRISE_API UClass* Z_Construct_UClass_UExecCalc_Damage();
SPELLRISE_API UClass* Z_Construct_UClass_UExecCalc_Damage_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UExecCalc_Damage *********************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UExecCalc_Damage;
UClass* UExecCalc_Damage::GetPrivateStaticClass()
{
	using TClass = UExecCalc_Damage;
	if (!Z_Registration_Info_UClass_UExecCalc_Damage.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("ExecCalc_Damage"),
			Z_Registration_Info_UClass_UExecCalc_Damage.InnerSingleton,
			StaticRegisterNativesUExecCalc_Damage,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UExecCalc_Damage.InnerSingleton;
}
UClass* Z_Construct_UClass_UExecCalc_Damage_NoRegister()
{
	return UExecCalc_Damage::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UExecCalc_Damage_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * SpellRise damage execution (AAA):\n * - Power multipliers: DerivedStatsAttributeSet (WeaponDamageMultiplier / SpellPowerMultiplier)\n * - Crit/Penetration: DerivedStatsAttributeSet (CritChance/CritDamage/ArmorPen/ElementPen)\n * - Target defenses: BasicAttributeSet (Armor/PhysicalResist/MagicResist)\n * - Output: ResourceAttributeSet::Damage (meta)\n */" },
#endif
		{ "IncludePath", "GameplayAbilitySystem/Calculations/ExecCalc_Damage.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Calculations/ExecCalc_Damage.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "SpellRise damage execution (AAA):\n- Power multipliers: DerivedStatsAttributeSet (WeaponDamageMultiplier / SpellPowerMultiplier)\n- Crit/Penetration: DerivedStatsAttributeSet (CritChance/CritDamage/ArmorPen/ElementPen)\n- Target defenses: BasicAttributeSet (Armor/PhysicalResist/MagicResist)\n- Output: ResourceAttributeSet::Damage (meta)" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UExecCalc_Damage constinit property declarations *************************
// ********** End Class UExecCalc_Damage constinit property declarations ***************************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UExecCalc_Damage>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UExecCalc_Damage_Statics
UObject* (*const Z_Construct_UClass_UExecCalc_Damage_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UGameplayEffectExecutionCalculation,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UExecCalc_Damage_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UExecCalc_Damage_Statics::ClassParams = {
	&UExecCalc_Damage::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UExecCalc_Damage_Statics::Class_MetaDataParams), Z_Construct_UClass_UExecCalc_Damage_Statics::Class_MetaDataParams)
};
void UExecCalc_Damage::StaticRegisterNativesUExecCalc_Damage()
{
}
UClass* Z_Construct_UClass_UExecCalc_Damage()
{
	if (!Z_Registration_Info_UClass_UExecCalc_Damage.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UExecCalc_Damage.OuterSingleton, Z_Construct_UClass_UExecCalc_Damage_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UExecCalc_Damage.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UExecCalc_Damage);
UExecCalc_Damage::~UExecCalc_Damage() {}
// ********** End Class UExecCalc_Damage ***********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_ExecCalc_Damage_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UExecCalc_Damage, UExecCalc_Damage::StaticClass, TEXT("UExecCalc_Damage"), &Z_Registration_Info_UClass_UExecCalc_Damage, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UExecCalc_Damage), 2753703652U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_ExecCalc_Damage_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_ExecCalc_Damage_h__Script_SpellRise_3951607142{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_ExecCalc_Damage_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_ExecCalc_Damage_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
