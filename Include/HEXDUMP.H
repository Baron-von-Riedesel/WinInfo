
                                          /* styles hexdump class */
#define HDS_WINDOWBK      0x0008L
#define HDS_PLAST         0x0010L
                                          /* notifications der hexboxen */
#define HEXN_ERROR1 11
#define HEXN_ERROR2 12
#define HEXN_ERROR3 13
#define HEXN_ERROR4 14
#define HEXN_ERROR5 15                    /* ??? */
#define HEXN_ERROR6 16                    /* UAE waehrend display vorgekommen */
#define HEXN_ERROR7 17                    /* fehler beim speichern */
#define HEXN_ERROR8 18
#define HEXN_ERROR9 19
#define HEXN_RBUTTONDOWN 20
                                          /* messages fuer hexdump */
#define HD_RESETCONTENT (WM_USER+2)
#define HD_GETMODE      (WM_USER+3)
#define HD_SETMODE      (WM_USER+4)
#define HD_SAVE         (WM_USER+5)
#define HD_SEARCHSTRING (WM_USER+6)
#define HD_SETPOSITION  (WM_USER+7)
#define HD_GETOFFSET    (WM_USER+8)
#define HD_GETSIZE      (WM_USER+9)
#define HD_SETVIEWOFFSET (WM_USER+10)

// bitwerte von HEXDUMPPARM.wType
//
#define HDP_NORMAL         0x0000
#define HDP_RESHDL         0x0002
#define HDP_FREEHDL        0x0004         // von wiview verwendet
#define HDP_LINEAR         0x0100         // von HexDump verwendet
#define HDP_SETWINDOWTEXT  0x0200         // von wiview verwendet
#define HDP_DISABLESELEDIT 0x0400         // von wiview verwendet

// Wert von Parameter HD_GETMODE/SETMODE (flags in HEXDUMP)

#define HDT_BYTE           0x0000
#define HDT_WORD           0x0001
#define HDT_DWORD          0x0002
#define HDT_TEXT           0x0004
#define HDT_USESTDCONTEXTMENU 0x0100

typedef struct tagHEXDUMPPARM {
     WORD   wType;
    DWORD   dwOffset;
    HGLOBAL hGlobal;
    DWORD   dwLength;
   } HEXDUMPPARM,FAR *LPHEXDUMPPARM;

#ifdef __cplusplus
extern "C" {
#endif

BOOL EXPORTED WINAPI HexDumpRegisterClass(HINSTANCE, LPSTR, WORD);

#define HexDump_ResetContent(hWnd,lpHexDumpParm) (int)SendMessage((hWnd),HD_RESETCONTENT,0,(LPARAM)(LPHEXDUMPPARM)(lpHexDumpParm))
#define HexDump_SetMode(hWnd,wMode) (int)SendMessage((hWnd),HD_SETMODE,(WPARAM)(wMode),(LPARAM)0)

#ifdef __cplusplus
}
#endif
