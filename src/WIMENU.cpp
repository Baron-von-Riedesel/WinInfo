
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

#define WM_F1DOWN   0x401

extern "C" {
WORD FAR PASCAL CheckRegister(LPSTR,LPSTR);
};

char   szPassword[40] = {0};
extern char szVersion[];
extern BYTE fNewReg;
extern HFONT hFont;
extern char szIniName[];
extern HFONT hFontAlt;

extern HWND hWndTask;
extern HWND hWndModul;
extern HWND hWndGlobal;
extern HWND hWndUser;
extern HWND hWndGDI;
extern HWND hWndDOS;
extern HWND hWndDPMI;
extern HWND hWndSystem;
extern HWND hWndAbout;

BOOL CALLBACK MenuDlg       (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GlobalDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlg      (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK UserDlg       (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GDIDlg        (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TaskDlg       (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ModulDlg      (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK SysDlg        (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DosDlg        (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DPMIDlg       (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MyDlgWndProc(HWND, UINT, WPARAM, LPARAM);

typedef struct tagDLGENTRY {
    int     cmdid;
    DLGPROC tProc;
    int     resid;
    HWND    * phWnd;
   }  DLGENTRY;

static DLGENTRY dlgtab[]  = {
                   ID_TASK,      TaskDlg,       IDD_TASK,   &hWndTask,
                   ID_MODUL,     ModulDlg,      IDD_MODUL,  &hWndModul,
                   ID_GLOBAL,    GlobalDlg,     IDD_GLOBAL, &hWndGlobal,
                   ID_USER,      UserDlg,       IDD_USER,   &hWndUser,
                   ID_GDI,       GDIDlg,        IDD_GDI,    &hWndGDI,
                   ID_DOS,       DosDlg,        IDD_DOS,    &hWndDOS,
                   ID_DPMI,      DPMIDlg,       IDD_DPMI,   &hWndDPMI,
                   ID_SYSTEM,    SysDlg,        IDD_SYSTEM, &hWndSystem,
                   ID_ABOUT,     AboutDlg,      IDD_ABOUT,  &hWndAbout,
                   0,            0,             0,          0};

static int maxwndtxt;
static int windex;
static BYTE fMenu;
static char szFStrDWd[] = {"%lu %*c"};

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WindowCB1 - Window Callback                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK WindowCB1(HWND hWnd, LPARAM lparam)
{
 char str[160];
 int x;
 HWND hDlg;

  if (IsWindowVisible(hWnd) && IsWindowEnabled(hWnd))
     {
      if (GetWindowLong(hWnd,GWL_STYLE) & WS_CAPTION)
         {
          if (HIWORD(lparam))
              hWnd = GetDlgItem(hWnd,HIWORD(lparam));
          x = GetWindowTextLength(hWnd);
//          GetWindowText(hWnd,str,sizeof(str));
          maxwndtxt = max(maxwndtxt,x);
          if (AppendMenu((HMENU)LOWORD(lparam),
                     MF_OWNERDRAW | MF_ENABLED | MF_BYPOSITION,
//                     MF_STRING | MF_ENABLED | MF_BYPOSITION,
                     windex,
                     (LPSTR)MAKELONG(hWnd,HIWORD(lparam))))
//                     (LPSTR)str))

              windex++;
          else
              fMenu = 0;
         }
     }

 return TRUE;
}

void CharFilter(char * pZiel, char * pQuelle, char c)
{
	for (;*pQuelle;pQuelle++)
		if (*pQuelle != c)
			*pZiel++ = *pQuelle;
	*pZiel = 0;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ MenuDlg - WinInfo Main Menu                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK MenuDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
    static WORD id;
    static HWND sWnd;
    int i;
    HTASK hTask;
    HMENU hPopupMenu;
    DWORD dwID;
    char str[80];
    HGLOBAL hSel;
    LPMEASUREITEMSTRUCT lpmi;
    LPDRAWITEMSTRUCT lpdi;
    LOGFONT lf;
    DLGENTRY *pdlgentry;
    POINT point;
    BOOL rc = FALSE;
    HWND hWnd;

    switch (message)
    {
    case WM_INITDIALOG:
        if (!hFontAlt)
            if (hFontAlt = GetWindowFont(hDlg))
               {
                GetObject(hFontAlt,sizeof(LOGFONT),&lf);
                lf.lfWeight = 1100 - lf.lfWeight;
                hFontAlt = CreateFontIndirect(&lf);
               }
        SetWindowPos(hDlg,0,xpos,ypos,0,0,SWP_NOSIZE | SWP_NOZORDER);
        ShowWindow(hDlg,SW_SHOWNORMAL);
        rc = TRUE;
        break;
    case WM_CLOSE:
        DestroyWindow(hDlg);
        DestroyWindow(hWndMain);
        rc = TRUE;
        break;
    case WM_DESTROY:
        if (hFontAlt)
            DeleteFont(hFontAlt);
        break;
    case WM_MOVE:
        SaveWindowPos(hDlg,&xpos,&ypos);
        break;
    case WM_MEASUREITEM:
        lpmi = (LPMEASUREITEMSTRUCT)lParam;
        lpmi->itemWidth = (maxwndtxt+2) * 8;
        lpmi->itemHeight = 16;
        rc = TRUE;
        break;
    case WM_DRAWITEM:
        lpdi = (LPDRAWITEMSTRUCT)lParam;
        hWnd = (HWND)LOWORD(lpdi->itemData);
        sWnd = hWnd;
        id   = HIWORD(lpdi->itemData);
        GetWindowText(hWnd,str+1,sizeof(str)-1);
        if (id)
           CharFilter(str+1,str+1,'&');
        str[0] = ' ';
        SetBkMode(lpdi->hDC,OPAQUE);
        if (lpdi->itemState == ODS_SELECTED)
           {
            SetBkColor(lpdi->hDC,GetSysColor(COLOR_HIGHLIGHT));
            SetTextColor(lpdi->hDC,GetSysColor(COLOR_HIGHLIGHTTEXT));
           }
        if (hFont)
            SelectObject(lpdi->hDC,hFont);
        ExtTextOut(lpdi->hDC,
                   lpdi->rcItem.left,
                   lpdi->rcItem.top,
                   ETO_CLIPPED | ETO_OPAQUE,
                   &lpdi->rcItem,
                   str,
                   strlen(str),
                   0 );
        rc = TRUE;
        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_KEYMENU)
           {
            PostMessage(hDlg,WM_COMMAND,ID_NEWSELWIN,0);
            rc = TRUE;
           }
        else
        if ((wParam >= ID_OWNERMENU) && (wParam < windex))
            PostMessage(hDlg,WM_COMMAND,wParam,lParam);
        break;
    case WM_COMMAND:
        switch (wParam)
        {
        case ID_TASK:
        case ID_MODUL:
        case ID_GLOBAL:
        case ID_USER:
        case ID_GDI:
        case ID_DOS:
        case ID_DPMI:
        case ID_SYSTEM:
        case ID_ABOUT:
            for (pdlgentry = dlgtab;pdlgentry->cmdid;pdlgentry++)
                if (wParam == pdlgentry->cmdid) {
                    switch(HIWORD(lParam)) {
                    case BN_CLICKED:
                        if (*pdlgentry->phWnd) {
                            ShowWindow(*pdlgentry->phWnd,SW_RESTORE);
                            SetActiveWindow(*pdlgentry->phWnd);
                        } else {
                            *pdlgentry->phWnd = CreateDialog(hInst,
                                                             MAKEINTRESOURCE(pdlgentry->resid),
                                                             hWndMain,
                                                             (DLGPROC)pdlgentry->tProc);
                        }
                    }
                    break;
                }
            break;
        case ID_READIDS:
            GetPrivateProfileString("strings","regid","???",szPassword,sizeof(szPassword),szIniName);
            break;
        case ID_WRITEIDS:
            fNewReg = 0;
            WritePrivateProfileString("strings","regid",szPassword,szIniName);
            break;
        case ID_NEWSELWIN:
            hTask = GetCurrentTask();
            windex = ID_OWNERMENU;
            maxwndtxt = 0;
            if (hPopupMenu = CreatePopupMenu())
            {
                fMenu = TRUE;
                EnumTaskWindows(hTask,WindowCB1,MAKELONG(hPopupMenu,0));
                if (fMenu)
                    WindowCB1(hDlg,MAKELONG(hPopupMenu,ID_EXIT));
                else
                    CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRMNU1),0,MB_OK);
                GetCursorPos(&point);
                TrackPopupMenu(hPopupMenu,
                               TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                               point.x,
                               point.y,
                               0,
                               hDlg,
                               0 );
                DestroyMenu(hPopupMenu);
            }
            break;
        case ID_HELP:
            PostMessage(hWndMain,WM_F1DOWN,0,0);
            break;
        case IDCANCEL:
        case ID_EXIT:
            PostMessage(hDlg,WM_CLOSE,0,0);
            break;
        default:
            if ((wParam >= ID_OWNERMENU) && (wParam < windex))
            {
                if (id)
                    PostMessage(hDlg,WM_COMMAND,id,0);
                else
                {
                    hWnd = sWnd;
                    if (IsIconic(hWnd))
                        PostMessage(hWnd,WM_SYSCOMMAND,SC_RESTORE,0);
                    SetActiveWindow(hWnd);
                }
            }
        }         // end switch (wParam)
        rc = TRUE;
        break;      // end case WM_COMMAND
    }
    return rc;
}
