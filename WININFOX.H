
#define LoadTabs(x,y) LoadUserBytes(hInst,MAKEINTRESOURCE(IDU_TABPOS),MAKEINTRESOURCE(RTI_USER),x,y,sizeof(y))

/* resourcen identifikatoren */
                                      /* icons */
#define ICON1       1
                                      /* accelerators */
#define ACCELTAB    10
                                      /* about string */
#define IDU_ABOUT3  7
                                      /* small byte sets */
#define IDU_TABPOS  2
                                      /* beginnen mit 0 */
                                      /* wimem */
#define IDUS_0      0
#define IDUS_1      1
#define IDUS_2      2
                                      /* wimodul */
#define IDUS_3      3
#define IDUS_4      4
#define IDUS_5      5
#define IDUS_6      6
                                      /* wires */
#define IDUS_7      7
#define IDUS_8      8
#define IDUS_9      9
                                      /* wiimport */
#define IDUS_10    10
#define IDUS_11    11
                                      /* wiexport */
#define IDUS_12    12
#define IDUS_13    13
#define IDUS_14    14
                                      /* wiclass */
#define IDUS_15    15
                                      /* wiwindow */
#define IDUS_16    16
#define IDUS_17    17
                                      /* wiuser */
#define IDUS_18    18
                                      /* wimcb */
#define IDUS_19    19
#define IDUS_23    23
                                      /* wilocal */
#define IDUS_20    20
#define IDUS_21    21
                                      /* widev */
#define IDUS_22    22
                                      /* wicds */
#define IDUS_24    24
#define IDUS_25    25
                                      /* widpb */
#define IDUS_26    26
#define IDUS_27    27
                                      /* widpmi */
#define IDUS_28    28
#define IDUS_29    29
                                      /* wisys */
#define IDUS_30    30
#define IDUS_31    31
                                      /* wiintr */
#define IDUS_32    32
#define IDUS_33    33
                                      /* wigdt */
#define IDUS_34    34
#define IDUS_35    35
                                      /* wiview */
#define IDUS_36    36
                                      /* wiidt */
#define IDUS_37    37
#define IDUS_38    38
                                      /* status2 system */
#define IDUS_39    39
                                      /* listbox XMS */
#define IDUS_40    40
                                      /* status1 XMS */
#define IDUS_41    41
                                      /* status2 XMS */
#define IDUS_42    42
                                      /* status DisAssembler */
#define IDUS_43    43
                                      /* listbox DOS */
#define IDUS_44    44
                                      /* status DOS */
#define IDUS_45    45
                                      /* wiwindow childwindows */
#define IDUS_46    46
                                      /* status Paging */
#define IDUS_47    47
                                      /* status DPMI VM */
#define IDUS_48    48
                                      /* listbox DPMI VM */
#define IDUS_49    49
                                      /* status 2 DPMI */
#define IDUS_50    50
                                      /* listbox GDI */
#define IDUS_51    51
                                      /* status atome */
#define IDUS_52    52
                                      /* listbox atome */
#define IDUS_53    53
                                      /* status objects */
#define IDUS_54    54
                                      /* listbox objects */
#define IDUS_55    55
                                      /* status all windows */
#define IDUS_56    56
                                      /* listbox entries */
#define IDUS_57    57
                                      /* status module all */
#define IDUS_58    58
                                      /* listbox page table */
#define IDUS_59    59
                                      /* listbox page dir */
#define IDUS_60    60
                                      /* listbox pm int vecs */
#define IDUS_61    61
                                      /* status pm int vecs */
#define IDUS_62    62


                                   /* dlgtemplates */
