// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Weapons/Data/DA_WeaponDefinition.h"
#include "GameplayTagContainer.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeDA_WeaponDefinition() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UClass_NoRegister();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FTransform();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UAnimMontage_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UDataAsset();
GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
SPELLRISE_API UClass* Z_Construct_UClass_UDA_WeaponDefinition();
SPELLRISE_API UClass* Z_Construct_UClass_UDA_WeaponDefinition_NoRegister();
SPELLRISE_API UEnum* Z_Construct_UEnum_SpellRise_EWeaponAttackShape();
SPELLRISE_API UScriptStruct* Z_Construct_UScriptStruct_FWeaponAttackTuning();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Enum EWeaponAttackShape ********************************************************
static FEnumRegistrationInfo Z_Registration_Info_UEnum_EWeaponAttackShape;
static UEnum* EWeaponAttackShape_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_EWeaponAttackShape.OuterSingleton)
	{
		Z_Registration_Info_UEnum_EWeaponAttackShape.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_SpellRise_EWeaponAttackShape, (UObject*)Z_Construct_UPackage__Script_SpellRise(), TEXT("EWeaponAttackShape"));
	}
	return Z_Registration_Info_UEnum_EWeaponAttackShape.OuterSingleton;
}
template<> SPELLRISE_NON_ATTRIBUTED_API UEnum* StaticEnum<EWeaponAttackShape>()
{
	return EWeaponAttackShape_StaticEnum();
}
struct Z_Construct_UEnum_SpellRise_EWeaponAttackShape_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "Box.DisplayName", "Box" },
		{ "Box.Name", "EWeaponAttackShape::Box" },
		{ "Capsule.DisplayName", "Capsule" },
		{ "Capsule.Name", "EWeaponAttackShape::Capsule" },
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
		{ "Sphere.DisplayName", "Sphere" },
		{ "Sphere.Name", "EWeaponAttackShape::Sphere" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EWeaponAttackShape::Sphere", (int64)EWeaponAttackShape::Sphere },
		{ "EWeaponAttackShape::Capsule", (int64)EWeaponAttackShape::Capsule },
		{ "EWeaponAttackShape::Box", (int64)EWeaponAttackShape::Box },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct Z_Construct_UEnum_SpellRise_EWeaponAttackShape_Statics 
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_SpellRise_EWeaponAttackShape_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_SpellRise,
	nullptr,
	"EWeaponAttackShape",
	"EWeaponAttackShape",
	Z_Construct_UEnum_SpellRise_EWeaponAttackShape_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_SpellRise_EWeaponAttackShape_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_SpellRise_EWeaponAttackShape_Statics::Enum_MetaDataParams), Z_Construct_UEnum_SpellRise_EWeaponAttackShape_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_SpellRise_EWeaponAttackShape()
{
	if (!Z_Registration_Info_UEnum_EWeaponAttackShape.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EWeaponAttackShape.InnerSingleton, Z_Construct_UEnum_SpellRise_EWeaponAttackShape_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_EWeaponAttackShape.InnerSingleton;
}
// ********** End Enum EWeaponAttackShape **********************************************************

// ********** Begin ScriptStruct FWeaponAttackTuning ***********************************************
struct Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics
{
	static inline consteval int32 GetStructSize() { return sizeof(FWeaponAttackTuning); }
	static inline consteval int16 GetStructAlignment() { return alignof(FWeaponAttackTuning); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BaseDamage_MetaData[] = {
		{ "Category", "Weapon|Damage" },
		{ "ClampMin", "0.0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Base weapon damage */" },
#endif
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Base weapon damage" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DamageScalar_MetaData[] = {
		{ "Category", "Weapon|Damage" },
		{ "ClampMin", "0.0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Damage scalar (rarity/quality) */" },
#endif
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Damage scalar (rarity/quality)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DamageScalingPerHit_MetaData[] = {
		{ "Category", "Weapon|Damage" },
		{ "ClampMin", "0.0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Damage scaling per combo hit */" },
#endif
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Damage scaling per combo hit" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Range_MetaData[] = {
		{ "Category", "Weapon|Trace" },
		{ "ClampMin", "0.0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Trace range */" },
#endif
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Trace range" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AttackShape_MetaData[] = {
		{ "Category", "Weapon|Trace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Attack shape */" },
#endif
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Attack shape" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SphereRadius_MetaData[] = {
		{ "Category", "Weapon|Trace" },
		{ "ClampMin", "0.0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Sphere radius */" },
#endif
		{ "EditCondition", "AttackShape==EWeaponAttackShape::Sphere" },
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Sphere radius" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CapsuleRadius_MetaData[] = {
		{ "Category", "Weapon|Trace" },
		{ "ClampMin", "0.0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Capsule radius */" },
#endif
		{ "EditCondition", "AttackShape==EWeaponAttackShape::Capsule" },
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Capsule radius" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CapsuleHalfHeight_MetaData[] = {
		{ "Category", "Weapon|Trace" },
		{ "ClampMin", "0.0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Capsule half height */" },
#endif
		{ "EditCondition", "AttackShape==EWeaponAttackShape::Capsule" },
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Capsule half height" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BoxExtent_MetaData[] = {
		{ "Category", "Weapon|Trace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Box extent */" },
#endif
		{ "EditCondition", "AttackShape==EWeaponAttackShape::Box" },
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Box extent" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DamageTypeTag_MetaData[] = {
		{ "Category", "Weapon|Tags" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Damage type tag */" },
#endif
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Damage type tag" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AttackMontage_MetaData[] = {
		{ "Category", "Weapon|Animation" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Attack montage */" },
#endif
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Attack montage" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FWeaponAttackTuning constinit property declarations ***************
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BaseDamage;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_DamageScalar;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_DamageScalingPerHit_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_DamageScalingPerHit;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Range;
	static const UECodeGen_Private::FBytePropertyParams NewProp_AttackShape_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_AttackShape;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_SphereRadius;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_CapsuleRadius;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_CapsuleHalfHeight;
	static const UECodeGen_Private::FStructPropertyParams NewProp_BoxExtent;
	static const UECodeGen_Private::FStructPropertyParams NewProp_DamageTypeTag;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_AttackMontage;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FWeaponAttackTuning constinit property declarations *****************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FWeaponAttackTuning>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FWeaponAttackTuning;
class UScriptStruct* FWeaponAttackTuning::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FWeaponAttackTuning.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FWeaponAttackTuning.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FWeaponAttackTuning, (UObject*)Z_Construct_UPackage__Script_SpellRise(), TEXT("WeaponAttackTuning"));
	}
	return Z_Registration_Info_UScriptStruct_FWeaponAttackTuning.OuterSingleton;
	}

// ********** Begin ScriptStruct FWeaponAttackTuning Property Definitions **************************
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_BaseDamage = { "BaseDamage", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponAttackTuning, BaseDamage), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BaseDamage_MetaData), NewProp_BaseDamage_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_DamageScalar = { "DamageScalar", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponAttackTuning, DamageScalar), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DamageScalar_MetaData), NewProp_DamageScalar_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_DamageScalingPerHit_Inner = { "DamageScalingPerHit", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_DamageScalingPerHit = { "DamageScalingPerHit", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponAttackTuning, DamageScalingPerHit), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DamageScalingPerHit_MetaData), NewProp_DamageScalingPerHit_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_Range = { "Range", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponAttackTuning, Range), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Range_MetaData), NewProp_Range_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_AttackShape_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_AttackShape = { "AttackShape", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponAttackTuning, AttackShape), Z_Construct_UEnum_SpellRise_EWeaponAttackShape, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AttackShape_MetaData), NewProp_AttackShape_MetaData) }; // 1328068482
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_SphereRadius = { "SphereRadius", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponAttackTuning, SphereRadius), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SphereRadius_MetaData), NewProp_SphereRadius_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_CapsuleRadius = { "CapsuleRadius", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponAttackTuning, CapsuleRadius), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CapsuleRadius_MetaData), NewProp_CapsuleRadius_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_CapsuleHalfHeight = { "CapsuleHalfHeight", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponAttackTuning, CapsuleHalfHeight), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CapsuleHalfHeight_MetaData), NewProp_CapsuleHalfHeight_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_BoxExtent = { "BoxExtent", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponAttackTuning, BoxExtent), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BoxExtent_MetaData), NewProp_BoxExtent_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_DamageTypeTag = { "DamageTypeTag", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponAttackTuning, DamageTypeTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DamageTypeTag_MetaData), NewProp_DamageTypeTag_MetaData) }; // 517357616
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_AttackMontage = { "AttackMontage", nullptr, (EPropertyFlags)0x0114000000000015, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FWeaponAttackTuning, AttackMontage), Z_Construct_UClass_UAnimMontage_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AttackMontage_MetaData), NewProp_AttackMontage_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_BaseDamage,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_DamageScalar,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_DamageScalingPerHit_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_DamageScalingPerHit,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_Range,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_AttackShape_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_AttackShape,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_SphereRadius,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_CapsuleRadius,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_CapsuleHalfHeight,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_BoxExtent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_DamageTypeTag,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewProp_AttackMontage,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::PropPointers) < 2048);
// ********** End ScriptStruct FWeaponAttackTuning Property Definitions ****************************
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
	nullptr,
	&NewStructOps,
	"WeaponAttackTuning",
	Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::PropPointers),
	sizeof(FWeaponAttackTuning),
	alignof(FWeaponAttackTuning),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000201),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FWeaponAttackTuning()
{
	if (!Z_Registration_Info_UScriptStruct_FWeaponAttackTuning.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FWeaponAttackTuning.InnerSingleton, Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FWeaponAttackTuning.InnerSingleton);
}
// ********** End ScriptStruct FWeaponAttackTuning *************************************************

