////////////////////////////////////////////////////////
// Konstanten
////////////////////////////////////////////////////////

// neuere window styles

#ifndef WS_EX_CLIENTEDGE
#define WS_EX_CLIENTEDGE   0x0200
#endif

// undokumentierte messages
                                          // anschliessend kann popup menu ausgegeben werden
#define WM_SIZEWAIT        0x0004         // nicht mehr ab 3.0
#define WM_SETVISIBLE      0x0009         // nicht mehr ab 3.1
#define WM_SYSTEMERROR     0x0017         // versendet bei Speichermangel
#define WM_PAINTICON       0x0026         // wenn class icon existiert
#define WM_ALTTABACTIVE    0x0029
#define WM_SETHOTKEY       0x0032
#define WM_GETHOTKEY       0x0033
#define WM_FILESYSCHANGE   0x0034
#define WM_ISACTIVEICON    0x0035
#define WM_QUERYPARKICON   0x0036
#define WM_OTHERWINDOWCREATED   0x0042
#define WM_OTHERWINDOWDESTROYED 0x0043
#define WM_ACTIVATESHELLWINDOW  0x0044
// #define WM_WINDOWPOSCHANGING 0x0046
// #define WM_WINDOWPOSCHANGED  0x0047
#define WM_SYSTIMER        0x0118
#define WM_LBTRACKPOINT    0x0131
#define WM_ENTERMENULOOP   0x0211
                                          // wird nach dem verschwinden eines (popup) menus verschickt
#define WM_EXITMENULOOP    0x0212

#define WM_ENTERSIZEMOVE   0x0231
#define WM_EXITSIZEMOVE    0x0232
                                          // wParam = Pos, lParam = paintflag
#define SB_SETPOSITION  (WM_USER+0)
#define SB_GETPOSITION  (WM_USER+1)
                                          // lParam = min,max
#define SB_SETRANGE     (WM_USER+2)
#define SB_GETRANGE     (WM_USER+3)
                                          // rechten unteren punkt eines windows holen
#define GWL_FP2   -36
                                          // data dictionary types
#define DDT_BYTE   0
#define DDT_WORD   1
#define DDT_DWORD  2
#define DDT_STRING 5
#define DDT_KBITS  7
#define DDT_KBYTE  8
#define DDT_KWORD  9
#define DDT_FUNCTION 10
#define DDT_USER   15
                                          // resource typen
//#define RTI_CURSOR           1
//#define RTI_BITMAP           2
//#define RTI_ICON             3
//#define RTI_MENU             4
//#define RTI_DIALOG           5
//#define RTI_STRING           6
//#define RTI_FONTDIR          7
//#define RTI_FONT             8
//#define RTI_ACCELERATOR      9
//#define RTI_RCDATA           10
#define RTI_USER             0x100
                                          /* resource attributes */
#define RA_MOVEABLE    0x0010
#define RA_PRELOAD     0x0040
#define RA_DISCARDABLE 0x1000
                                          /* allgemeine string ids */
#define IDS_OK      1
#define IDS_CANCEL  2
#define IDS_ERROR   3
#define IDS_WARNING 4
#define IDS_HINT    5
#define IDS_YES     6
#define IDS_NO      7
                                          /* control ids der User-Messagebox */
// #define CMSGBOX  900
// #define ID_CMSGTEXT 11
// #define IDOK1       12

#define BS_PUSHBOX  0x0AL
#ifndef SS_USERITEM
#define SS_USERITEM 0x0AL
#endif
                                          /* spezielle global heap owner */
#define GO_SENTINEL     (-1)
#define GO_BURGERMASTER (-3)
#define GO_NOT_THERE    (-4)
#define GO_PHANTOM      (-5)
#define GO_WRAITH       (-6)
#define GO_BOGUS        (-7)
#define GO_FREE         (0)

////////////////////////////////////////////////////////
// Macros
////////////////////////////////////////////////////////

#ifdef _DLL
  #define _dll_
#endif

#ifndef EXPORTED
#if WINVER >= 0x400
#define EXPORTED
#else
#define EXPORTED __export
#endif
#endif

