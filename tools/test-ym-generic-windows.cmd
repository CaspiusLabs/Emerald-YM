@echo off
setlocal
call "%~dp0msvc-env.cmd"
if errorlevel 1 exit /b 1
cd /d "%~dp0\.."
if not exist build\host\generic mkdir build\host\generic
cl /nologo /O2 /MT /GR- /std:c++14 /fp:strict /DNDEBUG /DYMFM_FREESTANDING=1 /DEMERALD_GENERIC_OPN=1 tests\ym_stream_test.c firmware\src\ym\ym_engine.cpp firmware\src\ym\ymfm\ymfm_opn.cpp firmware\src\ym\ymfm\ymfm_adpcm.cpp firmware\src\ym\ymfm\ymfm_ssg.cpp /Fobuild\host\generic\ /Febuild\host\generic\ym_stream.exe
if errorlevel 1 exit /b 1
build\host\generic\ym_stream.exe
exit /b %errorlevel%
