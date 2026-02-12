// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GameplayAbilitySystem/Abilities/SpellRiseGameplayAbility.h"

#ifdef SPELLRISE_SpellRiseGameplayAbility_generated_h
#error "SpellRiseGameplayAbility.generated.h already included, missing '#pragma once' in SpellRiseGameplayAbility.h"
#endif
#define SPELLRISE_SpellRiseGameplayAbility_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class USpellRiseGameplayAbility ************************************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h_47_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execHasPC); \
	DECLARE_FUNCTION(execOnInputReleased); \
	DECLARE_FUNCTION(execGetInputID_Int); \
	DECLARE_FUNCTION(execBP_CanFire); \
	DECLARE_FUNCTION(execActivateSpellFlow); \
	DECLARE_FUNCTION(execFireNow); \
	DECLARE_FUNCTION(execStartCastingFlow); \
	DECLARE_FUNCTION(execSetAbilityLevel);


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h_47_CALLBACK_WRAPPERS
struct Z_Construct_UClass_USpellRiseGameplayAbility_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseGameplayAbility_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h_47_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUSpellRiseGameplayAbility(); \
	friend struct ::Z_Construct_UClass_USpellRiseGameplayAbility_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_USpellRiseGameplayAbility_NoRegister(); \
public: \
	DECLARE_CLASS2(USpellRiseGameplayAbility, UGameplayAbility, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_USpellRiseGameplayAbility_NoRegister) \
	DECLARE_SERIALIZER(USpellRiseGameplayAbility)


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h_47_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	USpellRiseGameplayAbility(USpellRiseGameplayAbility&&) = delete; \
	USpellRiseGameplayAbility(const USpellRiseGameplayAbility&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, USpellRiseGameplayAbility); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(USpellRiseGameplayAbility); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(USpellRiseGameplayAbility) \
	NO_API virtual ~USpellRiseGameplayAbility();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h_44_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h_47_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h_47_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h_47_CALLBACK_WRAPPERS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h_47_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h_47_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class USpellRiseGameplayAbility;

// ********** End Class USpellRiseGameplayAbility **************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Abilities_SpellRiseGameplayAbility_h

// ********** Begin Enum EAbilityInputID ***********************************************************
#define FOREACH_ENUM_EABILITYINPUTID(op) \
	op(EAbilityInputID::None) \
	op(EAbilityInputID::Ability1) \
	op(EAbilityInputID::Ability2) \
	op(EAbilityInputID::Ability3) \
	op(EAbilityInputID::Ability4) \
	op(EAbilityInputID::Ability5) \
	op(EAbilityInputID::Ability6) \
	op(EAbilityInputID::Ability7) \
	op(EAbilityInputID::Ability8) \
	op(EAbilityInputID::PrimaryAttack) \
	op(EAbilityInputID::SecondaryAttack) \
	op(EAbilityInputID::Cancel) 

enum class EAbilityInputID : uint8;
template<> struct TIsUEnumClass<EAbilityInputID> { enum { Value = true }; };
template<> SPELLRISE_NON_ATTRIBUTED_API UEnum* StaticEnum<EAbilityInputID>();
// ********** End Enum EAbilityInputID *************************************************************

// ********** Begin Enum ESpellRiseCommitPolicy ****************************************************
#define FOREACH_ENUM_ESPELLRISECOMMITPOLICY(op) \
	op(ESpellRiseCommitPolicy::CommitOnStart) \
	op(ESpellRiseCommitPolicy::CommitOnFire) 

enum class ESpellRiseCommitPolicy : uint8;
template<> struct TIsUEnumClass<ESpellRiseCommitPolicy> { enum { Value = true }; };
template<> SPELLRISE_NON_ATTRIBUTED_API UEnum* StaticEnum<ESpellRiseCommitPolicy>();
// ********** End Enum ESpellRiseCommitPolicy ******************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
