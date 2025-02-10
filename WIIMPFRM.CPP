
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

#define strlen lstrlen
#define strcpy lstrcpy
#define strcat lstrcat

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT   hFontAlt;
extern BYTE fShow;

BOOL EXPORTED CALLBACK UnassembleDlg  (HWND,UINT,WPARAM,LPARAM);
BOOL EXPORTED CALLBACK ImportedFromDlg(HWND,UINT,WPARAM,LPARAM);
BOOL EXPORTED CALLBACK XModulDlg      (HWND,UINT,WPARAM,LPARAM);


/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ CheckCall - pruefen ob disassembler ok               บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
static BOOL PASCAL CheckCall(HWND hDlg,WORD wMode )
{
 int  y;
 FARPROC fpAddr;

  y = (int)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
  if (y == LB_ERR)
      return FALSE;
  else
      fpAddr = (FARPROC)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,y,0);

  if (!IsReadableSelector(HIWORD(fpAddr)))
      return FALSE;
  else
     {
      if (wMode)
          CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_UNASSEM), hDlg, UnassembleDlg,
                            (LPARAM)fpAddr
                           );
      return TRUE;
     }
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ImportedFromDlg - Calls eines Entries auflisten      บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK ImportedFromDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 int  y;
 char szStr[80];
 char szStr1[80];
 MODULEENTRY moduleentry;
 IMPORTENTRY importentry;
 REFMODULEENTRY refmodule;
 HWND  hWnd;
// BYTE fUsed;
// FARPROC fpAddr;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      SetWindowLong(hDlg,DLGWINDOWEXTRA,lParam);
      LoadTabs(IDUS_14,szStr);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,lParam);

      LoadString(hInst,IDS_IMPFROM,szStr1,sizeof(szStr));
      if (LOWORD(lParam))
          GetProcedureName((HMODULE)HIWORD(lParam),LOWORD(lParam),szStr,sizeof(szStr));
      else
         {
          SetDlgItemText(hDlg,ID_LISTBOX1,"Module");
          SetDlgItemText(hDlg,ID_SUBDLG1,"Detail");
          GetModuleName((HMODULE)HIWORD(lParam),szStr,sizeof(szStr));
         }
      strcat(szStr,szStr1);
      SetWindowText(hDlg,szStr);
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
         case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case IDOK:
              break;
         case ID_SUBDLG1:    /* disassembler */
              moduleentry.dwSize = sizeof(MODULEENTRY);
              y = GetWindowWord(hDlg,DLGWINDOWEXTRA);
              if (y)
                 {
                  if (!CheckCall(hDlg,1))
                      MessageBeep(0);
                 }
              else
                 {
                  hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
                  y = ListBox_GetCurSel(hWnd);
                  y = (int)ListBox_GetItemData(hWnd,y);
                  if (ModuleFindHandle(&moduleentry,(HMODULE)y))
                      CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_XMODULE),
                                    hDlg,
                                    XModulDlg,
                                    (LPARAM)y);

                  else
                      MessageBeep(0);
                 }
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                 {
                  case LBN_SELCHANGE:
                     if (GetWindowWord(hDlg,DLGWINDOWEXTRA))
                         EnableDlgItem(hDlg,ID_SUBDLG1,CheckCall(hDlg,0));
                     break;
                  case LBN_DBLCLK:
                     PostMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                     break;
                 }
              break;
         case ID_REFRESH:
              SetCursor(hCursorWait);
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              ListBox_ResetContent(hWnd);
              moduleentry.dwSize = sizeof(moduleentry);
              fShow = FALSE;
              y = ModuleFirst(&moduleentry);
              while (y)
                 {
                  if (LOWORD(lParam) == 0)
                     {
                      refmodule.hModule = moduleentry.hModule;
                      y = RefModuleFirst(&refmodule);
                      while (y)
                         {
                          if (refmodule.hRefModule == (HMODULE)HIWORD(lParam))
                             {
                              wsprintf(szStr,
                                       "%.9s",
                                       (LPSTR)moduleentry.szModule);
                              y = ListBox_AddString(hWnd,szStr);
                              ListBox_SetItemData(hWnd,y,(LPARAM)(LPVOID)moduleentry.hModule);
                              fShow = TRUE;
                             };
                          y = RefModuleNext(&refmodule);
                         }
                     }
                  else
                     {
                      y = ImportFirst(moduleentry.hModule,&importentry,0);
                      while (y)
                          {
                           if ((importentry.hModule == (HMODULE)HIWORD(lParam))
                            && (importentry.wEntry  == LOWORD(lParam)))
                               {
                                wsprintf(szStr,
                                         "%s\t%X:%X",
                                         (LPSTR)moduleentry.szModule,
                                         HIWORD(importentry.fpAddr),
                                         LOWORD(importentry.fpAddr)
                                        );
                                y = ListBox_AddString(hWnd,szStr);
                                ListBox_SetItemData(hWnd,y,
                                            (LPARAM)MAKELONG(LOWORD(importentry.fpAddr) - 1,
                                                     GetSegmentHandle(moduleentry.hModule,
                                                                      HIWORD(importentry.fpAddr))
                                                    )
                                           );
                                fShow = TRUE;
                               }
                           y = ImportNext(moduleentry.hModule,&importentry);
                          }
                      ImportClose(moduleentry.hModule,&importentry);
                     }
                  y = ModuleNext(&moduleentry);
                 }
              if (fShow)
                  ListBox_SetCurSel(hWnd,0);
              SetCursor(hCursor);
              break;
         }
      rc = TRUE;
      break;
    default:
      break;
   }

 return rc;
}