#define DEBOUT(y)  {OutputDebugString(y);OutputDebugString("\r\n");}
#define DEBIN()    {_asm mov ax,0001 _asm int 41h}
#define GetModuleFromTask(x) (HMODULE)*(HMODULE FAR *)MAKELONG(0x1E,x)

#ifdef _TRACE_
 #define TRACEOUT(y) DEBOUT(y)
#else
 #define TRACEOUT(y)
#endif

#define USESTACKENTRY() _asm mov ax,ss _asm push ds _asm mov ds,ax
#define USESTACKEXIT()  _asm pop ds

#ifndef _dll_
  #define CALLBACKENTRY() _asm push ds _asm mov ax,ss _asm mov ds,ax
  #define CALLBACKEXIT() _asm pop ds
  #define LOADDS
#else
  #define CALLBACKENTRY()
  #define CALLBACKEXIT()
  #define LOADDS _loadds
#endif
#define WNDCALLBACKENTRY() _asm push ds _asm mov ds,ax
#define WNDCALLBACKEXIT() _asm pop ds
// #define EXPENTRY  FAR PASCAL _loadds
#ifndef GetWindowStyle
#define GetWindowStyle(x) GetWindowLong(x,GWL_STYLE)
#endif
#ifndef SetWindowStyle
#define SetWindowStyle(x,y) SetWindowLong(x,GWL_STYLE,y)
#endif

////////////////////////////////////////////////////////
// Strukturen
////////////////////////////////////////////////////////

typedef unsigned int UINT;
                                       // file lesen (ReadFile())
typedef struct tagREADSTRUCT {
  int     nErrCode;
  int     nErrType;
  DWORD   dwSize;
  HGLOBAL hGlobal;
} READSTRUCT;
typedef READSTRUCT FAR * LPREADSTRUCT;

#ifdef TASKENTRY
typedef TASKENTRY FAR *LPTASKENTRY;
#endif

typedef struct tagEXDLGENTRY
  {
    HBRUSH      hbrush;
    HPALETTE    hpalette;
    HICON       hIcon;
  } EXDLGENTRY;

typedef EXDLGENTRY FAR *LPEXDLGENTRY;

typedef struct tagOPENFILEENTRY
  {
    HBRUSH      hbrush;
    HPALETTE    hpalette;
    HICON       hIcon;
    WORD        id;                /* ID ueber die messages ausgetauscht werden */
    HANDLE      hInst;
    LPSTR       fnstr;             /* filename */
    LPSTR       extstr;            /* extension string */
    LPSTR       dlgtmp;            /* dialog template */
    LPSTR       capstr;            /* dialog caption */
    LPSTR       fncap;             /* filename caption */
    int         fnsize;            /* maximale laenge des filenamen */
    int         extsize;           /* maximale laenge der extensions */
    HWND        startctl;          /* control, das openfiledialog startet */
    struct {
      unsigned nmodal     :1;
      unsigned minimizebox:1;
      unsigned openicon   :1;
      unsigned openexit   :1;
           } flags;

  } OPENFILEENTRY;

/* segmentflags */

typedef struct {
      unsigned data       :1;  //  0 0=CODE,1=DATA
      unsigned alloc      :1;  //  1 handle allocated?
      unsigned isloaded   :1;  //  2 memory allocated
      unsigned iterated   :1;  //  3 ???
      unsigned moveable   :1;  //  4 1=moveable,0=fixed
      unsigned shared     :1;  //  5
      unsigned preload    :1;  //  6 1=preload,0=loadoncall
      unsigned readonly   :1;  //  7 1=r/o bzw r/e
      unsigned relocs     :1;  //  8 reloc infos vorhanden
      unsigned conforming :1;  //  9
      unsigned reserved2  :1;  // 10 0=IOPL,1=NOIOPL
      unsigned reserved3  :1;  // 11
      unsigned discardable:1;  // 12 1=discardable
      unsigned mixed1632  :1;  // 13 1=32-Bit
      unsigned reserved5  :1;  // 14
      unsigned reserved6  :1;  // 15
           } SEGMFLAGS;

