
#ifndef _INC_USER
#define _INC_USER

#ifdef _INC_WINDOWS
#define __WINDOWS_H
#endif

#define  _TAB_ 9
#define  _LF_ 10
#define  _FF_ 12
#define  _CR_ 13

#define IsModule(x) (*(LPINT)MAKELONG(0,x) == 0x454E)

typedef void                *PVOID;

/* allgemeine funktionen */

#define MulDivWords(x,y,z) (DWORD)x*(DWORD)y/z

#ifndef NO_EXT_KEYS
    #define _CDECL  cdecl
#else
    #define _CDECL
#endif

/* typdefinitionen */

typedef char __huge         *HPSTR;
typedef const char __huge   *HPCSTR;
typedef BYTE __huge         *HPBYTE;
typedef const BYTE __huge   *HPCBYTE;
typedef WORD __huge         *HPWORD;
typedef void __huge         *HPVOID;

#define CODESEG _based(_segname("_CODE"))
typedef const char _based(_segname("_CODE")) CSTRC,*PCSTRC;

typedef struct tagFWORD {
        DWORD offset;
        WORD selector;
       } FWORD;

typedef FWORD FAR * LPFWORD;

typedef unsigned short SELECTOR;

typedef struct tagDESCRIPTOR
   {
     WORD limit1;
     WORD base1;
     BYTE base2;
     BYTE attr;
     unsigned char limit:4,attrh:4;
     BYTE base3;
   } DESCRIPTOR;

typedef DESCRIPTOR FAR * LPDESCRIPTOR;

typedef struct tagIDTENTRY
   {
     WORD id;
     DWORD offset;
     WORD selector;
     WORD attr;
     WORD wLimit;
     DWORD dwAddr;
   } IDTENTRY;

typedef IDTENTRY FAR * LPIDTENTRY;

typedef struct tagGDTENTRY
   {
     WORD selector;
     DWORD base;
     DWORD limit;
     WORD attr;
   } GDTENTRY;

typedef GDTENTRY FAR * LPGDTENTRY;

typedef struct tagIOBMENTRY
   {
     WORD selector;
     WORD limit;
     WORD base;
     WORD offset;
     WORD bit;
     WORD flag;
     WORD port;
   } IOBMENTRY;

typedef IOBMENTRY FAR * LPIOBMENTRY;

typedef struct tagLDTENTRY
   {
     WORD ldtselector;
     WORD ldtlimit;
     WORD selector;
     DWORD base;
     DWORD limit;
     WORD attr;
   } LDTENTRY;

typedef LDTENTRY FAR * LPLDTENTRY;

typedef struct tagPAGEENTRY
  {
   WORD  wEntry;          /* Entry */
   DWORD dwPage;          /* phys Page */
   WORD  wFlags;          /* Flags */
  } PAGEENTRY;

#ifdef _FIND_T_DEFINED
typedef struct _find_t _far * LPFINDT;
#else
typedef void _far * LPFINDT;
#endif

typedef struct tagENUMFILESTRUCT {
    LPSTR   lpszPath;
#ifdef __cplusplus
    BOOL (FAR PASCAL *fpCallbackFiles)(tagENUMFILESTRUCT FAR *,LPSTR,LPFINDT);
    BOOL (FAR PASCAL *fpCallbackDirs)(tagENUMFILESTRUCT FAR *,LPSTR,LPFINDT);
#else
    BOOL (FAR PASCAL *fpCallbackFiles)(LPENUMFILESTRUCT,LPSTR,LPFINDT);
    BOOL (FAR PASCAL *fpCallbackDirs)(LPENUMFILESTRUCT,LPSTR,LPFINDT);
#endif
    WORD    wTypes;      // welche Eintraege sollen aufgelistet werden?
    WORD    wNumFiles;
    WORD    wDirectories;
    DWORD   dwSizeOfDirs;
    WORD    wClSize;
    WORD    wDirEntries;
    WORD    wDirLevel;
   } ENUMFILESTRUCT,FAR * LPENUMFILESTRUCT;

typedef struct tagEXEC {    // Parameter EXEC (ax=4B00)
   unsigned segEnv;
   LPSTR    lpParm;
   LPVOID   lpFCB1;
   LPVOID   lpFCB2;
           } EXECPARM;

/* funktions prototypen */