#define IDD_MENU      100
#define IDD_frei2     101
#define IDD_TASK      102
#define IDD_MODUL     103
#define IDD_WINSTATUS 104
#define IDD_CLASS     105
#define IDD_DPMIINT   106
#define IDD_GLOBAL    107
#define IDD_ATOMBOX   108
#define IDD_DOS       109
#define IDD_SHOWRES   110
#define IDD_WINDOW    111
#define IDD_IMPORT    112
#define IDD_SELECTOR  113
#define IDD_EXPORT    114
#define IDD_LOCAL     115
#define IDD_RESOURCE  116
#define IDD_VIEW      117
#define IDD_ADDRESS   118
#define IDD_TEST      119
#define IDD_MEMOPT    120
#define IDD_PSP       121
#define IDD_DPMIMON   122
#define IDD_frei4     123
#define IDD_MCB       124
#define IDD_DOSDEV    125
#define IDD_DPMIVXD   126
#define IDD_USER      127
#define IDD_XMODULE   128
#define IDD_DLGTMP    129
#define IDD_frei5     130
#define IDD_VIEWCLS   131
#define IDD_ALLWIN    132
#define IDD_SYSPARMS  133
#define IDD_ENVIRON   134
#define IDD_EMS       135
#define IDD_UNASSEM   136
#define IDD_UNASSPARMS 137
#define IDD_ENTRY     138
#define IDD_GDI       139
#define IDD_DPMI      140
#define IDD_IMPORTEDFROM 141
#define IDD_REGISTER  142
#define IDD_CDS       143
#define IDD_DPB       144
#define IDD_SYSTEM    145
#define IDD_TSS       146
#define IDD_SYSIDT    147
#define IDD_LDT       148
#define IDD_XMS       149
#define IDD_UNASSEM32 150
#define IDD_HEXPARMS  151
#define IDD_SYSPAGE   152
#define IDD_PAGETAB   153
#define IDD_DPMIVM    154
#define IDD_XTASK     155
#define IDD_STACKTRACE 156
#define IDD_DEVCAP    157
#define IDD_OBJECTS   158
#define IDD_SEGMENT   159
#define IDD_SEGMFLAGS 160
#define IDD_PMIV      161
#define IDD_ABOUT     162
                                      /* dialoguebergreifende IDs */
#define ID_WARNING1  41
#define ID_TEST      42
#define ID_SNAPSHOT  43
#define ID_WARNING2  44
#define ID_POPUPMENU 45
#define ID_DESTROY   46
                                      /* allgemeine dialog IDs */
#define ID_LISTBOX1  51
#define ID_REFRESH   52
#define ID_SUBDLG1   53
#define ID_STATUS1   54
#define ID_SUBDLG2   55
#define ID_STATUS2   56
#define ID_SUBDLG3   57
#define ID_SUBDLG4   58
#define ID_SUBDLG5   59
#define ID_STATUS3   60
#define ID_RBTN1     61
#define ID_SUBDLG6   61
#define ID_RBTN2     62
#define ID_SUBDLG7   62
#define ID_RBTN3     63
#define ID_SUBDLG8   63
#define ID_RBTN4     64
#define ID_SUBDLG9   64
#define ID_RBTN5     65
#define ID_SUBDLG10  65
#define ID_SUBDLG11  66
#define ID_RBTNA     71
#define ID_RBTNB     72
#define ID_RBTNC     73
#define ID_RBTND     74
#define ID_RBTNE     75
#define ID_CONTROL1  51
#define ID_CONTROL2  52
#define ID_CONTROL3  53
                                      /* IDs Entry imported from */
                                      /* IDs entrys details modul */
                                      /* IDs unassemble parameter dialog */
#define ID_EDITUNASSLINES 51
#define ID_EDITUNASSADDR  52
#define ID_EDITUNASSBTN1  53
#define ID_EDITUNASSSTR   54
#define ID_EDITUNASSFIND  55
#define ID_EDITUNASSREFIND 56
                                      /* IDs User */
#define ID_WINDOWS    51
#define ID_SYSPARMLB  52
#define ID_WINMON     54
#define ID_CLASS      55
#define ID_USERLOCAL  56
#define ID_USERGLOBAL 57
                                      /* IDs GDI */
#define ID_GDILOCAL  53
#define ID_GDIDEV    54
#define ID_GDIGLOBAL 55
                                      /* IDs DPMI Main */
