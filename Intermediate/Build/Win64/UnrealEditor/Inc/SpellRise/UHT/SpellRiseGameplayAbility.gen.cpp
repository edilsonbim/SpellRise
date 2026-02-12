// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"
#include "GameplayTagContainer.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeSpellRiseGameplayAbility() {}

// ********** Begin Cross Module References ********************************************************
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UAbilityTask_WaitInputRelease_NoRegister();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayAbility();
GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGameplayAbility();
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGameplayAbility_NoRegister();
SPELLRISE_API UEnum* Z_Construct_UEnum_SpellRise_EAbilityInputID();
SPELLRISE_API UEnum* Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Enum EAbilityInputID ***********************************************************
static FEnumRegistrationInfo Z_Registration_Info_UEnum_EAbilityInputID;
static UEnum* EAbilityInputID_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_EAbilityInputID.OuterSingleton)
	{
		Z_Registration_Info_UEnum_EAbilityInputID.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_SpellRise_EAbilityInputID, (UObject*)Z_Construct_UPackage__Script_SpellRise(), TEXT("EAbilityInputID"));
	}
	return Z_Registration_Info_UEnum_EAbilityInputID.OuterSingleton;
}
template<> SPELLRISE_NON_ATTRIBUTED_API UEnum* StaticEnum<EAbilityInputID>()
{
	return EAbilityInputID_StaticEnum();
}
struct Z_Construct_UEnum_SpellRise_EAbilityInputID_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "Ability1.DisplayName", "Ability1" },
		{ "Ability1.Name", "EAbilityInputID::Ability1" },
		{ "Ability2.DisplayName", "Ability2" },
		{ "Ability2.Name", "EAbilityInputID::Ability2" },
		{ "Ability3.DisplayName", "Ability3" },
		{ "Ability3.Name", "EAbilityInputID::Ability3" },
		{ "Ability4.DisplayName", "Ability4" },
		{ "Ability4.Name", "EAbilityInputID::Ability4" },
		{ "Ability5.DisplayName", "Ability5" },
		{ "Ability5.Name", "EAbilityInputID::Ability5" },
		{ "Ability6.DisplayName", "Ability6" },
		{ "Ability6.Name", "EAbilityInputID::Ability6" },
		{ "Ability7.DisplayName", "Ability7" },
		{ "Ability7.Name", "EAbilityInputID::Ability7" },
		{ "Ability8.DisplayName", "Ability8" },
		{ "Ability8.Name", "EAbilityInputID::Ability8" },
		{ "BlueprintType", "true" },
		{ "Cancel.DisplayName", "Cancel" },
		{ "Cancel.Name", "EAbilityInputID::Cancel" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Input slots for ability bar / activation binding.\n *\n * AAA rule:\n * - NEVER change existing numeric values (Blueprints serialize the underlying byte).\n * - Only append new entries at the end, OR explicitly assign numeric values to freeze them forever.\n */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
		{ "None.Comment", "// ---- Frozen legacy slots (do not change) ----\n" },
		{ "None.DisplayName", "None" },
		{ "None.Name", "EAbilityInputID::None" },
		{ "None.ToolTip", "---- Frozen legacy slots (do not change) ----" },
		{ "PrimaryAttack.Comment", "// ---- Appended (safe) ----\n" },
		{ "PrimaryAttack.DisplayName", "PrimaryAttack" },
		{ "PrimaryAttack.Name", "EAbilityInputID::PrimaryAttack" },
		{ "PrimaryAttack.ToolTip", "---- Appended (safe) ----" },
		{ "SecondaryAttack.DisplayName", "SecondaryAttack" },
		{ "SecondaryAttack.Name", "EAbilityInputID::SecondaryAttack" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Input slots for ability bar / activation binding.\n\nAAA rule:\n- NEVER change existing numeric values (Blueprints serialize the underlying byte).\n- Only append new entries at the end, OR explicitly assign numeric values to freeze them forever." },
#endif
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EAbilityInputID::None", (int64)EAbilityInputID::None },
		{ "EAbilityInputID::Ability1", (int64)EAbilityInputID::Ability1 },
		{ "EAbilityInputID::Ability2", (int64)EAbilityInputID::Ability2 },
		{ "EAbilityInputID::Ability3", (int64)EAbilityInputID::Ability3 },
		{ "EAbilityInputID::Ability4", (int64)EAbilityInputID::Ability4 },
		{ "EAbilityInputID::Ability5", (int64)EAbilityInputID::Ability5 },
		{ "EAbilityInputID::Ability6", (int64)EAbilityInputID::Ability6 },
		{ "EAbilityInputID::Ability7", (int64)EAbilityInputID::Ability7 },
		{ "EAbilityInputID::Ability8", (int64)EAbilityInputID::Ability8 },
		{ "EAbilityInputID::PrimaryAttack", (int64)EAbilityInputID::PrimaryAttack },
		{ "EAbilityInputID::SecondaryAttack", (int64)EAbilityInputID::SecondaryAttack },
		{ "EAbilityInputID::Cancel", (int64)EAbilityInputID::Cancel },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct Z_Construct_UEnum_SpellRise_EAbilityInputID_Statics 
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_SpellRise_EAbilityInputID_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_SpellRise,
	nullptr,
	"EAbilityInputID",
	"EAbilityInputID",
	Z_Construct_UEnum_SpellRise_EAbilityInputID_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_SpellRise_EAbilityInputID_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_SpellRise_EAbilityInputID_Statics::Enum_MetaDataParams), Z_Construct_UEnum_SpellRise_EAbilityInputID_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_SpellRise_EAbilityInputID()
{
	if (!Z_Registration_Info_UEnum_EAbilityInputID.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EAbilityInputID.InnerSingleton, Z_Construct_UEnum_SpellRise_EAbilityInputID_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_EAbilityInputID.InnerSingleton;
}
// ********** End Enum EAbilityInputID *************************************************************

// ********** Begin Enum ESpellRiseCommitPolicy ****************************************************
static FEnumRegistrationInfo Z_Registration_Info_UEnum_ESpellRiseCommitPolicy;
static UEnum* ESpellRiseCommitPolicy_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_ESpellRiseCommitPolicy.OuterSingleton)
	{
		Z_Registration_Info_UEnum_ESpellRiseCommitPolicy.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy, (UObject*)Z_Construct_UPackage__Script_SpellRise(), TEXT("ESpellRiseCommitPolicy"));
	}
	return Z_Registration_Info_UEnum_ESpellRiseCommitPolicy.OuterSingleton;
}
template<> SPELLRISE_NON_ATTRIBUTED_API UEnum* StaticEnum<ESpellRiseCommitPolicy>()
{
	return ESpellRiseCommitPolicy_StaticEnum();
}
struct Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "CommitOnFire.DisplayName", "Commit On Fire" },
		{ "CommitOnFire.Name", "ESpellRiseCommitPolicy::CommitOnFire" },
		{ "CommitOnStart.DisplayName", "Commit On Start" },
		{ "CommitOnStart.Name", "ESpellRiseCommitPolicy::CommitOnStart" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "ESpellRiseCommitPolicy::CommitOnStart", (int64)ESpellRiseCommitPolicy::CommitOnStart },
		{ "ESpellRiseCommitPolicy::CommitOnFire", (int64)ESpellRiseCommitPolicy::CommitOnFire },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy_Statics 
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_SpellRise,
	nullptr,
	"ESpellRiseCommitPolicy",
	"ESpellRiseCommitPolicy",
	Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy_Statics::Enum_MetaDataParams), Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy()
{
	if (!Z_Registration_Info_UEnum_ESpellRiseCommitPolicy.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_ESpellRiseCommitPolicy.InnerSingleton, Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_ESpellRiseCommitPolicy.InnerSingleton;
}
// ********** End Enum ESpellRiseCommitPolicy ******************************************************

// ********** Begin Class USpellRiseGameplayAbility Function ActivateSpellFlow *********************
struct Z_Construct_UFunction_USpellRiseGameplayAbility_ActivateSpellFlow_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Casting" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Convenience: if bUseCasting, cast; otherwise fire. */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Convenience: if bUseCasting, cast; otherwise fire." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function ActivateSpellFlow constinit property declarations *********************
// ********** End Function ActivateSpellFlow constinit property declarations ***********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_ActivateSpellFlow_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "ActivateSpellFlow", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_ActivateSpellFlow_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_ActivateSpellFlow_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_ActivateSpellFlow()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_ActivateSpellFlow_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseGameplayAbility::execActivateSpellFlow)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->ActivateSpellFlow();
	P_NATIVE_END;
}
// ********** End Class USpellRiseGameplayAbility Function ActivateSpellFlow ***********************

