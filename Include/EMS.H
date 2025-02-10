
/* ems-funktionen */

#ifndef NO_EXT_KEYS
    #define pascal  pascal
#else
    #define pascal
#endif

#ifdef __cplusplus
extern "C" {
#endif

// EMS 3.2 Prototypen

int   FAR PASCAL  _EMS_Check(void);           /* ÅberprÅfen ob EMS vorhanden */
WORD  FAR PASCAL  _EMS_GetVersion(void);
DWORD FAR PASCAL  _EMS_GetPageFrame(void);    /* (HIWORD=Error) */
DWORD FAR PASCAL  _EMS_GetFreePages(void);    /* freie seiten (HIWORD=Error) */
DWORD FAR PASCAL  _EMS_GetPages(void);        /* seiten insgesamt (HIWORD=Error) */
int   FAR PASCAL  _EMS_GetHandlePages(HANDLE);/* pages eines handles ermitteln */
WORD  FAR PASCAL  _EMS_MapPages(HANDLE,WORD,WORD);

// EMS 4.0 Prototypen

int   FAR PASCAL  _EMS_GetHandleName(HANDLE,LPSTR);/* name eines Handles ermitteln */
int   FAR PASCAL  _EMS_GetHandleDirectory(LPVOID); /* aktuelle handles ermitteln (bereich <= 2550!)*/
DWORD FAR PASCAL  _EMS_GetTotalHandles(VOID);      /* anzahl handles gesamt */

#ifdef __cplusplus
}
#endif

