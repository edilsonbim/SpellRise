@echo off
set "EDITOR=C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
set "PROJECT=%~dp0..\SpellRise.uproject"
set "MAP=/Game/Maps/Stylized/Stylized"

if not exist "%EDITOR%" (
  echo UnrealEditor.exe nao encontrado em: %EDITOR%
  exit /b 1
)

"%EDITOR%" "%PROJECT%" %MAP% -game -log