#define ID_DPMIMON     62
#define ID_DPMIINT     64
#define ID_DPMIANZ     66
#define ID_DPMIVM      67
#define ID_DPMIVXD     69
                                      /* IDs VM */
#define ID_VMSWITCH   63
#define ID_VMDESTROY  64
                                      /* IDs DPMI interrupts */
#define ID_STATINTR   53
#define ID_UNASSINTR  54
#define ID_LISTEXC    57
#define ID_LISTRMINT  58
#define ID_GETEXC01   59
                                      /* IDs System Main */
#define ID_SYSVIEW     61
#define ID_SYSTSS      62
#define ID_SYSLDT      63
#define ID_SYSIDT      66
#define ID_SYSPAGE     68
                                      /* IDs DPMI monitor (ID_DPMISTAT1 braucht platz hinter sich!) */
#define ID_DPMISTAT1   51
#define ID_DPMIUEB1    70
                                      /* IDs details klassen */
#define ID_VIEWCLSLB   51
                                      /* IDs aboutbox */
#define ID_CANCEL1     51
#define ID_ABOUTTEXT   52
#define ID_REGISTER    53
                                      /* IDs show resource, dummy dialogbox */
#define ID_XDLG        51
                                      /* IDs menubox */
#define ID_EXIT        51
#define ID_HELP        52
#define ID_NEWSELWIN   53
#define ID_READIDS     54
#define ID_CHECKREGISTER 55
#define ID_WRITEIDS    56
#define ID_OWNERMENU   100
                                      /* IDs menu - muessen so hintereinander kommen */
#define ID_TASK      71
#define ID_MODUL     72
#define ID_GLOBAL    73
#define ID_USER      74
#define ID_GDI       75
#define ID_DOS       76
#define ID_DPMI      77
#define ID_SYSTEM    78
#define ID_ABOUT     79
                                      /* IDs details resourcen (historisch DLG) */
#define ID_DLGINFO   51
#define ID_DLGTMPS1  52
#define ID_DLGSTAT1  53
#define ID_DLGSTAT2  54
                                      /* IDs testdialog */
#define ID_FILLBOX   51
#define ID_TESTLB    52
                                      /* IDs memory filter */
#define ID_MEMFIL1   81
#define ID_MEMFIL2   82
#define ID_MEMFIL3   83
#define ID_MEMFIL4   84
#define ID_MEMFIL5   85
#define ID_MEMFIL6   86
#define ID_MEMFIL7   87
#define ID_MEMVON    88
#define ID_MEMBIS    89
#define ID_MEMFIL8   90
#define ID_SORTDIR   91
#define ID_MEMFIL9   92
#define ID_MEMFIL0   93
                                      /* IDs tasks */
#define ID_LISTWINDOWS  53
#define ID_DETAILTASK   54
#define ID_VIEWTASK     55
#define ID_LOCALTASK    56
#define ID_PSPTASK      57
#define ID_QUEUTASK     58
#define ID_SWTTASK      59
#define ID_TRMTASK      60
#define ID_STATTASK     61
#define ID_AUTOTASK     62
#define ID_KILLTASK     63
#define ID_STACKTASK    64
#define ID_GLOBALTASK   65
#define ID_MODULTASK    66
#define ID_WINDTASK     67
                                      /* IDs module */
#define ID_VIEWMODUL    56
#define ID_EXPORTS      59
#define ID_LISTSEGM     60
#define ID_LOCALMODUL   62
#define ID_STATALL      61
#define ID_RESMODUL     63
#define ID_LOADMODUL    65
#define ID_FREEMODUL    66
#define ID_STATMODUL    67
#define ID_IMPORTS      68
#define ID_OPENMODUL    69
#define ID_XVIEWMODUL   70
#define ID_GLOBALMODUL  73
#define ID_STATSEGM     74
#define ID_SEGMMODUL    75
#define ID_SORTMODUL    76
                                      /* IDs windows status */
