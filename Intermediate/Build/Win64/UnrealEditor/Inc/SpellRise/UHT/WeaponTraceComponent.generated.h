// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Combat/Melee/Trace/WeaponTraceComponent.h"

#ifdef SPELLRISE_WeaponTraceComponent_generated_h
#error "WeaponTraceComponent.generated.h already included, missing '#pragma once' in WeaponTraceComponent.h"
#endif
#define SPELLRISE_WeaponTraceComponent_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UAbilitySystemComponent;
class UClass;
class UGameplayEffect;
struct FWeaponTraceConfig;

// ********** Begin ScriptStruct FWeaponTraceConfig ************************************************
struct Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics;
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h_15_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FWeaponTraceConfig_Statics; \
	SPELLRISE_API static class UScriptStruct* StaticStruct();


struct FWeaponTraceConfig;
// ********** End ScriptStruct FWeaponTraceConfig **************************************************

// ********** Begin Class UWeaponTraceComponent ****************************************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h_48_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execResetHitActors); \
	DECLARE_FUNCTION(execSetDamagePayload); \
	DECLARE_FUNCTION(execEndTraceWindow); \
	DECLARE_FUNCTION(execBeginTraceWindow);


struct Z_Construct_UClass_UWeaponTraceComponent_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_UWeaponTraceComponent_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h_48_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUWeaponTraceComponent(); \
	friend struct ::Z_Construct_UClass_UWeaponTraceComponent_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_UWeaponTraceComponent_NoRegister(); \
public: \
	DECLARE_CLASS2(UWeaponTraceComponent, UActorComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_UWeaponTraceComponent_NoRegister) \
	DECLARE_SERIALIZER(UWeaponTraceComponent)


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h_48_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UWeaponTraceComponent(UWeaponTraceComponent&&) = delete; \
	UWeaponTraceComponent(const UWeaponTraceComponent&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UWeaponTraceComponent); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UWeaponTraceComponent); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UWeaponTraceComponent) \
	NO_API virtual ~UWeaponTraceComponent();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h_45_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h_48_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h_48_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h_48_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h_48_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UWeaponTraceComponent;

// ********** End Class UWeaponTraceComponent ******************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_Trace_WeaponTraceComponent_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
