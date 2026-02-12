// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/Calculations/MMC_DerivedStats.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeMMC_DerivedStats() {}

// ********** Begin Cross Module References ********************************************************
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayModMagnitudeCalculation();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_AttackSpeedMultiplier();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_AttackSpeedMultiplier_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_CastSpeedMultiplier();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_CastSpeedMultiplier_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_CritChance();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_CritChance_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_CritDamage();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_CritDamage_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_ManaCostMultiplier();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_ManaCostMultiplier_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_SpellPowerMultiplier();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_SpellPowerMultiplier_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_WeaponDamageMultiplier();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_WeaponDamageMultiplier_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseDerivedStatMMC();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseDerivedStatMMC_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class USpellRiseDerivedStatMMC *************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_USpellRiseDerivedStatMMC;
UClass* USpellRiseDerivedStatMMC::GetPrivateStaticClass()
{
	using TClass = USpellRiseDerivedStatMMC;
	if (!Z_Registration_Info_UClass_USpellRiseDerivedStatMMC.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRiseDerivedStatMMC"),
			Z_Registration_Info_UClass_USpellRiseDerivedStatMMC.InnerSingleton,
			StaticRegisterNativesUSpellRiseDerivedStatMMC,
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
	return Z_Registration_Info_UClass_USpellRiseDerivedStatMMC.InnerSingleton;
}
UClass* Z_Construct_UClass_USpellRiseDerivedStatMMC_NoRegister()
{
	return USpellRiseDerivedStatMMC::GetPrivateStaticClass();
}
struct Z_Construct_UClass_USpellRiseDerivedStatMMC_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// --------------------------------------------------------------------\n// Base class with common capture logic\n// --------------------------------------------------------------------\n" },
#endif
		{ "IncludePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Base class with common capture logic" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class USpellRiseDerivedStatMMC constinit property declarations *****************
// ********** End Class USpellRiseDerivedStatMMC constinit property declarations *******************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USpellRiseDerivedStatMMC>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_USpellRiseDerivedStatMMC_Statics
UObject* (*const Z_Construct_UClass_USpellRiseDerivedStatMMC_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UGameplayModMagnitudeCalculation,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseDerivedStatMMC_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USpellRiseDerivedStatMMC_Statics::ClassParams = {
	&USpellRiseDerivedStatMMC::StaticClass,
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
	0x001000A1u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseDerivedStatMMC_Statics::Class_MetaDataParams), Z_Construct_UClass_USpellRiseDerivedStatMMC_Statics::Class_MetaDataParams)
};
void USpellRiseDerivedStatMMC::StaticRegisterNativesUSpellRiseDerivedStatMMC()
{
}
UClass* Z_Construct_UClass_USpellRiseDerivedStatMMC()
{
	if (!Z_Registration_Info_UClass_USpellRiseDerivedStatMMC.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USpellRiseDerivedStatMMC.OuterSingleton, Z_Construct_UClass_USpellRiseDerivedStatMMC_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USpellRiseDerivedStatMMC.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, USpellRiseDerivedStatMMC);
USpellRiseDerivedStatMMC::~USpellRiseDerivedStatMMC() {}
// ********** End Class USpellRiseDerivedStatMMC ***************************************************

// ********** Begin Class UMMC_WeaponDamageMultiplier **********************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMMC_WeaponDamageMultiplier;
UClass* UMMC_WeaponDamageMultiplier::GetPrivateStaticClass()
{
	using TClass = UMMC_WeaponDamageMultiplier;
	if (!Z_Registration_Info_UClass_UMMC_WeaponDamageMultiplier.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MMC_WeaponDamageMultiplier"),
			Z_Registration_Info_UClass_UMMC_WeaponDamageMultiplier.InnerSingleton,
			StaticRegisterNativesUMMC_WeaponDamageMultiplier,
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
	return Z_Registration_Info_UClass_UMMC_WeaponDamageMultiplier.InnerSingleton;
}
UClass* Z_Construct_UClass_UMMC_WeaponDamageMultiplier_NoRegister()
{
	return UMMC_WeaponDamageMultiplier::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMMC_WeaponDamageMultiplier_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// --------------------------------------------------------------------\n// Individual MMCs - each handles ONE derived stat\n// --------------------------------------------------------------------\n" },
#endif
		{ "IncludePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Individual MMCs - each handles ONE derived stat" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UMMC_WeaponDamageMultiplier constinit property declarations **************
// ********** End Class UMMC_WeaponDamageMultiplier constinit property declarations ****************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMMC_WeaponDamageMultiplier>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMMC_WeaponDamageMultiplier_Statics
UObject* (*const Z_Construct_UClass_UMMC_WeaponDamageMultiplier_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USpellRiseDerivedStatMMC,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_WeaponDamageMultiplier_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMMC_WeaponDamageMultiplier_Statics::ClassParams = {
	&UMMC_WeaponDamageMultiplier::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_WeaponDamageMultiplier_Statics::Class_MetaDataParams), Z_Construct_UClass_UMMC_WeaponDamageMultiplier_Statics::Class_MetaDataParams)
};
void UMMC_WeaponDamageMultiplier::StaticRegisterNativesUMMC_WeaponDamageMultiplier()
{
}
UClass* Z_Construct_UClass_UMMC_WeaponDamageMultiplier()
{
	if (!Z_Registration_Info_UClass_UMMC_WeaponDamageMultiplier.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMMC_WeaponDamageMultiplier.OuterSingleton, Z_Construct_UClass_UMMC_WeaponDamageMultiplier_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMMC_WeaponDamageMultiplier.OuterSingleton;
}
UMMC_WeaponDamageMultiplier::UMMC_WeaponDamageMultiplier() {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMMC_WeaponDamageMultiplier);
UMMC_WeaponDamageMultiplier::~UMMC_WeaponDamageMultiplier() {}
// ********** End Class UMMC_WeaponDamageMultiplier ************************************************

// ********** Begin Class UMMC_AttackSpeedMultiplier ***********************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMMC_AttackSpeedMultiplier;
UClass* UMMC_AttackSpeedMultiplier::GetPrivateStaticClass()
{
	using TClass = UMMC_AttackSpeedMultiplier;
	if (!Z_Registration_Info_UClass_UMMC_AttackSpeedMultiplier.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MMC_AttackSpeedMultiplier"),
			Z_Registration_Info_UClass_UMMC_AttackSpeedMultiplier.InnerSingleton,
			StaticRegisterNativesUMMC_AttackSpeedMultiplier,
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
	return Z_Registration_Info_UClass_UMMC_AttackSpeedMultiplier.InnerSingleton;
}
UClass* Z_Construct_UClass_UMMC_AttackSpeedMultiplier_NoRegister()
{
	return UMMC_AttackSpeedMultiplier::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMMC_AttackSpeedMultiplier_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UMMC_AttackSpeedMultiplier constinit property declarations ***************
// ********** End Class UMMC_AttackSpeedMultiplier constinit property declarations *****************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMMC_AttackSpeedMultiplier>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMMC_AttackSpeedMultiplier_Statics
UObject* (*const Z_Construct_UClass_UMMC_AttackSpeedMultiplier_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USpellRiseDerivedStatMMC,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_AttackSpeedMultiplier_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMMC_AttackSpeedMultiplier_Statics::ClassParams = {
	&UMMC_AttackSpeedMultiplier::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_AttackSpeedMultiplier_Statics::Class_MetaDataParams), Z_Construct_UClass_UMMC_AttackSpeedMultiplier_Statics::Class_MetaDataParams)
};
void UMMC_AttackSpeedMultiplier::StaticRegisterNativesUMMC_AttackSpeedMultiplier()
{
}
UClass* Z_Construct_UClass_UMMC_AttackSpeedMultiplier()
{
	if (!Z_Registration_Info_UClass_UMMC_AttackSpeedMultiplier.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMMC_AttackSpeedMultiplier.OuterSingleton, Z_Construct_UClass_UMMC_AttackSpeedMultiplier_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMMC_AttackSpeedMultiplier.OuterSingleton;
}
UMMC_AttackSpeedMultiplier::UMMC_AttackSpeedMultiplier() {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMMC_AttackSpeedMultiplier);
UMMC_AttackSpeedMultiplier::~UMMC_AttackSpeedMultiplier() {}
// ********** End Class UMMC_AttackSpeedMultiplier *************************************************

// ********** Begin Class UMMC_CritChance **********************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMMC_CritChance;
UClass* UMMC_CritChance::GetPrivateStaticClass()
{
	using TClass = UMMC_CritChance;
	if (!Z_Registration_Info_UClass_UMMC_CritChance.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MMC_CritChance"),
			Z_Registration_Info_UClass_UMMC_CritChance.InnerSingleton,
			StaticRegisterNativesUMMC_CritChance,
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
	return Z_Registration_Info_UClass_UMMC_CritChance.InnerSingleton;
}
UClass* Z_Construct_UClass_UMMC_CritChance_NoRegister()
{
	return UMMC_CritChance::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMMC_CritChance_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UMMC_CritChance constinit property declarations **************************
// ********** End Class UMMC_CritChance constinit property declarations ****************************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMMC_CritChance>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMMC_CritChance_Statics
UObject* (*const Z_Construct_UClass_UMMC_CritChance_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USpellRiseDerivedStatMMC,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_CritChance_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMMC_CritChance_Statics::ClassParams = {
	&UMMC_CritChance::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_CritChance_Statics::Class_MetaDataParams), Z_Construct_UClass_UMMC_CritChance_Statics::Class_MetaDataParams)
};
void UMMC_CritChance::StaticRegisterNativesUMMC_CritChance()
{
}
UClass* Z_Construct_UClass_UMMC_CritChance()
{
	if (!Z_Registration_Info_UClass_UMMC_CritChance.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMMC_CritChance.OuterSingleton, Z_Construct_UClass_UMMC_CritChance_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMMC_CritChance.OuterSingleton;
}
UMMC_CritChance::UMMC_CritChance() {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMMC_CritChance);
UMMC_CritChance::~UMMC_CritChance() {}
// ********** End Class UMMC_CritChance ************************************************************

// ********** Begin Class UMMC_CritDamage **********************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMMC_CritDamage;
UClass* UMMC_CritDamage::GetPrivateStaticClass()
{
	using TClass = UMMC_CritDamage;
	if (!Z_Registration_Info_UClass_UMMC_CritDamage.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MMC_CritDamage"),
			Z_Registration_Info_UClass_UMMC_CritDamage.InnerSingleton,
			StaticRegisterNativesUMMC_CritDamage,
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
	return Z_Registration_Info_UClass_UMMC_CritDamage.InnerSingleton;
}
UClass* Z_Construct_UClass_UMMC_CritDamage_NoRegister()
{
	return UMMC_CritDamage::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMMC_CritDamage_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UMMC_CritDamage constinit property declarations **************************
// ********** End Class UMMC_CritDamage constinit property declarations ****************************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMMC_CritDamage>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMMC_CritDamage_Statics
UObject* (*const Z_Construct_UClass_UMMC_CritDamage_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USpellRiseDerivedStatMMC,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_CritDamage_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMMC_CritDamage_Statics::ClassParams = {
	&UMMC_CritDamage::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_CritDamage_Statics::Class_MetaDataParams), Z_Construct_UClass_UMMC_CritDamage_Statics::Class_MetaDataParams)
};
void UMMC_CritDamage::StaticRegisterNativesUMMC_CritDamage()
{
}
UClass* Z_Construct_UClass_UMMC_CritDamage()
{
	if (!Z_Registration_Info_UClass_UMMC_CritDamage.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMMC_CritDamage.OuterSingleton, Z_Construct_UClass_UMMC_CritDamage_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMMC_CritDamage.OuterSingleton;
}
UMMC_CritDamage::UMMC_CritDamage() {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMMC_CritDamage);
UMMC_CritDamage::~UMMC_CritDamage() {}
// ********** End Class UMMC_CritDamage ************************************************************

// ********** Begin Class UMMC_SpellPowerMultiplier ************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMMC_SpellPowerMultiplier;
UClass* UMMC_SpellPowerMultiplier::GetPrivateStaticClass()
{
	using TClass = UMMC_SpellPowerMultiplier;
	if (!Z_Registration_Info_UClass_UMMC_SpellPowerMultiplier.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MMC_SpellPowerMultiplier"),
			Z_Registration_Info_UClass_UMMC_SpellPowerMultiplier.InnerSingleton,
			StaticRegisterNativesUMMC_SpellPowerMultiplier,
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
	return Z_Registration_Info_UClass_UMMC_SpellPowerMultiplier.InnerSingleton;
}
UClass* Z_Construct_UClass_UMMC_SpellPowerMultiplier_NoRegister()
{
	return UMMC_SpellPowerMultiplier::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMMC_SpellPowerMultiplier_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UMMC_SpellPowerMultiplier constinit property declarations ****************
// ********** End Class UMMC_SpellPowerMultiplier constinit property declarations ******************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMMC_SpellPowerMultiplier>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMMC_SpellPowerMultiplier_Statics
UObject* (*const Z_Construct_UClass_UMMC_SpellPowerMultiplier_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USpellRiseDerivedStatMMC,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_SpellPowerMultiplier_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMMC_SpellPowerMultiplier_Statics::ClassParams = {
	&UMMC_SpellPowerMultiplier::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_SpellPowerMultiplier_Statics::Class_MetaDataParams), Z_Construct_UClass_UMMC_SpellPowerMultiplier_Statics::Class_MetaDataParams)
};
void UMMC_SpellPowerMultiplier::StaticRegisterNativesUMMC_SpellPowerMultiplier()
{
}
UClass* Z_Construct_UClass_UMMC_SpellPowerMultiplier()
{
	if (!Z_Registration_Info_UClass_UMMC_SpellPowerMultiplier.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMMC_SpellPowerMultiplier.OuterSingleton, Z_Construct_UClass_UMMC_SpellPowerMultiplier_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMMC_SpellPowerMultiplier.OuterSingleton;
}
UMMC_SpellPowerMultiplier::UMMC_SpellPowerMultiplier() {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMMC_SpellPowerMultiplier);
UMMC_SpellPowerMultiplier::~UMMC_SpellPowerMultiplier() {}
// ********** End Class UMMC_SpellPowerMultiplier **************************************************

// ********** Begin Class UMMC_CastSpeedMultiplier *************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMMC_CastSpeedMultiplier;
UClass* UMMC_CastSpeedMultiplier::GetPrivateStaticClass()
{
	using TClass = UMMC_CastSpeedMultiplier;
	if (!Z_Registration_Info_UClass_UMMC_CastSpeedMultiplier.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MMC_CastSpeedMultiplier"),
			Z_Registration_Info_UClass_UMMC_CastSpeedMultiplier.InnerSingleton,
			StaticRegisterNativesUMMC_CastSpeedMultiplier,
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
	return Z_Registration_Info_UClass_UMMC_CastSpeedMultiplier.InnerSingleton;
}
UClass* Z_Construct_UClass_UMMC_CastSpeedMultiplier_NoRegister()
{
	return UMMC_CastSpeedMultiplier::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMMC_CastSpeedMultiplier_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UMMC_CastSpeedMultiplier constinit property declarations *****************
// ********** End Class UMMC_CastSpeedMultiplier constinit property declarations *******************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMMC_CastSpeedMultiplier>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMMC_CastSpeedMultiplier_Statics
UObject* (*const Z_Construct_UClass_UMMC_CastSpeedMultiplier_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USpellRiseDerivedStatMMC,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_CastSpeedMultiplier_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMMC_CastSpeedMultiplier_Statics::ClassParams = {
	&UMMC_CastSpeedMultiplier::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_CastSpeedMultiplier_Statics::Class_MetaDataParams), Z_Construct_UClass_UMMC_CastSpeedMultiplier_Statics::Class_MetaDataParams)
};
void UMMC_CastSpeedMultiplier::StaticRegisterNativesUMMC_CastSpeedMultiplier()
{
}
UClass* Z_Construct_UClass_UMMC_CastSpeedMultiplier()
{
	if (!Z_Registration_Info_UClass_UMMC_CastSpeedMultiplier.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMMC_CastSpeedMultiplier.OuterSingleton, Z_Construct_UClass_UMMC_CastSpeedMultiplier_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMMC_CastSpeedMultiplier.OuterSingleton;
}
UMMC_CastSpeedMultiplier::UMMC_CastSpeedMultiplier() {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMMC_CastSpeedMultiplier);
UMMC_CastSpeedMultiplier::~UMMC_CastSpeedMultiplier() {}
// ********** End Class UMMC_CastSpeedMultiplier ***************************************************

// ********** Begin Class UMMC_ManaCostMultiplier **************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMMC_ManaCostMultiplier;
UClass* UMMC_ManaCostMultiplier::GetPrivateStaticClass()
{
	using TClass = UMMC_ManaCostMultiplier;
	if (!Z_Registration_Info_UClass_UMMC_ManaCostMultiplier.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MMC_ManaCostMultiplier"),
			Z_Registration_Info_UClass_UMMC_ManaCostMultiplier.InnerSingleton,
			StaticRegisterNativesUMMC_ManaCostMultiplier,
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
	return Z_Registration_Info_UClass_UMMC_ManaCostMultiplier.InnerSingleton;
}
UClass* Z_Construct_UClass_UMMC_ManaCostMultiplier_NoRegister()
{
	return UMMC_ManaCostMultiplier::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMMC_ManaCostMultiplier_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Calculations/MMC_DerivedStats.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UMMC_ManaCostMultiplier constinit property declarations ******************
// ********** End Class UMMC_ManaCostMultiplier constinit property declarations ********************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMMC_ManaCostMultiplier>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMMC_ManaCostMultiplier_Statics
UObject* (*const Z_Construct_UClass_UMMC_ManaCostMultiplier_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USpellRiseDerivedStatMMC,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_ManaCostMultiplier_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMMC_ManaCostMultiplier_Statics::ClassParams = {
	&UMMC_ManaCostMultiplier::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_ManaCostMultiplier_Statics::Class_MetaDataParams), Z_Construct_UClass_UMMC_ManaCostMultiplier_Statics::Class_MetaDataParams)
};
void UMMC_ManaCostMultiplier::StaticRegisterNativesUMMC_ManaCostMultiplier()
{
}
UClass* Z_Construct_UClass_UMMC_ManaCostMultiplier()
{
	if (!Z_Registration_Info_UClass_UMMC_ManaCostMultiplier.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMMC_ManaCostMultiplier.OuterSingleton, Z_Construct_UClass_UMMC_ManaCostMultiplier_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMMC_ManaCostMultiplier.OuterSingleton;
}
UMMC_ManaCostMultiplier::UMMC_ManaCostMultiplier() {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMMC_ManaCostMultiplier);
UMMC_ManaCostMultiplier::~UMMC_ManaCostMultiplier() {}
// ********** End Class UMMC_ManaCostMultiplier ****************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_DerivedStats_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USpellRiseDerivedStatMMC, USpellRiseDerivedStatMMC::StaticClass, TEXT("USpellRiseDerivedStatMMC"), &Z_Registration_Info_UClass_USpellRiseDerivedStatMMC, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USpellRiseDerivedStatMMC), 2353506512U) },
		{ Z_Construct_UClass_UMMC_WeaponDamageMultiplier, UMMC_WeaponDamageMultiplier::StaticClass, TEXT("UMMC_WeaponDamageMultiplier"), &Z_Registration_Info_UClass_UMMC_WeaponDamageMultiplier, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMMC_WeaponDamageMultiplier), 3900491863U) },
		{ Z_Construct_UClass_UMMC_AttackSpeedMultiplier, UMMC_AttackSpeedMultiplier::StaticClass, TEXT("UMMC_AttackSpeedMultiplier"), &Z_Registration_Info_UClass_UMMC_AttackSpeedMultiplier, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMMC_AttackSpeedMultiplier), 3738472647U) },
		{ Z_Construct_UClass_UMMC_CritChance, UMMC_CritChance::StaticClass, TEXT("UMMC_CritChance"), &Z_Registration_Info_UClass_UMMC_CritChance, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMMC_CritChance), 989332493U) },
		{ Z_Construct_UClass_UMMC_CritDamage, UMMC_CritDamage::StaticClass, TEXT("UMMC_CritDamage"), &Z_Registration_Info_UClass_UMMC_CritDamage, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMMC_CritDamage), 3679004190U) },
		{ Z_Construct_UClass_UMMC_SpellPowerMultiplier, UMMC_SpellPowerMultiplier::StaticClass, TEXT("UMMC_SpellPowerMultiplier"), &Z_Registration_Info_UClass_UMMC_SpellPowerMultiplier, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMMC_SpellPowerMultiplier), 820243454U) },
		{ Z_Construct_UClass_UMMC_CastSpeedMultiplier, UMMC_CastSpeedMultiplier::StaticClass, TEXT("UMMC_CastSpeedMultiplier"), &Z_Registration_Info_UClass_UMMC_CastSpeedMultiplier, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMMC_CastSpeedMultiplier), 635816253U) },
		{ Z_Construct_UClass_UMMC_ManaCostMultiplier, UMMC_ManaCostMultiplier::StaticClass, TEXT("UMMC_ManaCostMultiplier"), &Z_Registration_Info_UClass_UMMC_ManaCostMultiplier, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMMC_ManaCostMultiplier), 3068160370U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_DerivedStats_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_DerivedStats_h__Script_SpellRise_3196277282{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_DerivedStats_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_DerivedStats_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
