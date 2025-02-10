
;*** Tabulatorpositionen in Listboxen ***
;*** werden als 1 User-Resource eingebunden ***

_data	segment

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

mempos0 label byte	   ;Heap status 1
	   dw  8
	   dw  50		   ;/* pos: gesamtlaenge */
	   dw  90
	   dw  124		   ;/* pos: belegt */
	   dw  164
	   dw  208		   ;/* pos: discardable */
	   dw  248
	   dw  290		   ;/* pos: discarded */
	   dw  318

mempos1 label byte	   ;heap status 2
	   dw  6
	   dw 50
	   dw 90
	   dw 124
	   dw 248
	   dw 290
	   dw 318

mempos2 label byte	   ;global heap listbox 1
	   dw 8
	   dw 26
	   dw 66			;/* Addresse */
	   dw 94 OR 8000h	;/* Size rechtsbuendig */
	   dw 100			;/* Owner */
	   dw 128			;/* Name */
	   dw 172			;/* Locks */
	   dw 196			;/* Flag */
	   dw 206			;/* Attribute */

mempos3 label byte		; Module Status 1
	   dw 10
	   dw 26			;  /* segs */
	   dw 44
	   dw 76			; /* cs:ip */
	   dw 110
	   dw 144			; /* ss:sp */
	   dw 176
	   dw 202			; /* counts */
	   dw 218
	   dw 246			; /* auto */
	   dw 268

mempos4 label byte		;Module Listbox 2
	   dw 4
	   dw 16
	   dw 40
	   dw 64
	   dw 86

mempos5 label byte	   ;Module Listbox 1
	   dw 3
	   dw 24;
	   dw 68;
	   dw 86;

mempos6 label byte	   ;Module Status 2
	   dw 4
	   dw 26 ;		   /* beginn segm attr */
	   dw 166;		   /* ende	 segm attr */
	   dw 170;		   /* beginn selector attr */
	   dw 268;		   /* ende	 selector attr */

mempos7 label byte		 ;listbox resourcen 1
	   dw 6
	   dw 64;
	   dw 160;
	   dw 188;
	   dw 204;
	   dw 228;
	   dw 252;

mempos8 label byte		 ;status resourcen 1
	   dw 6
	   dw 38;
	   dw 60;
	   dw 100;
	   dw 120;
	   dw 160;
	   dw 210;

mempos9 label byte		 ;status resourcen 2
	   dw 6
	   dw 40;
	   dw 76;
	   dw 96;
	   dw 204;
	   dw 208;
	   dw 296;

mempos10 label byte 	  ;listbox importe
	   dw 3
	   dw 40;
	   dw 52;
	   dw 64;

mempos11 label byte 	  ;status importe
	   dw 8
	   dw 40;
	   dw 60;
	   dw 110;
	   dw 150;
	   dw 196;
	   dw 212;
	   dw 254;
	   dw 274;

mempos12 label byte 	  ;listbox exporte
	   dw 3
	   dw 40;
	   dw 80;
	   dw 90;

mempos13 label byte 	  ;status exporte
	   dw 8
	   dw 30;
	   dw 50;
	   dw 100;
	   dw 140;
	   dw 180;
	   dw 200;
	   dw 240;
	   dw 260;

mempos14 label byte 	  ;listbox "importiert von"
	   dw 1
	   dw 48;

mempos15 label byte 	  ;listbox klassen
	   dw 4
	   dw 24;
	   dw 68;
	   dw 92;
	   dw 132;

mempos16 label byte 	  ;status windows 1
	   dw 6
	   dw 34;
	   dw 58;
	   dw 100;
	   dw 160;
	   dw 180;
	   dw 210;

mempos17 label byte 	  ;status windows 2
	   dw 6
	   dw 46;
	   dw 88;
	   dw 128;
	   dw 160;
	   dw 180;
	   dw 210;
mempos18 label byte 	  ;listbox user windows
	   dw 6
	   dw 24;
	   dw 48;
	   dw 72;
	   dw 114;
	   dw 154;
	   dw 202;
mempos19 label byte 	  ;listbox mcbs
	   dw  5
	   dw  20;
	   dw  60;
	   dw  84;
	   dw  108;
	   dw  150;
mempos20 label byte 	  ;status local
	   dw  6
	   dw  38;
	   dw  62;
	   dw  100;
	   dw  130;
	   dw  180;
	   dw  220;
mempos21 label byte 	  ;listbox local
	   dw  6
	   dw 28;
	   dw 56;
	   dw 74 OR 08000h;
	   dw 80;
	   dw 104;
	   dw 128;
mempos22 label byte 	  ;listbox dos devices
	   dw 5
	   dw 20;
	   dw 74;
	   dw 114;
	   dw 154;
	   dw 164;
mempos23 label byte 	  ;status mcbs
	   dw 2
	   dw 90;
	   dw 120;
mempos24 label byte 	  ;listbox dos CDS
	   dw 4
	   dw 20;
	   dw 64;
	   dw 84;
	   dw 124;
mempos25 label byte 	  ;status dos CDS
	   dw 2
	   dw 30;
	   dw 124;
mempos26 label byte 	  ;listbox dos DPB
	   dw 8
	   dw 20;
	   dw 64;
	   dw 104;
	   dw 134;
	   dw 160;
	   dw 184;
	   dw 212;
	   dw 252;
