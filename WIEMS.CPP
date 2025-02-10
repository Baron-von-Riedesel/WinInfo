
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "ems.h"
#include "user.h"
#include "user386.h"
#include "dpmi.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

int  xEMSpos = 100;
int  yEMSpos = 100;

extern BYTE fShow;
extern HFONT hFontAlt;
extern HWND hWndEMS;

BOOL CALLBACK ViewDlg      (HWND, UINT, WPARAM, LPARAM);

typedef struct {
  FARPROC proc;
  WORD    parms;
  LPWORD  parmptr;
  DWORD   rc;
 } RMCALL;

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ CALLRMProc - Funktion im real mode aufrufen          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL CallRMProc(RMCALL *prmcall,WORD wStart)
{
 DWORD dwParm;
 WORD  wSegSize;
 RMCSTRUCT rmcstruct;
 DWORD rmaddr;
 LPBYTE lpbyte;

 prmcall->rc = 0;
 wSegSize    = 0;
 _asm {
       mov bx,prmcall
       mov ax,word ptr [bx.proc+2]
       lsl ax,ax
       jnz short label1
       sub ax,wStart
       mov wSegSize,ax
label1:
      }
 if (!wSegSize)
     return FALSE;

 if (wSegSize > 0x700)
     wSegSize = 0x700;


 if (dwParm = GlobalDosAlloc(0x800))
   {
    lpbyte       = (LPBYTE)MAKELP(LOWORD(dwParm),0);
    _lmemcpy(lpbyte,(LPBYTE)MAKELONG(wStart,HIWORD(prmcall->proc)),wSegSize);   /* code umkopieren */
    rmcstruct.rSS  = HIWORD(dwParm);
    rmcstruct.rSP  = 0x7F0;
    rmaddr = MAKELONG(LOWORD(prmcall->proc)-wStart,HIWORD(dwParm));
    DPMICallRMFarProc(rmaddr,&rmcstruct,prmcall->parms,prmcall->parmptr);
    prmcall->rc = rmcstruct.rEAX;
    GlobalDosFree(LOWORD(dwParm));
    return TRUE;
   }
 return FALSE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ GetEMSPages - EMS Pages lesen                        บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
HGLOBAL GetEMSPages(HWND hDlg,WORD wHandle,WORD wPages)
{
 HGLOBAL rc = 0;
 WORD wFrame;
 RMCALL rmcall;
 WORD i;
 LPBYTE lpByte,lpByte2;
 WORD hSel;
 WORD rmcw[3];

 rmcall.parms = 0;
 rmcall.proc = (FARPROC)_EMS_GetPageFrame;
 CallRMProc(&rmcall,LOWORD(_EMS_Check));

 if (!(wFrame = (WORD)rmcall.rc))
    {
     CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERREMS3),0,MB_OK);
     return (HGLOBAL)0;
    }
 if (!(rc = GlobalAlloc(GMEM_MOVEABLE,(DWORD)wPages*0x4000)))
    return (HGLOBAL)0;
 lpByte = (LPBYTE)GlobalLock(rc);

 hSel = AllocBigDescriptor((DWORD)wFrame<<4,0x4000);
 lpByte2 = (LPBYTE)MAKELP(hSel,0);

 for (i = 0;i < wPages;i++)
   {
    rmcall.proc  = (FARPROC)_EMS_MapPages;
    rmcall.parms = 3;
    rmcw[0]      = i;
    rmcw[1]      = 0;
    rmcw[2]      = wHandle;
    rmcall.parmptr = rmcw;
//    rmcall.rc = 1;
    CallRMProc(&rmcall,LOWORD(_EMS_Check));
    if (rmcall.rc)
       {
        _fmemcpy(lpByte,lpByte2,0x4000);
        lpByte = lpByte + 0x4000;
       }
   }
 FreeBigDescriptor(hSel);
 return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ EMSDlg - EMS Dialog                                  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
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
 RMCALL rmcall;


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
              if (sscanf(szStr,"%X %u",&x,&y) == 2)
                 {
                  wsprintf(szStr,"EMS Handle: %X, Pages: %u)", x, y);
                  if (!(hdp.hGlobal = GetEMSPages(hDlg,x,y)))
                     break;
                  hdp.dwLength = 0;
                  hdp.dwOffset = 0;
                  hdp.wType = HDP_FREEHDL;
                  hWnd = CreateDialogParam(hInst,
                                        MAKEINTRESOURCE(IDD_VIEW),
                                        hDlg,
                                        ViewDlg,
                                        (LPARAM)(LPVOID)&hdp
                                        );
                  SetWindowText(hWnd,szStr);
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
              if (!_EMS_Check())
                 {
                  CreateMessage(GetParent(hDlg),MAKEINTRESOURCE(IDS_ERREMS1),0,MB_OK);
                  break;
                 }
              rmcall.proc  = (FARPROC)_EMS_GetVersion;
              rmcall.parms = 0;
              CallRMProc(&rmcall,LOWORD(_EMS_Check));
              version = (WORD)rmcall.rc;
              if (!version)
                 {
                  CreateMessage(GetParent(hDlg),MAKEINTRESOURCE(IDS_ERREMS2),0,MB_OK);
                  break;
                 }
              i = 0;
              for (y=0;y<0x100;y++)
                 {
                  rmcall.proc  = (FARPROC)_EMS_GetHandlePages;
                  rmcall.parms = 1;
                  rmcall.parmptr = &y;
                  CallRMProc(&rmcall,LOWORD(_EMS_Check));
                  if (x = (WORD)rmcall.rc)
                     {
                      wsprintf(szStr,
                               "%u\t%u(%u)",
                               y,
                               x,
                               x * 16
                              );
                      i++;
                      SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)szStr);
                     }
                 }

              rmcall.proc = (FARPROC)_EMS_GetFreePages;
              rmcall.parms = 0;
              CallRMProc(&rmcall,LOWORD(_EMS_Check));
              x  = (WORD)rmcall.rc;

              rmcall.proc = (FARPROC)_EMS_GetPages;
              CallRMProc(&rmcall,LOWORD(_EMS_Check));
              y  = (WORD)rmcall.rc;

              wsprintf(szStr,
                       "Freie Seiten\t%u\tSeiten insg.\t%u\t",
                       x,
                       y
                      );
              SetDlgItemText(hDlg,ID_STATUS1,szStr);
              rmcall.proc = (FARPROC)_EMS_GetPageFrame;
              CallRMProc(&rmcall,LOWORD(_EMS_Check));
              x  = (WORD)rmcall.rc;

              EnableDlgItem(hDlg,ID_SUBDLG1,x && i);
              if (x && i)
                 PostMessage(hWnd,LB_SETCURSEL,0,0);

              rmcall.proc = (FARPROC)_EMS_GetTotalHandles;
              CallRMProc(&rmcall,LOWORD(_EMS_Check));
              y  = (WORD)rmcall.rc;
              wsprintf(szStr,
                       "Page Frame\t%04X\tHandles\t%u\t",
                       x,
                       y
                      );
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
