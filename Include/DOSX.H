
// undokumentierte DOS strukturen

typedef struct tagPSP {
   unsigned int  KB;
   unsigned int  memory;
   unsigned char filler1a[06];
   unsigned long iv22;
   unsigned long iv23;
   unsigned long iv24;
   unsigned int  parent;
   unsigned char fhandles[20];
   unsigned int  environment;
   unsigned long dwStack;         // 2E+30
   unsigned int  wHdlTabSize;     // 32
   unsigned long wHdlTabAddr;     // 34+36
   char res[72];
   unsigned char cCmdlineLength;
   char szCmdline[127];
           } PSP;

typedef struct tagMCB {
   unsigned char kennbyte;
   unsigned pspptr;
   unsigned mcblen;
   char     filler[3];
   union {
     char name[8];
     char sysk[2];
         };
 } MCB;

typedef MCB  far *LPMCB;
typedef PSP  far *LPPSP;
typedef char far *LPENV;

typedef struct tagMCBENTRY
  {
    DWORD       addr;            /* 0: flat addresse */
    DWORD       size;            /* 4 */
    char        name[9];         /* 8 */
    WORD        psp;             // 17 */
    DWORD       naddr;           /* 19 */
    WORD        sel;             /* 23: selector (nur temporaer gueltig) */
    BYTE        bMode;           /* 25: 1 = protected mode */
    BYTE        bKB;             // kennbyte - MCB[0]
    WORD        wSK;             // Systemkennung (falls name=SYSTEM)
  } MCBENTRY;

typedef struct tagDEVENTRY
  {
    DWORD       res;             /* 0: length */
    WORD        index;           /* 4: real mode addresse */
    WORD        offs;            /* 6: offset */
    WORD        segm;            /* 8: real mode */
    WORD        sel;             /* 10: protected mode */
    char        name[9];         /* 12: name */
    WORD        attr;            /* 21: attribute */
    WORD        typ;             /* 23: devicetyp */
    WORD        res1;            /* 25: res1 */
    WORD        res2;            /* 27: res2 */
    WORD        noffs;           /* 29: next offset */
    WORD        nsegm;           /* 31: next segment */
    BYTE        bMode;           /* 33: 1 = protected mode */
  } DEVENTRY;

typedef struct tagSFTENTRY
  {
    DWORD       addr;           /*  0 */
    WORD        anzahl;         /*  4 */
    WORD        index;          /*  6 index in der SFT (00 - FF) */
    WORD        length;         /*  8 laenge eines eintrags (DOS abhaengig) */
    WORD        offset;         /* 10 aktueller offset des eintrags */
    BYTE        bMode;          /* 12 1 = protected mode */
    BYTE        reserved;       /* 13 */

    WORD        handles;        /* anzahl handles, 0 = file nicht geoeffnet */
    WORD        open_mode;
    WORD        devinfo;        /* device info */
    WORD        w1[13];
    char        name[8];
    char        ext[3];
    DWORD       share_pref_sft;
    WORD        share_net_machine;
    WORD        psp;
    WORD        w2[10];
  } SFTENTRY;


typedef struct {
       unsigned flag0:1;
       unsigned flag1:1;
       unsigned flag2:1;
       unsigned flag3:1;
       unsigned flag4:1;
       unsigned flag5:1;
       unsigned flag6:1;
       unsigned flag7:1;
       unsigned flag8:1;
       unsigned flag9:1;
       unsigned flagA:1;
       unsigned flagB:1;
       unsigned subst:1;
       unsigned join:1;
       unsigned physical:1;
       unsigned network:1;
           }   CDSFLAGS;

typedef struct tagCDSENTRY
  {
   BYTE  bLastdrive;
   BYTE  bDrive;
   LPSTR lpEntry;
   WORD  wDistance;
   WORD  wSegment;
   union {
      CDSFLAGS flags;
      WORD wFlags;
      };
   DWORD lpDPB;
   char  szPath[68];
  } CDSENTRY;

typedef struct tagDPBENTRY
  {
   LPVOID lpNextDPB;      // intern
   WORD  wOffset;         // intern
   DWORD dwRMAdress;      // intern: real mode adress
   BYTE  bDrive;          // 00: Drive (0=A...)
   BYTE  bUnit;           // 01: Unit
   WORD  wBytesSector;    // 02: Bytes/Sektor
   BYTE  bSectorCluster;  // 04: Sektoren in Cluster - 1
   BYTE  bShiftCount;     // 05: Shift Count Cluster Size
   WORD  wResSectors;     // 06: reservierte Sektoren (boot sectors)
   BYTE  bNumFATs;        // 08: Anzahl FATs
   WORD  wNumEntries;     // 09: Anzahl rootdir Entries
   WORD  wFirstDataSector;// 11: first data sector
   WORD  wMaxCluster;     // 13: highest cluster
   WORD  wSectorsFAT;     // 15: Sektoren / FAT (DOS 4+)
   WORD  wSectorRoot;     // 17: first sector of root dir
   DWORD dwDevDriver;     // 19: Device Driver (RM)
   BYTE  bMediaDesc;      // 23: media descriptor
   BYTE  bAccessFlag;     // 24:
   DWORD reserved;        // 25:
   WORD  wStartCluster;   // 29: ???
   WORD  wFreeClusters;   // 31: ???
  } DPBENTRY;

typedef struct tagREDIRECTIONENTRY
  {
   WORD wNext;
   WORD wErrCode;
   BYTE bCode;
   BYTE bStatus;
   WORD wUser;
   char szLocalName[16];
   char szRemoteName[128];
  } REDIRECTIONENTRY,FAR * LPREDIRECTIONENTRY;

#ifdef __cplusplus
extern "C" {
#endif

BOOL     FAR pascal McbFirst(MCBENTRY FAR *, WORD);
BOOL     FAR pascal McbNext(MCBENTRY FAR *);
BOOL     FAR pascal McbSubFirst(MCBENTRY FAR *);
BOOL     FAR pascal McbSubNext(MCBENTRY FAR *);
BOOL     FAR pascal McbClose(MCBENTRY FAR *);
BOOL     FAR pascal DeviceFirst(DEVENTRY FAR *, WORD);
BOOL     FAR pascal DeviceNext(DEVENTRY FAR *);
BOOL     FAR pascal SFTFirst(SFTENTRY FAR *, WORD);
BOOL     FAR pascal SFTNext(SFTENTRY FAR *);
BOOL     FAR pascal SFTClose(SFTENTRY FAR *);
BOOL     FAR pascal CDSFirst(CDSENTRY FAR *);
BOOL     FAR pascal CDSNext(CDSENTRY FAR *);
BOOL     FAR pascal DPBFirst(DPBENTRY FAR *);
BOOL     FAR pascal DPBNext(DPBENTRY FAR *);
BOOL     FAR pascal RedirectionNext(LPREDIRECTIONENTRY);

#ifdef __cplusplus
}
#endif
