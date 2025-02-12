
#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "user386.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"

// #define strcpy lstrcpy
// #define strcat lstrcat

extern "C" {
// DWORD FAR PASCAL GetCR0(void);
DWORD FAR PASCAL GetCR3(void);
};

extern BYTE fShow;
extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT hFontAlt;

BOOL CALLBACK ViewDlg      (HWND, UINT, WPARAM, LPARAM);
extern HWND hWndSysPage;

DWORD dwCR3 = -1;
DWORD dwPageDir = -1;

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ PageTabDlg - Page Table ausgeben                     บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK PageTabDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
  static int BtnTab[] = {ID_SUBDLG1,
                        0};
  WORD x;
  DWORD dwLinBase;
  DWORD dwAddr;
  HEXDUMPPARM hdp;
  char szStr[80];
  char str1[40];
  DWORD dwPageTab;
  DWORD dwPhysAddr;
  LPDWORD lpdword;
  PAGEENTRY pageentry;
  HWND hWnd;
  WORD sel;
  BOOL rc;
  BYTE fEntryOK;

  rc = FALSE;

  switch (message)
     {
      case WM_INITDIALOG:
         SetWindowLong(hDlg,DLGWINDOWEXTRA,lParam);
         LoadTabs(IDUS_59,szStr);
         SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
         SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                            (LPARAM)(LPVOID)hFontAlt);

         SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);

         dwLinBase = (lParam & 0x00000FFC)<<20;
         LoadString(hInst,IDS_PAGELST1,str1,sizeof(str1));
         wsprintf(szStr,str1,dwLinBase);
         SetWindowText(hDlg,szStr);

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
           case ID_LISTBOX1:
               switch (HIWORD(lParam))
                  {
                   case LBN_DBLCLK:
                      PostMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                      break;
                   case XLBN_RBUTTONDOWN:
                      TrackPopup(hDlg,BtnTab);
                      break;
                  }
               break;
           case ID_SUBDLG1:
               dwPageTab = GetWindowLong(hDlg,DLGWINDOWEXTRA) & 0x00000001;
               x =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
               if (x == LB_ERR)
                  { MessageBeep(0); break;}
               dwAddr = (DWORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,x,0);
               if (dwPageTab)
                  if (dwAddr == 0xFFFFFFFF)
                      { MessageBeep(0); break;}
                  else
                  if (dwAddr <  0x000FFFFF)
                      dwPageTab = 0;
                  else
                      {
                       dwPhysAddr = dwAddr;
                       _asm {
                         pusha
                         mov byte ptr fEntryOK,0
                         mov bx,word ptr dwAddr+2
                         mov cx,word ptr dwAddr+0
                         mov si,0
                         mov di,1000h
                         mov ax,0800h
                         int 31h
                         jc  label1
                         mov word ptr dwAddr+0,cx
                         mov word ptr dwAddr+2,bx
                         mov byte ptr fEntryOK,1
                       label1:
                         popa}
                       if (!fEntryOK)
                          {MessageBeep(0);break;}
                      }

               hdp.dwOffset = dwAddr;
               hdp.dwLength = 0x1000;
               hdp.wType   = HDP_LINEAR;
               if (hWnd = CreateDialogParam( hInst,
                                             MAKEINTRESOURCE(IDD_VIEW),
                                             hDlg,
                                             ViewDlg,
                                             (LPARAM)(LPVOID)&hdp
                                            ))
                  {
                   if (dwPageTab)
                       wsprintf(szStr,"phys. Adresse: %lX",dwPhysAddr);
                   else
                       wsprintf(szStr,"Adresse: %lX",dwAddr);
                   SetWindowText(hWnd,szStr);
                  }
               break;
           case ID_REFRESH:
               SetCursor(hCursorWait);
               dwPageTab = GetWindowLong(hDlg,DLGWINDOWEXTRA);
               dwPageTab = dwPageTab & 0xFFFFF000;
               hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
               SendMessage(hWnd,LB_RESETCONTENT,0,0);

               if (!(sel = AllocBigDescriptor(dwPageTab,0x0FFF)))
                   break;
               lpdword = (LPDWORD)MAKELP(sel,0);
               dwPageTab = GetWindowLong(hDlg,DLGWINDOWEXTRA);
               dwLinBase = (dwPageTab & 0x00000FFC)<<20;
               dwPageTab = dwPageTab & 1;
               x = PageFirst(lpdword,&pageentry);
               while (x)
                  {
                   if (pageentry.wFlags & 2)
                       strcpy(str1,"r/w");
                   else
                       strcpy(str1,"r/o");
                   if (pageentry.wFlags & 4)
                       strcat(str1,",usr");
                   else
                       strcat(str1,",sys");
                   if (pageentry.wFlags & 0x20)
                       strcat(str1,",acc");
                   if (pageentry.wFlags & 0x40)
                       strcat(str1,",dirty");

                   wsprintf(szStr,
                            "%08lX\t%05lX\t%03X %s",
                            dwLinBase + ((DWORD)pageentry.wEntry<<12),
                            pageentry.dwPage,
                            pageentry.wFlags,
                            (LPSTR)str1
                           );
                   x = (WORD)SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)szStr);
                   if (dwPageTab)
                       if (pageentry.wFlags & 1)
                          SendMessage(hWnd,LB_SETITEMDATA,x,pageentry.dwPage<<12);
                       else
                          SendMessage(hWnd,LB_SETITEMDATA,x,0xFFFFFFFF);
                   else
                      SendMessage(hWnd,LB_SETITEMDATA,x,dwLinBase+((DWORD)pageentry.wEntry<<12));
                   x = PageNext(lpdword,&pageentry);
                  }

               FreeBigDescriptor(sel);
               SetCursor(hCursor);
               break;
           }                    /* end switch wParam */
         rc = TRUE;             /* end WM_COMMAND */
         break;
      case WM_CLOSE:
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
     }                          /* end switch message */
  return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ GetPageTable - Page Table fuer Adresse ermitteln     บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