// ********** Begin Class UDA_WeaponDefinition Function GetLightAttackFinalBaseDamage **************
struct Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics
{
	struct DA_WeaponDefinition_eventGetLightAttackFinalBaseDamage_Parms
	{
		float ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Weapon" },
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
	};
#endif // WITH_METADATA

// ********** Begin Function GetLightAttackFinalBaseDamage constinit property declarations *********
	static const UECodeGen_Private::FFloatPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GetLightAttackFinalBaseDamage constinit property declarations ***********
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GetLightAttackFinalBaseDamage Property Definitions ********************
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(DA_WeaponDefinition_eventGetLightAttackFinalBaseDamage_Parms, ReturnValue), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::PropPointers) < 2048);
// ********** End Function GetLightAttackFinalBaseDamage Property Definitions **********************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UDA_WeaponDefinition, nullptr, "GetLightAttackFinalBaseDamage", 	Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::DA_WeaponDefinition_eventGetLightAttackFinalBaseDamage_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::Function_MetaDataParams), Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::DA_WeaponDefinition_eventGetLightAttackFinalBaseDamage_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UDA_WeaponDefinition::execGetLightAttackFinalBaseDamage)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(float*)Z_Param__Result=P_THIS->GetLightAttackFinalBaseDamage();
	P_NATIVE_END;
}
// ********** End Class UDA_WeaponDefinition Function GetLightAttackFinalBaseDamage ****************

