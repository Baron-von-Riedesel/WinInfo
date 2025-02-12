
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

#define strcpy lstrcpy

extern HWND hWndDEV;
BOOL CALLBACK DevDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam );

int  xDevpos = 100;
int  yDevpos = 100;

// FARPROC lpprocDev = 0;

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ DevDlg - (DOS) Device Dialog                         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK DevDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 char  szStr[80];
 char  str1[20];
 int   y,x;
 HWND hWnd;
 DEVENTRY deventry;
 int   devs;
 BOOL rc;

 rc = FALSE;

 switch (message)
    {
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_REFRESH:
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
//              SendMessage(hWnd,WM_SETREDRAW,0,0);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);

              x = 1;
              y = DeviceFirst(&deventry,1);
              while (y)
                 {
                  if (deventry.attr & 0x8000)
                      strcpy(str1,deventry.name);
                  else
                     {
                      devs = deventry.name[0] + '0';
                      wsprintf(str1,"<%c Block units>",devs);
                     }
                  wsprintf(szStr,
                           "%u\t%s\t%04X:%04X\t%04X",
                           x,
                           (LPSTR)str1,
                           deventry.segm,
                           deventry.offs,
                           deventry.attr
                           );

                  SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)szStr);
                  x++;
                  y = DeviceNext(&deventry);
                 }

//              SendMessage(hWnd,WM_SETREDRAW,1,0);
              SendMessage(hWnd,LB_SETCURSEL,0,0);
              break;
         }
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xDevpos,&yDevpos);
      break;
    case WM_DESTROY:
      hWndDEV = 0;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_INITDIALOG:
      LoadTabs(IDUS_22,szStr);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      SetWindowPos(hDlg,0,xDevpos,yDevpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_SHOWNORMAL);
      rc = TRUE;
      break;
    default:
      break;
    }
 return rc;
}
