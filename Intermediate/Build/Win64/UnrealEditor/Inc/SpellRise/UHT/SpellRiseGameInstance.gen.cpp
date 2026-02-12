// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Core/SpellRiseGameInstance.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSpellRiseGameInstance() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_UGameInstance();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGameInstance();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGameInstance_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class USpellRiseGameInstance ***************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_USpellRiseGameInstance;
UClass* USpellRiseGameInstance::GetPrivateStaticClass()
{
	using TClass = USpellRiseGameInstance;
	if (!Z_Registration_Info_UClass_USpellRiseGameInstance.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRiseGameInstance"),
			Z_Registration_Info_UClass_USpellRiseGameInstance.InnerSingleton,
			StaticRegisterNativesUSpellRiseGameInstance,
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
	return Z_Registration_Info_UClass_USpellRiseGameInstance.InnerSingleton;
}
UClass* Z_Construct_UClass_USpellRiseGameInstance_NoRegister()
{
	return USpellRiseGameInstance::GetPrivateStaticClass();
}
struct Z_Construct_UClass_USpellRiseGameInstance_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "Core/SpellRiseGameInstance.h" },
		{ "ModuleRelativePath", "Core/SpellRiseGameInstance.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class USpellRiseGameInstance constinit property declarations *******************
// ********** End Class USpellRiseGameInstance constinit property declarations *********************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USpellRiseGameInstance>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_USpellRiseGameInstance_Statics
UObject* (*const Z_Construct_UClass_USpellRiseGameInstance_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UGameInstance,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGameInstance_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USpellRiseGameInstance_Statics::ClassParams = {
	&USpellRiseGameInstance::StaticClass,
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
	0x009000A8u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGameInstance_Statics::Class_MetaDataParams), Z_Construct_UClass_USpellRiseGameInstance_Statics::Class_MetaDataParams)
};
void USpellRiseGameInstance::StaticRegisterNativesUSpellRiseGameInstance()
{
}
UClass* Z_Construct_UClass_USpellRiseGameInstance()
{
	if (!Z_Registration_Info_UClass_USpellRiseGameInstance.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USpellRiseGameInstance.OuterSingleton, Z_Construct_UClass_USpellRiseGameInstance_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USpellRiseGameInstance.OuterSingleton;
}
USpellRiseGameInstance::USpellRiseGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, USpellRiseGameInstance);
USpellRiseGameInstance::~USpellRiseGameInstance() {}
// ********** End Class USpellRiseGameInstance *****************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameInstance_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USpellRiseGameInstance, USpellRiseGameInstance::StaticClass, TEXT("USpellRiseGameInstance"), &Z_Registration_Info_UClass_USpellRiseGameInstance, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USpellRiseGameInstance), 3854381902U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameInstance_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameInstance_h__Script_SpellRise_4285467153{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameInstance_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameInstance_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
