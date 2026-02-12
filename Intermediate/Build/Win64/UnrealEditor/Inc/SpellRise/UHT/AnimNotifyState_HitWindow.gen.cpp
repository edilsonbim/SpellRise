// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SpellRise/Combat/Melee/AnimNotifies/AnimNotifyState_HitWindow.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeAnimNotifyState_HitWindow() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UAnimNotifyState();
ENGINE_API UClass* Z_Construct_UClass_USceneComponent_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_USkeletalMeshComponent_NoRegister();
ENGINE_API UEnum* Z_Construct_UEnum_Engine_ETraceTypeQuery();
SPELLRISE_API UClass* Z_Construct_UClass_UAnimNotifyState_HitWindow();
SPELLRISE_API UClass* Z_Construct_UClass_UAnimNotifyState_HitWindow_NoRegister();
UPackage* Z_Construct_UPackage__Script_SpellRise();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UAnimNotifyState_HitWindow Function ResolveWeaponComponent ***************
struct Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics
{
	struct AnimNotifyState_HitWindow_eventResolveWeaponComponent_Parms
	{
		AActor* Owner;
		USkeletalMeshComponent* MeshComp;
		USceneComponent* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Trace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n\x09 * Resolve qual SceneComponent usar para o trace.\n\x09 * Ordem: 1) WeaponComponent setado manualmente, 2) WeaponActor->Mesh, 3) MeshComp (fallback)\n\x09 */" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_HitWindow.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Resolve qual SceneComponent usar para o trace.\nOrdem: 1) WeaponComponent setado manualmente, 2) WeaponActor->Mesh, 3) MeshComp (fallback)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MeshComp_MetaData[] = {
		{ "EditInline", "true" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif // WITH_METADATA

// ********** Begin Function ResolveWeaponComponent constinit property declarations ****************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Owner;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_MeshComp;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function ResolveWeaponComponent constinit property declarations ******************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function ResolveWeaponComponent Property Definitions ***************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::NewProp_Owner = { "Owner", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AnimNotifyState_HitWindow_eventResolveWeaponComponent_Parms, Owner), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::NewProp_MeshComp = { "MeshComp", nullptr, (EPropertyFlags)0x0010000000080080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AnimNotifyState_HitWindow_eventResolveWeaponComponent_Parms, MeshComp), Z_Construct_UClass_USkeletalMeshComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MeshComp_MetaData), NewProp_MeshComp_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000080588, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AnimNotifyState_HitWindow_eventResolveWeaponComponent_Parms, ReturnValue), Z_Construct_UClass_USceneComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ReturnValue_MetaData), NewProp_ReturnValue_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::NewProp_Owner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::NewProp_MeshComp,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::PropPointers) < 2048);
// ********** End Function ResolveWeaponComponent Property Definitions *****************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UAnimNotifyState_HitWindow, nullptr, "ResolveWeaponComponent", 	Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::AnimNotifyState_HitWindow_eventResolveWeaponComponent_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54080401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::Function_MetaDataParams), Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::AnimNotifyState_HitWindow_eventResolveWeaponComponent_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UAnimNotifyState_HitWindow::execResolveWeaponComponent)
{
	P_GET_OBJECT(AActor,Z_Param_Owner);
	P_GET_OBJECT(USkeletalMeshComponent,Z_Param_MeshComp);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(USceneComponent**)Z_Param__Result=P_THIS->ResolveWeaponComponent(Z_Param_Owner,Z_Param_MeshComp);
	P_NATIVE_END;
}
// ********** End Class UAnimNotifyState_HitWindow Function ResolveWeaponComponent *****************

// ********** Begin Class UAnimNotifyState_HitWindow ***********************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UAnimNotifyState_HitWindow;
UClass* UAnimNotifyState_HitWindow::GetPrivateStaticClass()
{
	using TClass = UAnimNotifyState_HitWindow;
	if (!Z_Registration_Info_UClass_UAnimNotifyState_HitWindow.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("AnimNotifyState_HitWindow"),
			Z_Registration_Info_UClass_UAnimNotifyState_HitWindow.InnerSingleton,
			StaticRegisterNativesUAnimNotifyState_HitWindow,
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
	return Z_Registration_Info_UClass_UAnimNotifyState_HitWindow.InnerSingleton;
}
UClass* Z_Construct_UClass_UAnimNotifyState_HitWindow_NoRegister()
{
	return UAnimNotifyState_HitWindow::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "DisplayName", "Hit Window" },
		{ "HideCategories", "Object" },
		{ "IncludePath", "Combat/Melee/AnimNotifies/AnimNotifyState_HitWindow.h" },
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_HitWindow.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WeaponComponent_MetaData[] = {
		{ "Category", "Trace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Componente da arma (opcional - se n\xc3\xa3o setado, tenta resolver automaticamente) */" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_HitWindow.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Componente da arma (opcional - se n\xc3\xa3o setado, tenta resolver automaticamente)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SocketBase_MetaData[] = {
		{ "Category", "Trace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Socket base da arma (in\xc3\xad""cio do trace) */" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_HitWindow.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Socket base da arma (in\xc3\xad""cio do trace)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SocketTip_MetaData[] = {
		{ "Category", "Trace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Socket tip da arma (fim do trace) */" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_HitWindow.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Socket tip da arma (fim do trace)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Radius_MetaData[] = {
		{ "Category", "Trace" },
		{ "ClampMin", "0.1" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Raio do trace */" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_HitWindow.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Raio do trace" },
#endif
		{ "UIMin", "0.1" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TraceChannel_MetaData[] = {
		{ "Category", "Trace" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Canal de colis\xc3\xa3o para o trace */" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_HitWindow.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Canal de colis\xc3\xa3o para o trace" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bDrawDebug_MetaData[] = {
		{ "Category", "Debug" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Desenhar debug? */" },
#endif
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_HitWindow.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Desenhar debug?" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DebugLifeTime_MetaData[] = {
		{ "Category", "Debug" },
		{ "ClampMin", "0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Tempo de vida do debug (segundos) */" },
#endif
		{ "EditCondition", "bDrawDebug" },
		{ "ModuleRelativePath", "Combat/Melee/AnimNotifies/AnimNotifyState_HitWindow.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Tempo de vida do debug (segundos)" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UAnimNotifyState_HitWindow constinit property declarations ***************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_WeaponComponent;
	static const UECodeGen_Private::FNamePropertyParams NewProp_SocketBase;
	static const UECodeGen_Private::FNamePropertyParams NewProp_SocketTip;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_Radius;
	static const UECodeGen_Private::FBytePropertyParams NewProp_TraceChannel;
	static void NewProp_bDrawDebug_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bDrawDebug;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_DebugLifeTime;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UAnimNotifyState_HitWindow constinit property declarations *****************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("ResolveWeaponComponent"), .Pointer = &UAnimNotifyState_HitWindow::execResolveWeaponComponent },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UAnimNotifyState_HitWindow_ResolveWeaponComponent, "ResolveWeaponComponent" }, // 2820151549
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAnimNotifyState_HitWindow>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics

// ********** Begin Class UAnimNotifyState_HitWindow Property Definitions **************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_WeaponComponent = { "WeaponComponent", nullptr, (EPropertyFlags)0x011400000008001d, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAnimNotifyState_HitWindow, WeaponComponent), Z_Construct_UClass_USceneComponent_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WeaponComponent_MetaData), NewProp_WeaponComponent_MetaData) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_SocketBase = { "SocketBase", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAnimNotifyState_HitWindow, SocketBase), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SocketBase_MetaData), NewProp_SocketBase_MetaData) };
const UECodeGen_Private::FNamePropertyParams Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_SocketTip = { "SocketTip", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAnimNotifyState_HitWindow, SocketTip), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SocketTip_MetaData), NewProp_SocketTip_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_Radius = { "Radius", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAnimNotifyState_HitWindow, Radius), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Radius_MetaData), NewProp_Radius_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_TraceChannel = { "TraceChannel", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAnimNotifyState_HitWindow, TraceChannel), Z_Construct_UEnum_Engine_ETraceTypeQuery, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TraceChannel_MetaData), NewProp_TraceChannel_MetaData) }; // 93441085
void Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_bDrawDebug_SetBit(void* Obj)
{
	((UAnimNotifyState_HitWindow*)Obj)->bDrawDebug = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_bDrawDebug = { "bDrawDebug", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UAnimNotifyState_HitWindow), &Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_bDrawDebug_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bDrawDebug_MetaData), NewProp_bDrawDebug_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_DebugLifeTime = { "DebugLifeTime", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAnimNotifyState_HitWindow, DebugLifeTime), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DebugLifeTime_MetaData), NewProp_DebugLifeTime_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_WeaponComponent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_SocketBase,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_SocketTip,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_Radius,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_TraceChannel,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_bDrawDebug,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::NewProp_DebugLifeTime,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::PropPointers) < 2048);
// ********** End Class UAnimNotifyState_HitWindow Property Definitions ****************************
UObject* (*const Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UAnimNotifyState,
	(UObject* (*)())Z_Construct_UPackage__Script_SpellRise,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::ClassParams = {
	&UAnimNotifyState_HitWindow::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::PropPointers),
	0,
	0x009130A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::Class_MetaDataParams), Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::Class_MetaDataParams)
};
void UAnimNotifyState_HitWindow::StaticRegisterNativesUAnimNotifyState_HitWindow()
{
	UClass* Class = UAnimNotifyState_HitWindow::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::Funcs));
}
UClass* Z_Construct_UClass_UAnimNotifyState_HitWindow()
{
	if (!Z_Registration_Info_UClass_UAnimNotifyState_HitWindow.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAnimNotifyState_HitWindow.OuterSingleton, Z_Construct_UClass_UAnimNotifyState_HitWindow_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAnimNotifyState_HitWindow.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UAnimNotifyState_HitWindow);
UAnimNotifyState_HitWindow::~UAnimNotifyState_HitWindow() {}
// ********** End Class UAnimNotifyState_HitWindow *************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_AnimNotifies_AnimNotifyState_HitWindow_h__Script_SpellRise_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAnimNotifyState_HitWindow, UAnimNotifyState_HitWindow::StaticClass, TEXT("UAnimNotifyState_HitWindow"), &Z_Registration_Info_UClass_UAnimNotifyState_HitWindow, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAnimNotifyState_HitWindow), 3044604844U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_AnimNotifies_AnimNotifyState_HitWindow_h__Script_SpellRise_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_AnimNotifies_AnimNotifyState_HitWindow_h__Script_SpellRise_208120555{
	TEXT("/Script/SpellRise"),
	Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_AnimNotifies_AnimNotifyState_HitWindow_h__Script_SpellRise_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_biM_Documents_Unreal_Projects_SpellRise_Source_SpellRise_Combat_Melee_AnimNotifies_AnimNotifyState_HitWindow_h__Script_SpellRise_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
