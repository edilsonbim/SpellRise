// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Weapons/Components/SpellRiseWeaponComponent.h"
#include "Engine/HitResult.h"
#include "GameplayTagContainer.h"
#include "UObject/Class.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSpellRiseWeaponComponent() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UClass_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UActorComponent();
ENGINE_API UEnum* Z_Construct_UEnum_Engine_ECollisionChannel();
ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FHitResult();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRiseWeaponActor_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UDA_WeaponDefinition_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseWeaponComponent();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseWeaponComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class USpellRiseWeaponComponent Function EquipWeapon ***************************
struct Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics
{
	struct SpellRiseWeaponComponent_eventEquipWeapon_Parms
	{
		UDA_WeaponDefinition* NewWeapon;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Weapon" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Equip weapon (safe to call on client; will route to server). */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Equip weapon (safe to call on client; will route to server)." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function EquipWeapon constinit property declarations ***************************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_NewWeapon;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function EquipWeapon constinit property declarations *****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function EquipWeapon Property Definitions **************************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::NewProp_NewWeapon = { "NewWeapon", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventEquipWeapon_Parms, NewWeapon), Z_Construct_UClass_UDA_WeaponDefinition_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::NewProp_NewWeapon,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::PropPointers) < 2048);
// ********** End Function EquipWeapon Property Definitions ****************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseWeaponComponent, nullptr, "EquipWeapon", 	Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::SpellRiseWeaponComponent_eventEquipWeapon_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::SpellRiseWeaponComponent_eventEquipWeapon_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseWeaponComponent::execEquipWeapon)
{
	P_GET_OBJECT(UDA_WeaponDefinition,Z_Param_NewWeapon);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->EquipWeapon(Z_Param_NewWeapon);
	P_NATIVE_END;
}
// ********** End Class USpellRiseWeaponComponent Function EquipWeapon *****************************

// ********** Begin Class USpellRiseWeaponComponent Function GetEquippedWeapon *********************
struct Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics
{
	struct SpellRiseWeaponComponent_eventGetEquippedWeapon_Parms
	{
		UDA_WeaponDefinition* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Weapon" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Returns the equipped weapon definition (may be null). */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Returns the equipped weapon definition (may be null)." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function GetEquippedWeapon constinit property declarations *********************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GetEquippedWeapon constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GetEquippedWeapon Property Definitions ********************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventGetEquippedWeapon_Parms, ReturnValue), Z_Construct_UClass_UDA_WeaponDefinition_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::PropPointers) < 2048);
// ********** End Function GetEquippedWeapon Property Definitions **********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseWeaponComponent, nullptr, "GetEquippedWeapon", 	Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::SpellRiseWeaponComponent_eventGetEquippedWeapon_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::SpellRiseWeaponComponent_eventGetEquippedWeapon_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseWeaponComponent::execGetEquippedWeapon)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UDA_WeaponDefinition**)Z_Param__Result=P_THIS->GetEquippedWeapon();
	P_NATIVE_END;
}
// ********** End Class USpellRiseWeaponComponent Function GetEquippedWeapon ***********************

// ********** Begin Class USpellRiseWeaponComponent Function GetEquippedWeaponActor ****************
struct Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics
{
	struct SpellRiseWeaponComponent_eventGetEquippedWeaponActor_Parms
	{
		ASpellRiseWeaponActor* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Weapon" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Returns the currently spawned weapon actor if it derives from ASpellRiseWeaponActor (may be null). */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Returns the currently spawned weapon actor if it derives from ASpellRiseWeaponActor (may be null)." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function GetEquippedWeaponActor constinit property declarations ****************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GetEquippedWeaponActor constinit property declarations ******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GetEquippedWeaponActor Property Definitions ***************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventGetEquippedWeaponActor_Parms, ReturnValue), Z_Construct_UClass_ASpellRiseWeaponActor_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::PropPointers) < 2048);
// ********** End Function GetEquippedWeaponActor Property Definitions *****************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseWeaponComponent, nullptr, "GetEquippedWeaponActor", 	Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::SpellRiseWeaponComponent_eventGetEquippedWeaponActor_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::SpellRiseWeaponComponent_eventGetEquippedWeaponActor_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseWeaponComponent::execGetEquippedWeaponActor)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(ASpellRiseWeaponActor**)Z_Param__Result=P_THIS->GetEquippedWeaponActor();
	P_NATIVE_END;
}
// ********** End Class USpellRiseWeaponComponent Function GetEquippedWeaponActor ******************

