// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Characters/SpellRiseCharacterBase.h"

#ifdef SPELLRISE_SpellRiseCharacterBase_generated_h
#error "SpellRiseCharacterBase.generated.h already included, missing '#pragma once' in SpellRiseCharacterBase.h"
#endif
#define SPELLRISE_SpellRiseCharacterBase_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AActor;
class UCatalystAttributeSet;
class UClass;
class UGameplayAbility;
class USpellRiseWeaponComponent;
struct FGameplayAbilitySpecHandle;
struct FGameplayEventData;
struct FGameplayTag;

// ********** Begin Class ASpellRiseCharacterBase **************************************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h_34_RPC_WRAPPERS_NO_PURE_DECLS \
	virtual void MultiShowDamagePop_Implementation(float Damage, AActor* InstigatorActor, FGameplayTag DamageTypeTag, bool bIsCrit); \
	virtual void MultiOnCatalystProc_Implementation(int32 CatalystTier); \
	virtual void MultiHandleDeath_Implementation(); \
	virtual void HandleDeath_Implementation(); \
	virtual void MultiSendGameplayEventToActor_Implementation(AActor* TargetActor, FGameplayEventData const& EventData); \
	virtual void ServerSendGameplayEventToSelf_Implementation(FGameplayEventData const& EventData); \
	DECLARE_FUNCTION(execMultiShowDamagePop); \
	DECLARE_FUNCTION(execMultiOnCatalystProc); \
	DECLARE_FUNCTION(execGetCatalystAttributeSet); \
	DECLARE_FUNCTION(execMultiHandleDeath); \
	DECLARE_FUNCTION(execHandleDeath); \
	DECLARE_FUNCTION(execOnDeadTagChanged); \
	DECLARE_FUNCTION(execIsDead); \
	DECLARE_FUNCTION(execMultiSendGameplayEventToActor); \
	DECLARE_FUNCTION(execServerSendGameplayEventToSelf); \
	DECLARE_FUNCTION(execSendAbilitiesChangedEvent); \
	DECLARE_FUNCTION(execRemoveAbilities); \
	DECLARE_FUNCTION(execGrantAbilities); \
	DECLARE_FUNCTION(execSR_ClearAbilityInput); \
	DECLARE_FUNCTION(execSR_ProcessAbilityInput); \
	DECLARE_FUNCTION(execGetWeaponComponent);


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h_34_CALLBACK_WRAPPERS
struct Z_Construct_UClass_ASpellRiseCharacterBase_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRiseCharacterBase_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h_34_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesASpellRiseCharacterBase(); \
	friend struct ::Z_Construct_UClass_ASpellRiseCharacterBase_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_ASpellRiseCharacterBase_NoRegister(); \
public: \
	DECLARE_CLASS2(ASpellRiseCharacterBase, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_ASpellRiseCharacterBase_NoRegister) \
	DECLARE_SERIALIZER(ASpellRiseCharacterBase) \
	virtual UObject* _getUObject() const override { return const_cast<ASpellRiseCharacterBase*>(this); }


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h_34_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	ASpellRiseCharacterBase(ASpellRiseCharacterBase&&) = delete; \
	ASpellRiseCharacterBase(const ASpellRiseCharacterBase&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASpellRiseCharacterBase); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASpellRiseCharacterBase); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ASpellRiseCharacterBase) \
	NO_API virtual ~ASpellRiseCharacterBase();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h_29_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h_34_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h_34_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h_34_CALLBACK_WRAPPERS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h_34_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h_34_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class ASpellRiseCharacterBase;

// ********** End Class ASpellRiseCharacterBase ****************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Characters_SpellRiseCharacterBase_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
