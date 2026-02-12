// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSpellRiseAbilitySystemComponent() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UClass_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilitySystemComponent();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_NoRegister();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilitySpec();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseAbilitySystemComponent();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseAbilitySystemComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class USpellRiseAbilitySystemComponent Function SR_AbilityInputPressed *********
struct Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics
{
	struct SpellRiseAbilitySystemComponent_eventSR_AbilityInputPressed_Parms
	{
		int32 InputID;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|GAS|Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// Input public API\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Input public API" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function SR_AbilityInputPressed constinit property declarations ****************
	static const UECodeGen_Private::FIntPropertyParams NewProp_InputID;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function SR_AbilityInputPressed constinit property declarations ******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function SR_AbilityInputPressed Property Definitions ***************************
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::NewProp_InputID = { "InputID", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseAbilitySystemComponent_eventSR_AbilityInputPressed_Parms, InputID), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::NewProp_InputID,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::PropPointers) < 2048);
// ********** End Function SR_AbilityInputPressed Property Definitions *****************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseAbilitySystemComponent, nullptr, "SR_AbilityInputPressed", 	Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::SpellRiseAbilitySystemComponent_eventSR_AbilityInputPressed_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::SpellRiseAbilitySystemComponent_eventSR_AbilityInputPressed_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseAbilitySystemComponent::execSR_AbilityInputPressed)
{
	P_GET_PROPERTY(FIntProperty,Z_Param_InputID);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SR_AbilityInputPressed(Z_Param_InputID);
	P_NATIVE_END;
}
// ********** End Class USpellRiseAbilitySystemComponent Function SR_AbilityInputPressed ***********

// ********** Begin Class USpellRiseAbilitySystemComponent Function SR_AbilityInputReleased ********
struct Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics
{
	struct SpellRiseAbilitySystemComponent_eventSR_AbilityInputReleased_Parms
	{
		int32 InputID;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|GAS|Input" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function SR_AbilityInputReleased constinit property declarations ***************
	static const UECodeGen_Private::FIntPropertyParams NewProp_InputID;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function SR_AbilityInputReleased constinit property declarations *****************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function SR_AbilityInputReleased Property Definitions **************************
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::NewProp_InputID = { "InputID", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseAbilitySystemComponent_eventSR_AbilityInputReleased_Parms, InputID), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::NewProp_InputID,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::PropPointers) < 2048);
// ********** End Function SR_AbilityInputReleased Property Definitions ****************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseAbilitySystemComponent, nullptr, "SR_AbilityInputReleased", 	Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::SpellRiseAbilitySystemComponent_eventSR_AbilityInputReleased_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::SpellRiseAbilitySystemComponent_eventSR_AbilityInputReleased_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseAbilitySystemComponent::execSR_AbilityInputReleased)
{
	P_GET_PROPERTY(FIntProperty,Z_Param_InputID);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SR_AbilityInputReleased(Z_Param_InputID);
	P_NATIVE_END;
}
// ********** End Class USpellRiseAbilitySystemComponent Function SR_AbilityInputReleased **********

// ********** Begin Class USpellRiseAbilitySystemComponent Function SR_ClearAbilityInput ***********
struct Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ClearAbilityInput_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|GAS|Input" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function SR_ClearAbilityInput constinit property declarations ******************
// ********** End Function SR_ClearAbilityInput constinit property declarations ********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ClearAbilityInput_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseAbilitySystemComponent, nullptr, "SR_ClearAbilityInput", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ClearAbilityInput_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ClearAbilityInput_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ClearAbilityInput()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ClearAbilityInput_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseAbilitySystemComponent::execSR_ClearAbilityInput)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SR_ClearAbilityInput();
	P_NATIVE_END;
}
// ********** End Class USpellRiseAbilitySystemComponent Function SR_ClearAbilityInput *************