mempos27 label byte 	  ;status dos DPB
mempos28 label byte 	  ;listbox DPMI LDT
	   dw 4
	   dw 20;
	   dw 60;
	   dw 100;
	   dw 120;
mempos29 label byte 	  ;status1 DPMI
	   dw 6
	   dw 60;
	   dw 100;
	   dw 130;
	   dw 160;
	   dw 210;
	   dw 250;
mempos30 label byte 	  ;listbox system GDT
	   dw 4
	   dw 24;
	   dw 64;
	   dw 104;
	   dw 124;
mempos31 label byte 	  ;status System GDT
	   dw 8
	   dw 48;
	   dw 88;
	   dw 136;
	   dw 156;
	   dw 184;
	   dw 204;
	   dw 224;
	   dw 244;
mempos32 label byte 	  ;listbox intr
	   dw 3
	   dw 16;
	   dw 56;
	   dw 104;
mempos33 label byte 	  ;status intr
	   dw 0
mempos34 label byte 	  ;listbox TSS
	   dw 7
	   dw 20;
	   dw 40;
	   dw 60;
	   dw 80;
	   dw 100;
	   dw 120;
	   dw 140;
mempos35 label byte 	  ;status TSS
	   dw 4
	   dw 30;
	   dw 50;
	   dw 80;
	   dw 100;
mempos36 label byte 	  ;status wiview
	   dw 10
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
mempos37 label byte 	  ;listbox IDT
	   dw 3
	   dw 16;
	   dw 74;
	   dw 98;
mempos38 label byte 	  ;status IDT
	   dw 4
	   dw 50;
	   dw 90;
	   dw 140;
	   dw 160;
mempos39 label byte 	  ;status2 TSS
	   dw 2
	   dw 60;
	   dw 110;
mempos40 label byte 	  ;listbox XMS
	   dw  4
	   dw  30;
	   dw  80;
	   dw  114;
	   dw  134;
mempos41 label byte 	  ;status1 XMS
	   dw  4
	   dw  40;
	   dw  80;
	   dw  120;
	   dw  160;
mempos42 label byte 	  ;status2 XMS
	   dw  4
	   dw  40;
	   dw  80;
	   dw  140;
	   dw  180;
mempos43 label byte 	  ;status DisAssembler
	   dw  2
	   dw  50;
	   dw  78;
mempos44 label byte 	  ;listbox DOS
	   dw  6
	   dw  20;			  /* psp */
	   dw  44;			  /* task */
	   dw  84;			  /* name */
	   dw  146; 		  /* usage */
	   dw  162; 		  /* handle */
	   dw  176; 		  /* mode */
mempos45 label byte 	  ;status DOS
	   dw  4
	   dw  40;
	   dw  60;
	   dw  110;
	   dw  130;
mempos46 label byte 	  ;childwindows wiwindow
	   dw 4
	   dw 22;
	   dw 44;
	   dw 80;
	   dw 116;
mempos47 label byte 	  ;status Paging
	   dw 4
	   dw 32;
	   dw 72;
	   dw 120;
	   dw 160;
mempos48 label byte 	  ;status DPMI enhanced
	   dw 2
	   dw 8;
	   dw -8;
mempos49 label byte 	  ;listbox DPMI enhanced
	   dw 7
	   dw 36;
	   dw 54;
	   dw 90;
	   dw 126;
	   dw 162;
	   dw 198;
	   dw 234;
mempos50 label byte 	  ;status 2 DPMI
	   dw 2
	   dw 8;
	   dw -8;
mempos51 label byte 	  ;listbox gdi devices
	   dw 2
	   dw 120;
	   dw 164;
mempos52 label byte 	  ;status atome
	   dw 4
	   dw 40
	   dw 60
	   dw 100
	   dw 120
mempos53 label byte 	  ;listbox atome
	   dw 4
	   dw 20;
	   dw 48;
	   dw 80
	   dw 110;
mempos54 label byte 	  ;status objects
	   dw 4
	   dw 30;
	   dw 50;
	   dw 80
	   dw 100;
mempos55 label byte 	  ;listbox objects
	   dw 2
	   dw 64;
	   dw 94;
mempos56 label byte 	  ;status all windows
	   dw 6
	   dw 40
	   dw 80
	   dw 120
	   dw 160
	   dw 190
	   dw 210
mempos57 label byte 	  ;listbox entries
	   dw 4
	   dw 24;
	   dw 48;
	   dw 80;
	   dw 104;
mempos58 label byte 	 ; Module Status 0
	   dw 4
	   dw 30
	   dw 46			;  /* anzahl */
	   dw 96
	   dw 130			; /* sortiert nach */
mempos59 label byte 	 ;listbox page table
	   dw 3
	   dw 46
	   dw 80
	   dw 120
mempos60 label byte 	 ;listbox page directory
	   dw 4
	   dw 80
	   dw 110
	   dw 150
	   dw 180
mempos61 label byte 	  ;listbox PM Int Vecs
	   dw 1
	   dw 32;
mempos62 label byte 	  ;status PM Int Vecs
	   dw 2
	   dw 50;
	   dw 90;
memende label byte

_data	ends

		end
