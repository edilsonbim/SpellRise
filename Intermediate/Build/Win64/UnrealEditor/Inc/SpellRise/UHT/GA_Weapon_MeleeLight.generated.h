// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Combat/Melee/Abilities/GA_Weapon_MeleeLight.h"

#ifdef SPELLRISE_GA_Weapon_MeleeLight_generated_h
#error "GA_Weapon_MeleeLight.generated.h already included, missing '#pragma once' in GA_Weapon_MeleeLight.h"
#endif
#define SPELLRISE_GA_Weapon_MeleeLight_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FGameplayEventData;

// ********** Begin Class UGA_Weapon_MeleeLight ****************************************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h_29_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execOnMontageInterrupted); \
	DECLARE_FUNCTION(execOnMontageCompleted); \
	DECLARE_FUNCTION(execOnComboWindowEnd); \
	DECLARE_FUNCTION(execOnComboWindowBegin);


struct Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_UGA_Weapon_MeleeLight_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h_29_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUGA_Weapon_MeleeLight(); \
	friend struct ::Z_Construct_UClass_UGA_Weapon_MeleeLight_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_UGA_Weapon_MeleeLight_NoRegister(); \
public: \
	DECLARE_CLASS2(UGA_Weapon_MeleeLight, USpellRiseGameplayAbility, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_UGA_Weapon_MeleeLight_NoRegister) \
	DECLARE_SERIALIZER(UGA_Weapon_MeleeLight)


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h_29_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UGA_Weapon_MeleeLight(UGA_Weapon_MeleeLight&&) = delete; \
	UGA_Weapon_MeleeLight(const UGA_Weapon_MeleeLight&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGA_Weapon_MeleeLight); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGA_Weapon_MeleeLight); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UGA_Weapon_MeleeLight) \
	NO_API virtual ~UGA_Weapon_MeleeLight();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h_26_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h_29_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h_29_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h_29_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h_29_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UGA_Weapon_MeleeLight;

// ********** End Class UGA_Weapon_MeleeLight ******************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
