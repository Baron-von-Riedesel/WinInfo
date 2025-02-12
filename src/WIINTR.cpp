
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT   hFontAlt;
extern BOOL    fGetExc01;

BOOL CALLBACK UnassembleDlg  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Unassemble32Dlg(HWND, UINT, WPARAM, LPARAM);
extern HWND    hWndDPMIInt;

int  xIntrpos = 100;
int  yIntrpos = 100;

static BOOL fGetExc01 = FALSE;

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ IntrDlg - Interrupt Dialog                           บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK IntrDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_UNASSINTR,
                        0};
 static WORD wLastID = 0;
 char  szStr0[80];
 char  szStr1[40];
 GLOBALENTRY ge;
// LPSTR lpstr;
 int   i,y;
 DWORD   rv;
 DWORD   ev;
 FARPROC inthdl;
// HMODULE hModule;
 FARPROC paddr;
 HWND  hWnd;
// int   tabpos[3];
// IDTENTRY idtentry;
// FWORD faddr;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      SendDlgItemMessage(hDlg,ID_GETEXC01,BM_SETCHECK,fGetExc01,0);
      LoadTabs(IDUS_32,szStr0);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)szStr0,(LONG)(LPINT)(szStr0+2));
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      SendDlgItemMessage(hDlg,ID_LISTRMINT,LB_SETTABSTOPS,*(LPINT)szStr0,(LONG)(LPINT)(szStr0+2));
      SendDlgItemMessage(hDlg,ID_LISTRMINT,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      SendDlgItemMessage(hDlg,ID_LISTEXC, LB_SETTABSTOPS,*(LPINT)szStr0,(LONG)(LPINT)(szStr0+2));
      SendDlgItemMessage(hDlg,ID_LISTEXC,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      LoadTabs(IDUS_33,szStr0);
      SendDlgItemMessage(hDlg,ID_STATINTR,ST_SETTABSTOPS,*(LPINT)szStr0,(LONG)(LPINT)(szStr0+2));
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      SetWindowPos(hDlg,0,xIntrpos,yIntrpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_NORMAL);
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xIntrpos,&yIntrpos);
      break;
    case WM_DESTROY:
      hWndDPMIInt = 0;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case IDOK:
              PostMessage(hDlg,WM_COMMAND,ID_UNASSINTR,0);
              break;
         case ID_GETEXC01:
              fGetExc01 = 1 - fGetExc01;
              break;
         case ID_SNAPSHOT:
              WriteLBSnapShot(0,hDlg,ID_LISTBOX1,"IPM",3);
              WriteLBSnapShot(0,hDlg,ID_LISTRMINT,"IRM",1);
              WriteLBSnapShot(0,hDlg,ID_LISTEXC,"EXC",1);
              break;
         case ID_UNASSINTR:
              hWnd = GetDlgItem(hDlg,wLastID);
              y =  ListBox_GetCurSel(hWnd);
              if (y == LB_ERR)
                 {
                  MessageBeep(0);
                  break;
                 }

              paddr = (FARPROC)ListBox_GetItemData(hWnd,y);
              if (wLastID == ID_LISTRMINT)
                 {
                  y = DPMISegToDescriptor(HIWORD(paddr));
                  paddr = (FARPROC)MK_FP(y,LOWORD(paddr));
                 }
              if (IsValidAddress(paddr) && IsReadableSelector(HIWORD(paddr)))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_UNASSEM),
                                    hDlg,
                                    UnassembleDlg,
                                    (LPARAM)paddr
                                   );
              else
                  MessageBeep(0);

              break;
         case ID_REFRESH:
              SetCursor(hCursorWait);

              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              ListBox_ResetContent(hWnd);
              for (i=0;i<256;i++)
                 {
                  inthdl = (FARPROC)DPMIGetPMIntVector(i);
//                  inthdl = (FARPROC)_dos_getvect(i);
                  szStr1[0] = 0;
                  if (IsPresent(HIWORD(inthdl)))
                     {
                      ge.dwSize = sizeof(GLOBALENTRY);
                      if (GlobalEntryHandle(&ge,(HGLOBAL)HIWORD(inthdl)))
                          GetModuleName((HMODULE)ge.hOwner,szStr1,sizeof(szStr1));
                     }
                  wsprintf(szStr0,
                           "%X\t%X:%X\t%s",
                           i,
                           HIWORD(inthdl),
                           LOWORD(inthdl),
                           (LPSTR)szStr1
                           );
                  y = ListBox_AddString(hWnd,szStr0);
                  ListBox_SetItemData(hWnd,y,(LPARAM)inthdl);
                 }

              hWnd = GetDlgItem(hDlg,ID_LISTRMINT);
              ListBox_ResetContent(hWnd);
              for (i=0;i<256;i++)
                 {
                  rv = DPMIGetRMIntVector(i);
                  wsprintf(szStr0,
                           "%X\t%X:%X",
                           i,
                           HIWORD(rv),
                           LOWORD(rv)
                           );
                  y = ListBox_AddString(hWnd,szStr0);
                  ListBox_SetItemData(hWnd,y,(LPARAM)rv);
                 }


              hWnd = GetDlgItem(hDlg,ID_LISTEXC);
              ListBox_ResetContent(hWnd);
              for (i=0;i<32;i++)
                 {
                  if ((i == 1) && (!fGetExc01))
                      continue;

                  ev = DPMIGetExcVector(i);
                  szStr1[0] = 0;
                  if (IsPresent(HIWORD(ev)))
                     {
                      ge.dwSize = sizeof(GLOBALENTRY);
                      if (GlobalEntryHandle(&ge,(HGLOBAL)HIWORD(ev)))
                          GetModuleName((HMODULE)ge.hOwner,szStr1,sizeof(szStr1));
                     }
                  wsprintf(szStr0,
                           "%X\t%X:%X\t%s",
                           i,
                           HIWORD(ev),
                           LOWORD(ev),
                           (LPSTR)szStr1
                          );
                  y = ListBox_AddString(hWnd,szStr0);
                  ListBox_SetItemData(hWnd,y,(LPARAM)ev);
                 }

              wLastID = 0;
              EnableDlgItem(hDlg,ID_UNASSINTR,0);
              SetCursor(hCursor);
              break;
         case ID_LISTBOX1:
         case ID_LISTEXC:
         case ID_LISTRMINT:
              switch(HIWORD(lParam))
                 {
                  case LBN_SELCHANGE:
                     EnableDlgItem(hDlg,ID_UNASSINTR,1);
                     wLastID = wParam;
                     break;
                  case LBN_DBLCLK:
                     PostMessage(hDlg,WM_COMMAND,ID_UNASSINTR,0);
                     break;
                  case XLBN_RBUTTONDOWN:
                     TrackPopup(hDlg,BtnTab);
                     break;
                 }
              break;
         }
      rc = TRUE;
      break;
    default:
      break;
   }
 return rc;
}
