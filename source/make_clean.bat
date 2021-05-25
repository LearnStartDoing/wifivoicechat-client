@echo off

echo クリーンコンパイルのために一時ファイルを削除します。makeの自動実行はしません。
echo 中止する場合は(Windows)閉じるボタンを押すか、(DOS)Ctrl+Cを押して下さい。
pause

rd /q /s arm7\build\
rd /q /s arm9\build\

del _BOOT_MP.arm7
del _BOOT_MP.arm9
del _BOOT_MP.nds
del _BOOT_MP.ds.gba
del arm7\_BOOT_MP.arm7.elf
del arm9\_BOOT_MP.arm9.elf

