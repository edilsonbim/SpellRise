// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Engine/NetSerialization.h"
#include "GameplayTagContainer.h"
#include "UObject/CoreNet.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeGA_Weapon_MeleeLight_ClientPrediction() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UClass_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UAnimMontage_NoRegister();
ENGINE_API UEnum* Z_Construct_UEnum_Engine_ECollisionChannel();
ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FVector_NetQuantize();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_PlayMontageAndWait_NoRegister();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_WaitGameplayEvent_NoRegister();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayEventData();
GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
SPELLRISE_API UClass* Z_Construct_UClass_UDA_WeaponDefinition_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction();
SPELLRISE_API UClass* Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGameplayAbility();
SPELLRISE_API UScriptStruct* Z_Construct_UScriptStruct_FCombatHitResult();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin ScriptStruct FCombatHitResult **************************************************
struct Z_Construct_UScriptStruct_FCombatHitResult_Statics
{
	static inline consteval int32 GetStructSize() { return sizeof(FCombatHitResult); }
	static inline consteval int16 GetStructAlignment() { return alignof(FCombatHitResult); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TargetActor_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DamageAmount_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bWasCrit_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_HitLocation_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_HitIndex_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FCombatHitResult constinit property declarations ******************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_TargetActor;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_DamageAmount;
	static void NewProp_bWasCrit_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bWasCrit;
	static const UECodeGen_Private::FStructPropertyParams NewProp_HitLocation;
	static const UECodeGen_Private::FBytePropertyParams NewProp_HitIndex;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FCombatHitResult constinit property declarations ********************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FCombatHitResult>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct Z_Construct_UScriptStruct_FCombatHitResult_Statics
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FCombatHitResult;
class UScriptStruct* FCombatHitResult::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FCombatHitResult.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FCombatHitResult.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FCombatHitResult, (UObject*)Z_Construct_UPackage__Script_SpellRise(), TEXT("CombatHitResult"));
	}
	return Z_Registration_Info_UScriptStruct_FCombatHitResult.OuterSingleton;
	}

// ********** Begin ScriptStruct FCombatHitResult Property Definitions *****************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_TargetActor = { "TargetActor", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCombatHitResult, TargetActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TargetActor_MetaData), NewProp_TargetActor_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_DamageAmount = { "DamageAmount", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCombatHitResult, DamageAmount), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DamageAmount_MetaData), NewProp_DamageAmount_MetaData) };
void Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_bWasCrit_SetBit(void* Obj)
{
	((FCombatHitResult*)Obj)->bWasCrit = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_bWasCrit = { "bWasCrit", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(FCombatHitResult), &Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_bWasCrit_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bWasCrit_MetaData), NewProp_bWasCrit_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_HitLocation = { "HitLocation", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCombatHitResult, HitLocation), Z_Construct_UScriptStruct_FVector_NetQuantize, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_HitLocation_MetaData), NewProp_HitLocation_MetaData) }; // 2388472055
const UECodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_HitIndex = { "HitIndex", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FCombatHitResult, HitIndex), nullptr, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_HitIndex_MetaData), NewProp_HitIndex_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FCombatHitResult_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_TargetActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_DamageAmount,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_bWasCrit,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_HitLocation,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewProp_HitIndex,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCombatHitResult_Statics::PropPointers) < 2048);
// ********** End ScriptStruct FCombatHitResult Property Definitions *******************************
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FCombatHitResult_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
	nullptr,
	&NewStructOps,
	"CombatHitResult",
	Z_Construct_UScriptStruct_FCombatHitResult_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCombatHitResult_Statics::PropPointers),
	sizeof(FCombatHitResult),
	alignof(FCombatHitResult),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FCombatHitResult_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FCombatHitResult_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FCombatHitResult()
{
	if (!Z_Registration_Info_UScriptStruct_FCombatHitResult.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FCombatHitResult.InnerSingleton, Z_Construct_UScriptStruct_FCombatHitResult_Statics::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FCombatHitResult.InnerSingleton);
}
// ********** End ScriptStruct FCombatHitResult ****************************************************

