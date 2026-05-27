@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Package-Client-Source.ps1" %*
exit /b %errorlevel%
