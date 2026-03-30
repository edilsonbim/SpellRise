@echo off
set "EDITOR=C:\UnrealSource\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
set "PROJECT=%~dp0..\SpellRise.uproject"
set "MAP=/Game/Maps/Stylized/Stylized"
set "PSQL=%SR_PSQL_PATH%"
set "PGCONN=%SR_PG_CONN%"
set "PERSISTENT_SEED=77777777"

if not exist "%EDITOR%" (
  echo UnrealEditor.exe nao encontrado em: %EDITOR%
  exit /b 1
)

if "%PGCONN%"=="" (
  echo SR_PG_CONN nao definido. Defina a variavel de ambiente ^(ver Database\README.md^).
  exit /b 1
)

if "%PSQL%"=="" (
  echo SR_PSQL_PATH nao definido. Defina a variavel de ambiente ^(ver Database\README.md^).
  exit /b 1
)

if not exist "%PSQL%" (
  echo psql.exe nao encontrado em: %PSQL%
  exit /b 1
)

set "SR_PSQL_PATH=%PSQL%"
set "SR_PG_CONN=%PGCONN%"

echo [SpellRise] Iniciando com PostgreSQL + NoSteam + TestPersistentId...
echo [SpellRise] SR_PSQL_PATH=%SR_PSQL_PATH%
echo [SpellRise] SR_PG_CONN=<redacted>
echo [SpellRise] SRTestPersistentId=%PERSISTENT_SEED%

"%EDITOR%" "%PROJECT%" %MAP% -game -log -nosteam -SRPersistenceProvider=postgres -SRPsqlPath="%PSQL%" -SRPgConn="%PGCONN%" -SRTestPersistentId=%PERSISTENT_SEED%

exit /b 0
