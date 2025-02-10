
                                          /* extended listbox messages */
#define XLB_REPLACESTRING  (WM_USER+4)
#define XLB_ADDMULTSTRINGS (WM_USER+30)
#define XLB_CHANGECURPOS   (WM_USER+31)
#define XLB_GETLINES       (WM_USER+32)
#define XLB_SETSORTPROC    (WM_USER+33)
#define XLB_SORT           (WM_USER+34)
#define XLB_SETSORTFLAG    (WM_USER+35)
#define XLB_SETSORTSTART   (WM_USER+36)
#define XLB_SAVEBOX        (WM_USER+37)
#define XLB_LOADBOX        (WM_USER+38)
#define XLB_SETEXTSTYLE    (WM_USER+39)
#define XLB_GETWINDOWDATA  (WM_USER+40)
#define XLB_SETWINDOWDATA  (WM_USER+41)
#define XLB_GETCAPHEIGHT   (WM_USER+42)
#define XLB_SETFONTALT     (WM_USER+43)
#define XLB_SETSORTDIR     (WM_USER+44)
                                          /* extended listbox styles */
                                          /* keine Selektion moeglich (statt LBS_NOREDRAW) */
#define XLBS_NOINPUT         0x0004L
                                          /* LBN_SELCHANGE msg bei seltrack (statt LBS_USETABSTOPS) */
#define XLBS_SELTRACK        0x0080L
#define XLBS_EDGEBORDER      0x0800L      // neu: 8.12.99
#define XLBS_SHADOWBORDER    0x1000L
#define XLBS_PLAST           0x2000L
#define XLBS_OWNERHASSTRINGS 0x4000L
#define XLBS_OWNERHASALL     0x8000L
#define XLBS_STANDARD WS_CHILD | WS_VSCROLL | WS_TABSTOP | WS_GROUP | XLBS_SHADOWBORDER | WS_DLGFRAME | XLBS_PLAST
#define XLBS_STANDARD2 WS_CHILD | WS_VSCROLL | WS_TABSTOP | WS_GROUP | XLBS_SHADOWBORDER | WS_DLGFRAME
                                          /* notification bei selwechsel mit gedrueckter maustaste */
#define LBN_SELTRACK         11
#define XLBN_STARTDRAW       12
#define XLBN_ENDDRAW         13
#define XLBN_SCROLLNOTIFY    14
#define XLBN_DROPFILES       15
#define XLBN_RBUTTONDOWN     16
                                          /* Extended Window Style (nur WORD!) */
#define XLBES_SCROLLNOTIFY 0x0001
#define XLBES_DRAGDETECT   0x0002         // Dragversuch ermitteln
#define XLBES_DROPOBJECT   0x0004         // WM_DROPOBJECT verarbeiten
#define XLBES_DROPACCEPT   0x0008         // bei WM_QUERYDROPOBJECT rc=TRUE
#define XLBES_RBUTTONTRACK 0x0010         // RButton wie LButton

#define WLBSS_CLOSE        0x0001         // am ende file schliessen
#define WLBSS_CLIPBOARD    0x0002         // zusaetzlich ins clipboard kopieren
#define WLBSS_XXXXX        0x0004         // windowtext als ueberschrift
#define WLBSS_MESSAGE      0x0008         // Messagebox ausgeben

#ifdef __cplusplus
extern "C" {
#endif
BOOL WINAPI XListBoxRegisterClass(HINSTANCE, LPSTR, WORD);
#ifdef __cplusplus
}
#endif

