
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dosx.h"
#include "wininfo.h"
#include "wininfox.h"

BOOL  WINAPI CheckMessage(MSG *,WORD,HWND);

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT hFontAlt;

BOOL CALLBACK ViewDlg (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK EnvironmentDlg (HWND, UINT, WPARAM, LPARAM);

static const char szcFStr1X[] = "%04X";
static const char szcFStr2X[] = "%04X:%04X";
static const char szcFStrS[]  = "%s";

static DDENTRY pspstrtab[] = {
        0,offsetof(PSP,iv22),        szcFStr2X,DDT_DWORD,
        0,offsetof(PSP,iv23),        szcFStr2X,DDT_DWORD,
        0,offsetof(PSP,iv24),        szcFStr2X,DDT_DWORD,
        0,offsetof(PSP,parent),      szcFStr1X,DDT_WORD,
        0,offsetof(PSP,environment), szcFStr1X,DDT_WORD,
        0,offsetof(PSP,dwStack),     szcFStr2X,DDT_DWORD,
        0,offsetof(PSP,wHdlTabSize), szcFStr1X,DDT_WORD,
        0,offsetof(PSP,wHdlTabAddr), szcFStr2X,DDT_DWORD,
        0,offsetof(PSP,szCmdline),   szcFStrS,DDT_FUNCTION,
        (PSTR)0};

static STRLOADENTRY CODESEG strloadtab[] = {
        &pspstrtab[ 0].name, IDS_XPSP00,
        &pspstrtab[ 1].name, IDS_XPSP01,
        &pspstrtab[ 2].name, IDS_XPSP02,
        &pspstrtab[ 3].name, IDS_XPSP03,
        &pspstrtab[ 4].name, IDS_XPSP04,
        &pspstrtab[ 5].name, IDS_XPSP05,
        &pspstrtab[ 6].name, IDS_XPSP06,
        &pspstrtab[ 7].name, IDS_XPSP07,
        &pspstrtab[ 8].name, IDS_XPSP08,
        (PSTR *)(0)};

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ PSPDlg - PSP Dialog                                  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK PSPDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = { ID_SUBDLG1,0};
 DDENTRY * pddentry;
 char  szStr[150];
 char  szStr2[128];
 PSP FAR * lpPSP;
 LPSTR lpStr2,lpStr3;
 int  i;
 HWND hWnd;
 HGLOBAL hPSP;
 HEXDUMPPARM hdp;
 int  tabpos[1];
 BOOL rc = FALSE;

 switch (message)
    {
    case WM_COMMAND:
      switch (wParam)
         {
         case ID_REFRESH:
              hPSP = (HGLOBAL)GetWindowWord(hDlg,DWL_USER);
              lpPSP = (PSP FAR *)MAKELONG(0,hPSP);
			  hWnd = GetDlgItem(hDlg,ID_LISTBOX1);

              tabpos[0] = 80;
              ListBox_SetTabStops(hWnd,1,&tabpos);

              pddentry = pspstrtab;
              ListBox_ResetContent(hWnd);
              while (pddentry->name)
                 {
                  if (pddentry->typ == DDT_FUNCTION)
                     {
                      lpStr2 = lpPSP->szCmdline;
                      lpStr3 = szStr2;
                      i = __min(lpPSP->cCmdlineLength,127);
                      for (;i;i--,lpStr2++,lpStr3++)
                        {
                         if (!(*lpStr2))
                            *lpStr3 = '.';
                         else
                            *lpStr3 = *lpStr2;
                        }
                      *lpStr3 = 0;
                     }
                  else
                      xsprintf(pddentry,szStr2,(LPSTR)lpPSP);

                  wsprintf(szStr,"%s\t%s",
                           (LPSTR)pddentry->name,
                           (LPSTR)szStr2
                          );
                  ListBox_AddString(hWnd,szStr);
                  pddentry++;
                 }
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                {
                  case XLBN_RBUTTONDOWN:
                    TrackPopup(hDlg,BtnTab);
                    break;
                }
              break;
         case ID_SUBDLG1:
              hdp.dwOffset = 0;
              hdp.hGlobal  = (HGLOBAL)GetWindowWord(hDlg,DWL_USER);
              hdp.dwLength = 0;
              hdp.wType    = 0;
              if (hWnd = CreateDialogParam( hInst,
                                            MAKEINTRESOURCE(IDD_VIEW),
                                            GetParent(hDlg),
                                            ViewDlg,
                                            (LPARAM)(LPVOID)&hdp
                                          ))
                 {
                  wsprintf(szStr,"PSP %X",LOWORD(lParam));
                  SetWindowText(hWnd,szStr);
                 }
              break;
         case ID_SUBDLG2:
              hPSP = (HGLOBAL)GetWindowWord(hDlg,DWL_USER);
              lpPSP = (PSP FAR *)MAKELONG(0,hPSP);
              if (IsValidAddress(lpPSP))
                  if (lpPSP->KB == 0x20CD && lpPSP->environment) {
                      CreateDialogParam(hInst,MAKEINTRESOURCE(IDD_ENVIRON),GetParent(hDlg),
                                         EnvironmentDlg,(LPARAM)lpPSP->environment);
                  }
              break;
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         }
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_INITDIALOG:
      SetWindowWord(hDlg,DWL_USER,LOWORD(lParam));

      if (!pspstrtab[0].name)
          LoadStringTab(hInst,strloadtab);

      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      wsprintf(szStr,"PSP %X",LOWORD(lParam));
      SetWindowText(hDlg,szStr);
      ShowWindow(hDlg,SW_NORMAL);
      rc = TRUE;
      break;
    default:
      break;
    }

 return rc;
}
