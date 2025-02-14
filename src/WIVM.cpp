
#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "user386.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"
#include "vmm.h"
#include "resource.h"

// #define strcat lstrcat
// #define strcpy lstrcpy

extern HFONT hFontAlt;
extern int fGetVMPSP;
int    xVMpos  = 100;
int    yVMpos  = 100;


BOOL CALLBACK ViewDlg   (HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI GetPageTable(DWORD dwAddr);
BOOL CALLBACK PageTabDlg(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK IDTDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK PMIVDlg    (HWND, UINT, WPARAM, LPARAM);

extern HWND hWndENHVM;

static char fClientStr1[] = {"\teax=%lX,ebx=%lX,ecx=%lX,edx=%lX,esi=%lX,edi=%lX,ebp=%lX,efl=%lX"};
static char fClientStr2[] = {"\tcs:eip=%lX:%lX,ss:esp=%lX:%lX,ds=%lX,es=%lX,fs=%lX,gs=%lX"};

void EnableVMItems(HWND hDlg,WORD wMode,VMENTRY *pVMEntry)
//////////////////////////////////////////////////////////
{
    DWORD vmflags = pVMEntry->dwFlags;
 EnableDlgItem(hDlg,ID_SUBDLG1,wMode);  // View pagetab
 EnableDlgItem(hDlg,ID_SUBDLG2,wMode);  // View VM Control block
 EnableDlgItem(hDlg,ID_SUBDLG3,wMode);  // View V86 High
 EnableDlgItem(hDlg,ID_VMSWITCH,wMode && (!(vmflags & VMStat_Not_Executeable)));
 EnableDlgItem(hDlg,ID_VMDESTROY,wMode);
 EnableDlgItem(hDlg,IDC_PMIV,wMode && (vmflags & VMStat_PM_App) && pVMEntry->dwPMIntVecs);
 EnableDlgItem(hDlg,IDC_IDT,wMode && pVMEntry->dwIDTAddr);
 
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ VMDlg - VMs auflisten                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK VMDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
  static int BtnTab[] = {ID_VMSWITCH,
                         ID_SUBDLG2,
                         ID_SUBDLG1,
                         ID_SUBDLG3,
                         ID_VMDESTROY,
                         0};
  WORD x;
  HWND hWnd;
  char str[132];
  char str1[20];
  DWORD dwHandle;
  DWORD dwPSPAddr;
  DWORD dwSDAFlat;
  WORD  wPSP;
  WORD  wWinPSP;
  VMENTRY vmentry;
  HEXDUMPPARM hdp;
  LPVOID lpvoid;
  DWORD  dwRegs[18];
  BOOL rc;

  rc = FALSE;

  switch (message)
     {
      case WM_INITDIALOG:
         LoadTabs(IDUS_48,str);
         SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
         SendDlgItemMessage(hDlg,ID_STATUS2,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
         SendDlgItemMessage(hDlg,ID_STATUS3,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
         LoadTabs(IDUS_49,str);
         SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
         SendDlgItemMessage(hDlg,
                            ID_LISTBOX1,
                            XLB_SETEXTSTYLE,
                            XLBES_RBUTTONTRACK,
                            (LPARAM)(LPVOID)hFontAlt);
         if (fGetVMPSP)
            {
             GetDlgItemText(hDlg,ID_LISTBOX1,str,sizeof(str));
             strcat(str,"\takt. PSP");
             SetDlgItemText(hDlg,ID_LISTBOX1,str);
            }
         SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
         SetWindowPos(hDlg,0,xVMpos,yVMpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
         ShowWindow(hDlg,SW_SHOWNORMAL);
         rc = TRUE;
         break;
      case WM_COMMAND:
         switch (wParam)
           {
           case IDOK:
           case IDCANCEL:
               PostMessage(hDlg,WM_CLOSE,0,0);
               break;
           case ID_SUBDLG1:
               hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
               x =  (WORD)SendMessage(hWnd,LB_GETCURSEL,0,0);
               dwHandle = (DWORD)SendMessage(hWnd,
                                             LB_GETITEMDATA,
                                             x,
                                             0);
               if (VMFindHandle(dwHandle,&vmentry))
                  {
                   vmentry.dwPageTab = GetPageTable(vmentry.dwV86High);

                   if (vmentry.dwCurHandle != vmentry.dwHandle)
                       vmentry.dwPageTab = vmentry.dwPageTab | 1;

                   hWnd = CreateDialogParam(hInst,
                                            MAKEINTRESOURCE(IDD_PAGETAB),
                                            hDlg,
                                            PageTabDlg,
                                            vmentry.dwPageTab);
                   GetWindowText(hWnd,str,sizeof(str));
                   wsprintf(str1,",VM=%X",LOWORD(vmentry.dwVMID));
                   strcat(str,str1);
                   SetWindowText(hWnd,str);
                  }
               else
                   MessageBeep(0);
               break;
           case ID_SUBDLG2:
               hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
               x =  (WORD)SendMessage(hWnd,LB_GETCURSEL,0,0);
               dwHandle = (DWORD)SendMessage(hWnd,
                                             LB_GETITEMDATA,
                                             x,
                                             0);
               if (VMFindHandle(dwHandle,&vmentry))
                  {
                   hdp.dwOffset = dwHandle;
                   hdp.dwLength = 0x1000;
                   hdp.wType    = HDP_LINEAR;
                   hWnd = CreateDialogParam(hInst,
                                            MAKEINTRESOURCE(IDD_VIEW),
                                            hDlg,
                                            ViewDlg,
                                            (LPARAM)(LPVOID)&hdp);
                   if (hWnd)
                      {
                       wsprintf(str,"VM: %lX",dwHandle);
                       SetWindowText(hWnd,str);
                      }
                  }
               else
                   MessageBeep(0);
               break;
           case ID_SUBDLG3:
               hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
               x =  (WORD)SendMessage(hWnd,LB_GETCURSEL,0,0);
               dwHandle = (DWORD)SendMessage(hWnd,
                                             LB_GETITEMDATA,
                                             x,
                                             0);
               if (VMFindHandle(dwHandle,&vmentry))
                  {
                   hdp.dwOffset = vmentry.dwV86High;
                   hdp.hGlobal  = 0;
                   hdp.dwLength = 0x000110000;
                   hdp.wType    = HDP_LINEAR;
                   hWnd = CreateDialogParam(hInst,
                                            MAKEINTRESOURCE(IDD_VIEW),
                                            hDlg,
                                            ViewDlg,
                                            (LPARAM)(LPVOID)&hdp);
                   if (hWnd)
                      {
                       wsprintf(str,"V86High VM: %lX",dwHandle);
                       SetWindowText(hWnd,str);
                      }
                  }
               else
                   MessageBeep(0);
               break;
           case ID_VMSWITCH:
               x =  (WORD)SendDlgItemMessage(hDlg,
                                             ID_LISTBOX1,
                                             LB_GETCURSEL,
                                             0,
                                             0);
               if (x == LB_ERR)
                   MessageBeep(0);
               else {
                   dwHandle = (DWORD)SendDlgItemMessage(hDlg,
                                                        ID_LISTBOX1,
                                                        LB_GETITEMDATA,
                                                        x,
                                                        0);
                   if (VMFindHandle(dwHandle,&vmentry) && SetVMFocus(dwHandle))
                       ;
                   else
                       MessageBeep(0);
               }
               break;
           case ID_VMDESTROY:
               x =  (WORD)SendDlgItemMessage(hDlg,
                                             ID_LISTBOX1,
                                             LB_GETCURSEL,
                                             0,
                                             0);
               if (x == LB_ERR)
                   MessageBeep(0);
               else
                  {
                   dwHandle = (DWORD)SendDlgItemMessage(hDlg,
                                                        ID_LISTBOX1,
                                                        LB_GETITEMDATA,
                                                        x,
                                                        0);
                   if (VMFindHandle(dwHandle,&vmentry))
                       DestroyVM(dwHandle);
                  }
               break;
           case IDC_IDT:
               x =  (WORD)SendDlgItemMessage(hDlg, ID_LISTBOX1, LB_GETCURSEL, 0, 0);
               if (x == LB_ERR)
                   MessageBeep(0);
               else {
                   dwHandle = (DWORD)SendDlgItemMessage(hDlg,
                                                        ID_LISTBOX1,
                                                        LB_GETITEMDATA,
                                                        x,
                                                        0);
                   if (VMFindHandle(dwHandle,&vmentry))
                       hWnd = CreateDialogParam(hInst,
                                            MAKEINTRESOURCE(IDD_SYSIDT),
                                            hDlg,
                                            IDTDlg,
                                            (LPARAM)(LPVOID)&vmentry);
                }
                break;
           case IDC_PMIV:
               x =  (WORD)SendDlgItemMessage(hDlg, ID_LISTBOX1, LB_GETCURSEL, 0, 0);
               if (x == LB_ERR)
                   MessageBeep(0);
               else {
                   dwHandle = (DWORD)SendDlgItemMessage(hDlg,
                                                        ID_LISTBOX1,
                                                        LB_GETITEMDATA,
                                                        x,
                                                        0);
                   if (VMFindHandle(dwHandle,&vmentry))
                       /* does VM run a protected-mode app? */
                       if (vmentry.dwFlags & VMStat_PM_App)
                           hWnd = CreateDialogParam(hInst,
                                                    MAKEINTRESOURCE(IDD_PMIV),
                                                    hDlg,
                                                    PMIVDlg,
                                                    (LPARAM)(LPVOID)dwHandle);
                       else
                           MessageBeep(0);
                }
                break;
           case ID_REFRESH:
               hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
               SendMessage(hWnd,LB_RESETCONTENT,0,0);

               lpvoid = GetSDAAddr();
               dwSDAFlat = GetSelectorBase(HIWORD(lpvoid)) + LOWORD(lpvoid);

               dwPSPAddr = dwSDAFlat + 0x0010;
               wWinPSP  = (WORD)(GetSelectorBase(GetKernelPDB())>>4);

               x = VMFirst(&vmentry);
               while (x)
                  {
                   str1[0] = 0;
                   if (fGetVMPSP)
                      {
                       GetFlatAddrBytes((LPVOID)&wPSP,vmentry.dwV86High+dwPSPAddr,2);
                       if ((vmentry.dwVMID == 1) && (wPSP != wWinPSP))
                           GetFlatAddrBytes((LPVOID)&str1,vmentry.dwV86High+(DWORD)wPSP*16-14,8);
                       else
                           GetFlatAddrBytes((LPVOID)&str1,vmentry.dwV86High+(DWORD)wPSP*16-8,8);
                       str1[8] = 0;
                      }
                   wsprintf(str,
                            "%08lX\t%04lX\t%08lX\t%08lX\t%08lX\t%08lX\t%08lX\t%s",
                            vmentry.dwHandle,
                            vmentry.dwVMID,
                            vmentry.dwFlags,
                            vmentry.dwV86High,
                            vmentry.dwClient,
                            vmentry.dwIDTAddr,
                            vmentry.dwPMIntVecs,
                            (LPSTR)str1
                           );
                   x = (WORD)SendMessage(hWnd,LB_ADDSTRING,0,(LPARAM)(LPSTR)str);
                   SendMessage(hWnd,LB_SETITEMDATA, x,vmentry.dwHandle);
                   x = VMNext(&vmentry);
                  }
               VMClose(&vmentry);
               SendMessage(hWnd,LB_SETCURSEL, 0, 0);
               break;
           case ID_LISTBOX1:
               switch(HIWORD(lParam))
                   {
                    case LBN_SELCHANGE:
                        x =  (WORD)SendDlgItemMessage(hDlg,
                                                      ID_LISTBOX1,
                                                      LB_GETCURSEL,
                                                      0,
                                                      0);
                        if (x == LB_ERR)
                           {
                            strcpy(str,"\tkeine VM gefunden");
                            EnableVMItems(hDlg,0,NULL);
                           }
                        else
                           {
                            dwHandle = (DWORD)SendDlgItemMessage(hDlg,
                                                                 ID_LISTBOX1,
                                                                 LB_GETITEMDATA,
                                                                 x,
                                                                 0);
                            if (VMFindHandle(dwHandle,&vmentry))
                               {
                                str[0] = 0;
                                if (vmentry.dwFlags & VMStat_Exclusive)
                                   strcat(str,",excl");
                                if (vmentry.dwFlags & VMStat_Background)
                                   strcat(str,",back");
                                if (vmentry.dwFlags & VMStat_Creating)
                                   strcat(str,",creating");
                                if (vmentry.dwFlags & VMStat_Suspended)
                                   strcat(str,",susp");
                                if (vmentry.dwFlags & VMStat_Not_Executeable)
                                   strcat(str,",not_executeable");
                                if (vmentry.dwFlags & VMStat_PM_Exec)
                                   strcat(str,",pm_exec");
                                if (vmentry.dwFlags & VMStat_PM_App)
                                   strcat(str,",pm_app");
                                if (vmentry.dwFlags & VMStat_PM_Use32)
                                   strcat(str,",pm_use32");
                                if (vmentry.dwFlags & VMStat_VxD_Exec)
                                   strcat(str,",vxd_exec");
                                if (vmentry.dwFlags & VMStat_High_Pri_Back)
                                   strcat(str,",high_pri_back");
                                if (vmentry.dwFlags & VMStat_Blocked)
                                   strcat(str,",blocked");
                                if (vmentry.dwFlags & VMStat_PageableV86)
                                   strcat(str,",pageablev86");
                                if (vmentry.dwFlags & VMStat_V86IntsLocked)
                                   strcat(str,",v86intslocked");
                                if (vmentry.dwFlags & VMStat_Idle)
                                   strcat(str,",idle");
                                str[0] = 9;
                                GetFlatAddrBytes((LPVOID)&dwRegs,vmentry.dwClient,18*4);
                                EnableVMItems(hDlg,1,&vmentry);
                               }
                            else
                               {
                                strcpy(str,"\tVM nicht gefunden");
                                EnableVMItems(hDlg,0,0);
                                x = LB_ERR;
                               }
                           }
                        SetDlgItemText(hDlg,ID_STATUS1,str);
                        if (x == LB_ERR)
                            str[0] = 0;
                        else
                            wsprintf(str,
                                 fClientStr1,
                                 dwRegs[7],   // eax
                                 dwRegs[4],   // ebx
                                 dwRegs[6],   // ecx
                                 dwRegs[5],   // edx
                                 dwRegs[1],   // esi
                                 dwRegs[0],   // edi
                                 dwRegs[2],   // ebp
                                 dwRegs[11]   // efl
                                );
                        SetDlgItemText(hDlg,ID_STATUS2,str);
                        if (x == LB_ERR)
                            str[0] = 0;
                        else
                            wsprintf(str,
                                 fClientStr2,
                                 dwRegs[10],  // cs
                                 dwRegs[9],   // eip
                                 dwRegs[13],  // ss
                                 dwRegs[12],  // esp
                                 dwRegs[15],  // ds
                                 dwRegs[14],  // es
                                 dwRegs[16],  // fs
                                 dwRegs[17]   // gs
                                );
                        SetDlgItemText(hDlg,ID_STATUS3,str);
                        break;
                    case LBN_DBLCLK:
                        PostMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                        break;
                    case XLBN_RBUTTONDOWN:
                        TrackPopup(hDlg,BtnTab);
                        break;
                   }
               break;
           }                    /* end switch wParam */
         rc = TRUE;             /* end WM_COMMAND */
         break;
      case WM_DESTROY:
         hWndENHVM = 0;
         break;
      case WM_MOVE:
        SaveWindowPos(hDlg,&xVMpos,&yVMpos);
         break;
      case WM_CLOSE:
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
     }                          /* end switch message */
  return rc;
}
