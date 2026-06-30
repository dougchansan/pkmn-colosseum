@echo off
REM ============================================================================
REM ghidra_batch_decompile.bat - Windows wrapper for Ghidra headless decompile
REM
REM Runs analyzeHeadless directly without the Python wrapper, for cases where
REM Python is not available or the user wants a simpler invocation.
REM
REM Usage:
REM     tools\ghidra_batch_decompile.bat
REM     tools\ghidra_batch_decompile.bat --reimport
REM ============================================================================

setlocal enabledelayedexpansion

set "PROJECT_ROOT=%~dp0.."
set "GHIDRA_HOME=%PROJECT_ROOT%\..\tools\ghidra_12.0.4_PUBLIC"
set "DOL=%PROJECT_ROOT%\orig\GC6E01\start.dol"
set "PROJECT_DIR=%PROJECT_ROOT%\build\ghidra_project"
set "PROJECT_NAME=PokemonColosseum"
set "SCRIPT_DIR=%PROJECT_ROOT%\tools\ghidra_scripts"
set "OUTPUT_DIR=%PROJECT_ROOT%\build\ghidra_output"
set "OUTPUT_FILE=%OUTPUT_DIR%\raw_decompilation.c"
set "PROCESSOR=PowerPC:BE:32:Gekko_Broadway"
set "ANALYZE=%GHIDRA_HOME%\support\analyzeHeadless.bat"

REM Check that Ghidra exists.
if not exist "%ANALYZE%" (
    echo ERROR: analyzeHeadless.bat not found at %ANALYZE%
    echo Set GHIDRA_HOME environment variable or edit this script.
    exit /b 1
)

REM Check that the DOL exists.
if not exist "%DOL%" (
    echo ERROR: DOL not found at %DOL%
    exit /b 1
)

REM Create output directories.
if not exist "%PROJECT_DIR%" mkdir "%PROJECT_DIR%"
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

REM Handle --reimport flag.
set "REIMPORT=0"
if "%~1"=="--reimport" set "REIMPORT=1"

REM Check if project already exists.
set "PROJECT_FILE=%PROJECT_DIR%\%PROJECT_NAME%.gpr"

if "%REIMPORT%"=="1" (
    if exist "%PROJECT_FILE%" (
        echo Removing existing Ghidra project...
        del /q "%PROJECT_DIR%\%PROJECT_NAME%.*" 2>nul
        rmdir /s /q "%PROJECT_DIR%\%PROJECT_NAME%.rep" 2>nul
    )
)

echo ========================================================================
echo Ghidra Headless Batch Decompilation
echo ========================================================================
echo.
echo Ghidra:   %ANALYZE%
echo DOL:      %DOL%
echo Output:   %OUTPUT_FILE%
echo.

if exist "%PROJECT_FILE%" (
    if "%REIMPORT%"=="0" (
        echo Project exists, running postScript only (no re-import^).
        echo.
        call "%ANALYZE%" "%PROJECT_DIR%" "%PROJECT_NAME%" ^
            -process "%PROJECT_NAME%" ^
            -noanalysis ^
            -scriptPath "%SCRIPT_DIR%" ^
            -postScript ExportDecompilation.java "%OUTPUT_FILE%"
        goto :done
    )
)

echo Importing DOL and running full analysis...
echo This may take 10-30 minutes.
echo.

call "%ANALYZE%" "%PROJECT_DIR%" "%PROJECT_NAME%" ^
    -import "%DOL%" ^
    -processor "%PROCESSOR%" ^
    -scriptPath "%SCRIPT_DIR%" ^
    -postScript ExportDecompilation.java "%OUTPUT_FILE%"

:done
echo.
if exist "%OUTPUT_FILE%" (
    echo Success! Raw decompilation written to:
    echo   %OUTPUT_FILE%
    echo.
    echo Next step: python tools\process_ghidra_output.py
) else (
    echo WARNING: Output file not found: %OUTPUT_FILE%
    echo Check the Ghidra log output above for errors.
    exit /b 1
)

endlocal
