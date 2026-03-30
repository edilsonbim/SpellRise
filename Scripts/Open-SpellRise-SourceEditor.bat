@echo off
set "EDITOR=C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
set "PROJECT=C:\Users\biM\Documents\Unreal Projects\SpellRise\SpellRise.uproject"
set "STEAM_APPID_FILE=C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\steam_appid.txt"

if not exist "%EDITOR%" (
  echo UnrealEditor.exe nao encontrado em: %EDITOR%
  exit /b 1
)

> "%STEAM_APPID_FILE%" echo 480
echo steam_appid.txt configurado em: %STEAM_APPID_FILE%

start "SpellRise Editor (Source)" "%EDITOR%" "%PROJECT%"
