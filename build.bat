@echo off
setlocal

set BUILD_DIR=build

if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)
cd %BUILD_DIR%

:: Thêm dòng này để tránh lỗi FreeType dùng CMake < 3.5
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5

cmake --build . --config Release

.\Release\SudokuGame.exe