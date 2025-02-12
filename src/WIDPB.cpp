
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dosx.h"
#include "wininfo.h"
#include "wininfox.h"

extern HWND hWndDPB;

int  xDPBpos = 100;
int  yDPBpos = 100;

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ DPBDlg - (DOS) DPB Dialog                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK DPBDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 char  szStr[80];
 int   y,i;
 HWND hWnd;
 DPBENTRY dpbentry;
 DWORD dwCapacity;
 BOOL rc = FALSE;

 switch (message)
    {
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                 {
                 }
              break;
         case ID_REFRESH:
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);

              y = DPBFirst(&dpbentry);
              while (y)
                 {
                  if (dpbentry.bSectorCluster != 254)
                     {
                      dwCapacity = (DWORD)(dpbentry.wMaxCluster-1)*(dpbentry.bSectorCluster+1);
                      dwCapacity = dwCapacity * dpbentry.wBytesSector / 1024;
                     }
                  else
                      dwCapacity = 0;
                  wsprintf(szStr,
                           "%c:\t%04X:%04X\t%04X:%04X\t%u\t%u\t%u\t%u\t%lu\t%u",
                           dpbentry.bDrive + 'A',
                           HIWORD(dpbentry.dwRMAdress),
                           LOWORD(dpbentry.dwRMAdress),
                           HIWORD(dpbentry.dwDevDriver),
                           LOWORD(dpbentry.dwDevDriver),
                           dpbentry.wBytesSector,
                           dpbentry.bSectorCluster+1,
                           dpbentry.bNumFATs,
                           dpbentry.wMaxCluster-1,
                           dwCapacity,
                           dpbentry.wFreeClusters
                          );

                  y = SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)szStr);
                  y = DPBNext(&dpbentry);
                 }
              SendMessage(hWnd,LB_SETCURSEL,0,0);
              break;
         }
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xDPBpos,&yDPBpos);
      break;
    case WM_DESTROY:
      hWndDPB = 0;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_INITDIALOG:
      LoadTabs(IDUS_26,szStr);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
//    LoadTabs(IDUS_27,szStr);
//    SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      SetWindowPos(hDlg,0,xDPBpos,yDPBpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_SHOWNORMAL);
      rc = TRUE;
      break;
    default:
      break;
    }
 return rc;
}
