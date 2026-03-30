@echo off
setlocal
powershell -ExecutionPolicy Bypass -File "%~dp0Run-Agent-Orchestrator.ps1" %*
exit /b %ERRORLEVEL%
