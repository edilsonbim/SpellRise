// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"
#include "AttributeSet.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeCatalystAttributeSet() {}

// ********** Begin Cross Module References ********************************************************
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAttributeSet();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAttributeData();
SPELLRISE_API UClass* Z_Construct_UClass_UCatalystAttributeSet();
SPELLRISE_API UClass* Z_Construct_UClass_UCatalystAttributeSet_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UCatalystAttributeSet Function OnRep_CatalystCharge **********************
struct Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics
{
	struct CatalystAttributeSet_eventOnRep_CatalystCharge_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// OnRep\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "OnRep" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_CatalystCharge constinit property declarations ******************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_CatalystCharge constinit property declarations ********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_CatalystCharge Property Definitions *****************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CatalystAttributeSet_eventOnRep_CatalystCharge_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::PropPointers) < 2048);
// ********** End Function OnRep_CatalystCharge Property Definitions *******************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCatalystAttributeSet, nullptr, "OnRep_CatalystCharge", 	Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::CatalystAttributeSet_eventOnRep_CatalystCharge_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::CatalystAttributeSet_eventOnRep_CatalystCharge_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCatalystAttributeSet::execOnRep_CatalystCharge)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_CatalystCharge(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCatalystAttributeSet Function OnRep_CatalystCharge ************************

// ********** Begin Class UCatalystAttributeSet Function OnRep_CatalystLevel ***********************
struct Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics
{
	struct CatalystAttributeSet_eventOnRep_CatalystLevel_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_CatalystLevel constinit property declarations *******************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_CatalystLevel constinit property declarations *********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_CatalystLevel Property Definitions ******************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CatalystAttributeSet_eventOnRep_CatalystLevel_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::PropPointers) < 2048);
// ********** End Function OnRep_CatalystLevel Property Definitions ********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCatalystAttributeSet, nullptr, "OnRep_CatalystLevel", 	Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::CatalystAttributeSet_eventOnRep_CatalystLevel_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::CatalystAttributeSet_eventOnRep_CatalystLevel_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCatalystAttributeSet::execOnRep_CatalystLevel)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_CatalystLevel(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCatalystAttributeSet Function OnRep_CatalystLevel *************************

// ********** Begin Class UCatalystAttributeSet Function OnRep_CatalystXP **************************
struct Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics
{
	struct CatalystAttributeSet_eventOnRep_CatalystXP_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_CatalystXP constinit property declarations **********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_CatalystXP constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_CatalystXP Property Definitions *********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CatalystAttributeSet_eventOnRep_CatalystXP_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::PropPointers) < 2048);
// ********** End Function OnRep_CatalystXP Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCatalystAttributeSet, nullptr, "OnRep_CatalystXP", 	Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::CatalystAttributeSet_eventOnRep_CatalystXP_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::CatalystAttributeSet_eventOnRep_CatalystXP_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCatalystAttributeSet::execOnRep_CatalystXP)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_CatalystXP(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCatalystAttributeSet Function OnRep_CatalystXP ****************************

