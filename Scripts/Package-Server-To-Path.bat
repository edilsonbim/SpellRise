@echo off
setlocal

REM Uso:
REM   Package-Server-To-Path.bat [DESTINO] [MAPA] [CONFIG]
REM Exemplo:
REM   Package-Server-To-Path.bat "C:\Users\biM\Desktop\SpellRise\Server" "/Game/Maps/Stylized/Stylized" "Development"

set "DEST=%~1"
set "MAP=%~2"
set "CFG=%~3"

if "%DEST%"=="" set "DEST=C:\Users\biM\Desktop\SpellRise\Server"
if "%MAP%"=="" set "MAP=/Game/Maps/Stylized/Stylized"
if "%CFG%"=="" set "CFG=Development"

set "ENGINE_ROOT=C:\UnrealSource\UnrealEngine"
set "PROJECT_PATH=%~dp0..\SpellRise.uproject"
set "ZIP_OUT_DIR=C:\SpellRise\Server"
set "ZIP_PATH=%ZIP_OUT_DIR%\SpellRiseServer_Win64_%CFG%.zip"
for %%I in ("%PROJECT_PATH%") do set "PROJECT_PATH=%%~fI"

if not exist "%ENGINE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" (
  echo [ERRO] RunUAT nao encontrado em %ENGINE_ROOT%
  exit /b 1
)

if not exist "%PROJECT_PATH%" (
  echo [ERRO] Projeto nao encontrado em %PROJECT_PATH%
  exit /b 1
)

if exist "%DEST%" rmdir /s /q "%DEST%"
mkdir "%DEST%"
if not exist "%ZIP_OUT_DIR%" mkdir "%ZIP_OUT_DIR%"

echo [STEP] Package SERVER para %DEST%
call "%ENGINE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ^
  -project="%PROJECT_PATH%" ^
  -noP4 -utf8output ^
  -target=SpellRiseServer ^
  -server -noclient ^
  -serverplatform=Win64 ^
  -serverconfig=%CFG% ^
  -build -cook -stage -pak -archive ^
  -archivedirectory="%DEST%" ^
  -map=%MAP%

if errorlevel 1 (
  echo [ERRO] Package do server falhou.
  exit /b 1
)

if exist "%ZIP_PATH%" del /f /q "%ZIP_PATH%"

echo [STEP] Compactando package em %ZIP_PATH%
where tar >nul 2>nul
if %errorlevel%==0 (
  pushd "%DEST%"
  tar -a -c -f "%ZIP_PATH%" *
  set "ZIP_RC=%errorlevel%"
  popd
  if not "%ZIP_RC%"=="0" (
    echo [ERRO] Falha ao compactar com tar.exe.
    exit /b 1
  )
) else (
  powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; Compress-Archive -Path '%DEST%\*' -DestinationPath '%ZIP_PATH%' -Force"
  if errorlevel 1 (
    echo [ERRO] Falha ao compactar o package do server.
    exit /b 1
  )
)

echo [OK] Package concluido em: %DEST%
echo [OK] ZIP gerado em: %ZIP_PATH%
exit /b 0
