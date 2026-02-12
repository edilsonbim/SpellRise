// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Core/SpellRisePlayerState.h"

#ifdef SPELLRISE_SpellRisePlayerState_generated_h
#error "SpellRisePlayerState.generated.h already included, missing '#pragma once' in SpellRisePlayerState.h"
#endif
#define SPELLRISE_SpellRisePlayerState_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class ASpellRisePlayerState ****************************************************
struct Z_Construct_UClass_ASpellRisePlayerState_Statics;
SPELLRISE_API UClass* Z_Construct_UClass_ASpellRisePlayerState_NoRegister();

#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h_19_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesASpellRisePlayerState(); \
	friend struct ::Z_Construct_UClass_ASpellRisePlayerState_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend SPELLRISE_API UClass* ::Z_Construct_UClass_ASpellRisePlayerState_NoRegister(); \
public: \
	DECLARE_CLASS2(ASpellRisePlayerState, APlayerState, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/SpellRise"), Z_Construct_UClass_ASpellRisePlayerState_NoRegister) \
	DECLARE_SERIALIZER(ASpellRisePlayerState) \
	virtual UObject* _getUObject() const override { return const_cast<ASpellRisePlayerState*>(this); }


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h_19_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	ASpellRisePlayerState(ASpellRisePlayerState&&) = delete; \
	ASpellRisePlayerState(const ASpellRisePlayerState&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASpellRisePlayerState); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASpellRisePlayerState); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ASpellRisePlayerState) \
	NO_API virtual ~ASpellRisePlayerState();


#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h_16_PROLOG
#define FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h_19_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h_19_INCLASS_NO_PURE_DECLS \
	FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h_19_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class ASpellRisePlayerState;

// ********** End Class ASpellRisePlayerState ******************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Core_SpellRisePlayerState_h

PRAGMA_ENABLE_DEPRECATION_WARNINGS