DWORD WINAPI GetPageTable(DWORD dwAddr)
{
 DWORD dwCR3;
 DWORD dwPageDir;
 DWORD dwPageTab;
 DWORD FAR * lpdw;
 WORD  sel;


   dwCR3 = 0xFFFFFFFF;
   if ((!CallRing0Proc((FARPROC)GetCR3,&dwCR3)) || (dwCR3 == 0xFFFFFFFF))
       return 0;

   dwPageDir = DPMIMapPhysToLinear(dwCR3 & 0xFFFFF000,0x1000);
   if (!dwPageDir)
       return 0;

   if (!(sel = AllocBigDescriptor(dwPageDir,0x0FFF)))
       return 0;

   lpdw = (LPDWORD)(MAKELONG(0,sel));

   dwPageTab = DPMIMapPhysToLinear(*(lpdw+(dwAddr>>22)) & 0xFFFFF000,0x1000);
   FreeBigDescriptor(sel);

   return dwPageTab;
}
///////////////////////////////////////////////////////////
LPDWORD GetPageDirPtr(HWND hDlg)
{
	WORD sel;
		
	dwCR3 = 0xFFFFFFFF;
    if ((!CallRing0Proc((FARPROC)GetCR3,&dwCR3)) || (dwCR3 == 0xFFFFFFFF))
      {
       CreateMessage(hDlg,"CR3 kann nicht gelesen werden",0,MB_OK);
       return 0;
      }

    dwPageDir = DPMIMapPhysToLinear(dwCR3 & 0xFFFFF000,0x1000);
    if (!dwPageDir)
      {
       CreateMessage(hDlg,"CR3 kann nicht in lineare Adresse umgesetzt werden",0,MB_OK);
       return 0;
      }
    if (!(sel = AllocBigDescriptor(dwPageDir,0x0FFF)))
       return 0;

    return (LPDWORD)MAKELP(sel,0);
}
///////////////////////////////////////////////////////////
// PageDirDlg - Page Directory ausgeben                 
///////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK PageDirDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
  static int BtnTab[] = {ID_SUBDLG1,
                        0};
  WORD x;
  char str[80];
  char str1[40];
  DWORD dwPageTab;
  LPDWORD lpdword;
  PAGEENTRY pageentry;
  HEXDUMPPARM hdp;
  HWND hWnd;
  BOOL rc;

  rc = FALSE;

  switch (message)
     {
      case WM_INITDIALOG:
         LoadTabs(IDUS_47,str);
         SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
         LoadTabs(IDUS_60,str);
         SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
         SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                            (LPARAM)(LPVOID)hFontAlt);

         SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
         if (fShow)
             ShowWindow(hDlg,SW_SHOWNORMAL);
         else
             ShowWindow(hDlg,SW_SHOWNOACTIVATE);
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
               x =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
               if (x == LB_ERR)
                  {
                   MessageBeep(0);
                   break;
                  }
               dwPageTab = (DWORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,x,0);
               CreateDialogParam(hInst,
                                 MAKEINTRESOURCE(IDD_PAGETAB),
                                 hDlg,
                                 PageTabDlg,
                                 (DWORD)dwPageTab);
               break;
           case ID_SUBDLG2:
               hdp.dwOffset = dwPageDir;
               hdp.dwLength = 0x1000;
               hdp.wType   = HDP_LINEAR;
               if (hWnd = CreateDialogParam( hInst,
                                             MAKEINTRESOURCE(IDD_VIEW),
                                             hDlg,
                                             ViewDlg,
                                             (LPARAM)(LPVOID)&hdp
                                            ))
                  {
                   wsprintf(str,"Page Directory at %lX",dwPageDir);
                   SetWindowText(hWnd,str);
                  }
               break;
           case ID_REFRESH:
               fShow = TRUE;
               hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
               SendMessage(hWnd,LB_RESETCONTENT,0,0);