// ********** Begin Class USpellRiseGameplayAbility Function BP_CanFire ****************************
struct SpellRiseGameplayAbility_eventBP_CanFire_Parms
{
	bool ReturnValue;

	/** Constructor, initializes return property only **/
	SpellRiseGameplayAbility_eventBP_CanFire_Parms()
		: ReturnValue(false)
	{
	}
};
static FName NAME_USpellRiseGameplayAbility_BP_CanFire = FName(TEXT("BP_CanFire"));
bool USpellRiseGameplayAbility::BP_CanFire() const
{
	UFunction* Func = FindFunctionChecked(NAME_USpellRiseGameplayAbility_BP_CanFire);
	if (!Func->GetOwnerClass()->HasAnyClassFlags(CLASS_Native))
	{
		SpellRiseGameplayAbility_eventBP_CanFire_Parms Parms;
		const_cast<USpellRiseGameplayAbility*>(this)->ProcessEvent(Func,&Parms);
		return !!Parms.ReturnValue;
	}
	else
	{
		return const_cast<USpellRiseGameplayAbility*>(this)->BP_CanFire_Implementation();
	}
}
struct Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Casting|BP" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function BP_CanFire constinit property declarations ****************************
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function BP_CanFire constinit property declarations ******************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function BP_CanFire Property Definitions ***************************************
void Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((SpellRiseGameplayAbility_eventBP_CanFire_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseGameplayAbility_eventBP_CanFire_Parms), &Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::PropPointers) < 2048);
// ********** End Function BP_CanFire Property Definitions *****************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "BP_CanFire", 	Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::PropPointers), 
sizeof(SpellRiseGameplayAbility_eventBP_CanFire_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x48080C00, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(SpellRiseGameplayAbility_eventBP_CanFire_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseGameplayAbility::execBP_CanFire)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->BP_CanFire_Implementation();
	P_NATIVE_END;
}
// ********** End Class USpellRiseGameplayAbility Function BP_CanFire ******************************