typedef struct tagSEGMDESC       /* segment descriptor im NE-segment */
  {
    WORD        addr;            /* addresse im file */
    WORD        filelen;         /* laenge im file */
    SEGMFLAGS   flags;
    WORD        memlen;          /* laenge im memory */
    HGLOBAL     handle;          /* dieser eintrag nur im memory, nicht im file */
  } SEGMDESC;

typedef struct tagMODSTRUC
  {
    WORD        id;            /* 00: sollte "NE" sein */
    WORD        count;         /* 02: anzahl verweise auf dieses modul */
    WORD        offsentries;   /* 04: offset auf exportierte funktionen */
    WORD        nextmod;       /* 06: naechster mod */
    WORD        pautodata;     /* 08: ^ autodata */
    WORD        pfileinfo;     /* 0a: ^ fileinfo */
    WORD        flags;         /* 0c: zustands flags */
    WORD        autods;        /* 0e: nr des automatic data segments */
    WORD        heapsize;      /* 10: start heap size */
    WORD        stacksize;     /* 12: start stack size */
    DWORD       initcsip;      /* 14: start CS:IP */
    DWORD       initsssp;      /* 18: start SS:SP */
    WORD        segs;          /* 1c: anzahl segmente */
    WORD        wNumRefModule; /* 1e: anzahl referenzierte Module */
    WORD        lennresname;   /* 20: laenge der tabelle nichtresidenter namen */
    WORD        offssegtab;    /* 22: offset segment tabelle */
    WORD        offsrctab;     /* 24: offset resourcen tabelle */
    WORD        offsresname;   /* 26: offset tabelle residenter namen */
    WORD        offsmodref;    /* 28: offset modulreferenz tabelle */
    WORD        offsimpname;   /* 2A: offset importierte namen */
    DWORD       addrnresname;  /* 2C: addresse nichtresidente namen */
    WORD        moventrys;     /* 30: movable entries */
    WORD        shiftfactor;   /* 32: schiebefaktor */
    WORD        reservedsegs;  /* 34: reservierte segmente */
    BYTE        os;            /* 36: betriebssystem */
    BYTE        res7;          /* 37: ??? */
    WORD        res8;          /* 38: ??? */
    WORD        res9;          /* 3A: ??? */
    WORD        swaparea;      /* 3C: swapareasize */
    WORD        version;       /* 3E: version */
  } MODSTRUC;

typedef MODSTRUC FAR *LPMODSTRUC;

typedef struct tagTASKSTRUC
  {
    WORD        wNextTask;     /* 00: */
    DWORD       dwSSSP;        /* 02: */
    WORD        wSemaphor;     /* 06: waitevent */
    WORD        wPriority;     /* 08: prioritaet */
    WORD        reserved1;     /* 0A */
    WORD        wSel;          /* 0C */
    WORD        reserved2;     /* 0E */
    WORD        reserved3;     /* 10 */
    WORD        reserved4;     /* 12 */
    WORD        w8087Cntl;     /* 14 */
    WORD        wFlags;        /* 16 */
    WORD        wError;        /* 18 */
    WORD        wVersion;      /* 1A */
    WORD        wInstance;     /* 1C */
    WORD        wModule;       /* 1E module handle */
    WORD        wQueue;        /* 20 */
    WORD        wParent;       /* 22 */
    WORD        wFlag1;        /* 24 */
    DWORD       dwSigProc;     /* 26 */
    DWORD       dwTaskSig;     /* 2A */
    DWORD       dwDiscardProc; /* 2E */
    DWORD       dwInt0vec;     /* 32: */
    DWORD       dwInt2vec;     /* 36: */
    DWORD       dwInt4vec;     /* 3A: */
    DWORD       dwInt6vec;     /* 3E: */
    DWORD       dwInt7vec;     /* 42: */
    DWORD       dwInt3Evec;    /* 46: */
    DWORD       dwInt75vec;    /* 4A: */
    WORD        wReserved[9];  /* 4E: */
    WORD        wPSP;          /* 60: */
    DWORD       dwDTA;         /* 62: */
    BYTE        bDrive;        /* 66: */
    BYTE        szPath[0x43];  /* 67: */
    WORD        wYieldtask;    /* AA: */
    WORD        wDLLSeg;       /* AC: */
    WORD        wDLLOffs;      /* AE: */
    WORD        wAlias;        /* B0: Codesegment Alias */
    WORD        wThunkNext;    /* B2: */
  } TASKSTRUC;

