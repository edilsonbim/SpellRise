// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"
#include "AttributeSet.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeResourceAttributeSet() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UClass_NoRegister();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAttributeSet();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAttributeData();
SPELLRISE_API UClass* Z_Construct_UClass_UResourceAttributeSet();
SPELLRISE_API UClass* Z_Construct_UClass_UResourceAttributeSet_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UResourceAttributeSet Function OnRep_Health ******************************
struct Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics
{
	struct ResourceAttributeSet_eventOnRep_Health_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Rep notifies\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Rep notifies" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_Health constinit property declarations **************************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_Health constinit property declarations ****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_Health Property Definitions *************************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ResourceAttributeSet_eventOnRep_Health_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::PropPointers) < 2048);
// ********** End Function OnRep_Health Property Definitions ***************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UResourceAttributeSet, nullptr, "OnRep_Health", 	Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::ResourceAttributeSet_eventOnRep_Health_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::Function_MetaDataParams), Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::ResourceAttributeSet_eventOnRep_Health_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UResourceAttributeSet::execOnRep_Health)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_Health(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UResourceAttributeSet Function OnRep_Health ********************************

// ********** Begin Class UResourceAttributeSet Function OnRep_HealthRegen *************************
struct Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics
{
	struct ResourceAttributeSet_eventOnRep_HealthRegen_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_HealthRegen constinit property declarations *********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_HealthRegen constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_HealthRegen Property Definitions ********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ResourceAttributeSet_eventOnRep_HealthRegen_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::PropPointers) < 2048);
// ********** End Function OnRep_HealthRegen Property Definitions **********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UResourceAttributeSet, nullptr, "OnRep_HealthRegen", 	Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::ResourceAttributeSet_eventOnRep_HealthRegen_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::Function_MetaDataParams), Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::ResourceAttributeSet_eventOnRep_HealthRegen_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UResourceAttributeSet::execOnRep_HealthRegen)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_HealthRegen(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UResourceAttributeSet Function OnRep_HealthRegen ***************************

// ********** Begin Class UResourceAttributeSet Function OnRep_Mana ********************************
struct Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics
{
	struct ResourceAttributeSet_eventOnRep_Mana_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_Mana constinit property declarations ****************************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_Mana constinit property declarations ******************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_Mana Property Definitions ***************************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ResourceAttributeSet_eventOnRep_Mana_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::PropPointers) < 2048);
// ********** End Function OnRep_Mana Property Definitions *****************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UResourceAttributeSet, nullptr, "OnRep_Mana", 	Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::ResourceAttributeSet_eventOnRep_Mana_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::Function_MetaDataParams), Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::ResourceAttributeSet_eventOnRep_Mana_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UResourceAttributeSet::execOnRep_Mana)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_Mana(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UResourceAttributeSet Function OnRep_Mana **********************************

// ********** Begin Class UResourceAttributeSet Function OnRep_ManaRegen ***************************
struct Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics
{
	struct ResourceAttributeSet_eventOnRep_ManaRegen_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_ManaRegen constinit property declarations ***********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_ManaRegen constinit property declarations *************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_ManaRegen Property Definitions **********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ResourceAttributeSet_eventOnRep_ManaRegen_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::PropPointers) < 2048);
// ********** End Function OnRep_ManaRegen Property Definitions ************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UResourceAttributeSet, nullptr, "OnRep_ManaRegen", 	Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::ResourceAttributeSet_eventOnRep_ManaRegen_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::Function_MetaDataParams), Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::ResourceAttributeSet_eventOnRep_ManaRegen_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UResourceAttributeSet::execOnRep_ManaRegen)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_ManaRegen(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UResourceAttributeSet Function OnRep_ManaRegen *****************************

// ********** Begin Class UResourceAttributeSet Function OnRep_MaxHealth ***************************
struct Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics
{
	struct ResourceAttributeSet_eventOnRep_MaxHealth_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_MaxHealth constinit property declarations ***********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_MaxHealth constinit property declarations *************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_MaxHealth Property Definitions **********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ResourceAttributeSet_eventOnRep_MaxHealth_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::PropPointers) < 2048);
// ********** End Function OnRep_MaxHealth Property Definitions ************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UResourceAttributeSet, nullptr, "OnRep_MaxHealth", 	Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::ResourceAttributeSet_eventOnRep_MaxHealth_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::Function_MetaDataParams), Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::ResourceAttributeSet_eventOnRep_MaxHealth_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UResourceAttributeSet::execOnRep_MaxHealth)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_MaxHealth(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UResourceAttributeSet Function OnRep_MaxHealth *****************************

