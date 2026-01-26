@echo off
setlocal enabledelayedexpansion

rem ==== 1) MMMENGINE_DIR: 이 bat이 있는 폴더를 엔진 루트로 간주 ====
set "ENGINE_DIR=%~dp0"
rem 끝의 \ 제거
if "%ENGINE_DIR:~-1%"=="\" set "ENGINE_DIR=%ENGINE_DIR:~0,-1%"

rem 현재 CMD 세션에서만 유효(안전)
set "MMMENGINE_DIR=%ENGINE_DIR%"
echo [INFO] MMMENGINE_DIR=%MMMENGINE_DIR%

rem 영구 등록이 필요하면 아래 줄을 사용(권한/환경에 따라 주의)
rem setx MMMENGINE_DIR "%ENGINE_DIR%" >nul

rem ==== 2) 구성 선택 (기본 Debug) ====
set "CONFIG=%1"
if "%CONFIG%"=="" set "CONFIG=Debug"
if /I not "%CONFIG%"=="Debug" if /I not "%CONFIG%"=="Release" (
  echo [ERROR] Usage: BuildEditor.bat [Debug^|Release]
  exit /b 1
)

set "PLATFORM=x64"
set "SLN=%ENGINE_DIR%\MMMEngine.sln"

if not exist "%SLN%" (
  echo [ERROR] Solution not found: %SLN%
  exit /b 1
)

rem ==== 3) MSBuild 찾기(vswhere) ====
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
  echo [ERROR] vswhere not found: %VSWHERE%
  exit /b 1
)

for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
  set "MSBUILD=%%i"
  goto :found_msbuild
)

:found_msbuild
if not exist "%MSBUILD%" (
  echo [ERROR] MSBuild not found.
  exit /b 1
)

echo [INFO] MSBuild=%MSBUILD%
echo [INFO] Building %SLN% (%PLATFORM%/%CONFIG%)

rem ==== 4) 빌드 ====
"%MSBUILD%" "%SLN%" /t:Build /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /m:1 /v:minimal /nologo
if errorlevel 1 (
  echo [ERROR] Build failed.
  exit /b 1
)

rem ==== 5) 산출물 복사: MMMEngine\x64\{Config} -> Common\Bin\{Config} ====
set "SRC=%ENGINE_DIR%\%PLATFORM%\%CONFIG%"
set "DST=%ENGINE_DIR%\Common\Bin\%CONFIG%"

if not exist "%DST%" mkdir "%DST%"

echo [INFO] Copy binaries: "%SRC%" -> "%DST%"
robocopy "%SRC%" "%DST%" *.dll *.pdb *.exe /NFL /NDL /NJH /NJS /NP /R:3 /W:1
if %ERRORLEVEL% GEQ 8 (
  echo [ERROR] Copy failed. robocopy errorlevel=%ERRORLEVEL%
  exit /b %ERRORLEVEL%
)

echo [OK] Done.
exit /b 0