// ********** Begin Class USpellRiseGameplayAbility Function BP_OnCastCancelled ********************
static FName NAME_USpellRiseGameplayAbility_BP_OnCastCancelled = FName(TEXT("BP_OnCastCancelled"));
void USpellRiseGameplayAbility::BP_OnCastCancelled()
{
	UFunction* Func = FindFunctionChecked(NAME_USpellRiseGameplayAbility_BP_OnCastCancelled);
	ProcessEvent(Func,NULL);
}
struct Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCancelled_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Casting|BP" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function BP_OnCastCancelled constinit property declarations ********************
// ********** End Function BP_OnCastCancelled constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCancelled_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "BP_OnCastCancelled", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08080800, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCancelled_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCancelled_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCancelled()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCancelled_Statics::FuncParams);
	}
	return ReturnFunction;
}
// ********** End Class USpellRiseGameplayAbility Function BP_OnCastCancelled **********************

// ********** Begin Class USpellRiseGameplayAbility Function BP_OnCastCompleted ********************
static FName NAME_USpellRiseGameplayAbility_BP_OnCastCompleted = FName(TEXT("BP_OnCastCompleted"));
void USpellRiseGameplayAbility::BP_OnCastCompleted()
{
	UFunction* Func = FindFunctionChecked(NAME_USpellRiseGameplayAbility_BP_OnCastCompleted);
	ProcessEvent(Func,NULL);
}
struct Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCompleted_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Casting|BP" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function BP_OnCastCompleted constinit property declarations ********************
// ********** End Function BP_OnCastCompleted constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCompleted_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "BP_OnCastCompleted", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08080800, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCompleted_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCompleted_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCompleted()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCompleted_Statics::FuncParams);
	}
	return ReturnFunction;
}
// ********** End Class USpellRiseGameplayAbility Function BP_OnCastCompleted **********************

