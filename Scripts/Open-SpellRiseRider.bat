@echo off
set "RIDER=C:\Program Files\JetBrains\JetBrains Rider 2025.3.1\bin\rider64.exe"
set "SOLUTION=%~dp0..\SpellRise.sln"
set "PORT_FILE=%LOCALAPPDATA%\JetBrains\Rider2025.3\.port"

if /I "%~1"=="--fix-lock" (
  if exist "%PORT_FILE%" (
    del /f /q "%PORT_FILE%" >nul 2>&1
  )
)

if not exist "%RIDER%" (
  echo Rider nao encontrado em: %RIDER%
  exit /b 1
)

start "" "%RIDER%" "%SOLUTION%"

