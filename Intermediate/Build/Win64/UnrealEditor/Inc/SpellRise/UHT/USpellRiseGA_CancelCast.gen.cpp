// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/USpellRiseGA_CancelCast.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeUSpellRiseGA_CancelCast() {}

// ********** Begin Cross Module References ********************************************************
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGA_CancelCast();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGA_CancelCast_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGameplayAbility();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class USpellRiseGA_CancelCast **************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_USpellRiseGA_CancelCast;
UClass* USpellRiseGA_CancelCast::GetPrivateStaticClass()
{
	using TClass = USpellRiseGA_CancelCast;
	if (!Z_Registration_Info_UClass_USpellRiseGA_CancelCast.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRiseGA_CancelCast"),
			Z_Registration_Info_UClass_USpellRiseGA_CancelCast.InnerSingleton,
			StaticRegisterNativesUSpellRiseGA_CancelCast,
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
	return Z_Registration_Info_UClass_USpellRiseGA_CancelCast.InnerSingleton;
}
UClass* Z_Construct_UClass_USpellRiseGA_CancelCast_NoRegister()
{
	return USpellRiseGA_CancelCast::GetPrivateStaticClass();
}
struct Z_Construct_UClass_USpellRiseGA_CancelCast_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "GameplayAbilitySystem/Abilities/USpellRiseGA_CancelCast.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/USpellRiseGA_CancelCast.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class USpellRiseGA_CancelCast constinit property declarations ******************
// ********** End Class USpellRiseGA_CancelCast constinit property declarations ********************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USpellRiseGA_CancelCast>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_USpellRiseGA_CancelCast_Statics
UObject* (*const Z_Construct_UClass_USpellRiseGA_CancelCast_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USpellRiseGameplayAbility,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGA_CancelCast_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USpellRiseGA_CancelCast_Statics::ClassParams = {
	&USpellRiseGA_CancelCast::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGA_CancelCast_Statics::Class_MetaDataParams), Z_Construct_UClass_USpellRiseGA_CancelCast_Statics::Class_MetaDataParams)
};
void USpellRiseGA_CancelCast::StaticRegisterNativesUSpellRiseGA_CancelCast()
{
}
UClass* Z_Construct_UClass_USpellRiseGA_CancelCast()
{
	if (!Z_Registration_Info_UClass_USpellRiseGA_CancelCast.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USpellRiseGA_CancelCast.OuterSingleton, Z_Construct_UClass_USpellRiseGA_CancelCast_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USpellRiseGA_CancelCast.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, USpellRiseGA_CancelCast);
USpellRiseGA_CancelCast::~USpellRiseGA_CancelCast() {}
// ********** End Class USpellRiseGA_CancelCast ****************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_USpellRiseGA_CancelCast_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USpellRiseGA_CancelCast, USpellRiseGA_CancelCast::StaticClass, TEXT("USpellRiseGA_CancelCast"), &Z_Registration_Info_UClass_USpellRiseGA_CancelCast, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USpellRiseGA_CancelCast), 2992051355U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_USpellRiseGA_CancelCast_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_USpellRiseGA_CancelCast_h__Script_SpellRise_3905072699{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_USpellRiseGA_CancelCast_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_USpellRiseGA_CancelCast_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