/*
               dwCR0 = 0;
               CallRing0Proc(GetCR0,&dwCR0);
               if (!(dwCR0 & 0x80000000))
                  {
                   CreateMessage(hDlg,
                                 "Paging ist nicht aktiv",
                                 0,
                                 MB_OK);
                   fShow = FALSE;
                   break;
                  }
                  */
				if (!(lpdword = GetPageDirPtr(hDlg))) {
					fShow = FALSE;
					break;
				}
               x = PageFirst(lpdword,&pageentry);
               while (x)
                  {
                   dwPageTab = DPMIMapPhysToLinear(pageentry.dwPage<<12,0x1000);

                   if (pageentry.wFlags & 2)
                       strcpy(str1,"r/w");
                   else
                       strcpy(str1,"r/o");
                   if (pageentry.wFlags & 4)
                       strcat(str1,",usr");
                   else
                       strcat(str1,",sys");
                   if (pageentry.wFlags & 0x20)
                       strcat(str1,",acc");
                   if (pageentry.wFlags & 0x40)
                       strcat(str1,",dirty");

                   wsprintf(str,
                            "%08lX-%08lX\t%05lX\t%08lX\t%03X %s",
                            (DWORD)pageentry.wEntry<<22,
                            ((DWORD)pageentry.wEntry<<22)+0x3FFFFF,
                            pageentry.dwPage,
                            dwPageTab,
                            pageentry.wFlags,
                            (LPSTR)str1
                           );
                   x = (WORD)SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                   SendMessage(hWnd,LB_SETITEMDATA,x,dwPageTab | (pageentry.wEntry<<2));
                   x = PageNext(lpdword,&pageentry);
                  }

               FreeBigDescriptor(HIWORD(lpdword));
               wsprintf(str,
                        "CR3\t%08lX\tPageDir\t%08lX",
                        dwCR3,
                        dwPageDir
                       );
               SetDlgItemText(hDlg,ID_STATUS1,str);
               PostMessage(hWnd,LB_SETCURSEL,0,0);
               break;
           case ID_LISTBOX1:
               switch(HIWORD(lParam))
                   {
                    case LBN_DBLCLK:
                      SendMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
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
         hWndSysPage = 0;
         break;
      case WM_CLOSE:
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
     }                          /* end switch message */
  return rc;
}