// ********** Begin Class USpellRiseWeaponComponent Function HandleOwnerDeath **********************
struct Z_Construct_UFunction_USpellRiseWeaponComponent_HandleOwnerDeath_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Weapon" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Called by owning character on death (visual cleanup). */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Called by owning character on death (visual cleanup)." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function HandleOwnerDeath constinit property declarations **********************
// ********** End Function HandleOwnerDeath constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseWeaponComponent_HandleOwnerDeath_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseWeaponComponent, nullptr, "HandleOwnerDeath", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_HandleOwnerDeath_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseWeaponComponent_HandleOwnerDeath_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseWeaponComponent_HandleOwnerDeath()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseWeaponComponent_HandleOwnerDeath_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseWeaponComponent::execHandleOwnerDeath)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->HandleOwnerDeath();
	P_NATIVE_END;
}
// ********** End Class USpellRiseWeaponComponent Function HandleOwnerDeath ************************

// ********** Begin Class USpellRiseWeaponComponent Function OnRep_EquippedWeapon ******************
struct Z_Construct_UFunction_USpellRiseWeaponComponent_OnRep_EquippedWeapon_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnRep_EquippedWeapon constinit property declarations ******************
// ********** End Function OnRep_EquippedWeapon constinit property declarations ********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseWeaponComponent_OnRep_EquippedWeapon_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseWeaponComponent, nullptr, "OnRep_EquippedWeapon", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_OnRep_EquippedWeapon_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseWeaponComponent_OnRep_EquippedWeapon_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseWeaponComponent_OnRep_EquippedWeapon()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseWeaponComponent_OnRep_EquippedWeapon_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseWeaponComponent::execOnRep_EquippedWeapon)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnRep_EquippedWeapon();
	P_NATIVE_END;
}
// ********** End Class USpellRiseWeaponComponent Function OnRep_EquippedWeapon ********************

// ********** Begin Class USpellRiseWeaponComponent Function PerformLightAttack_TraceAndApplyDamage 
struct Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics
{
	struct SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms
	{
		AActor* InstigatorActor;
		TSubclassOf<UGameplayEffect> DamageGE;
		float BaseDamage;
		float DamageScaling;
		TEnumAsByte<ECollisionChannel> TraceChannel;
		bool bDebug;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Weapon" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n\x09 * Server-authoritative: Trace + Apply damage via GAS.\n\x09 * Safe to call on client (routes to server).\n\x09 *\n\x09 * IMPORTANT:\n\x09 * - InstigatorActor should be the attacking Character (Avatar).\n\x09 * - If a PlayerState/Controller is passed, we will resolve it to the Pawn.\n\x09 * - If null, we will use the component Owner as instigator.\n\x09 *\n\x09 * @param BaseDamage      Base damage to set (weapon base)\n\x09 * @param DamageScaling   Multiplier (combo/buffs). 1.0 = normal.\n\x09 */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Server-authoritative: Trace + Apply damage via GAS.\nSafe to call on client (routes to server).\n\nIMPORTANT:\n- InstigatorActor should be the attacking Character (Avatar).\n- If a PlayerState/Controller is passed, we will resolve it to the Pawn.\n- If null, we will use the component Owner as instigator.\n\n@param BaseDamage      Base damage to set (weapon base)\n@param DamageScaling   Multiplier (combo/buffs). 1.0 = normal." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function PerformLightAttack_TraceAndApplyDamage constinit property declarations 
	static const UECodeGen_Private::FObjectPropertyParams NewProp_InstigatorActor;
	static const UECodeGen_Private::FClassPropertyParams NewProp_DamageGE;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BaseDamage;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_DamageScaling;
	static const UECodeGen_Private::FBytePropertyParams NewProp_TraceChannel;
	static void NewProp_bDebug_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bDebug;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function PerformLightAttack_TraceAndApplyDamage constinit property declarations **
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function PerformLightAttack_TraceAndApplyDamage Property Definitions ***********
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_InstigatorActor = { "InstigatorActor", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms, InstigatorActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_DamageGE = { "DamageGE", nullptr, (EPropertyFlags)0x0014000000000080, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms, DamageGE), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_BaseDamage = { "BaseDamage", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms, BaseDamage), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_DamageScaling = { "DamageScaling", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms, DamageScaling), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_TraceChannel = { "TraceChannel", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms, TraceChannel), Z_Construct_UEnum_Engine_ECollisionChannel, METADATA_PARAMS(0, nullptr) }; // 838391399
void Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_bDebug_SetBit(void* Obj)
{
	((SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms*)Obj)->bDebug = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_bDebug = { "bDebug", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms), &Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_bDebug_SetBit, METADATA_PARAMS(0, nullptr) };
void Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms), &Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_InstigatorActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_DamageGE,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_BaseDamage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_DamageScaling,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_TraceChannel,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_bDebug,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::PropPointers) < 2048);
// ********** End Function PerformLightAttack_TraceAndApplyDamage Property Definitions *************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseWeaponComponent, nullptr, "PerformLightAttack_TraceAndApplyDamage", 	Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::SpellRiseWeaponComponent_eventPerformLightAttack_TraceAndApplyDamage_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseWeaponComponent::execPerformLightAttack_TraceAndApplyDamage)
{
	P_GET_OBJECT(AActor,Z_Param_InstigatorActor);
	P_GET_OBJECT(UClass,Z_Param_DamageGE);
	P_GET_PROPERTY(FFloatProperty,Z_Param_BaseDamage);
	P_GET_PROPERTY(FFloatProperty,Z_Param_DamageScaling);
	P_GET_PROPERTY(FByteProperty,Z_Param_TraceChannel);
	P_GET_UBOOL(Z_Param_bDebug);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->PerformLightAttack_TraceAndApplyDamage(Z_Param_InstigatorActor,Z_Param_DamageGE,Z_Param_BaseDamage,Z_Param_DamageScaling,ECollisionChannel(Z_Param_TraceChannel),Z_Param_bDebug);
	P_NATIVE_END;
}
// ********** End Class USpellRiseWeaponComponent Function PerformLightAttack_TraceAndApplyDamage **

