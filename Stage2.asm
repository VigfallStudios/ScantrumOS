
jmp EnterPM

%include "Gdt.asm"
%include "print.asm"

EnterPM:
	call EnableA20
	;Disable interrupts
	cli
	;Load GDT
	lgdt [gdt_descriptor]
	;Clusterfucked x86 shit again
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	;Flush (not the toilet btw)
	jmp codeseg:StartPM

EnableA20:
	;Clusterfucked bitwise operations
	;I have no idea how these work
	in al, 0x92
	or al, 2
	out 0x92, al
	ret

[bits 32]

%include "CPUID.asm"
%include "Paging.asm"

StartPM:
	mov ax, dataseg
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	;Protected mode print test
	mov [0xb8000], byte 'H'

	;Detect CPUID and Long Mode
	;So I know you are not lying about having a x86_64 processor
	call DetectCPUID
	call DetectLongMode

	;Paging
	call SetupIdentityPaging

	;Change GDT
	call EditGDT

	;Far jump
	jmp codeseg:StartLM

[bits 64]

[extern _start]

%include "IDT.asm"

StartLM:
	;Clear blue screen
	mov edi, 0xb8000
	;I can assure you these are totally not random values off the top of my head
	mov rax, 0x1f201f201f201f20
	mov ecx, 500
	rep stosq
	call _start
	jmp $

times 2048-($-$$) db 0