// ********** Begin Class UGA_Weapon_MeleeLight_ClientPrediction Function Client_ConfirmDamage *****
struct GA_Weapon_MeleeLight_ClientPrediction_eventClient_ConfirmDamage_Parms
{
	FCombatHitResult HitResult;
};
static FName NAME_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage = FName(TEXT("Client_ConfirmDamage"));
void UGA_Weapon_MeleeLight_ClientPrediction::Client_ConfirmDamage(FCombatHitResult const& HitResult)
{
	GA_Weapon_MeleeLight_ClientPrediction_eventClient_ConfirmDamage_Parms Parms;
	Parms.HitResult=HitResult;
	UFunction* Func = FindFunctionChecked(NAME_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_HitResult_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function Client_ConfirmDamage constinit property declarations ******************
	static const UECodeGen_Private::FStructPropertyParams NewProp_HitResult;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function Client_ConfirmDamage constinit property declarations ********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function Client_ConfirmDamage Property Definitions *****************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage_Statics::NewProp_HitResult = { "HitResult", nullptr, (EPropertyFlags)0x0010000008000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GA_Weapon_MeleeLight_ClientPrediction_eventClient_ConfirmDamage_Parms, HitResult), Z_Construct_UScriptStruct_FCombatHitResult, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_HitResult_MetaData), NewProp_HitResult_MetaData) }; // 3717135170
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage_Statics::NewProp_HitResult,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage_Statics::PropPointers) < 2048);
// ********** End Function Client_ConfirmDamage Property Definitions *******************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction, nullptr, "Client_ConfirmDamage", 	Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage_Statics::PropPointers), 
sizeof(GA_Weapon_MeleeLight_ClientPrediction_eventClient_ConfirmDamage_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x01080CC0, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(GA_Weapon_MeleeLight_ClientPrediction_eventClient_ConfirmDamage_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGA_Weapon_MeleeLight_ClientPrediction::execClient_ConfirmDamage)
{
	P_GET_STRUCT(FCombatHitResult,Z_Param_HitResult);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->Client_ConfirmDamage_Implementation(Z_Param_HitResult);
	P_NATIVE_END;
}
// ********** End Class UGA_Weapon_MeleeLight_ClientPrediction Function Client_ConfirmDamage *******

// ********** Begin Class UGA_Weapon_MeleeLight_ClientPrediction Function HandleComboWindowBegin ***
struct Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics
{
	struct GA_Weapon_MeleeLight_ClientPrediction_eventHandleComboWindowBegin_Parms
	{
		FGameplayEventData Payload;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// ============================================\n// HANDLERS\n// ============================================\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "HANDLERS" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function HandleComboWindowBegin constinit property declarations ****************
	static const UECodeGen_Private::FStructPropertyParams NewProp_Payload;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function HandleComboWindowBegin constinit property declarations ******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function HandleComboWindowBegin Property Definitions ***************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::NewProp_Payload = { "Payload", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GA_Weapon_MeleeLight_ClientPrediction_eventHandleComboWindowBegin_Parms, Payload), Z_Construct_UScriptStruct_FGameplayEventData, METADATA_PARAMS(0, nullptr) }; // 1110230437
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::NewProp_Payload,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::PropPointers) < 2048);
// ********** End Function HandleComboWindowBegin Property Definitions *****************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction, nullptr, "HandleComboWindowBegin", 	Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::GA_Weapon_MeleeLight_ClientPrediction_eventHandleComboWindowBegin_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::GA_Weapon_MeleeLight_ClientPrediction_eventHandleComboWindowBegin_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGA_Weapon_MeleeLight_ClientPrediction::execHandleComboWindowBegin)
{
	P_GET_STRUCT(FGameplayEventData,Z_Param_Payload);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->HandleComboWindowBegin(Z_Param_Payload);
	P_NATIVE_END;
}
// ********** End Class UGA_Weapon_MeleeLight_ClientPrediction Function HandleComboWindowBegin *****

