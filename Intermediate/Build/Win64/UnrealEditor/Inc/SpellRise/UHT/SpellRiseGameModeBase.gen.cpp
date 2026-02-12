// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Core/SpellRiseGameModeBase.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSpellRiseGameModeBase() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRiseGameModeBase();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRiseGameModeBase_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class ASpellRiseGameModeBase ***************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_ASpellRiseGameModeBase;
UClass* ASpellRiseGameModeBase::GetPrivateStaticClass()
{
	using TClass = ASpellRiseGameModeBase;
	if (!Z_Registration_Info_UClass_ASpellRiseGameModeBase.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRiseGameModeBase"),
			Z_Registration_Info_UClass_ASpellRiseGameModeBase.InnerSingleton,
			StaticRegisterNativesASpellRiseGameModeBase,
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
	return Z_Registration_Info_UClass_ASpellRiseGameModeBase.InnerSingleton;
}
UClass* Z_Construct_UClass_ASpellRiseGameModeBase_NoRegister()
{
	return ASpellRiseGameModeBase::GetPrivateStaticClass();
}
struct Z_Construct_UClass_ASpellRiseGameModeBase_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  Simple GameMode for a third person game\n */" },
#endif
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "Core/SpellRiseGameModeBase.h" },
		{ "ModuleRelativePath", "Core/SpellRiseGameModeBase.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Simple GameMode for a third person game" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class ASpellRiseGameModeBase constinit property declarations *******************
// ********** End Class ASpellRiseGameModeBase constinit property declarations *********************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASpellRiseGameModeBase>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_ASpellRiseGameModeBase_Statics
UObject* (*const Z_Construct_UClass_ASpellRiseGameModeBase_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AGameModeBase,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseGameModeBase_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ASpellRiseGameModeBase_Statics::ClassParams = {
	&ASpellRiseGameModeBase::StaticClass,
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
	0x008002ADu,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseGameModeBase_Statics::Class_MetaDataParams), Z_Construct_UClass_ASpellRiseGameModeBase_Statics::Class_MetaDataParams)
};
void ASpellRiseGameModeBase::StaticRegisterNativesASpellRiseGameModeBase()
{
}
UClass* Z_Construct_UClass_ASpellRiseGameModeBase()
{
	if (!Z_Registration_Info_UClass_ASpellRiseGameModeBase.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ASpellRiseGameModeBase.OuterSingleton, Z_Construct_UClass_ASpellRiseGameModeBase_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ASpellRiseGameModeBase.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, ASpellRiseGameModeBase);
ASpellRiseGameModeBase::~ASpellRiseGameModeBase() {}
// ********** End Class ASpellRiseGameModeBase *****************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameModeBase_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ASpellRiseGameModeBase, ASpellRiseGameModeBase::StaticClass, TEXT("ASpellRiseGameModeBase"), &Z_Registration_Info_UClass_ASpellRiseGameModeBase, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ASpellRiseGameModeBase), 145537384U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameModeBase_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameModeBase_h__Script_SpellRise_1367546721{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameModeBase_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRiseGameModeBase_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