#define ID_STATS      51
#define ID_STATS1     52
#define ID_STATS2     53
#define ID_STATS3     54
#define ID_STATS4     55
#define ID_STATS5     56
#define ID_STATS6     57
#define ID_STATS7     58
#define ID_STATS8     59
#define ID_STATS9     60
#define ID_STATS10    61
#define ID_STATS11    62
                                      /* IDs klassen */
#define ID_LISTCLASS  51
#define ID_NEWCLASS   52
#define ID_VIEWCLASS  53
#define ID_STATCLASS  54
#define ID_UNASSCLASS 55
                                      /* IDs global heap */
#define ID_COMPMEM    63
#define ID_DISCMEM    65
#define ID_FILTMEM    66
#define ID_LOCKMEM    67
#define ID_UNLKMEM    68
#define ID_FREEMEM    69
#define ID_PLCKMEM    70
#define ID_PULKMEM    71
#define ID_FIXMEM     72
#define ID_UNFIXMEM   73
#define ID_SETRESETDISCARDABLEMEM 74
#define ID_DISCARDABLEWARNING 75
                                      /* IDs eingabe selektoren (memory) */
#define ID_EDITSEL    51
                                      /* IDs eingabe lineare addresse (memory) */
#define ID_EDITADDR   51
                                      /* IDs local heap (module + tasks) */
#define ID_VIEWLOCAL  54
#define ID_STATLOCAL  55
#define ID_COMPLOCAL  56
#define ID_SORTLOCAL  58
#define ID_ATOMLOCAL  59
                                      /* IDs DOS status */
#define ID_MCBDOS     55
#define ID_DEVDOS     57
#define ID_EMSDOS     61
#define ID_CDSDOS     63
#define ID_DPBDOS     65
#define ID_XMSDOS     67
#define ID_FLUSHDOS   69
                                      /* IDs Windows (Tasks) */
#define ID_WNDPARENT 51
#define ID_WNDSTYLE  52
#define ID_WNDCHILD  53
#define ID_WNDPROP   54
#define ID_WNDVIEW   55
#define ID_WNDSTAT1  56
#define ID_WNDUNASS  58
#define ID_WNDCHGSTYLE  59
#define ID_WNDEDITSTYLE 60
                                      /* IDs segment dialog */
#define ID_SEGFLG1   81
#define ID_SEGFLG2   82
#define ID_SEGFLG3   83
#define ID_SEGFLG4   84
#define ID_SEGFLG5   85
                                      // hex view dialog
#define ID_SELINVIEW  101
#define ID_ADDRINVIEW 102
                                      /* strings */
#define IDS_VIEW      48
#define IDS_TITLE     49
#define IDS_STATMEM1  50
#define IDS_INFOSTR   51
#define IDS_DWORDS    52
#define IDS_TEXT      53
#define IDS_RESOURCE  54
#define IDS_DISASM    55
#define IDS_LOCSTR1   57
#define IDS_DISASSSTR 58
#define IDS_IMPFROM   59
#define IDS_LDT       60
#define IDS_TSS       61
#define IDS_BYTES     62
#define IDS_WORDS     63

#define IDS_ERRMOD1  64
#define IDS_ERRTSK1  65
#define IDS_ERRMOD2  66
#define IDS_ERRMOD3  67
#define IDS_ERRMOD4  68
#define IDS_ERRMOD5  69
#define IDS_ERRMOD6  70
#define IDS_ERRMOD7  71
#define IDS_ERRMOD8  72
#define IDS_ERRMOD9  73
#define IDS_ERRMOD10 74
#define IDS_ERRVIE1  75
#define IDS_ERRVIE2  76
#define IDS_ERRVIE3  77
#define IDS_ERRVIE4  78
#define IDS_ERRVIE5  79