typedef TASKSTRUC FAR *LPTASKSTRUC;

typedef struct tagPROCSTRUC
  {
    LPVOID      fptr;
    WORD        flen;
    HGLOBAL     hMem;
  } PROCSTRUC;

typedef struct {
       unsigned flag0:1;
       unsigned flag1:1;
       unsigned flag2:1;
       unsigned flag3:1;
       unsigned moveable:1;               /* moveable / fixed */
       unsigned pure:1;                   /* pure / impure */
       unsigned preload:1;                /* preload / loadoncall */
       unsigned flag7:1;
       unsigned flag8:1;
       unsigned flag9:1;
       unsigned flagA:1;
       unsigned flagB:1;
       unsigned discardable:1;
       unsigned flagD:1;
       unsigned flagE:1;
       unsigned flagF:1;
           }   RESFLAGS;

typedef struct tagRESENTRY   /* resource entry */
  {
    WORD       typ;          // resource typ (string,dialog,menu,...)
    WORD       id;           // ID (Name/Nummer der Resource)
    HGLOBAL    handle;       // handle der resource
    WORD       count;        // ???
    union {
       RESFLAGS   flags;
       WORD       allflags;
          };
    WORD       offs;
    WORD       length;
    char       idname[20];
    char       typname[20];
    BYTE       typstart;
    BYTE       fName;
    DWORD      handler;
    WORD       anzahl;
    WORD       resakt;
    WORD       resend;
    HMODULE    hModule;
  } RESENTRY;

typedef struct tagKEYENTRY
   {
    PSTR   name;
    WORD   wert;
   } KEYENTRY;

typedef struct tagDDENTRY      /* data dictionary */
   {
    PSTR   name;               /* feldbezeichnung */
    WORD   addr;               /* addresse des Feldes */
    PCSTRC format;             /* formatierungsstring */
    WORD   typ;                /* typ des Feldes */
   } DDENTRY;
                              /* profile hilfsstruktur */
typedef struct tagPROFSTRUC
   {
    BYTE   flags;         /* 0=nur lesen | 1=lesen und zurueckschreiben | 2=bedingt zurueckschr. */
    BYTE   index;         /* bereich in .ini file */
    PSTR   name;
    PINT   wert;
   } PROFSTRUC;

typedef NPSTR NEAR* NPNPSTR;

typedef struct tagSTRLOADENTRY
   {
    NPNPSTR ppstr;        /* near pointer auf near stringpointer */
    WORD   id;            /* id der zu ladenden resource */
   } STRLOADENTRY, FAR * LPSTRLOADENTRY;

typedef struct tagATOMENTRY
   {
    PSTR   pNext;         /* zeiger auf n„chsten entry */
    WORD   count;         /* anzahl referenzen */
    BYTE   length;        /* string laenge */
    char   string[1];
   } ATOMENTRY;

typedef ATOMENTRY FAR *LPATOMENTRY;

// entry structuren (in NE-Header)
// ENTRYSTRUCT ist header fuer mehrere aufeinanderfolgende
// entries SINGLEENTRYSTRUCT

typedef struct tagENTRYSTRUCT
 {
  WORD firstid;     /* erste ordinalzahl - 1 */
  WORD lastid;      /* letzte ordinalzahl */
  WORD nextentry;   /* near ^ auf naechstes buendel */
 } ENTRYSTRUCT;

typedef struct tagSINGLEENTRYSTRUCT
 {
  BYTE type;        /* 0xFF bei beweglichen Segmenten */
  BYTE flags;       /* 1=exportiert,2=oeffentliche daten */
  BYTE segNum;      /* logische segmentnummer */
  WORD offset;      /* offset */
 } SINGLEENTRYSTRUCT,FAR * LPSINGLEENTRYSTRUCT;

// hilfsstruktur fuer GetFirstEntry/GetNextEntry

