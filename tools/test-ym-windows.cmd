@echo off
setlocal
call "%~dp0msvc-env.cmd"
if errorlevel 1 exit /b 1
cd /d "%~dp0\.."
if not exist build\host mkdir build\host
set /p EYM_VERSION=<VERSION
.runtime\python\python.exe tools\prepare-uac-desktop-test.py
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /std:c11 /Ibuild\host\uac tests\host\uac_test.c /Fobuild\host\ /Febuild\host\uac_test.exe
if errorlevel 1 exit /b 1
build\host\uac_test.exe build\amb.bin
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /GR- /std:c++14 /fp:strict /DNDEBUG /DYMFM_FREESTANDING=1 tests\ym_stream_test.c firmware\src\ym\ym_engine.cpp firmware\src\ym\ymfm\ymfm_opn.cpp firmware\src\ym\ymfm\ymfm_adpcm.cpp firmware\src\ym\ymfm\ymfm_ssg.cpp /Fobuild\host\ /Febuild\host\ym_stream_test.exe
if errorlevel 1 exit /b 1
build\host\ym_stream_test.exe build\host\ym-stream-60s.wav
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /std:c++14 tests\ym_psg_test.cpp /Fobuild\host\ /Febuild\host\ym_psg_test.exe
if errorlevel 1 exit /b 1
build\host\ym_psg_test.exe
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /std:c++14 tests\ym_fx_test.cpp /Fobuild\host\ /Febuild\host\ym_fx_test.exe
if errorlevel 1 exit /b 1
build\host\ym_fx_test.exe
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /std:c11 tests\ym_settings_test.c /Fobuild\host\ /Febuild\host\ym_settings_test.exe
if errorlevel 1 exit /b 1
build\host\ym_settings_test.exe
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /std:c11 tests\ota_test.c /Fobuild\host\ /Febuild\host\ota_test.exe
if errorlevel 1 exit /b 1
build\host\ota_test.exe build\emerald-ym-%EYM_VERSION%.fwsc
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /std:c11 tests\ldr_test.c /Fobuild\host\ /Febuild\host\ldr_test.exe
exit /b %errorlevel%
