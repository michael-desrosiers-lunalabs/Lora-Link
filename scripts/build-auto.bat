@echo off

echo WORKSPACE=[%WORKSPACE%]
echo BOARD=[%1]
echo EXTRA_ARGS=[%2]

rem Move to initial directory
cd C:\Users\MichaelDesrosiers\zephyrproject

rem Activate Python virtual environment
call .venv\Scripts\activate.bat

rem Change into the project directory
cd %WORKSPACE%

:: Get short commit hash
for /f %%i in ('git rev-parse --short HEAD') do set COMMIT_HASH=%%i

:: Check for any working tree or staged modifications
set DIRTY=
git diff --quiet
if %ERRORLEVEL% == 1 set DIRTY=-dirty

git diff --cached --quiet
if %ERRORLEVEL% == 1 set DIRTY=-dirty

:: Final version string
set GIT_VERSION=%COMMIT_HASH%%DIRTY%

echo Git version: %GIT_VERSION%


rem Run the Zephyr build
@REM west build -p auto -b %1 .
set ROLE_ARG=
if not "%~2"=="" set ROLE_ARG=-DEXTRA_CFLAGS="-D%~2"

rem Store the west command in a variable
set WEST_CMD=west build -p auto -b %1 . -DFW_VERSION="%GIT_VERSION%" %ROLE_ARG%

rem Print the command explicitly
echo Executing: %WEST_CMD%

rem Run the command
%WEST_CMD%
