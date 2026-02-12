// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Combat/Melee/Abilities/GA_Weapon_MeleeLight.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayTagContainer.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeGA_Weapon_MeleeLight() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UClass_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UAnimMontage_NoRegister();
ENGINE_API UEnum* Z_Construct_UEnum_Engine_ECollisionChannel();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_PlayMontageAndWait_NoRegister();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_WaitGameplayEvent_NoRegister();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayEventData();
GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
SPELLRISE_API UClass* Z_Construct_UClass_UDA_WeaponDefinition_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UGA_Weapon_MeleeLight();
SPELLRISE_API UClass* Z_Construct_UClass_UGA_Weapon_MeleeLight_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGameplayAbility();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UGA_Weapon_MeleeLight Function OnComboWindowBegin ************************
struct Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics
{
	struct GA_Weapon_MeleeLight_eventOnComboWindowBegin_Parms
	{
		FGameplayEventData Payload;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Events\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Events" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function OnComboWindowBegin constinit property declarations ********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_Payload;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnComboWindowBegin constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnComboWindowBegin Property Definitions *******************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::NewProp_Payload = { "Payload", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GA_Weapon_MeleeLight_eventOnComboWindowBegin_Parms, Payload), Z_Construct_UScriptStruct_FGameplayEventData, METADATA_PARAMS(0, nullptr) }; // 1110230437
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::NewProp_Payload,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::PropPointers) < 2048);
// ********** End Function OnComboWindowBegin Property Definitions *********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGA_Weapon_MeleeLight, nullptr, "OnComboWindowBegin", 	Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::GA_Weapon_MeleeLight_eventOnComboWindowBegin_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::GA_Weapon_MeleeLight_eventOnComboWindowBegin_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGA_Weapon_MeleeLight::execOnComboWindowBegin)
{
	P_GET_STRUCT(FGameplayEventData,Z_Param_Payload);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnComboWindowBegin(Z_Param_Payload);
	P_NATIVE_END;
}
// ********** End Class UGA_Weapon_MeleeLight Function OnComboWindowBegin **************************

// ********** Begin Class UGA_Weapon_MeleeLight Function OnComboWindowEnd **************************
struct Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics
{
	struct GA_Weapon_MeleeLight_eventOnComboWindowEnd_Parms
	{
		FGameplayEventData Payload;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnComboWindowEnd constinit property declarations **********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_Payload;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnComboWindowEnd constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnComboWindowEnd Property Definitions *********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::NewProp_Payload = { "Payload", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(GA_Weapon_MeleeLight_eventOnComboWindowEnd_Parms, Payload), Z_Construct_UScriptStruct_FGameplayEventData, METADATA_PARAMS(0, nullptr) }; // 1110230437
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::NewProp_Payload,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::PropPointers) < 2048);
// ********** End Function OnComboWindowEnd Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGA_Weapon_MeleeLight, nullptr, "OnComboWindowEnd", 	Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::GA_Weapon_MeleeLight_eventOnComboWindowEnd_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::GA_Weapon_MeleeLight_eventOnComboWindowEnd_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGA_Weapon_MeleeLight::execOnComboWindowEnd)
{
	P_GET_STRUCT(FGameplayEventData,Z_Param_Payload);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnComboWindowEnd(Z_Param_Payload);
	P_NATIVE_END;
}
// ********** End Class UGA_Weapon_MeleeLight Function OnComboWindowEnd ****************************

// ********** Begin Class UGA_Weapon_MeleeLight Function OnMontageCompleted ************************
struct Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageCompleted_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnMontageCompleted constinit property declarations ********************
// ********** End Function OnMontageCompleted constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageCompleted_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGA_Weapon_MeleeLight, nullptr, "OnMontageCompleted", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageCompleted_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageCompleted_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageCompleted()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageCompleted_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGA_Weapon_MeleeLight::execOnMontageCompleted)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnMontageCompleted();
	P_NATIVE_END;
}
// ********** End Class UGA_Weapon_MeleeLight Function OnMontageCompleted **************************