#define IDS_ERRVIE6  80
#define IDS_ERRMEM1  81
#define IDS_ERRMEM2  82
#define IDS_ERRMEM3  83
#define IDS_ERRXMS2  84
#define IDS_ERRMEM5  85
#define IDS_ERRRES1  86
#define IDS_ERRRES2  87
#define IDS_ERRRES3  88
#define IDS_ERRRES4  89
#define IDS_ERRCLS1  90
#define IDS_ERRSHW1  91
#define IDS_ERRSHW2  92
#define IDS_ERRSHW3  93
#define IDS_ERRSHW4  94
#define IDS_ERRSHW5  95

#define IDS_ERRSHW6  96
#define IDS_ERRDLG1  97
#define IDS_ERRDLG2  98
#define IDS_ERRDLG3  99
#define IDS_ERRTSK2 100
#define IDS_ERRIMP1 101
#define IDS_ERRIMP2 102
#define IDS_ERRIMP3 103
#define IDS_ERRIMP4 104
#define IDS_ERRIMP5 105
#define IDS_ERRMNU1 106
#define IDS_ERREXP1 107
#define IDS_ERREXP2 108
#define IDS_ERREXP3 109
#define IDS_ERRLOC1 110
#define IDS_ERRTSK3 111

#define IDS_ERREMS1 112
#define IDS_ERREXP4 113
#define IDS_ERRVIE7 114
#define IDS_ERRVIE8 115
#define IDS_ERRVIE9 116
#define IDS_ERRSHW7 117
#define IDS_ERRXMS1 118
#define IDS_ERRUNA1 119
#define IDS_ERRVIEA 120
#define IDS_ERRVIEB 121
#define IDS_ERRTSK4 122
#define IDS_ERRWND1 123
#define IDS_ERRMEM6 124
#define IDS_ERRMEM7 125
#define IDS_ERREMS2 126
#define IDS_ERREMS3 127

#define IDS_XMODUL00 160
#define IDS_XMODUL01 161
#define IDS_XMODUL02 162
#define IDS_XMODUL03 163
#define IDS_XMODUL04 164
#define IDS_XMODUL05 165
#define IDS_XMODUL06 166
#define IDS_XMODUL07 167
#define IDS_XMODUL08 168
#define IDS_XMODUL09 169
#define IDS_XMODUL10 170
#define IDS_XMODUL11 171
#define IDS_XMODUL12 172
#define IDS_XMODUL13 173
#define IDS_XMODUL14 174
#define IDS_XMODUL15 175

#define IDS_XMODUL16 176
#define IDS_XMODUL17 177
#define IDS_XMODUL18 178
#define IDS_XMODUL19 179
#define IDS_XMODUL20 180
#define IDS_XMODUL21 181
#define IDS_XMODUL22 182
#define IDS_XMODUL23 183

#define IDS_DPMIVER  192
#define IDS_XVER1    193
#define IDS_XVER2    194
#define IDS_XVER3    195
#define IDS_XVER4    196
#define IDS_XVER5    197
#define IDS_XVER6    198
#define IDS_XVER7    199
#define IDS_XVER8    200
#define IDS_XVER9    201
#define IDS_XVERA    202
#define IDS_XSTRB    203

#define IDS_XCLASS00 240
#define IDS_XCLASS01 241
#define IDS_XCLASS02 242
#define IDS_XCLASS03 243
#define IDS_XCLASS04 244
#define IDS_XCLASS05 245
#define IDS_XCLASS06 246
#define IDS_XCLASS07 247
#define IDS_XCLASS08 248

#define IDS_XDPMI00  256
#define IDS_XDPMI01  257
#define IDS_XDPMI02  258
#define IDS_XDPMI03  259
#define IDS_XDPMI04  260
#define IDS_XDPMI05  261
#define IDS_XDPMI06  262
#define IDS_XDPMI07  263
#define IDS_XDPMI08  264
#define IDS_XDPMI09  265
#define IDS_XDPMI0A  266

