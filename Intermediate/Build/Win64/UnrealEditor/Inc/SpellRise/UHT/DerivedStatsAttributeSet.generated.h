// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GameplayAbilitySystem/AttributeSets/DerivedStatsAttributeSet.h"

#ifdef SPELLRISE_DerivedStatsAttributeSet_generated_h
#error "DerivedStatsAttributeSet.generated.h already included, missing '#pragma once' in DerivedStatsAttributeSet.h"
#endif
#define SPELLRISE_DerivedStatsAttributeSet_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Net/Core/PushModel/PushModelMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FGameplayAttributeData;

// ********** Begin Class UDerivedStatsAttributeSet ************************************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h_27_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execOnRep_ElementPenetration); \
	DECLARE_FUNCTION(execOnRep_ArmorPenetration); \
	DECLARE_FUNCTION(execOnRep_CastStability); \
	DECLARE_FUNCTION(execOnRep_BreakPower); \
	DECLARE_FUNCTION(execOnRep_ManaCostMultiplier); \
	DECLARE_FUNCTION(execOnRep_CastSpeedMultiplier); \
	DECLARE_FUNCTION(execOnRep_SpellPowerMultiplier); \
	DECLARE_FUNCTION(execOnRep_CritDamage); \
	DECLARE_FUNCTION(execOnRep_CritChance); \
	DECLARE_FUNCTION(execOnRep_AttackSpeedMultiplier); \
	DECLARE_FUNCTION(execOnRep_WeaponDamageMultiplier);


struct Z_Construct_UClass_UDerivedStatsAttributeSet_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_UDerivedStatsAttributeSet_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h_27_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUDerivedStatsAttributeSet(); \
	friend struct ::Z_Construct_UClass_UDerivedStatsAttributeSet_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_UDerivedStatsAttributeSet_NoRegister(); \
public: \
	DECLARE_CLASS2(UDerivedStatsAttributeSet, UAttributeSet, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_UDerivedStatsAttributeSet_NoRegister) \
	DECLARE_SERIALIZER(UDerivedStatsAttributeSet) \
	enum class ENetFields_Private : uint16 \
	{ \
		NETFIELD_REP_START=(uint16)((int32)Super::ENetFields_Private::NETFIELD_REP_END + (int32)1), \
		WeaponDamageMultiplier=NETFIELD_REP_START, \
		AttackSpeedMultiplier, \
		CritChance, \
		CritDamage, \
		SpellPowerMultiplier, \
		CastSpeedMultiplier, \
		ManaCostMultiplier, \
		BreakPower, \
		CastStability, \
		ArmorPenetration, \
		ElementPenetration, \
		NETFIELD_REP_END=ElementPenetration	}; \
	DECLARE_VALIDATE_GENERATED_REP_ENUMS(NO_API) \
private: \
	REPLICATED_BASE_CLASS(UDerivedStatsAttributeSet) \
public:


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h_27_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UDerivedStatsAttributeSet(UDerivedStatsAttributeSet&&) = delete; \
	UDerivedStatsAttributeSet(const UDerivedStatsAttributeSet&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UDerivedStatsAttributeSet); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UDerivedStatsAttributeSet); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UDerivedStatsAttributeSet) \
	NO_API virtual ~UDerivedStatsAttributeSet();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h_24_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h_27_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h_27_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h_27_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h_27_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UDerivedStatsAttributeSet;

// ********** End Class UDerivedStatsAttributeSet **************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_DerivedStatsAttributeSet_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
