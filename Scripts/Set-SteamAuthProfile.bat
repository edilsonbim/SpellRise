@echo off
setlocal
set PROFILE=%~1
if "%PROFILE%"=="" (
  echo Uso: %~nx0 ^<prod^|test^>
  exit /b 1
)
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Set-SteamAuthProfile.ps1" -Profile "%PROFILE%"
exit /b %errorlevel%
