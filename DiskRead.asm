
PROGRAM_SPACE equ 0x8000

ReadDisk:
	;Function 02h
	mov ah, 0x02
	;Where to load it in
	mov bx, PROGRAM_SPACE
	;How many sectors
	mov al, 38
	;Disk
	mov dl, [BOOT_DISK]
	;CH
	mov ch, 0x00
	mov dh, 0x00
	;Starting sector
	mov cl, 0x02

	int 0x13

	jc DiskFail

	ret

BOOT_DISK:
	db 0

DiskError:
	db 'READ FAILED', 0

DiskFail:
	mov bx, DiskError
	call PutChars
	
	jmp $