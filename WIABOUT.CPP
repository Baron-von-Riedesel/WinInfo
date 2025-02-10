
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

//extern WORD wRegistered;
extern BYTE fNewReg;
extern char szPassword[40];
extern char szIniName[];

HWND hWndAbout = 0;

#if 0
//#define COPYRIGHTCHECK() {_asm {_asm cli _asm xor ax,ax _asm mov ss,ax}}
////////////////////////////////////////////////////////
// RegisterDlg - registrierdialog                       
////////////////////////////////////////////////////////
BOOL CALLBACK RegisterDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 BOOL rc = FALSE;

    switch (message)
      {
       case WM_COMMAND:
          switch (wParam)
            {
            case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
            case IDOK:
              GetDlgItemText(hDlg,ID_CONTROL2,szPassword,sizeof(szPassword));
              PostMessage(hDlg,WM_CLOSE,0,0);
              PostMessage(GetParent(hDlg),WM_CLOSE,0,0);
              fNewReg = 1;
              wRegistered = 0xFFFF;
              break;
            }
          rc = TRUE;
          break;
       case WM_CLOSE:
          EnableWindow(GetParent(hDlg),1);
          DestroyWindow(hDlg);
          rc = TRUE;
          break;
       case WM_INITDIALOG:
          SendDlgItemMessage(hDlg,ID_CONTROL2,EM_LIMITTEXT,39,0);
          SetDlgItemText(hDlg,ID_CONTROL2,szPassword);
          EnableWindow(GetParent(hDlg),0);
          ShowWindow(hDlg,SW_SHOWNORMAL);
          rc = TRUE;
          break;
      }
 return rc;
}
#endif
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
BOOL CALLBACK AboutDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 WORD i,k;
 char str[128];
 char str1[256];
 HGLOBAL hObj;
 WORD wString;
 LPWORD lpword;
 int  hFile;
 BOOL rc = FALSE;

 switch (message) {
 case WM_COMMAND:
     switch (wParam) {
     case IDOK:
#if 0
         hObj = LoadUserResource(hInst,
                                 MAKEINTRESOURCE(IDU_REGFRM),
                                 MAKEINTRESOURCE(RTI_USER),
                                 (LPBYTE)0,
                                 0
                                );
         if (hObj) {
             strcpy(str1,"NOTEPAD.EXE ");
             GetExePath(hInst,str1+12,sizeof(str1)-12);
             strcat(str1,"WININFO.REG");
             if ((hFile = _lopen(str1+12,OF_READ)) == -1)
                 if ((hFile = _lcreat(str1+12,0)) == -1) {
                     wsprintf(str,"Error Creating %s",(LPSTR)str1+12);
                     CreateMessage(hDlg,str,0,MB_OK);
                 } else {
                     lpword = (LPWORD)MAKELONG(0,hObj);
                     i = *lpword++;
                     k = *lpword++;
                     Decrypt((LPBYTE)lpword,i,k);
                     _lwrite(hFile,(LPBYTE)lpword,i);
                 }
             if (hFile != -1) {
                 _lclose(hFile);
                 SendMessage(hDlg,WM_CLOSE,1,0);
                 SendMessage(hWndMenu,WM_SYSCOMMAND,SC_MINIMIZE,0);
                 WinExec(str1,SW_SHOWMAXIMIZED);
             }
         } else
             CreateMessage(hDlg,"Error Reading Resource",0,MB_OK);
#else
         PostMessage(hDlg,WM_CLOSE,0,0);
#endif
         break;
#if 0
     case ID_REGISTER:
         CreateDialog(hInst,
                      MAKEINTRESOURCE(IDD_REGISTER),
                      hDlg,
                      RegisterDlg
                     );
         break;
#endif
     case IDCANCEL:                   /* "Weiter" bei unregistrierter Version */
#if 0
         MessageBeep(0);
         PostMessage(hDlg,WM_CLOSE,0,0);
     case ID_CANCEL1:                 /* "Beenden" bei registrierter Version */
#endif
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
     //if (wRegistered == 1)
     wString = IDU_ABOUT3;
     //else
     //    wString = IDU_ABOUT2;
     hObj = LoadUserResource(hInst,
                             MAKEINTRESOURCE(wString),
                             MAKEINTRESOURCE(RTI_USER),
                             (LPBYTE)0,
                             0
                            );
#if 0
     lpword = (LPWORD)MAKELONG(0,hObj);
     if (!lpword) COPYRIGHTCHECK();
     i = *lpword;
     *lpword = 0;
     lpword++;
     k = *lpword++;
     Decrypt((LPBYTE)lpword,i,k);
#endif
     //if (wRegistered == 1) {
     //wsprintf(str1,(LPCSTR)lpword,(LPSTR)szPassword);
     SetDlgItemText(hDlg,ID_ABOUTTEXT,(LPSTR)MAKELONG( 0, hObj ));
     // } else
     //  SetDlgItemText(hDlg,ID_ABOUTTEXT,(LPCSTR)lpword);

     //UnlockResource(hObj);
     //FreeResource(hObj);
     LoadString(hInst,IDS_INFOSTR,str1,sizeof(str1));
     SetWindowText(hDlg,str1);
     ShowWindow(hDlg,SW_SHOWNORMAL);
     rc = TRUE;
     break;
 }
 return rc;
}
