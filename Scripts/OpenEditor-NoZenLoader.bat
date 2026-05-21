@echo off
setlocal

set "UE_EDITOR=C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
set "PROJECT=C:\Users\biM\Documents\Unreal Projects\SpellRise\SpellRise.uproject"

"%UE_EDITOR%" "%PROJECT%" -NoZenLoader -NoRestoreOpenAssetTabs -NoLoadStartupPackages
