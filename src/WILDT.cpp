
#include "string.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"
#include "tss.h"

// #define strlen lstrlen

void FAR _WINFLAGS();

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT   hFontAlt;

BOOL CALLBACK ViewDlg (HWND, UINT, WPARAM, LPARAM);

int xLDTpos = 100;
int yLDTpos = 100;

////////////////////////////////////////////////////////
// LDTDlg - LDT Entries ausgeben                        
////////////////////////////////////////////////////////
BOOL EXPORTED WINAPI LDTDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
  static int BtnTab[] = {ID_SUBDLG1,
                         0};
  WORD x;
  WORD wLDT;
  char str[80];
  char szStr1[40];
  LDTENTRY ldtentry;
  HWND hWnd;
  DWORD dwParm;
  HEXDUMPPARM hdp;
  BOOL fEnable;
  BOOL rc;

  rc = FALSE;

  switch (message)
     {
      case WM_COMMAND:
         hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
         switch (wParam)
           {
           case IDOK:
           case IDCANCEL:
               PostMessage(hDlg,WM_CLOSE,0,0);
               break;
           case ID_SUBDLG1:
               wLDT = GetWindowWord(hDlg,DWL_USER);
               x =  ListBox_GetCurSel(hWnd);
               if (x == LB_ERR) { MessageBeep(0); break; }
               dwParm = ListBox_GetItemData(hWnd,x);
               ldtentry.selector = LOWORD(dwParm);
               if (LDTFind(wLDT,&ldtentry)) {
                   if ((ldtentry.attr & 0x0090) == 0x0090)
                       if (ldtentry.base >= 0x80000000) {
                           hdp.dwOffset = ldtentry.base;
                           hdp.dwLength = ldtentry.limit+1;
                           hdp.wType   = HDP_LINEAR;
                           hWnd = CreateDialogParam(hInst,
                                                    MAKEINTRESOURCE(IDD_VIEW),
                                                    hDlg,
                                                    ViewDlg,
                                                    (LPARAM)(LPVOID)&hdp);
                           wsprintf(str,"Segment %X",LOWORD(dwParm));
                           SetWindowText(hWnd,str);
                           break;
                       }
               }
               MessageBeep(0);
               break;
           case ID_LISTBOX1:
               switch (HIWORD(lParam)) {
                  case LBN_SELCHANGE:
                    x =  ListBox_GetCurSel(hWnd);
                    if (x == LB_ERR) break;
                    dwParm = ListBox_GetItemData(hWnd,x);
                    wLDT = GetWindowWord(hDlg,DWL_USER);
                    ldtentry.selector = LOWORD(dwParm);
                    fEnable = FALSE;
                    if (LDTFind(wLDT,&ldtentry))
                        if ((ldtentry.attr & 0x0090) == 0x0090)
                           if (ldtentry.base >= 0x80000000)	// nur anzeigbar wenn im oberen 2 GB Bereich
                              fEnable = TRUE;
                    EnableDlgItem(hDlg,ID_SUBDLG1,fEnable);
                    break;
                  case LBN_DBLCLK:
                    if (IsWindowEnabled(GetDlgItem(hDlg,ID_SUBDLG1)))
                        PostMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                    break;
                  case XLBN_RBUTTONDOWN:
                    TrackPopup(hDlg,BtnTab);
                    break;
               }
               break;
           case ID_REFRESH:
               SetCursor(hCursorWait);
               wLDT = GetWindowWord(hDlg,DWL_USER);
               ListBox_ResetContent(hWnd);

               x = LDTFirst(wLDT,&ldtentry);
               while (x) {
                   GetDescriptorType(ldtentry.attr,szStr1);
                   wsprintf(str,
                            "%X\t%08lX\t%08lX\t%04X\t%s",
                            ldtentry.selector,
                            ldtentry.base,
                            ldtentry.limit,
                            ldtentry.attr,
                            (LPSTR)szStr1
                           );
                   x = ListBox_AddString(hWnd,(LPSTR)str);
                   ListBox_SetItemData(hWnd,
                                       x,
                                       MAKELONG(ldtentry.selector,ldtentry.attr));
                   x = LDTNext(wLDT,&ldtentry);
               }
               LDTClose(wLDT,&ldtentry);
//             wsprintf(str,"LDT Base\t%lX\tLDT Limit\t%X",
//                      GetGDTAddress(),
//                      GetGDTLimit(),
//                     );
//             SetDlgItemText(hDlg,ID_STATUS1,str);

               SetCursor(hCursor);
               break;
           }                    /* end switch wParam */
         rc = TRUE;             /* end WM_COMMAND */
         break;
      case WM_MOVE:
         SaveWindowPos(hDlg,&xLDTpos,&yLDTpos);
         break;
      case WM_CLOSE:
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
      case WM_INITDIALOG:
         SetWindowLong(hDlg,DWL_USER,lParam);
         LoadTabs(IDUS_30,str);
         SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
         SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                            (LPARAM)(LPVOID)hFontAlt);
//       LoadTabs(IDUS_31,str);
//       SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));

         LoadString(hInst,IDS_LDT,szStr1,sizeof(szStr1));
         wsprintf(str, "%s %X", (LPSTR)szStr1, LOWORD(lParam) );
         SetWindowText(hDlg,str);

         SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
         SetWindowPos(hDlg,0,xLDTpos,yLDTpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
         ShowWindow(hDlg,SW_SHOWNORMAL);
         rc = TRUE;
         break;
     }                          /* end switch message */
  return rc;
}
