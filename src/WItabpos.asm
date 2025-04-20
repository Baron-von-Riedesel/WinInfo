
;*** Tabulatorpositionen in Listboxen ***

	.model small

	.data

TabArray label word
	   dw offset mempos0
	   dw offset mempos1
	   dw offset mempos2
	   dw offset mempos3
	   dw offset mempos4
	   dw offset mempos5
	   dw offset mempos6
	   dw offset mempos7
	   dw offset mempos8
	   dw offset mempos9
	   dw offset mempos10
	   dw offset mempos11
	   dw offset mempos12
	   dw offset mempos13
	   dw offset mempos14
	   dw offset mempos15
	   dw offset mempos16
	   dw offset mempos17
	   dw offset mempos18
	   dw offset mempos19
	   dw offset mempos20
	   dw offset mempos21
	   dw offset mempos22
	   dw offset mempos23
	   dw offset mempos24
	   dw offset mempos25
	   dw offset mempos26
	   dw offset mempos27
	   dw offset mempos28
	   dw offset mempos29
	   dw offset mempos30
	   dw offset mempos31
	   dw offset mempos32
	   dw offset mempos33
	   dw offset mempos34
	   dw offset mempos35
	   dw offset mempos36
	   dw offset mempos37
	   dw offset mempos38
	   dw offset mempos39
	   dw offset mempos40
	   dw offset mempos41
	   dw offset mempos42
	   dw offset mempos43
	   dw offset mempos44
	   dw offset mempos45
	   dw offset mempos46
	   dw offset mempos47
	   dw offset mempos48
	   dw offset mempos49
	   dw offset mempos50
	   dw offset mempos51
	   dw offset mempos52
	   dw offset mempos53
	   dw offset mempos54
	   dw offset mempos55
	   dw offset mempos56
	   dw offset mempos57
	   dw offset mempos58
	   dw offset mempos59
	   dw offset mempos60
	   dw offset mempos61
	   dw offset mempos62
	   dw offset memende

mempos0 label word	;GHeap status 1
	   dw  50		;/* pos: gesamtlaenge */
	   dw  90
	   dw  124		;/* pos: belegt */
	   dw  164
	   dw  208		;/* pos: discardable */
	   dw  248
	   dw  290		;/* pos: discarded */
	   dw  318

mempos1 label word	;Gheap status 2
	   dw 50, 90, 124, 248, 290, 318

mempos2 label word	;Gheap listbox 1
	   dw 26
	   dw 66		;/* Addresse */
	   dw 94 OR 8000h	;/* Size rechtsbuendig */
	   dw 100		;/* Owner */
	   dw 128		;/* Name */
	   dw 172		;/* Locks */
	   dw 196		;/* Flag */
	   dw 206		;/* Attribute */

mempos3 label word	;Module Status 1
	   dw 26		;  /* segs */
	   dw 44
	   dw 76		; /* cs:ip */
	   dw 110
	   dw 144		; /* ss:sp */
	   dw 176
	   dw 202		; /* counts */
	   dw 218
	   dw 246		; /* auto */
	   dw 268

mempos4 dw 16, 40, 64, 86   ;Module Listbox 2
mempos5 dw 24, 68, 86       ;Module Listbox 1
mempos6 dw 26, 166, 170, 268;Module Status 2
mempos7 dw 64, 160, 188, 204, 228, 252;listbox resourcen 1
mempos8 dw 38, 60, 100, 120, 160, 210; status 1 resourcen
mempos9 dw 40, 76, 96, 204, 208, 296;status 2 resourcen
mempos10 dw 40, 52, 64	  ;listbox importe
mempos11 dw 40, 60, 110, 150, 196, 212, 254, 274;status importe

mempos12 label word	;listbox exporte
	   dw 40;
	   dw 80;
	   dw 90;

mempos13 label word	;status exporte
	   dw 30;
	   dw 50;
	   dw 100;
	   dw 140;
	   dw 180;
	   dw 200;
	   dw 240;
	   dw 260;

mempos14 label word	;listbox "importiert von"
	   dw 48;

mempos15 label word	;listbox klassen
	   dw 24;
	   dw 68;
	   dw 92;
	   dw 132;

mempos16 label word	;status windows 1
	   dw 34;
	   dw 58;
	   dw 100;
	   dw 160;
	   dw 180;
	   dw 210;

mempos17 label word	;status windows 2
	   dw 46;
	   dw 88;
	   dw 128;
	   dw 160;
	   dw 180;
	   dw 210;
mempos18 label word	;listbox user windows
	   dw 24;
	   dw 48;
	   dw 72;
	   dw 114;
	   dw 154;
	   dw 202;
mempos19 label word	;listbox mcbs
	   dw  20;
	   dw  60;
	   dw  84;
	   dw  108;
	   dw  150;
mempos20 label word	;status local
	   dw  38;
	   dw  62;
	   dw  100;
	   dw  130;
	   dw  180;
	   dw  220;
mempos21 label word	;listbox local
	   dw 28;
	   dw 56;
	   dw 74 OR 08000h;
	   dw 80;
	   dw 104;
	   dw 128;
