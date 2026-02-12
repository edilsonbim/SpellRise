// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Characters/SpellRiseCharacterBase.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSpellRiseCharacterBase() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UClass_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_ACharacter();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilitySystemInterface_NoRegister();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility_NoRegister();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_EGameplayEffectReplicationMode();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayEventData();
GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRiseCharacterBase();
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRiseCharacterBase_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UBasicAttributeSet_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UCatalystAttributeSet_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UCombatAttributeSet_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UDA_WeaponDefinition_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UDerivedStatsAttributeSet_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_UResourceAttributeSet_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseAbilitySystemComponent_NoRegister();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseWeaponComponent_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class ASpellRiseCharacterBase Function BP_OnCatalystProc ***********************
struct SpellRiseCharacterBase_eventBP_OnCatalystProc_Parms
{
	int32 CatalystTier;
};
static FName NAME_ASpellRiseCharacterBase_BP_OnCatalystProc = FName(TEXT("BP_OnCatalystProc"));
void ASpellRiseCharacterBase::BP_OnCatalystProc(int32 CatalystTier)
{
	SpellRiseCharacterBase_eventBP_OnCatalystProc_Parms Parms;
	Parms.CatalystTier=CatalystTier;
	UFunction* Func = FindFunctionChecked(NAME_ASpellRiseCharacterBase_BP_OnCatalystProc);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Catalyst" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function BP_OnCatalystProc constinit property declarations *********************
	static const UECodeGen_Private::FIntPropertyParams NewProp_CatalystTier;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function BP_OnCatalystProc constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function BP_OnCatalystProc Property Definitions ********************************
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc_Statics::NewProp_CatalystTier = { "CatalystTier", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventBP_OnCatalystProc_Parms, CatalystTier), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc_Statics::NewProp_CatalystTier,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc_Statics::PropPointers) < 2048);
// ********** End Function BP_OnCatalystProc Property Definitions **********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "BP_OnCatalystProc", 	Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc_Statics::PropPointers), 
sizeof(SpellRiseCharacterBase_eventBP_OnCatalystProc_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08020800, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(SpellRiseCharacterBase_eventBP_OnCatalystProc_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc_Statics::FuncParams);
	}
	return ReturnFunction;
}
// ********** End Class ASpellRiseCharacterBase Function BP_OnCatalystProc *************************

// ********** Begin Class ASpellRiseCharacterBase Function BP_ShowDamagePop ************************
struct SpellRiseCharacterBase_eventBP_ShowDamagePop_Parms
{
	float Damage;
	AActor* InstigatorActor;
	FGameplayTag DamageTypeTag;
	bool bIsCrit;
};
static FName NAME_ASpellRiseCharacterBase_BP_ShowDamagePop = FName(TEXT("BP_ShowDamagePop"));
void ASpellRiseCharacterBase::BP_ShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit)
{
	SpellRiseCharacterBase_eventBP_ShowDamagePop_Parms Parms;
	Parms.Damage=Damage;
	Parms.InstigatorActor=InstigatorActor;
	Parms.DamageTypeTag=DamageTypeTag;
	Parms.bIsCrit=bIsCrit ? true : false;
	UFunction* Func = FindFunctionChecked(NAME_ASpellRiseCharacterBase_BP_ShowDamagePop);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|UI" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function BP_ShowDamagePop constinit property declarations **********************
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Damage;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_InstigatorActor;
	static const UECodeGen_Private::FStructPropertyParams NewProp_DamageTypeTag;
	static void NewProp_bIsCrit_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsCrit;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function BP_ShowDamagePop constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function BP_ShowDamagePop Property Definitions *********************************
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::NewProp_Damage = { "Damage", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventBP_ShowDamagePop_Parms, Damage), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::NewProp_InstigatorActor = { "InstigatorActor", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventBP_ShowDamagePop_Parms, InstigatorActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::NewProp_DamageTypeTag = { "DamageTypeTag", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventBP_ShowDamagePop_Parms, DamageTypeTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(0, nullptr) }; // 517357616
void Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::NewProp_bIsCrit_SetBit(void* Obj)
{
	((SpellRiseCharacterBase_eventBP_ShowDamagePop_Parms*)Obj)->bIsCrit = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::NewProp_bIsCrit = { "bIsCrit", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseCharacterBase_eventBP_ShowDamagePop_Parms), &Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::NewProp_bIsCrit_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::NewProp_Damage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::NewProp_InstigatorActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::NewProp_DamageTypeTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::NewProp_bIsCrit,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::PropPointers) < 2048);
// ********** End Function BP_ShowDamagePop Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "BP_ShowDamagePop", 	Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::PropPointers), 
sizeof(SpellRiseCharacterBase_eventBP_ShowDamagePop_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08020800, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(SpellRiseCharacterBase_eventBP_ShowDamagePop_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop_Statics::FuncParams);
	}
	return ReturnFunction;
}
// ********** End Class ASpellRiseCharacterBase Function BP_ShowDamagePop **************************

