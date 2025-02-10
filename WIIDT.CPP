
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "user.h"
#include "user386.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT   hFontAlt;
extern HWND    hWndSysIDT;

BOOL CALLBACK UnassembleDlg  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Unassemble32Dlg(HWND, UINT, WPARAM, LPARAM);

int  xIDTpos = 100;
int  yIDTpos = 100;

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ IDTDlg - Interrupt Dialog                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK IDTDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
    static int BtnTab[] = {ID_SUBDLG1,
                        0};
    static DWORD dwIDTAddr = 0;
    static WORD wIDTLimit = 0;
    char  str[80];
    char  szStr1[40];
    // LPSTR lpstr;
    int   i,y;
    HWND  hWnd;
    IDTENTRY idtentry;
    VMENTRY __far * lpvmentry;
    FWORD faddr;
    BOOL rc = FALSE;

    switch (message) {
    case WM_COMMAND:
        switch (wParam) {
        case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
        case IDOK:
              PostMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
              break;
        case ID_SUBDLG1:
              y =  ListBox_GetCurSel(hWnd = GetDlgItem(hDlg,ID_LISTBOX1));
              if (y == LB_ERR) {
                  MessageBeep(0);
                  break;
              }

              ListBox_GetText(hWnd,y,str);
              if (sscanf(str,"%X %X:%lX ",&i,&faddr.selector,&faddr.offset) > 2)
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_UNASSEM32),
                                    hDlg,
                                    Unassemble32Dlg,
                                    (DWORD)(LPSTR)&faddr
                                   );
              else
                  MessageBeep(0);

              break;
        case ID_REFRESH:
            SetCursor(hCursorWait);
            lParam = GetWindowLong(hDlg,DLGWINDOWEXTRA);
            if (lParam) {
                idtentry.wLimit = wIDTLimit;
                idtentry.dwAddr = dwIDTAddr;
            } else
                idtentry.wLimit = 0;	// aktuelle IDT lesen
            ListBox_ResetContent(hWnd = GetDlgItem(hDlg,ID_LISTBOX1));


            y = IDTFirst(&idtentry);
            while (y) {
                GetDescriptorType((WORD)HIBYTE(idtentry.attr),szStr1);
                wsprintf(str,
                         "%X\t%X:%lX\t%04X\t%s",
                         idtentry.id,
                         idtentry.selector,
                         idtentry.offset,
                         idtentry.attr,
                         (LPSTR)szStr1
                        );
                ListBox_AddString(hWnd,str);
                y = IDTNext(&idtentry);
            }

            wsprintf(str,"IDT Base\t%lX\tIDT Limit\t%X\t",
                     idtentry.dwAddr,idtentry.wLimit);

            SetDlgItemText(hDlg,ID_STATUS1,str);
            EnableDlgItem(hDlg,ID_SUBDLG1,0);
            SetCursor(hCursor);
            break;
        case ID_LISTBOX1:
            switch(HIWORD(lParam)) {
            case LBN_SELCHANGE:
                if (GetWindowLong(hDlg,DLGWINDOWEXTRA) == 0) // falls fremde IDT, kein Disasm
                    EnableDlgItem(hDlg,ID_SUBDLG1,1);
                break;
            case LBN_DBLCLK:
                PostMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                break;
            case XLBN_RBUTTONDOWN:
                TrackPopup(hDlg,BtnTab);
                break;
            }
            break;
        }
        rc = TRUE;
        break;
    case WM_MOVE:
        SaveWindowPos(hDlg,&xIDTpos,&yIDTpos);
        break;
    case WM_DESTROY:
        hWndSysIDT = 0;
        break;
    case WM_CLOSE:
        DestroyWindow(hDlg);
        rc = TRUE;
        break;
    case WM_INITDIALOG:
        SetWindowLong(hDlg,DLGWINDOWEXTRA,lParam);
        if (lParam) {
            lpvmentry = (VMENTRY __far *)lParam;
            dwIDTAddr = lpvmentry->dwIDTAddr;
            wIDTLimit = lpvmentry->wIDTLimit;
        }

        LoadTabs(IDUS_37,str);
        SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
        SendDlgItemMessage(hDlg,
                           ID_LISTBOX1,
                           XLB_SETEXTSTYLE,
                           XLBES_RBUTTONTRACK,
                           (LPARAM)(LPVOID)hFontAlt);
        LoadTabs(IDUS_38,str);
        SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
        SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
        SetWindowPos(hDlg,0,xIDTpos,yIDTpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
        ShowWindow(hDlg,SW_NORMAL);
        rc = TRUE;
        break;
    default:
        break;
    }
    return rc;
}
