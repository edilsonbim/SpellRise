@echo off
setlocal

REM Uso:
REM   AWS-Server-Download-And-Extract.bat <ZIP_URL> [ZIP_NAME]
REM Exemplo:
REM   AWS-Server-Download-And-Extract.bat https://SEU_BUCKET.s3.amazonaws.com/SpellRise_Server_WindowsServer_Development.zip

if "%~1"=="" (
  echo [ERRO] Informe a URL do ZIP.
  echo Uso: %~nx0 ^<ZIP_URL^> [ZIP_NAME]
  exit /b 1
)

set "ZIP_URL=%~1"
set "ZIP_NAME=%~2"

if "%ZIP_NAME%"=="" set "ZIP_NAME=SpellRise_Server.zip"

set "ROOT_DIR=C:\SpellRise\Server"
set "DOWNLOAD_DIR=%ROOT_DIR%\Downloads"
set "TARGET_DIR=%ROOT_DIR%\Current"
set "ZIP_PATH=%DOWNLOAD_DIR%\%ZIP_NAME%"

if not exist "%ROOT_DIR%" mkdir "%ROOT_DIR%"
if not exist "%DOWNLOAD_DIR%" mkdir "%DOWNLOAD_DIR%"
if exist "%TARGET_DIR%" rmdir /s /q "%TARGET_DIR%"
mkdir "%TARGET_DIR%"

echo [STEP] Baixando ZIP...
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$ProgressPreference='SilentlyContinue';" ^
  "[Net.ServicePointManager]::SecurityProtocol=[Net.SecurityProtocolType]::Tls12;" ^
  "Invoke-WebRequest -UseBasicParsing -Uri '%ZIP_URL%' -OutFile '%ZIP_PATH%'"

if errorlevel 1 (
  echo [ERRO] Falha no download: %ZIP_URL%
  exit /b 1
)

echo [STEP] Extraindo ZIP...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Expand-Archive -Path '%ZIP_PATH%' -DestinationPath '%TARGET_DIR%' -Force"
if errorlevel 1 (
  echo [ERRO] Falha ao extrair ZIP: %ZIP_PATH%
  exit /b 1
)

echo [STEP] Procurando executavel do servidor...
for /f "delims=" %%F in ('powershell -NoProfile -ExecutionPolicy Bypass -Command "Get-ChildItem -Path '%TARGET_DIR%' -Recurse -Filter '*Server.exe' | Select-Object -First 1 -ExpandProperty FullName"') do set "SERVER_EXE=%%F"

if "%SERVER_EXE%"=="" (
  echo [ERRO] Nenhum *Server.exe encontrado em %TARGET_DIR%
  exit /b 1
)

echo [OK] Download e extracao concluidos.
echo [OK] Executavel: %SERVER_EXE%
echo [TIP] Exemplo de start:
echo      "%SERVER_EXE%" /Game/Maps/Stylized/Stylized -log -Port=7777 -NetDriverListenPort=7777 -unattended -NoCrashDialog

exit /b 0

