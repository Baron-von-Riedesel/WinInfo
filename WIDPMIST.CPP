

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
////////////////////////////////////////////////////////
// OutputIconic
////////////////////////////////////////////////////////
static void OutputIconic(HWND hWnd,HDC hDC,LPDPMIMEMORY lpdm)
{
  char str[80];
  WORD wProz1,wProz2;
  HBRUSH hBrush,xBrush;
  TEXTMETRIC tm;
  RECT rect;
  DWORD xx;
  BOOL fWin95;

        xx = GetVersion();
        fWin95 = FALSE;
        if (LOBYTE(LOWORD(xx)) == 3)
                if (HIBYTE(LOWORD(xx)) > 10)
                        fWin95 = TRUE;

   xx = lpdm->dwLinearSpace - lpdm->dwFreeLinearSpace + lpdm->dwFreePages;
   if (xx)
       wProz1 = lpdm->dwFreePages * 100 / xx;
   else
       wProz1 = 0;

   if (lpdm->dwPhysPages)
       wProz2 = lpdm->dwFreePhysPages * 100 / lpdm->dwPhysPages;
   else
       wProz2 = 0;

        if (fWin95) {
                wsprintf(str,"%u%%/%u%%",wProz1,wProz2);
                SetWindowText(hWnd,str);
                //OutputDebugString(str);
                //OutputDebugString("\r\n");
        } else {
                hBrush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
                xBrush = (HBRUSH)SelectObject(hDC,hBrush);
                GetClientRect(hWnd,&rect);
                Rectangle(hDC,rect.left,rect.top,rect.right,rect.bottom);
                SetBkMode(hDC,TRANSPARENT);

                GetTextMetrics(hDC,&tm);
                tm.tmHeight--;

                wsprintf(str,"%u%%",wProz1);
                TextOut(hDC,2,tm.tmHeight*0,str,strlen(str));

                wsprintf(str,"%u%%",wProz2);
                TextOut(hDC,2,tm.tmHeight*1,str,strlen(str));

                SelectObject(hDC,xBrush);
        }
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Update (Timer)                                       บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void UpdateProc(HWND hDlg,int initf)
{
  static DPMIMEMORY dm1 = {0,0,0,0,0,0,0,0,0,0};
  static WORD dosaltmem = 0xFFFF;
  WORD dosmem;
  char str[80];
//  char str1[80];
  int x;

    if (initf) {
        memset(&dm1,0xFF,sizeof(dm1));
        dosaltmem = 0xFFFF;
    }

    if (!wFlag)
        DPMIGetMemoryInfo(&dm);
    else
        memset(&dm,0xFF,sizeof(dm));

    x = 0;
    if (IsIconic(hDlg)) {
        InvalidateRect(hDlg,0,1);
        return;
    }                        //  freier linearer Adressraum
    if (dm.dwFreeLinearSpace != dm1.dwFreeLinearSpace) {
        wsprintf(str,fstr[x],dm.dwFreeLinearSpace<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;                        // freie lineare Pages
    if (dm.dwFreePages != dm1.dwFreePages) {
        wsprintf(str,fstr[x],dm.dwFreePages<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;                        // groesster freier Block in Bytes
    if (dm.maxFreeBytes != dm1.maxFreeBytes) {
        wsprintf(str,fstr[x],dm.maxFreeBytes>>10);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;                        // linearer Adressraum
    if (dm.dwLinearSpace != dm1.dwLinearSpace) {
        wsprintf(str,fstr[x],dm.dwLinearSpace<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;                        // groesster freier + verriegelbarer Block
    if (dm.dwMaxAllocLockLinPages != dm1.dwMaxAllocLockLinPages) {
        wsprintf(str,fstr[x],dm.dwMaxAllocLockLinPages<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;                        // phys. Pages, die nicht fix zugeordnet sind
    if (dm.dwUnlockedPhysPages != dm1.dwUnlockedPhysPages) {
        wsprintf(str,fstr[x],dm.dwUnlockedPhysPages<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;                        // phys. Pages im freien Pool
    if (dm.dwFreePhysPages != dm1.dwFreePhysPages) {
        wsprintf(str,fstr[x],dm.dwFreePhysPages<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;                        // phys. Pages insgesamt
    if (dm.dwPhysPages != dm1.dwPhysPages) {
        wsprintf(str,fstr[x],dm.dwPhysPages<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;
    if (dm.numSwapPages != dm1.numSwapPages) {
        wsprintf(str,fstr[x],dm.numSwapPages<<2);
        SetDlgItemText(hDlg,x+ID_DPMISTAT1,str);
    }

    x++;
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

   rc = FALSE;

   switch (message)
     {
      case WM_ICONERASEBKGND:
        break;
      case WM_SYSCOMMAND:
        if (wParam == SC_MINIMIZE)
            SetWindowText(hWnd,"VMem/RMem");
        else
        if (wParam == SC_RESTORE)
            SetWindowText(hWnd,szMonText);
        rc = CallWindowProc(fpDPMIStatWndProc,hWnd,message,wParam,lParam);
        break;
      case WM_PAINT:
        if (!IsIconic(hWnd)) {
                rc = CallWindowProc(fpDPMIStatWndProc,hWnd,message,wParam,lParam);
                break;
            }
      case WM_PAINTICON:
        BeginPaint(hWnd,(LPPAINTSTRUCT)&ps);
        OutputIconic(hWnd,ps.hdc,&dm);
        EndPaint(hWnd,(LPPAINTSTRUCT)&ps);
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
