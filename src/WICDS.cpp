
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

#define strcat lstrcat
#define strcpy lstrcpy
#define strlen lstrlen

extern HWND hWndCDS;

int  xCDSpos = 100;
int  yCDSpos = 100;

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ CDSDlg - (DOS) CDS Dialog                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK CDSDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 char  szStr[80];
 int   y,x;
 HWND hWnd;
 CDSENTRY cdsentry;
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
                  case LBN_SELCHANGE:
                     hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
                     x = (WORD)SendMessage(hWnd,LB_GETCURSEL,0,0);
                     if (x == LB_ERR)
                        break;
                     cdsentry.wFlags = (WORD)SendMessage(hWnd,LB_GETITEMDATA,x,0);
                     strcpy(szStr,"Flags\t");
                     if (cdsentry.flags.network)
                         strcat(szStr,"network,");
                     if (cdsentry.flags.physical)
                         strcat(szStr,"physical,");
                     if (cdsentry.flags.join)
                         strcat(szStr,"join,");
                     if (cdsentry.flags.subst)
                         strcat(szStr,"subst,");
                     x = strlen(szStr) - 1;
                     if (szStr[x] == ',')
                         szStr[x] = 0;
                     strcat(szStr,"\t");
                     SetDlgItemText(hDlg,ID_STATUS1,szStr);
                     break;
                 }
              break;
         case ID_REFRESH:
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);

              y = CDSFirst(&cdsentry);
              while (y)
                 {
                  wsprintf(szStr,
                           "%c:\t%04X:%04X\t%04X\t%04X:%04X\t%s",
                           cdsentry.bDrive + 'A' - 1,
                           cdsentry.wSegment,
                           LOWORD(cdsentry.lpEntry),
                           cdsentry.wFlags,
                           HIWORD(cdsentry.lpDPB),
                           LOWORD(cdsentry.lpDPB),
                           (LPSTR)cdsentry.szPath
                           );

                  y = SendMessage(hWnd,LB_ADDSTRING,0,(LPARAM)(LPSTR)szStr);
                  SendMessage(hWnd,LB_SETITEMDATA,y,(LPARAM)cdsentry.wFlags);
                  y = CDSNext(&cdsentry);
                 }
              SendMessage(hWnd,LB_SETCURSEL,0,0);
              break;
         }
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xCDSpos,&yCDSpos);
      break;
    case WM_DESTROY:
      hWndCDS = 0;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_INITDIALOG:
      LoadTabs(IDUS_24,szStr);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
      LoadTabs(IDUS_25,szStr);
      SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      SetWindowPos(hDlg,0,xCDSpos,yCDSpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_SHOWNORMAL);
      rc = TRUE;
      break;
    default:
      break;
    }

 return rc;
}