typedef struct tagENTRYENTRY
 {
  BYTE flags;
  WORD segNum;
  WORD offset;
  WORD id;
  LPSINGLEENTRYSTRUCT lpAdr; // adresse des entries in MD
  ENTRYSTRUCT FAR * lpentry;
 } ENTRYENTRY,FAR *LPENTRYENTRY;

// zum auflisten von importen eines moduls

#define IL_SORT 1         /* sortieren */
#define IL_ALL  2         /* alles zurueckgeben */

typedef struct tagIMPORTENTRY
   {
    HMODULE hModule;      /* Modul Handle des Imports */
    WORD    wEntry;       /* Entry des Imports */
    BYTE    bAddrType;    /* Adresstyp des Imports */
    WORD    bRelocType;   /* Relocationstyp des Imports */
    FARPROC fpAddr;       /* Addresse des Imports (z.Z nicht versorgt) */
                          /* interne steuervariable */
    WORD   wMode;         /* Modus: sortieren? - selektion */
    WORD   wSegment;      /* aktuelles segment */
    WORD   wLimit;        /* maximaler offset im globalen Speicherobjekt */
    HANDLE hGlobal;       /* speicherhandle */
    LPVOID lpvoid;        /* addresse des gelockten Objektes */
    HANDLE hFile;         /* filehandle */
    WORD   wError;        /* fehlercode */
   } IMPORTENTRY;

#define IMPERROR_OPEN    0x0001
#define IMPERROR_READ    0x0002
#define IMPERROR_ALLOC   0x0004
#define IMPERROR_REALLOC 0x0008

typedef struct tagEXPORTENTRY
   {
    WORD    wEntry;        /* Nr des Exports */
    WORD    wType;         /* typ des Exports */
    FARPROC fpAddr;        /* Addresse des Exports */
    char    szProcName[64];/* Name des Exports */
                           /* interne steuervariable */
    HANDLE hGlobal;        /* speicherhandle */
    LPVOID lpvoid;         /* pointer im gelockten Objekt */
    WORD   wLimit;         /* maximaler offset im globalen Speicherobjekt */
    LPVOID lpvoidres;      /* pointer im gelockten Objekt */
    WORD   wLimitres;      /* maximaler offset im globalen Speicherobjekt */
    WORD   wError;         /* fehlercode */
   } EXPORTENTRY;

#define EXPERROR_OPEN  0x0001
#define EXPERROR_READ  0x0002
#define EXPERROR_ALLOC 0x0004

typedef struct tagREFMODULEENTRY {
  DWORD  dwSize;
  HINSTANCE hModule;
  HINSTANCE hRefModule;
  char   szRefModule[20];
  WORD   wNext;
} REFMODULEENTRY;

typedef REFMODULEENTRY FAR * LPREFMODULEENTRY;

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////
// Funktionsprototypen
////////////////////////////////////////////////////////

// undokumentierte Funktionen

LONG    WINAPI GetHeapSpaces(HGLOBAL);        // Local Heap(HIGH=gesamt,LOW=frei)
WORD    WINAPI FillWindow(HWND,HWND,HDC,HBRUSH); // statt hbrush auch CTLCOLOR_
WORD    WINAPI SetSystemTimer(HWND, int, WORD, FARPROC);
WORD    WINAPI KillSystemTimer(HWND, int);
HMODULE WINAPI GetExePtr(HGLOBAL);                      // hModule ermitteln
int     WINAPI DeletePathName(LPSTR);                   // KERNEL
void    WINAPI FlushCachedFileHandle(HANDLE);
FARPROC WINAPI ToolHelpHook(FARPROC);                   // KERNEL
// WORD    WINAPI DoEnvironmentSubst(LPSTR,WORD);          // SHELLAPI
// void    WINAPI ShellAbout(HWND,LPSTR,WORD,WORD,WORD);   // SHELLAPI
WORD    WINAPI SelectorAccessRights(HGLOBAL,WORD,WORD); // KERNEL

// eigene Funktionen