// ********** Begin Class UGA_Weapon_MeleeLight Function OnMontageInterrupted **********************
struct Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageInterrupted_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnMontageInterrupted constinit property declarations ******************
// ********** End Function OnMontageInterrupted constinit property declarations ********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageInterrupted_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UGA_Weapon_MeleeLight, nullptr, "OnMontageInterrupted", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageInterrupted_Statics::Function_MetaDataParams), Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageInterrupted_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageInterrupted()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageInterrupted_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UGA_Weapon_MeleeLight::execOnMontageInterrupted)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnMontageInterrupted();
	P_NATIVE_END;
}
// ********** End Class UGA_Weapon_MeleeLight Function OnMontageInterrupted ************************

// ********** Begin Class UGA_Weapon_MeleeLight ****************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UGA_Weapon_MeleeLight;
UClass* UGA_Weapon_MeleeLight::GetPrivateStaticClass()
{
	using TClass = UGA_Weapon_MeleeLight;
	if (!Z_Registration_Info_UClass_UGA_Weapon_MeleeLight.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("GA_Weapon_MeleeLight"),
			Z_Registration_Info_UClass_UGA_Weapon_MeleeLight.InnerSingleton,
			StaticRegisterNativesUGA_Weapon_MeleeLight,
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
	return Z_Registration_Info_UClass_UGA_Weapon_MeleeLight.InnerSingleton;
}
UClass* Z_Construct_UClass_UGA_Weapon_MeleeLight_NoRegister()
{
	return UGA_Weapon_MeleeLight::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Light melee combo ability (3-hit default).\n *\n * Multiplayer goals:\n * - LocalPredicted for responsiveness (client plays montage immediately).\n * - Server authoritative for damage + combo progression (anti-cheat / hardcore).\n * - Simulated proxies get montage via ASC montage replication.\n * - Input is replicated directly so the server can advance combos reliably.\n */" },
#endif
		{ "IncludePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Light melee combo ability (3-hit default).\n\nMultiplayer goals:\n- LocalPredicted for responsiveness (client plays montage immediately).\n- Server authoritative for damage + combo progression (anti-cheat / hardcore).\n- Simulated proxies get montage via ASC montage replication.\n- Input is replicated directly so the server can advance combos reliably." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DamageGE_MetaData[] = {
		{ "Category", "Melee|Damage" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BaseDamagePerHit_MetaData[] = {
		{ "Category", "Melee|Damage|Legacy" },
		{ "ClampMin", "0.0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Legacy fallback (pre-DA)\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Legacy fallback (pre-DA)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BaseDamage_MetaData[] = {
		{ "Category", "Melee|Damage|Legacy" },
		{ "ClampMin", "0.0" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AttackMontage_MetaData[] = {
		{ "Category", "Melee|Anim" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Default montage if DA doesn't provide one.\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Default montage if DA doesn't provide one." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MontagePlayRate_MetaData[] = {
		{ "Category", "Melee|Anim" },
		{ "ClampMin", "0.1" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ComboSections_MetaData[] = {
		{ "Category", "Melee|Combo" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxComboHits_MetaData[] = {
		{ "Category", "Melee|Combo" },
		{ "ClampMax", "3" },
		{ "ClampMin", "1" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ComboWindowBeginTag_MetaData[] = {
		{ "Category", "Melee|Combo" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ComboWindowEndTag_MetaData[] = {
		{ "Category", "Melee|Combo" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bDebug_MetaData[] = {
		{ "Category", "Melee|Debug" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TraceChannel_MetaData[] = {
		{ "Category", "Melee|Trace" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AttackSpeedMin_MetaData[] = {
		{ "Category", "Melee|Anim" },
		{ "ClampMax", "3.0" },
		{ "ClampMin", "0.25" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// AAA limits to prevent absurd speed via buffs/lag/exploits\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "AAA limits to prevent absurd speed via buffs/lag/exploits" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AttackSpeedMax_MetaData[] = {
		{ "Category", "Melee|Anim" },
		{ "ClampMax", "3.0" },
		{ "ClampMin", "0.25" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_StopBlendOutTime_MetaData[] = {
		{ "Category", "Melee|Anim" },
		{ "ClampMin", "0.0" },
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MontageTask_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WaitComboBeginTask_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WaitComboEndTask_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CachedWeaponDA_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CachedAttackMontage_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UGA_Weapon_MeleeLight constinit property declarations ********************
	static const UECodeGen_Private::FClassPropertyParams NewProp_DamageGE;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BaseDamagePerHit_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_BaseDamagePerHit;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BaseDamage;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_AttackMontage;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_MontagePlayRate;
	static const UECodeGen_Private::FNamePropertyParams NewProp_ComboSections_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ComboSections;
	static const UECodeGen_Private::FIntPropertyParams NewProp_MaxComboHits;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ComboWindowBeginTag;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ComboWindowEndTag;
	static void NewProp_bDebug_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bDebug;
	static const UECodeGen_Private::FBytePropertyParams NewProp_TraceChannel;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_AttackSpeedMin;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_AttackSpeedMax;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_StopBlendOutTime;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_MontageTask;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WaitComboBeginTask;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WaitComboEndTask;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CachedWeaponDA;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CachedAttackMontage;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UGA_Weapon_MeleeLight constinit property declarations **********************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("OnComboWindowBegin"), .Pointer = &UGA_Weapon_MeleeLight::execOnComboWindowBegin },
		{ .NameUTF8 = UTF8TEXT("OnComboWindowEnd"), .Pointer = &UGA_Weapon_MeleeLight::execOnComboWindowEnd },
		{ .NameUTF8 = UTF8TEXT("OnMontageCompleted"), .Pointer = &UGA_Weapon_MeleeLight::execOnMontageCompleted },
		{ .NameUTF8 = UTF8TEXT("OnMontageInterrupted"), .Pointer = &UGA_Weapon_MeleeLight::execOnMontageInterrupted },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowBegin, "OnComboWindowBegin" }, // 4274254603
		{ &Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnComboWindowEnd, "OnComboWindowEnd" }, // 2457988522
		{ &Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageCompleted, "OnMontageCompleted" }, // 421601363
		{ &Z_Construct_UFunction_UGA_Weapon_MeleeLight_OnMontageInterrupted, "OnMontageInterrupted" }, // 3896694985
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGA_Weapon_MeleeLight>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics

// ********** Begin Class UGA_Weapon_MeleeLight Property Definitions *******************************
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_DamageGE = { "DamageGE", nullptr, (EPropertyFlags)0x0024080000010001, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, DamageGE), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DamageGE_MetaData), NewProp_DamageGE_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_BaseDamagePerHit_Inner = { "BaseDamagePerHit", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_BaseDamagePerHit = { "BaseDamagePerHit", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, BaseDamagePerHit), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BaseDamagePerHit_MetaData), NewProp_BaseDamagePerHit_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_BaseDamage = { "BaseDamage", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, BaseDamage), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BaseDamage_MetaData), NewProp_BaseDamage_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_AttackMontage = { "AttackMontage", nullptr, (EPropertyFlags)0x0124080000010001, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, AttackMontage), Z_Construct_UClass_UAnimMontage_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AttackMontage_MetaData), NewProp_AttackMontage_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_MontagePlayRate = { "MontagePlayRate", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, MontagePlayRate), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MontagePlayRate_MetaData), NewProp_MontagePlayRate_MetaData) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_ComboSections_Inner = { "ComboSections", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_ComboSections = { "ComboSections", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, ComboSections), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ComboSections_MetaData), NewProp_ComboSections_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_MaxComboHits = { "MaxComboHits", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, MaxComboHits), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxComboHits_MetaData), NewProp_MaxComboHits_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_ComboWindowBeginTag = { "ComboWindowBeginTag", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, ComboWindowBeginTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ComboWindowBeginTag_MetaData), NewProp_ComboWindowBeginTag_MetaData) }; // 517357616
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_ComboWindowEndTag = { "ComboWindowEndTag", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, ComboWindowEndTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ComboWindowEndTag_MetaData), NewProp_ComboWindowEndTag_MetaData) }; // 517357616
void Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_bDebug_SetBit(void* Obj)
{
	((UGA_Weapon_MeleeLight*)Obj)->bDebug = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_bDebug = { "bDebug", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UGA_Weapon_MeleeLight), &Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_bDebug_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bDebug_MetaData), NewProp_bDebug_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_TraceChannel = { "TraceChannel", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, TraceChannel), Z_Construct_UEnum_Engine_ECollisionChannel, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TraceChannel_MetaData), NewProp_TraceChannel_MetaData) }; // 838391399
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_AttackSpeedMin = { "AttackSpeedMin", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, AttackSpeedMin), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AttackSpeedMin_MetaData), NewProp_AttackSpeedMin_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_AttackSpeedMax = { "AttackSpeedMax", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, AttackSpeedMax), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AttackSpeedMax_MetaData), NewProp_AttackSpeedMax_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_StopBlendOutTime = { "StopBlendOutTime", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, StopBlendOutTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_StopBlendOutTime_MetaData), NewProp_StopBlendOutTime_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_MontageTask = { "MontageTask", nullptr, (EPropertyFlags)0x0144000000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, MontageTask), Z_Construct_UClass_UAbilityTask_PlayMontageAndWait_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MontageTask_MetaData), NewProp_MontageTask_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_WaitComboBeginTask = { "WaitComboBeginTask", nullptr, (EPropertyFlags)0x0144000000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, WaitComboBeginTask), Z_Construct_UClass_UAbilityTask_WaitGameplayEvent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WaitComboBeginTask_MetaData), NewProp_WaitComboBeginTask_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_WaitComboEndTask = { "WaitComboEndTask", nullptr, (EPropertyFlags)0x0144000000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, WaitComboEndTask), Z_Construct_UClass_UAbilityTask_WaitGameplayEvent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WaitComboEndTask_MetaData), NewProp_WaitComboEndTask_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_CachedWeaponDA = { "CachedWeaponDA", nullptr, (EPropertyFlags)0x0144000000002000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, CachedWeaponDA), Z_Construct_UClass_UDA_WeaponDefinition_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CachedWeaponDA_MetaData), NewProp_CachedWeaponDA_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_CachedAttackMontage = { "CachedAttackMontage", nullptr, (EPropertyFlags)0x0144000000002000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UGA_Weapon_MeleeLight, CachedAttackMontage), Z_Construct_UClass_UAnimMontage_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CachedAttackMontage_MetaData), NewProp_CachedAttackMontage_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_DamageGE,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_BaseDamagePerHit_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_BaseDamagePerHit,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_BaseDamage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_AttackMontage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_MontagePlayRate,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_ComboSections_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_ComboSections,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_MaxComboHits,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_ComboWindowBeginTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_ComboWindowEndTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_bDebug,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_TraceChannel,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_AttackSpeedMin,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_AttackSpeedMax,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_StopBlendOutTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_MontageTask,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_WaitComboBeginTask,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_WaitComboEndTask,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_CachedWeaponDA,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::NewProp_CachedAttackMontage,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::PropPointers) < 2048);
// ********** End Class UGA_Weapon_MeleeLight Property Definitions *********************************
UObject* (*const Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USpellRiseGameplayAbility,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::ClassParams = {
	&UGA_Weapon_MeleeLight::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::PropPointers),
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::Class_MetaDataParams), Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::Class_MetaDataParams)
};
void UGA_Weapon_MeleeLight::StaticRegisterNativesUGA_Weapon_MeleeLight()
{
	UClass* Class = UGA_Weapon_MeleeLight::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::Funcs));
}
UClass* Z_Construct_UClass_UGA_Weapon_MeleeLight()
{
	if (!Z_Registration_Info_UClass_UGA_Weapon_MeleeLight.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGA_Weapon_MeleeLight.OuterSingleton, Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UGA_Weapon_MeleeLight.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UGA_Weapon_MeleeLight);
UGA_Weapon_MeleeLight::~UGA_Weapon_MeleeLight() {}
// ********** End Class UGA_Weapon_MeleeLight ******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGA_Weapon_MeleeLight, UGA_Weapon_MeleeLight::StaticClass, TEXT("UGA_Weapon_MeleeLight"), &Z_Registration_Info_UClass_UGA_Weapon_MeleeLight, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGA_Weapon_MeleeLight), 3339990638U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h__Script_SpellRise_3527655229{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
