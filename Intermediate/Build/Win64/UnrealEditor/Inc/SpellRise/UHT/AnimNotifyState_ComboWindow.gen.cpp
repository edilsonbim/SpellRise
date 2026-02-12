// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Combat/Melee/AnimNotifies/AnimNotifyState_ComboWindow.h"
#include "GameplayTagContainer.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeAnimNotifyState_ComboWindow() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_UAnimNotifyState();
GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
SPELLRISE_API UClass* Z_Construct_UClass_UAnimNotifyState_ComboWindow();
SPELLRISE_API UClass* Z_Construct_UClass_UAnimNotifyState_ComboWindow_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UAnimNotifyState_ComboWindow *********************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UAnimNotifyState_ComboWindow;
UClass* UAnimNotifyState_ComboWindow::GetPrivateStaticClass()
{
	using TClass = UAnimNotifyState_ComboWindow;
	if (!Z_Registration_Info_UClass_UAnimNotifyState_ComboWindow.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("AnimNotifyState_ComboWindow"),
			Z_Registration_Info_UClass_UAnimNotifyState_ComboWindow.InnerSingleton,
			StaticRegisterNativesUAnimNotifyState_ComboWindow,
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
	return Z_Registration_Info_UClass_UAnimNotifyState_ComboWindow.InnerSingleton;
}
UClass* Z_Construct_UClass_UAnimNotifyState_ComboWindow_NoRegister()
{
	return UAnimNotifyState_ComboWindow::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "DisplayName", "SpellRise Combo Window" },
		{ "HideCategories", "Object" },
		{ "IncludePath", "Combat/Melee/AnimNotifies/AnimNotifyState_ComboWindow.h" },
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_ComboWindow.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EventBeginTag_MetaData[] = {
		{ "Category", "ComboWindow" },
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_ComboWindow.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EventEndTag_MetaData[] = {
		{ "Category", "ComboWindow" },
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_ComboWindow.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UAnimNotifyState_ComboWindow constinit property declarations *************
	static const UECodeGen_Private::FStructPropertyParams NewProp_EventBeginTag;
	static const UECodeGen_Private::FStructPropertyParams NewProp_EventEndTag;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UAnimNotifyState_ComboWindow constinit property declarations ***************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAnimNotifyState_ComboWindow>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics

// ********** Begin Class UAnimNotifyState_ComboWindow Property Definitions ************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::NewProp_EventBeginTag = { "EventBeginTag", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAnimNotifyState_ComboWindow, EventBeginTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EventBeginTag_MetaData), NewProp_EventBeginTag_MetaData) }; // 517357616
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::NewProp_EventEndTag = { "EventEndTag", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAnimNotifyState_ComboWindow, EventEndTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EventEndTag_MetaData), NewProp_EventEndTag_MetaData) }; // 517357616
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::NewProp_EventBeginTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::NewProp_EventEndTag,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::PropPointers) < 2048);
// ********** End Class UAnimNotifyState_ComboWindow Property Definitions **************************
UObject* (*const Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAnimNotifyState,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::ClassParams = {
	&UAnimNotifyState_ComboWindow::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::PropPointers),
	0,
	0x001130A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::Class_MetaDataParams), Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::Class_MetaDataParams)
};
void UAnimNotifyState_ComboWindow::StaticRegisterNativesUAnimNotifyState_ComboWindow()
{
}
UClass* Z_Construct_UClass_UAnimNotifyState_ComboWindow()
{
	if (!Z_Registration_Info_UClass_UAnimNotifyState_ComboWindow.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAnimNotifyState_ComboWindow.OuterSingleton, Z_Construct_UClass_UAnimNotifyState_ComboWindow_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAnimNotifyState_ComboWindow.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UAnimNotifyState_ComboWindow);
UAnimNotifyState_ComboWindow::~UAnimNotifyState_ComboWindow() {}
// ********** End Class UAnimNotifyState_ComboWindow ***********************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_AnimNotifies_AnimNotifyState_ComboWindow_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAnimNotifyState_ComboWindow, UAnimNotifyState_ComboWindow::StaticClass, TEXT("UAnimNotifyState_ComboWindow"), &Z_Registration_Info_UClass_UAnimNotifyState_ComboWindow, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAnimNotifyState_ComboWindow), 3167762737U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_AnimNotifies_AnimNotifyState_ComboWindow_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_AnimNotifies_AnimNotifyState_ComboWindow_h__Script_SpellRise_3001533876{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_AnimNotifies_AnimNotifyState_ComboWindow_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_AnimNotifies_AnimNotifyState_ComboWindow_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
