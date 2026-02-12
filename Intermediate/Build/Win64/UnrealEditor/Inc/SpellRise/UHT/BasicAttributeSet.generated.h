// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GameplayAbilitySystem/AttributeSets/BasicAttributeSet.h"

#ifdef SPELLRISE_BasicAttributeSet_generated_h
#error "BasicAttributeSet.generated.h already included, missing '#pragma once' in BasicAttributeSet.h"
#endif
#define SPELLRISE_BasicAttributeSet_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Net/Core/PushModel/PushModelMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FGameplayAttributeData;

// ********** Begin Class UBasicAttributeSet *******************************************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h_17_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execOnRep_Armor); \
	DECLARE_FUNCTION(execOnRep_ElementalResistance); \
	DECLARE_FUNCTION(execOnRep_MagicResistance); \
	DECLARE_FUNCTION(execOnRep_PhysicalResistance); \
	DECLARE_FUNCTION(execOnRep_CarryWeight); \
	DECLARE_FUNCTION(execOnRep_LightningResistance); \
	DECLARE_FUNCTION(execOnRep_FrostResistance); \
	DECLARE_FUNCTION(execOnRep_FireResistance);


struct Z_Construct_UClass_UBasicAttributeSet_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_UBasicAttributeSet_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h_17_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUBasicAttributeSet(); \
	friend struct ::Z_Construct_UClass_UBasicAttributeSet_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_UBasicAttributeSet_NoRegister(); \
public: \
	DECLARE_CLASS2(UBasicAttributeSet, UAttributeSet, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_UBasicAttributeSet_NoRegister) \
	DECLARE_SERIALIZER(UBasicAttributeSet) \
	enum class ENetFields_Private : uint16 \
	{ \
		NETFIELD_REP_START=(uint16)((int32)Super::ENetFields_Private::NETFIELD_REP_END + (int32)1), \
		CarryWeight=NETFIELD_REP_START, \
		PhysicalResistance, \
		MagicResistance, \
		ElementalResistance, \
		FireResistance, \
		FrostResistance, \
		LightningResistance, \
		Armor, \
		NETFIELD_REP_END=Armor	}; \
	DECLARE_VALIDATE_GENERATED_REP_ENUMS(NO_API) \
private: \
	REPLICATED_BASE_CLASS(UBasicAttributeSet) \
public:


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h_17_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UBasicAttributeSet(UBasicAttributeSet&&) = delete; \
	UBasicAttributeSet(const UBasicAttributeSet&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UBasicAttributeSet); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UBasicAttributeSet); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UBasicAttributeSet) \
	NO_API virtual ~UBasicAttributeSet();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h_14_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h_17_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h_17_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h_17_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h_17_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UBasicAttributeSet;

// ********** End Class UBasicAttributeSet *********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_BasicAttributeSet_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
