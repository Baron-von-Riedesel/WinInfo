
#define ITEMS 10

#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"
#include "tss.h"

// #define strlen lstrlen

#define Is386TSS(x) (((x & 0x001F) == 0x000B) || ((x & 0x001F) == 0x0009))
#define IsLDT(x) ((x & 0x001F) == 0x0002)

extern "C" {
void FAR _WINFLAGS();
DWORD FAR PASCAL GetCR0(void);
};

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT hFontAlt;

BOOL CALLBACK ViewDlg    (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK PageDirDlg (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TSSDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK IDTDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK LDTDlg     (HWND, UINT, WPARAM, LPARAM);

int  xSyspos   = 100;
int  ySyspos   = 100;
HWND hWndSystem  = 0;
HWND hWndSysIDT  = 0;
HWND hWndSysPage = 0;

////////////////////////////////////////////////////////
// SysDlg - GDT Entries ausgeben                       
////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK SysDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
  static int BtnTab[] = {ID_SYSVIEW,
                         ID_SYSTSS,
                         ID_SYSLDT,
                         0};
  int x;
  WORD k;
  char str[80];
  char szStr1[40];
  GDTENTRY gdtentry;
  HWND hWnd;
  DWORD dwParm;
  BOOL rc;
  DWORD dwCR0;
  HEXDUMPPARM hdp;

  rc = FALSE;

  switch (message)
     {
      case WM_COMMAND:
         switch (wParam)
           {
           case IDOK:
           case IDCANCEL:
               PostMessage(hDlg,WM_CLOSE,0,0);
               break;
          case ID_SNAPSHOT:
               WriteLBSnapShot(0,
                               hDlg,
                               ID_LISTBOX1,
                               "SYS",
                               WLBSS_CLOSE | WLBSS_CLIPBOARD | WLBSS_MESSAGE);
               break;
           case ID_SYSIDT:
               if (hWndSysIDT)
                   ShowWindow(hWndSysIDT,SW_NORMAL);
               else
                   hWndSysIDT = CreateDialog(hInst,
                                             MAKEINTRESOURCE(IDD_SYSIDT),
                                             hDlg,
                                             IDTDlg);
               break;
           case ID_SYSPAGE:
               if (hWndSysPage)
                   ShowWindow(hWndSysPage,SW_NORMAL);
               else {
                   dwCR0 = 0;
                   CallRing0Proc((FARPROC)GetCR0,&dwCR0);
                   if ( dwCR0 & 0x80000000 ) {
                       hWndSysPage = CreateDialog(hInst,
                                                  MAKEINTRESOURCE(IDD_SYSPAGE),
                                                  hDlg,
                                                  PageDirDlg);
                   } else
                       MessageBeep(0);
               }
               break;
           case ID_SYSVIEW:
           case ID_SYSTSS:
           case ID_SYSLDT:
               x =  (int)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
               if (x == LB_ERR)
                  {
                   MessageBeep(0);
                   break;
                  }
               dwParm = (DWORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,x,0);
               if (Is386TSS(HIWORD(dwParm)))
                   CreateDialogParam(hInst,MAKEINTRESOURCE(IDD_TSS),hDlg,TSSDlg,dwParm);
               else
               if (IsLDT(HIWORD(dwParm)))
                   CreateDialogParam(hInst,MAKEINTRESOURCE(IDD_LDT),hDlg,LDTDlg,dwParm);
               else
               if (IsReadableSelector(LOWORD(dwParm)))
                  {
                   hdp.dwOffset = 0;
                   hdp.hGlobal  = (HGLOBAL)LOWORD(dwParm);
                   hdp.dwLength = 0;
                   hdp.wType    = 0;
                   hWnd = CreateDialogParam(hInst,MAKEINTRESOURCE(IDD_VIEW),hDlg,ViewDlg,(LPARAM)(LPVOID)&hdp);
                   wsprintf(str,"Segment %X",LOWORD(dwParm));
                   SetWindowText(hWnd,str);
                  }
               else
                   MessageBeep(0);
               break;
           case ID_LISTBOX1:
               switch(HIWORD(lParam))
                   {
                    case LBN_SELCHANGE:
                    case LBN_DBLCLK:
                      x =  (int)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
                      if (x == LB_ERR)
                          dwParm = 0;
                      else
                          dwParm = (DWORD)SendDlgItemMessage(hDlg,
                                                             ID_LISTBOX1,
                                                             LB_GETITEMDATA,
                                                             x,0);
                      if (HIWORD(lParam) == LBN_SELCHANGE)
                         {
                          EnableDlgItem(hDlg,ID_SYSTSS,Is386TSS(HIWORD(dwParm)));
                          EnableDlgItem(hDlg,ID_SYSLDT,IsLDT(HIWORD(dwParm)));
                          EnableDlgItem(hDlg,ID_SYSVIEW,IsReadableSelector(LOWORD(dwParm)));
                         }
                      else
                         {
                          if (Is386TSS(HIWORD(dwParm)))
                              PostMessage(hDlg,WM_COMMAND,ID_SYSTSS,0);
                          else
                          if (IsLDT(HIWORD(dwParm)))
                              PostMessage(hDlg,WM_COMMAND,ID_SYSLDT,0);
                          else
                              PostMessage(hDlg,WM_COMMAND,ID_SYSVIEW,0);
                         }
                      break;
                    case XLBN_RBUTTONDOWN:
                      TrackPopup(hDlg,BtnTab);
                      break;
                   }
               break;
           case ID_REFRESH:
               SetCursor(hCursorWait);
               hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
               SendMessage(hWnd,LB_RESETCONTENT,0,0);

               x = GDTFirst(&gdtentry);
               while (x)
                  {
                   GetDescriptorType(gdtentry.attr,szStr1);
                   wsprintf(str,
                            "%X\t%08lX\t%08lX\t%04X\t%s",
                            gdtentry.selector,
                            gdtentry.base,
                            gdtentry.limit,
                            gdtentry.attr,
                            (LPSTR)szStr1
                           );
                   x = (int)SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                   SendMessage(hWnd,LB_SETITEMDATA,x,MAKELONG(gdtentry.selector,gdtentry.attr));
                   x = GDTNext(&gdtentry);
                  }
               wsprintf(str,"GDT Base\t%lX\tGDT Limit\t%X\tLDTR\t%X\tTR\t%X",
                        GetGDTAddress(),
                        GetGDTLimit(),
                        GetLDTR(),
                        GetTR()
                       );
               SetDlgItemText(hDlg,ID_STATUS1,str);
               SendMessage(hWnd,LB_SETCURSEL,0,0);
               SetCursor(hCursor);


               break;
           }                    /* end switch wParam */
         rc = TRUE;             /* end WM_COMMAND */
         break;
      case WM_MOVE:
         SaveWindowPos(hDlg,&xSyspos,&ySyspos);
         break;
      case WM_DESTROY:
         hWndSystem = 0;
         break;
      case WM_CLOSE:
//       PostMessage(hWndMenu,WM_COMMAND,ID_DESTROY,(LPARAM)hDlg);
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
      case WM_INITDIALOG:
         LoadTabs(IDUS_30,str);
         SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
         SendDlgItemMessage(hDlg,
                            ID_LISTBOX1,
                            XLB_SETEXTSTYLE,
                            XLBES_RBUTTONTRACK,
                            (LPARAM)(LPVOID)hFontAlt);
         LoadTabs(IDUS_31,str);
         SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
         k = LOWORD(&_WINFLAGS) & WF_PAGING;
         x = GetTrueVersion();
                                        // fuer NT + OS/2 deaktivieren
         if ((x == 0x532) || (x >= 0x1400))
            {
             EnableDlgItem(hDlg,ID_SYSIDT,0);
             EnableDlgItem(hDlg,ID_SYSPAGE,0);
            }
         SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
         SetWindowPos(hDlg,0,xSyspos,ySyspos,0,0,SWP_NOSIZE | SWP_NOZORDER);
         ShowWindow(hDlg,SW_SHOWNORMAL);
         rc = TRUE;
         break;
     }                          /* end switch message */
  return rc;
}
