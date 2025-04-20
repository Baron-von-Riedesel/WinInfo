
/* DPMI status monitor;
 * uses a timer to update status display every 2 seconds.
 */

#define ITEMS 10

#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"

#define strlen lstrlen
#define memset _fmemset

extern HWND hwndDPMIStat;

int    xDPMIStatpos = 100;
int    yDPMIStatpos = 100;

static WNDPROC fpDPMIStatWndProc;
static int wFlag = 0;
static DPMIMEMORY dm;
                                      /* strings */
static PSTR fstr[10] = {0};
static BOOL bIconPainted = FALSE;
static char szMonText[20];

static STRLOADENTRY CODESEG strloadtab[] = {
                       &fstr[0],  IDS_XDPMI0A,
                       &fstr[1],  IDS_XDPMI01,
                       &fstr[2],  IDS_XDPMI02,
                       &fstr[3],  IDS_XDPMI03,
                       &fstr[4],  IDS_XDPMI04,
                       &fstr[5],  IDS_XDPMI05,
                       &fstr[6],  IDS_XDPMI06,
                       &fstr[7],  IDS_XDPMI07,
                       &fstr[8],  IDS_XDPMI08,
                       &fstr[9],  IDS_XDPMI09,
                       (PSTR *)(0)};

static DWORD GetVPData(LPDPMIMEMORY lpdm)
/////////////////////////////////////////
{
    DWORD xx;
    WORD wProz1,wProz2;

    /* return free virtual and physical memory in percent */

    /* there's no "virtual total" field returned by DPMI host,
     * so it has to be calculated:
     */
    xx = lpdm->dwLinearSpace - lpdm->dwFreeLinearSpace + lpdm->dwFreePages;
    if (xx)
        wProz1 = (WORD)(lpdm->dwFreePages * 100 / xx);
    else
        wProz1 = 0;

    if (lpdm->dwPhysPages)
        wProz2 = (WORD)(lpdm->dwFreePhysPages * 100 / lpdm->dwPhysPages);
    else
        wProz2 = 0;
    return MAKELONG(wProz1, wProz2);

}
////////////////////////////////////////////////////////
// OutputIconic
////////////////////////////////////////////////////////
static void OutputIconic(HWND hWnd,HDC hDC,LPDPMIMEMORY lpdm)
{
    char str[80];
    HBRUSH hBrush,oldBrush;
    TEXTMETRIC tm;
    RECT rect;
    DWORD xx;

    bIconPainted = TRUE;
    xx = GetVPData(lpdm);
    SetMapMode( hDC, MM_TEXT );
    GetClientRect(hWnd,&rect);
    hBrush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    oldBrush = (HBRUSH)SelectObject(hDC,hBrush);
    Rectangle(hDC,0,0,rect.right,rect.bottom);
    SetBkMode(hDC,TRANSPARENT);

    GetTextMetrics(hDC,&tm);
    tm.tmHeight--;

    wsprintf(str,"%u%%",LOWORD(xx));
    TextOut(hDC,2,tm.tmHeight*0,str,strlen(str));

    wsprintf(str,"%u%%",HIWORD(xx));
    TextOut(hDC,2,tm.tmHeight*1,str,strlen(str));

    SelectObject(hDC,oldBrush);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Update (Timer)                                       บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void UpdateProc(HWND hDlg,int initf)
{
    static DPMIMEMORY dm1;
    static WORD dosaltmem;
    WORD dosmem;
    int x;
    char str[80];

    if (initf) {
        memset(&dm1,0xFF,sizeof(dm1));
        dosaltmem = 0xFFFF;
    }

    if (!wFlag)
        DPMIGetMemoryInfo(&dm);
    else
        memset(&dm,0xFF,sizeof(dm));

    if (IsIconic(hDlg)) {
        DWORD xx;
        InvalidateRect(hDlg,0,1);
        if (bIconPainted == FALSE) {
            xx = GetVPData(&dm);
            wsprintf(str,"V/P=%u%%/%u%%",LOWORD(xx),HIWORD(xx));
            SetWindowText(hDlg,str);
        }
        return;
    }
    x = 0;  // 0 free address space (pages)
    if (dm.dwFreeLinearSpace != dm1.dwFreeLinearSpace) {
        wsprintf(str,fstr[x],dm.dwFreeLinearSpace<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;   // 1 free virtual total (pages)
    if (dm.dwFreePages != dm1.dwFreePages) {
        wsprintf(str,fstr[x],dm.dwFreePages<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;   // 2 max free virtual block (bytes)
    if (dm.maxFreeBytes != dm1.maxFreeBytes) {
        wsprintf(str,fstr[x],dm.maxFreeBytes>>10);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;   // 3 total address space (pages)
    if (dm.dwLinearSpace != dm1.dwLinearSpace) {
        wsprintf(str,fstr[x],dm.dwLinearSpace<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;   // 4 max virtual block, lockable (pages)
    if (dm.dwMaxAllocLockLinPages != dm1.dwMaxAllocLockLinPages) {
        wsprintf(str,fstr[x],dm.dwMaxAllocLockLinPages<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;   // 5 free physical, unlocked (pages)
    if (dm.dwUnlockedPhysPages != dm1.dwUnlockedPhysPages) {
        wsprintf(str,fstr[x],dm.dwUnlockedPhysPages<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;   // 6 free physical (pages) in pool
    if (dm.dwFreePhysPages != dm1.dwFreePhysPages) {
        wsprintf(str,fstr[x],dm.dwFreePhysPages<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;   // 7 total physical (pages)
    if (dm.dwPhysPages != dm1.dwPhysPages) {
        wsprintf(str,fstr[x],dm.dwPhysPages<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;   // 8
    if (initf || dm.numSwapPages != dm1.numSwapPages) {
        wsprintf(str,fstr[x],(dm.numSwapPages != 0xffffffff) ? dm.numSwapPages << 2 : dm.numSwapPages);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;   // 9
    dosmem = HIWORD(DPMIAllocDosMemory(0xFFF0));
    if ( dosmem != dosaltmem) {
        dosaltmem = dosmem;
        wsprintf(str,fstr[x],dosmem);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    dm1 = dm;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ DPMIStatWndProc - window subclass                    บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
LRESULT EXPORTED CALLBACK DPMIStatWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    PAINTSTRUCT ps;
    // HBRUSH hBrush;
    LRESULT rc;

    rc = 0;

    switch (message)
    {
    case WM_ICONERASEBKGND:
        break;
    case WM_SYSCOMMAND:
        if (wParam == SC_MINIMIZE)
            SetWindowText(hWnd,"Virt/Phys");

        rc = CallWindowProc(fpDPMIStatWndProc,hWnd,message,wParam,lParam);
        break;
    case WM_ACTIVATE:
        if (wParam != WA_INACTIVE)
            if (HIWORD(lParam) == 0)
                SetWindowText(hWnd,szMonText);
        break;
    case WM_PAINT:
        if (!IsIconic(hWnd)) {
            rc = CallWindowProc(fpDPMIStatWndProc,hWnd,message,wParam,lParam);
            break;
        }
        /* fall thru */
    case WM_PAINTICON:
        BeginPaint(hWnd,&ps);
        OutputIconic(hWnd,ps.hdc,&dm);
        EndPaint(hWnd,&ps);
        break;
    default:
        rc = CallWindowProc(fpDPMIStatWndProc,hWnd,message,wParam,lParam);
        break;
    }
    return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ DPMIStatusDlg - DPMI Monitor                         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK DPMIStatusDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
// PAINTSTRUCT ps;
// HDC thDC;
 int x;
 int tabpos[2];
 BOOL rc;
// char str[80];

 rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
        hwndDPMIStat = hDlg;
        GetWindowText(hDlg,szMonText,sizeof(szMonText));
        fpDPMIStatWndProc = (WNDPROC)GetWindowLong(hDlg,GWL_WNDPROC);
        SetWindowLong(hDlg,GWL_WNDPROC,(LONG)(WNDPROC)DPMIStatWndProc);
        if (!fstr[0])
            LoadStringTab(hInst,strloadtab);
        SetTimer(hDlg,1,2000,0);
        tabpos[0] = 90;
        tabpos[1] = 120;
        for (x=0;x<ITEMS;x++)
           {
            SendDlgItemMessage(hDlg,x + ID_DPMISTAT1,ST_SETTABSTOPS,2,(LPARAM)(LPVOID)&tabpos);
            SendDlgItemMessage(hDlg,x + ID_DPMISTAT1,ST_SETALTFONT,-1,0);
           }
        SendDlgItemMessage(hDlg,ID_DPMIUEB1,ST_SETTABSTOPS,2,(LPARAM)(LPVOID)&tabpos);
        SetDlgItemText(hDlg,ID_DPMIUEB1,"Speicherart\tkBytes");
        UpdateProc(hDlg,1);

        SetWindowPos(hDlg,
                     0,
                     xDPMIStatpos,
                     yDPMIStatpos,
                     0,0,SWP_NOSIZE | SWP_NOZORDER);
        ShowWindow(hDlg,SW_SHOWNORMAL);
        rc = TRUE;
        break;
    case WM_DESTROY:
        KillTimer(hDlg,1);
        hwndDPMIStat = 0;
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
            }
        rc = TRUE;
        break;
    case WM_MOVE:
        SaveWindowPos(hDlg,&xDPMIStatpos,&yDPMIStatpos);
        break;
    case WM_TIMER:
        UpdateProc(hDlg,0);
        break;
    default:
        break;
   }
  return rc;
}
