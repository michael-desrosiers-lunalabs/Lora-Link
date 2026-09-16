echo WORKSPACE=[%WORKSPACE%]
echo programmer=[%1]

@echo off

rem Move to initial directory
cd C:\Users\MichaelDesrosiers\zephyrproject

rem Activate Python virtual environment
call .venv\Scripts\activate

rem Change into the project directory
cd %WORKSPACE%
set FLASH_CMD=west -v flash --runner %1
echo Flash cmd = %FLASH_CMD%
%FLASH_CMD%

rem Flash the build
@REM west -v flash --runner %1