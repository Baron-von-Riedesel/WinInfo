
#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"

extern "C" {
void FAR _WINFLAGS();
};

extern HFONT hFontAlt;

BOOL WINAPI  CheckMessage(MSG *,WORD,HWND);

BOOL CALLBACK ViewDlg  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK VxDDlg (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK VMDlg  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK IntrDlg(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DPMIStatusDlg(HWND, UINT, WPARAM, LPARAM);

int    xDPMIpos  = 100;
int    yDPMIpos  = 100;
HWND   hWndDPMI     = 0;
HWND   hwndDPMIStat = 0;
HWND   hWndDPMIInt  = 0;
HWND   hWndDPMIVM   = 0;
HWND   hWndDPMIVxD  = 0;

static WNDPROC fpDPMIStatWndProc;
static int wFlag = 0;

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ DPMIDlg - DPMI Dialog                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK DPMIDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static BYTE fSpaceErr;
 static int BtnTab[] = {ID_DPMIANZ,
                        0};
// PAINTSTRUCT ps;
 WORD dpmivers;
 DWORD dpmiex;
 HDC thDC;
 int w,x,y,z;
 BOOL rc;
 HWND hWnd;
 HANDLE hGlobal;
 LPSTR lpstr,lpstr1;
 DPMISTRUCT dpmistruct;
 HEXDUMPPARM hdp;
 WORD wLDT;
 char str[100];
 char str1[80];
 MSG msg;

 rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
        if (GetVersion() == 3)
            if (!(LOWORD(&_WINFLAGS) & WF_ENHANCED))
                wFlag = 1;
        LoadTabs(IDUS_28,str);
        SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
        SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                           (LPARAM)(LPVOID)hFontAlt);
        LoadTabs(IDUS_29,str);
        SendDlgItemMessage(hDlg,ID_STATUS2,ST_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
        LoadTabs(IDUS_50,str);
        SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
//        SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
        PostMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
        SetWindowPos(hDlg,0,xDPMIpos,yDPMIpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
        if ((!(LOWORD(&_WINFLAGS) & WF_ENHANCED)))
            {
             EnableDlgItem(hDlg,ID_DPMIVM,0);
             EnableDlgItem(hDlg,ID_DPMIVXD,0);
            }
        ShowWindow(hDlg,SW_SHOWNORMAL);
        if (fSpaceErr)
            CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRIMP4),0,MB_OK);
        rc = TRUE;
        break;
    case WM_CLOSE:
//        PostMessage(hWndMenu,WM_COMMAND,ID_DESTROY,(LPARAM)hDlg);
        DestroyWindow(hDlg);
        rc = TRUE;
        break;
    case WM_DESTROY:
        hWndDPMI = 0;
        break;
    case WM_MOVE:
        SaveWindowPos(hDlg,&xDPMIpos,&yDPMIpos);
        break;
    case WM_COMMAND:
        switch (wParam)
           {
            case IDCANCEL:
                PostMessage(hDlg,WM_CLOSE,0,0);
                break;
            case ID_DPMIMON:
                if (hwndDPMIStat)
                    ShowWindow(hwndDPMIStat,SW_NORMAL);
                else
                    hwndDPMIStat = CreateDialog(hInst,
                                                MAKEINTRESOURCE(IDD_DPMIMON),
                                                0,
                                                DPMIStatusDlg);
                break;
/*          case ID_DPMIKON:
                CreateDialog(hInst,MAKEINTRESOURCE(DPMI2BOX),hDlg,DPMI2Dlg);
                EnableWindow(GetDlgItem(hDlg,wParam),0);
                break;
            case ID_DPMIKONACT:
                EnableWindow(GetDlgItem(hDlg,ID_DPMIKON),1);
                break;
*/
            case ID_DPMIVM:
                if (hWndDPMIVM)
                    ShowWindow(hWndDPMIVM,SW_RESTORE);
                else
                    hWndDPMIVM = CreateDialog(hInst,
                                              MAKEINTRESOURCE(IDD_DPMIVM),
                                              hDlg,
                                              VMDlg);
                break;
            case ID_DPMIVXD:
                if (hWndDPMIVxD)
                    ShowWindow(hWndDPMIVxD,SW_RESTORE);
                else
                    hWndDPMIVxD = CreateDialog(hInst,
                                               MAKEINTRESOURCE(IDD_DPMIVXD),
                                               hDlg,
                                               VxDDlg);
                break;
            case ID_DPMIINT:
                if (hWndDPMIInt)
                    ShowWindow(hWndDPMIInt,SW_RESTORE);
                else
                    hWndDPMIInt = CreateDialog(hInst,
                                               MAKEINTRESOURCE(IDD_DPMIINT),
                                               hDlg,
                                               IntrDlg);
                break;
            case ID_LISTBOX1:
                switch(HIWORD(lParam))
                   {
                    case LBN_SELCHANGE:
                    case LBN_DBLCLK:
                      y =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
                      if (y == LB_ERR)
                         {
                          MessageBeep(0);
                          break;
                         }
                      hGlobal = (HANDLE)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,y,0);
                      y = IsReadableSelector((WORD)hGlobal);
                      if (HIWORD(lParam) == LBN_SELCHANGE)
                         EnableDlgItem(hDlg,ID_DPMIANZ,y);
                      else
                         if (y)
                             PostMessage(hDlg,WM_COMMAND,ID_DPMIANZ,0);
                         else
                             MessageBeep(0);
                      break;
                    case XLBN_RBUTTONDOWN:
                      TrackPopup(hDlg,BtnTab);
                      break;
                   }
                break;
            case ID_DPMIANZ:
                y =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
                if (y == LB_ERR)
                   MessageBeep(0);
                else
                   {
                    hGlobal = (HANDLE)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,y,0);
                    hdp.dwOffset = 0;
                    hdp.hGlobal  = hGlobal;
                    hdp.dwLength = 0;
                    hdp.wType    = HDP_SETWINDOWTEXT;
                    CreateDialogParam(hInst,
                                      MAKEINTRESOURCE(IDD_VIEW),
                                      hDlg,
                                      ViewDlg,
                                      (LPARAM)(LPVOID)&hdp
                                     );
                   }
                break;
            case ID_REFRESH:
               fSpaceErr = FALSE;
