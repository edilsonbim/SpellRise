@echo off
setlocal

REM Verifica no log se o servidor subiu com Postgres ativo e sem fallback.
REM Uso:
REM   Check-Server-Persistence-Health.bat [LOGPATH]
REM Exemplo:
REM   Check-Server-Persistence-Health.bat C:\SpellRise\Server\SpellRise.log

set "LOGFILE=%~1"
if "%LOGFILE%"=="" set "LOGFILE=C:\SpellRise\Server\SpellRise.log"

if not exist "%LOGFILE%" (
  echo [ERRO] Log nao encontrado: %LOGFILE%
  exit /b 1
)

set "HAS_REQ=0"
set "HAS_ACTIVE=0"
set "HAS_FALLBACK=0"
set "HAS_DB_INACTIVE=0"
set "HAS_FATAL=0"

findstr /I /C:"[Persistence][ProviderInit] Requested=postgres" "%LOGFILE%" >nul && set "HAS_REQ=1"
findstr /I /C:"[Persistence][ProviderInit] Active=Postgres Ready=1" "%LOGFILE%" >nul && set "HAS_ACTIVE=1"
findstr /I /C:"falling back to file provider" "%LOGFILE%" >nul && set "HAS_FALLBACK=1"
findstr /I /C:"[Persistence][DatabaseInactive]" "%LOGFILE%" >nul && set "HAS_DB_INACTIVE=1"
findstr /I /C:"[Persistence][ProviderInitFatal]" "%LOGFILE%" >nul && set "HAS_FATAL=1"

echo [INFO] LOG=%LOGFILE%
echo [INFO] RequestedPostgres=%HAS_REQ% ActivePostgresReady=%HAS_ACTIVE% FallbackToFile=%HAS_FALLBACK% DatabaseInactive=%HAS_DB_INACTIVE% ProviderInitFatal=%HAS_FATAL%

if "%HAS_REQ%"=="1" if "%HAS_ACTIVE%"=="1" if "%HAS_FALLBACK%"=="0" if "%HAS_DB_INACTIVE%"=="0" if "%HAS_FATAL%"=="0" (
  echo [OK] Persistencia saudavel: Postgres ativo, sem fallback.
  exit /b 0
)

echo [ERRO] Persistencia NAO saudavel. Revise SR_PG_CONN, conectividade com banco e flags de inicializacao.
exit /b 2
