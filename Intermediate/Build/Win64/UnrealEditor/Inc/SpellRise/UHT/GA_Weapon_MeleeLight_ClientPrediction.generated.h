// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Combat/Melee/Abilities/GA_Weapon_MeleeLight_ClientPrediction.h"

#ifdef SPELLRISE_GA_Weapon_MeleeLight_ClientPrediction_generated_h
#error "GA_Weapon_MeleeLight_ClientPrediction.generated.h already included, missing '#pragma once' in GA_Weapon_MeleeLight_ClientPrediction.h"
#endif
#define SPELLRISE_GA_Weapon_MeleeLight_ClientPrediction_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FCombatHitResult;
struct FGameplayEventData;

// ********** Begin ScriptStruct FCombatHitResult **************************************************
struct Z_Construct_UScriptStruct_FCombatHitResult_Statics;
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h_20_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FCombatHitResult_Statics; \
	SPELLRISE_API static class UScriptStruct* StaticStruct();


struct FCombatHitResult;
// ********** End ScriptStruct FCombatHitResult ****************************************************

// ********** Begin Class UGA_Weapon_MeleeLight_ClientPrediction ***********************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h_41_RPC_WRAPPERS_NO_PURE_DECLS \
	virtual void Client_ConfirmDamage_Implementation(FCombatHitResult const& HitResult); \
	virtual bool Server_ReportHitResult_Validate(FCombatHitResult const& ); \
	virtual void Server_ReportHitResult_Implementation(FCombatHitResult const& HitResult); \
	DECLARE_FUNCTION(execHandleMontageCancelled); \
	DECLARE_FUNCTION(execHandleMontageInterrupted); \
	DECLARE_FUNCTION(execHandleMontageCompleted); \
	DECLARE_FUNCTION(execHandleComboWindowEnd); \
	DECLARE_FUNCTION(execHandleComboWindowBegin); \
	DECLARE_FUNCTION(execClient_ConfirmDamage); \
	DECLARE_FUNCTION(execServer_ReportHitResult);


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h_41_CALLBACK_WRAPPERS
struct Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h_41_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUGA_Weapon_MeleeLight_ClientPrediction(); \
	friend struct ::Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_NoRegister(); \
public: \
	DECLARE_CLASS2(UGA_Weapon_MeleeLight_ClientPrediction, USpellRiseGameplayAbility, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_UGA_Weapon_MeleeLight_ClientPrediction_NoRegister) \
	DECLARE_SERIALIZER(UGA_Weapon_MeleeLight_ClientPrediction)


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h_41_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UGA_Weapon_MeleeLight_ClientPrediction(UGA_Weapon_MeleeLight_ClientPrediction&&) = delete; \
	UGA_Weapon_MeleeLight_ClientPrediction(const UGA_Weapon_MeleeLight_ClientPrediction&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UGA_Weapon_MeleeLight_ClientPrediction); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UGA_Weapon_MeleeLight_ClientPrediction); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UGA_Weapon_MeleeLight_ClientPrediction) \
	NO_API virtual ~UGA_Weapon_MeleeLight_ClientPrediction();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h_38_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h_41_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h_41_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h_41_CALLBACK_WRAPPERS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h_41_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h_41_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UGA_Weapon_MeleeLight_ClientPrediction;

// ********** End Class UGA_Weapon_MeleeLight_ClientPrediction *************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Abilities_GA_Weapon_MeleeLight_ClientPrediction_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
