@echo off
setlocal

REM =========================================================
REM Spell Rise - Package Client
REM Unreal Engine 5.7.2
REM =========================================================

set "UE_PATH=C:\UnrealEngine-5.7.2-release"
set "PROJECT_PATH=C:\Users\biM\Documents\Unreal Projects\SpellRise\SpellRise.uproject"

set "TARGET=SpellRiseClient"
set "PLATFORM=Win64"
set "CONFIG=Development"
set "OUTPUT_DIR=C:\Users\biM\Desktop\SR_Client"

echo ========================================================
echo Packaging CLIENT
echo Target   : %TARGET%
echo Platform : %PLATFORM%
echo Config   : %CONFIG%
echo Output   : %OUTPUT_DIR%
echo ========================================================

if not exist "%UE_PATH%\Engine\Build\BatchFiles\RunUAT.bat" (
    echo ERROR: RunUAT.bat not found:
    echo %UE_PATH%\Engine\Build\BatchFiles\RunUAT.bat
    pause
    exit /b 1
)

if not exist "%PROJECT_PATH%" (
    echo ERROR: Project file not found:
    echo %PROJECT_PATH%
    pause
    exit /b 1
)

if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%"
)

call "%UE_PATH%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ^
 -project="%PROJECT_PATH%" ^
 -target=%TARGET% ^
 -platform=%PLATFORM% ^
 -clientconfig=%CONFIG% ^
 -build ^
 -cook ^
 -stage ^
 -package ^
 -archive ^
 -archivedirectory="%OUTPUT_DIR%" ^
 -nop4

set "ERR=%ERRORLEVEL%"

if not "%ERR%"=="0" (
    echo.
    echo CLIENT PACKAGE FAILED
    pause
    exit /b %ERR%
)

echo.
echo CLIENT PACKAGE SUCCESS
pause
endlocal