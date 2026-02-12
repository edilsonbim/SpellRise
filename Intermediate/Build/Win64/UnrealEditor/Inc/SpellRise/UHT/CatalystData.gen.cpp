// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/GameplayAbilitySystem/Catalyst/CatalystData.h"
#include "GameplayTagContainer.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeCatalystData() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UClass_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UPrimaryDataAsset();
GAMEPLAYABILITIES_API UClass* Z_Construct_UClass_UGameplayEffect_NoRegister();
GAMEPLAYTAGS_API UScriptStruct* Z_Construct_UScriptStruct_FGameplayTag();
SPELLRISE_API UClass* Z_Construct_UClass_UCatalystData();
SPELLRISE_API UClass* Z_Construct_UClass_UCatalystData_NoRegister();
SPELLRISE_API UEnum* Z_Construct_UEnum_SpellRise_ECatalystType();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Enum ECatalystType *************************************************************
static FEnumRegistrationInfo Z_Registration_Info_UEnum_ECatalystType;
static UEnum* ECatalystType_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_ECatalystType.OuterSingleton)
	{
		Z_Registration_Info_UEnum_ECatalystType.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_SpellRise_ECatalystType, (UObject*)Z_Construct_UPackage__Script_SpellRise(), TEXT("ECatalystType"));
	}
	return Z_Registration_Info_UEnum_ECatalystType.OuterSingleton;
}
template<> SPELLRISE_NON_ATTRIBUTED_API UEnum* StaticEnum<ECatalystType>()
{
	return ECatalystType_StaticEnum();
}
struct Z_Construct_UEnum_SpellRise_ECatalystType_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
		{ "Archer.Name", "ECatalystType::Archer" },
		{ "BlueprintType", "true" },
		{ "Mage.Name", "ECatalystType::Mage" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
		{ "Physical.Name", "ECatalystType::Physical" },
		{ "Support.Name", "ECatalystType::Support" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "ECatalystType::Physical", (int64)ECatalystType::Physical },
		{ "ECatalystType::Archer", (int64)ECatalystType::Archer },
		{ "ECatalystType::Mage", (int64)ECatalystType::Mage },
		{ "ECatalystType::Support", (int64)ECatalystType::Support },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct Z_Construct_UEnum_SpellRise_ECatalystType_Statics 
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_SpellRise_ECatalystType_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_SpellRise,
	nullptr,
	"ECatalystType",
	"ECatalystType",
	Z_Construct_UEnum_SpellRise_ECatalystType_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_SpellRise_ECatalystType_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_SpellRise_ECatalystType_Statics::Enum_MetaDataParams), Z_Construct_UEnum_SpellRise_ECatalystType_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_SpellRise_ECatalystType()
{
	if (!Z_Registration_Info_UEnum_ECatalystType.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_ECatalystType.InnerSingleton, Z_Construct_UEnum_SpellRise_ECatalystType_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_ECatalystType.InnerSingleton;
}
// ********** End Enum ECatalystType ***************************************************************

// ********** Begin Class UCatalystData ************************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UCatalystData;
UClass* UCatalystData::GetPrivateStaticClass()
{
	using TClass = UCatalystData;
	if (!Z_Registration_Info_UClass_UCatalystData.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("CatalystData"),
			Z_Registration_Info_UClass_UCatalystData.InnerSingleton,
			StaticRegisterNativesUCatalystData,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UCatalystData.InnerSingleton;
}
UClass* Z_Construct_UClass_UCatalystData_NoRegister()
{
	return UCatalystData::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UCatalystData_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CatalystId_MetaData[] = {
		{ "Category", "CatalystData" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Type_MetaData[] = {
		{ "Category", "CatalystData" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_Active_L1_MetaData[] = {
		{ "Category", "CatalystData" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_Active_L2_MetaData[] = {
		{ "Category", "CatalystData" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_Active_L3_MetaData[] = {
		{ "Category", "CatalystData" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_PassiveBonus_L1_MetaData[] = {
		{ "Category", "CatalystData" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_PassiveBonus_L2_MetaData[] = {
		{ "Category", "CatalystData" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GE_PassiveBonus_L3_MetaData[] = {
		{ "Category", "CatalystData" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CatalystTypeTag_MetaData[] = {
		{ "Category", "CatalystData" },
		{ "ModuleRelativePath", "GameplayAbilitySystem/Catalyst/CatalystData.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UCatalystData constinit property declarations ****************************
	static const UECodeGen_Private::FNamePropertyParams NewProp_CatalystId;
	static const UECodeGen_Private::FBytePropertyParams NewProp_Type_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_Type;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_Active_L1;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_Active_L2;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_Active_L3;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_PassiveBonus_L1;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_PassiveBonus_L2;
	static const UECodeGen_Private::FClassPropertyParams NewProp_GE_PassiveBonus_L3;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CatalystTypeTag;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UCatalystData constinit property declarations ******************************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCatalystData>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UCatalystData_Statics

// ********** Begin Class UCatalystData Property Definitions ***************************************
const UECodeGen_Private::FNamePropertyParams Z_Construct_UClass_UCatalystData_Statics::NewProp_CatalystId = { "CatalystId", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystData, CatalystId), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CatalystId_MetaData), NewProp_CatalystId_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UCatalystData_Statics::NewProp_Type_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UCatalystData_Statics::NewProp_Type = { "Type", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystData, Type), Z_Construct_UEnum_SpellRise_ECatalystType, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Type_MetaData), NewProp_Type_MetaData) }; // 3094575136
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_Active_L1 = { "GE_Active_L1", nullptr, (EPropertyFlags)0x0014000000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystData, GE_Active_L1), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_Active_L1_MetaData), NewProp_GE_Active_L1_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_Active_L2 = { "GE_Active_L2", nullptr, (EPropertyFlags)0x0014000000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystData, GE_Active_L2), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_Active_L2_MetaData), NewProp_GE_Active_L2_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_Active_L3 = { "GE_Active_L3", nullptr, (EPropertyFlags)0x0014000000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystData, GE_Active_L3), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_Active_L3_MetaData), NewProp_GE_Active_L3_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_PassiveBonus_L1 = { "GE_PassiveBonus_L1", nullptr, (EPropertyFlags)0x0014000000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystData, GE_PassiveBonus_L1), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_PassiveBonus_L1_MetaData), NewProp_GE_PassiveBonus_L1_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_PassiveBonus_L2 = { "GE_PassiveBonus_L2", nullptr, (EPropertyFlags)0x0014000000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystData, GE_PassiveBonus_L2), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_PassiveBonus_L2_MetaData), NewProp_GE_PassiveBonus_L2_MetaData) };
const UECodeGen_Private::FClassPropertyParams Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_PassiveBonus_L3 = { "GE_PassiveBonus_L3", nullptr, (EPropertyFlags)0x0014000000010015, UECodeGen_Private::EPropertyGenFlags::Class, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystData, GE_PassiveBonus_L3), Z_Construct_UClass_UClass_NoRegister, Z_Construct_UClass_UGameplayEffect_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GE_PassiveBonus_L3_MetaData), NewProp_GE_PassiveBonus_L3_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UCatalystData_Statics::NewProp_CatalystTypeTag = { "CatalystTypeTag", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UCatalystData, CatalystTypeTag), Z_Construct_UScriptStruct_FGameplayTag, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CatalystTypeTag_MetaData), NewProp_CatalystTypeTag_MetaData) }; // 517357616
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UCatalystData_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystData_Statics::NewProp_CatalystId,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystData_Statics::NewProp_Type_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystData_Statics::NewProp_Type,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_Active_L1,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_Active_L2,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_Active_L3,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_PassiveBonus_L1,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_PassiveBonus_L2,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystData_Statics::NewProp_GE_PassiveBonus_L3,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UCatalystData_Statics::NewProp_CatalystTypeTag,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCatalystData_Statics::PropPointers) < 2048);
// ********** End Class UCatalystData Property Definitions *****************************************
UObject* (*const Z_Construct_UClass_UCatalystData_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UPrimaryDataAsset,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCatalystData_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UCatalystData_Statics::ClassParams = {
	&UCatalystData::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UCatalystData_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UCatalystData_Statics::PropPointers),
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCatalystData_Statics::Class_MetaDataParams), Z_Construct_UClass_UCatalystData_Statics::Class_MetaDataParams)
};
void UCatalystData::StaticRegisterNativesUCatalystData()
{
}
UClass* Z_Construct_UClass_UCatalystData()
{
	if (!Z_Registration_Info_UClass_UCatalystData.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCatalystData.OuterSingleton, Z_Construct_UClass_UCatalystData_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UCatalystData.OuterSingleton;
}
UCatalystData::UCatalystData(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UCatalystData);
UCatalystData::~UCatalystData() {}
// ********** End Class UCatalystData **************************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h__Script_SpellRise_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ ECatalystType_StaticEnum, TEXT("ECatalystType"), &Z_Registration_Info_UEnum_ECatalystType, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 3094575136U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UCatalystData, UCatalystData::StaticClass, TEXT("UCatalystData"), &Z_Registration_Info_UClass_UCatalystData, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCatalystData), 2518929173U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h__Script_SpellRise_473766277{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h__Script_SpellRise_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_GameplayAbilitySystem_Catalyst_CatalystData_h__Script_SpellRise_Statics::EnumInfo),
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
