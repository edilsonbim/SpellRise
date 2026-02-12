// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "PrimaryAttributeRow.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodePrimaryAttributeRow() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UScriptStruct* Z_Construct_UScriptStruct_FTableRowBase();
SPELLRISE_API UScriptStruct* Z_Construct_UScriptStruct_FPrimaryAttributeRow();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin ScriptStruct FPrimaryAttributeRow **********************************************
struct Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics
{
	static inline consteval int32 GetStructSize() { return sizeof(FPrimaryAttributeRow); }
	static inline consteval int16 GetStructAlignment() { return alignof(FPrimaryAttributeRow); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/PrimaryAttributeRow.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RowName_MetaData[] = {
		{ "Category", "PrimaryAttributeRow" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// (Opcional) Voc\xc3\xaa n\xc3\xa3o precisa disso na DataTable porque o RowName j\xc3\xa1 existe no DataTable.\n// Se quiser manter, ok.\n" },
#endif
		{ "ModuleRelativePath", "Public/PrimaryAttributeRow.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "(Opcional) Voc\xc3\xaa n\xc3\xa3o precisa disso na DataTable porque o RowName j\xc3\xa1 existe no DataTable.\nSe quiser manter, ok." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Abbr_MetaData[] = {
		{ "Category", "PrimaryAttributeRow" },
		{ "ModuleRelativePath", "Public/PrimaryAttributeRow.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Description_MetaData[] = {
		{ "Category", "PrimaryAttributeRow" },
		{ "ModuleRelativePath", "Public/PrimaryAttributeRow.h" },
		{ "MultiLine", "TRUE" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Min_MetaData[] = {
		{ "Category", "PrimaryAttributeRow" },
		{ "ModuleRelativePath", "Public/PrimaryAttributeRow.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Max_MetaData[] = {
		{ "Category", "PrimaryAttributeRow" },
		{ "ModuleRelativePath", "Public/PrimaryAttributeRow.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Base_MetaData[] = {
		{ "Category", "PrimaryAttributeRow" },
		{ "ModuleRelativePath", "Public/PrimaryAttributeRow.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FPrimaryAttributeRow constinit property declarations **************
	static const UECodeGen_Private::FNamePropertyParams NewProp_RowName;
	static const UECodeGen_Private::FStrPropertyParams NewProp_Abbr;
	static const UECodeGen_Private::FStrPropertyParams NewProp_Description;
	static const UECodeGen_Private::FIntPropertyParams NewProp_Min;
	static const UECodeGen_Private::FIntPropertyParams NewProp_Max;
	static const UECodeGen_Private::FIntPropertyParams NewProp_Base;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FPrimaryAttributeRow constinit property declarations ****************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FPrimaryAttributeRow>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics
static_assert(std::is_polymorphic<FPrimaryAttributeRow>() == std::is_polymorphic<FTableRowBase>(), "USTRUCT FPrimaryAttributeRow cannot be polymorphic unless super FTableRowBase is polymorphic");
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FPrimaryAttributeRow;
class UScriptStruct* FPrimaryAttributeRow::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FPrimaryAttributeRow.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FPrimaryAttributeRow.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FPrimaryAttributeRow, (UObject*)Z_Construct_UPackage__Script_SpellRise(), TEXT("PrimaryAttributeRow"));
	}
	return Z_Registration_Info_UScriptStruct_FPrimaryAttributeRow.OuterSingleton;
	}

// ********** Begin ScriptStruct FPrimaryAttributeRow Property Definitions *************************
const UECodeGen_Private::FNamePropertyParams Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_RowName = { "RowName", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPrimaryAttributeRow, RowName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RowName_MetaData), NewProp_RowName_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_Abbr = { "Abbr", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPrimaryAttributeRow, Abbr), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Abbr_MetaData), NewProp_Abbr_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_Description = { "Description", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPrimaryAttributeRow, Description), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Description_MetaData), NewProp_Description_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_Min = { "Min", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPrimaryAttributeRow, Min), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Min_MetaData), NewProp_Min_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_Max = { "Max", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPrimaryAttributeRow, Max), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Max_MetaData), NewProp_Max_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_Base = { "Base", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPrimaryAttributeRow, Base), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Base_MetaData), NewProp_Base_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_RowName,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_Abbr,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_Description,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_Min,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_Max,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewProp_Base,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::PropPointers) < 2048);
// ********** End ScriptStruct FPrimaryAttributeRow Property Definitions ***************************
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
	Z_Construct_UScriptStruct_FTableRowBase,
	&NewStructOps,
	"PrimaryAttributeRow",
	Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::PropPointers),
	sizeof(FPrimaryAttributeRow),
	alignof(FPrimaryAttributeRow),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FPrimaryAttributeRow()
{
	if (!Z_Registration_Info_UScriptStruct_FPrimaryAttributeRow.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FPrimaryAttributeRow.InnerSingleton, Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FPrimaryAttributeRow.InnerSingleton);
}
// ********** End ScriptStruct FPrimaryAttributeRow ************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Public_PrimaryAttributeRow_h__Script_SpellRise_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FPrimaryAttributeRow::StaticStruct, Z_Construct_UScriptStruct_FPrimaryAttributeRow_Statics::NewStructOps, TEXT("PrimaryAttributeRow"),&Z_Registration_Info_UScriptStruct_FPrimaryAttributeRow, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FPrimaryAttributeRow), 1943517542U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Public_PrimaryAttributeRow_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Public_PrimaryAttributeRow_h__Script_SpellRise_499251243{
	TEXT("/Script/SpellRise"),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Public_PrimaryAttributeRow_h__Script_SpellRise_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Public_PrimaryAttributeRow_h__Script_SpellRise_Statics::ScriptStructInfo),
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
