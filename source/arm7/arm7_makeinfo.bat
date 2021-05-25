sort.exe build/.map > arm7_sort.map
arm-eabi-objdump.exe -a _BOOT_MP.arm7.elf > arm7_objdump_a.txt
arm-eabi-objdump.exe -f _BOOT_MP.arm7.elf > arm7_objdump_f.txt
arm-eabi-objdump.exe -p _BOOT_MP.arm7.elf > arm7_objdump_p.txt
arm-eabi-objdump.exe -h _BOOT_MP.arm7.elf > arm7_objdump_h.txt
arm-eabi-objdump.exe -x _BOOT_MP.arm7.elf > arm7_objdump_x.txt
arm-eabi-objdump.exe -s _BOOT_MP.arm7.elf > arm7_objdump_s.txt
arm-eabi-objdump.exe -S _BOOT_MP.arm7.elf > arm7_objdump_src.txt
