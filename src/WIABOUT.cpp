
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "user.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

#define strcpy lstrcpy
#define strcat lstrcat

HWND hWndAbout = 0;

////////////////////////////////////////////////////////
// GetExePath - Pfad des .EXE-Files ermitteln          
////////////////////////////////////////////////////////
int GetExePath(HINSTANCE hInst,PSTR pstr,int strsize)
{
 int len;
  len = GetModuleFileName(hInst,pstr,strsize);
  for (pstr=pstr+len-1;len;len--,pstr--)
      if ((*pstr == '\\') || (*pstr == ':'))
          break;
  *(pstr+1) = 0;
  return 1;
}
////////////////////////////////////////////////////////
// AboutDlg - About Dialog                              
////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK AboutDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 char str1[256];
 HGLOBAL hObj;
 WORD wString;
 BOOL rc = FALSE;

 switch (message) {
 case WM_COMMAND:
     switch (wParam) {
     case IDOK:
     case IDCANCEL:
         PostMessage(hDlg,WM_CLOSE,0,0);
         break;
     }
     rc = TRUE;
     break;
 case WM_DESTROY:
     hWndAbout = 0;
     break;
 case WM_CLOSE:
     EnableWindow(hWndMenu,1);
     //   PostMessage(hWndMenu,WM_COMMAND,ID_DESTROY,(LPARAM)hDlg);
     DestroyWindow(hDlg);
     rc = TRUE;
     break;
 case WM_INITDIALOG:
     EnableWindow(hWndMenu,0);
     wString = IDU_ABOUT3;
     hObj = LoadUserResource(hInst,
                             MAKEINTRESOURCE(wString),
                             MAKEINTRESOURCE(RTI_USER),
                             (LPBYTE)0,
                             0
                            );
     SetDlgItemText(hDlg,ID_ABOUTTEXT,(LPSTR)MAKELONG( 0, hObj ));
     LoadString(hInst,IDS_INFOSTR,str1,sizeof(str1));
     SetWindowText(hDlg,str1);
     ShowWindow(hDlg,SW_SHOWNORMAL);
     rc = TRUE;
     break;
 }
 return rc;
}
