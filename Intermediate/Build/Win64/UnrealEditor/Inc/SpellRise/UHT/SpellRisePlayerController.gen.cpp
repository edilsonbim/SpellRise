// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Core/SpellRisePlayerController.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSpellRisePlayerController() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_APlayerController();
ENHANCEDINPUT_API UClass* Z_Construct_UClass_UInputMappingContext_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRisePlayerController();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRisePlayerController_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class ASpellRisePlayerController ***********************************************
FClassRegistrationInfo Z_Registration_Info_UClass_ASpellRisePlayerController;
UClass* ASpellRisePlayerController::GetPrivateStaticClass()
{
	using TClass = ASpellRisePlayerController;
	if (!Z_Registration_Info_UClass_ASpellRisePlayerController.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRisePlayerController"),
			Z_Registration_Info_UClass_ASpellRisePlayerController.InnerSingleton,
			StaticRegisterNativesASpellRisePlayerController,
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
	return Z_Registration_Info_UClass_ASpellRisePlayerController.InnerSingleton;
}
UClass* Z_Construct_UClass_ASpellRisePlayerController_NoRegister()
{
	return ASpellRisePlayerController::GetPrivateStaticClass();
}
struct Z_Construct_UClass_ASpellRisePlayerController_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Collision Rendering Transformation" },
		{ "IncludePath", "Core/SpellRisePlayerController.h" },
		{ "ModuleRelativePath", "Core/SpellRisePlayerController.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultMappingContext_MetaData[] = {
		{ "Category", "Input|Enhanced" },
		{ "ModuleRelativePath", "Core/SpellRisePlayerController.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultMappingPriority_MetaData[] = {
		{ "Category", "Input|Enhanced" },
		{ "ClampMin", "0" },
		{ "ModuleRelativePath", "Core/SpellRisePlayerController.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class ASpellRisePlayerController constinit property declarations ***************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_DefaultMappingContext;
	static const UECodeGen_Private::FIntPropertyParams NewProp_DefaultMappingPriority;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class ASpellRisePlayerController constinit property declarations *****************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASpellRisePlayerController>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_ASpellRisePlayerController_Statics

// ********** Begin Class ASpellRisePlayerController Property Definitions **************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRisePlayerController_Statics::NewProp_DefaultMappingContext = { "DefaultMappingContext", nullptr, (EPropertyFlags)0x0124080000010001, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRisePlayerController, DefaultMappingContext), Z_Construct_UClass_UInputMappingContext_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultMappingContext_MetaData), NewProp_DefaultMappingContext_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_ASpellRisePlayerController_Statics::NewProp_DefaultMappingPriority = { "DefaultMappingPriority", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRisePlayerController, DefaultMappingPriority), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultMappingPriority_MetaData), NewProp_DefaultMappingPriority_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ASpellRisePlayerController_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRisePlayerController_Statics::NewProp_DefaultMappingContext,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRisePlayerController_Statics::NewProp_DefaultMappingPriority,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRisePlayerController_Statics::PropPointers) < 2048);
// ********** End Class ASpellRisePlayerController Property Definitions ****************************
UObject* (*const Z_Construct_UClass_ASpellRisePlayerController_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_APlayerController,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRisePlayerController_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ASpellRisePlayerController_Statics::ClassParams = {
	&ASpellRisePlayerController::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_ASpellRisePlayerController_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRisePlayerController_Statics::PropPointers),
	0,
	0x009002A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRisePlayerController_Statics::Class_MetaDataParams), Z_Construct_UClass_ASpellRisePlayerController_Statics::Class_MetaDataParams)
};
void ASpellRisePlayerController::StaticRegisterNativesASpellRisePlayerController()
{
}
UClass* Z_Construct_UClass_ASpellRisePlayerController()
{
	if (!Z_Registration_Info_UClass_ASpellRisePlayerController.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ASpellRisePlayerController.OuterSingleton, Z_Construct_UClass_ASpellRisePlayerController_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ASpellRisePlayerController.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, ASpellRisePlayerController);
ASpellRisePlayerController::~ASpellRisePlayerController() {}
// ********** End Class ASpellRisePlayerController *************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerController_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ASpellRisePlayerController, ASpellRisePlayerController::StaticClass, TEXT("ASpellRisePlayerController"), &Z_Registration_Info_UClass_ASpellRisePlayerController, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ASpellRisePlayerController), 260989009U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerController_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerController_h__Script_SpellRise_3302223550{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerController_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerController_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
