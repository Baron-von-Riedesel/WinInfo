
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

//#define strlen lstrlen

BOOL CALLBACK DevCapDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK LocalDlg      (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GlobalDlg     (HWND, UINT, WPARAM, LPARAM);

extern HFONT hFontAlt;

int xGDIpos   = 100;
int yGDIpos   = 100;
HWND hWndGDI       = 0;
HWND hWndGDILocal  = 0;
HWND hWndGDIGlobal = 0;

static GMEMFILTER gmf = {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,
                         TRUE,TRUE,FALSE,FALSE,FALSE,
                         0x00000000,0xFFFFFFFF,GLOBAL_ALL,0,"\0",0
                        };

static PSTR fstr[32] = {0};

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ GDIDlg - GDI Dialog                                  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK GDIDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
 static int BtnTab[] = {ID_GDIDEV,
                        0};
 HMODULE hModule;
 HGLOBAL hSegm;
 HANDLE  hDev;
 HWND    hWnd;
 int i,j;
// int tabpos[4];
 char szStr[140];
 char szPrinter[60];
 char szDriver[9];
 char szPort[64];
 LPSTR lpDev;
 BOOL rc = FALSE;

 switch (message)
    {
    case WM_INITDIALOG:
      LoadTabs(IDUS_51,szStr);
      SendDlgItemMessage(hDlg,
                         ID_LISTBOX1,
                         LB_SETTABSTOPS,
                         *(LPINT)szStr,
                         (LONG)(LPINT)(szStr+2));
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      SetWindowPos(hDlg,0,xGDIpos,yGDIpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_NORMAL);
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_DESTROY:
      hWndGDI = 0;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xGDIpos,&yGDIpos);
      break;
    case WM_COMMAND:
      switch (wParam)
        {
         case ID_REFRESH:
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              ListBox_ResetContent(hWnd);
              i = 0;
              ListBox_AddString(hWnd,"Display\tDISPLAY");
              while (j = EnumDevice(i,
                                    szPrinter,
                                    sizeof(szPrinter),
                                    szDriver,
                                    szPort,
                                    sizeof(szPort)
                                   ))
                 {
                  wsprintf(szStr,
                           "%s\t%s\t%s",
                           (LPSTR)szPrinter,
                           (LPSTR)szDriver,
                           (LPSTR)szPort);
                  ListBox_AddString(hWnd,szStr);
                  i++;
                 }
              break;
         case ID_LISTBOX1:
              switch (HIWORD(lParam))
                 {
                  case LBN_DBLCLK:
                    PostMessage(hDlg,WM_COMMAND,ID_GDIDEV,0);
                    break;
                  case LBN_SELCHANGE:
                    EnableDlgItem(hDlg,
                                  ID_GDIDEV,
                                  LB_ERR != SendMessage((HWND)LOWORD(lParam),LB_GETCURSEL,0,0)
                                 );
                    break;
                  case XLBN_RBUTTONDOWN:
                    TrackPopup(hDlg,BtnTab);
                    break;
                 }
              break;
         case ID_GDIDEV:
              i = ListBox_GetCurSel(hWnd = GetDlgItem(hDlg,ID_LISTBOX1));
              if (i == LB_ERR)
                  break;
              ListBox_GetText(hWnd,i,szStr);
              if (hDev = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,strlen(szStr)+2))
                 {
                  lpDev = (LPSTR)GlobalLock(hDev);
                  _fstrcpy(lpDev,szStr);
                  while (lpDev = _fstrchr(lpDev,9)) *lpDev++ = 0;
                  GlobalUnlock(hDev);
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_DEVCAP),
                                    hDlg,
                                    DevCapDlg,
                                    (LPARAM)(LPVOID)hDev
                                   );
                 }
              break;
         case ID_GDILOCAL:
              if (IsWindow(hWndGDILocal))
                  ShowWindow(hWndGDILocal,SW_RESTORE);
              else {
                  hModule = GetModuleHandle("GDI");
                  hSegm   = GetModuleDgroup(hModule);
                  hWndGDILocal = CreateDialogParam(hInst,
                                       MAKEINTRESOURCE(IDD_LOCAL),
                                       hDlg,
                                       LocalDlg,
                                       (LPARAM)MAKELONG(hSegm,hModule)
                                      );
              }
              break;
         case ID_GDIGLOBAL:
              if (IsWindow(hWndGDIGlobal))
                  ShowWindow(hWndGDIGlobal,SW_RESTORE);
              else {
                  gmf.hOwner = GetModuleHandle("GDI");
                  hWndGDIGlobal = CreateDialogParam(hInst,
                                                    MAKEINTRESOURCE(IDD_GLOBAL),
                                                    hDlg,
                                                    GlobalDlg,
                                                    (LPARAM)(LPVOID)&gmf
                                                   );
              }
              break;
         case ID_DESTROY:
              if ((HWND)lParam == hWndGDILocal)
                  hWndGDILocal = 0;
              else
              if ((HWND)lParam == hWndGDIGlobal)
                  hWndGDIGlobal = 0;
              break;
         case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
        }
      rc = TRUE;
      break;
    }
 return rc;
}