// ********** Begin Class UGA_Weapon_MeleeLight_ClientPrediction Function HandleComboWindowEnd *****
struct Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics
{
	struct GA_Weapon_MeleeLight_ClientPrediction_eventHandleComboWindowEnd_Parms
	{
		FGameplayEventData Payload;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function HandleComboWindowEnd constinit property declarations ******************
	static const UECodeGen_Private::FStructPropertyParams NewProp_Payload;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function HandleComboWindowEnd constinit property declarations ********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function HandleComboWindowEnd Property Definitions *****************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::NewProp_Payload = { "Payload", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GA_Weapon_MeleeLight_ClientPrediction_eventHandleComboWindowEnd_Parms, Payload), Z_Construct_UScriptStruct_FGameplayEventData, METADATA_PARAMS(0, nullptr) }; // 1110230437
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::NewProp_Payload,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::PropPointers) < 2048);
// ********** End Function HandleComboWindowEnd Property Definitions *******************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction, nullptr, "HandleComboWindowEnd", 	Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::GA_Weapon_MeleeLight_ClientPrediction_eventHandleComboWindowEnd_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::GA_Weapon_MeleeLight_ClientPrediction_eventHandleComboWindowEnd_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGA_Weapon_MeleeLight_ClientPrediction::execHandleComboWindowEnd)
{
	P_GET_STRUCT(FGameplayEventData,Z_Param_Payload);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->HandleComboWindowEnd(Z_Param_Payload);
	P_NATIVE_END;
}
// ********** End Class UGA_Weapon_MeleeLight_ClientPrediction Function HandleComboWindowEnd *******

// ********** Begin Class UGA_Weapon_MeleeLight_ClientPrediction Function HandleMontageCancelled ***
struct Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCancelled_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function HandleMontageCancelled constinit property declarations ****************
// ********** End Function HandleMontageCancelled constinit property declarations ******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCancelled_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction, nullptr, "HandleMontageCancelled", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCancelled_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCancelled_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCancelled()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCancelled_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGA_Weapon_MeleeLight_ClientPrediction::execHandleMontageCancelled)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->HandleMontageCancelled();
	P_NATIVE_END;
}
// ********** End Class UGA_Weapon_MeleeLight_ClientPrediction Function HandleMontageCancelled *****

// ********** Begin Class UGA_Weapon_MeleeLight_ClientPrediction Function HandleMontageCompleted ***
struct Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCompleted_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function HandleMontageCompleted constinit property declarations ****************
// ********** End Function HandleMontageCompleted constinit property declarations ******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCompleted_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction, nullptr, "HandleMontageCompleted", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCompleted_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCompleted_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCompleted()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCompleted_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGA_Weapon_MeleeLight_ClientPrediction::execHandleMontageCompleted)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->HandleMontageCompleted();
	P_NATIVE_END;
}
// ********** End Class UGA_Weapon_MeleeLight_ClientPrediction Function HandleMontageCompleted *****

// ********** Begin Class UGA_Weapon_MeleeLight_ClientPrediction Function HandleMontageInterrupted *
struct Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageInterrupted_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function HandleMontageInterrupted constinit property declarations **************
// ********** End Function HandleMontageInterrupted constinit property declarations ****************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageInterrupted_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction, nullptr, "HandleMontageInterrupted", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageInterrupted_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageInterrupted_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageInterrupted()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageInterrupted_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGA_Weapon_MeleeLight_ClientPrediction::execHandleMontageInterrupted)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->HandleMontageInterrupted();
	P_NATIVE_END;
}
// ********** End Class UGA_Weapon_MeleeLight_ClientPrediction Function HandleMontageInterrupted ***