// ********** Begin Class UResourceAttributeSet Function OnRep_MaxMana *****************************
struct Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics
{
	struct ResourceAttributeSet_eventOnRep_MaxMana_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_MaxMana constinit property declarations *************************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_MaxMana constinit property declarations ***************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_MaxMana Property Definitions ************************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ResourceAttributeSet_eventOnRep_MaxMana_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::PropPointers) < 2048);
// ********** End Function OnRep_MaxMana Property Definitions **************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UResourceAttributeSet, nullptr, "OnRep_MaxMana", 	Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::ResourceAttributeSet_eventOnRep_MaxMana_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::Function_MetaDataParams), Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::ResourceAttributeSet_eventOnRep_MaxMana_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UResourceAttributeSet::execOnRep_MaxMana)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_MaxMana(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UResourceAttributeSet Function OnRep_MaxMana *******************************

// ********** Begin Class UResourceAttributeSet Function OnRep_MaxShield ***************************
struct Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics
{
	struct ResourceAttributeSet_eventOnRep_MaxShield_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_MaxShield constinit property declarations ***********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_MaxShield constinit property declarations *************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_MaxShield Property Definitions **********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ResourceAttributeSet_eventOnRep_MaxShield_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::PropPointers) < 2048);
// ********** End Function OnRep_MaxShield Property Definitions ************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UResourceAttributeSet, nullptr, "OnRep_MaxShield", 	Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::ResourceAttributeSet_eventOnRep_MaxShield_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::Function_MetaDataParams), Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::ResourceAttributeSet_eventOnRep_MaxShield_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UResourceAttributeSet::execOnRep_MaxShield)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_MaxShield(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UResourceAttributeSet Function OnRep_MaxShield *****************************

// ********** Begin Class UResourceAttributeSet Function OnRep_MaxStamina **************************
struct Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics
{
	struct ResourceAttributeSet_eventOnRep_MaxStamina_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_MaxStamina constinit property declarations **********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_MaxStamina constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_MaxStamina Property Definitions *********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ResourceAttributeSet_eventOnRep_MaxStamina_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::PropPointers) < 2048);
// ********** End Function OnRep_MaxStamina Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UResourceAttributeSet, nullptr, "OnRep_MaxStamina", 	Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::ResourceAttributeSet_eventOnRep_MaxStamina_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::Function_MetaDataParams), Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::ResourceAttributeSet_eventOnRep_MaxStamina_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UResourceAttributeSet::execOnRep_MaxStamina)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_MaxStamina(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UResourceAttributeSet Function OnRep_MaxStamina ****************************

// ********** Begin Class UResourceAttributeSet Function OnRep_Shield ******************************
struct Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics
{
	struct ResourceAttributeSet_eventOnRep_Shield_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_Shield constinit property declarations **************************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_Shield constinit property declarations ****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_Shield Property Definitions *************************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ResourceAttributeSet_eventOnRep_Shield_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::PropPointers) < 2048);
// ********** End Function OnRep_Shield Property Definitions ***************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UResourceAttributeSet, nullptr, "OnRep_Shield", 	Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::ResourceAttributeSet_eventOnRep_Shield_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::Function_MetaDataParams), Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::ResourceAttributeSet_eventOnRep_Shield_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UResourceAttributeSet::execOnRep_Shield)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_Shield(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UResourceAttributeSet Function OnRep_Shield ********************************

// ********** Begin Class UResourceAttributeSet Function OnRep_Stamina *****************************
struct Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics
{
	struct ResourceAttributeSet_eventOnRep_Stamina_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_Stamina constinit property declarations *************************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_Stamina constinit property declarations ***************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_Stamina Property Definitions ************************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ResourceAttributeSet_eventOnRep_Stamina_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::PropPointers) < 2048);
// ********** End Function OnRep_Stamina Property Definitions **************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UResourceAttributeSet, nullptr, "OnRep_Stamina", 	Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::ResourceAttributeSet_eventOnRep_Stamina_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::Function_MetaDataParams), Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::ResourceAttributeSet_eventOnRep_Stamina_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UResourceAttributeSet::execOnRep_Stamina)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_Stamina(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UResourceAttributeSet Function OnRep_Stamina *******************************