//               SetCursor(hCursorWait);
               EnableDlgItem(hDlg,ID_REFRESH,0);
               SetDlgItemText(hDlg,ID_STATUS1,"\tscaning LDT ...");

               hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
               SendMessage(hWnd,LB_RESETCONTENT,0,0);

               x = GetDPMIDescriptorFirst(&dpmistruct);
               y = 0;
               z = 0;

               while (x && CheckMessage(&msg,PM_REMOVE,hDlg))
                  {
                   GetDescriptorType(dpmistruct.attribute,str1);
                   wsprintf(str,
                            "%X\t%08lX\t%08lX\t%04X\t%s",
                            dpmistruct.selector,
                            dpmistruct.base,
                            dpmistruct.limit,
                            dpmistruct.attribute,
                            (LPSTR)str1
                           );
                   if (!fSpaceErr)
                      {
                       x = (int)SendMessage(hWnd,LB_ADDSTRING,0,(LPARAM)(LPSTR)str);
                       if (x == LB_ERRSPACE)
                           fSpaceErr = TRUE;
                       else
                           SendMessage(hWnd,LB_SETITEMDATA,x,MAKELONG(dpmistruct.selector,0));
                      }
                   if (!y)
                      SendMessage(hWnd,LB_SETCURSEL,0,0);
                   x = GetDPMIDescriptorNext(&dpmistruct);
//                   if (GetQueueStatus(QS_ALLINPUT & (~QS_MOUSEMOVE)))
                   y++;
                   if (dpmistruct.attribute)
                       z++;
                  }
               InvalidateRect(hWnd,0,1);

               LoadString(hInst,IDS_XVER1, str1, sizeof(str1));
               dpmivers = DPMIGetVersion();
               dpmiex = DPMIGetVersionEx();
               if (LOWORD(dpmiex) & 2)
                  lpstr = "Real";
               else
                  lpstr = "V86";
               if (LOWORD(dpmiex) & 4)
                  lpstr1 = "";
               else
                  lpstr1 = "kein";
               wsprintf(str,
                        str1,
                        HIBYTE(dpmivers),
                        LOBYTE(dpmivers),
                        (LOWORD(dpmiex) & 1) + '2',
                        lpstr,
                        lpstr1
                       );
               if (wLDT = GetLDTSelector())
                  {
                   wsprintf(str1,", LDT-Alias=%X",wLDT);
                   strcat(str,str1);
                  }

               SetDlgItemText(hDlg,ID_STATUS1,str);

               LoadString(hInst,IDS_XVER2, str1, sizeof(str1));
               w = (dpmiex >> 16) & 0xff;
               wsprintf(str,
                        str1,
                        y,
                        z,
                        w + '0',
                        DPMIGetPageSize()
                       );
               SetDlgItemText(hDlg,ID_STATUS2,str);

               EnableDlgItem(hDlg,ID_REFRESH,1);
//               SetCursor(hCursor);
               break;
            }
        rc = TRUE;
        break;
    default:
        break;
   }
  return rc;
}