// ********** Begin Class UGA_Weapon_MeleeLight_ClientPrediction Function Server_ReportHitResult ***
struct GA_Weapon_MeleeLight_ClientPrediction_eventServer_ReportHitResult_Parms
{
	FCombatHitResult HitResult;
};
static FName NAME_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult = FName(TEXT("Server_ReportHitResult"));
void UGA_Weapon_MeleeLight_ClientPrediction::Server_ReportHitResult(FCombatHitResult const& HitResult)
{
	GA_Weapon_MeleeLight_ClientPrediction_eventServer_ReportHitResult_Parms Parms;
	Parms.HitResult=HitResult;
	UFunction* Func = FindFunctionChecked(NAME_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// ============================================\n// RPCs\n// ============================================\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "RPCs" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_HitResult_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function Server_ReportHitResult constinit property declarations ****************
	static const UECodeGen_Private::FStructPropertyParams NewProp_HitResult;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function Server_ReportHitResult constinit property declarations ******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function Server_ReportHitResult Property Definitions ***************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult_Statics::NewProp_HitResult = { "HitResult", nullptr, (EPropertyFlags)0x0010000008000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GA_Weapon_MeleeLight_ClientPrediction_eventServer_ReportHitResult_Parms, HitResult), Z_Construct_UScriptStruct_FCombatHitResult, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_HitResult_MetaData), NewProp_HitResult_MetaData) }; // 3717135170
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult_Statics::NewProp_HitResult,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult_Statics::PropPointers) < 2048);
// ********** End Function Server_ReportHitResult Property Definitions *****************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction, nullptr, "Server_ReportHitResult", 	Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult_Statics::PropPointers), 
sizeof(GA_Weapon_MeleeLight_ClientPrediction_eventServer_ReportHitResult_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x80280CC0, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(GA_Weapon_MeleeLight_ClientPrediction_eventServer_ReportHitResult_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGA_Weapon_MeleeLight_ClientPrediction::execServer_ReportHitResult)
{
	P_GET_STRUCT(FCombatHitResult,Z_Param_HitResult);
	P_FINISH;
	P_NATIVE_BEGIN;
	if (!P_THIS->Server_ReportHitResult_Validate(Z_Param_HitResult))
	{
		RPC_ValidateFailed(TEXT("Server_ReportHitResult_Validate"));
		return;
	}
	P_THIS->Server_ReportHitResult_Implementation(Z_Param_HitResult);
	P_NATIVE_END;
}
// ********** End Class UGA_Weapon_MeleeLight_ClientPrediction Function Server_ReportHitResult *****

// ********** Begin Class UGA_Weapon_MeleeLight_ClientPrediction ***********************************
FClassRegistrationInfo Z_Registration_Info_UClass_UGA_Weapon_MeleeLight_ClientPrediction;
UClass* UGA_Weapon_MeleeLight_ClientPrediction::GetPrivateStaticClass()
{
	using TClass = UGA_Weapon_MeleeLight_ClientPrediction;
	if (!Z_Registration_Info_UClass_UGA_Weapon_MeleeLight_ClientPrediction.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("GA_Weapon_MeleeLight_ClientPrediction"),
			Z_Registration_Info_UClass_UGA_Weapon_MeleeLight_ClientPrediction.InnerSingleton,
			StaticRegisterNativesUGA_Weapon_MeleeLight_ClientPrediction,
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
	return Z_Registration_Info_UClass_UGA_Weapon_MeleeLight_ClientPrediction.InnerSingleton;
}
UClass* Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_NoRegister()
{
	return UGA_Weapon_MeleeLight_ClientPrediction::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DamageGE_MetaData[] = {
		{ "Category", "Config|Damage" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// ============================================\n// CONFIGURA\xc3\x87\xc3\x83O\n// ============================================\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "CONFIGURA\xc3\x87\xc3\x83O" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AttackMontage_MetaData[] = {
		{ "Category", "Config|Animation" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ComboSections_MetaData[] = {
		{ "Category", "Config|Combo" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxComboHits_MetaData[] = {
		{ "Category", "Config|Combo" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ComboWindowBeginTag_MetaData[] = {
		{ "Category", "Config|Combo" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ComboWindowEndTag_MetaData[] = {
		{ "Category", "Config|Combo" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TraceChannel_MetaData[] = {
		{ "Category", "Config|Trace" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MontagePlayRate_MetaData[] = {
		{ "Category", "Config|Animation" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_StopBlendOutTime_MetaData[] = {
		{ "Category", "Config|Animation" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BaseDamage_MetaData[] = {
		{ "Category", "Config|Damage" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bDebug_MetaData[] = {
		{ "Category", "Config|Debug" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CachedWeaponData_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Cache - CORRIGIDO: Usando raw pointer em vez de TObjectPtr\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Cache - CORRIGIDO: Usando raw pointer em vez de TObjectPtr" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CachedMontage_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MontageTask_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Tasks\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Tasks" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WaitBeginTask_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WaitEndTask_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UGA_Weapon_MeleeLight_ClientPrediction constinit property declarations ***
	static const UECodeGen_Private::FClassPropertyParams NewProp_DamageGE;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_AttackMontage;
	static const UECodeGen_Private::FNamePropertyParams NewProp_ComboSections_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ComboSections;
	static const UECodeGen_Private::FIntPropertyParams NewProp_MaxComboHits;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ComboWindowBeginTag;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ComboWindowEndTag;
	static const UECodeGen_Private::FBytePropertyParams NewProp_TraceChannel;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_MontagePlayRate;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_StopBlendOutTime;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BaseDamage;
	static void NewProp_bDebug_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bDebug;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CachedWeaponData;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CachedMontage;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_MontageTask;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WaitBeginTask;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WaitEndTask;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UGA_Weapon_MeleeLight_ClientPrediction constinit property declarations *****
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("Client_ConfirmDamage"), .Pointer = &UGA_Weapon_MeleeLight_ClientPrediction::execClient_ConfirmDamage },
		{ .NameUTF8 = UTF8TEXT("HandleComboWindowBegin"), .Pointer = &UGA_Weapon_MeleeLight_ClientPrediction::execHandleComboWindowBegin },
		{ .NameUTF8 = UTF8TEXT("HandleComboWindowEnd"), .Pointer = &UGA_Weapon_MeleeLight_ClientPrediction::execHandleComboWindowEnd },
		{ .NameUTF8 = UTF8TEXT("HandleMontageCancelled"), .Pointer = &UGA_Weapon_MeleeLight_ClientPrediction::execHandleMontageCancelled },
		{ .NameUTF8 = UTF8TEXT("HandleMontageCompleted"), .Pointer = &UGA_Weapon_MeleeLight_ClientPrediction::execHandleMontageCompleted },
		{ .NameUTF8 = UTF8TEXT("HandleMontageInterrupted"), .Pointer = &UGA_Weapon_MeleeLight_ClientPrediction::execHandleMontageInterrupted },
		{ .NameUTF8 = UTF8TEXT("Server_ReportHitResult"), .Pointer = &UGA_Weapon_MeleeLight_ClientPrediction::execServer_ReportHitResult },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Client_ConfirmDamage, "Client_ConfirmDamage" }, // 3299684058
		{ &Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowBegin, "HandleComboWindowBegin" }, // 841785828
		{ &Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleComboWindowEnd, "HandleComboWindowEnd" }, // 3881823934
		{ &Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCancelled, "HandleMontageCancelled" }, // 70925318
		{ &Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageCompleted, "HandleMontageCompleted" }, // 2455638697
		{ &Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_HandleMontageInterrupted, "HandleMontageInterrupted" }, // 1360317006
		{ &Z_Construct_UFunction_UGA_Weapon_MeleeLight_ClientPrediction_Server_ReportHitResult, "Server_ReportHitResult" }, // 2851084583
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGA_Weapon_MeleeLight_ClientPrediction>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics

// ********** Begin Class UGA_Weapon_MeleeLight_ClientPrediction Property Definitions **************
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_DamageGE = { "DamageGE", nullptr, (EPropertyFlags)0x0024080000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, DamageGE), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DamageGE_MetaData), NewProp_DamageGE_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_AttackMontage = { "AttackMontage", nullptr, (EPropertyFlags)0x0124080000010015, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, AttackMontage), Z_Construct_UClass_UAnimMontage_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AttackMontage_MetaData), NewProp_AttackMontage_MetaData) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_ComboSections_Inner = { "ComboSections", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_ComboSections = { "ComboSections", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, ComboSections), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ComboSections_MetaData), NewProp_ComboSections_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_MaxComboHits = { "MaxComboHits", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, MaxComboHits), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxComboHits_MetaData), NewProp_MaxComboHits_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_ComboWindowBeginTag = { "ComboWindowBeginTag", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, ComboWindowBeginTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ComboWindowBeginTag_MetaData), NewProp_ComboWindowBeginTag_MetaData) }; // 517357616
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_ComboWindowEndTag = { "ComboWindowEndTag", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, ComboWindowEndTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ComboWindowEndTag_MetaData), NewProp_ComboWindowEndTag_MetaData) }; // 517357616
const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_TraceChannel = { "TraceChannel", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, TraceChannel), Z_Construct_UEnum_Engine_ECollisionChannel, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TraceChannel_MetaData), NewProp_TraceChannel_MetaData) }; // 838391399
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_MontagePlayRate = { "MontagePlayRate", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, MontagePlayRate), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MontagePlayRate_MetaData), NewProp_MontagePlayRate_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_StopBlendOutTime = { "StopBlendOutTime", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, StopBlendOutTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_StopBlendOutTime_MetaData), NewProp_StopBlendOutTime_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_BaseDamage = { "BaseDamage", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, BaseDamage), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BaseDamage_MetaData), NewProp_BaseDamage_MetaData) };
void Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_bDebug_SetBit(void* Obj)
{
	((UGA_Weapon_MeleeLight_ClientPrediction*)Obj)->bDebug = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_bDebug = { "bDebug", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UGA_Weapon_MeleeLight_ClientPrediction), &Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_bDebug_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bDebug_MetaData), NewProp_bDebug_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_CachedWeaponData = { "CachedWeaponData", nullptr, (EPropertyFlags)0x0040000000002000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, CachedWeaponData), Z_Construct_UClass_UDA_WeaponDefinition_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CachedWeaponData_MetaData), NewProp_CachedWeaponData_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_CachedMontage = { "CachedMontage", nullptr, (EPropertyFlags)0x0040000000002000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, CachedMontage), Z_Construct_UClass_UAnimMontage_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CachedMontage_MetaData), NewProp_CachedMontage_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_MontageTask = { "MontageTask", nullptr, (EPropertyFlags)0x0144000000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, MontageTask), Z_Construct_UClass_UAbilityTask_PlayMontageAndWait_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MontageTask_MetaData), NewProp_MontageTask_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_WaitBeginTask = { "WaitBeginTask", nullptr, (EPropertyFlags)0x0144000000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, WaitBeginTask), Z_Construct_UClass_UAbilityTask_WaitGameplayEvent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WaitBeginTask_MetaData), NewProp_WaitBeginTask_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_WaitEndTask = { "WaitEndTask", nullptr, (EPropertyFlags)0x0144000000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight_ClientPrediction, WaitEndTask), Z_Construct_UClass_UAbilityTask_WaitGameplayEvent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WaitEndTask_MetaData), NewProp_WaitEndTask_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_DamageGE,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_AttackMontage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_ComboSections_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_ComboSections,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_MaxComboHits,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_ComboWindowBeginTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_ComboWindowEndTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_TraceChannel,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_MontagePlayRate,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_StopBlendOutTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_BaseDamage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_bDebug,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_CachedWeaponData,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_CachedMontage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_MontageTask,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_WaitBeginTask,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::NewProp_WaitEndTask,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::PropPointers) < 2048);
