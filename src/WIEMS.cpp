
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "user.h"
#include "user386.h"
#include "dpmi.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

#define VCPIHOST 1 /* 1=assume EMS (re)mapping works in V86-mode only (0 is ok for Windows VMM) */

int  xEMSpos = 100;
int  yEMSpos = 100;

extern BYTE fShow;
extern HFONT hFontAlt;
extern HWND hWndEMS;

BOOL CALLBACK ViewDlg      (HWND, UINT, WPARAM, LPARAM);

BOOL EMSCheck(void)
///////////////////
{
    LPSTR lpByte;
    DWORD rmint;
    char *pEMM = "EMMXXXX0";
    int i;

    if ( rmint = DPMIGetRMIntVector(0x67) ) {
        lpByte = (LPSTR)MAKELONG(0x000A,DPMISegToDescriptor(HIWORD(rmint)));
        for (i = 0; i < 8; i++, lpByte++, pEMM++)
            if ( *lpByte != *pEMM )
                break;
        if ( i == 8 ) return 1;
    }
    return 0;
}
/*
 * EMSCall - call EMS real-mode function
 */
static BOOL EMSCall(RMCSTRUCT *rmcs)
////////////////////////////////////
{
    DWORD dwParm;
    DWORD rmaddr;
    LPBYTE lpByte;

    if (dwParm = GlobalDosAlloc(0x10)) {
        lpByte  = (LPBYTE)MAKELP(LOWORD(dwParm),0);
        *lpByte = 0xCD;
        *(lpByte+1) = 0x67;
        *(lpByte+2) = 0xCB;
        rmaddr = MAKELONG(LOWORD(lpByte),HIWORD(dwParm));
        rmcs->rFlags = 0x202;
        rmcs->rSP  = 0;
        rmcs->rSS  = 0;
        DPMICallRMFarProc(rmaddr,rmcs,0,0);
        GlobalDosFree(LOWORD(dwParm));
        return TRUE;
    }
    return FALSE;
}
/*
 * GetEMSPages - read pages of an EMS handle into memory.
 * This function will only work for DPMI hosts that have full control
 * of conventional memory mapping - that is, not for DPMI host running
 * as VCPI clients.
 */
static BOOL GetEMSPages(HWND hDlg,WORD wHandle,WORD wPages, HGLOBAL hMem)
/////////////////////////////////////////////////////////////////////////
{
    WORD wFrame;
    RMCSTRUCT rmcs;
    WORD i;
    LPBYTE lpByte,lpByte2;
    FWORD dest,src;
    WORD hSel;
    DWORD dwParm;

    if (!wPages)
        return 0;

    rmcs.rAH = 0x41;   //get page frame
    if ((!EMSCall(&rmcs)) || (rmcs.rAH != 0) || (rmcs.rBX == 0) ) {
        CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERREMS3),0,MB_OK);
        return 0;
    }
    wFrame = rmcs.rBX;

    lpByte = (LPBYTE)GlobalLock(hMem);

    /* alloc a 16-kB descriptor that covers EMS page 0 (first page of page frame) */
#if VCPIHOST
    if ((dwParm = GlobalDosAlloc(0x4010)) == 0 )
        return 0;
    lpByte2 = (LPBYTE)MAKELP(LOWORD(dwParm),0);
    *(lpByte2+0x4000) = 0xf3; /* f3 66 a5 = rep movsd */
    *(lpByte2+0x4001) = 0x66;
    *(lpByte2+0x4002) = 0xA5;
    *(lpByte2+0x4003) = 0xCB; /* cb = retf */
#else
    hSel = AllocBigDescriptor( (DWORD)wFrame << 4, 0x4000 - 1);
    lpByte2 = (LPBYTE)MAKELP(hSel,0);
#endif
    dest.selector = HIWORD(lpByte);
    src.selector = HIWORD(lpByte2);
    src.offset = 0;
    for (i = 0, dest.offset = 0;i < wPages;i++) {
        rmcs.rAX  = 0x4400;    /* map page into physical page 0 */
        rmcs.rDX  = wHandle;
        rmcs.rBX  = i;         /* logical page */
        if ( EMSCall(&rmcs) && (rmcs.rAH == 0)) {
#if VCPIHOST
            /* copy in v86-mode */
            rmcs.rSI = 0;
            rmcs.rDI = 0;
            rmcs.rCX = 0x4000/4;
            rmcs.rDS = wFrame;
            rmcs.rES = HIWORD(dwParm);
            DPMICallRMFarProc(MAKELONG(0x4000,HIWORD(dwParm)),&rmcs,0,0);
#endif
            _hmemcpy32(dest,src,0x4000);
            dest.offset += 0x4000;
        }
    }
#if VCPIHOST
    GlobalDosFree(LOWORD(dwParm));
#endif
    FreeBigDescriptor(hSel);
    return 1;
}
/*
 * EMSDlg - EMS Dialog
 */
BOOL EXPORTED CALLBACK EMSDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_SUBDLG1,
                        0};
 char  szStr[80];
 int   x,i;
 WORD  y;
 WORD  version;
// DWORD dw;
 HWND  hWnd;
