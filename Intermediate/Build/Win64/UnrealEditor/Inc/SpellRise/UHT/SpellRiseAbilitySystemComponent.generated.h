// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GameplayAbilitySystem/SpellRiseAbilitySystemComponent.h"

#ifdef SPELLRISE_SpellRiseAbilitySystemComponent_generated_h
#error "SpellRiseAbilitySystemComponent.generated.h already included, missing '#pragma once' in SpellRiseAbilitySystemComponent.h"
#endif
#define SPELLRISE_SpellRiseAbilitySystemComponent_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AActor;

// ********** Begin Class USpellRiseAbilitySystemComponent *****************************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execTryProcCatalystIfReady); \
	DECLARE_FUNCTION(execTryAddCatalystCharge_OnDamageTaken); \
	DECLARE_FUNCTION(execTryAddCatalystCharge_OnValidHit); \
	DECLARE_FUNCTION(execSR_ClearAbilityInput); \
	DECLARE_FUNCTION(execSR_ProcessAbilityInput); \
	DECLARE_FUNCTION(execSR_AbilityInputReleased); \
	DECLARE_FUNCTION(execSR_AbilityInputPressed);


struct Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseAbilitySystemComponent_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h_13_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUSpellRiseAbilitySystemComponent(); \
	friend struct ::Z_Construct_UClass_USpellRiseAbilitySystemComponent_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_USpellRiseAbilitySystemComponent_NoRegister(); \
public: \
	DECLARE_CLASS2(USpellRiseAbilitySystemComponent, UAbilitySystemComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_USpellRiseAbilitySystemComponent_NoRegister) \
	DECLARE_SERIALIZER(USpellRiseAbilitySystemComponent)


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h_13_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	USpellRiseAbilitySystemComponent(USpellRiseAbilitySystemComponent&&) = delete; \
	USpellRiseAbilitySystemComponent(const USpellRiseAbilitySystemComponent&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, USpellRiseAbilitySystemComponent); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(USpellRiseAbilitySystemComponent); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(USpellRiseAbilitySystemComponent) \
	NO_API virtual ~USpellRiseAbilitySystemComponent();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h_10_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h_13_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h_13_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h_13_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class USpellRiseAbilitySystemComponent;

// ********** End Class USpellRiseAbilitySystemComponent *******************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_SpellRiseAbilitySystemComponent_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