// ********** Begin Class UDA_WeaponDefinition *****************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UDA_WeaponDefinition;
UClass* UDA_WeaponDefinition::GetPrivateStaticClass()
{
	using TClass = UDA_WeaponDefinition;
	if (!Z_Registration_Info_UClass_UDA_WeaponDefinition.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("DA_WeaponDefinition"),
			Z_Registration_Info_UClass_UDA_WeaponDefinition.InnerSingleton,
			StaticRegisterNativesUDA_WeaponDefinition,
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
	return Z_Registration_Info_UClass_UDA_WeaponDefinition.InnerSingleton;
}
UClass* Z_Construct_UClass_UDA_WeaponDefinition_NoRegister()
{
	return UDA_WeaponDefinition::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UDA_WeaponDefinition_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "Weapons/Data/DA_WeaponDefinition.h" },
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DisplayName_MetaData[] = {
		{ "Category", "Weapon" },
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WeaponId_MetaData[] = {
		{ "Category", "Weapon" },
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LightAttack_MetaData[] = {
		{ "Category", "Weapon|Attacks" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Light attack tuning */" },
#endif
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Light attack tuning" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WeaponActorClass_MetaData[] = {
		{ "Category", "Weapon|Visual" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Weapon actor class to spawn */" },
#endif
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Weapon actor class to spawn" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AttachSocketName_MetaData[] = {
		{ "Category", "Weapon|Visual" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Socket to attach weapon */" },
#endif
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Socket to attach weapon" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AttachOffset_MetaData[] = {
		{ "Category", "Weapon|Visual" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Attachment offset */" },
#endif
		{ "ModuleRelativePath", "Weapons/Data/DA_WeaponDefinition.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Attachment offset" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UDA_WeaponDefinition constinit property declarations *********************
	static const UECodeGen_Private::FTextPropertyParams NewProp_DisplayName;
	static const UECodeGen_Private::FNamePropertyParams NewProp_WeaponId;
	static const UECodeGen_Private::FStructPropertyParams NewProp_LightAttack;
	static const UECodeGen_Private::FClassPropertyParams NewProp_WeaponActorClass;
	static const UECodeGen_Private::FNamePropertyParams NewProp_AttachSocketName;
	static const UECodeGen_Private::FStructPropertyParams NewProp_AttachOffset;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UDA_WeaponDefinition constinit property declarations ***********************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("GetLightAttackFinalBaseDamage"), .Pointer = &UDA_WeaponDefinition::execGetLightAttackFinalBaseDamage },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UDA_WeaponDefinition_GetLightAttackFinalBaseDamage, "GetLightAttackFinalBaseDamage" }, // 2009632035
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UDA_WeaponDefinition>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UDA_WeaponDefinition_Statics

// ********** Begin Class UDA_WeaponDefinition Property Definitions ********************************
const UECodeGen_Private::FTextPropertyParams Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_DisplayName = { "DisplayName", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Text, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDA_WeaponDefinition, DisplayName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DisplayName_MetaData), NewProp_DisplayName_MetaData) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_WeaponId = { "WeaponId", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDA_WeaponDefinition, WeaponId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WeaponId_MetaData), NewProp_WeaponId_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_LightAttack = { "LightAttack", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDA_WeaponDefinition, LightAttack), Z_Construct_UScriptStruct_FWeaponAttackTuning, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LightAttack_MetaData), NewProp_LightAttack_MetaData) }; // 455411927
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_WeaponActorClass = { "WeaponActorClass", nullptr, (EPropertyFlags)0x0014000000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDA_WeaponDefinition, WeaponActorClass), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WeaponActorClass_MetaData), NewProp_WeaponActorClass_MetaData) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_AttachSocketName = { "AttachSocketName", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDA_WeaponDefinition, AttachSocketName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AttachSocketName_MetaData), NewProp_AttachSocketName_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_AttachOffset = { "AttachOffset", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDA_WeaponDefinition, AttachOffset), Z_Construct_UScriptStruct_FTransform, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AttachOffset_MetaData), NewProp_AttachOffset_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UDA_WeaponDefinition_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_DisplayName,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_WeaponId,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_LightAttack,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_WeaponActorClass,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_AttachSocketName,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDA_WeaponDefinition_Statics::NewProp_AttachOffset,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDA_WeaponDefinition_Statics::PropPointers) < 2048);
// ********** End Class UDA_WeaponDefinition Property Definitions **********************************
UObject* (*const Z_Construct_UClass_UDA_WeaponDefinition_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UDataAsset,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDA_WeaponDefinition_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UDA_WeaponDefinition_Statics::ClassParams = {
	&UDA_WeaponDefinition::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UDA_WeaponDefinition_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UDA_WeaponDefinition_Statics::PropPointers),
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDA_WeaponDefinition_Statics::Class_MetaDataParams), Z_Construct_UClass_UDA_WeaponDefinition_Statics::Class_MetaDataParams)
};
void UDA_WeaponDefinition::StaticRegisterNativesUDA_WeaponDefinition()
{
	UClass* Class = UDA_WeaponDefinition::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UDA_WeaponDefinition_Statics::Funcs));
}
UClass* Z_Construct_UClass_UDA_WeaponDefinition()
{
	if (!Z_Registration_Info_UClass_UDA_WeaponDefinition.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UDA_WeaponDefinition.OuterSingleton, Z_Construct_UClass_UDA_WeaponDefinition_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UDA_WeaponDefinition.OuterSingleton;
}
UDA_WeaponDefinition::UDA_WeaponDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UDA_WeaponDefinition);
UDA_WeaponDefinition::~UDA_WeaponDefinition() {}
// ********** End Class UDA_WeaponDefinition *******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h__Script_SpellRise_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ EWeaponAttackShape_StaticEnum, TEXT("EWeaponAttackShape"), &Z_Registration_Info_UEnum_EWeaponAttackShape, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 1328068482U) },
	};
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FWeaponAttackTuning::StaticStruct, Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics::NewStructOps, TEXT("WeaponAttackTuning"),&Z_Registration_Info_UScriptStruct_FWeaponAttackTuning, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FWeaponAttackTuning), 455411927U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UDA_WeaponDefinition, UDA_WeaponDefinition::StaticClass, TEXT("UDA_WeaponDefinition"), &Z_Registration_Info_UClass_UDA_WeaponDefinition, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UDA_WeaponDefinition), 3284035933U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h__Script_SpellRise_2167646108{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h__Script_SpellRise_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h__Script_SpellRise_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h__Script_SpellRise_Statics::ScriptStructInfo),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h__Script_SpellRise_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h__Script_SpellRise_Statics::EnumInfo),
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
