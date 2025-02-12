
// zur zeit nicht verwendet

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT   hFontAlt;

BOOL EXPORTED CALLBACK ModulEntriesDlg(HWND,UINT,WPARAM,LPARAM);
BOOL EXPORTED CALLBACK UnassembleDlg  (HWND,UINT,WPARAM,LPARAM);
HMODULE WINAPI CheckModul(HWND,MODULEENTRY *,PINT,WORD);

static int idtab[] = {ID_SUBDLG1,ID_REFRESH,0};
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ModulEntriesDlg                                      บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK ModulEntriesDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_SUBDLG1,0};
// int  tabpos[4];
 int  x;
 char str[128];
 char str1[60];
 char str2[20];
 WORD y;
 HWND hWnd;
 MODULEENTRY moduleentry;
 ENTRYENTRY entryentry;
 FARPROC paddr;
 HMODULE hModule;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      LoadTabs(IDUS_57,str);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      SetWindowLong(hDlg,DLGWINDOWEXTRA,lParam); // parameter hModule
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      ShowWindow(hDlg,SW_SHOWNORMAL);
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_COMMAND:
      switch (wParam)
        {
         case ID_SNAPSHOT:
              WriteLBSnapShot(0,
                              hDlg,
                              ID_LISTBOX1,
                              "ENT",
                              WLBSS_CLOSE | WLBSS_CLIPBOARD | WLBSS_MESSAGE);
              break;
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_SUBDLG1:  /* disassembler */
              if (!(hModule = CheckModul(hDlg,&moduleentry,idtab,1)))
                  break;
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              y =  (WORD)SendMessage(hWnd,LB_GETCURSEL,0,0);
              if (y == LB_ERR)
                  paddr = 0;
              else
                  paddr = (FARPROC)SendMessage(hWnd,LB_GETITEMDATA,y,0);
              if (paddr)
                 {
//                  GetCodeHandle(paddr);
                  if (IsValidAddress(paddr))
                     {
                      CreateDialogParam(hInst,
                                        MAKEINTRESOURCE(IDD_UNASSEM),
                                        hDlg,
                                        UnassembleDlg,
                                        (DWORD)paddr
                                       );
                      break;
                     }
                 }
              MessageBeep(0);
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                 {
                  case LBN_SELCHANGE:
                     EnableWindow(GetDlgItem(hDlg,ID_SUBDLG1),1);
                     break;
                  case LBN_DBLCLK:
                     PostMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                     break;
                  case XLBN_RBUTTONDOWN:
                     TrackPopup(hDlg,BtnTab);
                     break;
                 }
              break;
         case ID_REFRESH:
              if (!(hModule = CheckModul(hDlg,&moduleentry,idtab,1)))
                  break;
              SetCursor(hCursorWait);
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);
              GetWindowText(hDlg,str1,sizeof(str1));
              GetModuleName(hModule,str2,sizeof(str2));
              wsprintf(str,str1,(LPSTR)str2);
              SetWindowText(hDlg,str);
              x = GetFirstEntry(hModule,&entryentry);
              while (x)
                 {
                  paddr = (FARPROC)GetEntryAddress(hModule,entryentry.id);
                  str1[0] = 0;
                  GetProcedureName(hModule,entryentry.id,str1,sizeof(str1));
                  wsprintf(str,"%04X\t%u\t%X:%X\t%02X\t%s",
                           LOWORD(entryentry.lpAdr),
                           entryentry.id,
                           entryentry.segNum,
                           entryentry.offset,
                           entryentry.flags,
                           (LPSTR)str1
                          );
                  x = (int)SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                  SendMessage(hWnd,LB_SETITEMDATA,x,(LONG)paddr);
                  x = GetNextEntry(hModule,&entryentry);
                 }
              EnableWindow(GetDlgItem(hDlg,ID_SUBDLG1),0);
              SetCursor(hCursor);
              break;
        }
      rc = TRUE;
      break;
   }
 return rc;
}
