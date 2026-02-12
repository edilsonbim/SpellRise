// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/Calculations/MMC_CarryWeight.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeMMC_CarryWeight() {}

// ********** Begin Cross Module References ********************************************************
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayModMagnitudeCalculation();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_CarryWeight();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_CarryWeight_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UMMC_CarryWeight *********************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMMC_CarryWeight;
UClass* UMMC_CarryWeight::GetPrivateStaticClass()
{
	using TClass = UMMC_CarryWeight;
	if (!Z_Registration_Info_UClass_UMMC_CarryWeight.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MMC_CarryWeight"),
			Z_Registration_Info_UClass_UMMC_CarryWeight.InnerSingleton,
			StaticRegisterNativesUMMC_CarryWeight,
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
	return Z_Registration_Info_UClass_UMMC_CarryWeight.InnerSingleton;
}
UClass* Z_Construct_UClass_UMMC_CarryWeight_NoRegister()
{
	return UMMC_CarryWeight::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMMC_CarryWeight_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayAbilitySystem/Calculations/MMC_CarryWeight.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Calculations/MMC_CarryWeight.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UMMC_CarryWeight constinit property declarations *************************
// ********** End Class UMMC_CarryWeight constinit property declarations ***************************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMMC_CarryWeight>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMMC_CarryWeight_Statics
UObject* (*const Z_Construct_UClass_UMMC_CarryWeight_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UGameplayModMagnitudeCalculation,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_CarryWeight_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMMC_CarryWeight_Statics::ClassParams = {
	&UMMC_CarryWeight::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_CarryWeight_Statics::Class_MetaDataParams), Z_Construct_UClass_UMMC_CarryWeight_Statics::Class_MetaDataParams)
};
void UMMC_CarryWeight::StaticRegisterNativesUMMC_CarryWeight()
{
}
UClass* Z_Construct_UClass_UMMC_CarryWeight()
{
	if (!Z_Registration_Info_UClass_UMMC_CarryWeight.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMMC_CarryWeight.OuterSingleton, Z_Construct_UClass_UMMC_CarryWeight_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMMC_CarryWeight.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMMC_CarryWeight);
UMMC_CarryWeight::~UMMC_CarryWeight() {}
// ********** End Class UMMC_CarryWeight ***********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_CarryWeight_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UMMC_CarryWeight, UMMC_CarryWeight::StaticClass, TEXT("UMMC_CarryWeight"), &Z_Registration_Info_UClass_UMMC_CarryWeight, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMMC_CarryWeight), 1591378399U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_CarryWeight_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_CarryWeight_h__Script_SpellRise_905266517{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_CarryWeight_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_CarryWeight_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
