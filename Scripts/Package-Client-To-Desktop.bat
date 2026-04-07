@echo off
setlocal

REM Uso:
REM   Package-Client-To-Desktop.bat [DESTINO] [MAPA] [CONFIG]
REM Exemplo:
REM   Package-Client-To-Desktop.bat "%USERPROFILE%\Desktop\SpellRise\Client\Current" "/Game/Maps/Stylized/Stylized" "Development"

set "DEST=%~1"
set "MAP=%~2"
set "CFG=%~3"

if "%DEST%"=="" set "DEST=%USERPROFILE%\Desktop\SpellRise\Client\Current"
if "%MAP%"=="" set "MAP=/Game/Maps/Stylized/Stylized"
if "%CFG%"=="" set "CFG=Development"

set "ENGINE_ROOT=C:\UnrealSource\UnrealEngine"
set "PROJECT_PATH=%~dp0..\SpellRise.uproject"
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

echo [STEP] Package CLIENT para %DEST%
call "%ENGINE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ^
  -project="%PROJECT_PATH%" ^
  -noP4 -utf8output ^
  -target=SpellRiseClient ^
  -platform=Win64 ^
  -clientconfig=%CFG% ^
  -build -cook -stage -pak -archive ^
  -archivedirectory="%DEST%" ^
  -map=%MAP%

if errorlevel 1 (
  echo [ERRO] Package do client falhou.
  exit /b 1
)

echo [OK] Package CLIENT concluido em: %DEST%
exit /b 0

