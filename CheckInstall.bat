@echo off
REM CLEAN VARIABLES

set targetCheck=%1

set PrerequisitesFound=
set CMAKE_PATH=

@echo off

echo %targetCheck%

if not defined targetCheck (call :ValidateAllPrerequisites) else (call :ValidateCMakeOnly)

if ErrorLevel == 1 (
    goto fail
)

goto success

:ValidateCMakeOnly
    echo Checking CMAKE Prerequisite

    set PrerequisitesFound=0

    @echo off
    IF NOT EXIST "%CMAKE_PATH%" (
       for /f "delims=" %%a in ('where cmake') do @set CMAKE_PATH=%%a
    )
    IF NOT EXIST "%CMAKE_PATH%" (
       echo    CMake.......................Error
       set PrerequisitesFound=1
    ) else (
       echo    CMake.......................Found   ["%CMAKE_PATH%"]
    )

    exit /B %PrerequisitesFound%

:ValidateAllPrerequisites
    echo Checking Prerequisites

    set PrerequisitesFound=0

    @echo off
    IF NOT EXIST "%CMAKE_PATH%" (
       for /f "delims=" %%a in ('where cmake') do @set CMAKE_PATH=%%a
    )
    IF NOT EXIST "%CMAKE_PATH%" (
       echo    CMake.......................Error
       set PrerequisitesFound=1
    ) else (
       echo    CMake.......................Found   ["%CMAKE_PATH%"]
    )

    exit /B %PrerequisitesFound%

:success
    echo.
    echo    Checking Prerequisites...SUCCESS
    goto end

:fail
    echo.
    echo    Checking Prerequisites...FAILED
    goto end

:end