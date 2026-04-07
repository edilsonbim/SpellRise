@echo off
setlocal

set "PROJECT=%~dp0..\SpellRise.uproject"
for %%I in ("%PROJECT%") do set "PROJECT=%%~fI"

if not exist "%PROJECT%" (
  echo [ERRO] Projeto nao encontrado: %PROJECT%
  exit /b 1
)

start "" "%PROJECT%"
exit /b 0

