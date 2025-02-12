
;--- interface to 32-bit disassembler

	.286
	.model small
	.386


_FTEXT segment dword use32 public 'CODE'

;--- Unassemble(near32 pData, far32 pCode, dword accbits, far16 readproc )

extern _Unassemble@20:near32

;--- returns a WORD
GetSymbolicNameEx proto far16 pascal pCode:far16 ptr, pData:far16 ptr

;--- callback for Unassemble()
;--- pOut: near32 ptr - hiword=0
;--- qwAddress: far16 ptr: hiwords=0

SearchSymbol proc stdcall pOut:dword, qwAddress:qword

;--- it's 32-bit code, but still a 16-bit stack. The
;--- Windows VMM may trash hiword(esp)!
	movzx ebp,bp

	mov ecx, es
	shl ecx, 16
	mov cx, word ptr pOut+0
	mov eax, dword ptr qwAddress+0
	mov edx, dword ptr qwAddress+4
	push es
	push ebx
	invoke GetSymbolicNameEx, dx::ax, ecx
	pop ebx
	pop es
	movzx eax,ax
	ret
SearchSymbol endp

;--- call the disassembler

_unass:

    push esi
    mov esi,esp
    and sp,not 3   ;align stack to dword

    push cs
    pop ds
    pushw _TEXT
    push offset readproc
    push ebx       ;flags
    push edx       ;pCode
    push eax       ;pCode
    movzx ecx,cx
    push ecx       ;pData
    call _Unassemble@20

    mov sp,si
    pop esi

    retw

_FTEXT ends

	.code

;--- returns new offset in DX:AX

UnAssemble proc far pascal public pCode:fword,pData:ptr far16,flags:word

	push ds
	mov eax, dword ptr pCode+0
	mov dx,  word ptr pCode+4
	mov cx, word ptr pData+0
	mov es, word ptr pData+2
	movzx ebx, flags
	db 09ah                    ; call far16 _unass()
	dw lowword offset _unass
	dw _FTEXT
	pop ds
	shld edx,eax,16
	ret

UnAssemble endp

;--- callback for disassembler to read code bytes
;--- in: ds:esi -> bytes to read
;--- return value in AL/AX/EAX

readproc proc far stdcall dwType:dword
	pushf
	cmp dwType,1
	jz read1
	cmp dwType,2
	jz read2
	lodsd [esi]
	popf
if 0        ; don't use RET; the assembler would create a LEAVE then
	ret
else
	pop bp
	retf 4
endif
read2:
	lodsw [esi]
	popf
if 0
	ret
else
	pop bp
	retf 4
endif
read1:
	lodsb [esi]
	popf
if 0
	ret
else
	pop bp
	retf 4
endif
readproc endp

	END

