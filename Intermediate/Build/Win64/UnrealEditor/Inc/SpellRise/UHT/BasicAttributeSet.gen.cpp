// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"
#include "AttributeSet.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeBasicAttributeSet() {}

// ********** Begin Cross Module References ********************************************************
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAttributeSet();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAttributeData();
SPELLRISE_API UClass* Z_Construct_UClass_UBasicAttributeSet();
SPELLRISE_API UClass* Z_Construct_UClass_UBasicAttributeSet_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UBasicAttributeSet Function OnRep_Armor **********************************
struct Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics
{
	struct BasicAttributeSet_eventOnRep_Armor_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_Armor constinit property declarations ***************************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_Armor constinit property declarations *****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_Armor Property Definitions **************************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(BasicAttributeSet_eventOnRep_Armor_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::PropPointers) < 2048);
// ********** End Function OnRep_Armor Property Definitions ****************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBasicAttributeSet, nullptr, "OnRep_Armor", 	Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::BasicAttributeSet_eventOnRep_Armor_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::BasicAttributeSet_eventOnRep_Armor_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBasicAttributeSet::execOnRep_Armor)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_Armor(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UBasicAttributeSet Function OnRep_Armor ************************************

// ********** Begin Class UBasicAttributeSet Function OnRep_CarryWeight ****************************
struct Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics
{
	struct BasicAttributeSet_eventOnRep_CarryWeight_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/* =======================\n\x09   OnRep\n\x09   ======================= */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "OnRep" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_CarryWeight constinit property declarations *********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_CarryWeight constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_CarryWeight Property Definitions ********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(BasicAttributeSet_eventOnRep_CarryWeight_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::PropPointers) < 2048);
// ********** End Function OnRep_CarryWeight Property Definitions **********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBasicAttributeSet, nullptr, "OnRep_CarryWeight", 	Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::BasicAttributeSet_eventOnRep_CarryWeight_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::BasicAttributeSet_eventOnRep_CarryWeight_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBasicAttributeSet::execOnRep_CarryWeight)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_CarryWeight(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UBasicAttributeSet Function OnRep_CarryWeight ******************************

// ********** Begin Class UBasicAttributeSet Function OnRep_ElementalResistance ********************
struct Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics
{
	struct BasicAttributeSet_eventOnRep_ElementalResistance_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_ElementalResistance constinit property declarations *************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_ElementalResistance constinit property declarations ***************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_ElementalResistance Property Definitions ************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(BasicAttributeSet_eventOnRep_ElementalResistance_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::PropPointers) < 2048);
// ********** End Function OnRep_ElementalResistance Property Definitions **************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBasicAttributeSet, nullptr, "OnRep_ElementalResistance", 	Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::BasicAttributeSet_eventOnRep_ElementalResistance_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::BasicAttributeSet_eventOnRep_ElementalResistance_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBasicAttributeSet::execOnRep_ElementalResistance)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_ElementalResistance(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UBasicAttributeSet Function OnRep_ElementalResistance **********************

// ********** Begin Class UBasicAttributeSet Function OnRep_FireResistance *************************
struct Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics
{
	struct BasicAttributeSet_eventOnRep_FireResistance_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_FireResistance constinit property declarations ******************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_FireResistance constinit property declarations ********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_FireResistance Property Definitions *****************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(BasicAttributeSet_eventOnRep_FireResistance_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::PropPointers) < 2048);
// ********** End Function OnRep_FireResistance Property Definitions *******************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBasicAttributeSet, nullptr, "OnRep_FireResistance", 	Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::BasicAttributeSet_eventOnRep_FireResistance_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00420401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::BasicAttributeSet_eventOnRep_FireResistance_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBasicAttributeSet::execOnRep_FireResistance)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_FireResistance(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UBasicAttributeSet Function OnRep_FireResistance ***************************

// ********** Begin Class UBasicAttributeSet Function OnRep_FrostResistance ************************
struct Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics
{
	struct BasicAttributeSet_eventOnRep_FrostResistance_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_FrostResistance constinit property declarations *****************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_FrostResistance constinit property declarations *******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_FrostResistance Property Definitions ****************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(BasicAttributeSet_eventOnRep_FrostResistance_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::PropPointers) < 2048);
// ********** End Function OnRep_FrostResistance Property Definitions ******************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBasicAttributeSet, nullptr, "OnRep_FrostResistance", 	Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::BasicAttributeSet_eventOnRep_FrostResistance_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00420401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::BasicAttributeSet_eventOnRep_FrostResistance_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBasicAttributeSet::execOnRep_FrostResistance)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_FrostResistance(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UBasicAttributeSet Function OnRep_FrostResistance **************************

