@echo off
setlocal
if "%~1"=="" (
  echo Uso: %~nx0 prod ^| test
  exit /b 1
)
powershell -ExecutionPolicy Bypass -File "%~dp0Set-SteamAuthProfile.ps1" -Profile %1
endlocal
