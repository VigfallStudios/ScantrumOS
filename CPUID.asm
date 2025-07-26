
DetectCPUID:
	pushfd
	pop eax

	mov ecx, eax

	xor eax, 1 << 21

	push eax
	popfd

	pushfd
	pop eax

	push ecx
	popfd

	xor eax, ecx
	jz NoLMOrCPUID
	ret

DetectLongMode:
	mov eax, 0x80000001
	cpuid
	test edx, 1 << 29
	jz NoLMOrCPUID
	ret

NoLMOrCPUID:
	hlt ;Fucker I said you need x86_64 hardware