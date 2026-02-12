// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h"
#include "AttributeSet.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeCombatAttributeSet() {}

// ********** Begin Cross Module References ********************************************************
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAttributeSet();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAttributeData();
SPELLRISE_API UClass* Z_Construct_UClass_UCombatAttributeSet();
SPELLRISE_API UClass* Z_Construct_UClass_UCombatAttributeSet_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UCombatAttributeSet Function OnRep_Agility *******************************
struct Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics
{
	struct CombatAttributeSet_eventOnRep_Agility_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_Agility constinit property declarations *************************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_Agility constinit property declarations ***************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_Agility Property Definitions ************************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CombatAttributeSet_eventOnRep_Agility_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::PropPointers) < 2048);
// ********** End Function OnRep_Agility Property Definitions **************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCombatAttributeSet, nullptr, "OnRep_Agility", 	Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::CombatAttributeSet_eventOnRep_Agility_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::CombatAttributeSet_eventOnRep_Agility_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCombatAttributeSet::execOnRep_Agility)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_Agility(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCombatAttributeSet Function OnRep_Agility *********************************

// ********** Begin Class UCombatAttributeSet Function OnRep_Attunement ****************************
struct Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics
{
	struct CombatAttributeSet_eventOnRep_Attunement_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_Attunement constinit property declarations **********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_Attunement constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_Attunement Property Definitions *********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CombatAttributeSet_eventOnRep_Attunement_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::PropPointers) < 2048);
// ********** End Function OnRep_Attunement Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCombatAttributeSet, nullptr, "OnRep_Attunement", 	Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::CombatAttributeSet_eventOnRep_Attunement_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::CombatAttributeSet_eventOnRep_Attunement_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCombatAttributeSet::execOnRep_Attunement)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_Attunement(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCombatAttributeSet Function OnRep_Attunement ******************************

// ********** Begin Class UCombatAttributeSet Function OnRep_BreakPower ****************************
struct Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics
{
	struct CombatAttributeSet_eventOnRep_BreakPower_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
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
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CombatAttributeSet_eventOnRep_BreakPower_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::PropPointers) < 2048);
// ********** End Function OnRep_BreakPower Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCombatAttributeSet, nullptr, "OnRep_BreakPower", 	Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::CombatAttributeSet_eventOnRep_BreakPower_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::CombatAttributeSet_eventOnRep_BreakPower_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCombatAttributeSet::execOnRep_BreakPower)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_BreakPower(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCombatAttributeSet Function OnRep_BreakPower ******************************

// ********** Begin Class UCombatAttributeSet Function OnRep_CastSpeed *****************************
struct Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics
{
	struct CombatAttributeSet_eventOnRep_CastSpeed_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_CastSpeed constinit property declarations ***********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_CastSpeed constinit property declarations *************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_CastSpeed Property Definitions **********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CombatAttributeSet_eventOnRep_CastSpeed_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::PropPointers) < 2048);
// ********** End Function OnRep_CastSpeed Property Definitions ************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCombatAttributeSet, nullptr, "OnRep_CastSpeed", 	Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::CombatAttributeSet_eventOnRep_CastSpeed_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::CombatAttributeSet_eventOnRep_CastSpeed_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCombatAttributeSet::execOnRep_CastSpeed)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_CastSpeed(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCombatAttributeSet Function OnRep_CastSpeed *******************************

// ********** Begin Class UCombatAttributeSet Function OnRep_CastStability *************************
struct Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics
{
	struct CombatAttributeSet_eventOnRep_CastStability_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
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
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CombatAttributeSet_eventOnRep_CastStability_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::PropPointers) < 2048);
// ********** End Function OnRep_CastStability Property Definitions ********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCombatAttributeSet, nullptr, "OnRep_CastStability", 	Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::CombatAttributeSet_eventOnRep_CastStability_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::CombatAttributeSet_eventOnRep_CastStability_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCombatAttributeSet::execOnRep_CastStability)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_CastStability(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCombatAttributeSet Function OnRep_CastStability ***************************

