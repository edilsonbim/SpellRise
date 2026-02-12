// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGA_CastSpellBase.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayTagContainer.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSpellRiseGA_CastSpellBase() {}

// ********** Begin Cross Module References ********************************************************
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_WaitGameplayEvent_NoRegister();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayEventData();
GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGA_CastSpellBase();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGA_CastSpellBase_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGameplayAbility();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class USpellRiseGA_CastSpellBase Function HandleInterruptEventReceived *********
struct Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics
{
	struct SpellRiseGA_CastSpellBase_eventHandleInterruptEventReceived_Parms
	{
		FGameplayEventData Payload;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGA_CastSpellBase.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function HandleInterruptEventReceived constinit property declarations **********
	static const UECodeGen_Private::FStructPropertyParams NewProp_Payload;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function HandleInterruptEventReceived constinit property declarations ************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function HandleInterruptEventReceived Property Definitions *********************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::NewProp_Payload = { "Payload", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseGA_CastSpellBase_eventHandleInterruptEventReceived_Parms, Payload), Z_Construct_UScriptStruct_FGameplayEventData, METADATA_PARAMS(0, nullptr) }; // 1110230437
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::NewProp_Payload,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::PropPointers) < 2048);
// ********** End Function HandleInterruptEventReceived Property Definitions ***********************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGA_CastSpellBase, nullptr, "HandleInterruptEventReceived", 	Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::SpellRiseGA_CastSpellBase_eventHandleInterruptEventReceived_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::SpellRiseGA_CastSpellBase_eventHandleInterruptEventReceived_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseGA_CastSpellBase::execHandleInterruptEventReceived)
{
	P_GET_STRUCT(FGameplayEventData,Z_Param_Payload);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->HandleInterruptEventReceived(Z_Param_Payload);
	P_NATIVE_END;
}
// ********** End Class USpellRiseGA_CastSpellBase Function HandleInterruptEventReceived ***********

// ********** Begin Class USpellRiseGA_CastSpellBase ***********************************************
FClassRegistrationInfo Z_Registration_Info_UClass_USpellRiseGA_CastSpellBase;
UClass* USpellRiseGA_CastSpellBase::GetPrivateStaticClass()
{
	using TClass = USpellRiseGA_CastSpellBase;
	if (!Z_Registration_Info_UClass_USpellRiseGA_CastSpellBase.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRiseGA_CastSpellBase"),
			Z_Registration_Info_UClass_USpellRiseGA_CastSpellBase.InnerSingleton,
			StaticRegisterNativesUSpellRiseGA_CastSpellBase,
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
	return Z_Registration_Info_UClass_USpellRiseGA_CastSpellBase.InnerSingleton;
}
UClass* Z_Construct_UClass_USpellRiseGA_CastSpellBase_NoRegister()
{
	return USpellRiseGA_CastSpellBase::GetPrivateStaticClass();
}
struct Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Base class for castable spells (hold to cast, release to fire).\n * Uses the casting flow implemented in USpellRiseGameplayAbility (StartCastingFlow / BP_OnCastFired).\n *\n * This class only adds an optional interrupt listener (GameplayEvent) to cancel casting.\n */" },
#endif
		{ "IncludePath", "GameplayAbilitySystem/Abilities/SpellRiseGA_CastSpellBase.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGA_CastSpellBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Base class for castable spells (hold to cast, release to fire).\nUses the casting flow implemented in USpellRiseGameplayAbility (StartCastingFlow / BP_OnCastFired).\n\nThis class only adds an optional interrupt listener (GameplayEvent) to cancel casting." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InterruptEventTag_MetaData[] = {
		{ "Category", "Cast|Interrupt" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** If received while casting, cancels the ability. */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGA_CastSpellBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "If received while casting, cancels the ability." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WaitInterruptTask_MetaData[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGA_CastSpellBase.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class USpellRiseGA_CastSpellBase constinit property declarations ***************
	static const UECodeGen_Private::FStructPropertyParams NewProp_InterruptEventTag;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WaitInterruptTask;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class USpellRiseGA_CastSpellBase constinit property declarations *****************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("HandleInterruptEventReceived"), .Pointer = &USpellRiseGA_CastSpellBase::execHandleInterruptEventReceived },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_USpellRiseGA_CastSpellBase_HandleInterruptEventReceived, "HandleInterruptEventReceived" }, // 4247489342
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USpellRiseGA_CastSpellBase>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics

// ********** Begin Class USpellRiseGA_CastSpellBase Property Definitions **************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::NewProp_InterruptEventTag = { "InterruptEventTag", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseGA_CastSpellBase, InterruptEventTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InterruptEventTag_MetaData), NewProp_InterruptEventTag_MetaData) }; // 517357616
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::NewProp_WaitInterruptTask = { "WaitInterruptTask", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseGA_CastSpellBase, WaitInterruptTask), Z_Construct_UClass_UAbilityTask_WaitGameplayEvent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WaitInterruptTask_MetaData), NewProp_WaitInterruptTask_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::NewProp_InterruptEventTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::NewProp_WaitInterruptTask,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::PropPointers) < 2048);
// ********** End Class USpellRiseGA_CastSpellBase Property Definitions ****************************
UObject* (*const Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USpellRiseGameplayAbility,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::ClassParams = {
	&USpellRiseGA_CastSpellBase::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::PropPointers),
	0,
	0x001000A1u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::Class_MetaDataParams), Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::Class_MetaDataParams)
};
void USpellRiseGA_CastSpellBase::StaticRegisterNativesUSpellRiseGA_CastSpellBase()
{
	UClass* Class = USpellRiseGA_CastSpellBase::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::Funcs));
}
UClass* Z_Construct_UClass_USpellRiseGA_CastSpellBase()
{
	if (!Z_Registration_Info_UClass_USpellRiseGA_CastSpellBase.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USpellRiseGA_CastSpellBase.OuterSingleton, Z_Construct_UClass_USpellRiseGA_CastSpellBase_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USpellRiseGA_CastSpellBase.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, USpellRiseGA_CastSpellBase);
USpellRiseGA_CastSpellBase::~USpellRiseGA_CastSpellBase() {}
// ********** End Class USpellRiseGA_CastSpellBase *************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGA_CastSpellBase_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USpellRiseGA_CastSpellBase, USpellRiseGA_CastSpellBase::StaticClass, TEXT("USpellRiseGA_CastSpellBase"), &Z_Registration_Info_UClass_USpellRiseGA_CastSpellBase, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USpellRiseGA_CastSpellBase), 1731080649U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGA_CastSpellBase_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGA_CastSpellBase_h__Script_SpellRise_4117875882{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGA_CastSpellBase_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGA_CastSpellBase_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