// ********** Begin Class USpellRiseAbilitySystemComponent Function SR_ProcessAbilityInput *********
struct Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics
{
	struct SpellRiseAbilitySystemComponent_eventSR_ProcessAbilityInput_Parms
	{
		float DeltaTime;
		bool bGamePaused;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|GAS|Input" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function SR_ProcessAbilityInput constinit property declarations ****************
	static const UECodeGen_Private::FFloatPropertyParams NewProp_DeltaTime;
	static void NewProp_bGamePaused_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bGamePaused;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function SR_ProcessAbilityInput constinit property declarations ******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function SR_ProcessAbilityInput Property Definitions ***************************
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::NewProp_DeltaTime = { "DeltaTime", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseAbilitySystemComponent_eventSR_ProcessAbilityInput_Parms, DeltaTime), METADATA_PARAMS(0, nullptr) };
void Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::NewProp_bGamePaused_SetBit(void* Obj)
{
	((SpellRiseAbilitySystemComponent_eventSR_ProcessAbilityInput_Parms*)Obj)->bGamePaused = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::NewProp_bGamePaused = { "bGamePaused", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseAbilitySystemComponent_eventSR_ProcessAbilityInput_Parms), &Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::NewProp_bGamePaused_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::NewProp_DeltaTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::NewProp_bGamePaused,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::PropPointers) < 2048);
// ********** End Function SR_ProcessAbilityInput Property Definitions *****************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseAbilitySystemComponent, nullptr, "SR_ProcessAbilityInput", 	Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::SpellRiseAbilitySystemComponent_eventSR_ProcessAbilityInput_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::SpellRiseAbilitySystemComponent_eventSR_ProcessAbilityInput_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseAbilitySystemComponent::execSR_ProcessAbilityInput)
{
	P_GET_PROPERTY(FFloatProperty,Z_Param_DeltaTime);
	P_GET_UBOOL(Z_Param_bGamePaused);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SR_ProcessAbilityInput(Z_Param_DeltaTime,Z_Param_bGamePaused);
	P_NATIVE_END;
}
// ********** End Class USpellRiseAbilitySystemComponent Function SR_ProcessAbilityInput ***********

// ********** Begin Class USpellRiseAbilitySystemComponent Function TryAddCatalystCharge_OnDamageTaken 
struct Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics
{
	struct SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnDamageTaken_Parms
	{
		AActor* InstigatorActor;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Catalyst" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Adiciona charge quando toma dano real. (SERVER ONLY) (para a v\xc3\xadtima) */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Adiciona charge quando toma dano real. (SERVER ONLY) (para a v\xc3\xadtima)" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function TryAddCatalystCharge_OnDamageTaken constinit property declarations ****
	static const UECodeGen_Private::FObjectPropertyParams NewProp_InstigatorActor;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function TryAddCatalystCharge_OnDamageTaken constinit property declarations ******
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function TryAddCatalystCharge_OnDamageTaken Property Definitions ***************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::NewProp_InstigatorActor = { "InstigatorActor", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnDamageTaken_Parms, InstigatorActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
void Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnDamageTaken_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnDamageTaken_Parms), &Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::NewProp_InstigatorActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::PropPointers) < 2048);
// ********** End Function TryAddCatalystCharge_OnDamageTaken Property Definitions *****************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseAbilitySystemComponent, nullptr, "TryAddCatalystCharge_OnDamageTaken", 	Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnDamageTaken_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnDamageTaken_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseAbilitySystemComponent::execTryAddCatalystCharge_OnDamageTaken)
{
	P_GET_OBJECT(AActor,Z_Param_InstigatorActor);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->TryAddCatalystCharge_OnDamageTaken(Z_Param_InstigatorActor);
	P_NATIVE_END;
}
// ********** End Class USpellRiseAbilitySystemComponent Function TryAddCatalystCharge_OnDamageTaken 

