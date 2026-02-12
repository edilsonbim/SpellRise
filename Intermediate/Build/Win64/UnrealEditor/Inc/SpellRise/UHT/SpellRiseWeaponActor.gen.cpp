// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Weapons/Actors/SpellRiseWeaponActor.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSpellRiseWeaponActor() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UClass* Z_Construct_UClass_UStaticMeshComponent_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRiseWeaponActor();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRiseWeaponActor_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class ASpellRiseWeaponActor ****************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_ASpellRiseWeaponActor;
UClass* ASpellRiseWeaponActor::GetPrivateStaticClass()
{
	using TClass = ASpellRiseWeaponActor;
	if (!Z_Registration_Info_UClass_ASpellRiseWeaponActor.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRiseWeaponActor"),
			Z_Registration_Info_UClass_ASpellRiseWeaponActor.InnerSingleton,
			StaticRegisterNativesASpellRiseWeaponActor,
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
	return Z_Registration_Info_UClass_ASpellRiseWeaponActor.InnerSingleton;
}
UClass* Z_Construct_UClass_ASpellRiseWeaponActor_NoRegister()
{
	return ASpellRiseWeaponActor::GetPrivateStaticClass();
}
struct Z_Construct_UClass_ASpellRiseWeaponActor_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "Weapons/Actors/SpellRiseWeaponActor.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Weapons/Actors/SpellRiseWeaponActor.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Mesh_MetaData[] = {
		{ "Category", "Weapon" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Weapons/Actors/SpellRiseWeaponActor.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class ASpellRiseWeaponActor constinit property declarations ********************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Mesh;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class ASpellRiseWeaponActor constinit property declarations **********************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASpellRiseWeaponActor>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_ASpellRiseWeaponActor_Statics

// ********** Begin Class ASpellRiseWeaponActor Property Definitions *******************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRiseWeaponActor_Statics::NewProp_Mesh = { "Mesh", nullptr, (EPropertyFlags)0x01140000000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseWeaponActor, Mesh), Z_Construct_UClass_UStaticMeshComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Mesh_MetaData), NewProp_Mesh_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ASpellRiseWeaponActor_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseWeaponActor_Statics::NewProp_Mesh,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseWeaponActor_Statics::PropPointers) < 2048);
// ********** End Class ASpellRiseWeaponActor Property Definitions *********************************
UObject* (*const Z_Construct_UClass_ASpellRiseWeaponActor_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseWeaponActor_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ASpellRiseWeaponActor_Statics::ClassParams = {
	&ASpellRiseWeaponActor::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_ASpellRiseWeaponActor_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseWeaponActor_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseWeaponActor_Statics::Class_MetaDataParams), Z_Construct_UClass_ASpellRiseWeaponActor_Statics::Class_MetaDataParams)
};
void ASpellRiseWeaponActor::StaticRegisterNativesASpellRiseWeaponActor()
{
}
UClass* Z_Construct_UClass_ASpellRiseWeaponActor()
{
	if (!Z_Registration_Info_UClass_ASpellRiseWeaponActor.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ASpellRiseWeaponActor.OuterSingleton, Z_Construct_UClass_ASpellRiseWeaponActor_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ASpellRiseWeaponActor.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, ASpellRiseWeaponActor);
ASpellRiseWeaponActor::~ASpellRiseWeaponActor() {}
// ********** End Class ASpellRiseWeaponActor ******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Actors_SpellRiseWeaponActor_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ASpellRiseWeaponActor, ASpellRiseWeaponActor::StaticClass, TEXT("ASpellRiseWeaponActor"), &Z_Registration_Info_UClass_ASpellRiseWeaponActor, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ASpellRiseWeaponActor), 3341523028U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Actors_SpellRiseWeaponActor_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Actors_SpellRiseWeaponActor_h__Script_SpellRise_2075743475{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Actors_SpellRiseWeaponActor_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Actors_SpellRiseWeaponActor_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