// ********** Begin Class USpellRiseWeaponComponent Function PerformLightAttackTrace ***************
struct Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics
{
	struct SpellRiseWeaponComponent_eventPerformLightAttackTrace_Parms
	{
		AActor* InstigatorActor;
		TEnumAsByte<ECollisionChannel> TraceChannel;
		bool bDebug;
		FHitResult OutHit;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Weapon" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Performs a melee trace using the currently equipped weapon tuning (LightAttack). Pure trace only. */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Performs a melee trace using the currently equipped weapon tuning (LightAttack). Pure trace only." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function PerformLightAttackTrace constinit property declarations ***************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_InstigatorActor;
	static const UECodeGen_Private::FBytePropertyParams NewProp_TraceChannel;
	static void NewProp_bDebug_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bDebug;
	static const UECodeGen_Private::FStructPropertyParams NewProp_OutHit;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function PerformLightAttackTrace constinit property declarations *****************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function PerformLightAttackTrace Property Definitions **************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_InstigatorActor = { "InstigatorActor", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventPerformLightAttackTrace_Parms, InstigatorActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_TraceChannel = { "TraceChannel", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventPerformLightAttackTrace_Parms, TraceChannel), Z_Construct_UEnum_Engine_ECollisionChannel, METADATA_PARAMS(0, nullptr) }; // 838391399
void Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_bDebug_SetBit(void* Obj)
{
	((SpellRiseWeaponComponent_eventPerformLightAttackTrace_Parms*)Obj)->bDebug = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_bDebug = { "bDebug", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseWeaponComponent_eventPerformLightAttackTrace_Parms), &Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_bDebug_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_OutHit = { "OutHit", nullptr, (EPropertyFlags)0x0010008000000180, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventPerformLightAttackTrace_Parms, OutHit), Z_Construct_UScriptStruct_FHitResult, METADATA_PARAMS(0, nullptr) }; // 222120718
void Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((SpellRiseWeaponComponent_eventPerformLightAttackTrace_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseWeaponComponent_eventPerformLightAttackTrace_Parms), &Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_InstigatorActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_TraceChannel,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_bDebug,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_OutHit,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::PropPointers) < 2048);
// ********** End Function PerformLightAttackTrace Property Definitions ****************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseWeaponComponent, nullptr, "PerformLightAttackTrace", 	Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::SpellRiseWeaponComponent_eventPerformLightAttackTrace_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54420401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::SpellRiseWeaponComponent_eventPerformLightAttackTrace_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseWeaponComponent::execPerformLightAttackTrace)
{
	P_GET_OBJECT(AActor,Z_Param_InstigatorActor);
	P_GET_PROPERTY(FByteProperty,Z_Param_TraceChannel);
	P_GET_UBOOL(Z_Param_bDebug);
	P_GET_STRUCT_REF(FHitResult,Z_Param_Out_OutHit);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->PerformLightAttackTrace(Z_Param_InstigatorActor,ECollisionChannel(Z_Param_TraceChannel),Z_Param_bDebug,Z_Param_Out_OutHit);
	P_NATIVE_END;
}
// ********** End Class USpellRiseWeaponComponent Function PerformLightAttackTrace *****************

// ********** Begin Class USpellRiseWeaponComponent Function ServerEquipWeapon *********************
struct SpellRiseWeaponComponent_eventServerEquipWeapon_Parms
{
	UDA_WeaponDefinition* NewWeapon;
};
static FName NAME_USpellRiseWeaponComponent_ServerEquipWeapon = FName(TEXT("ServerEquipWeapon"));
void USpellRiseWeaponComponent::ServerEquipWeapon(UDA_WeaponDefinition* NewWeapon)
{
	SpellRiseWeaponComponent_eventServerEquipWeapon_Parms Parms;
	Parms.NewWeapon=NewWeapon;
	UFunction* Func = FindFunctionChecked(NAME_USpellRiseWeaponComponent_ServerEquipWeapon);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// ---------------------------\n// RPCs\n// ---------------------------\n" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "RPCs" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function ServerEquipWeapon constinit property declarations *********************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_NewWeapon;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function ServerEquipWeapon constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function ServerEquipWeapon Property Definitions ********************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon_Statics::NewProp_NewWeapon = { "NewWeapon", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventServerEquipWeapon_Parms, NewWeapon), Z_Construct_UClass_UDA_WeaponDefinition_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon_Statics::NewProp_NewWeapon,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon_Statics::PropPointers) < 2048);
// ********** End Function ServerEquipWeapon Property Definitions **********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseWeaponComponent, nullptr, "ServerEquipWeapon", 	Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon_Statics::PropPointers), 
sizeof(SpellRiseWeaponComponent_eventServerEquipWeapon_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00280CC0, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(SpellRiseWeaponComponent_eventServerEquipWeapon_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseWeaponComponent::execServerEquipWeapon)
{
	P_GET_OBJECT(UDA_WeaponDefinition,Z_Param_NewWeapon);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->ServerEquipWeapon_Implementation(Z_Param_NewWeapon);
	P_NATIVE_END;
}
// ********** End Class USpellRiseWeaponComponent Function ServerEquipWeapon ***********************

// ********** Begin Class USpellRiseWeaponComponent Function ServerPerformLightAttack_TraceAndApplyDamage 
struct SpellRiseWeaponComponent_eventServerPerformLightAttack_TraceAndApplyDamage_Parms
{
	AActor* InstigatorActor;
	TSubclassOf<UGameplayEffect> DamageGE;
	float BaseDamage;
	float DamageScaling;
	TEnumAsByte<ECollisionChannel> TraceChannel;
	bool bDebug;
};
static FName NAME_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage = FName(TEXT("ServerPerformLightAttack_TraceAndApplyDamage"));
void USpellRiseWeaponComponent::ServerPerformLightAttack_TraceAndApplyDamage(AActor* InstigatorActor, TSubclassOf<UGameplayEffect> DamageGE, float BaseDamage, float DamageScaling, ECollisionChannel TraceChannel, bool bDebug)
{
	SpellRiseWeaponComponent_eventServerPerformLightAttack_TraceAndApplyDamage_Parms Parms;
	Parms.InstigatorActor=InstigatorActor;
	Parms.DamageGE=DamageGE;
	Parms.BaseDamage=BaseDamage;
	Parms.DamageScaling=DamageScaling;
	Parms.TraceChannel=TraceChannel;
	Parms.bDebug=bDebug ? true : false;
	UFunction* Func = FindFunctionChecked(NAME_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** IMPORTANT: RPC must use ECollisionChannel (not TEnumAsByte) */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "IMPORTANT: RPC must use ECollisionChannel (not TEnumAsByte)" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function ServerPerformLightAttack_TraceAndApplyDamage constinit property declarations 
	static const UECodeGen_Private::FObjectPropertyParams NewProp_InstigatorActor;
	static const UECodeGen_Private::FClassPropertyParams NewProp_DamageGE;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BaseDamage;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_DamageScaling;
	static const UECodeGen_Private::FBytePropertyParams NewProp_TraceChannel;
	static void NewProp_bDebug_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bDebug;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function ServerPerformLightAttack_TraceAndApplyDamage constinit property declarations 
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function ServerPerformLightAttack_TraceAndApplyDamage Property Definitions *****
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_InstigatorActor = { "InstigatorActor", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventServerPerformLightAttack_TraceAndApplyDamage_Parms, InstigatorActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_DamageGE = { "DamageGE", nullptr, (EPropertyFlags)0x0014000000000080, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventServerPerformLightAttack_TraceAndApplyDamage_Parms, DamageGE), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_BaseDamage = { "BaseDamage", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventServerPerformLightAttack_TraceAndApplyDamage_Parms, BaseDamage), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_DamageScaling = { "DamageScaling", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventServerPerformLightAttack_TraceAndApplyDamage_Parms, DamageScaling), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_TraceChannel = { "TraceChannel", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseWeaponComponent_eventServerPerformLightAttack_TraceAndApplyDamage_Parms, TraceChannel), Z_Construct_UEnum_Engine_ECollisionChannel, METADATA_PARAMS(0, nullptr) }; // 838391399
void Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_bDebug_SetBit(void* Obj)
{
	((SpellRiseWeaponComponent_eventServerPerformLightAttack_TraceAndApplyDamage_Parms*)Obj)->bDebug = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_bDebug = { "bDebug", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseWeaponComponent_eventServerPerformLightAttack_TraceAndApplyDamage_Parms), &Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_bDebug_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_InstigatorActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_DamageGE,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_BaseDamage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_DamageScaling,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_TraceChannel,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::NewProp_bDebug,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::PropPointers) < 2048);
// ********** End Function ServerPerformLightAttack_TraceAndApplyDamage Property Definitions *******
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseWeaponComponent, nullptr, "ServerPerformLightAttack_TraceAndApplyDamage", 	Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::PropPointers), 
sizeof(SpellRiseWeaponComponent_eventServerPerformLightAttack_TraceAndApplyDamage_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00280CC0, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(SpellRiseWeaponComponent_eventServerPerformLightAttack_TraceAndApplyDamage_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseWeaponComponent::execServerPerformLightAttack_TraceAndApplyDamage)
{
	P_GET_OBJECT(AActor,Z_Param_InstigatorActor);
	P_GET_OBJECT(UClass,Z_Param_DamageGE);
	P_GET_PROPERTY(FFloatProperty,Z_Param_BaseDamage);
	P_GET_PROPERTY(FFloatProperty,Z_Param_DamageScaling);
	P_GET_PROPERTY(FByteProperty,Z_Param_TraceChannel);
	P_GET_UBOOL(Z_Param_bDebug);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->ServerPerformLightAttack_TraceAndApplyDamage_Implementation(Z_Param_InstigatorActor,Z_Param_DamageGE,Z_Param_BaseDamage,Z_Param_DamageScaling,ECollisionChannel(Z_Param_TraceChannel),Z_Param_bDebug);
	P_NATIVE_END;
}
// ********** End Class USpellRiseWeaponComponent Function ServerPerformLightAttack_TraceAndApplyDamage 

// ********** Begin Class USpellRiseWeaponComponent Function ServerUnequipWeapon *******************
static FName NAME_USpellRiseWeaponComponent_ServerUnequipWeapon = FName(TEXT("ServerUnequipWeapon"));
void USpellRiseWeaponComponent::ServerUnequipWeapon()
{
	UFunction* Func = FindFunctionChecked(NAME_USpellRiseWeaponComponent_ServerUnequipWeapon);
	ProcessEvent(Func,NULL);
}
struct Z_Construct_UFunction_USpellRiseWeaponComponent_ServerUnequipWeapon_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function ServerUnequipWeapon constinit property declarations *******************
// ********** End Function ServerUnequipWeapon constinit property declarations *********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseWeaponComponent_ServerUnequipWeapon_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseWeaponComponent, nullptr, "ServerUnequipWeapon", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00280CC0, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_ServerUnequipWeapon_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseWeaponComponent_ServerUnequipWeapon_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseWeaponComponent_ServerUnequipWeapon()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseWeaponComponent_ServerUnequipWeapon_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseWeaponComponent::execServerUnequipWeapon)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->ServerUnequipWeapon_Implementation();
	P_NATIVE_END;
}
// ********** End Class USpellRiseWeaponComponent Function ServerUnequipWeapon *********************

// ********** Begin Class USpellRiseWeaponComponent Function UnequipWeapon *************************
struct Z_Construct_UFunction_USpellRiseWeaponComponent_UnequipWeapon_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Weapon" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Unequip weapon (safe to call on client; will route to server). */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Unequip weapon (safe to call on client; will route to server)." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function UnequipWeapon constinit property declarations *************************
// ********** End Function UnequipWeapon constinit property declarations ***************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseWeaponComponent_UnequipWeapon_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseWeaponComponent, nullptr, "UnequipWeapon", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseWeaponComponent_UnequipWeapon_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseWeaponComponent_UnequipWeapon_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseWeaponComponent_UnequipWeapon()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseWeaponComponent_UnequipWeapon_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseWeaponComponent::execUnequipWeapon)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->UnequipWeapon();
	P_NATIVE_END;
}
// ********** End Class USpellRiseWeaponComponent Function UnequipWeapon ***************************

// ********** Begin Class USpellRiseWeaponComponent ************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_USpellRiseWeaponComponent;
UClass* USpellRiseWeaponComponent::GetPrivateStaticClass()
{
	using TClass = USpellRiseWeaponComponent;
	if (!Z_Registration_Info_UClass_USpellRiseWeaponComponent.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRiseWeaponComponent"),
			Z_Registration_Info_UClass_USpellRiseWeaponComponent.InnerSingleton,
			StaticRegisterNativesUSpellRiseWeaponComponent,
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
	return Z_Registration_Info_UClass_USpellRiseWeaponComponent.InnerSingleton;
}
UClass* Z_Construct_UClass_USpellRiseWeaponComponent_NoRegister()
{
	return USpellRiseWeaponComponent::GetPrivateStaticClass();
}
struct Z_Construct_UClass_USpellRiseWeaponComponent_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "BlueprintType", "true" },
		{ "ClassGroupNames", "SpellRise" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Server-authoritative weapon equip component.\n * Replicates the equipped weapon definition (DataAsset).\n * Spawns/attaches a WeaponActor for visuals on both server and clients.\n *\n * AAA Damage Tagging:\n * - NEW (preferred):\n *   - Data.BaseWeaponDamage  (weapon/melee/ranged physical)\n *   - Data.DamageScaling     (generic multiplier)\n * - LEGACY (fallback support):\n *   - Data.BaseDamage\n *   - Data.DamageMultiplier\n *\n * We set BOTH where possible to keep older assets working during migration.\n */" },
#endif
		{ "IncludePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Server-authoritative weapon equip component.\nReplicates the equipped weapon definition (DataAsset).\nSpawns/attaches a WeaponActor for visuals on both server and clients.\n\nAAA Damage Tagging:\n- NEW (preferred):\n  - Data.BaseWeaponDamage  (weapon/melee/ranged physical)\n  - Data.DamageScaling     (generic multiplier)\n- LEGACY (fallback support):\n  - Data.BaseDamage\n  - Data.DamageMultiplier\n\nWe set BOTH where possible to keep older assets working during migration." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EquippedWeapon_MetaData[] = {
		{ "Category", "SpellRise|Weapon" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Replicated: data definition of equipped weapon */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Replicated: data definition of equipped weapon" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EquippedWeaponVisualActor_MetaData[] = {
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EquippedWeaponActor_MetaData[] = {
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bRebuildVisualOnServer_MetaData[] = {
		{ "Category", "SpellRise|Weapon" },
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SetByCallerBaseWeaponDamageTag_MetaData[] = {
		{ "Category", "SpellRise|Weapon|Damage" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** NEW (AAA): Base weapon damage tag used by ExecCalc_Damage. */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "NEW (AAA): Base weapon damage tag used by ExecCalc_Damage." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SetByCallerDamageScalingTag_MetaData[] = {
		{ "Category", "SpellRise|Weapon|Damage" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** NEW (AAA): Generic scaling multiplier tag (combo/buffs). */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "NEW (AAA): Generic scaling multiplier tag (combo/buffs)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SetByCallerBaseDamageTag_Legacy_MetaData[] = {
		{ "Category", "SpellRise|Weapon|Damage" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** LEGACY: Old base damage tag kept for backward compatibility with older GEs. */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "LEGACY: Old base damage tag kept for backward compatibility with older GEs." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SetByCallerDamageMultiplierTag_Legacy_MetaData[] = {
		{ "Category", "SpellRise|Weapon|Damage" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** LEGACY: Old multiplier tag kept for backward compatibility with older GEs. */" },
#endif
		{ "ModuleRelativePath", "Weapons/Components/SpellRiseWeaponComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "LEGACY: Old multiplier tag kept for backward compatibility with older GEs." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class USpellRiseWeaponComponent constinit property declarations ****************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_EquippedWeapon;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_EquippedWeaponVisualActor;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_EquippedWeaponActor;
	static void NewProp_bRebuildVisualOnServer_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bRebuildVisualOnServer;
	static const UECodeGen_Private::FStructPropertyParams NewProp_SetByCallerBaseWeaponDamageTag;
	static const UECodeGen_Private::FStructPropertyParams NewProp_SetByCallerDamageScalingTag;
	static const UECodeGen_Private::FStructPropertyParams NewProp_SetByCallerBaseDamageTag_Legacy;
	static const UECodeGen_Private::FStructPropertyParams NewProp_SetByCallerDamageMultiplierTag_Legacy;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class USpellRiseWeaponComponent constinit property declarations ******************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("EquipWeapon"), .Pointer = &USpellRiseWeaponComponent::execEquipWeapon },
		{ .NameUTF8 = UTF8TEXT("GetEquippedWeapon"), .Pointer = &USpellRiseWeaponComponent::execGetEquippedWeapon },
		{ .NameUTF8 = UTF8TEXT("GetEquippedWeaponActor"), .Pointer = &USpellRiseWeaponComponent::execGetEquippedWeaponActor },
		{ .NameUTF8 = UTF8TEXT("HandleOwnerDeath"), .Pointer = &USpellRiseWeaponComponent::execHandleOwnerDeath },
		{ .NameUTF8 = UTF8TEXT("OnRep_EquippedWeapon"), .Pointer = &USpellRiseWeaponComponent::execOnRep_EquippedWeapon },
		{ .NameUTF8 = UTF8TEXT("PerformLightAttack_TraceAndApplyDamage"), .Pointer = &USpellRiseWeaponComponent::execPerformLightAttack_TraceAndApplyDamage },
		{ .NameUTF8 = UTF8TEXT("PerformLightAttackTrace"), .Pointer = &USpellRiseWeaponComponent::execPerformLightAttackTrace },
		{ .NameUTF8 = UTF8TEXT("ServerEquipWeapon"), .Pointer = &USpellRiseWeaponComponent::execServerEquipWeapon },
		{ .NameUTF8 = UTF8TEXT("ServerPerformLightAttack_TraceAndApplyDamage"), .Pointer = &USpellRiseWeaponComponent::execServerPerformLightAttack_TraceAndApplyDamage },
		{ .NameUTF8 = UTF8TEXT("ServerUnequipWeapon"), .Pointer = &USpellRiseWeaponComponent::execServerUnequipWeapon },
		{ .NameUTF8 = UTF8TEXT("UnequipWeapon"), .Pointer = &USpellRiseWeaponComponent::execUnequipWeapon },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_USpellRiseWeaponComponent_EquipWeapon, "EquipWeapon" }, // 1624749358
		{ &Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeapon, "GetEquippedWeapon" }, // 427439561
		{ &Z_Construct_UFunction_USpellRiseWeaponComponent_GetEquippedWeaponActor, "GetEquippedWeaponActor" }, // 2503986223
		{ &Z_Construct_UFunction_USpellRiseWeaponComponent_HandleOwnerDeath, "HandleOwnerDeath" }, // 4205996335
		{ &Z_Construct_UFunction_USpellRiseWeaponComponent_OnRep_EquippedWeapon, "OnRep_EquippedWeapon" }, // 614349194
		{ &Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttack_TraceAndApplyDamage, "PerformLightAttack_TraceAndApplyDamage" }, // 2235214138
		{ &Z_Construct_UFunction_USpellRiseWeaponComponent_PerformLightAttackTrace, "PerformLightAttackTrace" }, // 1187995398
		{ &Z_Construct_UFunction_USpellRiseWeaponComponent_ServerEquipWeapon, "ServerEquipWeapon" }, // 2489683446
		{ &Z_Construct_UFunction_USpellRiseWeaponComponent_ServerPerformLightAttack_TraceAndApplyDamage, "ServerPerformLightAttack_TraceAndApplyDamage" }, // 1546033789
		{ &Z_Construct_UFunction_USpellRiseWeaponComponent_ServerUnequipWeapon, "ServerUnequipWeapon" }, // 1053890057
		{ &Z_Construct_UFunction_USpellRiseWeaponComponent_UnequipWeapon, "UnequipWeapon" }, // 656232438
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USpellRiseWeaponComponent>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_USpellRiseWeaponComponent_Statics

// ********** Begin Class USpellRiseWeaponComponent Property Definitions ***************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_EquippedWeapon = { "EquippedWeapon", "OnRep_EquippedWeapon", (EPropertyFlags)0x0124080100010035, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseWeaponComponent, EquippedWeapon), Z_Construct_UClass_UDA_WeaponDefinition_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EquippedWeapon_MetaData), NewProp_EquippedWeapon_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_EquippedWeaponVisualActor = { "EquippedWeaponVisualActor", nullptr, (EPropertyFlags)0x0124080000002000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseWeaponComponent, EquippedWeaponVisualActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EquippedWeaponVisualActor_MetaData), NewProp_EquippedWeaponVisualActor_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_EquippedWeaponActor = { "EquippedWeaponActor", nullptr, (EPropertyFlags)0x0124080000002000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseWeaponComponent, EquippedWeaponActor), Z_Construct_UClass_ASpellRiseWeaponActor_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EquippedWeaponActor_MetaData), NewProp_EquippedWeaponActor_MetaData) };
void Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_bRebuildVisualOnServer_SetBit(void* Obj)
{
	((USpellRiseWeaponComponent*)Obj)->bRebuildVisualOnServer = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_bRebuildVisualOnServer = { "bRebuildVisualOnServer", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(USpellRiseWeaponComponent), &Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_bRebuildVisualOnServer_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bRebuildVisualOnServer_MetaData), NewProp_bRebuildVisualOnServer_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_SetByCallerBaseWeaponDamageTag = { "SetByCallerBaseWeaponDamageTag", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseWeaponComponent, SetByCallerBaseWeaponDamageTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SetByCallerBaseWeaponDamageTag_MetaData), NewProp_SetByCallerBaseWeaponDamageTag_MetaData) }; // 517357616
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_SetByCallerDamageScalingTag = { "SetByCallerDamageScalingTag", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseWeaponComponent, SetByCallerDamageScalingTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SetByCallerDamageScalingTag_MetaData), NewProp_SetByCallerDamageScalingTag_MetaData) }; // 517357616
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_SetByCallerBaseDamageTag_Legacy = { "SetByCallerBaseDamageTag_Legacy", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseWeaponComponent, SetByCallerBaseDamageTag_Legacy), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SetByCallerBaseDamageTag_Legacy_MetaData), NewProp_SetByCallerBaseDamageTag_Legacy_MetaData) }; // 517357616
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_SetByCallerDamageMultiplierTag_Legacy = { "SetByCallerDamageMultiplierTag_Legacy", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseWeaponComponent, SetByCallerDamageMultiplierTag_Legacy), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SetByCallerDamageMultiplierTag_Legacy_MetaData), NewProp_SetByCallerDamageMultiplierTag_Legacy_MetaData) }; // 517357616
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_USpellRiseWeaponComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_EquippedWeapon,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_EquippedWeaponVisualActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_EquippedWeaponActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_bRebuildVisualOnServer,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_SetByCallerBaseWeaponDamageTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_SetByCallerDamageScalingTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_SetByCallerBaseDamageTag_Legacy,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseWeaponComponent_Statics::NewProp_SetByCallerDamageMultiplierTag_Legacy,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseWeaponComponent_Statics::PropPointers) < 2048);
// ********** End Class USpellRiseWeaponComponent Property Definitions *****************************
UObject* (*const Z_Construct_UClass_USpellRiseWeaponComponent_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UActorComponent,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseWeaponComponent_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USpellRiseWeaponComponent_Statics::ClassParams = {
	&USpellRiseWeaponComponent::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_USpellRiseWeaponComponent_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseWeaponComponent_Statics::PropPointers),
	0,
	0x00B000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseWeaponComponent_Statics::Class_MetaDataParams), Z_Construct_UClass_USpellRiseWeaponComponent_Statics::Class_MetaDataParams)
};
void USpellRiseWeaponComponent::StaticRegisterNativesUSpellRiseWeaponComponent()
{
	UClass* Class = USpellRiseWeaponComponent::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_USpellRiseWeaponComponent_Statics::Funcs));
}
UClass* Z_Construct_UClass_USpellRiseWeaponComponent()
{
	if (!Z_Registration_Info_UClass_USpellRiseWeaponComponent.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USpellRiseWeaponComponent.OuterSingleton, Z_Construct_UClass_USpellRiseWeaponComponent_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USpellRiseWeaponComponent.OuterSingleton;
}
#if VALIDATE_CLASS_REPS
void USpellRiseWeaponComponent::ValidateGeneratedRepEnums(const TArray<struct FRepRecord>& ClassReps) const
{
	static FName Name_EquippedWeapon(TEXT("EquippedWeapon"));
	const bool bIsValid = true
		&& Name_EquippedWeapon == ClassReps[(int32)ENetFields_Private::EquippedWeapon].Property->GetFName();
	checkf(bIsValid, TEXT("UHT Generated Rep Indices do not match runtime populated Rep Indices for properties in USpellRiseWeaponComponent"));
}
#endif
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, USpellRiseWeaponComponent);
USpellRiseWeaponComponent::~USpellRiseWeaponComponent() {}
// ********** End Class USpellRiseWeaponComponent **************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USpellRiseWeaponComponent, USpellRiseWeaponComponent::StaticClass, TEXT("USpellRiseWeaponComponent"), &Z_Registration_Info_UClass_USpellRiseWeaponComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USpellRiseWeaponComponent), 3021703273U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h__Script_SpellRise_2825888708{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