// ********** Begin Class UCombatAttributeSet Function OnRep_Focus *********************************
struct Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics
{
	struct CombatAttributeSet_eventOnRep_Focus_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_Focus constinit property declarations ***************************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_Focus constinit property declarations *****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_Focus Property Definitions **************************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CombatAttributeSet_eventOnRep_Focus_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::PropPointers) < 2048);
// ********** End Function OnRep_Focus Property Definitions ****************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCombatAttributeSet, nullptr, "OnRep_Focus", 	Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::CombatAttributeSet_eventOnRep_Focus_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::CombatAttributeSet_eventOnRep_Focus_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCombatAttributeSet::execOnRep_Focus)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_Focus(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCombatAttributeSet Function OnRep_Focus ***********************************

// ********** Begin Class UCombatAttributeSet Function OnRep_MoveSpeed *****************************
struct Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics
{
	struct CombatAttributeSet_eventOnRep_MoveSpeed_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_MoveSpeed constinit property declarations ***********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_MoveSpeed constinit property declarations *************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_MoveSpeed Property Definitions **********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CombatAttributeSet_eventOnRep_MoveSpeed_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::PropPointers) < 2048);
// ********** End Function OnRep_MoveSpeed Property Definitions ************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCombatAttributeSet, nullptr, "OnRep_MoveSpeed", 	Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::CombatAttributeSet_eventOnRep_MoveSpeed_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::CombatAttributeSet_eventOnRep_MoveSpeed_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCombatAttributeSet::execOnRep_MoveSpeed)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_MoveSpeed(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCombatAttributeSet Function OnRep_MoveSpeed *******************************

// ********** Begin Class UCombatAttributeSet Function OnRep_MoveSpeedMultiplier *******************
struct Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics
{
	struct CombatAttributeSet_eventOnRep_MoveSpeedMultiplier_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_MoveSpeedMultiplier constinit property declarations *************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_MoveSpeedMultiplier constinit property declarations ***************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_MoveSpeedMultiplier Property Definitions ************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CombatAttributeSet_eventOnRep_MoveSpeedMultiplier_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::PropPointers) < 2048);
// ********** End Function OnRep_MoveSpeedMultiplier Property Definitions **************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCombatAttributeSet, nullptr, "OnRep_MoveSpeedMultiplier", 	Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::CombatAttributeSet_eventOnRep_MoveSpeedMultiplier_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::CombatAttributeSet_eventOnRep_MoveSpeedMultiplier_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCombatAttributeSet::execOnRep_MoveSpeedMultiplier)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_MoveSpeedMultiplier(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCombatAttributeSet Function OnRep_MoveSpeedMultiplier *********************

// ********** Begin Class UCombatAttributeSet Function OnRep_SpellPowerBonusPct ********************
struct Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics
{
	struct CombatAttributeSet_eventOnRep_SpellPowerBonusPct_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_SpellPowerBonusPct constinit property declarations **************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_SpellPowerBonusPct constinit property declarations ****************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_SpellPowerBonusPct Property Definitions *************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CombatAttributeSet_eventOnRep_SpellPowerBonusPct_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::PropPointers) < 2048);
// ********** End Function OnRep_SpellPowerBonusPct Property Definitions ***************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCombatAttributeSet, nullptr, "OnRep_SpellPowerBonusPct", 	Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::CombatAttributeSet_eventOnRep_SpellPowerBonusPct_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::CombatAttributeSet_eventOnRep_SpellPowerBonusPct_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCombatAttributeSet::execOnRep_SpellPowerBonusPct)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_SpellPowerBonusPct(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCombatAttributeSet Function OnRep_SpellPowerBonusPct **********************

