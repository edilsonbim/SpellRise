// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GameplayAbilitySystem/Catalyst/CatalystData.h"

#ifdef SPELLRISE_CatalystData_generated_h
#error "CatalystData.generated.h already included, missing '#pragma once' in CatalystData.h"
#endif
#define SPELLRISE_CatalystData_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UCatalystData ************************************************************
struct Z_Construct_UClass_UCatalystData_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_UCatalystData_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h_20_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUCatalystData(); \
	friend struct ::Z_Construct_UClass_UCatalystData_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_UCatalystData_NoRegister(); \
public: \
	DECLARE_CLASS2(UCatalystData, UPrimaryDataAsset, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_UCatalystData_NoRegister) \
	DECLARE_SERIALIZER(UCatalystData)


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h_20_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UCatalystData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UCatalystData(UCatalystData&&) = delete; \
	UCatalystData(const UCatalystData&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UCatalystData); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UCatalystData); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UCatalystData) \
	NO_API virtual ~UCatalystData();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h_17_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h_20_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h_20_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h_20_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UCatalystData;

// ********** End Class UCatalystData **************************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h

// ********** Begin Enum ECatalystType *************************************************************
#define FOREACH_ENUM_ECATALYSTTYPE(op) \
	op(ECatalystType::Physical) \
	op(ECatalystType::Archer) \
	op(ECatalystType::Mage) \
	op(ECatalystType::Support) 

enum class ECatalystType : uint8;
template<> struct TIsUEnumClass<ECatalystType> { enum { Value = true }; };
template<> SPELLRISE_NON_ATTRIBUTED_API UEnum* StaticEnum<ECatalystType>();
// ********** End Enum ECatalystType ***************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
