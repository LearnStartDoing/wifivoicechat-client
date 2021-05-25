@echo off
call setenv_devkitPror20.bat

:loop
cls
goto skipclean
del /Q arm7\build\*.*
del /Q arm9\build\*.*
:skipclean

del arm7\_BOOT_MP.arm7.elf
del arm9\_BOOT_MP.arm9.elf
del _BOOT_MP.nds
make
if exist _BOOT_MP.nds goto run
pause
goto loop

:run
pause
goto loop

