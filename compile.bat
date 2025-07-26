nasm bootloader.asm -f bin -o bootloader.bin
nasm Stage2.asm -f elf64 -o Stage2.o
nasm Binaries.asm -f elf64 -o Binaries.o

wsl $WSLENV/x86_64-elf-gcc -Ttext 0x8000 -ffreestanding -mno-red-zone -m64 -c "Kernel.cpp" -o "Kernel.o"

wsl $WSLENV/x86_64-elf-ld -T"link.ld"

copy /b bootloader.bin+kernel.bin os.img