// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Core/SpellRisePlayerState.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSpellRisePlayerState() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_APlayerState();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilitySystemInterface_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRisePlayerState();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRisePlayerState_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UBasicAttributeSet_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UCatalystAttributeSet_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UCombatAttributeSet_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UDerivedStatsAttributeSet_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UResourceAttributeSet_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseAbilitySystemComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class ASpellRisePlayerState ****************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_ASpellRisePlayerState;
UClass* ASpellRisePlayerState::GetPrivateStaticClass()
{
	using TClass = ASpellRisePlayerState;
	if (!Z_Registration_Info_UClass_ASpellRisePlayerState.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRisePlayerState"),
			Z_Registration_Info_UClass_ASpellRisePlayerState.InnerSingleton,
			StaticRegisterNativesASpellRisePlayerState,
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
	return Z_Registration_Info_UClass_ASpellRisePlayerState.InnerSingleton;
}
UClass* Z_Construct_UClass_ASpellRisePlayerState_NoRegister()
{
	return ASpellRisePlayerState::GetPrivateStaticClass();
}
struct Z_Construct_UClass_ASpellRisePlayerState_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "Core/SpellRisePlayerState.h" },
		{ "ModuleRelativePath", "Core/SpellRisePlayerState.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AbilitySystemComponent_MetaData[] = {
		{ "Category", "GAS" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Core/SpellRisePlayerState.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BasicAttributeSet_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "GAS" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Attribute sets live on PlayerState (authoritative GAS owner)\n" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Core/SpellRisePlayerState.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Attribute sets live on PlayerState (authoritative GAS owner)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CombatAttributeSet_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "GAS" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Core/SpellRisePlayerState.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ResourceAttributeSet_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "GAS" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Core/SpellRisePlayerState.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CatalystAttributeSet_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "GAS" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Core/SpellRisePlayerState.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DerivedStatsAttributeSet_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "GAS" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Core/SpellRisePlayerState.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class ASpellRisePlayerState constinit property declarations ********************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_AbilitySystemComponent;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_BasicAttributeSet;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CombatAttributeSet;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ResourceAttributeSet;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CatalystAttributeSet;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_DerivedStatsAttributeSet;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class ASpellRisePlayerState constinit property declarations **********************
	static UObject* (*const DependentSingletons[])();
	static const UECodeGen_Private::FImplementedInterfaceParams InterfaceParams[];
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASpellRisePlayerState>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_ASpellRisePlayerState_Statics

// ********** Begin Class ASpellRisePlayerState Property Definitions *******************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_AbilitySystemComponent = { "AbilitySystemComponent", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRisePlayerState, AbilitySystemComponent), Z_Construct_UClass_USpellRiseAbilitySystemComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AbilitySystemComponent_MetaData), NewProp_AbilitySystemComponent_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_BasicAttributeSet = { "BasicAttributeSet", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRisePlayerState, BasicAttributeSet), Z_Construct_UClass_UBasicAttributeSet_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BasicAttributeSet_MetaData), NewProp_BasicAttributeSet_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_CombatAttributeSet = { "CombatAttributeSet", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRisePlayerState, CombatAttributeSet), Z_Construct_UClass_UCombatAttributeSet_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CombatAttributeSet_MetaData), NewProp_CombatAttributeSet_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_ResourceAttributeSet = { "ResourceAttributeSet", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRisePlayerState, ResourceAttributeSet), Z_Construct_UClass_UResourceAttributeSet_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ResourceAttributeSet_MetaData), NewProp_ResourceAttributeSet_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_CatalystAttributeSet = { "CatalystAttributeSet", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRisePlayerState, CatalystAttributeSet), Z_Construct_UClass_UCatalystAttributeSet_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CatalystAttributeSet_MetaData), NewProp_CatalystAttributeSet_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_DerivedStatsAttributeSet = { "DerivedStatsAttributeSet", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRisePlayerState, DerivedStatsAttributeSet), Z_Construct_UClass_UDerivedStatsAttributeSet_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DerivedStatsAttributeSet_MetaData), NewProp_DerivedStatsAttributeSet_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ASpellRisePlayerState_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_AbilitySystemComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_BasicAttributeSet,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_CombatAttributeSet,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_ResourceAttributeSet,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_CatalystAttributeSet,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRisePlayerState_Statics::NewProp_DerivedStatsAttributeSet,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRisePlayerState_Statics::PropPointers) < 2048);
// ********** End Class ASpellRisePlayerState Property Definitions *********************************
UObject* (*const Z_Construct_UClass_ASpellRisePlayerState_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_APlayerState,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRisePlayerState_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FImplementedInterfaceParams Z_Construct_UClass_ASpellRisePlayerState_Statics::InterfaceParams[] = {
	{ Z_Construct_UClass_UAbilitySystemInterface_NoRegister, (int32)VTABLE_OFFSET(ASpellRisePlayerState, IAbilitySystemInterface), false },  // 2722098046
};
const UECodeGen_Private::FClassParams Z_Construct_UClass_ASpellRisePlayerState_Statics::ClassParams = {
	&ASpellRisePlayerState::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_ASpellRisePlayerState_Statics::PropPointers,
	InterfaceParams,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRisePlayerState_Statics::PropPointers),
	UE_ARRAY_COUNT(InterfaceParams),
	0x009002A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRisePlayerState_Statics::Class_MetaDataParams), Z_Construct_UClass_ASpellRisePlayerState_Statics::Class_MetaDataParams)
};
void ASpellRisePlayerState::StaticRegisterNativesASpellRisePlayerState()
{
}
UClass* Z_Construct_UClass_ASpellRisePlayerState()
{
	if (!Z_Registration_Info_UClass_ASpellRisePlayerState.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ASpellRisePlayerState.OuterSingleton, Z_Construct_UClass_ASpellRisePlayerState_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ASpellRisePlayerState.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, ASpellRisePlayerState);
ASpellRisePlayerState::~ASpellRisePlayerState() {}
// ********** End Class ASpellRisePlayerState ******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ASpellRisePlayerState, ASpellRisePlayerState::StaticClass, TEXT("ASpellRisePlayerState"), &Z_Registration_Info_UClass_ASpellRisePlayerState, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ASpellRisePlayerState), 3286566693U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h__Script_SpellRise_124100320{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