// ********** Begin Class UBasicAttributeSet Function OnRep_LightningResistance ********************
struct Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics
{
	struct BasicAttributeSet_eventOnRep_LightningResistance_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_LightningResistance constinit property declarations *************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_LightningResistance constinit property declarations ***************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_LightningResistance Property Definitions ************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(BasicAttributeSet_eventOnRep_LightningResistance_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::PropPointers) < 2048);
// ********** End Function OnRep_LightningResistance Property Definitions **************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBasicAttributeSet, nullptr, "OnRep_LightningResistance", 	Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::BasicAttributeSet_eventOnRep_LightningResistance_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00420401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::BasicAttributeSet_eventOnRep_LightningResistance_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBasicAttributeSet::execOnRep_LightningResistance)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_LightningResistance(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UBasicAttributeSet Function OnRep_LightningResistance **********************

// ********** Begin Class UBasicAttributeSet Function OnRep_MagicResistance ************************
struct Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics
{
	struct BasicAttributeSet_eventOnRep_MagicResistance_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_MagicResistance constinit property declarations *****************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_MagicResistance constinit property declarations *******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_MagicResistance Property Definitions ****************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(BasicAttributeSet_eventOnRep_MagicResistance_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::PropPointers) < 2048);
// ********** End Function OnRep_MagicResistance Property Definitions ******************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBasicAttributeSet, nullptr, "OnRep_MagicResistance", 	Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::BasicAttributeSet_eventOnRep_MagicResistance_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::BasicAttributeSet_eventOnRep_MagicResistance_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBasicAttributeSet::execOnRep_MagicResistance)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_MagicResistance(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UBasicAttributeSet Function OnRep_MagicResistance **************************

// ********** Begin Class UBasicAttributeSet Function OnRep_PhysicalResistance *********************
struct Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics
{
	struct BasicAttributeSet_eventOnRep_PhysicalResistance_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_PhysicalResistance constinit property declarations **************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_PhysicalResistance constinit property declarations ****************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_PhysicalResistance Property Definitions *************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(BasicAttributeSet_eventOnRep_PhysicalResistance_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::PropPointers) < 2048);
// ********** End Function OnRep_PhysicalResistance Property Definitions ***************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UBasicAttributeSet, nullptr, "OnRep_PhysicalResistance", 	Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::BasicAttributeSet_eventOnRep_PhysicalResistance_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::Function_MetaDataParams), Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::BasicAttributeSet_eventOnRep_PhysicalResistance_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UBasicAttributeSet::execOnRep_PhysicalResistance)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_PhysicalResistance(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UBasicAttributeSet Function OnRep_PhysicalResistance ***********************