typedef struct tagPUSHAREGS   /* registerbelegung bei pusha */
  {
    WORD       rdi;
    WORD       rsi;
    WORD       rbp;
    WORD       rsp;
    WORD       rbx;
    WORD       rdx;
    WORD       rcx;
    WORD       rax;
  } PUSHAREGS;

typedef struct tagPUSHADREGS   /* registerbelegung bei pushad */
  {
    DWORD       redi;
    DWORD       resi;
    DWORD       rebp;
    DWORD       resp;
    DWORD       rebx;
    DWORD       redx;
    DWORD       recx;
    DWORD       reax;
  } PUSHADREGS;

#ifdef __cplusplus
extern "C" {
#endif

int          _CDECL _setalarm(long int,char *);
long int     _CDECL _resetalarm(void);
void     FAR pascal DosBeep(int,int);                 /* tonhohe,dauer   */

BOOL     FAR pascal DosGetCurrentDir(LPSTR);           /* winptr                   */
BOOL     FAR pascal DosChangeDir(LPSTR);               /* analog chdir  */
BOOL     FAR pascal DosMakeDir(LPSTR);                 /* analog mkdir  */
int      FAR pascal DosFindFirst(LPSTR,WORD,LPSTR);    /* rc = 0 ist OK! */
int      FAR pascal DosFindNext(LPSTR);
BOOL     FAR pascal DriveFirst(LPSTR);                 /* erstes gueltiges drive -> lpstr */
BOOL     FAR pascal DriveNext(LPSTR);

unsigned     _CDECL _GetPMode(void);                   /* processor mode (0,1,2) */

unsigned  pascal _GetSegWord(unsigned,unsigned);       /* wort aus unsigned:unsigned holen */

long FAR PASCAL     _GetString(char far *,int ,int );  /* string von Tastatur lesen  */
                                                       /* strptr,maxlen,cursorpos    */
                                                       /* RC: letzter KBD/MOU-Status */
void         _CDECL _VioSetCursor(int,int,int);        /* zeile,spalte,page          */
void         _CDECL _VioResetCursor(int,int,int);      /* zeile,spalte,page          */

char *       _CDECL  strcatc(char *,int);  /* P2 an P1 anfgen           */
char *       pascal  _memcatc(char *,int); /* P2 an P1 anfgen (schnell) */
char *       _CDECL  _memxchg(char *,char,char,int);/* P2 durch P3 in P1 ersetzen */
int          _CDECL  strnclr(char *,int,unsigned);/* die ersten P3 Bytes von P1 abloeschen mit P2 */
       /* P2 nach P1 kopieren, dabei alle vorkommen von P3 durch P4 ersetzen */
int          _CDECL  strchng(char *,char *,char *,char *);
int      FAR _CDECL  _fstrchng(LPSTR,LPSTR,LPSTR,LPSTR);
int          _CDECL  strfilter(char *,char); /* zeichen aus string rausfiltern */
char *       _CDECL  strip(char *,char);   /* trailing chars entfernen */
LPSTR    FAR _CDECL  _fstrip(LPSTR,char); /* trailing chars entfernen */

#ifndef __WINDOWS_H
int          _CDECL  lstrcpy(LPSTR,LPCSTR);               /* stringcopy mit far pointern */
int          _CDECL  _hlinecpy(HPSTR,char *, int);        /* zeile aus huge array holen  */
#endif
int          _CDECL  lstrncpy(char far *,char far *,int); /* strncpy mit far pointern   */
int      FAR PASCAL  _lmemcpy (LPBYTE,LPBYTE,WORD);
int          _CDECL  lstrncat(char far *,char far *,int); /* P2 an P1 anfgen           */
int          _CDECL  _lstrcmp(LPCSTR,LPCSTR);             /* modellspezifisches strcmp */
int          _CDECL  _lstrncmp(LPCSTR,LPCSTR,WORD);       /* modellspezifisches strncmp */

LPSTR        _CDECL  _hlmemcpy(LPSTR,HPSTR,WORD);       /* P2 -> P1,aber nur ueber 1 segment hinweg */
LPSTR        _CDECL  _hlmemcpyd(LPSTR,HPSTR,WORD);      /* P2 -> P1,aber nur ueber 1 segment hinweg */

DWORD    FAR PASCAL  _hstrlen (HPCSTR);                 /* gibt eigentlich DWORD zurueck */
int      FAR PASCAL  _hstrcmp (HPCSTR,HPCSTR);          /* */
HPVOID   FAR PASCAL  _hmemchr (HPVOID,char, DWORD);     /* P2 in P1 suchen */
HPVOID   FAR PASCAL  _hmemchri(HPVOID,char, DWORD);     /* P2 in P1 suchen */
DWORD    FAR PASCAL  _hmemchr32i(FWORD,char, DWORD);    /* P2 in P1 suchen */
HPVOID   FAR PASCAL  _hmemchrd(HPVOID,char, DWORD);     /* P2 in P1 suchen (down) */
HPVOID   FAR PASCAL  _hmemcpy (HPVOID,HPVOID,DWORD);   /* P2 -> P1 (DWORD <= 64k!) */
HPVOID   FAR PASCAL  _hmemcpyd(HPVOID,HPVOID,DWORD);   /* P2 -> P1 (DWORD <= 64k!) */
long     FAR PASCAL  _hmemset (HPVOID,char, DWORD);
                /* wie _hmemcpy, aber vorzeitiger abbruch wenn P3 auftritt */
HPVOID   FAR PASCAL  _hmemcpyx(HPVOID,HPVOID,WORD,WORD);
HPVOID   FAR PASCAL  _hmemcpy32(FWORD,FWORD,DWORD);
int      FAR PASCAL  _hmemicmp(HPVOID,HPVOID,DWORD);
int      FAR PASCAL  _hmemicmp32(FWORD,FWORD,DWORD);
HPSTR    FAR PASCAL  _hmemsearch(HPVOID,HPSTR,DWORD);
int      FAR PASCAL  _hmemcmp(HPVOID,HPVOID,DWORD);

int      FAR _cdecl  lsscanf(const char *, const char *, ...);

#ifndef __WINDOWS_H
int      FAR PASCAL  _lread(int,LPVOID,WORD);
int      FAR PASCAL  _lwrite(int,LPVOID,WORD);
#endif
long     FAR PASCAL  _hread(int,HPVOID,long);
long     FAR PASCAL  _hwrite(int,const void _huge *,long);

HPVOID   FAR PASCAL  _hmalloc(DWORD);
int      FAR PASCAL  _hmfree(HPVOID);

int          _CDECL  _setport(int,int,int);            /* ausgabe byte auf "indexport"  */
void         _CDECL  _ExecInt28(void);                 /* nichts zu tun, INT 28 aufrufen */
char *       _CDECL  xtoa(unsigned,char *);            /* P1 in string (hex) umwandeln   */
char *       _CDECL  utoa(unsigned,char *);            /* P1 in string (dez) umwandeln   */
char *       _CDECL  _axtou(char *,unsigned *);        /* P1 in unsigned umwandeln   */
void far* FAR _CDECL _GetStartofMCBs(void);            /* anfang der MCB-Kette */
int          _CDECL  _GetOwner(char far *,char *);     /* owner aus environment holen */
LPSTR        _CDECL  _ConvByteHex(LPSTR,char);         /* byte -> string */
char *       _CDECL  _ConvDWordDez(char *,unsigned long,int);  /* DWORD -> string */
unsigned     _CDECL  _loadprg(char *,char *,int);      /* programm laden, nicht ausfhren */
WORD         _CDECL  GetMyDS();

WORD         pascal  xsetjmp(void *);                  /* 9 register */
WORD         _CDECL  xlongjmp(void *,WORD);

WORD     FAR PASCAL GetLastException(void);
BOOL     FAR PASCAL RegisterKeyboardDispatcher(FARPROC);
BOOL     FAR PASCAL UnregisterKeyboardDispatcher(void);

WORD     FAR PASCAL IsValidSelector(WORD);             // LAR ausfuehren
WORD     FAR PASCAL IsPresent(WORD);                   // LAR + test auf NP
WORD     FAR PASCAL IsValidAddress(LPVOID);            // LAR + SEL -> ES
WORD     FAR PASCAL IsReadableSelector(WORD);          // VERR ausfuehren
BOOL     FAR PASCAL IsMemoryDescriptor(WORD);
WORD     FAR PASCAL AllocDescriptor(DWORD,WORD);       /* basis, limit */
WORD     FAR PASCAL FreeDescriptor(WORD);
WORD     FAR PASCAL AllocRMDescriptor(WORD);           /* segment */
BOOL     FAR PASCAL GetDescriptor(WORD,LPDESCRIPTOR);
BOOL     FAR PASCAL SetDescriptor(WORD,DWORD,DWORD,WORD);  /* selector,basis,limit,attr */
BOOL     FAR PASCAL GetBaseofSelector(WORD,LPDWORD);
BOOL     FAR PASCAL GetLimitofSelector(WORD,LPDWORD);
BOOL     FAR PASCAL GetAttrofSelector(WORD,UINT FAR *);
WORD     FAR PASCAL AdjustDescriptorAttr(WORD);
BOOL     FAR PASCAL IDTFirst(LPIDTENTRY);
BOOL     FAR PASCAL IDTNext(LPIDTENTRY);
BOOL     FAR PASCAL IDTFind(LPIDTENTRY);
BOOL     FAR PASCAL GDTFirst(LPGDTENTRY);
BOOL     FAR PASCAL GDTNext(LPGDTENTRY);
BOOL     FAR PASCAL IOBMFirst(WORD,LPIOBMENTRY);
BOOL     FAR PASCAL IOBMNext(WORD,LPIOBMENTRY);
BOOL     FAR PASCAL IOBMClose(WORD,LPIOBMENTRY);
BOOL     FAR PASCAL LDTFirst(WORD,LPLDTENTRY);
BOOL     FAR PASCAL LDTNext(WORD,LPLDTENTRY);
BOOL     FAR PASCAL LDTClose(WORD,LPLDTENTRY);
BOOL     FAR PASCAL LDTFind(WORD,LPLDTENTRY);
WORD     FAR PASCAL GetTextField(PSTR,WORD,PSTR,WORD); /* feldinhalt aus tabstring holen */

int          _CDECL GetDrive();
int          _CDECL SetDrive(int);
DWORD    FAR PASCAL Unassemble(FWORD,LPSTR,WORD);
void     FAR        lqsort(void FAR *,WORD,WORD,FARPROC);
void                lqsorts(void FAR *,WORD,WORD,NEARPROC);
int      FAR PASCAL _hqsortlp(void FAR *,DWORD);
void     FAR PASCAL GetDescriptorType(WORD,LPSTR);
void     FAR PASCAL WaitTime(WORD);
WORD     FAR PASCAL GetLDTR(void);
WORD     FAR PASCAL GetTR(void);
DWORD    FAR PASCAL GetGDTAddress(void);
WORD     FAR PASCAL GetGDTLimit(void);
DWORD    FAR PASCAL GetIDTAddress(void);
WORD     FAR PASCAL GetIDTLimit(void);
WORD     FAR PASCAL GetIOPL(void);
WORD     FAR PASCAL GetCPL(void);
VOID     FAR PASCAL _strout(LPSTR);
DWORD    FAR PASCAL _GetFileSize(LPSTR);
VOID     FAR PASCAL _lwriteline(int,LPSTR);
BOOL     FAR PASCAL CallRing0Proc(FARPROC,LPDWORD);
BOOL     FAR PASCAL PageFirst(LPDWORD,PAGEENTRY FAR *);
BOOL     FAR PASCAL PageNext(LPDWORD,PAGEENTRY FAR *);
BOOL     FAR PASCAL CreateRegister(LPSTR,DWORD,LPSTR);
BOOL     FAR PASCAL SetComSpeed(WORD wPort, WORD wSpeed);
LPSTR    FAR PASCAL FindEnvironmentString(LPSTR lpKey);
WORD     FAR PASCAL GetEnvironmentString(LPSTR lpKey,LPSTR lpDest,WORD wSize);
WORD     FAR PASCAL ReplaceEnvironmentString(LPSTR lpKey,LPSTR lpNewString);
WORD     FAR PASCAL GetEnvironmentSize();
BOOL     FAR PASCAL CopyCS(LPSTR,LPSTR,WORD);
BOOL     FAR PASCAL EnumFiles(LPENUMFILESTRUCT);
LPVOID   FAR PASCAL GetSDAAddr();
WORD     FAR PASCAL GetLDTSelector();
WORD     FAR PASCAL GetTrueVersion();
int  __cdecl _fcputs(const char _far *);
#ifndef _lopen
int      FAR PASCAL _lopen(const char _far *, int );
#endif

#ifndef _WINDOWS
unsigned _cdecl _dos_reallocmem(unsigned, unsigned);
int     FAR CDECL wsprintf(LPSTR lpszOut, LPCSTR lpszFmt, ...);

#endif

#ifdef __cplusplus
}
#endif

#endif

