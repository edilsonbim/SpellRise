// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GameplayAbilitySystem/AttributeSets/ResourceAttributeSet.h"

#ifdef SPELLRISE_ResourceAttributeSet_generated_h
#error "ResourceAttributeSet.generated.h already included, missing '#pragma once' in ResourceAttributeSet.h"
#endif
#define SPELLRISE_ResourceAttributeSet_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Net/Core/PushModel/PushModelMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FGameplayAttributeData;

// ********** Begin Class UResourceAttributeSet ****************************************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h_19_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execOnRep_StaminaRegen); \
	DECLARE_FUNCTION(execOnRep_ManaRegen); \
	DECLARE_FUNCTION(execOnRep_HealthRegen); \
	DECLARE_FUNCTION(execOnRep_MaxShield); \
	DECLARE_FUNCTION(execOnRep_Shield); \
	DECLARE_FUNCTION(execOnRep_MaxStamina); \
	DECLARE_FUNCTION(execOnRep_Stamina); \
	DECLARE_FUNCTION(execOnRep_MaxMana); \
	DECLARE_FUNCTION(execOnRep_Mana); \
	DECLARE_FUNCTION(execOnRep_MaxHealth); \
	DECLARE_FUNCTION(execOnRep_Health);


struct Z_Construct_UClass_UResourceAttributeSet_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_UResourceAttributeSet_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h_19_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUResourceAttributeSet(); \
	friend struct ::Z_Construct_UClass_UResourceAttributeSet_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_UResourceAttributeSet_NoRegister(); \
public: \
	DECLARE_CLASS2(UResourceAttributeSet, UAttributeSet, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_UResourceAttributeSet_NoRegister) \
	DECLARE_SERIALIZER(UResourceAttributeSet) \
	enum class ENetFields_Private : uint16 \
	{ \
		NETFIELD_REP_START=(uint16)((int32)Super::ENetFields_Private::NETFIELD_REP_END + (int32)1), \
		Health=NETFIELD_REP_START, \
		MaxHealth, \
		Mana, \
		MaxMana, \
		Stamina, \
		MaxStamina, \
		Shield, \
		MaxShield, \
		HealthRegen, \
		ManaRegen, \
		StaminaRegen, \
		NETFIELD_REP_END=StaminaRegen	}; \
	DECLARE_VALIDATE_GENERATED_REP_ENUMS(NO_API) \
private: \
	REPLICATED_BASE_CLASS(UResourceAttributeSet) \
public:


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h_19_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UResourceAttributeSet(UResourceAttributeSet&&) = delete; \
	UResourceAttributeSet(const UResourceAttributeSet&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UResourceAttributeSet); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UResourceAttributeSet); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UResourceAttributeSet) \
	NO_API virtual ~UResourceAttributeSet();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h_16_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h_19_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h_19_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h_19_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h_19_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UResourceAttributeSet;

// ********** End Class UResourceAttributeSet ******************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_ResourceAttributeSet_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
