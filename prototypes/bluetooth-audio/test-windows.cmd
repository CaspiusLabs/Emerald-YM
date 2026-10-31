@echo off
setlocal
call "%~dp0..\..\tools\msvc-env.cmd"
if errorlevel 1 exit /b 1
cd /d "%~dp0..\.."
if not exist build\bt-prototype mkdir build\bt-prototype
cl /nologo /W4 /WX /O2 /MT /TC prototypes\bluetooth-audio\test_pcm_queue.c prototypes\bluetooth-audio\pcm_queue.c prototypes\bluetooth-audio\sbc_queue.c /Fobuild\bt-prototype\ /Febuild\bt-prototype\test-pcm.exe
if errorlevel 1 exit /b 1
build\bt-prototype\test-pcm.exe
if errorlevel 1 exit /b 1
cl /nologo /W4 /WX /O2 /MT /TC /DEYM_BT_PCM_FRAMES=1024 /DEYM_BT_SBC_SLOTS=4 prototypes\bluetooth-audio\test_pcm_queue.c prototypes\bluetooth-audio\pcm_queue.c prototypes\bluetooth-audio\sbc_queue.c /Fobuild\bt-prototype\ /Febuild\bt-prototype\test-pcm-lean.exe
if errorlevel 1 exit /b 1
build\bt-prototype\test-pcm-lean.exe
exit /b %errorlevel%