// ********** Begin Class USpellRiseGameplayAbility Function BP_OnCastFired ************************
struct SpellRiseGameplayAbility_eventBP_OnCastFired_Parms
{
	float ChargeAlpha;
};
static FName NAME_USpellRiseGameplayAbility_BP_OnCastFired = FName(TEXT("BP_OnCastFired"));
void USpellRiseGameplayAbility::BP_OnCastFired(float ChargeAlpha)
{
	SpellRiseGameplayAbility_eventBP_OnCastFired_Parms Parms;
	Parms.ChargeAlpha=ChargeAlpha;
	UFunction* Func = FindFunctionChecked(NAME_USpellRiseGameplayAbility_BP_OnCastFired);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Casting|BP" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Fires the spell. ChargeAlpha is 1.0 when completed. */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Fires the spell. ChargeAlpha is 1.0 when completed." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function BP_OnCastFired constinit property declarations ************************
	static const UECodeGen_Private::FFloatPropertyParams NewProp_ChargeAlpha;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function BP_OnCastFired constinit property declarations **************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function BP_OnCastFired Property Definitions ***********************************
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired_Statics::NewProp_ChargeAlpha = { "ChargeAlpha", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseGameplayAbility_eventBP_OnCastFired_Parms, ChargeAlpha), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired_Statics::NewProp_ChargeAlpha,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired_Statics::PropPointers) < 2048);
// ********** End Function BP_OnCastFired Property Definitions *************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "BP_OnCastFired", 	Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired_Statics::PropPointers), 
sizeof(SpellRiseGameplayAbility_eventBP_OnCastFired_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08080800, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(SpellRiseGameplayAbility_eventBP_OnCastFired_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired_Statics::FuncParams);
	}
	return ReturnFunction;
}
// ********** End Class USpellRiseGameplayAbility Function BP_OnCastFired **************************

// ********** Begin Class USpellRiseGameplayAbility Function BP_OnCastStart ************************
static FName NAME_USpellRiseGameplayAbility_BP_OnCastStart = FName(TEXT("BP_OnCastStart"));
void USpellRiseGameplayAbility::BP_OnCastStart()
{
	UFunction* Func = FindFunctionChecked(NAME_USpellRiseGameplayAbility_BP_OnCastStart);
	ProcessEvent(Func,NULL);
}
struct Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastStart_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Casting|BP" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// BP hooks\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "BP hooks" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function BP_OnCastStart constinit property declarations ************************
// ********** End Function BP_OnCastStart constinit property declarations **************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastStart_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "BP_OnCastStart", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08080800, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastStart_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastStart_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastStart()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastStart_Statics::FuncParams);
	}
	return ReturnFunction;
}
// ********** End Class USpellRiseGameplayAbility Function BP_OnCastStart **************************

// ********** Begin Class USpellRiseGameplayAbility Function FireNow *******************************
struct Z_Construct_UFunction_USpellRiseGameplayAbility_FireNow_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Casting" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Fires immediately (ignores casting). */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Fires immediately (ignores casting)." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function FireNow constinit property declarations *******************************
// ********** End Function FireNow constinit property declarations *********************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_FireNow_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "FireNow", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_FireNow_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_FireNow_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_FireNow()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_FireNow_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseGameplayAbility::execFireNow)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->FireNow();
	P_NATIVE_END;
}
// ********** End Class USpellRiseGameplayAbility Function FireNow *********************************

// ********** Begin Class USpellRiseGameplayAbility Function GetInputID_Int ************************
struct Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics
{
	struct SpellRiseGameplayAbility_eventGetInputID_Int_Parms
	{
		int32 ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Debug helper */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Debug helper" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function GetInputID_Int constinit property declarations ************************
	static const UECodeGen_Private::FIntPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GetInputID_Int constinit property declarations **************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GetInputID_Int Property Definitions ***********************************
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseGameplayAbility_eventGetInputID_Int_Parms, ReturnValue), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::PropPointers) < 2048);
// ********** End Function GetInputID_Int Property Definitions *************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "GetInputID_Int", 	Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::SpellRiseGameplayAbility_eventGetInputID_Int_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::SpellRiseGameplayAbility_eventGetInputID_Int_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseGameplayAbility::execGetInputID_Int)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(int32*)Z_Param__Result=P_THIS->GetInputID_Int();
	P_NATIVE_END;
}
// ********** End Class USpellRiseGameplayAbility Function GetInputID_Int **************************

