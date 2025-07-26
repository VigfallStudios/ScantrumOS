;Function to print a string from BX
;Only works in real mode and uses
;int 10h
PutChars:
	push ax
	push bx

	mov ah, 0x0e
.Loop:
	cmp [bx], byte 0
	je .Done
	mov al, [bx]
	int 0x10
	inc bx
	jmp .Loop
.Done:
	pop ax
	pop bx
	ret