// ********** End Class UGA_Weapon_MeleeLight_ClientPrediction Property Definitions ****************
UObject* (*const Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USpellRiseGameplayAbility,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::ClassParams = {
	&UGA_Weapon_MeleeLight_ClientPrediction::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::PropPointers),
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::Class_MetaDataParams), Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::Class_MetaDataParams)
};
void UGA_Weapon_MeleeLight_ClientPrediction::StaticRegisterNativesUGA_Weapon_MeleeLight_ClientPrediction()
{
	UClass* Class = UGA_Weapon_MeleeLight_ClientPrediction::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::Funcs));
}
UClass* Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction()
{
	if (!Z_Registration_Info_UClass_UGA_Weapon_MeleeLight_ClientPrediction.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGA_Weapon_MeleeLight_ClientPrediction.OuterSingleton, Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UGA_Weapon_MeleeLight_ClientPrediction.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UGA_Weapon_MeleeLight_ClientPrediction);
UGA_Weapon_MeleeLight_ClientPrediction::~UGA_Weapon_MeleeLight_ClientPrediction() {}
// ********** End Class UGA_Weapon_MeleeLight_ClientPrediction *************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h__Script_SpellRise_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FCombatHitResult::StaticStruct, Z_Construct_UScriptStruct_FCombatHitResult_Statics::NewStructOps, TEXT("CombatHitResult"),&Z_Registration_Info_UScriptStruct_FCombatHitResult, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FCombatHitResult), 3717135170U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction, UGA_Weapon_MeleeLight_ClientPrediction::StaticClass, TEXT("UGA_Weapon_MeleeLight_ClientPrediction"), &Z_Registration_Info_UClass_UGA_Weapon_MeleeLight_ClientPrediction, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGA_Weapon_MeleeLight_ClientPrediction), 279941708U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h__Script_SpellRise_2738678614{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h__Script_SpellRise_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h__Script_SpellRise_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h__Script_SpellRise_Statics::ScriptStructInfo),
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
