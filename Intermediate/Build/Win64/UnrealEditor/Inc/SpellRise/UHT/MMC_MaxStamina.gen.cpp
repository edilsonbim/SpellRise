// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/Calculations/MMC_MaxStamina.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeMMC_MaxStamina() {}

// ********** Begin Cross Module References ********************************************************
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayModMagnitudeCalculation();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_MaxStamina();
SPELLRISE_API UClass* Z_Construct_UClass_UMMC_MaxStamina_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UMMC_MaxStamina **********************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMMC_MaxStamina;
UClass* UMMC_MaxStamina::GetPrivateStaticClass()
{
	using TClass = UMMC_MaxStamina;
	if (!Z_Registration_Info_UClass_UMMC_MaxStamina.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MMC_MaxStamina"),
			Z_Registration_Info_UClass_UMMC_MaxStamina.InnerSingleton,
			StaticRegisterNativesUMMC_MaxStamina,
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
	return Z_Registration_Info_UClass_UMMC_MaxStamina.InnerSingleton;
}
UClass* Z_Construct_UClass_UMMC_MaxStamina_NoRegister()
{
	return UMMC_MaxStamina::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMMC_MaxStamina_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayAbilitySystem/Calculations/MMC_MaxStamina.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Calculations/MMC_MaxStamina.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UMMC_MaxStamina constinit property declarations **************************
// ********** End Class UMMC_MaxStamina constinit property declarations ****************************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMMC_MaxStamina>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMMC_MaxStamina_Statics
UObject* (*const Z_Construct_UClass_UMMC_MaxStamina_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UGameplayModMagnitudeCalculation,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_MaxStamina_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMMC_MaxStamina_Statics::ClassParams = {
	&UMMC_MaxStamina::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMMC_MaxStamina_Statics::Class_MetaDataParams), Z_Construct_UClass_UMMC_MaxStamina_Statics::Class_MetaDataParams)
};
void UMMC_MaxStamina::StaticRegisterNativesUMMC_MaxStamina()
{
}
UClass* Z_Construct_UClass_UMMC_MaxStamina()
{
	if (!Z_Registration_Info_UClass_UMMC_MaxStamina.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMMC_MaxStamina.OuterSingleton, Z_Construct_UClass_UMMC_MaxStamina_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMMC_MaxStamina.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMMC_MaxStamina);
UMMC_MaxStamina::~UMMC_MaxStamina() {}
// ********** End Class UMMC_MaxStamina ************************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_MaxStamina_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UMMC_MaxStamina, UMMC_MaxStamina::StaticClass, TEXT("UMMC_MaxStamina"), &Z_Registration_Info_UClass_UMMC_MaxStamina, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMMC_MaxStamina), 2226824449U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_MaxStamina_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_MaxStamina_h__Script_SpellRise_4191980165{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_MaxStamina_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Calculations_MMC_MaxStamina_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
