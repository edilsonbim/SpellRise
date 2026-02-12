// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"
#include "AttributeSet.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeDerivedStatsAttributeSet() {}

// ********** Begin Cross Module References ********************************************************
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAttributeSet();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAttributeData();
SPELLRISE_API UClass* Z_Construct_UClass_UDerivedStatsAttributeSet();
SPELLRISE_API UClass* Z_Construct_UClass_UDerivedStatsAttributeSet_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UDerivedStatsAttributeSet Function OnRep_ArmorPenetration ****************
struct Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics
{
	struct DerivedStatsAttributeSet_eventOnRep_ArmorPenetration_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_ArmorPenetration constinit property declarations ****************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_ArmorPenetration constinit property declarations ******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_ArmorPenetration Property Definitions ***************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DerivedStatsAttributeSet_eventOnRep_ArmorPenetration_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::PropPointers) < 2048);
// ********** End Function OnRep_ArmorPenetration Property Definitions *****************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDerivedStatsAttributeSet, nullptr, "OnRep_ArmorPenetration", 	Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::DerivedStatsAttributeSet_eventOnRep_ArmorPenetration_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::DerivedStatsAttributeSet_eventOnRep_ArmorPenetration_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDerivedStatsAttributeSet::execOnRep_ArmorPenetration)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_ArmorPenetration(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UDerivedStatsAttributeSet Function OnRep_ArmorPenetration ******************

// ********** Begin Class UDerivedStatsAttributeSet Function OnRep_AttackSpeedMultiplier ***********
struct Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics
{
	struct DerivedStatsAttributeSet_eventOnRep_AttackSpeedMultiplier_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_AttackSpeedMultiplier constinit property declarations ***********
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_AttackSpeedMultiplier constinit property declarations *************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_AttackSpeedMultiplier Property Definitions **********************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DerivedStatsAttributeSet_eventOnRep_AttackSpeedMultiplier_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::PropPointers) < 2048);
// ********** End Function OnRep_AttackSpeedMultiplier Property Definitions ************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDerivedStatsAttributeSet, nullptr, "OnRep_AttackSpeedMultiplier", 	Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::DerivedStatsAttributeSet_eventOnRep_AttackSpeedMultiplier_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::DerivedStatsAttributeSet_eventOnRep_AttackSpeedMultiplier_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDerivedStatsAttributeSet::execOnRep_AttackSpeedMultiplier)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_AttackSpeedMultiplier(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UDerivedStatsAttributeSet Function OnRep_AttackSpeedMultiplier *************

// ********** Begin Class UDerivedStatsAttributeSet Function OnRep_BreakPower **********************
struct Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics
{
	struct DerivedStatsAttributeSet_eventOnRep_BreakPower_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_BreakPower constinit property declarations **********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_BreakPower constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_BreakPower Property Definitions *********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DerivedStatsAttributeSet_eventOnRep_BreakPower_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::PropPointers) < 2048);
// ********** End Function OnRep_BreakPower Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDerivedStatsAttributeSet, nullptr, "OnRep_BreakPower", 	Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::DerivedStatsAttributeSet_eventOnRep_BreakPower_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::DerivedStatsAttributeSet_eventOnRep_BreakPower_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDerivedStatsAttributeSet::execOnRep_BreakPower)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_BreakPower(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UDerivedStatsAttributeSet Function OnRep_BreakPower ************************

// ********** Begin Class UDerivedStatsAttributeSet Function OnRep_CastSpeedMultiplier *************
struct Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics
{
	struct DerivedStatsAttributeSet_eventOnRep_CastSpeedMultiplier_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_CastSpeedMultiplier constinit property declarations *************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_CastSpeedMultiplier constinit property declarations ***************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_CastSpeedMultiplier Property Definitions ************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DerivedStatsAttributeSet_eventOnRep_CastSpeedMultiplier_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::PropPointers) < 2048);
// ********** End Function OnRep_CastSpeedMultiplier Property Definitions **************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDerivedStatsAttributeSet, nullptr, "OnRep_CastSpeedMultiplier", 	Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::DerivedStatsAttributeSet_eventOnRep_CastSpeedMultiplier_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::DerivedStatsAttributeSet_eventOnRep_CastSpeedMultiplier_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDerivedStatsAttributeSet::execOnRep_CastSpeedMultiplier)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_CastSpeedMultiplier(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UDerivedStatsAttributeSet Function OnRep_CastSpeedMultiplier ***************

