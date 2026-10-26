@echo off
setlocal
call "%~dp0msvc-env.cmd"
if errorlevel 1 exit /b 1
cd /d "%~dp0\.."
if not exist build\host mkdir build\host
cl /nologo /O2 /MT /std:c11 /Ibuild\gen tests\ym_ui_test.c /Fobuild\host\ /Febuild\host\ym_ui_test.exe
if errorlevel 1 exit /b 1
build\host\ym_ui_test.exe
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /std:c11 tests\ym_led_test.c /Fobuild\host\ /Febuild\host\ym_led_test.exe
if errorlevel 1 exit /b 1
build\host\ym_led_test.exe
exit /b %errorlevel%