// ********** Begin Class UResourceAttributeSet Function OnRep_StaminaRegen ************************
struct Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics
{
	struct ResourceAttributeSet_eventOnRep_StaminaRegen_Parms
	{
		FGameplayAttributeData OldValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OldValue_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_StaminaRegen constinit property declarations ********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_OldValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnRep_StaminaRegen constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnRep_StaminaRegen Property Definitions *******************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::NewProp_OldValue = { "OldValue", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ResourceAttributeSet_eventOnRep_StaminaRegen_Parms, OldValue), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OldValue_MetaData), NewProp_OldValue_MetaData) }; // 3543875268
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::NewProp_OldValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::PropPointers) < 2048);
// ********** End Function OnRep_StaminaRegen Property Definitions *********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UResourceAttributeSet, nullptr, "OnRep_StaminaRegen", 	Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::ResourceAttributeSet_eventOnRep_StaminaRegen_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00480401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::Function_MetaDataParams), Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::ResourceAttributeSet_eventOnRep_StaminaRegen_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UResourceAttributeSet::execOnRep_StaminaRegen)
{
	P_GET_STRUCT_REF(FGameplayAttributeData,Z_Param_Out_OldValue);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_StaminaRegen(Z_Param_Out_OldValue);
	P_NATIVE_END;
}
// ********** End Class UResourceAttributeSet Function OnRep_StaminaRegen **************************

