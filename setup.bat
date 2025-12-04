@echo off
echo ==========================================
echo      Gameboy Emulator Setup (CMake)
echo ==========================================

:: Create build directory if it doesn't exist
if not exist build mkdir build

:: Enter build directory
cd build

:: Run CMake to generate Visual Studio solution
:: -S .. tells CMake the source is in the parent dir
:: -B . tells CMake to build artifacts in current dir
cmake -S .. -B .

echo.
echo ==========================================
echo Setup Complete! 
echo ==========================================
pause