BOOL     WINAPI ReadFile(LPSTR lpFileName, LPREADSTRUCT lprs);
BOOL     WINAPI ReadFileEx(LPSTR lpFileName, LPREADSTRUCT lprs);
int      WINAPI CreateMessage( HWND, LPCSTR , LPCSTR, WORD);                     /* aus creatmsg.c */
int      WINAPI CreateMessageParam( HWND, LPCSTR , LPCSTR, WORD, WORD); /* aus creatmsg.c */
BOOL     WINAPI CreateOpenFileDialog (HWND, OPENFILEENTRY FAR *);
WORD     WINAPI GetStdString(WORD, LPSTR ,WORD);   /* standardstring "ok","ja" usw. holen */
LPSTR    WINAPI GetStdStringAddress(WORD);

HDC      WINAPI GetPrinterDC(LPSTR,LPVOID,LPSTR);
WORD     WINAPI GetDefaultDevice(LPSTR);
HANDLE   WINAPI GetDefaultDevMode(LPSTR, LPSTR);
BOOL     WINAPI EnumDevice(WORD,LPSTR,WORD,LPSTR,LPSTR,WORD);
void            GetPrinterDeviceMode(HWND,char *);
DWORD    WINAPI GetWindowSize(HWND);
DWORD    WINAPI GetClientSize(HWND);
void     WINAPI GetClientPos(HWND,LPPOINT);             /* get client pos in window */
void     WINAPI AdjustClientSize(HWND,int,int,int);
BOOL     WINAPI EnableDlgItem(HWND,UINT,BOOL);
PSTR     WINAPI SearchKeyTable(PSTR,WORD);
int      WINAPI xsprintf(DDENTRY * ,PSTR,LPSTR);
HGLOBAL  WINAPI LoadUserResource(HINSTANCE,LPCSTR,LPCSTR,LPBYTE,WORD);
int      WINAPI LoadUserBytes(HINSTANCE,LPCSTR,LPCSTR,WORD,LPVOID,WORD);
int      WINAPI LoadStringTab(HANDLE,LPSTRLOADENTRY);
int             GetChildRect(HWND,LPRECT);
void     WINAPI GetChildWindowRect(HWND,LPRECT);
BOOL     WINAPI SaveWindowPos(HWND,LPINT,LPINT);

HINSTANCE WINAPI GetModuleDgroup(HMODULE);
// WORD     WINAPI GetModuleName(HMODULE, LPSTR, WORD); // in KRNL386 definiert!
#define GetModuleName GetModuleNameEx
WORD     WINAPI GetModuleNameEx(HMODULE, LPSTR, WORD); /* rc: laenge des namens */
HGLOBAL  WINAPI GetNResNames(HMODULE);
DWORD    WINAPI GetEntryAddress(HMODULE, WORD);
WORD     WINAPI GetEntryID(HMODULE, WORD, WORD);
WORD     WINAPI GetNextEntryAddress(HMODULE, WORD, WORD);
FARPROC  WINAPI GetPrevEntryAddress(HMODULE, FARPROC);
BOOL     WINAPI GetFirstEntry(HMODULE, LPENTRYENTRY);
BOOL     WINAPI GetNextEntry(HMODULE, LPENTRYENTRY);
HGLOBAL  WINAPI GetSegmentHandle(HMODULE, WORD);  /* segmentnr -> segmentselector */
WORD     WINAPI GetSegmentID(HMODULE, WORD);      /* segmentselector -> segmentnr */
WORD     WINAPI GetProcedureName(HINSTANCE, WORD, LPSTR, WORD);
WORD     WINAPI GetProcedureNameEx(HINSTANCE, WORD, LPSTR, WORD, HANDLE);
WORD     WINAPI GetImportedName(HMODULE, WORD, LPSTR, WORD);
BOOL     WINAPI GetProcName(HMODULE,WORD,LPSTR,WORD,PROCSTRUC FAR *);
DWORD    WINAPI GetSymbolAddr(LPSTR lpName,HMODULE hModule,HGLOBAL hMem);
WORD     WINAPI GetSymbolicNameEx(FARPROC dwProc,LPSTR lpStr);

