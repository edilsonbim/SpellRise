// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Weapons/Data/DA_WeaponDefinition.h"

#ifdef SPELLRISE_DA_WeaponDefinition_generated_h
#error "DA_WeaponDefinition.generated.h already included, missing '#pragma once' in DA_WeaponDefinition.h"
#endif
#define SPELLRISE_DA_WeaponDefinition_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin ScriptStruct FWeaponAttackTuning ***********************************************
struct Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics;
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h_22_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FWeaponAttackTuning_Statics; \
	static class UScriptStruct* StaticStruct();


struct FWeaponAttackTuning;
// ********** End ScriptStruct FWeaponAttackTuning *************************************************

// ********** Begin Class UDA_WeaponDefinition *****************************************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h_76_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execGetLightAttackFinalBaseDamage);


struct Z_Construct_UClass_UDA_WeaponDefinition_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_UDA_WeaponDefinition_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h_76_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUDA_WeaponDefinition(); \
	friend struct ::Z_Construct_UClass_UDA_WeaponDefinition_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_UDA_WeaponDefinition_NoRegister(); \
public: \
	DECLARE_CLASS2(UDA_WeaponDefinition, UDataAsset, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_UDA_WeaponDefinition_NoRegister) \
	DECLARE_SERIALIZER(UDA_WeaponDefinition)


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h_76_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UDA_WeaponDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UDA_WeaponDefinition(UDA_WeaponDefinition&&) = delete; \
	UDA_WeaponDefinition(const UDA_WeaponDefinition&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UDA_WeaponDefinition); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UDA_WeaponDefinition); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UDA_WeaponDefinition) \
	NO_API virtual ~UDA_WeaponDefinition();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h_73_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h_76_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h_76_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h_76_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h_76_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UDA_WeaponDefinition;

// ********** End Class UDA_WeaponDefinition *******************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Data_DA_WeaponDefinition_h

// ********** Begin Enum EWeaponAttackShape ********************************************************
#define FOREACH_ENUM_EWEAPONATTACKSHAPE(op) \
	op(EWeaponAttackShape::Sphere) \
	op(EWeaponAttackShape::Capsule) \
	op(EWeaponAttackShape::Box) 

enum class EWeaponAttackShape : uint8;
template<> struct TIsUEnumClass<EWeaponAttackShape> { enum { Value = true }; };
template<> SPELLRISE_NON_ATTRIBUTED_API UEnum* StaticEnum<EWeaponAttackShape>();
// ********** End Enum EWeaponAttackShape **********************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
