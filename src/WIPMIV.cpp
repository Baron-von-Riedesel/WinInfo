
/* display protected-mode interrupt vectors of a VM.
 * This dialog is called by VMDlg() - requires Windows VMM.
 */

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
#include "vmm.h"
#include "wininfo.h"
#include "wininfox.h"

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern LPSTR lpszHint;
extern HFONT   hFontAlt;
extern HWND    hWndSysIDT;

BOOL CALLBACK UnassembleDlg  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Unassemble32Dlg(HWND, UINT, WPARAM, LPARAM);

int  xPMIVpos = 100;
int  yPMIVpos = 100;

////////////////////////////////////////////////////////
// PMIVDlg - Protected Mode IntVec Dialog 
////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK PMIVDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
// static int BtnTab[] = {ID_SUBDLG1,
//                        0};
    char  str[80];
    //char  szStr1[40];
    // LPSTR lpstr;
    DWORD dwVMHandle;
    DWORD dwPMIV[2];
    int   i;
    HWND  hWnd;
    VMENTRY vmentry;
    //FWORD faddr;
    BOOL rc = FALSE;

    switch (message) {
    case WM_COMMAND:
        switch (wParam) {
        case IDCANCEL:
            PostMessage(hDlg,WM_CLOSE,0,0);
            break;
/*
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
                                    (DWORD)(LPSTR)&faddr);
              else
                  MessageBeep(0);

              break;
*/
        case ID_REFRESH:
            SetCursor(hCursorWait);
            ListBox_ResetContent(hWnd = GetDlgItem(hDlg,ID_LISTBOX1));

            dwVMHandle = GetWindowLong(hDlg,DLGWINDOWEXTRA);

            if (!VMFindHandle(dwVMHandle,&vmentry)) {
                MessageBeep(0);
                break;         
            }
            if (!(vmentry.dwFlags & VMStat_PM_App)) {
                CreateMessage(hDlg,"VM nicht (mehr) im Protected-Mode",lpszHint,MB_OK);
                MessageBeep(0);
                break;         
            }
            if (!vmentry.dwPMIntVecs) {
                CreateMessage(hDlg,"PM Interrupt Vector Table not initialized",lpszHint,MB_OK);
                MessageBeep(0);
                break;         
            }


            for (i = 0; i < 0x100; i++) {
                /* read Interrupt vector (8 bytes) */
                if ( GetFlatAddrBytes(&dwPMIV,vmentry.dwPMIntVecs+i*8,8) ) {
                    wsprintf(str,
                             "%X\t%X:%lX",
                             i,
                             LOWORD(dwPMIV[1]),
                             dwPMIV[0]
                            );
                    ListBox_AddString(hWnd,str);
                }
            }
            wsprintf(str,"PMIV Adresse\t%lX\t", vmentry.dwPMIntVecs);

            SetDlgItemText(hDlg,ID_STATUS1,str);
            EnableDlgItem(hDlg,ID_SUBDLG1,0);
            SetCursor(hCursor);
            break;
        case ID_LISTBOX1:
#if 0
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
#endif
            break;
        }
        rc = TRUE;
        break;
    case WM_MOVE:
        SaveWindowPos(hDlg,&xPMIVpos,&yPMIVpos);
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
            
        LoadTabs(IDUS_61,str);
        SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
        SendDlgItemMessage(hDlg,
                         ID_LISTBOX1,
                         XLB_SETEXTSTYLE,
                         XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
        LoadTabs(IDUS_62,str);
        SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
        SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
        SetWindowPos(hDlg,0,xPMIVpos,yPMIVpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
        ShowWindow(hDlg,SW_NORMAL);
        rc = TRUE;
        break;
    default:
        break;
    }
    return rc;
}
