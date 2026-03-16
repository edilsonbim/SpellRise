@echo off
setlocal

REM =========================================================
REM Spell Rise - Package Dedicated Server
REM Unreal Engine 5.7.2
REM =========================================================

set "UE_PATH=C:\UnrealEngine-5.7.2-release"
set "PROJECT_PATH=C:\Users\biM\Documents\Unreal Projects\SpellRise\SpellRise.uproject"

set "TARGET=SpellRiseServer"
set "PLATFORM=Win64"
set "CONFIG=Development"
set "OUTPUT_DIR=C:\Users\biM\Desktop\SR_Server"

echo ========================================================
echo Packaging DEDICATED SERVER
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
 -server ^
 -serverconfig=%CONFIG% ^
 -noclient ^
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
    echo SERVER PACKAGE FAILED
    pause
    exit /b %ERR%
)

echo.
echo SERVER PACKAGE SUCCESS
pause
endlocal