BOOL     WINAPI ResourceFirst(HMODULE,RESENTRY FAR *);
BOOL     WINAPI ResourceNext(HMODULE,RESENTRY FAR *);
BOOL     WINAPI ResourceFindID(HMODULE,RESENTRY FAR *);
BOOL     WINAPI GetResourceName(HMODULE,WORD,LPSTR,WORD);    /* module, id, lpstr, size */
BOOL     WINAPI ResourceTypFirst(HANDLE,RESENTRY FAR *);     /* ??? */
BOOL     WINAPI ResourceTypNext(HANDLE,RESENTRY FAR *);      /* ??? */

BOOL     WINAPI ImportFirst(HMODULE, IMPORTENTRY FAR*, WORD);
BOOL     WINAPI ImportNext (HMODULE, IMPORTENTRY FAR*);
BOOL     WINAPI ImportClose(HMODULE, IMPORTENTRY FAR*);
BOOL     WINAPI ExportFirst(HMODULE, EXPORTENTRY FAR*);
BOOL     WINAPI ExportNext (HMODULE, EXPORTENTRY FAR*);
BOOL     WINAPI ExportClose(HMODULE, EXPORTENTRY FAR*);
BOOL     WINAPI RefModuleFirst(LPREFMODULEENTRY);
BOOL     WINAPI RefModuleNext(LPREFMODULEENTRY);



BOOL     WINAPI IsClass(HANDLE);
int      WINAPI GetOwnedPopups(HWND);
BOOL     WINAPI IsValidLocalHeap(HANDLE);
PVOID    WINAPI BasedLocalLock(HGLOBAL,HLOCAL);
BOOL     WINAPI BasedLocalUnlock(HGLOBAL,HLOCAL);
UINT     WINAPI BasedLocalCompact(HGLOBAL,UINT);
BOOL     WINAPI RegisterDosMon(WORD wIntnr,FARPROC,WORD wMode,WORD wStack);
BOOL     WINAPI UnregisterDosMon(WORD,WORD);
                                                // funktionen intmon.dll
BOOL     WINAPI RegisterIntMon(WORD wIntnr,FARPROC,WORD wMode,PBYTE wStack);
BOOL     WINAPI UnregisterIntMon(WORD,WORD);
DWORD    WINAPI AllocDosMemory(DWORD);
BOOL     WINAPI FreeDosMemory(HGLOBAL);

BOOL     WINAPI ReplaceItem(HWND,WORD,LPSTR,DWORD);
void     WINAPI IncBrightness(COLORREF FAR *,WORD);
LPSTR    WINAPI ReadCommandLine(LPSTR, LPSTR,int, LPSTR, int);
BOOL     WINAPI AddFileList(HWND,LPSTR,WORD,WORD);
int      WINAPI GetHelpPath(HANDLE,LPSTR,WORD);

LONG     WINAPI TabbedTextOutEx(HDC, int, int, LPCSTR, int, int, int FAR*, int);
                                              /* Parm5: modus   */
                                              /* Bit 0: close ausfuehren */
                                              /* Bit 1: ins CB kopieren */
HGLOBAL  WINAPI WriteLBSnapShot(int , HWND , WORD , LPCSTR , WORD);
HTASK    WINAPI GethTaskFromPSP(WORD);
BOOL     WINAPI PutFiletoClipboard(LPSTR,HWND);

#ifdef TASKENTRY
BOOL     WINAPI TaskFindInstance(TASKENTRY FAR*,HANDLE);      /* ergaenzung zu toolhelp */
#endif
HTASK     WINAPI GetTaskFromInstance(HINSTANCE);
HINSTANCE WINAPI GetInstanceFromTask(HTASK);
WORD      WINAPI GetTaskName(HTASK,LPSTR lpDest,WORD wSize);
HINSTANCE WINAPI TryCtl3dRegister(HINSTANCE hInst,HINSTANCE hLib);
void      WINAPI AdjustCBSize(HWND);
BOOL      WINAPI HandleDlgIcon(HWND,UINT,WPARAM,LPARAM,HICON);
HINSTANCE WINAPI GetTaskDS();       // in kernel.exe

WORD      WINAPI GetKernelPDB();     // return HIWORD(GetCurrentPDB())

#define TaskGetModule(HTASK) *(HMODULE FAR *)MAKELONG(offsetof(TASKSTRUC,wModule),HTASK)

#ifdef __cplusplus
}
#endif

