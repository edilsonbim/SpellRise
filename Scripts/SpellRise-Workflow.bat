@echo off
setlocal
powershell -ExecutionPolicy Bypass -File "%~dp0SpellRise-Workflow.ps1" %*
exit /b %ERRORLEVEL%