#define IDS_XWIN00   272
#define IDS_XWIN01   273
#define IDS_XWIN02   274
#define IDS_XWIN03   275
#define IDS_XWIN04   276
#define IDS_XWIN05   277
#define IDS_XWIN06   278
#define IDS_XWIN07   279
#define IDS_XWIN08   280
#define IDS_XWIN09   281
#define IDS_XWIN10   282

#define IDS_SPSTR1    304
#define IDS_SPSTR2    305
#define IDS_SPSTR3    306
#define IDS_SPSTR4    307
#define IDS_SPSTR5    308
#define IDS_SPSTR6    309
#define IDS_SPSTR7    310
#define IDS_SPSTR8    311
#define IDS_SPSTR9    312
#define IDS_SPSTR10   313
#define IDS_SPSTR11   314
#define IDS_SPSTR12   315
#define IDS_SPSTR13   316
#define IDS_SPSTR14   317
#define IDS_SPSTR15   318
#define IDS_SPSTR16   319
#define IDS_SPSTR17   320
#define IDS_SPSTR18   321
#define IDS_SPSTR19   322
#define IDS_SPSTR20   323
#define IDS_SPSTR21   324
#define IDS_SPSTR22   325
#define IDS_SPSTR23   326
#define IDS_SPSTR24   327
#define IDS_SPSTR25   328
#define IDS_SPSTR26   329
#define IDS_SPSTR27   330
#define IDS_SPSTR28   331
#define IDS_SPSTR29   332
#define IDS_SPSTR30   333
#define IDS_SPSTR31   334
#define IDS_SPSTR32   335

#define IDS_MODLDS1   360
#define IDS_MODLDS2   361
#define IDS_RESVIEW1  362
#define IDS_RESVIEW2  363
#define IDS_RESVIEW3  364
#define IDS_RESVIEW4  365

#define IDS_DRIVERVERSION 400
#define IDS_TECHNOLOGY    401
#define IDS_HORZSIZE      402
#define IDS_VERTSIZE      403
#define IDS_HORZRES       404
#define IDS_VERTRES       405
#define IDS_BITSPIXEL     406
#define IDS_PLANES        407
#define IDS_NUMBRUSHES    408
#define IDS_NUMPENS       409
#define IDS_NUMMARKERS    410
#define IDS_NUMFONTS      411
#define IDS_NUMCOLORS     412
#define IDS_PDEVICESIZE   413
#define IDS_CURVECAPS     414
#define IDS_LINECAPS      415
#define IDS_POLYGONALCAPS 416
#define IDS_TEXTCAPS      417
#define IDS_CLIPCAPS      418
#define IDS_RASTERCAPS    419
#define IDS_ASPECTX       420
#define IDS_ASPECTY       421
#define IDS_ASPECTXY      422
#define IDS_LOGPIXELSX    423
#define IDS_LOGPIXELSY    424
#define IDS_SIZEPALETTE   425
#define IDS_NUMRESERVED   426
#define IDS_COLORRES      427

#define IDS_XMS1          440
#define IDS_XMS2          441
#define IDS_XMS3          442
#define IDS_XMS4          443

#define IDS_PAGELST1      450

#define IDS_XTASK00 460
#define IDS_XTASK01 461
#define IDS_XTASK02 462
#define IDS_XTASK03 463
#define IDS_XTASK04 464
#define IDS_XTASK05 465
#define IDS_XTASK06 466
#define IDS_XTASK07 467
#define IDS_XTASK08 468
#define IDS_XTASK09 469
#define IDS_XTASK0A 470
#define IDS_XTASK0B 471
#define IDS_XTASK0C 472
#define IDS_XTASK0D 473
#define IDS_XTASK0E 474
#define IDS_XTASK0F 475
#define IDS_XTASK10 476
#define IDS_XTASK11 477

#define IDS_XPSP00 500
#define IDS_XPSP01 501
#define IDS_XPSP02 502
#define IDS_XPSP03 503
#define IDS_XPSP04 504
#define IDS_XPSP05 505
#define IDS_XPSP06 506
#define IDS_XPSP07 507
#define IDS_XPSP08 508

