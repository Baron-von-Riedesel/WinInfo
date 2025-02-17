
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"


/* externals */

/* konstanten + typdefinitionen */

#define WM_F1DOWN   0x401
// #define strcpy(x,y) lstrcpy((LPSTR)x,(LPSTR)y)

typedef struct tagXLOGPALETTE {
    WORD          palVersion;
    WORD          palNumEntries;
    PALETTEENTRY  palPalEntry[21];
} XLOGPALETTE;

/* funktions prototypen */

BOOL    CALLBACK MenuDlg     (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ShowWndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WIDlgWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SIWndProc   (HWND, UINT, WPARAM, LPARAM);

// int  FAR PASCAL FilterFunc(int, WORD, DWORD);

/* globale variable */

char fnstr[128];                    /* statischer Bereich fuer filenamen */
char   ext[64] = {"*.exe;*.dll;*.drv"};

HINSTANCE hInst         = 0;       /* Instanz der Task */
HICON    hIcon          = 0;
HFONT    hFont          = 0;
HCURSOR  hCursor        = 0;
HCURSOR  hCursorWait    = 0;
HPALETTE hPalette       = 0;       /* eigene Palette (1 zustzliche Farbe) */
HBRUSH   hBrush         = 0;       /* Brush fr Dialogbox-Hintergrund */
HWND     hWndMain       = 0;       /* Handle des Hauptfensters */
HWND     hWndMenu       = 0;       /* Menu Window */
HINSTANCE hLibCtl3d      = 0;
HFONT    hFontAlt       = 0;

int fTest             = 0;       /* soll testmodus ueber F9 eingeschaltet werden? */
int fUnloadwarning    = 1;       /* warnungen beim entladen ausgeben */
int fOwnerDrawWarning = 1;       /* warnungen bei Dialogboxen mit Ownerdraw Controls */
int fNativeimport     = 0;       /* native mode beim import dialog */
int unassLines        = 0x100;   /* disassemblierte Zeilen bei unassemble */
int fShowBytes        = 0;
int fGetVMPSP         = 0;
BOOL fLoadCtl3d       = 0;
BYTE fNewReg = 0;
BYTE fShow;                      /* globales Flag zur Kommunikation (allg. verwendbar) */
RECT prect;                      /* ??? */
HHOOK lpFilterFunc;
char szIniName[] = {"wininfo.ini"};
char szVersion[] = {"WI16"};
char szNo[5];
char szYes[5];
LPSTR lpszHint = 0;

int xpos      = 100,ypos      = 100;

extern int xTaskpos,yTaskpos;
extern int xModulpos,yModulpos;
extern int xGlobalpos,yGlobalpos;
extern int xUserpos,yUserpos;
extern int xGDIpos,yGDIpos;
extern int xDOSpos,yDOSpos;
extern int xDPMIpos,yDPMIpos;
extern int xSyspos,ySyspos;
extern int xImppos,yImppos;
extern int xExppos,yExppos;
extern int xRespos,yRespos;
extern int xClasspos,yClasspos;
extern int xLocpos,yLocpos;
extern int xFiltpos,yFiltpos;
extern int xStatpos,yStatpos;
// extern int xSelpos,ySelpos;
// extern int xLinpos,yLinpos;


                                             /* pointer fuer string load */
PSTR   pszWinName;
PSTR   pszStatMem1;
PSTR   errorstr1;
PSTR   errorstr2;

// statische Variable

static const char * pProfString[] = {
         "xMain", "yMain", "xTask", "yTask",         // 0-3
         "xModul", "yModul", "xGlobal", "yGlobal",   // 4-7
         "xUser", "yUser", "xGDI", "yGDI",           // 8-11
         "xDOS", "yDOS", "xDPMI", "yDPMI",           //12-15
         "xSys", "ySys", "xClass", "yClass",         //16-19
         "xImport", "yImport", "xExport", "yExport", //20-23
         "xRes", "yRes", "xLocal", "yLocal",         //24-27
//       "xSel", "ySel", "xLin", "yLin",             //28-31
         "xFilt", "yFilt", "xStatus", "yStatus",     //32-35
         "UnloadWarning", "OwnerDrawWarning",        //36-37
         "TestFlags", "NativeImport",                //38-39
         "GetVMPSP", "ShowBytes",                    //40-41
         "UnassLines", "LoadCtl3d",                  //42-43
         "Modul",
         0};

typedef struct tagPROFILEENTRY {
    BYTE flags;
    BYTE index;
    PINT wert;
   } PROFILEENTRY;

typedef PROFILEENTRY *PPROFILEENTRY;
                            
// zuerst müssen die Fensterpositionen kommen!                            

#define POSENTRIES 32            // Vorsicht: zaehler anpassen!
                            
static PROFILEENTRY proftab[] = {
                       1,0,&xpos,
                       1,0,&ypos,
                       1,0,&xTaskpos,
                       1,0,&yTaskpos,
                       1,0,&xModulpos,
                       1,0,&yModulpos,
                       1,0,&xGlobalpos,
                       1,0,&yGlobalpos,
                       1,0,&xUserpos,
                       1,0,&yUserpos,
                       1,0,&xGDIpos,
                       1,0,&yGDIpos,
                       1,0,&xDOSpos,
                       1,0,&yDOSpos,
                       1,0,&xDPMIpos,
                       1,0,&yDPMIpos,
                       1,0,&xSyspos,
                       1,0,&ySyspos,
                       1,0,&xClasspos,
                       1,0,&yClasspos,
                       1,0,&xImppos,
                       1,0,&yImppos,
                       1,0,&xExppos,
                       1,0,&yExppos,
                       1,0,&xRespos,
                       1,0,&yRespos,
                       1,0,&xLocpos,
                       1,0,&yLocpos,
//                       1,0,&xSelpos,
//                       1,0,&ySelpos,
//                       1,0,&xLinpos,
//                       1,0,&yLinpos,
                       1,0,&xFiltpos,
                       1,0,&yFiltpos,
                       1,0,&xStatpos,
                       1,0,&yStatpos,
                       0,1,&fUnloadwarning,
                       0,1,&fOwnerDrawWarning,
                       0,1,&fTest,
                       0,1,&fNativeimport,
                       0,1,&fGetVMPSP,
                       1,1,&fShowBytes,
                       1,2,&unassLines,
                       0,1,&fLoadCtl3d,
                       2,3,(PINT)ext,
                       0,0,NULL
                      };

static char * kaptab[] = {"positions","flags","ints","filter"};

                                             /* tabelle fuer string load */
static STRLOADENTRY CODESEG strloadtab[] = {
                       &pszWinName, IDS_TITLE,
                       &pszStatMem1,IDS_STATMEM1,
                       &errorstr1,  IDS_ERRMOD1,
                       &errorstr2,  IDS_ERRTSK1,
                       (PSTR *)(0)};

static LOGFONT lf = {16,0,0,0,700,0,0,0,0,0,0,0,0,"Helv"};
///////////////////////////////////////////////////////////
// CheckDlgBoxIcon - aufgerufen bei WM_PAINT aus Dialogen
// der Dialogboxklasse ist kein Icon zugeordnet          
// beim Verkleinern muss deshalb das Icon von Hand       
// gemalt werden.                                        
///////////////////////////////////////////////////////////
BOOL WINAPI CheckDlgBoxIcon(HWND hWnd)
{
 PAINTSTRUCT ps;

 if (IsIconic(hWnd))
    {
     BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
     DrawIcon(ps.hdc,0,0,hIcon);
     EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
     return TRUE;
    }
 return FALSE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Dialogbuttons enable/disable                         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void WINAPI EnableItems(HWND hDlg,PINT pInt,WORD wMode)
{
  if (pInt)
      for (;*pInt;pInt++)
          EnableDlgItem(hDlg,*pInt,wMode);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Popup Menu aus Dialog Buttons erzeugen               บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
HMENU MakePopupMenu(HWND hDlg,PINT pCmd)
{
  HMENU hPopupMenu;
  char  szStr[80];
  WORD  wCmd;
  HWND  hWnd;

  if (hPopupMenu = CreatePopupMenu())
      for (;*pCmd;pCmd++)
         {
          if (*pCmd == -1)
             ;
          else
          if (*pCmd & 0x8000)
             {
              wCmd = *pCmd & 0x7FFF;
              pCmd++;
              if (LoadString(hInst,*pCmd,szStr,sizeof(szStr)))
                  AppendMenu(hPopupMenu,MF_STRING | MF_ENABLED,wCmd,szStr);
             }
          else
          if (hWnd = GetDlgItem(hDlg,*pCmd))
             if (IsWindowEnabled(hWnd))
                {
                 GetWindowText(hWnd,szStr,sizeof(szStr));
                 AppendMenu(hPopupMenu,MF_STRING | MF_ENABLED,*pCmd,szStr);
                }
         }
  return hPopupMenu;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Popup erzeugen und warten bis zuende                 บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
HMENU WINAPI TrackPopup(HWND hDlg,PINT piCmd)
{
 HMENU hPopupMenu;
 POINT point;

  if (hPopupMenu = MakePopupMenu(hDlg,piCmd))
     {
      GetCursorPos(&point);
      TrackPopupMenu(hPopupMenu,
                     TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                     point.x,
                     point.y,
                     0,
                     hDlg,
                     0);
      DestroyMenu(hPopupMenu);
     }
  return 0;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ CheckMessage                                         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL WINAPI CheckMessage(MSG * pMsg, WORD wMode, HWND hWnd)
{
 BOOL rc = TRUE;

 if (PeekMessage(pMsg,0,0,0,PM_NOREMOVE))
    {
     if (pMsg->message == WM_QUIT)
         rc = FALSE;
     else               // WM_DESTROY wird direkt gesendet!!!
     if (pMsg->message == WM_CLOSE)
        {
         for (;hWnd;hWnd = GetParent(hWnd))
             if (pMsg->hwnd == hWnd)
                 rc = FALSE;
        }
     if (rc)
        {
         GetMessage(pMsg,0,0,0);
         DispatchMessage(pMsg);
        }
    }
 return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ FilterFunc - Message Filter (fuer F1, F6, F9)        บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK FilterFunc(int nCode,WORD wParam,LPARAM lParam)
{
 LPMSG lpmsg = (LPMSG)lParam;

 if  (nCode == MSGF_DIALOGBOX || nCode == MSGF_MENU)
       if (lpmsg->message == WM_KEYDOWN)
          switch (lpmsg->wParam)
            {
             case VK_F1:
               PostMessage(hWndMain, WM_F1DOWN, nCode, 0L);
               break;
             case VK_F6:
               PostMessage(hWndMenu,WM_COMMAND,ID_NEWSELWIN,0);
               break;
             case VK_F9:
               if (fTest & 0x0001)
                   PostMessage(GetActiveWindow(), WM_COMMAND, ID_TEST,0L);
               break;
            }

 DefHookProc(nCode, wParam, lParam, (HHOOK FAR *)&lpFilterFunc);

 return 0;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WIDlgWndProc - Dialogbox Windowfunktion              บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
LRESULT CALLBACK WIDlgWndProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 LRESULT rc = FALSE;
 HDC hdc;
 WORD i;
// LOGFONT lf;
 HPALETTE hPalOrg;

 switch (message)
   {
    case WM_CTLCOLOR:
        if (hPalette)
           {
            SelectPalette((HDC)wParam,hPalette,0);
            RealizePalette((HDC)wParam);
            rc = (LRESULT)MAKELONG(hBrush,0);
           }
        if ((HIWORD(lParam) == CTLCOLOR_STATIC)
          || (HIWORD(lParam) == CTLCOLOR_BTN))
           {
            SetTextColor((HDC)wParam,GetSysColor(COLOR_BTNTEXT));
            if (hPalette)
                SetBkColor((HDC)wParam,PALETTEINDEX(20));
           }
        if (!rc)
            rc = DefDlgProc( hDlg, message, wParam, lParam );
        break;
//  case WM_ERASEBKGND:
//      if (hPalette)
//          {
//           SelectPalette((HDC)wParam,hPalette,0);
//           RealizePalette((HDC)wParam);
//          }
//        rc = DefWindowProc( hDlg, message, wParam, lParam );
//      break;
    case WM_PALETTECHANGED:
        if ((HWND)wParam == hDlg)
            break;
    case WM_QUERYNEWPALETTE:           /* ueberpruefen ob sich ein eintrag in der */
        if (hPalette)                  /* systempalette geaendert hat (durch Realizepalette) */
           {                           /* falls ja, alles neu zeichnen */
            hdc = GetDC(hDlg);
            hPalOrg = SelectPalette(hdc,hPalette,0);
            i = RealizePalette(hdc);
            SelectPalette(hdc,hPalOrg,0);
            ReleaseDC(hDlg,hdc);
            if (i)
               InvalidateRect(hDlg,0,1);
            rc = i;
           }
        break;
    case WM_RBUTTONDOWN:
        PostMessage(hWndMenu,WM_COMMAND,ID_NEWSELWIN,0);
        rc = DefDlgProc( hDlg, message, wParam, lParam );
        break;
    default:
        rc = DefDlgProc( hDlg, message, wParam, lParam );
   }
 return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ GetHelpFileName - Name des Helpfiles ermitteln       บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int GetHelpFileName(HINSTANCE hInst,PSTR pstr,int strsize)
{
 int len;
                             /* aktuellen .EXE-Pointer laden */
  len = GetModuleFileName(hInst,pstr,strsize);
  pstr = pstr + len - 3;
  strcpy(pstr,"HLP");
  return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WndProc - Window Methode des (unsichtbaren) MainWnd  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
LRESULT EXPORTED CALLBACK SIWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam )
{
 PPROFILEENTRY pprofstruc;
 const char * * ppStr;
 LRESULT rc = 0;
// char  str[20];

 switch (message)
   {
    case WM_CREATE:
        break;
    case WM_DESTROY:
        GetHelpFileName(hInst,fnstr,sizeof(fnstr));
        WinHelp(hWnd,fnstr,HELP_QUIT,0);

        for (pprofstruc = proftab,ppStr = pProfString;
             pprofstruc->wert;
             pprofstruc++,ppStr++)
            if (pprofstruc->flags) {
                if (pprofstruc->flags & 1)
                    wsprintf(fnstr,"%u",*(pprofstruc->wert));
                else
                if (pprofstruc->flags & 2)
                    strcpy(fnstr,(PSTR)pprofstruc->wert);

                WritePrivateProfileString(kaptab[pprofstruc->index],
                                          *ppStr,
                                          fnstr,
                                          szIniName);
            }
        UnhookWindowsHook(WH_MSGFILTER,(HOOKPROC)FilterFunc);
        if (hLibCtl3d)
            TryCtl3dRegister(hInst,hLibCtl3d);
        PostQuitMessage(0);
        break;
    case WM_F1DOWN:
        GetHelpFileName(hInst,fnstr,sizeof(fnstr));
        WinHelp(hWnd,fnstr,HELP_INDEX,0);
        break;
    case WM_ERASEBKGND:
        rc = 1;
        break;
    default:
        rc = DefWindowProc( hWnd, message, wParam, lParam );
        break;
   }

 return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ InitFirstInstance - Initialisierung beim 1. Aufruf   บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL InitFirstInstance()
{
    WNDCLASS tWndClass;
//    HDC hDC;
    char  str[20];
//    HPALETTE xPalette;
    XLOGPALETTE palette;
    int   i,j,k;

    if (GetPrivateProfileString("colors","DlgBkgnd","",str,sizeof(str),szIniName))
       {
        if (3 == sscanf(str,"%u,%u,%u",&i,&j,&k))
           {
            hBrush = CreateSolidBrush(RGB(i,j,k));
            palette.palVersion = 0x300;
            palette.palNumEntries = 21;
            GetPaletteEntries((HPALETTE)GetStockObject(DEFAULT_PALETTE),
                              0,
                              20,
                              (LPPALETTEENTRY)&palette.palPalEntry[0].peRed
                             );
            palette.palPalEntry[20].peRed   = i;
            palette.palPalEntry[20].peGreen = j;
            palette.palPalEntry[20].peBlue  = k;
            palette.palPalEntry[20].peFlags = 0;
            hPalette = CreatePalette((LPLOGPALETTE)&palette);
           }
       }

    hFont = CreateFontIndirect(&lf);
                                                /* dialogbox window class */
    tWndClass.style          = 0;
    tWndClass.lpfnWndProc    = (WNDPROC)WIDlgWndProc;
    tWndClass.cbClsExtra     = 0;
    tWndClass.cbWndExtra     = DLGWINDOWEXTRA + sizeof(LPVOID);   /* 4 Bytes extra */
    tWndClass.hInstance      = hInst;
    tWndClass.hIcon          = hIcon;
    tWndClass.hCursor        = hCursor;
    if (hBrush)
        tWndClass.hbrBackground  = hBrush;
    else
        tWndClass.hbrBackground  = (HBRUSH)COLOR_WINDOW + 1;
    tWndClass.lpszMenuName   = NULL;
    tWndClass.lpszClassName  = "WIDlg";

    if (!RegisterClass(&tWndClass))
       return FALSE;

    if (!StatustextRegisterClass(hInst,(LPSTR)0,0))
       return FALSE;
    if (!XListBoxRegisterClass(hInst,(LPSTR)0,0))
       return FALSE;
    if (!HexDumpRegisterClass(hInst,(LPSTR)0,0))
       return FALSE;

    return TRUE;
}
/////////////////////////////////////////////////////////////////////
void FreeMyObjects()
{
    if (hFont) {
        DeleteFont(hFont);
        hFont = 0;
    }
    if (hBrush) {
        DeleteBrush(hBrush);
        hBrush = 0;
    }
    if (hPalette) {
        DeletePalette(hPalette);
        hPalette = 0;
    }
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ InitInstance - Instanz initialisieren                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL InitInstance(HINSTANCE hInstance, HINSTANCE hPrevInstance)
{
  PPROFILEENTRY pprofstruc;
  const char  * *ppStr;
  HDC   hDC;
  WORD  i,xm,ym;

    hInst = hInstance;
    SetErrorMode(SEM_NOOPENFILEERRORBOX);
    LoadStringTab(hInst,strloadtab);
    hCursor     = LoadCursor(0,IDC_ARROW);
    hCursorWait = LoadCursor(0,IDC_WAIT);
                                                     /* wininfo.ini lesen */
    hDC  = GetDC(0);
    xm = GetDeviceCaps(hDC,HORZRES);
    ym = GetDeviceCaps(hDC,VERTRES);
    ReleaseDC(0,hDC);

    for (i=0,pprofstruc = proftab,ppStr=pProfString; pprofstruc->wert; pprofstruc++,ppStr++,i++)
         if (pprofstruc->flags & 2) {
             GetPrivateProfileString(kaptab[pprofstruc->index],
                                     *ppStr,
                                     (LPSTR)pprofstruc->wert,
                                     (LPSTR)pprofstruc->wert,
                                     64,
                                     szIniName);
         } else {
             *(pprofstruc->wert) = GetPrivateProfileInt(kaptab[pprofstruc->index],
                                                    *ppStr,
                                                    *(pprofstruc->wert),
                                                    szIniName);
                              // ueberpruefen ob windowpositionen ok
             if (i < POSENTRIES)
                 if ((i & 1) && (*(pprofstruc->wert) >= ym))
             		*(pprofstruc->wert) = 32;
                 else
                    if (*(pprofstruc->wert) >= xm)
             		*(pprofstruc->wert) = 32;
         }                                           

                                                     /* resourcen laden */
//    hAccelTable = LoadAccelerators(hInstance,MAKEINTRESOURCE(ACCELTAB));
    hIcon       = LoadIcon(hInstance,MAKEINTRESOURCE(ICON1));
    GetStdString(IDS_YES,szYes,sizeof(szYes));
    GetStdString(IDS_NO,szNo,sizeof(szNo));
    lpszHint = GetStdStringAddress(IDS_HINT);
                                                     /* fensterklassen erzeugen */
    if (!hPrevInstance)
       {
        if (!InitFirstInstance())
            return 0;
       }
    else
       {
        GetInstanceData(hPrevInstance,(PBYTE)&hBrush,sizeof(hBrush));
        GetInstanceData(hPrevInstance,(PBYTE)&hPalette,sizeof(hPalette));
        GetInstanceData(hPrevInstance,(PBYTE)&hFont,sizeof(hFont));
       }

    lpFilterFunc = SetWindowsHook(WH_MSGFILTER, (HOOKPROC)FilterFunc);

    hWndMain = CreateWindow("WIDlg",
                        pszWinName,
                        WS_POPUP | WS_VISIBLE | WS_DISABLED,
                        xpos,
                        ypos,
                        0,
                        0,
                        (HWND)NULL,				// no parent
                        (HMENU)NULL,			// use class menu
                        hInstance,				// handle to window instance
                        (LPSTR)NULL				// no params to pass on
                        );
    SetWindowLong(hWndMain,GWL_WNDPROC,(LONG)(WNDPROC)SIWndProc);

    hWndMenu = CreateDialog(hInst,MAKEINTRESOURCE(IDD_MENU),hWndMain,(DLGPROC)MenuDlg);

    if (fLoadCtl3d)
        hLibCtl3d = TryCtl3dRegister(hInst,(HINSTANCE)0);

    SetFocus(hWndMenu);

    return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WinMain - Init Instance + Messageloop                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int cmdShow )
{
    MSG   msg;
    HWND  hWnd;
#if 0
    char szStr[80];
    wsprintf(szStr,"WinInfo: WinVer=%X\r\n",WINVER);
    OutputDebugString(szStr);
#endif
    /* instance global verfuegbar machen */
    if (InitInstance(hInstance,hPrevInstance)) {
        while (GetMessage((LPMSG)&msg, 0, 0, 0)) {
             hWnd = GetParent(msg.hwnd);
             if (hWnd && IsDialogMessage(hWnd,&msg))
                continue;
//             if (!TranslateAccelerator(hWndMain, hAccelTable, &msg))
//                {
                 TranslateMessage(&msg);
                 DispatchMessage(&msg);
//                }
        }
    } else {
        MessageBox(0,"Fehler bei Initialisierung",0,MB_OK);
        msg.wParam = 1;
    }

    if (GetModuleUsage(hInst) == 1) {
        FreeMyObjects();
    }
    return msg.wParam;
}
