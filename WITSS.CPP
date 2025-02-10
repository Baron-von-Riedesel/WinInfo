
#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"
#include "tss.h"

#define strlen lstrlen

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT   hFontAlt;

HMENU WINAPI TrackPopup(HWND, PINT);

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ TSSDlg - Task State Segment Dialog                   บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK TSSDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
  WORD x,i;
  char str[80];
  char * pszStr;
  HWND hWnd;
  DWORD dwParm;
  char szStr1[40];
  LPTSSENTRY lptssentry;
  IOBMENTRY iobmentry;
  BOOL rc;


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
           case ID_REFRESH:
               SetCursor(hCursorWait);
               hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
               SendMessage(hWnd,LB_RESETCONTENT,0,0);
               dwParm = GetWindowLong(hDlg,DLGWINDOWEXTRA);

               if ( x = IOBMFirst(LOWORD(dwParm),&iobmentry) ) {
                   i = 0;
                   pszStr = str;
                   while (x) {
                       if (iobmentry.flag) {
                           wsprintf(pszStr,"%X\t",iobmentry.port);
                           pszStr = pszStr + strlen(pszStr);
                       }
                       i++;
                       if (!(i%8) && (pszStr != str)) {
                           SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                           pszStr = str;
                       }
                       x = IOBMNext(LOWORD(dwParm),&iobmentry);
                   }
                   IOBMClose(LOWORD(dwParm),&iobmentry);
               }

               wsprintf(str,"IOPL\t%X\tCPL\t%X\t",
                        GetIOPL(),
                        GetCPL()
                       );
               SetDlgItemText(hDlg,ID_STATUS1,str);

               if (lptssentry = GetTSS(LOWORD(dwParm))) {
                   wsprintf(str, "Ring-0 SS:ESP\t%X:%lX",
                            lptssentry->ring0sp.selector,
                            lptssentry->ring0sp.offset
                           );
                   FreeDescriptor(HIWORD(lptssentry));
                   SetDlgItemText(hDlg,ID_STATUS2,str);
                   SetCursor(hCursor);
               } else {
                   SetCursor(hCursor);
                   MessageBox( hDlg, "TSS cannot be accessed", 0, MB_OK );
                   PostMessage( hDlg, WM_CLOSE, 0, 0 );
               }

               break;
           }                    /* end switch wParam */
         rc = TRUE;             /* end WM_COMMAND */
         break;
      case WM_CLOSE:
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
      case WM_INITDIALOG:
         SetWindowLong(hDlg,DLGWINDOWEXTRA,lParam);
         LoadTabs(IDUS_34,str);
         SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
         LoadTabs(IDUS_35,str);
         SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
         LoadTabs(IDUS_39,str);
         SendDlgItemMessage(hDlg,ID_STATUS2,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));

         LoadString(hInst,IDS_TSS,szStr1,sizeof(szStr1));
         wsprintf(str, "%s %X", (LPSTR)szStr1, LOWORD(lParam) );
         SetWindowText(hDlg,str);

         SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
         ShowWindow(hDlg,SW_SHOWNORMAL);
         rc = TRUE;
         break;
     }                          /* end switch message */
  return rc;
}