// ********** Begin Class UCombatAttributeSet Function OnRep_Vigor *********************************
struct Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics
{
	struct CombatAttributeSet_eventOnRep_Vigor_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Replication callbacks\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Replication callbacks" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_Vigor constinit property declarations ***************************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_Vigor constinit property declarations *****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_Vigor Property Definitions **************************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CombatAttributeSet_eventOnRep_Vigor_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::PropPointers) < 2048);
// ********** End Function OnRep_Vigor Property Definitions ****************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCombatAttributeSet, nullptr, "OnRep_Vigor", 	Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::CombatAttributeSet_eventOnRep_Vigor_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::CombatAttributeSet_eventOnRep_Vigor_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCombatAttributeSet::execOnRep_Vigor)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_Vigor(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCombatAttributeSet Function OnRep_Vigor ***********************************

// ********** Begin Class UCombatAttributeSet Function OnRep_Willpower *****************************
struct Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics
{
	struct CombatAttributeSet_eventOnRep_Willpower_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_Willpower constinit property declarations ***********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_Willpower constinit property declarations *************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_Willpower Property Definitions **********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(CombatAttributeSet_eventOnRep_Willpower_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::PropPointers) < 2048);
// ********** End Function OnRep_Willpower Property Definitions ************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UCombatAttributeSet, nullptr, "OnRep_Willpower", 	Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::CombatAttributeSet_eventOnRep_Willpower_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::Function_MetaDataParams), Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::CombatAttributeSet_eventOnRep_Willpower_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UCombatAttributeSet::execOnRep_Willpower)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_Willpower(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UCombatAttributeSet Function OnRep_Willpower *******************************

// ********** Begin Class UCombatAttributeSet ******************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UCombatAttributeSet;
UClass* UCombatAttributeSet::GetPrivateStaticClass()
{
	using TClass = UCombatAttributeSet;
	if (!Z_Registration_Info_UClass_UCombatAttributeSet.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("CombatAttributeSet"),
			Z_Registration_Info_UClass_UCombatAttributeSet.InnerSingleton,
			StaticRegisterNativesUCombatAttributeSet,
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
	return Z_Registration_Info_UClass_UCombatAttributeSet.InnerSingleton;
}
UClass* Z_Construct_UClass_UCombatAttributeSet_NoRegister()
{
	return UCombatAttributeSet::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UCombatAttributeSet_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Primary / combat attributes for SpellRise.\n *\n * Primaries (clamped 10..60):\n *   - Vigor, Focus, Agility, Willpower, Attunement\n *\n * Secondary bonuses (gear/buffs/catalyst):\n *   - CastSpeed      (% faster/slower cast)\n *   - CastStability  (bonus stability; reduces interrupt pressure)\n *   - BreakPower     (bonus break; increases interrupt pressure)\n *   - SpellPowerBonusPct     (bonus spell damage scaling - additive)\n *   - MoveSpeed      (bonus walk speed in uu/s; applied by character code)\n *   - MoveSpeedMultiplier (multiplier for final walk speed; 1.0 = normal)\n *\n * DPS/Derived stats were moved to UDerivedStatsAttributeSet.\n */" },
#endif
		{ "IncludePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Primary / combat attributes for SpellRise.\n\nPrimaries (clamped 10..60):\n  - Vigor, Focus, Agility, Willpower, Attunement\n\nSecondary bonuses (gear/buffs/catalyst):\n  - CastSpeed      (% faster/slower cast)\n  - CastStability  (bonus stability; reduces interrupt pressure)\n  - BreakPower     (bonus break; increases interrupt pressure)\n  - SpellPowerBonusPct     (bonus spell damage scaling - additive)\n  - MoveSpeed      (bonus walk speed in uu/s; applied by character code)\n  - MoveSpeedMultiplier (multiplier for final walk speed; 1.0 = normal)\n\nDPS/Derived stats were moved to UDerivedStatsAttributeSet." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Vigor_MetaData[] = {
		{ "Category", "Attributes|Primary" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// -------------------------\n// Primaries (replicated)\n// -------------------------\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Primaries (replicated)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Focus_MetaData[] = {
		{ "Category", "Attributes|Primary" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Agility_MetaData[] = {
		{ "Category", "Attributes|Primary" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Willpower_MetaData[] = {
		{ "Category", "Attributes|Primary" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Attunement_MetaData[] = {
		{ "Category", "Attributes|Primary" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CastSpeed_MetaData[] = {
		{ "Category", "Attributes|Casting" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Percent. Example: 0 = normal, 25 = 25% faster, -20 = 20% slower. */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Percent. Example: 0 = normal, 25 = 25% faster, -20 = 20% slower." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CastStability_MetaData[] = {
		{ "Category", "Attributes|Casting" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** BONUS stability (additive). Used by abilities to reduce interrupt/cast break. */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "BONUS stability (additive). Used by abilities to reduce interrupt/cast break." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BreakPower_MetaData[] = {
		{ "Category", "Attributes|Combat" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** BONUS break power (additive). Used by damage system to increase interrupt pressure. */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "BONUS break power (additive). Used by damage system to increase interrupt pressure." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SpellPowerBonusPct_MetaData[] = {
		{ "Category", "Attributes|Combat" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** BONUS spell power (additive). Used by spell damage calc. */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "BONUS spell power (additive). Used by spell damage calc." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MoveSpeed_MetaData[] = {
		{ "Category", "Attributes|Utility" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n\x09 * BONUS move speed in uu/s (additive to MaxWalkSpeed).\n\x09 * Example: +50 means +50 uu/s.\n\x09 */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "BONUS move speed in uu/s (additive to MaxWalkSpeed).\nExample: +50 means +50 uu/s." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MoveSpeedMultiplier_MetaData[] = {
		{ "Category", "Attributes|Utility" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n\x09 * Multiplier for final movement speed.\n\x09 * 1.00 = normal, 1.15 = +15%, 0.90 = -10%.\n\x09 */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/CombatAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Multiplier for final movement speed.\n1.00 = normal, 1.15 = +15%, 0.90 = -10%." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UCombatAttributeSet constinit property declarations **********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_Vigor;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Focus;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Agility;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Willpower;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Attunement;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CastSpeed;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CastStability;
	static const UECodeGen_Private::FStructPropertyParams NewProp_BreakPower;
	static const UECodeGen_Private::FStructPropertyParams NewProp_SpellPowerBonusPct;
	static const UECodeGen_Private::FStructPropertyParams NewProp_MoveSpeed;
	static const UECodeGen_Private::FStructPropertyParams NewProp_MoveSpeedMultiplier;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UCombatAttributeSet constinit property declarations ************************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("OnRep_Agility"), .Pointer = &UCombatAttributeSet::execOnRep_Agility },
		{ .NameUTF8 = UTF8TEXT("OnRep_Attunement"), .Pointer = &UCombatAttributeSet::execOnRep_Attunement },
		{ .NameUTF8 = UTF8TEXT("OnRep_BreakPower"), .Pointer = &UCombatAttributeSet::execOnRep_BreakPower },
		{ .NameUTF8 = UTF8TEXT("OnRep_CastSpeed"), .Pointer = &UCombatAttributeSet::execOnRep_CastSpeed },
		{ .NameUTF8 = UTF8TEXT("OnRep_CastStability"), .Pointer = &UCombatAttributeSet::execOnRep_CastStability },
		{ .NameUTF8 = UTF8TEXT("OnRep_Focus"), .Pointer = &UCombatAttributeSet::execOnRep_Focus },
		{ .NameUTF8 = UTF8TEXT("OnRep_MoveSpeed"), .Pointer = &UCombatAttributeSet::execOnRep_MoveSpeed },
		{ .NameUTF8 = UTF8TEXT("OnRep_MoveSpeedMultiplier"), .Pointer = &UCombatAttributeSet::execOnRep_MoveSpeedMultiplier },
		{ .NameUTF8 = UTF8TEXT("OnRep_SpellPowerBonusPct"), .Pointer = &UCombatAttributeSet::execOnRep_SpellPowerBonusPct },
		{ .NameUTF8 = UTF8TEXT("OnRep_Vigor"), .Pointer = &UCombatAttributeSet::execOnRep_Vigor },
		{ .NameUTF8 = UTF8TEXT("OnRep_Willpower"), .Pointer = &UCombatAttributeSet::execOnRep_Willpower },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UCombatAttributeSet_OnRep_Agility, "OnRep_Agility" }, // 4266511782
		{ &Z_Construct_UFunction_UCombatAttributeSet_OnRep_Attunement, "OnRep_Attunement" }, // 2532495755
		{ &Z_Construct_UFunction_UCombatAttributeSet_OnRep_BreakPower, "OnRep_BreakPower" }, // 285001209
		{ &Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastSpeed, "OnRep_CastSpeed" }, // 2271723877
		{ &Z_Construct_UFunction_UCombatAttributeSet_OnRep_CastStability, "OnRep_CastStability" }, // 1356407066
		{ &Z_Construct_UFunction_UCombatAttributeSet_OnRep_Focus, "OnRep_Focus" }, // 3620205308
		{ &Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeed, "OnRep_MoveSpeed" }, // 3523614195
		{ &Z_Construct_UFunction_UCombatAttributeSet_OnRep_MoveSpeedMultiplier, "OnRep_MoveSpeedMultiplier" }, // 3110841904
		{ &Z_Construct_UFunction_UCombatAttributeSet_OnRep_SpellPowerBonusPct, "OnRep_SpellPowerBonusPct" }, // 826299178
		{ &Z_Construct_UFunction_UCombatAttributeSet_OnRep_Vigor, "OnRep_Vigor" }, // 4147446365
		{ &Z_Construct_UFunction_UCombatAttributeSet_OnRep_Willpower, "OnRep_Willpower" }, // 1353565308
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCombatAttributeSet>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UCombatAttributeSet_Statics

// ********** Begin Class UCombatAttributeSet Property Definitions *********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_Vigor = { "Vigor", "OnRep_Vigor", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCombatAttributeSet, Vigor), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Vigor_MetaData), NewProp_Vigor_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_Focus = { "Focus", "OnRep_Focus", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCombatAttributeSet, Focus), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Focus_MetaData), NewProp_Focus_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_Agility = { "Agility", "OnRep_Agility", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCombatAttributeSet, Agility), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Agility_MetaData), NewProp_Agility_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_Willpower = { "Willpower", "OnRep_Willpower", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCombatAttributeSet, Willpower), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Willpower_MetaData), NewProp_Willpower_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_Attunement = { "Attunement", "OnRep_Attunement", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCombatAttributeSet, Attunement), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Attunement_MetaData), NewProp_Attunement_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_CastSpeed = { "CastSpeed", "OnRep_CastSpeed", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCombatAttributeSet, CastSpeed), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CastSpeed_MetaData), NewProp_CastSpeed_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_CastStability = { "CastStability", "OnRep_CastStability", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCombatAttributeSet, CastStability), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CastStability_MetaData), NewProp_CastStability_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_BreakPower = { "BreakPower", "OnRep_BreakPower", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCombatAttributeSet, BreakPower), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BreakPower_MetaData), NewProp_BreakPower_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_SpellPowerBonusPct = { "SpellPowerBonusPct", "OnRep_SpellPowerBonusPct", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCombatAttributeSet, SpellPowerBonusPct), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SpellPowerBonusPct_MetaData), NewProp_SpellPowerBonusPct_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_MoveSpeed = { "MoveSpeed", "OnRep_MoveSpeed", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCombatAttributeSet, MoveSpeed), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MoveSpeed_MetaData), NewProp_MoveSpeed_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_MoveSpeedMultiplier = { "MoveSpeedMultiplier", "OnRep_MoveSpeedMultiplier", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCombatAttributeSet, MoveSpeedMultiplier), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MoveSpeedMultiplier_MetaData), NewProp_MoveSpeedMultiplier_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UCombatAttributeSet_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_Vigor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_Focus,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_Agility,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_Willpower,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_Attunement,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_CastSpeed,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_CastStability,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_BreakPower,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_SpellPowerBonusPct,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_MoveSpeed,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCombatAttributeSet_Statics::NewProp_MoveSpeedMultiplier,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCombatAttributeSet_Statics::PropPointers) < 2048);
// ********** End Class UCombatAttributeSet Property Definitions ***********************************
UObject* (*const Z_Construct_UClass_UCombatAttributeSet_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAttributeSet,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCombatAttributeSet_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCombatAttributeSet_Statics::ClassParams = {
	&UCombatAttributeSet::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UCombatAttributeSet_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UCombatAttributeSet_Statics::PropPointers),
	0,
	0x003000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCombatAttributeSet_Statics::Class_MetaDataParams), Z_Construct_UClass_UCombatAttributeSet_Statics::Class_MetaDataParams)
};
void UCombatAttributeSet::StaticRegisterNativesUCombatAttributeSet()
{
	UClass* Class = UCombatAttributeSet::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UCombatAttributeSet_Statics::Funcs));
}
UClass* Z_Construct_UClass_UCombatAttributeSet()
{
	if (!Z_Registration_Info_UClass_UCombatAttributeSet.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCombatAttributeSet.OuterSingleton, Z_Construct_UClass_UCombatAttributeSet_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCombatAttributeSet.OuterSingleton;
}
#if VALIDATE_CLASS_REPS
void UCombatAttributeSet::ValidateGeneratedRepEnums(const TArray<struct FRepRecord>& ClassReps) const
{
	static FName Name_Vigor(TEXT("Vigor"));
	static FName Name_Focus(TEXT("Focus"));
	static FName Name_Agility(TEXT("Agility"));
	static FName Name_Willpower(TEXT("Willpower"));
	static FName Name_Attunement(TEXT("Attunement"));
	static FName Name_CastSpeed(TEXT("CastSpeed"));
	static FName Name_CastStability(TEXT("CastStability"));
	static FName Name_BreakPower(TEXT("BreakPower"));
	static FName Name_SpellPowerBonusPct(TEXT("SpellPowerBonusPct"));
	static FName Name_MoveSpeed(TEXT("MoveSpeed"));
	static FName Name_MoveSpeedMultiplier(TEXT("MoveSpeedMultiplier"));
	const bool bIsValid = true
		&& Name_Vigor == ClassReps[(int32)ENetFields_Private::Vigor].Property->GetFName()
		&& Name_Focus == ClassReps[(int32)ENetFields_Private::Focus].Property->GetFName()
		&& Name_Agility == ClassReps[(int32)ENetFields_Private::Agility].Property->GetFName()
		&& Name_Willpower == ClassReps[(int32)ENetFields_Private::Willpower].Property->GetFName()
		&& Name_Attunement == ClassReps[(int32)ENetFields_Private::Attunement].Property->GetFName()
		&& Name_CastSpeed == ClassReps[(int32)ENetFields_Private::CastSpeed].Property->GetFName()
		&& Name_CastStability == ClassReps[(int32)ENetFields_Private::CastStability].Property->GetFName()
		&& Name_BreakPower == ClassReps[(int32)ENetFields_Private::BreakPower].Property->GetFName()
		&& Name_SpellPowerBonusPct == ClassReps[(int32)ENetFields_Private::SpellPowerBonusPct].Property->GetFName()
		&& Name_MoveSpeed == ClassReps[(int32)ENetFields_Private::MoveSpeed].Property->GetFName()
		&& Name_MoveSpeedMultiplier == ClassReps[(int32)ENetFields_Private::MoveSpeedMultiplier].Property->GetFName();
	checkf(bIsValid, TEXT("UHT Generated Rep Indices do not match runtime populated Rep Indices for properties in UCombatAttributeSet"));
}
#endif
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UCombatAttributeSet);
UCombatAttributeSet::~UCombatAttributeSet() {}
// ********** End Class UCombatAttributeSet ********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CombatAttributeSet_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCombatAttributeSet, UCombatAttributeSet::StaticClass, TEXT("UCombatAttributeSet"), &Z_Registration_Info_UClass_UCombatAttributeSet, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCombatAttributeSet), 2996240886U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CombatAttributeSet_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CombatAttributeSet_h__Script_SpellRise_727408723{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CombatAttributeSet_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CombatAttributeSet_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