// LPBYTE lpbyte;
 int tabpos[5];
 BOOL rc = FALSE;
 HEXDUMPPARM hdp;
 RMCSTRUCT rmcs;


 switch (message)
    {
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_SUBDLG1:
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              if ((x = (WORD)ListBox_GetCurSel(hWnd)) == LB_ERR)
                 {
                  MessageBeep(0);
                  break;
                 }
              ListBox_GetText(hWnd,x,szStr);
              if ((sscanf(szStr,"%X %u",&x,&y) == 2) && y) {
                  if ( hdp.hGlobal = GlobalAlloc(GMEM_MOVEABLE,(DWORD)y*0x4000)) {
                      if ( GetEMSPages( hDlg, x, y, hdp.hGlobal ) ) {
                          hdp.dwLength = 0;
                          hdp.dwOffset = 0;
                          hdp.wType = HDP_FREEHDL;
                          hWnd = CreateDialogParam(hInst,
                                                   MAKEINTRESOURCE(IDD_VIEW),
                                                   hDlg,
                                                   ViewDlg,
                                                   (LPARAM)(LPVOID)&hdp
                                                  );
                          wsprintf(szStr,"EMS Handle: %X, Pages: %u)", x, y);
                          SetWindowText(hWnd,szStr);
                      } else
                          GlobalFree( hdp.hGlobal );
                  }
              }
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                {
                 case LBN_SELCHANGE:
                   EnableDlgItem(hDlg,
                                 ID_SUBDLG1,
                                 LB_ERR != SendMessage((HWND)LOWORD(lParam),
                                               LB_GETCURSEL,0,0));
                   break;
                 case LBN_DBLCLK:
                   SendMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                   break;
                  case XLBN_RBUTTONDOWN:
                    TrackPopup(hDlg,BtnTab);
                    break;
                }
              break;
         case ID_REFRESH:
              fShow = FALSE;
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);
              if (!EMSCheck())
                 {
                  CreateMessage(GetParent(hDlg),MAKEINTRESOURCE(IDS_ERREMS1),0,MB_OK);
                  break;
                 }
              version = 0;
              rmcs.rAH  = 0x46;   /* get version */
              if ( EMSCall(&rmcs) && rmcs.rAH == 0 )
                  version = rmcs.rAL;
              if (!version) {
                  CreateMessage(GetParent(hDlg),MAKEINTRESOURCE(IDS_ERREMS2),0,MB_OK);
                  break;
                 }
              i = 0;
              for (y=0;y<0x100;y++)
                 {
                  rmcs.rAH  = 0x4C; /* get handle pages */
                  rmcs.rDX  = y;    /* handle */
                  if ( EMSCall(&rmcs) && (rmcs.rAH == 0)) {
                      wsprintf(szStr,"%u\t%u(%u)", y, rmcs.rBX, rmcs.rBX * 16 );
                      i++;
                      SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)szStr);
                     }
                 }

              rmcs.rAH = 0x42; /* get free pages in BX, total pages in DX */
              if ( EMSCall(&rmcs) && (rmcs.rAH == 0)) {
                  x  = rmcs.rBX;
                  y  = rmcs.rDX;
              } else {
                  x = 0;
                  y = 0;
              }
              wsprintf(szStr, "Freie Seiten\t%u\tSeiten insg.\t%u\t", x, y );
              SetDlgItemText(hDlg,ID_STATUS1,szStr);
              rmcs.rAH = 0x41;
              if ( EMSCall(&rmcs) && (rmcs.rAH == 0) )
                  x = rmcs.rBX;
              else
                  x = 0;

              EnableDlgItem(hDlg,ID_SUBDLG1,x && i);
              if (x && i)
                 PostMessage(hWnd,LB_SETCURSEL,0,0);

              rmcs.rAX = 0x5402; /* get total handles (v4.0) */
              if ( EMSCall(&rmcs) && (rmcs.rAH == 0) )
                  y = (WORD)rmcs.rBX;
              else
                  y = 0;
              wsprintf(szStr, "Page Frame\t%04X\tHandles\t%u\t", x, y );
              SetDlgItemText(hDlg,ID_STATUS2,szStr);
              wsprintf(szStr,
                       "Version\t%u.%u\t",
                       version>>4,
                       version&0xF
                      );
              SetDlgItemText(hDlg,ID_STATUS3,szStr);
              fShow = TRUE;
              break;
         }
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xEMSpos,&yEMSpos);
      break;
    case WM_DESTROY:
      hWndEMS = 0;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_INITDIALOG:
      tabpos[0] = 40;
      tabpos[1] = 60;
      tabpos[2] = 90;
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,3,(LPARAM)(LPVOID)&tabpos);
      SendDlgItemMessage(hDlg,
                         ID_LISTBOX1,
                         XLB_SETEXTSTYLE,
                         XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      tabpos[0] = 50;
      tabpos[1] = 75;
      tabpos[2] = 125;
      tabpos[3] = 150;
      SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,4,(LPARAM)(LPVOID)&tabpos);
      SendDlgItemMessage(hDlg,ID_STATUS2,ST_SETTABSTOPS,4,(LPARAM)(LPVOID)&tabpos);
      SendDlgItemMessage(hDlg,ID_STATUS3,ST_SETTABSTOPS,4,(LPARAM)(LPVOID)&tabpos);
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      if (fShow)
         {
          SetWindowPos(hDlg,0,xEMSpos,yEMSpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
          ShowWindow(hDlg,SW_SHOWNORMAL);
         }
      else
          PostMessage(hDlg,WM_CLOSE,0,0);
      rc = TRUE;
      break;
    default:
      break;
    }
 return rc;
}
