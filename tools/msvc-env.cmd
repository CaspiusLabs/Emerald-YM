@echo off
if defined VSCMD_VER exit /b 0
set "EYM_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%EYM_VSWHERE%" (
 echo Visual Studio C++ Build Tools are required for desktop tests.
 exit /b 1
)
for /f "usebackq tokens=*" %%i in (`"%EYM_VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "EYM_VS=%%i"
if not defined EYM_VS exit /b 1
call "%EYM_VS%\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
exit /b %errorlevel%
