// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Weapons/Components/SpellRiseWeaponComponent.h"

#ifdef SPELLRISE_SpellRiseWeaponComponent_generated_h
#error "SpellRiseWeaponComponent.generated.h already included, missing '#pragma once' in SpellRiseWeaponComponent.h"
#endif
#define SPELLRISE_SpellRiseWeaponComponent_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AActor;
class ASpellRiseWeaponActor;
class UClass;
class UDA_WeaponDefinition;
class UGameplayEffect;
struct FHitResult;

// ********** Begin Class USpellRiseWeaponComponent ************************************************
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h_33_RPC_WRAPPERS_NO_PURE_DECLS \
	virtual void ServerPerformLightAttack_TraceAndApplyDamage_Implementation(AActor* InstigatorActor, TSubclassOf<UGameplayEffect> DamageGE, float BaseDamage, float DamageScaling, ECollisionChannel TraceChannel, bool bDebug); \
	virtual void ServerUnequipWeapon_Implementation(); \
	virtual void ServerEquipWeapon_Implementation(UDA_WeaponDefinition* NewWeapon); \
	DECLARE_FUNCTION(execServerPerformLightAttack_TraceAndApplyDamage); \
	DECLARE_FUNCTION(execServerUnequipWeapon); \
	DECLARE_FUNCTION(execServerEquipWeapon); \
	DECLARE_FUNCTION(execOnRep_EquippedWeapon); \
	DECLARE_FUNCTION(execHandleOwnerDeath); \
	DECLARE_FUNCTION(execGetEquippedWeaponActor); \
	DECLARE_FUNCTION(execGetEquippedWeapon); \
	DECLARE_FUNCTION(execUnequipWeapon); \
	DECLARE_FUNCTION(execEquipWeapon); \
	DECLARE_FUNCTION(execPerformLightAttack_TraceAndApplyDamage); \
	DECLARE_FUNCTION(execPerformLightAttackTrace);


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h_33_CALLBACK_WRAPPERS
struct Z_Construct_UClass_USpellRiseWeaponComponent_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_USpellRiseWeaponComponent_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h_33_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUSpellRiseWeaponComponent(); \
	friend struct ::Z_Construct_UClass_USpellRiseWeaponComponent_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_USpellRiseWeaponComponent_NoRegister(); \
public: \
	DECLARE_CLASS2(USpellRiseWeaponComponent, UActorComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_USpellRiseWeaponComponent_NoRegister) \
	DECLARE_SERIALIZER(USpellRiseWeaponComponent) \
	enum class ENetFields_Private : uint16 \
	{ \
		NETFIELD_REP_START=(uint16)((int32)Super::ENetFields_Private::NETFIELD_REP_END + (int32)1), \
		EquippedWeapon=NETFIELD_REP_START, \
		NETFIELD_REP_END=EquippedWeapon	}; \
	DECLARE_VALIDATE_GENERATED_REP_ENUMS(NO_API)


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h_33_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	USpellRiseWeaponComponent(USpellRiseWeaponComponent&&) = delete; \
	USpellRiseWeaponComponent(const USpellRiseWeaponComponent&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, USpellRiseWeaponComponent); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(USpellRiseWeaponComponent); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(USpellRiseWeaponComponent) \
	NO_API virtual ~USpellRiseWeaponComponent();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h_30_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h_33_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h_33_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h_33_CALLBACK_WRAPPERS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h_33_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h_33_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class USpellRiseWeaponComponent;

// ********** End Class USpellRiseWeaponComponent **************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Weapons_Components_SpellRiseWeaponComponent_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
