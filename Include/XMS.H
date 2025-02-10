
typedef struct tagEMBENTRY
   {
     WORD wHandle;
     FARPROC fpEntry;  // Entry (real mode)
     BYTE bFree;
     BYTE bLocks;
     WORD wLength;
     WORD wNumTries;   // anzahl versuche bis ruecksprung
     DWORD dwAddr;     // lineare adresse
   } EMBENTRY;

typedef EMBENTRY FAR * LPEMBENTRY;


/* xms-funktionen */

#ifdef __cplusplus
extern "C" {
#endif

FARPROC PASCAL FAR _XMS_Check(void);      // ÅberprÅfen ob XMS vorhanden (real mode)
FARPROC PASCAL FAR _XMS_Alloc(FARPROC,WORD);   // speicher allokieren
FARPROC PASCAL FAR _XMS_Free(FARPROC,WORD);    // speicher freigeben
FARPROC PASCAL FAR _XMS_Lock(FARPROC,WORD);    // speicher sperren
FARPROC PASCAL FAR _XMS_Unlock(FARPROC,WORD);    // speicher entsperren
BOOL    PASCAL FAR EMBFirst(LPEMBENTRY,FARPROC);
BOOL    PASCAL FAR EMBNext(LPEMBENTRY);
BOOL    PASCAL FAR EMBClose(LPEMBENTRY);

#ifdef __cplusplus
}
#endif