// ********** Begin Class USpellRiseGameplayAbility Function HasPC *********************************
struct Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics
{
	struct SpellRiseGameplayAbility_eventHasPC_Parms
	{
		bool ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Helpers" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function HasPC constinit property declarations *********************************
	static void NewProp_ReturnValue_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function HasPC constinit property declarations ***********************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function HasPC Property Definitions ********************************************
void Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::NewProp_ReturnValue_SetBit(void* Obj)
{
	((SpellRiseGameplayAbility_eventHasPC_Parms*)Obj)->ReturnValue = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(SpellRiseGameplayAbility_eventHasPC_Parms), &Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::PropPointers) < 2048);
// ********** End Function HasPC Property Definitions **********************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "HasPC", 	Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::SpellRiseGameplayAbility_eventHasPC_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::SpellRiseGameplayAbility_eventHasPC_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseGameplayAbility::execHasPC)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result=P_THIS->HasPC();
	P_NATIVE_END;
}
// ********** End Class USpellRiseGameplayAbility Function HasPC ***********************************

// ********** Begin Class USpellRiseGameplayAbility Function OnInputReleased ***********************
struct Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics
{
	struct SpellRiseGameplayAbility_eventOnInputReleased_Parms
	{
		float HeldTime;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function OnInputReleased constinit property declarations ***********************
	static const UECodeGen_Private::FFloatPropertyParams NewProp_HeldTime;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function OnInputReleased constinit property declarations *************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function OnInputReleased Property Definitions **********************************
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::NewProp_HeldTime = { "HeldTime", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseGameplayAbility_eventOnInputReleased_Parms, HeldTime), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::NewProp_HeldTime,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::PropPointers) < 2048);
// ********** End Function OnInputReleased Property Definitions ************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "OnInputReleased", 	Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::SpellRiseGameplayAbility_eventOnInputReleased_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00040401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::SpellRiseGameplayAbility_eventOnInputReleased_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseGameplayAbility::execOnInputReleased)
{
	P_GET_PROPERTY(FFloatProperty,Z_Param_HeldTime);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->OnInputReleased(Z_Param_HeldTime);
	P_NATIVE_END;
}
// ********** End Class USpellRiseGameplayAbility Function OnInputReleased *************************

// ********** Begin Class USpellRiseGameplayAbility Function SetAbilityLevel ***********************
struct Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics
{
	struct SpellRiseGameplayAbility_eventSetAbilityLevel_Parms
	{
		int32 NewLevel;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Ability" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function SetAbilityLevel constinit property declarations ***********************
	static const UECodeGen_Private::FIntPropertyParams NewProp_NewLevel;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function SetAbilityLevel constinit property declarations *************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function SetAbilityLevel Property Definitions **********************************
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::NewProp_NewLevel = { "NewLevel", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(SpellRiseGameplayAbility_eventSetAbilityLevel_Parms, NewLevel), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::NewProp_NewLevel,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::PropPointers) < 2048);
// ********** End Function SetAbilityLevel Property Definitions ************************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "SetAbilityLevel", 	Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::SpellRiseGameplayAbility_eventSetAbilityLevel_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::SpellRiseGameplayAbility_eventSetAbilityLevel_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseGameplayAbility::execSetAbilityLevel)
{
	P_GET_PROPERTY(FIntProperty,Z_Param_NewLevel);
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->SetAbilityLevel(Z_Param_NewLevel);
	P_NATIVE_END;
}
// ********** End Class USpellRiseGameplayAbility Function SetAbilityLevel *************************

// ********** Begin Class USpellRiseGameplayAbility Function StartCastingFlow **********************
struct Z_Construct_UFunction_USpellRiseGameplayAbility_StartCastingFlow_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Casting" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Starts hold-to-cast flow. */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Starts hold-to-cast flow." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function StartCastingFlow constinit property declarations **********************
// ********** End Function StartCastingFlow constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_USpellRiseGameplayAbility_StartCastingFlow_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_USpellRiseGameplayAbility, nullptr, "StartCastingFlow", 	nullptr, 
	0, 
0,
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_USpellRiseGameplayAbility_StartCastingFlow_Statics::Function_MetaDataParams), Z_Construct_UFunction_USpellRiseGameplayAbility_StartCastingFlow_Statics::Function_MetaDataParams)},  };
UFunction* Z_Construct_UFunction_USpellRiseGameplayAbility_StartCastingFlow()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_USpellRiseGameplayAbility_StartCastingFlow_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(USpellRiseGameplayAbility::execStartCastingFlow)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->StartCastingFlow();
	P_NATIVE_END;
}
// ********** End Class USpellRiseGameplayAbility Function StartCastingFlow ************************

