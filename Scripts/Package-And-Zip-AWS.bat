@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
set "ENGINE_ROOT=C:\UnrealSource\UnrealEngine"
set "ARCHIVE_ROOT=%USERPROFILE%\Desktop\SpellRise_TestBuild"
set "UPLOAD_ROOT=%USERPROFILE%\Desktop\SpellRise_AWS_Upload"
set "CONFIG=Development"
set "MAP=/Game/Maps/Stylized/Stylized"

if not exist "%UPLOAD_ROOT%" mkdir "%UPLOAD_ROOT%"

echo [STEP] Packaging client + server (Unreal Source)...
powershell -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_DIR%Package-ClientServer-Source.ps1" -EngineRoot "%ENGINE_ROOT%" -DesktopOutputRoot "%ARCHIVE_ROOT%" -Config "%CONFIG%" -Map "%MAP%" -CleanOutput
if errorlevel 1 (
  echo [ERROR] Package falhou.
  exit /b 1
)

set "CLIENT_SRC=%ARCHIVE_ROOT%\Client\WindowsClient"
set "SERVER_SRC=%ARCHIVE_ROOT%\Server\WindowsServer"
set "CLIENT_ZIP=%UPLOAD_ROOT%\SpellRise_Client_WindowsClient_%CONFIG%.zip"
set "SERVER_ZIP=%UPLOAD_ROOT%\SpellRise_Server_WindowsServer_%CONFIG%.zip"

if exist "%CLIENT_ZIP%" del /f /q "%CLIENT_ZIP%"
if exist "%SERVER_ZIP%" del /f /q "%SERVER_ZIP%"

echo [STEP] Compactando client...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%CLIENT_SRC%\*' -DestinationPath '%CLIENT_ZIP%' -CompressionLevel Optimal"
if errorlevel 1 (
  echo [ERROR] Zip do client falhou.
  exit /b 1
)

echo [STEP] Compactando server...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%SERVER_SRC%\*' -DestinationPath '%SERVER_ZIP%' -CompressionLevel Optimal"
if errorlevel 1 (
  echo [ERROR] Zip do server falhou.
  exit /b 1
)

echo.
echo [OK] Artefatos prontos para upload RDP:
echo  - %CLIENT_ZIP%
echo  - %SERVER_ZIP%
echo.
echo [TIP] Copie primeiro o ZIP do server para C:\SpellRise\Server na instancia AWS.
exit /b 0
