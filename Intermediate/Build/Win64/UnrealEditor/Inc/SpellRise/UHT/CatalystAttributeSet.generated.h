// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GameplayAbilitySystem/AttributeSets/CatalystAttributeSet.h"

#ifdef SPELLRISE_CatalystAttributeSet_generated_h
#error "CatalystAttributeSet.generated.h already included, missing '#pragma once' in CatalystAttributeSet.h"
#endif
#define SPELLRISE_CatalystAttributeSet_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Net/Core/PushModel/PushModelMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
struct FGameplayAttributeData;

// ********** Begin Class UCatalystAttributeSet ****************************************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h_18_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execOnRep_CatalystLevel); \
	DECLARE_FUNCTION(execOnRep_CatalystXP); \
	DECLARE_FUNCTION(execOnRep_CatalystCharge);


struct Z_Construct_UClass_UCatalystAttributeSet_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_UCatalystAttributeSet_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h_18_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUCatalystAttributeSet(); \
	friend struct ::Z_Construct_UClass_UCatalystAttributeSet_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_UCatalystAttributeSet_NoRegister(); \
public: \
	DECLARE_CLASS2(UCatalystAttributeSet, UAttributeSet, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_UCatalystAttributeSet_NoRegister) \
	DECLARE_SERIALIZER(UCatalystAttributeSet) \
	enum class ENetFields_Private : uint16 \
	{ \
		NETFIELD_REP_START=(uint16)((int32)Super::ENetFields_Private::NETFIELD_REP_END + (int32)1), \
		CatalystCharge=NETFIELD_REP_START, \
		CatalystXP, \
		CatalystLevel, \
		NETFIELD_REP_END=CatalystLevel	}; \
	DECLARE_VALIDATE_GENERATED_REP_ENUMS(NO_API) \
private: \
	REPLICATED_BASE_CLASS(UCatalystAttributeSet) \
public:


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h_18_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UCatalystAttributeSet(UCatalystAttributeSet&&) = delete; \
	UCatalystAttributeSet(const UCatalystAttributeSet&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UCatalystAttributeSet); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UCatalystAttributeSet); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UCatalystAttributeSet) \
	NO_API virtual ~UCatalystAttributeSet();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h_15_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h_18_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h_18_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h_18_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h_18_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UCatalystAttributeSet;

// ********** End Class UCatalystAttributeSet ******************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_AttributeSets_CatalystAttributeSet_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
