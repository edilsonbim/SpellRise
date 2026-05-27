@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Package-Server-Source.ps1" %*
exit /b %errorlevel%
