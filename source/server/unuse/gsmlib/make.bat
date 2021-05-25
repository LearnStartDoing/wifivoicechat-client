call D:\MyDocuments\bcctest\set_bccenv.bat
@echo off
cls
del gsmlib.obj
bcc32.exe -O2 -5 -u -DNeedFunctionPrototypes=1 gsmlib.c
pause
make.bat