// ********** Begin Class USpellRiseAbilitySystemComponent Function TryAddCatalystCharge_OnValidHit 
struct Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics
{
	struct SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnValidHit_Parms
	{
		AActor* TargetActor;
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Catalyst" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Adiciona charge quando o hit for v\xc3\xa1lido. (SERVER ONLY) (para o agressor) */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Adiciona charge quando o hit for v\xc3\xa1lido. (SERVER ONLY) (para o agressor)" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function TryAddCatalystCharge_OnValidHit constinit property declarations *******
	static const UECodeGen_Private::FObjectPropertyParams NewProp_TargetActor;
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function TryAddCatalystCharge_OnValidHit constinit property declarations *********
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function TryAddCatalystCharge_OnValidHit Property Definitions ******************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::NewProp_TargetActor = { "TargetActor", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnValidHit_Parms, TargetActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
void Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnValidHit_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnValidHit_Parms), &Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::NewProp_TargetActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::PropPointers) < 2048);
// ********** End Function TryAddCatalystCharge_OnValidHit Property Definitions ********************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseAbilitySystemComponent, nullptr, "TryAddCatalystCharge_OnValidHit", 	Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnValidHit_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::SpellRiseAbilitySystemComponent_eventTryAddCatalystCharge_OnValidHit_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseAbilitySystemComponent::execTryAddCatalystCharge_OnValidHit)
{
	P_GET_OBJECT(AActor,Z_Param_TargetActor);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->TryAddCatalystCharge_OnValidHit(Z_Param_TargetActor);
	P_NATIVE_END;
}
// ********** End Class USpellRiseAbilitySystemComponent Function TryAddCatalystCharge_OnValidHit **

// ********** Begin Class USpellRiseAbilitySystemComponent Function TryProcCatalystIfReady *********
struct Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryProcCatalystIfReady_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Catalyst" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Tenta proc se estiver pronto. (SERVER ONLY) */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Tenta proc se estiver pronto. (SERVER ONLY)" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function TryProcCatalystIfReady constinit property declarations ****************
// ********** End Function TryProcCatalystIfReady constinit property declarations ******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryProcCatalystIfReady_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseAbilitySystemComponent, nullptr, "TryProcCatalystIfReady", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryProcCatalystIfReady_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryProcCatalystIfReady_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryProcCatalystIfReady()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryProcCatalystIfReady_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseAbilitySystemComponent::execTryProcCatalystIfReady)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->TryProcCatalystIfReady();
	P_NATIVE_END;
}
// ********** End Class USpellRiseAbilitySystemComponent Function TryProcCatalystIfReady ***********