mempos22 label word	;listbox dos devices
	   dw 20;
	   dw 74;
	   dw 114;
	   dw 154;
	   dw 164;
mempos23 label word	;status mcbs
	   dw 90;
	   dw 120;
mempos24 label word	;listbox dos CDS
	   dw 20;
	   dw 64;
	   dw 84;
	   dw 124;
mempos25 label word	;status dos CDS
	   dw 30;
	   dw 124;
mempos26 label word	;listbox dos DPB
	   dw 20;
	   dw 64;
	   dw 104;
	   dw 134;
	   dw 160;
	   dw 184;
	   dw 212;
	   dw 252;
mempos27 label word	;status dos DPB
	   dw 20;
	   dw 60;
	   dw 100;
	   dw 120;
mempos28 label word	;listbox DPMI LDT
	   dw 20;
	   dw 60;
	   dw 100;
	   dw 120;
mempos29 label word	;status1 DPMI
	   dw 60;
	   dw 100;
	   dw 130;
	   dw 160;
	   dw 210;
	   dw 250;
mempos30 label word	;listbox system GDT
	   dw 24;
	   dw 64;
	   dw 104;
	   dw 124;
mempos31 label word	;status System GDT
	   dw 34        ;gdtr base:limit
	   dw 84
	   dw 122       ;ldtr
	   dw 152
	   dw 174       ;tr
	   dw 194
	   dw 220       ;cr0
	   dw 252
mempos32 label word	;listbox intr (pm,rm,exc)
	   dw 16;
	   dw 56;
	   dw 104;
mempos33 label word	;status intr
mempos34 label word	;listbox TSS
	   dw 20;
	   dw 40;
	   dw 60;
	   dw 80;
	   dw 100;
	   dw 120;
	   dw 140;
mempos35 label word	;status TSS
	   dw 30;
	   dw 50;
	   dw 80;
	   dw 100;
mempos36 label word	;status wiview
	   dw 20;	/* start selector */
	   dw 44;
	   dw 76;  /* start base */
	   dw 120;
	   dw 152;	/* start limit */
	   dw 190;
	   dw 216;	/* start attr */
	   dw 246;
	   dw 250;
	   dw 308;
mempos37 label word	;listbox IDT
	   dw 16;
	   dw 74;
	   dw 98;
mempos38 label word	;status IDT
	   dw 34, 100
mempos39 label word	;status2 TSS
	   dw 60;
	   dw 110;
mempos40 label word	;listbox XMS
	   dw  30;
	   dw  80;
	   dw  114;
	   dw  134;
mempos41 label word	;status1 XMS
	   dw  40;
	   dw  80;
	   dw  120;
	   dw  160;
mempos42 label word	;status2 XMS
	   dw  40;
	   dw  80;
	   dw  140;
	   dw  180;
mempos43 label word	;status DisAssembler
	   dw  50;
	   dw  78;
mempos44 label word	;listbox DOS
	   dw  20;		/* psp */
	   dw  44;		/* task */
	   dw  84;		/* name */
	   dw  146; 	/* usage */
	   dw  162; 	/* handle */
	   dw  176; 	/* mode */
mempos45 label word	;status DOS
	   dw  40;
	   dw  60;
	   dw  110;
	   dw  130;
mempos46 label word	;childwindows wiwindow
	   dw 22;
	   dw 44;
	   dw 80;
	   dw 116;
mempos47 label word	;status Paging
	   dw 32, 72, 120, 160;
mempos48 label word	;status DPMI enhanced
	   dw 8, -8
mempos49 label word	;listbox DPMI enhanced
	   dw 36, 54, 90, 126, 162, 198, 234;
mempos50 label word	;status 2 DPMI
	   dw 8, -8;
mempos51 label word	;listbox gdi devices
	   dw 120, 164;
mempos52 label word	;status atome
	   dw 40, 60, 100, 120
mempos53 label word	;listbox atome
	   dw 20, 48, 80, 110;
mempos54 label word	;status objects
	   dw 30, 50, 80, 100;
mempos55 label word	;listbox objects
	   dw 64, 94;
mempos56 label word	;status all windows
	   dw 40, 80, 120, 160, 190, 210
mempos57 label word	;listbox entries
	   dw 24, 48, 80, 104
mempos58 label word	; Module Status 0
	   dw 30
	   dw 46		;/* anzahl */
	   dw 96
	   dw 130		;/* sortiert nach */
mempos59 label word	;listbox page table
	   dw 46, 80, 120
mempos60 label word	;listbox page directory
	   dw 80, 110, 150, 180
mempos61 label word	;listbox PM Int Vecs
	   dw 32
mempos62 label word	;status PM Int Vecs
	   dw 50, 90
memende label word

	.code

LoadTabs proc far pascal uses si di wIndex:word, wDst:word
	mov si, wIndex
	shl si, 1
	mov ax,[si+TabArray]
	mov cx,[si+TabArray+2]
	sub cx,ax
	shr cx,1
	mov si,ax
	mov di,wDst
	push ds
	pop es
	cld
	mov ax,cx
;	stosw
	rep movsw
	ret
LoadTabs endp

	end
