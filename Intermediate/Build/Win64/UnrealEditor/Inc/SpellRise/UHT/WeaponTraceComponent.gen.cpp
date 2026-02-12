// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Combat/Melee/Trace/WeaponTraceComponent.h"
#include "GameplayTagContainer.h"
#include "UObject/Class.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeWeaponTraceComponent() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UClass_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UActorComponent();
ENGINE_API UClass* Z_Construct_UClass_USceneComponent_NoRegister();
ENGINE_API UEnum* Z_Construct_UEnum_Engine_ECollisionChannel();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilitySystemComponent_NoRegister();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
SPELLRISE_API UClass* Z_Construct_UClass_UWeaponTraceComponent();
SPELLRISE_API UClass* Z_Construct_UClass_UWeaponTraceComponent_NoRegister();
SPELLRISE_API UScriptStruct* Z_Construct_UScriptStruct_FWeaponTraceConfig();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin ScriptStruct FWeaponTraceConfig ************************************************
struct Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics
{
	static inline consteval int32 GetStructSize() { return sizeof(FWeaponTraceConfig); }
	static inline consteval int16 GetStructAlignment() { return alignof(FWeaponTraceConfig); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WeaponComponent_MetaData[] = {
		{ "Category", "WeaponTrace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Componente que cont\xc3\xa9m os sockets (geralmente o SkeletalMesh da arma)\n" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Componente que cont\xc3\xa9m os sockets (geralmente o SkeletalMesh da arma)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SocketBase_MetaData[] = {
		{ "Category", "WeaponTrace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Sockets usados para sweep (ex: BladeBase / BladeTip)\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Sockets usados para sweep (ex: BladeBase / BladeTip)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SocketTip_MetaData[] = {
		{ "Category", "WeaponTrace" },
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Radius_MetaData[] = {
		{ "Category", "WeaponTrace" },
		{ "ClampMin", "1.0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Raio do sweep (capsule/sphere)\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Raio do sweep (capsule/sphere)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TraceChannel_MetaData[] = {
		{ "Category", "WeaponTrace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Canal de colis\xc3\xa3o\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Canal de colis\xc3\xa3o" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bDrawDebug_MetaData[] = {
		{ "Category", "WeaponTrace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Desenhar debug\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Desenhar debug" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DebugLifeTime_MetaData[] = {
		{ "Category", "WeaponTrace" },
		{ "ClampMin", "0.0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Tempo do debug\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Tempo do debug" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FWeaponTraceConfig constinit property declarations ****************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WeaponComponent;
	static const UECodeGen_Private::FNamePropertyParams NewProp_SocketBase;
	static const UECodeGen_Private::FNamePropertyParams NewProp_SocketTip;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Radius;
	static const UECodeGen_Private::FBytePropertyParams NewProp_TraceChannel;
	static void NewProp_bDrawDebug_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bDrawDebug;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_DebugLifeTime;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FWeaponTraceConfig constinit property declarations ******************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FWeaponTraceConfig>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FWeaponTraceConfig;
class UScriptStruct* FWeaponTraceConfig::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FWeaponTraceConfig.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FWeaponTraceConfig.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FWeaponTraceConfig, (UObject*)Z_Construct_UPackage__Script_SpellRise(), TEXT("WeaponTraceConfig"));
	}
	return Z_Registration_Info_UScriptStruct_FWeaponTraceConfig.OuterSingleton;
	}

// ********** Begin ScriptStruct FWeaponTraceConfig Property Definitions ***************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_WeaponComponent = { "WeaponComponent", nullptr, (EPropertyFlags)0x011400000008000d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponTraceConfig, WeaponComponent), Z_Construct_UClass_USceneComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WeaponComponent_MetaData), NewProp_WeaponComponent_MetaData) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_SocketBase = { "SocketBase", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponTraceConfig, SocketBase), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SocketBase_MetaData), NewProp_SocketBase_MetaData) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_SocketTip = { "SocketTip", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponTraceConfig, SocketTip), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SocketTip_MetaData), NewProp_SocketTip_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_Radius = { "Radius", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponTraceConfig, Radius), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Radius_MetaData), NewProp_Radius_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_TraceChannel = { "TraceChannel", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponTraceConfig, TraceChannel), Z_Construct_UEnum_Engine_ECollisionChannel, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TraceChannel_MetaData), NewProp_TraceChannel_MetaData) }; // 838391399
void Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_bDrawDebug_SetBit(void* Obj)
{
	((FWeaponTraceConfig*)Obj)->bDrawDebug = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_bDrawDebug = { "bDrawDebug", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(FWeaponTraceConfig), &Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_bDrawDebug_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bDrawDebug_MetaData), NewProp_bDrawDebug_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_DebugLifeTime = { "DebugLifeTime", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponTraceConfig, DebugLifeTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DebugLifeTime_MetaData), NewProp_DebugLifeTime_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_WeaponComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_SocketBase,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_SocketTip,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_Radius,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_TraceChannel,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_bDrawDebug,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewProp_DebugLifeTime,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::PropPointers) < 2048);
// ********** End ScriptStruct FWeaponTraceConfig Property Definitions *****************************
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
	nullptr,
	&NewStructOps,
	"WeaponTraceConfig",
	Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::PropPointers),
	sizeof(FWeaponTraceConfig),
	alignof(FWeaponTraceConfig),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000005),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FWeaponTraceConfig()
{
	if (!Z_Registration_Info_UScriptStruct_FWeaponTraceConfig.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FWeaponTraceConfig.InnerSingleton, Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FWeaponTraceConfig.InnerSingleton);
}
// ********** End ScriptStruct FWeaponTraceConfig **************************************************

// ********** Begin Class UWeaponTraceComponent Function BeginTraceWindow **************************
struct Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics
{
	struct WeaponTraceComponent_eventBeginTraceWindow_Parms
	{
		FWeaponTraceConfig InConfig;
		UAbilitySystemComponent* InSourceASC;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "WeaponTrace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Abre janela e \xe2\x80\x9c""arma\xe2\x80\x9d o trace. Ideal chamar no AnimNotifyState.\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Abre janela e \xe2\x80\x9c""arma\xe2\x80\x9d o trace. Ideal chamar no AnimNotifyState." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InConfig_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_InSourceASC_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif // WITH_METADATA

// ********** Begin Function BeginTraceWindow constinit property declarations **********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_InConfig;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_InSourceASC;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function BeginTraceWindow constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function BeginTraceWindow Property Definitions *********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::NewProp_InConfig = { "InConfig", nullptr, (EPropertyFlags)0x0010008008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(WeaponTraceComponent_eventBeginTraceWindow_Parms, InConfig), Z_Construct_UScriptStruct_FWeaponTraceConfig, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InConfig_MetaData), NewProp_InConfig_MetaData) }; // 1307011405
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::NewProp_InSourceASC = { "InSourceASC", nullptr, (EPropertyFlags)0x0010000000080080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(WeaponTraceComponent_eventBeginTraceWindow_Parms, InSourceASC), Z_Construct_UClass_UAbilitySystemComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_InSourceASC_MetaData), NewProp_InSourceASC_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::NewProp_InConfig,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::NewProp_InSourceASC,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::PropPointers) < 2048);
// ********** End Function BeginTraceWindow Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UWeaponTraceComponent, nullptr, "BeginTraceWindow", 	Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::WeaponTraceComponent_eventBeginTraceWindow_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04420401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::Function_MetaDataParams), Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::WeaponTraceComponent_eventBeginTraceWindow_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UWeaponTraceComponent::execBeginTraceWindow)
{
	P_GET_STRUCT_REF(FWeaponTraceConfig,Z_Param_Out_InConfig);
	P_GET_OBJECT(UAbilitySystemComponent,Z_Param_InSourceASC);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->BeginTraceWindow(Z_Param_Out_InConfig,Z_Param_InSourceASC);
	P_NATIVE_END;
}
// ********** End Class UWeaponTraceComponent Function BeginTraceWindow ****************************

// ********** Begin Class UWeaponTraceComponent Function EndTraceWindow ****************************
struct Z_Construct_UFunction_UWeaponTraceComponent_EndTraceWindow_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "WeaponTrace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Fecha janela e limpa estado\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Fecha janela e limpa estado" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function EndTraceWindow constinit property declarations ************************
// ********** End Function EndTraceWindow constinit property declarations **************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UWeaponTraceComponent_EndTraceWindow_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UWeaponTraceComponent, nullptr, "EndTraceWindow", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UWeaponTraceComponent_EndTraceWindow_Statics::Function_MetaDataParams), Z_Construct_UFunction_UWeaponTraceComponent_EndTraceWindow_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UWeaponTraceComponent_EndTraceWindow()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UWeaponTraceComponent_EndTraceWindow_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UWeaponTraceComponent::execEndTraceWindow)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->EndTraceWindow();
	P_NATIVE_END;
}
// ********** End Class UWeaponTraceComponent Function EndTraceWindow ******************************

// ********** Begin Class UWeaponTraceComponent Function ResetHitActors ****************************
struct Z_Construct_UFunction_UWeaponTraceComponent_ResetHitActors_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "WeaponTrace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Para combos: permite limpar lista de alvos j\xc3\xa1 atingidos\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Para combos: permite limpar lista de alvos j\xc3\xa1 atingidos" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function ResetHitActors constinit property declarations ************************
// ********** End Function ResetHitActors constinit property declarations **************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UWeaponTraceComponent_ResetHitActors_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UWeaponTraceComponent, nullptr, "ResetHitActors", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UWeaponTraceComponent_ResetHitActors_Statics::Function_MetaDataParams), Z_Construct_UFunction_UWeaponTraceComponent_ResetHitActors_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_UWeaponTraceComponent_ResetHitActors()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UWeaponTraceComponent_ResetHitActors_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UWeaponTraceComponent::execResetHitActors)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->ResetHitActors();
	P_NATIVE_END;
}
// ********** End Class UWeaponTraceComponent Function ResetHitActors ******************************

// ********** Begin Class UWeaponTraceComponent Function SetDamagePayload **************************
struct Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics
{
	struct WeaponTraceComponent_eventSetDamagePayload_Parms
	{
		TSubclassOf<UGameplayEffect> InDamageEffect;
		float InBaseDamage;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "WeaponTrace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Configura o \xe2\x80\x9cpayload\xe2\x80\x9d do dano (GE + SetByCaller)\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Configura o \xe2\x80\x9cpayload\xe2\x80\x9d do dano (GE + SetByCaller)" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function SetDamagePayload constinit property declarations **********************
	static const UECodeGen_Private::FClassPropertyParams NewProp_InDamageEffect;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_InBaseDamage;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function SetDamagePayload constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function SetDamagePayload Property Definitions *********************************
const UECodeGen_Private::FClassPropertyParams Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::NewProp_InDamageEffect = { "InDamageEffect", nullptr, (EPropertyFlags)0x0014000000000080, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(WeaponTraceComponent_eventSetDamagePayload_Parms, InDamageEffect), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::NewProp_InBaseDamage = { "InBaseDamage", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(WeaponTraceComponent_eventSetDamagePayload_Parms, InBaseDamage), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::NewProp_InDamageEffect,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::NewProp_InBaseDamage,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::PropPointers) < 2048);
// ********** End Function SetDamagePayload Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UWeaponTraceComponent, nullptr, "SetDamagePayload", 	Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::WeaponTraceComponent_eventSetDamagePayload_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::Function_MetaDataParams), Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::WeaponTraceComponent_eventSetDamagePayload_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UWeaponTraceComponent::execSetDamagePayload)
{
	P_GET_OBJECT(UClass,Z_Param_InDamageEffect);
	P_GET_PROPERTY(FFloatProperty,Z_Param_InBaseDamage);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SetDamagePayload(Z_Param_InDamageEffect,Z_Param_InBaseDamage);
	P_NATIVE_END;
}
// ********** End Class UWeaponTraceComponent Function SetDamagePayload ****************************

// ********** Begin Class UWeaponTraceComponent ****************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UWeaponTraceComponent;
UClass* UWeaponTraceComponent::GetPrivateStaticClass()
{
	using TClass = UWeaponTraceComponent;
	if (!Z_Registration_Info_UClass_UWeaponTraceComponent.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("WeaponTraceComponent"),
			Z_Registration_Info_UClass_UWeaponTraceComponent.InnerSingleton,
			StaticRegisterNativesUWeaponTraceComponent,
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
	return Z_Registration_Info_UClass_UWeaponTraceComponent.InnerSingleton;
}
UClass* Z_Construct_UClass_UWeaponTraceComponent_NoRegister()
{
	return UWeaponTraceComponent::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UWeaponTraceComponent_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "Combat" },
		{ "IncludePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SetByCallerDamageTag_MetaData[] = {
		{ "Category", "WeaponTrace|GAS" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Tag SetByCaller (edit\xc3\xa1vel em BP se quiser trocar no futuro)\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Tag SetByCaller (edit\xc3\xa1vel em BP se quiser trocar no futuro)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SourceASC_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DamageEffect_MetaData[] = {
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_HitActors_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Evita hit m\xc3\xbaltiplo no mesmo swing\n" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/Trace/WeaponTraceComponent.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Evita hit m\xc3\xbaltiplo no mesmo swing" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UWeaponTraceComponent constinit property declarations ********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_SetByCallerDamageTag;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_SourceASC;
	static const UECodeGen_Private::FClassPropertyParams NewProp_DamageEffect;
	static const UECodeGen_Private::FWeakObjectPropertyParams NewProp_HitActors_ElementProp;
	static const UECodeGen_Private::FSetPropertyParams NewProp_HitActors;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UWeaponTraceComponent constinit property declarations **********************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("BeginTraceWindow"), .Pointer = &UWeaponTraceComponent::execBeginTraceWindow },
		{ .NameUTF8 = UTF8TEXT("EndTraceWindow"), .Pointer = &UWeaponTraceComponent::execEndTraceWindow },
		{ .NameUTF8 = UTF8TEXT("ResetHitActors"), .Pointer = &UWeaponTraceComponent::execResetHitActors },
		{ .NameUTF8 = UTF8TEXT("SetDamagePayload"), .Pointer = &UWeaponTraceComponent::execSetDamagePayload },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UWeaponTraceComponent_BeginTraceWindow, "BeginTraceWindow" }, // 3445856637
		{ &Z_Construct_UFunction_UWeaponTraceComponent_EndTraceWindow, "EndTraceWindow" }, // 642409393
		{ &Z_Construct_UFunction_UWeaponTraceComponent_ResetHitActors, "ResetHitActors" }, // 3648138754
		{ &Z_Construct_UFunction_UWeaponTraceComponent_SetDamagePayload, "SetDamagePayload" }, // 2423846616
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UWeaponTraceComponent>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UWeaponTraceComponent_Statics

// ********** Begin Class UWeaponTraceComponent Property Definitions *******************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UWeaponTraceComponent_Statics::NewProp_SetByCallerDamageTag = { "SetByCallerDamageTag", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UWeaponTraceComponent, SetByCallerDamageTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SetByCallerDamageTag_MetaData), NewProp_SetByCallerDamageTag_MetaData) }; // 517357616
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UWeaponTraceComponent_Statics::NewProp_SourceASC = { "SourceASC", nullptr, (EPropertyFlags)0x0144000000080008, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UWeaponTraceComponent, SourceASC), Z_Construct_UClass_UAbilitySystemComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SourceASC_MetaData), NewProp_SourceASC_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UWeaponTraceComponent_Statics::NewProp_DamageEffect = { "DamageEffect", nullptr, (EPropertyFlags)0x0044000000000000, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UWeaponTraceComponent, DamageEffect), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DamageEffect_MetaData), NewProp_DamageEffect_MetaData) };
const UECodeGen_Private::FWeakObjectPropertyParams Z_Construct_UClass_UWeaponTraceComponent_Statics::NewProp_HitActors_ElementProp = { "HitActors", nullptr, (EPropertyFlags)0x0004000000000000, UECodeGen_Private::EPropertyGenFlags::WeakObject, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FSetPropertyParams Z_Construct_UClass_UWeaponTraceComponent_Statics::NewProp_HitActors = { "HitActors", nullptr, (EPropertyFlags)0x0044000000000000, UECodeGen_Private::EPropertyGenFlags::Set, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UWeaponTraceComponent, HitActors), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_HitActors_MetaData), NewProp_HitActors_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UWeaponTraceComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWeaponTraceComponent_Statics::NewProp_SetByCallerDamageTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWeaponTraceComponent_Statics::NewProp_SourceASC,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWeaponTraceComponent_Statics::NewProp_DamageEffect,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWeaponTraceComponent_Statics::NewProp_HitActors_ElementProp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWeaponTraceComponent_Statics::NewProp_HitActors,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UWeaponTraceComponent_Statics::PropPointers) < 2048);
// ********** End Class UWeaponTraceComponent Property Definitions *********************************
UObject* (*const Z_Construct_UClass_UWeaponTraceComponent_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UActorComponent,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UWeaponTraceComponent_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UWeaponTraceComponent_Statics::ClassParams = {
	&UWeaponTraceComponent::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UWeaponTraceComponent_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UWeaponTraceComponent_Statics::PropPointers),
	0,
	0x00B000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UWeaponTraceComponent_Statics::Class_MetaDataParams), Z_Construct_UClass_UWeaponTraceComponent_Statics::Class_MetaDataParams)
};
void UWeaponTraceComponent::StaticRegisterNativesUWeaponTraceComponent()
{
	UClass* Class = UWeaponTraceComponent::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UWeaponTraceComponent_Statics::Funcs));
}
UClass* Z_Construct_UClass_UWeaponTraceComponent()
{
	if (!Z_Registration_Info_UClass_UWeaponTraceComponent.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UWeaponTraceComponent.OuterSingleton, Z_Construct_UClass_UWeaponTraceComponent_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UWeaponTraceComponent.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UWeaponTraceComponent);
UWeaponTraceComponent::~UWeaponTraceComponent() {}
// ********** End Class UWeaponTraceComponent ******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h__Script_SpellRise_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FWeaponTraceConfig::StaticStruct, Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics::NewStructOps, TEXT("WeaponTraceConfig"),&Z_Registration_Info_UScriptStruct_FWeaponTraceConfig, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FWeaponTraceConfig), 1307011405U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UWeaponTraceComponent, UWeaponTraceComponent::StaticClass, TEXT("UWeaponTraceComponent"), &Z_Registration_Info_UClass_UWeaponTraceComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UWeaponTraceComponent), 2166267473U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h__Script_SpellRise_3436640849{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h__Script_SpellRise_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h__Script_SpellRise_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h__Script_SpellRise_Statics::ScriptStructInfo),
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