// ********** Begin Class UCatalystAttributeSet ****************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UCatalystAttributeSet;
UClass* UCatalystAttributeSet::GetPrivateStaticClass()
{
	using TClass = UCatalystAttributeSet;
	if (!Z_Registration_Info_UClass_UCatalystAttributeSet.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("CatalystAttributeSet"),
			Z_Registration_Info_UClass_UCatalystAttributeSet.InnerSingleton,
			StaticRegisterNativesUCatalystAttributeSet,
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
	return Z_Registration_Info_UClass_UCatalystAttributeSet.InnerSingleton;
}
UClass* Z_Construct_UClass_UCatalystAttributeSet_NoRegister()
{
	return UCatalystAttributeSet::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UCatalystAttributeSet_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CatalystCharge_MetaData[] = {
		{ "Category", "Attributes|Catalyst" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// 0..100\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "0..100" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CatalystXP_MetaData[] = {
		{ "Category", "Attributes|Catalyst" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// persistent progression (>=0)\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "persistent progression (>=0)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CatalystLevel_MetaData[] = {
		{ "Category", "Attributes|Catalyst" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// 1..3\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "1..3" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CatalystChargeDelta_MetaData[] = {
		{ "Category", "Attributes|Catalyst|Meta" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Meta: somar carga via GE simples (Add no Delta) e consumir aqui\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Meta: somar carga via GE simples (Add no Delta) e consumir aqui" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UCatalystAttributeSet constinit property declarations ********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_CatalystCharge;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CatalystXP;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CatalystLevel;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CatalystChargeDelta;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UCatalystAttributeSet constinit property declarations **********************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("OnRep_CatalystCharge"), .Pointer = &UCatalystAttributeSet::execOnRep_CatalystCharge },
		{ .NameUTF8 = UTF8TEXT("OnRep_CatalystLevel"), .Pointer = &UCatalystAttributeSet::execOnRep_CatalystLevel },
		{ .NameUTF8 = UTF8TEXT("OnRep_CatalystXP"), .Pointer = &UCatalystAttributeSet::execOnRep_CatalystXP },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystCharge, "OnRep_CatalystCharge" }, // 2141432755
		{ &Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystLevel, "OnRep_CatalystLevel" }, // 3402080011
		{ &Z_Construct_UFunction_UCatalystAttributeSet_OnRep_CatalystXP, "OnRep_CatalystXP" }, // 3462323162
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCatalystAttributeSet>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UCatalystAttributeSet_Statics

// ********** Begin Class UCatalystAttributeSet Property Definitions *******************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCatalystAttributeSet_Statics::NewProp_CatalystCharge = { "CatalystCharge", "OnRep_CatalystCharge", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystAttributeSet, CatalystCharge), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CatalystCharge_MetaData), NewProp_CatalystCharge_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCatalystAttributeSet_Statics::NewProp_CatalystXP = { "CatalystXP", "OnRep_CatalystXP", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystAttributeSet, CatalystXP), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CatalystXP_MetaData), NewProp_CatalystXP_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCatalystAttributeSet_Statics::NewProp_CatalystLevel = { "CatalystLevel", "OnRep_CatalystLevel", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystAttributeSet, CatalystLevel), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CatalystLevel_MetaData), NewProp_CatalystLevel_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCatalystAttributeSet_Statics::NewProp_CatalystChargeDelta = { "CatalystChargeDelta", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystAttributeSet, CatalystChargeDelta), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CatalystChargeDelta_MetaData), NewProp_CatalystChargeDelta_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UCatalystAttributeSet_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystAttributeSet_Statics::NewProp_CatalystCharge,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystAttributeSet_Statics::NewProp_CatalystXP,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystAttributeSet_Statics::NewProp_CatalystLevel,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystAttributeSet_Statics::NewProp_CatalystChargeDelta,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCatalystAttributeSet_Statics::PropPointers) < 2048);
// ********** End Class UCatalystAttributeSet Property Definitions *********************************
UObject* (*const Z_Construct_UClass_UCatalystAttributeSet_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAttributeSet,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCatalystAttributeSet_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCatalystAttributeSet_Statics::ClassParams = {
	&UCatalystAttributeSet::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UCatalystAttributeSet_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UCatalystAttributeSet_Statics::PropPointers),
	0,
	0x003000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCatalystAttributeSet_Statics::Class_MetaDataParams), Z_Construct_UClass_UCatalystAttributeSet_Statics::Class_MetaDataParams)
};
void UCatalystAttributeSet::StaticRegisterNativesUCatalystAttributeSet()
{
	UClass* Class = UCatalystAttributeSet::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UCatalystAttributeSet_Statics::Funcs));
}
UClass* Z_Construct_UClass_UCatalystAttributeSet()
{
	if (!Z_Registration_Info_UClass_UCatalystAttributeSet.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCatalystAttributeSet.OuterSingleton, Z_Construct_UClass_UCatalystAttributeSet_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCatalystAttributeSet.OuterSingleton;
}
#if VALIDATE_CLASS_REPS
void UCatalystAttributeSet::ValidateGeneratedRepEnums(const TArray<struct FRepRecord>& ClassReps) const
{
	static FName Name_CatalystCharge(TEXT("CatalystCharge"));
	static FName Name_CatalystXP(TEXT("CatalystXP"));
	static FName Name_CatalystLevel(TEXT("CatalystLevel"));
	const bool bIsValid = true
		&& Name_CatalystCharge == ClassReps[(int32)ENetFields_Private::CatalystCharge].Property->GetFName()
		&& Name_CatalystXP == ClassReps[(int32)ENetFields_Private::CatalystXP].Property->GetFName()
		&& Name_CatalystLevel == ClassReps[(int32)ENetFields_Private::CatalystLevel].Property->GetFName();
	checkf(bIsValid, TEXT("UHT Generated Rep Indices do not match runtime populated Rep Indices for properties in UCatalystAttributeSet"));
}
#endif
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UCatalystAttributeSet);
UCatalystAttributeSet::~UCatalystAttributeSet() {}
// ********** End Class UCatalystAttributeSet ******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCatalystAttributeSet, UCatalystAttributeSet::StaticClass, TEXT("UCatalystAttributeSet"), &Z_Registration_Info_UClass_UCatalystAttributeSet, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCatalystAttributeSet), 3831621733U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h__Script_SpellRise_4182038985{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
