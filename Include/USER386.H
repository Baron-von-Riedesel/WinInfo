
/* this is the struct for Win9x */

typedef struct tagVMENTRY
  {
   DWORD dwNextVM;      /* intern */
   DWORD dwFlatSel;     /* intern */
   DWORD dwCurHandle;   /* 08 */
   DWORD dwHandle;      /* 12 */
   DWORD dwFlags;       /* 16 */
   DWORD dwV86High;     /* 20 */
   DWORD dwVMID;        /* 24 */
   DWORD dwPageTab;     /* 28 */
   DWORD dwClient;      /* 32 */
   DWORD dwIDTAddr;     /* 36 */
   WORD  wIDTLimit;
   WORD  wRes;          // 38
   DWORD dwPMIntVecs;   // protected mode int vecs ring 3
   WORD  wLDTSel;       // 44
  } VMENTRY;

typedef struct tagVMPGCNTENTRY
  {
   DWORD dwHandle;
   DWORD dwPages;
   DWORD dwNmPages;
  } VMPGCNTENTRY;

// Prototypen

#ifdef __cplusplus
extern "C" {
#endif

DWORD    FAR PASCAL  _hfmalloc (DWORD,DWORD);          /* addresse in DX:AX, handle ptr in parm2 */
BOOL     FAR PASCAL  _hffree   (DWORD);                /* handle in parm1 */
DWORD    FAR PASCAL  _hfmemset (DWORD,char, DWORD);
DWORD    FAR PASCAL  _hfmemchri(DWORD,char, DWORD);
DWORD    FAR PASCAL  _hfmemcpy (DWORD,DWORD,DWORD);
int      FAR PASCAL  _hfmemicmp(DWORD,DWORD,DWORD);

WORD     FAR PASCAL _lar(WORD);
BOOL     FAR PASCAL VMFirst(VMENTRY FAR *);
BOOL     FAR PASCAL VMNext(VMENTRY FAR *);
BOOL     FAR PASCAL VMClose(VMENTRY FAR *);
BOOL     FAR PASCAL VMFindHandle(DWORD,VMENTRY FAR *);
BOOL     FAR PASCAL GetVMPgCount(VMPGCNTENTRY FAR *);
BOOL     FAR PASCAL SetVMFocus(DWORD);
BOOL     FAR PASCAL DestroyVM(DWORD);
DWORD    FAR PASCAL GetLinearPSPAddress(void);
WORD     FAR PASCAL AllocBigDescriptor(DWORD,DWORD);
WORD     FAR PASCAL FreeBigDescriptor(WORD);
BOOL     FAR PASCAL GetFlatAddrBytes(LPVOID,DWORD,DWORD);
DWORD    FAR PASCAL _read32(DWORD,char _far *,DWORD);
DWORD    FAR PASCAL _write32(DWORD,char _far *,DWORD);
DWORD    FAR PASCAL GetCR3(void);

#ifdef __cplusplus
}
#endif

