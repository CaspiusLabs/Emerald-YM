@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 exit /b 1
cd /d "%~dp0\.."
if not exist build\host mkdir build\host
cl /nologo /O2 /MT /std:c11 /Ibuild\gen tests\ym_ui_test.c /Febuild\host\ym_ui_test.exe
if errorlevel 1 exit /b 1
build\host\ym_ui_test.exe
exit /b %errorlevel%
