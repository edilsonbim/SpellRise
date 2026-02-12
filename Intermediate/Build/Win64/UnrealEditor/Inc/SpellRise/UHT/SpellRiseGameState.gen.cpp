// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Core/SpellRiseGameState.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSpellRiseGameState() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_AGameStateBase();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRiseGameState();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRiseGameState_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class ASpellRiseGameState ******************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_ASpellRiseGameState;
UClass* ASpellRiseGameState::GetPrivateStaticClass()
{
	using TClass = ASpellRiseGameState;
	if (!Z_Registration_Info_UClass_ASpellRiseGameState.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRiseGameState"),
			Z_Registration_Info_UClass_ASpellRiseGameState.InnerSingleton,
			StaticRegisterNativesASpellRiseGameState,
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
	return Z_Registration_Info_UClass_ASpellRiseGameState.InnerSingleton;
}
UClass* Z_Construct_UClass_ASpellRiseGameState_NoRegister()
{
	return ASpellRiseGameState::GetPrivateStaticClass();
}
struct Z_Construct_UClass_ASpellRiseGameState_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "Core/SpellRiseGameState.h" },
		{ "ModuleRelativePath", "Core/SpellRiseGameState.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif // WITH_METADATA

// ********** Begin Class ASpellRiseGameState constinit property declarations **********************
// ********** End Class ASpellRiseGameState constinit property declarations ************************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASpellRiseGameState>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_ASpellRiseGameState_Statics
UObject* (*const Z_Construct_UClass_ASpellRiseGameState_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AGameStateBase,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseGameState_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ASpellRiseGameState_Statics::ClassParams = {
	&ASpellRiseGameState::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x009002A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseGameState_Statics::Class_MetaDataParams), Z_Construct_UClass_ASpellRiseGameState_Statics::Class_MetaDataParams)
};
void ASpellRiseGameState::StaticRegisterNativesASpellRiseGameState()
{
}
UClass* Z_Construct_UClass_ASpellRiseGameState()
{
	if (!Z_Registration_Info_UClass_ASpellRiseGameState.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ASpellRiseGameState.OuterSingleton, Z_Construct_UClass_ASpellRiseGameState_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ASpellRiseGameState.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, ASpellRiseGameState);
ASpellRiseGameState::~ASpellRiseGameState() {}
// ********** End Class ASpellRiseGameState ********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameState_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ASpellRiseGameState, ASpellRiseGameState::StaticClass, TEXT("ASpellRiseGameState"), &Z_Registration_Info_UClass_ASpellRiseGameState, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ASpellRiseGameState), 752028707U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameState_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameState_h__Script_SpellRise_3805176877{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameState_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameState_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
