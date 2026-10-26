@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
if errorlevel 1 exit /b 1
cd /d "%~dp0\.."
if not exist build\host mkdir build\host
cl /nologo /O2 /MT /GR- /std:c++14 /fp:strict /DNDEBUG /DYMFM_FREESTANDING=1 tests\ym_stream_test.c firmware\src\ym\ym_engine.cpp firmware\src\ym\ymfm\ymfm_opn.cpp firmware\src\ym\ymfm\ymfm_adpcm.cpp firmware\src\ym\ymfm\ymfm_ssg.cpp /Febuild\host\ym_stream_test.exe
if errorlevel 1 exit /b 1
build\host\ym_stream_test.exe build\host\ym-stream-60s.wav
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /std:c11 tests\ota_test.c /Febuild\host\ota_test.exe
if errorlevel 1 exit /b 1
build\host\ota_test.exe build\emerald-ym-0.1.4.fwsc
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /std:c11 tests\ldr_test.c /Febuild\host\ldr_test.exe
if errorlevel 1 exit /b 1
build\host\ldr_test.exe build\emerald-ym-0.1.0.fwsc build\emerald-ym-0.1.4.fwsc
exit /b %errorlevel%
