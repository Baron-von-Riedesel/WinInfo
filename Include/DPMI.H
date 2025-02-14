
// structures

#ifndef SELECTOR
typedef unsigned short SELECTOR;
#endif


typedef struct _tagRMCSTRUCT              /* DPMI real mode call structure */
  {
    union {
    DWORD       rEDI;
    WORD        rDI;
    };
    union {
    DWORD       rESI;
    WORD        rSI;
    };
    DWORD       rEBP;
    DWORD       rRsvd;
    union {
    DWORD       rEBX;
    WORD        rBX;
    BYTE        rBL;
    };
    union {
    DWORD       rEDX;
    WORD        rDX;
    BYTE        rDL;
    };
    union {
    DWORD       rECX;
    WORD        rCX;
    BYTE        rCL;
    };
    union {
    DWORD       rEAX;
    WORD        rAX;
    struct {
        BYTE    rAL;
        BYTE    rAH;
    };
    };
     WORD       rFlags;
     WORD       rES;
     WORD       rDS;
     WORD       rFS;
     WORD       rGS;
     WORD       rIP;
     WORD       rCS;
     WORD       rSP;
     WORD       rSS;
  } RMCSTRUCT;

typedef RMCSTRUCT FAR * LPRMCSTRUCT;

typedef struct tagDPMIMEMORY
  {
    DWORD       maxFreeBytes;
    DWORD       dwFreePages;             /* freie pages */
    DWORD       dwMaxAllocLockLinPages;
    DWORD       dwLinearSpace;           /* linearer adressraum */
    DWORD       dwUnlockedPhysPages;
    DWORD       dwFreePhysPages;
    DWORD       dwPhysPages;
    DWORD       dwFreeLinearSpace;       /* freier linearer adressraum */
    DWORD       numSwapPages;
    DWORD       reserved1;
    DWORD       reserved2;
    DWORD       reserved3;
  } DPMIMEMORY,FAR * LPDPMIMEMORY;

// Prototypen

#ifdef __cplusplus
extern "C" {
#endif

WORD    FAR PASCAL  DPMIExist(void);
DWORD   FAR PASCAL  DPMIGetVersion(void);               /* AX: version, DX: Interruptvektoren */
DWORD   FAR PASCAL  DPMIGetVersionEx(void);             /* AX: flags, DX: Prozessor */
void    FAR PASCAL  DPMIGetRawModeSwitchAddress(LPSTR); /* NPSTR -> 8 Bytes */
DWORD   FAR PASCAL  DPMIGetPageSize(void);              /* */
                                                        /* bei fehler: AX < 0x40,DX=maxsize  */
void    FAR PASCAL  DPMIGetMemoryInfo(LPDPMIMEMORY);    /* LPSTR -> 48 Bytes */
DWORD   FAR PASCAL  DPMIAllocDosMemory(WORD);           /* RC: AX=Segmentadresse,DX=Selektor */
WORD    FAR PASCAL  DPMIFreeDosMemory(SELECTOR);        /* RC: BOOL */
WORD    FAR PASCAL  DPMIAllocMemory(DWORD,DWORD FAR *); /* RC: 1=OK */
WORD    FAR PASCAL  DPMIFreeMemory(DWORD);              /* RC: 1=OK */

SELECTOR FAR PASCAL DPMIAllocDescriptor(WORD);          /* P1 Descriptoren reservieren */
BOOL    FAR PASCAL  DPMIFreeDescriptor(SELECTOR);       /* P1 Descriptor freigeben */
WORD    FAR PASCAL  DPMISegToDescriptor(WORD);          /* RC: real mode Segment */
WORD    FAR PASCAL  DPMIGetIncValue(VOID);              /* RC: increment value */
BOOL    FAR PASCAL  DPMISelectorLock(SELECTOR);         /* RC: 1=ok (undok) */
BOOL    FAR PASCAL  DPMISelectorUnlock(SELECTOR);       /* RC: 1=ok (undok) */
DWORD   FAR PASCAL  DPMIGetSegmentBase(SELECTOR);       /* */
void    FAR PASCAL  DPMIGetDescriptor(WORD,DESCRIPTOR FAR *);// RC: SSSS:OOOO
void    FAR PASCAL  DPMISetAccessRights(WORD,WORD);      /* RC: SSSS:OOOO */

DWORD   FAR PASCAL  DPMIGetPMIntVector(WORD);           /* RC: SSSS:OOOO */
DWORD   FAR PASCAL  DPMIGetRMIntVector(WORD);           /* RC: SSSS:OOOO */
void    FAR PASCAL  DPMISetRMIntVector(WORD,LPSTR);     /*    */
void    FAR PASCAL  DPMISetPMIntVector(WORD,LPSTR);     /*    */
DWORD   FAR PASCAL  DPMIGetExcVector(WORD);             /* RC: SSSS:OOOO */
void    FAR PASCAL  DPMISetExcVector(WORD,FARPROC);     /* */
void    FAR PASCAL  DPMISimRMInterrupt(WORD,WORD,LPRMCSTRUCT);/* */
void    FAR PASCAL  DPMICallRMFarProc(DWORD,LPRMCSTRUCT,WORD,LPWORD);/* */

BOOL    FAR PASCAL  DPMILockRegion(DWORD,DWORD);         /* RC: 1=ok parm1: LinAddr Parm2: Laenge */
BOOL    FAR PASCAL  DPMIUnlockRegion(DWORD,DWORD);       /* RC: 1=ok parm1: LinAddr Parm2: Laenge */
DWORD   FAR PASCAL  DPMIMapPhysToLinear(DWORD,DWORD);

// structure for LDT browse

typedef struct _tagDPMISTRUCT
   {
     WORD  selector;           // 00: aktueller selector
     DWORD base;               // 02: base
     DWORD limit;              // 06: limit
     WORD  attribute;          // 0A: attr
     WORD  wSize;              // 0C: groesse der LDT
     WORD  wLDTSel;            // 0E: LDT selector
   } DPMISTRUCT;

typedef DPMISTRUCT FAR * LPDPMISTRUCT;

BOOL    FAR PASCAL  GetDPMIDescriptorFirst(LPDPMISTRUCT);
BOOL    FAR PASCAL  GetDPMIDescriptorNext(LPDPMISTRUCT);

typedef struct _tagINT2FSTRUC
  {
    DWORD       rEAX;
    DWORD       rEBX;
    DWORD       rECX;
    DWORD       rEDX;
    DWORD       rESI;
    DWORD       rEDI;
    WORD        rES;
  } INT2FSTRUC;

typedef INT2FSTRUC FAR * LPINT2FSTRUC;

BOOL    FAR PASCAL  Int2FCall(LPINT2FSTRUC,LPINT2FSTRUC);          /* int 2f call */
BOOL    FAR PASCAL  VxDPMCall(FARPROC,LPINT2FSTRUC,LPINT2FSTRUC);  /* int 2f call */


#ifdef __cplusplus
}
#endif

