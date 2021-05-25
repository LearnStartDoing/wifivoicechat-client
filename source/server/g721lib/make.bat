call D:\MyDocuments\bcctest\set_bccenv.bat
@echo off
cls
del g721lib.obj
bcc32.exe -O2 -5 -u g721lib.c
pause
make.bat
