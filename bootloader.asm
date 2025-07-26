
;Origin
[org 0x7c00]

;Store for later use
mov [BOOT_DISK], dl

;Setup the stack
mov bp, 0x7c00
mov sp, bp

;Read the disk
call ReadDisk

;Load it
jmp PROGRAM_SPACE

%include "print.asm"
%include "DiskRead.asm"

;Boot sector bytes
times 510-($-$$) db 0
dw 0xAA55