// ********** Begin Class UResourceAttributeSet ****************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UResourceAttributeSet;
UClass* UResourceAttributeSet::GetPrivateStaticClass()
{
	using TClass = UResourceAttributeSet;
	if (!Z_Registration_Info_UClass_UResourceAttributeSet.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("ResourceAttributeSet"),
			Z_Registration_Info_UClass_UResourceAttributeSet.InnerSingleton,
			StaticRegisterNativesUResourceAttributeSet,
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
	return Z_Registration_Info_UClass_UResourceAttributeSet.InnerSingleton;
}
UClass* Z_Construct_UClass_UResourceAttributeSet_NoRegister()
{
	return UResourceAttributeSet::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UResourceAttributeSet_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Health_MetaData[] = {
		{ "Category", "Attributes|Resources" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// -------------------------\n// Resources (replicated)\n// -------------------------\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Resources (replicated)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxHealth_MetaData[] = {
		{ "Category", "Attributes|Resources" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Mana_MetaData[] = {
		{ "Category", "Attributes|Resources" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxMana_MetaData[] = {
		{ "Category", "Attributes|Resources" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Stamina_MetaData[] = {
		{ "Category", "Attributes|Resources" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxStamina_MetaData[] = {
		{ "Category", "Attributes|Resources" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Shield_MetaData[] = {
		{ "Category", "Attributes|Resources" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Shield opcional\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Shield opcional" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxShield_MetaData[] = {
		{ "Category", "Attributes|Resources" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_HealthRegen_MetaData[] = {
		{ "Category", "Attributes|Regen" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// -------------------------\n// Regen (replicated)\n// - valores em \"por segundo\"\n// - o tick \xc3\xa9 feito por GameplayEffect peri\xc3\xb3""dico (n\xc3\xa3o aqui)\n// -------------------------\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Regen (replicated)\n- valores em \"por segundo\"\n- o tick \xc3\xa9 feito por GameplayEffect peri\xc3\xb3""dico (n\xc3\xa3o aqui)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ManaRegen_MetaData[] = {
		{ "Category", "Attributes|Regen" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_StaminaRegen_MetaData[] = {
		{ "Category", "Attributes|Regen" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Damage_MetaData[] = {
		{ "Category", "Attributes|Meta" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// -------------------------\n// Meta (NOT replicated)\n// -------------------------\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Meta (NOT replicated)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_Catalyst_AddCharge_MetaData[] = {
		{ "Category", "Catalyst" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// -------------------------\n// Catalyst (aplica charge via GE instant)\n// -------------------------\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Catalyst (aplica charge via GE instant)" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UResourceAttributeSet constinit property declarations ********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_Health;
	static const UECodeGen_Private::FStructPropertyParams NewProp_MaxHealth;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Mana;
	static const UECodeGen_Private::FStructPropertyParams NewProp_MaxMana;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Stamina;
	static const UECodeGen_Private::FStructPropertyParams NewProp_MaxStamina;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Shield;
	static const UECodeGen_Private::FStructPropertyParams NewProp_MaxShield;
	static const UECodeGen_Private::FStructPropertyParams NewProp_HealthRegen;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ManaRegen;
	static const UECodeGen_Private::FStructPropertyParams NewProp_StaminaRegen;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Damage;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_Catalyst_AddCharge;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UResourceAttributeSet constinit property declarations **********************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("OnRep_Health"), .Pointer = &UResourceAttributeSet::execOnRep_Health },
		{ .NameUTF8 = UTF8TEXT("OnRep_HealthRegen"), .Pointer = &UResourceAttributeSet::execOnRep_HealthRegen },
		{ .NameUTF8 = UTF8TEXT("OnRep_Mana"), .Pointer = &UResourceAttributeSet::execOnRep_Mana },
		{ .NameUTF8 = UTF8TEXT("OnRep_ManaRegen"), .Pointer = &UResourceAttributeSet::execOnRep_ManaRegen },
		{ .NameUTF8 = UTF8TEXT("OnRep_MaxHealth"), .Pointer = &UResourceAttributeSet::execOnRep_MaxHealth },
		{ .NameUTF8 = UTF8TEXT("OnRep_MaxMana"), .Pointer = &UResourceAttributeSet::execOnRep_MaxMana },
		{ .NameUTF8 = UTF8TEXT("OnRep_MaxShield"), .Pointer = &UResourceAttributeSet::execOnRep_MaxShield },
		{ .NameUTF8 = UTF8TEXT("OnRep_MaxStamina"), .Pointer = &UResourceAttributeSet::execOnRep_MaxStamina },
		{ .NameUTF8 = UTF8TEXT("OnRep_Shield"), .Pointer = &UResourceAttributeSet::execOnRep_Shield },
		{ .NameUTF8 = UTF8TEXT("OnRep_Stamina"), .Pointer = &UResourceAttributeSet::execOnRep_Stamina },
		{ .NameUTF8 = UTF8TEXT("OnRep_StaminaRegen"), .Pointer = &UResourceAttributeSet::execOnRep_StaminaRegen },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UResourceAttributeSet_OnRep_Health, "OnRep_Health" }, // 593068343
		{ &Z_Construct_UFunction_UResourceAttributeSet_OnRep_HealthRegen, "OnRep_HealthRegen" }, // 2165256073
		{ &Z_Construct_UFunction_UResourceAttributeSet_OnRep_Mana, "OnRep_Mana" }, // 3629799652
		{ &Z_Construct_UFunction_UResourceAttributeSet_OnRep_ManaRegen, "OnRep_ManaRegen" }, // 750242199
		{ &Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxHealth, "OnRep_MaxHealth" }, // 4177156231
		{ &Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxMana, "OnRep_MaxMana" }, // 350659188
		{ &Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxShield, "OnRep_MaxShield" }, // 2412495860
		{ &Z_Construct_UFunction_UResourceAttributeSet_OnRep_MaxStamina, "OnRep_MaxStamina" }, // 1001818492
		{ &Z_Construct_UFunction_UResourceAttributeSet_OnRep_Shield, "OnRep_Shield" }, // 2302105742
		{ &Z_Construct_UFunction_UResourceAttributeSet_OnRep_Stamina, "OnRep_Stamina" }, // 532230957
		{ &Z_Construct_UFunction_UResourceAttributeSet_OnRep_StaminaRegen, "OnRep_StaminaRegen" }, // 1146124498
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UResourceAttributeSet>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UResourceAttributeSet_Statics

// ********** Begin Class UResourceAttributeSet Property Definitions *******************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_Health = { "Health", "OnRep_Health", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, Health), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Health_MetaData), NewProp_Health_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_MaxHealth = { "MaxHealth", "OnRep_MaxHealth", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, MaxHealth), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxHealth_MetaData), NewProp_MaxHealth_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_Mana = { "Mana", "OnRep_Mana", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, Mana), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Mana_MetaData), NewProp_Mana_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_MaxMana = { "MaxMana", "OnRep_MaxMana", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, MaxMana), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxMana_MetaData), NewProp_MaxMana_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_Stamina = { "Stamina", "OnRep_Stamina", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, Stamina), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Stamina_MetaData), NewProp_Stamina_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_MaxStamina = { "MaxStamina", "OnRep_MaxStamina", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, MaxStamina), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxStamina_MetaData), NewProp_MaxStamina_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_Shield = { "Shield", "OnRep_Shield", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, Shield), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Shield_MetaData), NewProp_Shield_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_MaxShield = { "MaxShield", "OnRep_MaxShield", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, MaxShield), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxShield_MetaData), NewProp_MaxShield_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_HealthRegen = { "HealthRegen", "OnRep_HealthRegen", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, HealthRegen), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_HealthRegen_MetaData), NewProp_HealthRegen_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_ManaRegen = { "ManaRegen", "OnRep_ManaRegen", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, ManaRegen), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ManaRegen_MetaData), NewProp_ManaRegen_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_StaminaRegen = { "StaminaRegen", "OnRep_StaminaRegen", (EPropertyFlags)0x0010000100000034, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, StaminaRegen), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_StaminaRegen_MetaData), NewProp_StaminaRegen_MetaData) }; // 3543875268
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_Damage = { "Damage", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, Damage), Z_Construct_UScriptStruct_FGameplayAttributeData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Damage_MetaData), NewProp_Damage_MetaData) }; // 3543875268
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_GE_Catalyst_AddCharge = { "GE_Catalyst_AddCharge", nullptr, (EPropertyFlags)0x0014000000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UResourceAttributeSet, GE_Catalyst_AddCharge), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_Catalyst_AddCharge_MetaData), NewProp_GE_Catalyst_AddCharge_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UResourceAttributeSet_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_Health,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_MaxHealth,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_Mana,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_MaxMana,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_Stamina,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_MaxStamina,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_Shield,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_MaxShield,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_HealthRegen,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_ManaRegen,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_StaminaRegen,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_Damage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UResourceAttributeSet_Statics::NewProp_GE_Catalyst_AddCharge,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UResourceAttributeSet_Statics::PropPointers) < 2048);
// ********** End Class UResourceAttributeSet Property Definitions *********************************
UObject* (*const Z_Construct_UClass_UResourceAttributeSet_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAttributeSet,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UResourceAttributeSet_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UResourceAttributeSet_Statics::ClassParams = {
	&UResourceAttributeSet::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UResourceAttributeSet_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UResourceAttributeSet_Statics::PropPointers),
	0,
	0x003000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UResourceAttributeSet_Statics::Class_MetaDataParams), Z_Construct_UClass_UResourceAttributeSet_Statics::Class_MetaDataParams)
};
void UResourceAttributeSet::StaticRegisterNativesUResourceAttributeSet()
{
	UClass* Class = UResourceAttributeSet::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UResourceAttributeSet_Statics::Funcs));
}
UClass* Z_Construct_UClass_UResourceAttributeSet()
{
	if (!Z_Registration_Info_UClass_UResourceAttributeSet.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UResourceAttributeSet.OuterSingleton, Z_Construct_UClass_UResourceAttributeSet_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UResourceAttributeSet.OuterSingleton;
}
#if VALIDATE_CLASS_REPS
void UResourceAttributeSet::ValidateGeneratedRepEnums(const TArray<struct FRepRecord>& ClassReps) const
{
	static FName Name_Health(TEXT("Health"));
	static FName Name_MaxHealth(TEXT("MaxHealth"));
	static FName Name_Mana(TEXT("Mana"));
	static FName Name_MaxMana(TEXT("MaxMana"));
	static FName Name_Stamina(TEXT("Stamina"));
	static FName Name_MaxStamina(TEXT("MaxStamina"));
	static FName Name_Shield(TEXT("Shield"));
	static FName Name_MaxShield(TEXT("MaxShield"));
	static FName Name_HealthRegen(TEXT("HealthRegen"));
	static FName Name_ManaRegen(TEXT("ManaRegen"));
	static FName Name_StaminaRegen(TEXT("StaminaRegen"));
	const bool bIsValid = true
		&& Name_Health == ClassReps[(int32)ENetFields_Private::Health].Property->GetFName()
		&& Name_MaxHealth == ClassReps[(int32)ENetFields_Private::MaxHealth].Property->GetFName()
		&& Name_Mana == ClassReps[(int32)ENetFields_Private::Mana].Property->GetFName()
		&& Name_MaxMana == ClassReps[(int32)ENetFields_Private::MaxMana].Property->GetFName()
		&& Name_Stamina == ClassReps[(int32)ENetFields_Private::Stamina].Property->GetFName()
		&& Name_MaxStamina == ClassReps[(int32)ENetFields_Private::MaxStamina].Property->GetFName()
		&& Name_Shield == ClassReps[(int32)ENetFields_Private::Shield].Property->GetFName()
		&& Name_MaxShield == ClassReps[(int32)ENetFields_Private::MaxShield].Property->GetFName()
		&& Name_HealthRegen == ClassReps[(int32)ENetFields_Private::HealthRegen].Property->GetFName()
		&& Name_ManaRegen == ClassReps[(int32)ENetFields_Private::ManaRegen].Property->GetFName()
		&& Name_StaminaRegen == ClassReps[(int32)ENetFields_Private::StaminaRegen].Property->GetFName();
	checkf(bIsValid, TEXT("UHT Generated Rep Indices do not match runtime populated Rep Indices for properties in UResourceAttributeSet"));
}
#endif
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UResourceAttributeSet);
UResourceAttributeSet::~UResourceAttributeSet() {}
// ********** End Class UResourceAttributeSet ******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UResourceAttributeSet, UResourceAttributeSet::StaticClass, TEXT("UResourceAttributeSet"), &Z_Registration_Info_UClass_UResourceAttributeSet, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UResourceAttributeSet), 4238454355U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h__Script_SpellRise_722982658{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