// ********** Begin Class USpellRiseGameplayAbility ************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_USpellRiseGameplayAbility;
UClass* USpellRiseGameplayAbility::GetPrivateStaticClass()
{
	using TClass = USpellRiseGameplayAbility;
	if (!Z_Registration_Info_UClass_USpellRiseGameplayAbility.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("SpellRiseGameplayAbility"),
			Z_Registration_Info_UClass_USpellRiseGameplayAbility.InnerSingleton,
			StaticRegisterNativesUSpellRiseGameplayAbility,
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
	return Z_Registration_Info_UClass_USpellRiseGameplayAbility.InnerSingleton;
}
UClass* Z_Construct_UClass_USpellRiseGameplayAbility_NoRegister()
{
	return USpellRiseGameplayAbility::GetPrivateStaticClass();
}
struct Z_Construct_UClass_USpellRiseGameplayAbility_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ShouldShowInAbilityBar_MetaData[] = {
		{ "Category", "UI" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// ==========================\n// UI / Input\n// ==========================\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "UI / Input" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AutoActivateWhenGranted_MetaData[] = {
		{ "Category", "Activation" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AbilityInputID_MetaData[] = {
		{ "Category", "Input" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Slot used in GiveAbility: becomes InputID in FGameplayAbilitySpec. */" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Slot used in GiveAbility: becomes InputID in FGameplayAbilitySpec." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bUseCasting_MetaData[] = {
		{ "Category", "Casting" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// ==========================\n// Casting (opt-in)\n// ==========================\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Casting (opt-in)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CastTime_MetaData[] = {
		{ "Category", "Casting" },
		{ "ClampMin", "0.0" },
		{ "EditCondition", "bUseCasting" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bCancelIfReleasedEarly_MetaData[] = {
		{ "Category", "Casting" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n\x09 * Design:\n\x09 * - FALSE: release before completion -> keep casting -> fire when complete\n\x09 * - TRUE : release before completion -> cancel immediately\n\x09 */" },
#endif
		{ "EditCondition", "bUseCasting" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Design:\n- FALSE: release before completion -> keep casting -> fire when complete\n- TRUE : release before completion -> cancel immediately" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CommitPolicy_MetaData[] = {
		{ "Category", "Casting" },
		{ "EditCondition", "bUseCasting" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CastingStateTag_MetaData[] = {
		{ "Category", "Casting" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Tag applied during casting. Default: State.Casting */" },
#endif
		{ "EditCondition", "bUseCasting" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Tag applied during casting. Default: State.Casting" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bCastCompleted_MetaData[] = {
		{ "Category", "Casting|State" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// ==========================\n// Runtime state (exposed for UI/debug)\n// ==========================\n" },
#endif
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Runtime state (exposed for UI/debug)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bInputReleased_MetaData[] = {
		{ "Category", "Casting|State" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TimeHeld_MetaData[] = {
		{ "Category", "Casting|State" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WaitReleaseTask_MetaData[] = {
		{ "ModuleRelativePath", "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class USpellRiseGameplayAbility constinit property declarations ****************
	static void NewProp_ShouldShowInAbilityBar_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ShouldShowInAbilityBar;
	static void NewProp_AutoActivateWhenGranted_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_AutoActivateWhenGranted;
	static const UECodeGen_Private::FBytePropertyParams NewProp_AbilityInputID_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_AbilityInputID;
	static void NewProp_bUseCasting_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bUseCasting;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_CastTime;
	static void NewProp_bCancelIfReleasedEarly_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bCancelIfReleasedEarly;
	static const UECodeGen_Private::FBytePropertyParams NewProp_CommitPolicy_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_CommitPolicy;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CastingStateTag;
	static void NewProp_bCastCompleted_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bCastCompleted;
	static void NewProp_bInputReleased_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bInputReleased;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_TimeHeld;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WaitReleaseTask;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class USpellRiseGameplayAbility constinit property declarations ******************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("ActivateSpellFlow"), .Pointer = &USpellRiseGameplayAbility::execActivateSpellFlow },
		{ .NameUTF8 = UTF8TEXT("BP_CanFire"), .Pointer = &USpellRiseGameplayAbility::execBP_CanFire },
		{ .NameUTF8 = UTF8TEXT("FireNow"), .Pointer = &USpellRiseGameplayAbility::execFireNow },
		{ .NameUTF8 = UTF8TEXT("GetInputID_Int"), .Pointer = &USpellRiseGameplayAbility::execGetInputID_Int },
		{ .NameUTF8 = UTF8TEXT("HasPC"), .Pointer = &USpellRiseGameplayAbility::execHasPC },
		{ .NameUTF8 = UTF8TEXT("OnInputReleased"), .Pointer = &USpellRiseGameplayAbility::execOnInputReleased },
		{ .NameUTF8 = UTF8TEXT("SetAbilityLevel"), .Pointer = &USpellRiseGameplayAbility::execSetAbilityLevel },
		{ .NameUTF8 = UTF8TEXT("StartCastingFlow"), .Pointer = &USpellRiseGameplayAbility::execStartCastingFlow },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_ActivateSpellFlow, "ActivateSpellFlow" }, // 46978763
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_BP_CanFire, "BP_CanFire" }, // 2472696068
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCancelled, "BP_OnCastCancelled" }, // 2032109586
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastCompleted, "BP_OnCastCompleted" }, // 902118981
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastFired, "BP_OnCastFired" }, // 322756521
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_BP_OnCastStart, "BP_OnCastStart" }, // 1346407364
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_FireNow, "FireNow" }, // 2686670426
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_GetInputID_Int, "GetInputID_Int" }, // 568674011
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_HasPC, "HasPC" }, // 3866254497
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_OnInputReleased, "OnInputReleased" }, // 2639500192
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_SetAbilityLevel, "SetAbilityLevel" }, // 1073705864
		{ &Z_Construct_UFunction_USpellRiseGameplayAbility_StartCastingFlow, "StartCastingFlow" }, // 2262182792
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USpellRiseGameplayAbility>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_USpellRiseGameplayAbility_Statics

// ********** Begin Class USpellRiseGameplayAbility Property Definitions ***************************
void Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_ShouldShowInAbilityBar_SetBit(void* Obj)
{
	((USpellRiseGameplayAbility*)Obj)->ShouldShowInAbilityBar = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_ShouldShowInAbilityBar = { "ShouldShowInAbilityBar", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(USpellRiseGameplayAbility), &Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_ShouldShowInAbilityBar_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ShouldShowInAbilityBar_MetaData), NewProp_ShouldShowInAbilityBar_MetaData) };
void Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_AutoActivateWhenGranted_SetBit(void* Obj)
{
	((USpellRiseGameplayAbility*)Obj)->AutoActivateWhenGranted = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_AutoActivateWhenGranted = { "AutoActivateWhenGranted", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(USpellRiseGameplayAbility), &Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_AutoActivateWhenGranted_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AutoActivateWhenGranted_MetaData), NewProp_AutoActivateWhenGranted_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_AbilityInputID_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_AbilityInputID = { "AbilityInputID", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseGameplayAbility, AbilityInputID), Z_Construct_UEnum_SpellRise_EAbilityInputID, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AbilityInputID_MetaData), NewProp_AbilityInputID_MetaData) }; // 1853176719
void Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bUseCasting_SetBit(void* Obj)
{
	((USpellRiseGameplayAbility*)Obj)->bUseCasting = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bUseCasting = { "bUseCasting", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(USpellRiseGameplayAbility), &Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bUseCasting_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bUseCasting_MetaData), NewProp_bUseCasting_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_CastTime = { "CastTime", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseGameplayAbility, CastTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CastTime_MetaData), NewProp_CastTime_MetaData) };
void Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bCancelIfReleasedEarly_SetBit(void* Obj)
{
	((USpellRiseGameplayAbility*)Obj)->bCancelIfReleasedEarly = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bCancelIfReleasedEarly = { "bCancelIfReleasedEarly", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(USpellRiseGameplayAbility), &Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bCancelIfReleasedEarly_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bCancelIfReleasedEarly_MetaData), NewProp_bCancelIfReleasedEarly_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_CommitPolicy_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_CommitPolicy = { "CommitPolicy", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseGameplayAbility, CommitPolicy), Z_Construct_UEnum_SpellRise_ESpellRiseCommitPolicy, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CommitPolicy_MetaData), NewProp_CommitPolicy_MetaData) }; // 894723323
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_CastingStateTag = { "CastingStateTag", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseGameplayAbility, CastingStateTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CastingStateTag_MetaData), NewProp_CastingStateTag_MetaData) }; // 517357616
void Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bCastCompleted_SetBit(void* Obj)
{
	((USpellRiseGameplayAbility*)Obj)->bCastCompleted = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bCastCompleted = { "bCastCompleted", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(USpellRiseGameplayAbility), &Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bCastCompleted_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bCastCompleted_MetaData), NewProp_bCastCompleted_MetaData) };
void Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bInputReleased_SetBit(void* Obj)
{
	((USpellRiseGameplayAbility*)Obj)->bInputReleased = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bInputReleased = { "bInputReleased", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(USpellRiseGameplayAbility), &Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bInputReleased_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bInputReleased_MetaData), NewProp_bInputReleased_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_TimeHeld = { "TimeHeld", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseGameplayAbility, TimeHeld), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TimeHeld_MetaData), NewProp_TimeHeld_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_WaitReleaseTask = { "WaitReleaseTask", nullptr, (EPropertyFlags)0x0144000000002000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(USpellRiseGameplayAbility, WaitReleaseTask), Z_Construct_UClass_UAbilityTask_WaitInputRelease_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WaitReleaseTask_MetaData), NewProp_WaitReleaseTask_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_USpellRiseGameplayAbility_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_ShouldShowInAbilityBar,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_AutoActivateWhenGranted,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_AbilityInputID_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_AbilityInputID,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bUseCasting,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_CastTime,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bCancelIfReleasedEarly,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_CommitPolicy_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_CommitPolicy,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_CastingStateTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bCastCompleted,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_bInputReleased,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_TimeHeld,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USpellRiseGameplayAbility_Statics::NewProp_WaitReleaseTask,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGameplayAbility_Statics::PropPointers) < 2048);
