
        .286
        .model small
        .386

_TRACE_ = 0

OutputdebugString proto far pascal pString:dword

        .code

if _TRACE_
text0   db "WINUTIL2 will start now",cr,lf,00
text1   db "WINUTIL2 will exit now",cr,lf,00
endif

;--- return stack space available
;--- called by lqsort()
;--- which in turn is called by ImportNext() [implist.c]

stackavail proc near c

        mov ax,sp
        sub ax,ss:[000Ah]  ;assume contains _end + x
        ret

stackavail endp

;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³** windows main procedure (DLL)   **³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
LibEntry proc far pascal

if _TRACE_
        push  cs
        push  offset text0
        call  OutputDebugString
endif
        mov     ax,1
        ret
LibEntry endp

;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³** windows exit procedure (DLL)   **³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
WEP     proc far pascal w1:word

if _TRACE_
        push  cs
        push  offset text1
        call  OutputDebugString
endif
        mov     ax,1
        ret
WEP     endp

        END LibEntry

