@echo off
setlocal
powershell -ExecutionPolicy Bypass -File "%~dp0Package-ClientServer-Source.ps1" %*
exit /b %errorlevel%
