@echo off
echo ==========================================
echo    Gameboy Emulator Web Build (CMake)
echo ==========================================

if not exist build_web mkdir build_web

echo [1/2] Configuring...
call emcmake cmake -S . -B build_web -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo CONFIGURATION FAILED - exit code %ERRORLEVEL%
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [2/2] Building...
cmake --build build_web
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo BUILD FAILED - exit code %ERRORLEVEL%
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ==========================================
echo SUCCESS
echo Output in build_web\bin\:
echo   FeelsEmuGB.html
echo   FeelsEmuGB.js
echo   FeelsEmuGB.wasm
echo ==========================================
pause