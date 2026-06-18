// Copyright Sam Bonifacio. All Rights Reserved.

#include "AutoSettingsEditorSubsystem.h"

#include "Framework/SettingRegistry.h"
#include "Utility/SettingRegistryKeyUtils.h"
#include "Widgets/AutoSettingWidget.h"
#include "Utility/SettingUtils.h"

void UAutoSettingsEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UAutoSettingWidget::GetDesignTimeRegistry.BindWeakLambda(this, [this](ESettingDomain Domain)
		{
		USettingRegistry*& Registry = Domain == ESettingDomain::Global ? DesignTimeGlobalRegistry : DesignTimePlayerRegistry;

		if (!Registry)
		{
			// Initialize the design time registry if required
			Registry = NewObject<USettingRegistry>();
			Registry->bDesignTime = true;
			Registry->BaseKey = Domain == ESettingDomain::Player
				? SB::AutoSettings::SettingRegistries::GetBaseKey(Domain, TEXT("0"))
				: SB::AutoSettings::SettingRegistries::GetBaseKey(Domain);
			
			// Load setting assets for the appropriate domain so default values can be shown
			const TArray<USettingType*> Settings = FSettingUtils::GetAllSettingTypes().FilterByPredicate(
				[Domain](const USettingType* Setting)
				{
					return Setting->SettingDomain == Domain;
				});
			
			Registry->Initialize(Settings);
		}

		return Registry; });
}