// ********** Begin Class UDerivedStatsAttributeSet Function OnRep_CastStability *******************
struct Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics
{
	struct DerivedStatsAttributeSet_eventOnRep_CastStability_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_CastStability constinit property declarations *******************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_CastStability constinit property declarations *********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_CastStability Property Definitions ******************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DerivedStatsAttributeSet_eventOnRep_CastStability_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::PropPointers) < 2048);
// ********** End Function OnRep_CastStability Property Definitions ********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDerivedStatsAttributeSet, nullptr, "OnRep_CastStability", 	Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::DerivedStatsAttributeSet_eventOnRep_CastStability_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::DerivedStatsAttributeSet_eventOnRep_CastStability_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDerivedStatsAttributeSet::execOnRep_CastStability)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_CastStability(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UDerivedStatsAttributeSet Function OnRep_CastStability *********************

// ********** Begin Class UDerivedStatsAttributeSet Function OnRep_CritChance **********************
struct Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics
{
	struct DerivedStatsAttributeSet_eventOnRep_CritChance_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_CritChance constinit property declarations **********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_CritChance constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_CritChance Property Definitions *********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DerivedStatsAttributeSet_eventOnRep_CritChance_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::PropPointers) < 2048);
// ********** End Function OnRep_CritChance Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDerivedStatsAttributeSet, nullptr, "OnRep_CritChance", 	Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::DerivedStatsAttributeSet_eventOnRep_CritChance_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::DerivedStatsAttributeSet_eventOnRep_CritChance_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDerivedStatsAttributeSet::execOnRep_CritChance)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_CritChance(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UDerivedStatsAttributeSet Function OnRep_CritChance ************************

// ********** Begin Class UDerivedStatsAttributeSet Function OnRep_CritDamage **********************
struct Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics
{
	struct DerivedStatsAttributeSet_eventOnRep_CritDamage_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_CritDamage constinit property declarations **********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_CritDamage constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_CritDamage Property Definitions *********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DerivedStatsAttributeSet_eventOnRep_CritDamage_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::PropPointers) < 2048);
// ********** End Function OnRep_CritDamage Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDerivedStatsAttributeSet, nullptr, "OnRep_CritDamage", 	Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::DerivedStatsAttributeSet_eventOnRep_CritDamage_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::DerivedStatsAttributeSet_eventOnRep_CritDamage_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDerivedStatsAttributeSet::execOnRep_CritDamage)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_CritDamage(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UDerivedStatsAttributeSet Function OnRep_CritDamage ************************

// ********** Begin Class UDerivedStatsAttributeSet Function OnRep_ElementPenetration **************
struct Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics
{
	struct DerivedStatsAttributeSet_eventOnRep_ElementPenetration_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_ElementPenetration constinit property declarations **************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_ElementPenetration constinit property declarations ****************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_ElementPenetration Property Definitions *************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DerivedStatsAttributeSet_eventOnRep_ElementPenetration_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::PropPointers) < 2048);
// ********** End Function OnRep_ElementPenetration Property Definitions ***************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDerivedStatsAttributeSet, nullptr, "OnRep_ElementPenetration", 	Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::DerivedStatsAttributeSet_eventOnRep_ElementPenetration_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::DerivedStatsAttributeSet_eventOnRep_ElementPenetration_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDerivedStatsAttributeSet::execOnRep_ElementPenetration)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_ElementPenetration(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UDerivedStatsAttributeSet Function OnRep_ElementPenetration ****************

// ********** Begin Class UDerivedStatsAttributeSet Function OnRep_ManaCostMultiplier **************
struct Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics
{
	struct DerivedStatsAttributeSet_eventOnRep_ManaCostMultiplier_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_ManaCostMultiplier constinit property declarations **************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_ManaCostMultiplier constinit property declarations ****************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_ManaCostMultiplier Property Definitions *************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DerivedStatsAttributeSet_eventOnRep_ManaCostMultiplier_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::PropPointers) < 2048);