// ********** End Class USpellRiseGameplayAbility Property Definitions *****************************
UObject* (*const Z_Construct_UClass_USpellRiseGameplayAbility_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UGameplayAbility,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGameplayAbility_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_USpellRiseGameplayAbility_Statics::ClassParams = {
	&USpellRiseGameplayAbility::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_USpellRiseGameplayAbility_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGameplayAbility_Statics::PropPointers),
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_USpellRiseGameplayAbility_Statics::Class_MetaDataParams), Z_Construct_UClass_USpellRiseGameplayAbility_Statics::Class_MetaDataParams)
};
void USpellRiseGameplayAbility::StaticRegisterNativesUSpellRiseGameplayAbility()
{
	UClass* Class = USpellRiseGameplayAbility::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_USpellRiseGameplayAbility_Statics::Funcs));
}
UClass* Z_Construct_UClass_USpellRiseGameplayAbility()
{
	if (!Z_Registration_Info_UClass_USpellRiseGameplayAbility.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_USpellRiseGameplayAbility.OuterSingleton, Z_Construct_UClass_USpellRiseGameplayAbility_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_USpellRiseGameplayAbility.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, USpellRiseGameplayAbility);
USpellRiseGameplayAbility::~USpellRiseGameplayAbility() {}
// ********** End Class USpellRiseGameplayAbility **************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h__Script_SpellRise_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ EAbilityInputID_StaticEnum, TEXT("EAbilityInputID"), &Z_Registration_Info_UEnum_EAbilityInputID, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 1853176719U) },
		{ ESpellRiseCommitPolicy_StaticEnum, TEXT("ESpellRiseCommitPolicy"), &Z_Registration_Info_UEnum_ESpellRiseCommitPolicy, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 894723323U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_USpellRiseGameplayAbility, USpellRiseGameplayAbility::StaticClass, TEXT("USpellRiseGameplayAbility"), &Z_Registration_Info_UClass_USpellRiseGameplayAbility, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(USpellRiseGameplayAbility), 2511588101U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h__Script_SpellRise_3289184276{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h__Script_SpellRise_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h__Script_SpellRise_Statics::EnumInfo),
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