// ********** Begin Class ASpellRiseCharacterBase Function GetCatalystAttributeSet *****************
struct Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics
{
	struct SpellRiseCharacterBase_eventGetCatalystAttributeSet_Parms
	{
		UCatalystAttributeSet* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Catalyst" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// Catalyst (somente UI/VFX hooks)\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Catalyst (somente UI/VFX hooks)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif // WITH_METADATA

// ********** Begin Function GetCatalystAttributeSet constinit property declarations ***************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GetCatalystAttributeSet constinit property declarations *****************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GetCatalystAttributeSet Property Definitions **************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000080588, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventGetCatalystAttributeSet_Parms, ReturnValue), Z_Construct_UClass_UCatalystAttributeSet_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ReturnValue_MetaData), NewProp_ReturnValue_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::PropPointers) < 2048);
// ********** End Function GetCatalystAttributeSet Property Definitions ****************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "GetCatalystAttributeSet", 	Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::SpellRiseCharacterBase_eventGetCatalystAttributeSet_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::SpellRiseCharacterBase_eventGetCatalystAttributeSet_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execGetCatalystAttributeSet)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UCatalystAttributeSet**)Z_Param__Result=P_THIS->GetCatalystAttributeSet();
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function GetCatalystAttributeSet *******************

// ********** Begin Class ASpellRiseCharacterBase Function GetWeaponComponent **********************
struct Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics
{
	struct SpellRiseCharacterBase_eventGetWeaponComponent_Parms
	{
		USpellRiseWeaponComponent* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Weapon" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// Weapon\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Weapon" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif // WITH_METADATA

// ********** Begin Function GetWeaponComponent constinit property declarations ********************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GetWeaponComponent constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GetWeaponComponent Property Definitions *******************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000080588, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventGetWeaponComponent_Parms, ReturnValue), Z_Construct_UClass_USpellRiseWeaponComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ReturnValue_MetaData), NewProp_ReturnValue_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::PropPointers) < 2048);
// ********** End Function GetWeaponComponent Property Definitions *********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "GetWeaponComponent", 	Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::SpellRiseCharacterBase_eventGetWeaponComponent_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::SpellRiseCharacterBase_eventGetWeaponComponent_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execGetWeaponComponent)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(USpellRiseWeaponComponent**)Z_Param__Result=P_THIS->GetWeaponComponent();
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function GetWeaponComponent ************************

// ********** Begin Class ASpellRiseCharacterBase Function GrantAbilities **************************
struct Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics
{
	struct SpellRiseCharacterBase_eventGrantAbilities_Parms
	{
		TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant;
		int32 Level;
		TArray<FGameplayAbilitySpecHandle> ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|GAS" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// GAS API\n// =========================================================\n" },
#endif
		{ "CPP_Default_Level", "1" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "GAS API" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AbilitiesToGrant_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function GrantAbilities constinit property declarations ************************
	static const UECodeGen_Private::FClassPropertyParams NewProp_AbilitiesToGrant_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_AbilitiesToGrant;
	static const UECodeGen_Private::FIntPropertyParams NewProp_Level;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ReturnValue_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GrantAbilities constinit property declarations **************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GrantAbilities Property Definitions ***********************************
const UECodeGen_Private::FClassPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::NewProp_AbilitiesToGrant_Inner = { "AbilitiesToGrant", nullptr, (EPropertyFlags)0x0004000000000000, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayAbility_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::NewProp_AbilitiesToGrant = { "AbilitiesToGrant", nullptr, (EPropertyFlags)0x0014000008000182, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventGrantAbilities_Parms, AbilitiesToGrant), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AbilitiesToGrant_MetaData), NewProp_AbilitiesToGrant_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::NewProp_Level = { "Level", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventGrantAbilities_Parms, Level), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::NewProp_ReturnValue_Inner = { "ReturnValue", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle, METADATA_PARAMS(0, nullptr) }; // 224356909
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventGrantAbilities_Parms, ReturnValue), EArrayPropertyFlags::None, METADATA_PARAMS(0, nullptr) }; // 224356909
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::NewProp_AbilitiesToGrant_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::NewProp_AbilitiesToGrant,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::NewProp_Level,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::NewProp_ReturnValue_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::PropPointers) < 2048);
// ********** End Function GrantAbilities Property Definitions *************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "GrantAbilities", 	Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::SpellRiseCharacterBase_eventGrantAbilities_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04420401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::SpellRiseCharacterBase_eventGrantAbilities_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execGrantAbilities)
{
	P_GET_TARRAY_REF(TSubclassOf<UGameplayAbility>,Z_Param_Out_AbilitiesToGrant);
	P_GET_PROPERTY(FIntProperty,Z_Param_Level);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(TArray<FGameplayAbilitySpecHandle>*)Z_Param__Result=P_THIS->GrantAbilities(Z_Param_Out_AbilitiesToGrant,Z_Param_Level);
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function GrantAbilities ****************************

// ********** Begin Class ASpellRiseCharacterBase Function HandleDeath *****************************
static FName NAME_ASpellRiseCharacterBase_HandleDeath = FName(TEXT("HandleDeath"));
void ASpellRiseCharacterBase::HandleDeath()
{
	UFunction* Func = FindFunctionChecked(NAME_ASpellRiseCharacterBase_HandleDeath);
	if (!Func->GetOwnerClass()->HasAnyClassFlags(CLASS_Native))
	{
	ProcessEvent(Func,NULL);
	}
	else
	{
		HandleDeath_Implementation();
	}
}
struct Z_Construct_UFunction_ASpellRiseCharacterBase_HandleDeath_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Death" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function HandleDeath constinit property declarations ***************************
// ********** End Function HandleDeath constinit property declarations *****************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_HandleDeath_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "HandleDeath", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x0C020C00, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_HandleDeath_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_HandleDeath_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_HandleDeath()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_HandleDeath_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execHandleDeath)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->HandleDeath_Implementation();
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function HandleDeath *******************************

// ********** Begin Class ASpellRiseCharacterBase Function IsDead **********************************
struct Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics
{
	struct SpellRiseCharacterBase_eventIsDead_Parms
	{
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|Death" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// Death\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Death" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function IsDead constinit property declarations ********************************
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function IsDead constinit property declarations **********************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function IsDead Property Definitions *******************************************
void Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((SpellRiseCharacterBase_eventIsDead_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseCharacterBase_eventIsDead_Parms), &Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::PropPointers) < 2048);
// ********** End Function IsDead Property Definitions *********************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "IsDead", 	Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::SpellRiseCharacterBase_eventIsDead_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::SpellRiseCharacterBase_eventIsDead_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execIsDead)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->IsDead();
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function IsDead ************************************

// ********** Begin Class ASpellRiseCharacterBase Function MultiHandleDeath ************************
static FName NAME_ASpellRiseCharacterBase_MultiHandleDeath = FName(TEXT("MultiHandleDeath"));
void ASpellRiseCharacterBase::MultiHandleDeath()
{
	UFunction* Func = FindFunctionChecked(NAME_ASpellRiseCharacterBase_MultiHandleDeath);
	ProcessEvent(Func,NULL);
}
struct Z_Construct_UFunction_ASpellRiseCharacterBase_MultiHandleDeath_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function MultiHandleDeath constinit property declarations **********************
// ********** End Function MultiHandleDeath constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_MultiHandleDeath_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "MultiHandleDeath", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00024CC0, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_MultiHandleDeath_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_MultiHandleDeath_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_MultiHandleDeath()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_MultiHandleDeath_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execMultiHandleDeath)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->MultiHandleDeath_Implementation();
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function MultiHandleDeath **************************

// ********** Begin Class ASpellRiseCharacterBase Function MultiOnCatalystProc *********************
struct SpellRiseCharacterBase_eventMultiOnCatalystProc_Parms
{
	int32 CatalystTier;
};
static FName NAME_ASpellRiseCharacterBase_MultiOnCatalystProc = FName(TEXT("MultiOnCatalystProc"));
void ASpellRiseCharacterBase::MultiOnCatalystProc(int32 CatalystTier)
{
	SpellRiseCharacterBase_eventMultiOnCatalystProc_Parms Parms;
	Parms.CatalystTier=CatalystTier;
	UFunction* Func = FindFunctionChecked(NAME_ASpellRiseCharacterBase_MultiOnCatalystProc);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function MultiOnCatalystProc constinit property declarations *******************
	static const UECodeGen_Private::FIntPropertyParams NewProp_CatalystTier;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function MultiOnCatalystProc constinit property declarations *********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function MultiOnCatalystProc Property Definitions ******************************
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc_Statics::NewProp_CatalystTier = { "CatalystTier", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventMultiOnCatalystProc_Parms, CatalystTier), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc_Statics::NewProp_CatalystTier,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc_Statics::PropPointers) < 2048);
// ********** End Function MultiOnCatalystProc Property Definitions ********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "MultiOnCatalystProc", 	Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc_Statics::PropPointers), 
sizeof(SpellRiseCharacterBase_eventMultiOnCatalystProc_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00024C40, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(SpellRiseCharacterBase_eventMultiOnCatalystProc_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execMultiOnCatalystProc)
{
	P_GET_PROPERTY(FIntProperty,Z_Param_CatalystTier);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->MultiOnCatalystProc_Implementation(Z_Param_CatalystTier);
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function MultiOnCatalystProc ***********************

// ********** Begin Class ASpellRiseCharacterBase Function MultiSendGameplayEventToActor ***********
struct SpellRiseCharacterBase_eventMultiSendGameplayEventToActor_Parms
{
	AActor* TargetActor;
	FGameplayEventData EventData;
};
static FName NAME_ASpellRiseCharacterBase_MultiSendGameplayEventToActor = FName(TEXT("MultiSendGameplayEventToActor"));
void ASpellRiseCharacterBase::MultiSendGameplayEventToActor(AActor* TargetActor, FGameplayEventData const& EventData)
{
	SpellRiseCharacterBase_eventMultiSendGameplayEventToActor_Parms Parms;
	Parms.TargetActor=TargetActor;
	Parms.EventData=EventData;
	UFunction* Func = FindFunctionChecked(NAME_ASpellRiseCharacterBase_MultiSendGameplayEventToActor);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|GAS" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EventData_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function MultiSendGameplayEventToActor constinit property declarations *********
	static const UECodeGen_Private::FObjectPropertyParams NewProp_TargetActor;
	static const UECodeGen_Private::FStructPropertyParams NewProp_EventData;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function MultiSendGameplayEventToActor constinit property declarations ***********
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function MultiSendGameplayEventToActor Property Definitions ********************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::NewProp_TargetActor = { "TargetActor", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventMultiSendGameplayEventToActor_Parms, TargetActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::NewProp_EventData = { "EventData", nullptr, (EPropertyFlags)0x0010000008000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventMultiSendGameplayEventToActor_Parms, EventData), Z_Construct_UScriptStruct_FGameplayEventData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EventData_MetaData), NewProp_EventData_MetaData) }; // 1110230437
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::NewProp_TargetActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::NewProp_EventData,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::PropPointers) < 2048);
// ********** End Function MultiSendGameplayEventToActor Property Definitions **********************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "MultiSendGameplayEventToActor", 	Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::PropPointers), 
sizeof(SpellRiseCharacterBase_eventMultiSendGameplayEventToActor_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04024CC0, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(SpellRiseCharacterBase_eventMultiSendGameplayEventToActor_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execMultiSendGameplayEventToActor)
{
	P_GET_OBJECT(AActor,Z_Param_TargetActor);
	P_GET_STRUCT(FGameplayEventData,Z_Param_EventData);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->MultiSendGameplayEventToActor_Implementation(Z_Param_TargetActor,Z_Param_EventData);
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function MultiSendGameplayEventToActor *************

// ********** Begin Class ASpellRiseCharacterBase Function MultiShowDamagePop **********************
struct SpellRiseCharacterBase_eventMultiShowDamagePop_Parms
{
	float Damage;
	AActor* InstigatorActor;
	FGameplayTag DamageTypeTag;
	bool bIsCrit;
};
static FName NAME_ASpellRiseCharacterBase_MultiShowDamagePop = FName(TEXT("MultiShowDamagePop"));
void ASpellRiseCharacterBase::MultiShowDamagePop(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit)
{
	SpellRiseCharacterBase_eventMultiShowDamagePop_Parms Parms;
	Parms.Damage=Damage;
	Parms.InstigatorActor=InstigatorActor;
	Parms.DamageTypeTag=DamageTypeTag;
	Parms.bIsCrit=bIsCrit ? true : false;
	UFunction* Func = FindFunctionChecked(NAME_ASpellRiseCharacterBase_MultiShowDamagePop);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|UI" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// Damage Pop\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Damage Pop" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function MultiShowDamagePop constinit property declarations ********************
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Damage;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_InstigatorActor;
	static const UECodeGen_Private::FStructPropertyParams NewProp_DamageTypeTag;
	static void NewProp_bIsCrit_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsCrit;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function MultiShowDamagePop constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function MultiShowDamagePop Property Definitions *******************************
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::NewProp_Damage = { "Damage", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventMultiShowDamagePop_Parms, Damage), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::NewProp_InstigatorActor = { "InstigatorActor", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventMultiShowDamagePop_Parms, InstigatorActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::NewProp_DamageTypeTag = { "DamageTypeTag", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventMultiShowDamagePop_Parms, DamageTypeTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(0, nullptr) }; // 517357616
void Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::NewProp_bIsCrit_SetBit(void* Obj)
{
	((SpellRiseCharacterBase_eventMultiShowDamagePop_Parms*)Obj)->bIsCrit = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::NewProp_bIsCrit = { "bIsCrit", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseCharacterBase_eventMultiShowDamagePop_Parms), &Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::NewProp_bIsCrit_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::NewProp_Damage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::NewProp_InstigatorActor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::NewProp_DamageTypeTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::NewProp_bIsCrit,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::PropPointers) < 2048);
// ********** End Function MultiShowDamagePop Property Definitions *********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "MultiShowDamagePop", 	Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::PropPointers), 
sizeof(SpellRiseCharacterBase_eventMultiShowDamagePop_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04024C40, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(SpellRiseCharacterBase_eventMultiShowDamagePop_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execMultiShowDamagePop)
{
	P_GET_PROPERTY(FFloatProperty,Z_Param_Damage);
	P_GET_OBJECT(AActor,Z_Param_InstigatorActor);
	P_GET_STRUCT(FGameplayTag,Z_Param_DamageTypeTag);
	P_GET_UBOOL(Z_Param_bIsCrit);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->MultiShowDamagePop_Implementation(Z_Param_Damage,Z_Param_InstigatorActor,Z_Param_DamageTypeTag,Z_Param_bIsCrit);
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function MultiShowDamagePop ************************

// ********** Begin Class ASpellRiseCharacterBase Function OnDeadTagChanged ************************
struct Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics
{
	struct SpellRiseCharacterBase_eventOnDeadTagChanged_Parms
	{
		FGameplayTag CallbackTag;
		int32 NewCount;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnDeadTagChanged constinit property declarations **********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_CallbackTag;
	static const UECodeGen_Private::FIntPropertyParams NewProp_NewCount;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnDeadTagChanged constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnDeadTagChanged Property Definitions *********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::NewProp_CallbackTag = { "CallbackTag", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventOnDeadTagChanged_Parms, CallbackTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(0, nullptr) }; // 517357616
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::NewProp_NewCount = { "NewCount", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventOnDeadTagChanged_Parms, NewCount), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::NewProp_CallbackTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::NewProp_NewCount,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::PropPointers) < 2048);
// ********** End Function OnDeadTagChanged Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "OnDeadTagChanged", 	Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::SpellRiseCharacterBase_eventOnDeadTagChanged_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::SpellRiseCharacterBase_eventOnDeadTagChanged_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execOnDeadTagChanged)
{
	P_GET_STRUCT(FGameplayTag,Z_Param_CallbackTag);
	P_GET_PROPERTY(FIntProperty,Z_Param_NewCount);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnDeadTagChanged(Z_Param_CallbackTag,Z_Param_NewCount);
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function OnDeadTagChanged **************************

// ********** Begin Class ASpellRiseCharacterBase Function RemoveAbilities *************************
struct Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics
{
	struct SpellRiseCharacterBase_eventRemoveAbilities_Parms
	{
		TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|GAS" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AbilityHandlesToRemove_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function RemoveAbilities constinit property declarations ***********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_AbilityHandlesToRemove_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_AbilityHandlesToRemove;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function RemoveAbilities constinit property declarations *************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function RemoveAbilities Property Definitions **********************************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::NewProp_AbilityHandlesToRemove_Inner = { "AbilityHandlesToRemove", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGameplayAbilitySpecHandle, METADATA_PARAMS(0, nullptr) }; // 224356909
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::NewProp_AbilityHandlesToRemove = { "AbilityHandlesToRemove", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventRemoveAbilities_Parms, AbilityHandlesToRemove), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AbilityHandlesToRemove_MetaData), NewProp_AbilityHandlesToRemove_MetaData) }; // 224356909
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::NewProp_AbilityHandlesToRemove_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::NewProp_AbilityHandlesToRemove,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::PropPointers) < 2048);
// ********** End Function RemoveAbilities Property Definitions ************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "RemoveAbilities", 	Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::SpellRiseCharacterBase_eventRemoveAbilities_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04420401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::SpellRiseCharacterBase_eventRemoveAbilities_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execRemoveAbilities)
{
	P_GET_TARRAY_REF(FGameplayAbilitySpecHandle,Z_Param_Out_AbilityHandlesToRemove);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->RemoveAbilities(Z_Param_Out_AbilityHandlesToRemove);
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function RemoveAbilities ***************************

// ********** Begin Class ASpellRiseCharacterBase Function SendAbilitiesChangedEvent ***************
struct Z_Construct_UFunction_ASpellRiseCharacterBase_SendAbilitiesChangedEvent_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|GAS" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function SendAbilitiesChangedEvent constinit property declarations *************
// ********** End Function SendAbilitiesChangedEvent constinit property declarations ***************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_SendAbilitiesChangedEvent_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "SendAbilitiesChangedEvent", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_SendAbilitiesChangedEvent_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_SendAbilitiesChangedEvent_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_SendAbilitiesChangedEvent()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_SendAbilitiesChangedEvent_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execSendAbilitiesChangedEvent)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SendAbilitiesChangedEvent();
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function SendAbilitiesChangedEvent *****************

// ********** Begin Class ASpellRiseCharacterBase Function ServerSendGameplayEventToSelf ***********
struct SpellRiseCharacterBase_eventServerSendGameplayEventToSelf_Parms
{
	FGameplayEventData EventData;
};
static FName NAME_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf = FName(TEXT("ServerSendGameplayEventToSelf"));
void ASpellRiseCharacterBase::ServerSendGameplayEventToSelf(FGameplayEventData const& EventData)
{
	SpellRiseCharacterBase_eventServerSendGameplayEventToSelf_Parms Parms;
	Parms.EventData=EventData;
	UFunction* Func = FindFunctionChecked(NAME_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|GAS" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EventData_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function ServerSendGameplayEventToSelf constinit property declarations *********
	static const UECodeGen_Private::FStructPropertyParams NewProp_EventData;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function ServerSendGameplayEventToSelf constinit property declarations ***********
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function ServerSendGameplayEventToSelf Property Definitions ********************
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf_Statics::NewProp_EventData = { "EventData", nullptr, (EPropertyFlags)0x0010000008000082, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventServerSendGameplayEventToSelf_Parms, EventData), Z_Construct_UScriptStruct_FGameplayEventData, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EventData_MetaData), NewProp_EventData_MetaData) }; // 1110230437
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf_Statics::NewProp_EventData,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf_Statics::PropPointers) < 2048);
// ********** End Function ServerSendGameplayEventToSelf Property Definitions **********************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "ServerSendGameplayEventToSelf", 	Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf_Statics::PropPointers), 
sizeof(SpellRiseCharacterBase_eventServerSendGameplayEventToSelf_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04220CC0, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(SpellRiseCharacterBase_eventServerSendGameplayEventToSelf_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execServerSendGameplayEventToSelf)
{
	P_GET_STRUCT(FGameplayEventData,Z_Param_EventData);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->ServerSendGameplayEventToSelf_Implementation(Z_Param_EventData);
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function ServerSendGameplayEventToSelf *************

// ********** Begin Class ASpellRiseCharacterBase Function SR_ClearAbilityInput ********************
struct Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ClearAbilityInput_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|GAS|Input" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function SR_ClearAbilityInput constinit property declarations ******************
// ********** End Function SR_ClearAbilityInput constinit property declarations ********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ClearAbilityInput_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "SR_ClearAbilityInput", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ClearAbilityInput_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ClearAbilityInput_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ClearAbilityInput()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ClearAbilityInput_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execSR_ClearAbilityInput)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SR_ClearAbilityInput();
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function SR_ClearAbilityInput **********************

// ********** Begin Class ASpellRiseCharacterBase Function SR_ProcessAbilityInput ******************
struct Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics
{
	struct SpellRiseCharacterBase_eventSR_ProcessAbilityInput_Parms
	{
		float DeltaTime;
		bool bGamePaused;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "SpellRise|GAS|Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// GAS Input bridge (Character -> ASC)\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "GAS Input bridge (Character -> ASC)" },
#endif
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
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::NewProp_DeltaTime = { "DeltaTime", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseCharacterBase_eventSR_ProcessAbilityInput_Parms, DeltaTime), METADATA_PARAMS(0, nullptr) };
void Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::NewProp_bGamePaused_SetBit(void* Obj)
{
	((SpellRiseCharacterBase_eventSR_ProcessAbilityInput_Parms*)Obj)->bGamePaused = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::NewProp_bGamePaused = { "bGamePaused", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseCharacterBase_eventSR_ProcessAbilityInput_Parms), &Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::NewProp_bGamePaused_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::NewProp_DeltaTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::NewProp_bGamePaused,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::PropPointers) < 2048);
// ********** End Function SR_ProcessAbilityInput Property Definitions *****************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_ASpellRiseCharacterBase, nullptr, "SR_ProcessAbilityInput", 	Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::SpellRiseCharacterBase_eventSR_ProcessAbilityInput_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::Function_MetaDataParams), Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::SpellRiseCharacterBase_eventSR_ProcessAbilityInput_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(ASpellRiseCharacterBase::execSR_ProcessAbilityInput)
{
	P_GET_PROPERTY(FFloatProperty,Z_Param_DeltaTime);
	P_GET_UBOOL(Z_Param_bGamePaused);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SR_ProcessAbilityInput(Z_Param_DeltaTime,Z_Param_bGamePaused);
	P_NATIVE_END;
}
// ********** End Class ASpellRiseCharacterBase Function SR_ProcessAbilityInput ********************

// ********** Begin Class ASpellRiseCharacterBase **************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_ASpellRiseCharacterBase;
UClass* ASpellRiseCharacterBase::GetPrivateStaticClass()
{
	using TClass = ASpellRiseCharacterBase;
	if (!Z_Registration_Info_UClass_ASpellRiseCharacterBase.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRiseCharacterBase"),
			Z_Registration_Info_UClass_ASpellRiseCharacterBase.InnerSingleton,
			StaticRegisterNativesASpellRiseCharacterBase,
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
	return Z_Registration_Info_UClass_ASpellRiseCharacterBase.InnerSingleton;
}
UClass* Z_Construct_UClass_ASpellRiseCharacterBase_NoRegister()
{
	return ASpellRiseCharacterBase::GetPrivateStaticClass();
}
struct Z_Construct_UClass_ASpellRiseCharacterBase_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "Characters/SpellRiseCharacterBase.h" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BaseWalkSpeed_MetaData[] = {
		{ "Category", "SpellRise|Movement" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Base walk speed (before bonuses). Final speed = (BaseWalkSpeed + MoveSpeedBonus) * MoveSpeedMultiplier */" },
#endif
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Base walk speed (before bonuses). Final speed = (BaseWalkSpeed + MoveSpeedBonus) * MoveSpeedMultiplier" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WeaponComponent_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "SpellRise|Components" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// Components\n// =========================================================\n" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Components" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DefaultWeapon_MetaData[] = {
		{ "Category", "SpellRise|Weapon|Startup" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// Weapon Startup\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Weapon Startup" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AbilitySystemComponent_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "SpellRise|GAS" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// GAS: Core + AttributeSets\n// =========================================================\n// NOTE:\n// - For Players, we prefer the PlayerState ASC (authoritative GAS owner).\n// - For AI/NPC (no PlayerState), we use the Character-owned ASC.\n" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "GAS: Core + AttributeSets\n\nNOTE:\n- For Players, we prefer the PlayerState ASC (authoritative GAS owner).\n- For AI/NPC (no PlayerState), we use the Character-owned ASC." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BasicAttributeSet_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "SpellRise|GAS" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CombatAttributeSet_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "SpellRise|GAS" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ResourceAttributeSet_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "SpellRise|GAS" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CatalystAttributeSet_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "SpellRise|GAS" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DerivedStatsAttributeSet_MetaData[] = {
		{ "AllowPrivateAccess", "true" },
		{ "Category", "SpellRise|GAS" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AscReplicationMode_MetaData[] = {
		{ "Category", "SpellRise|GAS" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// GAS: Startup / Config\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "GAS: Startup / Config" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_StartingAbilities_MetaData[] = {
		{ "Category", "SpellRise|GAS|Startup" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_RecalculateResources_MetaData[] = {
		{ "Category", "SpellRise|GAS|Startup" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_DerivedStatsInfinite_MetaData[] = {
		{ "Category", "SpellRise|GAS|Startup" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_RegenBase_MetaData[] = {
		{ "Category", "SpellRise|GAS|Regen" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_RegenEffects_MetaData[] = {
		{ "Category", "SpellRise|GAS|Regen" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Action_PrimaryAttack_MetaData[] = {
		{ "Category", "SpellRise|Input|Legacy" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// Input (Legacy Input)\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Input (Legacy Input)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Action_SecondaryAttack_MetaData[] = {
		{ "Category", "SpellRise|Input|Legacy" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Action_Cancel_MetaData[] = {
		{ "Category", "SpellRise|Input|Legacy" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DeadStateTag_MetaData[] = {
		{ "Category", "SpellRise|Death" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// =========================================================\n// Death\n// =========================================================\n" },
#endif
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Death" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_Death_MetaData[] = {
		{ "Category", "SpellRise|Death" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bIsDead_MetaData[] = {
		{ "Category", "SpellRise|Death" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bASCDelegatesBound_MetaData[] = {
		{ "Category", "SpellRise|GAS" },
		{ "ModuleRelativePath", "Characters/SpellRiseCharacterBase.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class ASpellRiseCharacterBase constinit property declarations ******************
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BaseWalkSpeed;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WeaponComponent;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_DefaultWeapon;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_AbilitySystemComponent;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_BasicAttributeSet;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CombatAttributeSet;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ResourceAttributeSet;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_CatalystAttributeSet;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_DerivedStatsAttributeSet;
	static const UECodeGen_Private::FBytePropertyParams NewProp_AscReplicationMode_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_AscReplicationMode;
	static const UECodeGen_Private::FClassPropertyParams NewProp_StartingAbilities_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_StartingAbilities;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_RecalculateResources;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_DerivedStatsInfinite;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_RegenBase;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_RegenEffects_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_GE_RegenEffects;
	static const UECodeGen_Private::FNamePropertyParams NewProp_Action_PrimaryAttack;
	static const UECodeGen_Private::FNamePropertyParams NewProp_Action_SecondaryAttack;
	static const UECodeGen_Private::FNamePropertyParams NewProp_Action_Cancel;
	static const UECodeGen_Private::FStructPropertyParams NewProp_DeadStateTag;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_Death;
	static void NewProp_bIsDead_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsDead;
	static void NewProp_bASCDelegatesBound_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bASCDelegatesBound;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class ASpellRiseCharacterBase constinit property declarations ********************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("GetCatalystAttributeSet"), .Pointer = &ASpellRiseCharacterBase::execGetCatalystAttributeSet },
		{ .NameUTF8 = UTF8TEXT("GetWeaponComponent"), .Pointer = &ASpellRiseCharacterBase::execGetWeaponComponent },
		{ .NameUTF8 = UTF8TEXT("GrantAbilities"), .Pointer = &ASpellRiseCharacterBase::execGrantAbilities },
		{ .NameUTF8 = UTF8TEXT("HandleDeath"), .Pointer = &ASpellRiseCharacterBase::execHandleDeath },
		{ .NameUTF8 = UTF8TEXT("IsDead"), .Pointer = &ASpellRiseCharacterBase::execIsDead },
		{ .NameUTF8 = UTF8TEXT("MultiHandleDeath"), .Pointer = &ASpellRiseCharacterBase::execMultiHandleDeath },
		{ .NameUTF8 = UTF8TEXT("MultiOnCatalystProc"), .Pointer = &ASpellRiseCharacterBase::execMultiOnCatalystProc },
		{ .NameUTF8 = UTF8TEXT("MultiSendGameplayEventToActor"), .Pointer = &ASpellRiseCharacterBase::execMultiSendGameplayEventToActor },
		{ .NameUTF8 = UTF8TEXT("MultiShowDamagePop"), .Pointer = &ASpellRiseCharacterBase::execMultiShowDamagePop },
		{ .NameUTF8 = UTF8TEXT("OnDeadTagChanged"), .Pointer = &ASpellRiseCharacterBase::execOnDeadTagChanged },
		{ .NameUTF8 = UTF8TEXT("RemoveAbilities"), .Pointer = &ASpellRiseCharacterBase::execRemoveAbilities },
		{ .NameUTF8 = UTF8TEXT("SendAbilitiesChangedEvent"), .Pointer = &ASpellRiseCharacterBase::execSendAbilitiesChangedEvent },
		{ .NameUTF8 = UTF8TEXT("ServerSendGameplayEventToSelf"), .Pointer = &ASpellRiseCharacterBase::execServerSendGameplayEventToSelf },
		{ .NameUTF8 = UTF8TEXT("SR_ClearAbilityInput"), .Pointer = &ASpellRiseCharacterBase::execSR_ClearAbilityInput },
		{ .NameUTF8 = UTF8TEXT("SR_ProcessAbilityInput"), .Pointer = &ASpellRiseCharacterBase::execSR_ProcessAbilityInput },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_BP_OnCatalystProc, "BP_OnCatalystProc" }, // 1460873770
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_BP_ShowDamagePop, "BP_ShowDamagePop" }, // 1337796507
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_GetCatalystAttributeSet, "GetCatalystAttributeSet" }, // 3740385981
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_GetWeaponComponent, "GetWeaponComponent" }, // 1932161431
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_GrantAbilities, "GrantAbilities" }, // 1302242019
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_HandleDeath, "HandleDeath" }, // 3539173346
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_IsDead, "IsDead" }, // 1606539895
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_MultiHandleDeath, "MultiHandleDeath" }, // 4261188573
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_MultiOnCatalystProc, "MultiOnCatalystProc" }, // 1459208610
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_MultiSendGameplayEventToActor, "MultiSendGameplayEventToActor" }, // 1766363161
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_MultiShowDamagePop, "MultiShowDamagePop" }, // 1969902126
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_OnDeadTagChanged, "OnDeadTagChanged" }, // 3598187880
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_RemoveAbilities, "RemoveAbilities" }, // 3563052596
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_SendAbilitiesChangedEvent, "SendAbilitiesChangedEvent" }, // 2567107304
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_ServerSendGameplayEventToSelf, "ServerSendGameplayEventToSelf" }, // 1260454589
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ClearAbilityInput, "SR_ClearAbilityInput" }, // 1561070177
		{ &Z_Construct_UFunction_ASpellRiseCharacterBase_SR_ProcessAbilityInput, "SR_ProcessAbilityInput" }, // 927411414
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static const UECodeGen_Private::FImplementedInterfaceParams InterfaceParams[];
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASpellRiseCharacterBase>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_ASpellRiseCharacterBase_Statics

// ********** Begin Class ASpellRiseCharacterBase Property Definitions *****************************
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_BaseWalkSpeed = { "BaseWalkSpeed", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, BaseWalkSpeed), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BaseWalkSpeed_MetaData), NewProp_BaseWalkSpeed_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_WeaponComponent = { "WeaponComponent", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, WeaponComponent), Z_Construct_UClass_USpellRiseWeaponComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WeaponComponent_MetaData), NewProp_WeaponComponent_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_DefaultWeapon = { "DefaultWeapon", nullptr, (EPropertyFlags)0x0124080000010015, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, DefaultWeapon), Z_Construct_UClass_UDA_WeaponDefinition_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DefaultWeapon_MetaData), NewProp_DefaultWeapon_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_AbilitySystemComponent = { "AbilitySystemComponent", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, AbilitySystemComponent), Z_Construct_UClass_USpellRiseAbilitySystemComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AbilitySystemComponent_MetaData), NewProp_AbilitySystemComponent_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_BasicAttributeSet = { "BasicAttributeSet", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, BasicAttributeSet), Z_Construct_UClass_UBasicAttributeSet_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BasicAttributeSet_MetaData), NewProp_BasicAttributeSet_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_CombatAttributeSet = { "CombatAttributeSet", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, CombatAttributeSet), Z_Construct_UClass_UCombatAttributeSet_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CombatAttributeSet_MetaData), NewProp_CombatAttributeSet_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_ResourceAttributeSet = { "ResourceAttributeSet", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, ResourceAttributeSet), Z_Construct_UClass_UResourceAttributeSet_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ResourceAttributeSet_MetaData), NewProp_ResourceAttributeSet_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_CatalystAttributeSet = { "CatalystAttributeSet", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, CatalystAttributeSet), Z_Construct_UClass_UCatalystAttributeSet_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CatalystAttributeSet_MetaData), NewProp_CatalystAttributeSet_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_DerivedStatsAttributeSet = { "DerivedStatsAttributeSet", nullptr, (EPropertyFlags)0x01240800000a001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, DerivedStatsAttributeSet), Z_Construct_UClass_UDerivedStatsAttributeSet_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DerivedStatsAttributeSet_MetaData), NewProp_DerivedStatsAttributeSet_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_AscReplicationMode_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_AscReplicationMode = { "AscReplicationMode", nullptr, (EPropertyFlags)0x0020080000000005, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, AscReplicationMode), Z_Construct_UEnum_GameplayAbilities_EGameplayEffectReplicationMode, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AscReplicationMode_MetaData), NewProp_AscReplicationMode_MetaData) }; // 3513395660
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_StartingAbilities_Inner = { "StartingAbilities", nullptr, (EPropertyFlags)0x0004000000000000, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayAbility_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_StartingAbilities = { "StartingAbilities", nullptr, (EPropertyFlags)0x0024080000000005, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, StartingAbilities), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_StartingAbilities_MetaData), NewProp_StartingAbilities_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_RecalculateResources = { "GE_RecalculateResources", nullptr, (EPropertyFlags)0x0024080000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, GE_RecalculateResources), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_RecalculateResources_MetaData), NewProp_GE_RecalculateResources_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_DerivedStatsInfinite = { "GE_DerivedStatsInfinite", nullptr, (EPropertyFlags)0x0024080000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, GE_DerivedStatsInfinite), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_DerivedStatsInfinite_MetaData), NewProp_GE_DerivedStatsInfinite_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_RegenBase = { "GE_RegenBase", nullptr, (EPropertyFlags)0x0024080000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, GE_RegenBase), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_RegenBase_MetaData), NewProp_GE_RegenBase_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_RegenEffects_Inner = { "GE_RegenEffects", nullptr, (EPropertyFlags)0x0004000000000000, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_RegenEffects = { "GE_RegenEffects", nullptr, (EPropertyFlags)0x0024080000010015, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, GE_RegenEffects), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_RegenEffects_MetaData), NewProp_GE_RegenEffects_MetaData) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_Action_PrimaryAttack = { "Action_PrimaryAttack", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, Action_PrimaryAttack), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Action_PrimaryAttack_MetaData), NewProp_Action_PrimaryAttack_MetaData) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_Action_SecondaryAttack = { "Action_SecondaryAttack", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, Action_SecondaryAttack), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Action_SecondaryAttack_MetaData), NewProp_Action_SecondaryAttack_MetaData) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_Action_Cancel = { "Action_Cancel", nullptr, (EPropertyFlags)0x0020080000010001, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, Action_Cancel), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Action_Cancel_MetaData), NewProp_Action_Cancel_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_DeadStateTag = { "DeadStateTag", nullptr, (EPropertyFlags)0x0020080000010015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, DeadStateTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DeadStateTag_MetaData), NewProp_DeadStateTag_MetaData) }; // 517357616
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_Death = { "GE_Death", nullptr, (EPropertyFlags)0x0024080000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ASpellRiseCharacterBase, GE_Death), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_Death_MetaData), NewProp_GE_Death_MetaData) };
void Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_bIsDead_SetBit(void* Obj)
{
	((ASpellRiseCharacterBase*)Obj)->bIsDead = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_bIsDead = { "bIsDead", nullptr, (EPropertyFlags)0x0020080000020015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(ASpellRiseCharacterBase), &Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_bIsDead_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bIsDead_MetaData), NewProp_bIsDead_MetaData) };
void Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_bASCDelegatesBound_SetBit(void* Obj)
{
	((ASpellRiseCharacterBase*)Obj)->bASCDelegatesBound = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_bASCDelegatesBound = { "bASCDelegatesBound", nullptr, (EPropertyFlags)0x0020080000020015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(ASpellRiseCharacterBase), &Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_bASCDelegatesBound_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bASCDelegatesBound_MetaData), NewProp_bASCDelegatesBound_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ASpellRiseCharacterBase_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_BaseWalkSpeed,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_WeaponComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_DefaultWeapon,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_AbilitySystemComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_BasicAttributeSet,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_CombatAttributeSet,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_ResourceAttributeSet,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_CatalystAttributeSet,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_DerivedStatsAttributeSet,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_AscReplicationMode_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_AscReplicationMode,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_StartingAbilities_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_StartingAbilities,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_RecalculateResources,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_DerivedStatsInfinite,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_RegenBase,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_RegenEffects_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_RegenEffects,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_Action_PrimaryAttack,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_Action_SecondaryAttack,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_Action_Cancel,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_DeadStateTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_GE_Death,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_bIsDead,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ASpellRiseCharacterBase_Statics::NewProp_bASCDelegatesBound,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseCharacterBase_Statics::PropPointers) < 2048);
// ********** End Class ASpellRiseCharacterBase Property Definitions *******************************
UObject* (*const Z_Construct_UClass_ASpellRiseCharacterBase_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ACharacter,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseCharacterBase_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FImplementedInterfaceParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::InterfaceParams[] = {
	{ Z_Construct_UClass_UAbilitySystemInterface_NoRegister, (int32)VTABLE_OFFSET(ASpellRiseCharacterBase, IAbilitySystemInterface), false },  // 2722098046
};
const UECodeGen_Private::FClassParams Z_Construct_UClass_ASpellRiseCharacterBase_Statics::ClassParams = {
	&ASpellRiseCharacterBase::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_ASpellRiseCharacterBase_Statics::PropPointers,
	InterfaceParams,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseCharacterBase_Statics::PropPointers),
	UE_ARRAY_COUNT(InterfaceParams),
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ASpellRiseCharacterBase_Statics::Class_MetaDataParams), Z_Construct_UClass_ASpellRiseCharacterBase_Statics::Class_MetaDataParams)
};
void ASpellRiseCharacterBase::StaticRegisterNativesASpellRiseCharacterBase()
{
	UClass* Class = ASpellRiseCharacterBase::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_ASpellRiseCharacterBase_Statics::Funcs));
}
UClass* Z_Construct_UClass_ASpellRiseCharacterBase()
{
	if (!Z_Registration_Info_UClass_ASpellRiseCharacterBase.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ASpellRiseCharacterBase.OuterSingleton, Z_Construct_UClass_ASpellRiseCharacterBase_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ASpellRiseCharacterBase.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, ASpellRiseCharacterBase);
ASpellRiseCharacterBase::~ASpellRiseCharacterBase() {}
// ********** End Class ASpellRiseCharacterBase ****************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ASpellRiseCharacterBase, ASpellRiseCharacterBase::StaticClass, TEXT("ASpellRiseCharacterBase"), &Z_Registration_Info_UClass_ASpellRiseCharacterBase, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ASpellRiseCharacterBase), 2561392000U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h__Script_SpellRise_1121043461{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