// ********** End Function OnRep_ManaCostMultiplier Property Definitions ***************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDerivedStatsAttributeSet, nullptr, "OnRep_ManaCostMultiplier", 	Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::DerivedStatsAttributeSet_eventOnRep_ManaCostMultiplier_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::DerivedStatsAttributeSet_eventOnRep_ManaCostMultiplier_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDerivedStatsAttributeSet::execOnRep_ManaCostMultiplier)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_ManaCostMultiplier(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UDerivedStatsAttributeSet Function OnRep_ManaCostMultiplier ****************

// ********** Begin Class UDerivedStatsAttributeSet Function OnRep_SpellPowerMultiplier ************
struct Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics
{
	struct DerivedStatsAttributeSet_eventOnRep_SpellPowerMultiplier_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_SpellPowerMultiplier constinit property declarations ************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_SpellPowerMultiplier constinit property declarations **************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_SpellPowerMultiplier Property Definitions ***********************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DerivedStatsAttributeSet_eventOnRep_SpellPowerMultiplier_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::PropPointers) < 2048);
// ********** End Function OnRep_SpellPowerMultiplier Property Definitions *************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDerivedStatsAttributeSet, nullptr, "OnRep_SpellPowerMultiplier", 	Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::DerivedStatsAttributeSet_eventOnRep_SpellPowerMultiplier_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::DerivedStatsAttributeSet_eventOnRep_SpellPowerMultiplier_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDerivedStatsAttributeSet::execOnRep_SpellPowerMultiplier)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_SpellPowerMultiplier(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UDerivedStatsAttributeSet Function OnRep_SpellPowerMultiplier **************

// ********** Begin Class UDerivedStatsAttributeSet Function OnRep_WeaponDamageMultiplier **********
struct Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics
{
	struct DerivedStatsAttributeSet_eventOnRep_WeaponDamageMultiplier_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_WeaponDamageMultiplier constinit property declarations **********
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_WeaponDamageMultiplier constinit property declarations ************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_WeaponDamageMultiplier Property Definitions *********************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DerivedStatsAttributeSet_eventOnRep_WeaponDamageMultiplier_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::PropPointers) < 2048);
// ********** End Function OnRep_WeaponDamageMultiplier Property Definitions ***********************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDerivedStatsAttributeSet, nullptr, "OnRep_WeaponDamageMultiplier", 	Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::DerivedStatsAttributeSet_eventOnRep_WeaponDamageMultiplier_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::DerivedStatsAttributeSet_eventOnRep_WeaponDamageMultiplier_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDerivedStatsAttributeSet::execOnRep_WeaponDamageMultiplier)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_WeaponDamageMultiplier(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UDerivedStatsAttributeSet Function OnRep_WeaponDamageMultiplier ************

