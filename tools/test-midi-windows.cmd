@echo off
setlocal
call "%~dp0msvc-env.cmd"
if errorlevel 1 exit /b 1
cd /d "%~dp0\.."
if not exist build\host mkdir build\host
.runtime\python\python.exe tools\prepare-midi-desktop-test.py
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /std:c11 /Ibuild\host\midi tests\ym_midi_test.c /Fobuild\host\ /Febuild\host\ym_midi_test.exe
if errorlevel 1 exit /b 1
build\host\ym_midi_test.exe
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /GR- /std:c++14 /fp:strict /DYMFM_FREESTANDING=1 tests\ym_clock_test.cpp firmware\src\ym\ym_engine.cpp firmware\src\ym\ymfm\ymfm_opn.cpp firmware\src\ym\ymfm\ymfm_adpcm.cpp firmware\src\ym\ymfm\ymfm_ssg.cpp /Fobuild\host\ /Febuild\host\ym_clock_test.exe
if errorlevel 1 exit /b 1
build\host\ym_clock_test.exe
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /GR- /std:c++14 /fp:strict /DYMFM_FREESTANDING=1 /Ibuild\host\midi tests\ym_midi_out_test.cpp firmware\src\ym\ym_engine.cpp firmware\src\ym\ymfm\ymfm_opn.cpp firmware\src\ym\ymfm\ymfm_adpcm.cpp firmware\src\ym\ymfm\ymfm_ssg.cpp /Fobuild\host\ /Febuild\host\ym_midi_out_test.exe
if errorlevel 1 exit /b 1
build\host\ym_midi_out_test.exe
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /GR- /std:c++14 /fp:strict /DYMFM_FREESTANDING=1 /Ibuild\host\midi tests\ym_midi_sync_test.cpp firmware\src\ym\ym_engine.cpp firmware\src\ym\ymfm\ymfm_opn.cpp firmware\src\ym\ymfm\ymfm_adpcm.cpp firmware\src\ym\ymfm\ymfm_ssg.cpp /Fobuild\host\ /Febuild\host\ym_midi_sync_test.exe
if errorlevel 1 exit /b 1
build\host\ym_midi_sync_test.exe
if errorlevel 1 exit /b 1
cl /nologo /O2 /MT /GR- /std:c++14 /fp:strict /DYMFM_FREESTANDING=1 /Ibuild\host\midi tests\ym_song_test.cpp firmware\src\ym\ym_engine.cpp firmware\src\ym\ymfm\ymfm_opn.cpp firmware\src\ym\ymfm\ymfm_adpcm.cpp firmware\src\ym\ymfm\ymfm_ssg.cpp /Fobuild\host\ /Febuild\host\ym_song_test.exe
if errorlevel 1 exit /b 1
build\host\ym_song_test.exe
exit /b %errorlevel%