// ********** Begin Class USpellRiseAbilitySystemComponent *****************************************
FClassRegistrationInfo Z_Registration_Info_UClass_USpellRiseAbilitySystemComponent;
UClass* USpellRiseAbilitySystemComponent::GetPrivateStaticClass()
{
	using TClass = USpellRiseAbilitySystemComponent;
	if (!Z_Registration_Info_UClass_USpellRiseAbilitySystemComponent.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRiseAbilitySystemComponent"),
			Z_Registration_Info_UClass_USpellRiseAbilitySystemComponent.InnerSingleton,
			StaticRegisterNativesUSpellRiseAbilitySystemComponent,
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
	return Z_Registration_Info_UClass_USpellRiseAbilitySystemComponent.InnerSingleton;
}
UClass* Z_Construct_UClass_USpellRiseAbilitySystemComponent_NoRegister()
{
	return USpellRiseAbilitySystemComponent::GetPrivateStaticClass();
}
struct Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "Custom" },
		{ "HideCategories", "Object LOD Lighting Transform Sockets TextureStreaming Object LOD Lighting Transform Sockets TextureStreaming" },
		{ "IncludePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LastActivatableAbilities_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Cache para detectar mudan\xc3\xa7""as em abilities replicadas e disparar evento no Character\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Cache para detectar mudan\xc3\xa7""as em abilities replicadas e disparar evento no Character" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InputPressedSpecHandles_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// Input pipeline (custom, like Lyra)\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Input pipeline (custom, like Lyra)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InputReleasedSpecHandles_MetaData[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InputHeldSpecHandles_MetaData[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_CatalystAddCharge_MetaData[] = {
		{ "Category", "SpellRise|Catalyst" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// Catalyst (\xc3\xbanico dono da mec\xc3\xa2nica)\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Catalyst (\xc3\xbanico dono da mec\xc3\xa2nica)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CatalystProcAbilityClass_MetaData[] = {
		{ "Category", "SpellRise|Catalyst" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Habilidade que faz o proc (deve aplicar State.Catalyst.Active e consumir charge). */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Habilidade que faz o proc (deve aplicar State.Catalyst.Active e consumir charge)." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class USpellRiseAbilitySystemComponent constinit property declarations *********
	static const UECodeGen_Private::FStructPropertyParams NewProp_LastActivatableAbilities_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_LastActivatableAbilities;
	static const UECodeGen_Private::FStructPropertyParams NewProp_InputPressedSpecHandles_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_InputPressedSpecHandles;
	static const UECodeGen_Private::FStructPropertyParams NewProp_InputReleasedSpecHandles_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_InputReleasedSpecHandles;
	static const UECodeGen_Private::FStructPropertyParams NewProp_InputHeldSpecHandles_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_InputHeldSpecHandles;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_CatalystAddCharge;
	static const UECodeGen_Private::FClassPropertyParams NewProp_CatalystProcAbilityClass;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class USpellRiseAbilitySystemComponent constinit property declarations ***********
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("SR_AbilityInputPressed"), .Pointer = &USpellRiseAbilitySystemComponent::execSR_AbilityInputPressed },
		{ .NameUTF8 = UTF8TEXT("SR_AbilityInputReleased"), .Pointer = &USpellRiseAbilitySystemComponent::execSR_AbilityInputReleased },
		{ .NameUTF8 = UTF8TEXT("SR_ClearAbilityInput"), .Pointer = &USpellRiseAbilitySystemComponent::execSR_ClearAbilityInput },
		{ .NameUTF8 = UTF8TEXT("SR_ProcessAbilityInput"), .Pointer = &USpellRiseAbilitySystemComponent::execSR_ProcessAbilityInput },
		{ .NameUTF8 = UTF8TEXT("TryAddCatalystCharge_OnDamageTaken"), .Pointer = &USpellRiseAbilitySystemComponent::execTryAddCatalystCharge_OnDamageTaken },
		{ .NameUTF8 = UTF8TEXT("TryAddCatalystCharge_OnValidHit"), .Pointer = &USpellRiseAbilitySystemComponent::execTryAddCatalystCharge_OnValidHit },
		{ .NameUTF8 = UTF8TEXT("TryProcCatalystIfReady"), .Pointer = &USpellRiseAbilitySystemComponent::execTryProcCatalystIfReady },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputPressed, "SR_AbilityInputPressed" }, // 2528480616
		{ &Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_AbilityInputReleased, "SR_AbilityInputReleased" }, // 3948682962
		{ &Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ClearAbilityInput, "SR_ClearAbilityInput" }, // 963281842
		{ &Z_Construct_UFunction_USpellRiseAbilitySystemComponent_SR_ProcessAbilityInput, "SR_ProcessAbilityInput" }, // 3918194405
		{ &Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnDamageTaken, "TryAddCatalystCharge_OnDamageTaken" }, // 2676702728
		{ &Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryAddCatalystCharge_OnValidHit, "TryAddCatalystCharge_OnValidHit" }, // 2038649974
		{ &Z_Construct_UFunction_USpellRiseAbilitySystemComponent_TryProcCatalystIfReady, "TryProcCatalystIfReady" }, // 511643324
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USpellRiseAbilitySystemComponent>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics

// ********** Begin Class USpellRiseAbilitySystemComponent Property Definitions ********************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_LastActivatableAbilities_Inner = { "LastActivatableAbilities", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGameplayAbilitySpec, METADATA_PARAMS(0, nullptr) }; // 135524981
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_LastActivatableAbilities = { "LastActivatableAbilities", nullptr, (EPropertyFlags)0x0020080000002000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseAbilitySystemComponent, LastActivatableAbilities), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LastActivatableAbilities_MetaData), NewProp_LastActivatableAbilities_MetaData) }; // 135524981
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputPressedSpecHandles_Inner = { "InputPressedSpecHandles", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle, METADATA_PARAMS(0, nullptr) }; // 224356909
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputPressedSpecHandles = { "InputPressedSpecHandles", nullptr, (EPropertyFlags)0x0020080000002000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseAbilitySystemComponent, InputPressedSpecHandles), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InputPressedSpecHandles_MetaData), NewProp_InputPressedSpecHandles_MetaData) }; // 224356909
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputReleasedSpecHandles_Inner = { "InputReleasedSpecHandles", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle, METADATA_PARAMS(0, nullptr) }; // 224356909
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputReleasedSpecHandles = { "InputReleasedSpecHandles", nullptr, (EPropertyFlags)0x0020080000002000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseAbilitySystemComponent, InputReleasedSpecHandles), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InputReleasedSpecHandles_MetaData), NewProp_InputReleasedSpecHandles_MetaData) }; // 224356909
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputHeldSpecHandles_Inner = { "InputHeldSpecHandles", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle, METADATA_PARAMS(0, nullptr) }; // 224356909
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputHeldSpecHandles = { "InputHeldSpecHandles", nullptr, (EPropertyFlags)0x0020080000002000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseAbilitySystemComponent, InputHeldSpecHandles), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InputHeldSpecHandles_MetaData), NewProp_InputHeldSpecHandles_MetaData) }; // 224356909
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_GE_CatalystAddCharge = { "GE_CatalystAddCharge", nullptr, (EPropertyFlags)0x0014000000010001, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseAbilitySystemComponent, GE_CatalystAddCharge), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_CatalystAddCharge_MetaData), NewProp_GE_CatalystAddCharge_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_CatalystProcAbilityClass = { "CatalystProcAbilityClass", nullptr, (EPropertyFlags)0x0014000000010001, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseAbilitySystemComponent, CatalystProcAbilityClass), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayAbility_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CatalystProcAbilityClass_MetaData), NewProp_CatalystProcAbilityClass_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_LastActivatableAbilities_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_LastActivatableAbilities,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputPressedSpecHandles_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputPressedSpecHandles,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputReleasedSpecHandles_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputReleasedSpecHandles,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputHeldSpecHandles_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_InputHeldSpecHandles,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_GE_CatalystAddCharge,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::NewProp_CatalystProcAbilityClass,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::PropPointers) < 2048);
// ********** End Class USpellRiseAbilitySystemComponent Property Definitions **********************
UObject* (*const Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAbilitySystemComponent,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::ClassParams = {
	&USpellRiseAbilitySystemComponent::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::PropPointers),
	0,
	0x00B010A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::Class_MetaDataParams), Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::Class_MetaDataParams)
};
void USpellRiseAbilitySystemComponent::StaticRegisterNativesUSpellRiseAbilitySystemComponent()
{
	UClass* Class = USpellRiseAbilitySystemComponent::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::Funcs));
}
UClass* Z_Construct_UClass_USpellRiseAbilitySystemComponent()
{
	if (!Z_Registration_Info_UClass_USpellRiseAbilitySystemComponent.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USpellRiseAbilitySystemComponent.OuterSingleton, Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USpellRiseAbilitySystemComponent.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, USpellRiseAbilitySystemComponent);
USpellRiseAbilitySystemComponent::~USpellRiseAbilitySystemComponent() {}
// ********** End Class USpellRiseAbilitySystemComponent *******************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USpellRiseAbilitySystemComponent, USpellRiseAbilitySystemComponent::StaticClass, TEXT("USpellRiseAbilitySystemComponent"), &Z_Registration_Info_UClass_USpellRiseAbilitySystemComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USpellRiseAbilitySystemComponent), 3562481026U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h__Script_SpellRise_115885400{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