// ********** Begin Class UDerivedStatsAttributeSet ************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UDerivedStatsAttributeSet;
UClass* UDerivedStatsAttributeSet::GetPrivateStaticClass()
{
	using TClass = UDerivedStatsAttributeSet;
	if (!Z_Registration_Info_UClass_UDerivedStatsAttributeSet.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("DerivedStatsAttributeSet"),
			Z_Registration_Info_UClass_UDerivedStatsAttributeSet.InnerSingleton,
			StaticRegisterNativesUDerivedStatsAttributeSet,
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
	return Z_Registration_Info_UClass_UDerivedStatsAttributeSet.InnerSingleton;
}
UClass* Z_Construct_UClass_UDerivedStatsAttributeSet_NoRegister()
{
	return UDerivedStatsAttributeSet::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UDerivedStatsAttributeSet_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Derived stats (calculated from primaries + gear/buffs/catalyst).\n *\n * Conventions:\n * - Multipliers: 1.0 = normal (e.g. 1.10 = +10%)\n * - CritChance:  0..1 (e.g. 0.10 = 10%)\n * - CritDamage:  multiplier total (e.g. 1.5 = 150%)\n * - Penetrations: percent 0..75\n * - BreakPower / CastStability: scalar values\n */" },
#endif
		{ "IncludePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Derived stats (calculated from primaries + gear/buffs/catalyst).\n\nConventions:\n- Multipliers: 1.0 = normal (e.g. 1.10 = +10%)\n- CritChance:  0..1 (e.g. 0.10 = 10%)\n- CritDamage:  multiplier total (e.g. 1.5 = 150%)\n- Penetrations: percent 0..75\n- BreakPower / CastStability: scalar values" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WeaponDamageMultiplier_MetaData[] = {
		{ "Category", "Attributes|Derived|Multipliers" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// -------------------------\n// Multipliers / Crit\n// -------------------------\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Multipliers / Crit" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AttackSpeedMultiplier_MetaData[] = {
		{ "Category", "Attributes|Derived|Multipliers" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CritChance_MetaData[] = {
		{ "Category", "Attributes|Derived|Crit" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CritDamage_MetaData[] = {
		{ "Category", "Attributes|Derived|Crit" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SpellPowerMultiplier_MetaData[] = {
		{ "Category", "Attributes|Derived|Multipliers" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CastSpeedMultiplier_MetaData[] = {
		{ "Category", "Attributes|Derived|Multipliers" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ManaCostMultiplier_MetaData[] = {
		{ "Category", "Attributes|Derived|Multipliers" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BreakPower_MetaData[] = {
		{ "Category", "Attributes|Derived|Utility" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// -------------------------\n// Utility (scalar values)\n// -------------------------\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Utility (scalar values)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CastStability_MetaData[] = {
		{ "Category", "Attributes|Derived|Utility" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ArmorPenetration_MetaData[] = {
		{ "Category", "Attributes|Derived|Penetration" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// -------------------------\n// Penetrations (percent)\n// -------------------------\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Penetrations (percent)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ElementPenetration_MetaData[] = {
		{ "Category", "Attributes|Derived|Penetration" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UDerivedStatsAttributeSet constinit property declarations ****************
	static const UECodeGen_Private::FStructPropertyParams NewProp_WeaponDamageMultiplier;
	static const UECodeGen_Private::FStructPropertyParams NewProp_AttackSpeedMultiplier;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CritChance;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CritDamage;
	static const UECodeGen_Private::FStructPropertyParams NewProp_SpellPowerMultiplier;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CastSpeedMultiplier;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ManaCostMultiplier;
	static const UECodeGen_Private::FStructPropertyParams NewProp_BreakPower;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CastStability;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ArmorPenetration;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ElementPenetration;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UDerivedStatsAttributeSet constinit property declarations ******************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("OnRep_ArmorPenetration"), .Pointer = &UDerivedStatsAttributeSet::execOnRep_ArmorPenetration },
		{ .NameUTF8 = UTF8TEXT("OnRep_AttackSpeedMultiplier"), .Pointer = &UDerivedStatsAttributeSet::execOnRep_AttackSpeedMultiplier },
		{ .NameUTF8 = UTF8TEXT("OnRep_BreakPower"), .Pointer = &UDerivedStatsAttributeSet::execOnRep_BreakPower },
		{ .NameUTF8 = UTF8TEXT("OnRep_CastSpeedMultiplier"), .Pointer = &UDerivedStatsAttributeSet::execOnRep_CastSpeedMultiplier },
		{ .NameUTF8 = UTF8TEXT("OnRep_CastStability"), .Pointer = &UDerivedStatsAttributeSet::execOnRep_CastStability },
		{ .NameUTF8 = UTF8TEXT("OnRep_CritChance"), .Pointer = &UDerivedStatsAttributeSet::execOnRep_CritChance },
		{ .NameUTF8 = UTF8TEXT("OnRep_CritDamage"), .Pointer = &UDerivedStatsAttributeSet::execOnRep_CritDamage },
		{ .NameUTF8 = UTF8TEXT("OnRep_ElementPenetration"), .Pointer = &UDerivedStatsAttributeSet::execOnRep_ElementPenetration },
		{ .NameUTF8 = UTF8TEXT("OnRep_ManaCostMultiplier"), .Pointer = &UDerivedStatsAttributeSet::execOnRep_ManaCostMultiplier },
		{ .NameUTF8 = UTF8TEXT("OnRep_SpellPowerMultiplier"), .Pointer = &UDerivedStatsAttributeSet::execOnRep_SpellPowerMultiplier },
		{ .NameUTF8 = UTF8TEXT("OnRep_WeaponDamageMultiplier"), .Pointer = &UDerivedStatsAttributeSet::execOnRep_WeaponDamageMultiplier },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ArmorPenetration, "OnRep_ArmorPenetration" }, // 2615981277
		{ &Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_AttackSpeedMultiplier, "OnRep_AttackSpeedMultiplier" }, // 2105122198
		{ &Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_BreakPower, "OnRep_BreakPower" }, // 3006278291
		{ &Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastSpeedMultiplier, "OnRep_CastSpeedMultiplier" }, // 3485453316
		{ &Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CastStability, "OnRep_CastStability" }, // 2870632701
		{ &Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritChance, "OnRep_CritChance" }, // 949174153
		{ &Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_CritDamage, "OnRep_CritDamage" }, // 1618171615
		{ &Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ElementPenetration, "OnRep_ElementPenetration" }, // 2917886012
		{ &Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_ManaCostMultiplier, "OnRep_ManaCostMultiplier" }, // 2220872511
		{ &Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_SpellPowerMultiplier, "OnRep_SpellPowerMultiplier" }, // 1536967407
		{ &Z_Construct_UFunction_UDerivedStatsAttributeSet_OnRep_WeaponDamageMultiplier, "OnRep_WeaponDamageMultiplier" }, // 232062640
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UDerivedStatsAttributeSet>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UDerivedStatsAttributeSet_Statics

// ********** Begin Class UDerivedStatsAttributeSet Property Definitions ***************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_WeaponDamageMultiplier = { "WeaponDamageMultiplier", "OnRep_WeaponDamageMultiplier", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDerivedStatsAttributeSet, WeaponDamageMultiplier), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WeaponDamageMultiplier_MetaData), NewProp_WeaponDamageMultiplier_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_AttackSpeedMultiplier = { "AttackSpeedMultiplier", "OnRep_AttackSpeedMultiplier", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDerivedStatsAttributeSet, AttackSpeedMultiplier), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AttackSpeedMultiplier_MetaData), NewProp_AttackSpeedMultiplier_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_CritChance = { "CritChance", "OnRep_CritChance", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDerivedStatsAttributeSet, CritChance), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CritChance_MetaData), NewProp_CritChance_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_CritDamage = { "CritDamage", "OnRep_CritDamage", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDerivedStatsAttributeSet, CritDamage), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CritDamage_MetaData), NewProp_CritDamage_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_SpellPowerMultiplier = { "SpellPowerMultiplier", "OnRep_SpellPowerMultiplier", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDerivedStatsAttributeSet, SpellPowerMultiplier), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SpellPowerMultiplier_MetaData), NewProp_SpellPowerMultiplier_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_CastSpeedMultiplier = { "CastSpeedMultiplier", "OnRep_CastSpeedMultiplier", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDerivedStatsAttributeSet, CastSpeedMultiplier), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CastSpeedMultiplier_MetaData), NewProp_CastSpeedMultiplier_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_ManaCostMultiplier = { "ManaCostMultiplier", "OnRep_ManaCostMultiplier", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDerivedStatsAttributeSet, ManaCostMultiplier), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ManaCostMultiplier_MetaData), NewProp_ManaCostMultiplier_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_BreakPower = { "BreakPower", "OnRep_BreakPower", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDerivedStatsAttributeSet, BreakPower), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BreakPower_MetaData), NewProp_BreakPower_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_CastStability = { "CastStability", "OnRep_CastStability", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDerivedStatsAttributeSet, CastStability), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CastStability_MetaData), NewProp_CastStability_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_ArmorPenetration = { "ArmorPenetration", "OnRep_ArmorPenetration", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDerivedStatsAttributeSet, ArmorPenetration), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ArmorPenetration_MetaData), NewProp_ArmorPenetration_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_ElementPenetration = { "ElementPenetration", "OnRep_ElementPenetration", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDerivedStatsAttributeSet, ElementPenetration), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ElementPenetration_MetaData), NewProp_ElementPenetration_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_WeaponDamageMultiplier,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_AttackSpeedMultiplier,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_CritChance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_CritDamage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_SpellPowerMultiplier,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_CastSpeedMultiplier,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_ManaCostMultiplier,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_BreakPower,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_CastStability,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_ArmorPenetration,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::NewProp_ElementPenetration,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::PropPointers) < 2048);
// ********** End Class UDerivedStatsAttributeSet Property Definitions *****************************
UObject* (*const Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAttributeSet,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::ClassParams = {
	&UDerivedStatsAttributeSet::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::PropPointers),
	0,
	0x003000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::Class_MetaDataParams), Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::Class_MetaDataParams)
};
void UDerivedStatsAttributeSet::StaticRegisterNativesUDerivedStatsAttributeSet()
{
	UClass* Class = UDerivedStatsAttributeSet::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::Funcs));
}
UClass* Z_Construct_UClass_UDerivedStatsAttributeSet()
{
	if (!Z_Registration_Info_UClass_UDerivedStatsAttributeSet.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UDerivedStatsAttributeSet.OuterSingleton, Z_Construct_UClass_UDerivedStatsAttributeSet_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UDerivedStatsAttributeSet.OuterSingleton;
}
#if VALIDATE_CLASS_REPS
void UDerivedStatsAttributeSet::ValidateGeneratedRepEnums(const TArray<struct FRepRecord>& ClassReps) const
{
	static FName Name_WeaponDamageMultiplier(TEXT("WeaponDamageMultiplier"));
	static FName Name_AttackSpeedMultiplier(TEXT("AttackSpeedMultiplier"));
	static FName Name_CritChance(TEXT("CritChance"));
	static FName Name_CritDamage(TEXT("CritDamage"));
	static FName Name_SpellPowerMultiplier(TEXT("SpellPowerMultiplier"));
	static FName Name_CastSpeedMultiplier(TEXT("CastSpeedMultiplier"));
	static FName Name_ManaCostMultiplier(TEXT("ManaCostMultiplier"));
	static FName Name_BreakPower(TEXT("BreakPower"));
	static FName Name_CastStability(TEXT("CastStability"));
	static FName Name_ArmorPenetration(TEXT("ArmorPenetration"));
	static FName Name_ElementPenetration(TEXT("ElementPenetration"));
	const bool bIsValid = true
		&& Name_WeaponDamageMultiplier == ClassReps[(int32)ENetFields_Private::WeaponDamageMultiplier].Property->GetFName()
		&& Name_AttackSpeedMultiplier == ClassReps[(int32)ENetFields_Private::AttackSpeedMultiplier].Property->GetFName()
		&& Name_CritChance == ClassReps[(int32)ENetFields_Private::CritChance].Property->GetFName()
		&& Name_CritDamage == ClassReps[(int32)ENetFields_Private::CritDamage].Property->GetFName()
		&& Name_SpellPowerMultiplier == ClassReps[(int32)ENetFields_Private::SpellPowerMultiplier].Property->GetFName()
		&& Name_CastSpeedMultiplier == ClassReps[(int32)ENetFields_Private::CastSpeedMultiplier].Property->GetFName()
		&& Name_ManaCostMultiplier == ClassReps[(int32)ENetFields_Private::ManaCostMultiplier].Property->GetFName()
		&& Name_BreakPower == ClassReps[(int32)ENetFields_Private::BreakPower].Property->GetFName()
		&& Name_CastStability == ClassReps[(int32)ENetFields_Private::CastStability].Property->GetFName()
		&& Name_ArmorPenetration == ClassReps[(int32)ENetFields_Private::ArmorPenetration].Property->GetFName()
		&& Name_ElementPenetration == ClassReps[(int32)ENetFields_Private::ElementPenetration].Property->GetFName();
	checkf(bIsValid, TEXT("UHT Generated Rep Indices do not match runtime populated Rep Indices for properties in UDerivedStatsAttributeSet"));
}
#endif
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UDerivedStatsAttributeSet);
UDerivedStatsAttributeSet::~UDerivedStatsAttributeSet() {}
// ********** End Class UDerivedStatsAttributeSet **************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UDerivedStatsAttributeSet, UDerivedStatsAttributeSet::StaticClass, TEXT("UDerivedStatsAttributeSet"), &Z_Registration_Info_UClass_UDerivedStatsAttributeSet, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UDerivedStatsAttributeSet), 1786595718U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h__Script_SpellRise_2347433220{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