// ********** Begin Class UBasicAttributeSet *******************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UBasicAttributeSet;
UClass* UBasicAttributeSet::GetPrivateStaticClass()
{
	using TClass = UBasicAttributeSet;
	if (!Z_Registration_Info_UClass_UBasicAttributeSet.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("BasicAttributeSet"),
			Z_Registration_Info_UClass_UBasicAttributeSet.InnerSingleton,
			StaticRegisterNativesUBasicAttributeSet,
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
	return Z_Registration_Info_UClass_UBasicAttributeSet.InnerSingleton;
}
UClass* Z_Construct_UClass_UBasicAttributeSet_NoRegister()
{
	return UBasicAttributeSet::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UBasicAttributeSet_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CarryWeight_MetaData[] = {
		{ "Category", "Attributes|Inventory" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/* =======================\n\x09   INVENTORY / WEIGHT\n\x09   ======================= */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "INVENTORY / WEIGHT" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PhysicalResistance_MetaData[] = {
		{ "Category", "Attributes|Defense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/* =======================\n\x09   DEFENSE\n\x09   ======================= */// Resist\xc3\xaancias em % (0..75)\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "DEFENSE\n======================= // Resist\xc3\xaancias em % (0..75)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MagicResistance_MetaData[] = {
		{ "Category", "Attributes|Defense" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ElementalResistance_MetaData[] = {
		{ "Category", "Attributes|Defense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xe2\x9c\x85 NEW: Resist\xc3\xaancia elemental em % (0..75) para DamageType.Spell.Fire/Ice/Lightning\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xe2\x9c\x85 NEW: Resist\xc3\xaancia elemental em % (0..75) para DamageType.Spell.Fire/Ice/Lightning" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FireResistance_MetaData[] = {
		{ "Category", "Attributes|Defense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Resist\xc3\xaancias elementais espec\xc3\xad""ficas em % (0..75). Somam com ElementalResistance.\n// Usadas quando o spell tiver tags DamageElement.Fire/Frost/Lightning (veja ExecCalc_Damage).\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Resist\xc3\xaancias elementais espec\xc3\xad""ficas em % (0..75). Somam com ElementalResistance.\nUsadas quando o spell tiver tags DamageElement.Fire/Frost/Lightning (veja ExecCalc_Damage)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_FrostResistance_MetaData[] = {
		{ "Category", "Attributes|Defense" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LightningResistance_MetaData[] = {
		{ "Category", "Attributes|Defense" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Armor_MetaData[] = {
		{ "Category", "Attributes|Defense" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Armor bruto (diminishing returns no ExecCalc)\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Armor bruto (diminishing returns no ExecCalc)" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UBasicAttributeSet constinit property declarations ***********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_CarryWeight;
	static const UECodeGen_Private::FStructPropertyParams NewProp_PhysicalResistance;
	static const UECodeGen_Private::FStructPropertyParams NewProp_MagicResistance;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ElementalResistance;
	static const UECodeGen_Private::FStructPropertyParams NewProp_FireResistance;
	static const UECodeGen_Private::FStructPropertyParams NewProp_FrostResistance;
	static const UECodeGen_Private::FStructPropertyParams NewProp_LightningResistance;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Armor;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UBasicAttributeSet constinit property declarations *************************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("OnRep_Armor"), .Pointer = &UBasicAttributeSet::execOnRep_Armor },
		{ .NameUTF8 = UTF8TEXT("OnRep_CarryWeight"), .Pointer = &UBasicAttributeSet::execOnRep_CarryWeight },
		{ .NameUTF8 = UTF8TEXT("OnRep_ElementalResistance"), .Pointer = &UBasicAttributeSet::execOnRep_ElementalResistance },
		{ .NameUTF8 = UTF8TEXT("OnRep_FireResistance"), .Pointer = &UBasicAttributeSet::execOnRep_FireResistance },
		{ .NameUTF8 = UTF8TEXT("OnRep_FrostResistance"), .Pointer = &UBasicAttributeSet::execOnRep_FrostResistance },
		{ .NameUTF8 = UTF8TEXT("OnRep_LightningResistance"), .Pointer = &UBasicAttributeSet::execOnRep_LightningResistance },
		{ .NameUTF8 = UTF8TEXT("OnRep_MagicResistance"), .Pointer = &UBasicAttributeSet::execOnRep_MagicResistance },
		{ .NameUTF8 = UTF8TEXT("OnRep_PhysicalResistance"), .Pointer = &UBasicAttributeSet::execOnRep_PhysicalResistance },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UBasicAttributeSet_OnRep_Armor, "OnRep_Armor" }, // 4171444443
		{ &Z_Construct_UFunction_UBasicAttributeSet_OnRep_CarryWeight, "OnRep_CarryWeight" }, // 3971576947
		{ &Z_Construct_UFunction_UBasicAttributeSet_OnRep_ElementalResistance, "OnRep_ElementalResistance" }, // 1636873065
		{ &Z_Construct_UFunction_UBasicAttributeSet_OnRep_FireResistance, "OnRep_FireResistance" }, // 2821466562
		{ &Z_Construct_UFunction_UBasicAttributeSet_OnRep_FrostResistance, "OnRep_FrostResistance" }, // 3683265374
		{ &Z_Construct_UFunction_UBasicAttributeSet_OnRep_LightningResistance, "OnRep_LightningResistance" }, // 1312943980
		{ &Z_Construct_UFunction_UBasicAttributeSet_OnRep_MagicResistance, "OnRep_MagicResistance" }, // 1021996182
		{ &Z_Construct_UFunction_UBasicAttributeSet_OnRep_PhysicalResistance, "OnRep_PhysicalResistance" }, // 3891126179
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UBasicAttributeSet>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UBasicAttributeSet_Statics

// ********** Begin Class UBasicAttributeSet Property Definitions **********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_CarryWeight = { "CarryWeight", "OnRep_CarryWeight", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBasicAttributeSet, CarryWeight), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CarryWeight_MetaData), NewProp_CarryWeight_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_PhysicalResistance = { "PhysicalResistance", "OnRep_PhysicalResistance", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBasicAttributeSet, PhysicalResistance), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PhysicalResistance_MetaData), NewProp_PhysicalResistance_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_MagicResistance = { "MagicResistance", "OnRep_MagicResistance", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBasicAttributeSet, MagicResistance), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MagicResistance_MetaData), NewProp_MagicResistance_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_ElementalResistance = { "ElementalResistance", "OnRep_ElementalResistance", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBasicAttributeSet, ElementalResistance), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ElementalResistance_MetaData), NewProp_ElementalResistance_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_FireResistance = { "FireResistance", "OnRep_FireResistance", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBasicAttributeSet, FireResistance), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FireResistance_MetaData), NewProp_FireResistance_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_FrostResistance = { "FrostResistance", "OnRep_FrostResistance", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBasicAttributeSet, FrostResistance), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_FrostResistance_MetaData), NewProp_FrostResistance_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_LightningResistance = { "LightningResistance", "OnRep_LightningResistance", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBasicAttributeSet, LightningResistance), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LightningResistance_MetaData), NewProp_LightningResistance_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_Armor = { "Armor", "OnRep_Armor", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UBasicAttributeSet, Armor), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Armor_MetaData), NewProp_Armor_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UBasicAttributeSet_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_CarryWeight,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_PhysicalResistance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_MagicResistance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_ElementalResistance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_FireResistance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_FrostResistance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_LightningResistance,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UBasicAttributeSet_Statics::NewProp_Armor,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBasicAttributeSet_Statics::PropPointers) < 2048);
// ********** End Class UBasicAttributeSet Property Definitions ************************************
UObject* (*const Z_Construct_UClass_UBasicAttributeSet_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAttributeSet,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UBasicAttributeSet_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UBasicAttributeSet_Statics::ClassParams = {
	&UBasicAttributeSet::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UBasicAttributeSet_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UBasicAttributeSet_Statics::PropPointers),
	0,
	0x003000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UBasicAttributeSet_Statics::Class_MetaDataParams), Z_Construct_UClass_UBasicAttributeSet_Statics::Class_MetaDataParams)
};
void UBasicAttributeSet::StaticRegisterNativesUBasicAttributeSet()
{
	UClass* Class = UBasicAttributeSet::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UBasicAttributeSet_Statics::Funcs));
}
UClass* Z_Construct_UClass_UBasicAttributeSet()
{
	if (!Z_Registration_Info_UClass_UBasicAttributeSet.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UBasicAttributeSet.OuterSingleton, Z_Construct_UClass_UBasicAttributeSet_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UBasicAttributeSet.OuterSingleton;
}
#if VALIDATE_CLASS_REPS
void UBasicAttributeSet::ValidateGeneratedRepEnums(const TArray<struct FRepRecord>& ClassReps) const
{
	static FName Name_CarryWeight(TEXT("CarryWeight"));
	static FName Name_PhysicalResistance(TEXT("PhysicalResistance"));
	static FName Name_MagicResistance(TEXT("MagicResistance"));
	static FName Name_ElementalResistance(TEXT("ElementalResistance"));
	static FName Name_FireResistance(TEXT("FireResistance"));
	static FName Name_FrostResistance(TEXT("FrostResistance"));
	static FName Name_LightningResistance(TEXT("LightningResistance"));
	static FName Name_Armor(TEXT("Armor"));
	const bool bIsValid = true
		&& Name_CarryWeight == ClassReps[(int32)ENetFields_Private::CarryWeight].Property->GetFName()
		&& Name_PhysicalResistance == ClassReps[(int32)ENetFields_Private::PhysicalResistance].Property->GetFName()
		&& Name_MagicResistance == ClassReps[(int32)ENetFields_Private::MagicResistance].Property->GetFName()
		&& Name_ElementalResistance == ClassReps[(int32)ENetFields_Private::ElementalResistance].Property->GetFName()
		&& Name_FireResistance == ClassReps[(int32)ENetFields_Private::FireResistance].Property->GetFName()
		&& Name_FrostResistance == ClassReps[(int32)ENetFields_Private::FrostResistance].Property->GetFName()
		&& Name_LightningResistance == ClassReps[(int32)ENetFields_Private::LightningResistance].Property->GetFName()
		&& Name_Armor == ClassReps[(int32)ENetFields_Private::Armor].Property->GetFName();
	checkf(bIsValid, TEXT("UHT Generated Rep Indices do not match runtime populated Rep Indices for properties in UBasicAttributeSet"));
}
#endif
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UBasicAttributeSet);
UBasicAttributeSet::~UBasicAttributeSet() {}
// ********** End Class UBasicAttributeSet *********************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UBasicAttributeSet, UBasicAttributeSet::StaticClass, TEXT("UBasicAttributeSet"), &Z_Registration_Info_UClass_UBasicAttributeSet, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UBasicAttributeSet), 2494974568U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h__Script_SpellRise_48735261{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
