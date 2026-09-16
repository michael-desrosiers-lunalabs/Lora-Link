echo WORKSPACE=[%WORKSPACE%]

@echo off
set /p addr="Enter hex address (e.g., 0x00001234): "

echo.
echo Resolving address: %addr%
echo -----------------------------------------

cd %WORKSPACE%

"C:\Xilinx\SDK\2018.2\gnu\aarch32\nt\gcc-arm-none-eabi\bin\arm-none-eabi-addr2line.exe" -e build/zephyr/zephyr.elf %addr%

echo -----------------------------------------
pause