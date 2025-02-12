
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Callback fr Enumfonts                               บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int EXPORTED CALLBACK GettheFonts(LPLOGFONT lplogfont,
                            LPTEXTMETRIC tMetrics,
                            int fonttype,
                            LPARAM pWnd)
{
  char str[128];
  LPLOGFONT lplf = lplogfont;	
  HWND hWnd = *(HWND FAR *)pWnd;
  
  wsprintf(str,
           "%s,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X",
           (LPSTR)lplf->lfFaceName,
           fonttype,
           lplf->lfHeight,
           lplf->lfWidth,
           lplf->lfEscapement,
           lplf->lfOrientation,
           lplf->lfWeight,
           lplf->lfItalic,
           lplf->lfUnderline,
           lplf->lfStrikeOut,
           lplf->lfCharSet,
           lplf->lfOutPrecision,
           lplf->lfClipPrecision,
           lplf->lfQuality,
           lplf->lfPitchAndFamily
          );

 SendDlgItemMessage(hWnd,
                    ID_TESTLB,
                    LB_ADDSTRING,
                    0,
                    (LPARAM)(LPSTR) str);
 return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ TestDlg - Dialogbox zur Testausgabe                  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK TestDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
  HDC tDC;
  BOOL rc = FALSE;

  switch (message)
      {
      case WM_INITDIALOG:
         tDC = GetDC(hDlg);
         SendDlgItemMessage(hDlg, ID_TESTLB, LB_RESETCONTENT, 0, 0L);
         EnumFonts(tDC,(LPCSTR)NULL,(OLDFONTENUMPROC)GettheFonts,(LPARAM)(LPVOID)&hDlg);
         ReleaseDC(hWndMain,tDC);
         SendDlgItemMessage(hDlg, ID_TESTLB, LB_SETCURSEL, 0, 0L);
         ShowWindow(hDlg,SW_SHOWNORMAL);
         rc = TRUE;
         break;
      case WM_CLOSE:
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
      default:
         break;
      }

  return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ TestDlg - Dialogbox zur Testausgabe                  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
/*
BOOL EXPORTED CALLBACK TestDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 HWND hWnd;
 WORD y;
 char str[80];
 char str1[40];
 int  tabpos[4];
 BOOL rc = FALSE;

 switch (message)
    {
    case WM_COMMAND:
       switch (wParam)
         {
         case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_FILLBOX:
              SendDlgItemMessage(hDlg,ID_TESTLB,WM_SETREDRAW,0,0);
              SendDlgItemMessage(hDlg,ID_TESTLB,LB_RESETCONTENT,0,0);

              hWnd = hDlg;
              for (y=0;y<50;y++)
                  {
                   hWnd = GetNextWindow(hWnd,GW_HWNDNEXT);
                   GetWindowText(hWnd,str1,sizeof(str1));
                   wsprintf(str,
                            "%X,%X,%X,%X,%X,%s",
                            hWnd,
                            GetWindowTask(hWnd),
                            IsWindowEnabled(hWnd),
                            IsWindowVisible(hWnd),
                            IsIconic(hWnd),
                            (LPSTR)str1
                           );
                   SendDlgItemMessage(hDlg,
                                        ID_TESTLB,
                                        LB_ADDSTRING,
                                        0,
                                        (LONG)(LPSTR) str);
                  }

              SendDlgItemMessage(hDlg,ID_TESTLB,WM_SETREDRAW,1,0);
              break;
         }
        rc = TRUE;
        break;
    case WM_CLOSE:
        DestroyWindow(hDlg);
        rc = TRUE;
        break;
    case WM_INITDIALOG:
        zaehler++;
        tabpos[0] = 30;
        tabpos[1] = 72;
        tabpos[2] = 102;
        tabpos[3] = 144;
        SendDlgItemMessage(hDlg,ID_TESTLB,ST_SETTABSTOPS,4,(LONG)tabpos);
        SendMessage(hDlg,WM_COMMAND,ID_FILLBOX,0);
        ShowWindow(hDlg,SHOW_OPENWINDOW);
        rc = TRUE;
        break;
    default:
        break;
    }

 return rc;
}
BOOL DoTestDlg(HWND hDlg)
{
 FARPROC lpproc;
 DWORD dw;

   return CreateDialogParam(hInst,
                            MAKEINTRESOURCE(IDD_TEST),
                            hWndMain,
                            TestDlg,
                            (DWORD)&dw);
}
*/
