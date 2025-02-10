
	.286
	.model small
	.386

externdef __WINFLAGS:far

NULL = 0

WF_PMODE        =    0001h
WF_CPU286       =    0002h
WF_CPU386       =    0004h
WF_CPU486       =    0008h
WF_STANDARD     =    0010h
WF_WIN286       =    0010h
WF_ENHANCED     =    0020h

OutputDebugString proto far pascal lpString:far ptr byte

_FTEXT segment use32 dword public 'CODE'
_FTEXT ends

	.code

errtext db 'WINUTIL3: CPU must be 80386 or better',13,10,00

;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³** windows main procedure (DLL)   **³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
LibEntry proc pascal

	mov ax,offset __WINFLAGS
	and ax,WF_CPU386 or WF_CPU486
	jnz @F
	invoke OutputDebugString, addr errtext
	xor ax,ax
	retf
@@:
if 1
	sub sp,8
	mov di,sp
	push ss
	pop es
	mov bx,_FTEXT
	mov ax,000Bh
	int 31h
	or byte ptr es:[di+6],40h
	mov ax,000Ch
	int 31h
	add sp,8
endif
	mov ax,1
	retf
LibEntry endp


;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³** windows exit procedure (DLL)   **³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
Wep proc far pascal w1:word

	mov ax,1
	ret
Wep endp

	END  LibEntry

