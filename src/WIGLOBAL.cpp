	
#include "stddef.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "user.h"
#include "dpmi.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

// #define memcpy _hmemcpy

BOOL WINAPI LocalDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ViewDlg    (HWND, UINT, WPARAM, LPARAM);
BOOL WINAPI MemFilterDlg (HWND, UINT, WPARAM, LPARAM);

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT   hFontAlt;
extern PSTR    pszStatMem1;

int xGlobalpos   = 100;
int yGlobalpos   = 100;

HWND hWndGlobal = 0;

// verkuerzte globalinfo struktur

typedef struct tagKGMEM {
  DWORD   dwBase;
  DWORD   dwSize;
  HGLOBAL hOwner;
  HGLOBAL hBlock;
  WORD    wType;
  WORD    wData;
  WORD    wcLock;
  WORD    wcPageLock;
  WORD    wFlags;
  WORD    wHeapPresent;
  } KGMEM;

static GMEMFILTER gmf = {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,
                         FALSE,TRUE,FALSE,FALSE,FALSE,
                         0x00000000,0xFFFFFFFF,GLOBAL_ALL,0,"\0",0
                        };

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ GlobalDlg - Global Heap Dialog                       บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK GlobalDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int  BtnTab[] = {ID_SUBDLG1,ID_SUBDLG2,ID_LOCKMEM,ID_UNLKMEM,
                         ID_DISCMEM,ID_PLCKMEM,ID_PULKMEM,ID_FIXMEM,ID_UNFIXMEM,
                         ID_SETRESETDISCARDABLEMEM,ID_FILTMEM,
                         0};
 GLOBALINFO  globalinfo;
 GLOBALENTRY globalentry;
 HEXDUMPPARM hdp;
 GMEMFILTER  *pgmf;
 SELECTOR    tsel;
 DESCRIPTOR  desc;
 KGMEM _huge * hpkgmem;
 KGMEM _huge * hpkgmem2;
 char   str[100];
 char   str1[20];
 char   str2[40];
 char   szStr3[20];
 char   szStr4[8];
 char * pStr;
 char * pszStr2;
 char   c;
 int    y,x;
 HWND   hWnd;
 WORD   wSortPos;
 WORD   wLocks;
 DWORD  dw[2];
 WORD   hflags;
 DWORD  maxmem,usedmem,discmem,discablemem;
 HGLOBAL hHeapw;
// HFILE	hFile;
 WORD   entries;
 LPVOID lpvoid;
 WORD   memflags;
 HGLOBAL hGlobal,hGlobalx;
// POINT  point;
 BOOL   rc = FALSE;

 pgmf = (GMEMFILTER *)GetWindowWord(hDlg,DLGWINDOWEXTRA);

 switch (message)
   {
    case WM_INITDIALOG:
      if (lParam) {
          pgmf = (GMEMFILTER *)lParam;
          if (!pgmf->fStatic)
              if (pgmf = (GMEMFILTER *)LocalAlloc(LMEM_FIXED,sizeof(GMEMFILTER)))
                  memcpy(pgmf,(GMEMFILTER *)lParam,sizeof(GMEMFILTER));
              else
                  pgmf = &gmf;
      } else
          pgmf = &gmf;

      SetWindowWord(hDlg,DLGWINDOWEXTRA,(WORD)pgmf);

      LoadTabs(IDUS_0,str);                /* status all */
      SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));

      LoadTabs(IDUS_1,str);                /* status Handle */
      SendDlgItemMessage(hDlg,ID_STATUS2,ST_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));

      LoadTabs(IDUS_2,str);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,(LPARAM)(LPVOID)hFontAlt);

      if (pgmf != &gmf)
         {
          if (pgmf->hOwner)
              if (IsTask((HTASK)pgmf->hOwner)) {
                  GetModuleName(GetModuleFromTask(pgmf->hOwner),
                                pgmf->szOwner,sizeof(pgmf->szOwner));
                  wsprintf(str,"Global Heap Task %s",(LPSTR)pgmf->szOwner);
              } else {
                  if (pgmf->hOwner = GetExePtr(pgmf->hOwner)) {
                      GetModuleName((HMODULE)pgmf->hOwner,pgmf->szOwner,sizeof(pgmf->szOwner));
                      wsprintf(str,"Global Heap Modul %s",(LPSTR)pgmf->szOwner);
                  }    
              }

          if (!pgmf->hOwner)
              wsprintf(str,"Global Heap %s",(LPSTR)pgmf->szOwner);

          SetWindowText(hDlg,str);
         }
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      SetWindowPos(hDlg,0,xGlobalpos,yGlobalpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_SHOWNORMAL);
      rc = TRUE;
      break;
    case WM_CLOSE:
      if (!pgmf->fStatic)
          LocalFree((HLOCAL)pgmf);
      if (pgmf == &gmf)
          hWnd = hWndMenu;
      else
          hWnd = GetParent(hDlg);
//      PostMessage(hWnd,WM_COMMAND,ID_DESTROY,(LPARAM)hDlg);
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_DESTROY:
		if (hDlg == hWndGlobal)
			hWndGlobal = 0;
		break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xGlobalpos,&yGlobalpos);
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_SNAPSHOT:
              WriteLBSnapShot(0,
                              hDlg,
                              ID_LISTBOX1,
                              "GLH",
                              WLBSS_CLOSE | WLBSS_CLIPBOARD | WLBSS_MESSAGE);
              break;
         case ID_POPUPMENU:
              TrackPopup(hDlg,BtnTab);
              break;
         case ID_LOCKMEM:
         case ID_UNLKMEM:
         case ID_PLCKMEM:
         case ID_PULKMEM:
         case ID_FIXMEM:
         case ID_UNFIXMEM:
         case ID_DISCMEM:
         case ID_WARNING1:
         case ID_FREEMEM:
         case ID_SETRESETDISCARDABLEMEM:
         case ID_DISCARDABLEWARNING:
         case ID_WARNING2:
			  hWnd = GetDlgItem(hDlg,ID_LISTBOX1);	
              x = (WORD)SendMessage(hWnd, LB_GETCURSEL,0,0);
              hGlobal = (HGLOBAL)SendMessage(hWnd, LB_GETITEMDATA,x,0);
              switch (wParam)
                {
                 case ID_LOCKMEM:
                    GlobalLock(hGlobal);
                    break;
                 case ID_UNLKMEM:
                    GlobalUnlock(hGlobal);
                    break;
                 case ID_PLCKMEM:
                    GlobalPageLock(hGlobal);
                    break;
                 case ID_PULKMEM:
                    GlobalPageUnlock(hGlobal);
                    break;
                 case ID_FIXMEM:
                    GlobalFix(hGlobal);
                    break;
                 case ID_UNFIXMEM:
                    GlobalUnfix(hGlobal);
                    break;
                 case ID_SETRESETDISCARDABLEMEM:
                    globalentry.dwSize = sizeof(GLOBALENTRY);
                    x = SelectorAccessRights(hGlobal,0,0);
                    if (GlobalEntryHandle(&globalentry,hGlobal))
                       {
                        switch (globalentry.wType) {
                          case GT_UNKNOWN:
                          case GT_DGROUP:
                          case GT_DATA:
                          case GT_TASK:
                          case GT_MODULE:
                             if (!(x & 0x1000))
                                 CreateMessageParam(hDlg,
                                     MAKEINTRESOURCE(IDS_ERRMEM7),
                                     MAKEINTRESOURCE(IDS_WARNING),
                                     MB_OKCANCEL,
                                     ID_DISCARDABLEWARNING);
                             else
                                 SelectorAccessRights(hGlobal,1,x ^ 0x1000);
                             break;
                          default:
                             SelectorAccessRights(hGlobal,1,x ^ 0x1000);
                             break;
                          }
                       }
                    break;
                 case ID_DISCARDABLEWARNING:
                    if (HIWORD(lParam) == IDOK)
                       {
                        x = SelectorAccessRights(hGlobal,0,0);
                        SelectorAccessRights(hGlobal,1,x ^ 0x1000);
                       }
                    break;
                 case ID_DISCMEM:
                    GlobalDiscard(hGlobal);
                    break;
                 case ID_FREEMEM:
                    CreateMessageParam(hDlg,
                                     MAKEINTRESOURCE(IDS_ERRMEM6),
                                     MAKEINTRESOURCE(IDS_WARNING),
                                     MB_OKCANCEL,
                                     ID_WARNING2);
                    break;
                 case ID_WARNING2:
                    if (HIWORD(lParam) == IDOK)
                       GlobalFree(hGlobal);
                    break;
                } // end switch (wParam)
              PostMessage(hDlg,WM_COMMAND,ID_LISTBOX1,MAKELONG(hWnd,LBN_SELCHANGE));
              break;
         case ID_COMPMEM:
              SetCursor(hCursorWait);
              dw[1] = GlobalCompact(0xFFFFFFFF);
              if (hGlobal  = GlobalAlloc(GMEM_FIXED,dw[1]))
                 {
                  GlobalLock(hGlobal);
                  GlobalUnlock(hGlobal);
                  GlobalFree(hGlobal);
                 }
              SetCursor(hCursor);
              break;
         case ID_FILTMEM:
              hWnd = (HWND)GetWindowWord(hDlg,DLGWINDOWEXTRA+2);
              if (hWnd)
                  ShowWindow(hWnd,SW_RESTORE);
              else
                 {
                  hWnd = CreateDialogParam(hInst,
                                           MAKEINTRESOURCE(IDD_MEMOPT),
                                           hDlg,
                                           MemFilterDlg,
                                           (LPARAM)(LPVOID)pgmf
                                          );
                  SetWindowWord(hDlg,DLGWINDOWEXTRA+2,(WORD)hWnd);
                 }
              break;
         case ID_DESTROY:
              hWnd = (HWND)GetWindowWord(hDlg,DLGWINDOWEXTRA+2);
              if (hWnd == (HWND)lParam)
                  SetWindowWord(hDlg,DLGWINDOWEXTRA+2,0);
              break;
         case ID_SUBDLG2:     /* Local Heap */
         case ID_SUBDLG1:     /* view (hexdump) */
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              x = (WORD)ListBox_GetCurSel(hWnd);
              if (x == LB_ERR)
                  break;
              dw[0] = ListBox_GetItemData(hWnd,x);
              hGlobal = (HGLOBAL)LOWORD(dw[0]);
              hflags = HIWORD(dw[0]); /* die flags von GlobalFind holen */
              if (hGlobal)
                  memflags = GlobalFlags(hGlobal);
              else
                  memflags = 0;
              hdp.dwOffset = 0;
              hdp.hGlobal = hGlobal;
              hdp.dwLength = 0;
              hdp.wType   = HDP_SETWINDOWTEXT;

              if ((!hGlobal) || (memflags & GMEM_DISCARDED))
                  if (hflags == GT_INTERNAL || hflags == GT_SENTINEL)
                     {
                      MessageBeep(0);
                      break;
                     }
                  else
                     {
                      ListBox_GetText(hWnd,x,str);
                      if (sscanf(str,"%*X\t%lX\t%lX",&hdp.dwOffset,&hdp.dwLength) < 2)
                          break;
                      hdp.wType   = hdp.wType | HDP_LINEAR;
                     }

              if ((wParam == ID_SUBDLG2) && (IsValidLocalHeap(hGlobal)))
                 {
                  hWnd = CreateDialogParam(hInst,
                                           MAKEINTRESOURCE(IDD_LOCAL),
                                           hDlg,
                                           LocalDlg,
                                           MAKELONG(GlobalHandleToSel(hGlobal),0)
                                          );
                  strcpy(str1,"Local Heap");
                  wsprintf(str,"%s: %X",(LPSTR)str1,hGlobal);
                  SetWindowText(hWnd,str);
                 }
              else
                 {
                  hWnd = CreateDialogParam(hInst,
                                           MAKEINTRESOURCE(IDD_VIEW),
                                           hDlg,
                                           ViewDlg,
                                           (DWORD)(LPVOID)&hdp
                                          );
                 }
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam)) {
                  int dflag;
                  case LBN_SELCHANGE: 
                     hWnd = (HWND)LOWORD(lParam);
                     x = (WORD)ListBox_GetCurSel(hWnd);
                     if (x == LB_ERR) {
                         hGlobal = 0;
                         hflags = -1;
                     } else {
                         dw[0] = ListBox_GetItemData(hWnd,x);
                         hGlobal = (HGLOBAL)LOWORD(dw[0]);
                         hflags = HIWORD(dw[0]);
                     }
                     if (hGlobal) {
                         memflags = GlobalFlags(hGlobal);
                         tsel = GlobalHandleToSel(hGlobal);
                         DPMIGetDescriptor(tsel,&desc);
                     } else {
                         memflags  = 0;
                         desc.attr = 0;
                         tsel      = 0;
                     }

                     globalentry.wcLock = 0;
                     globalentry.wcPageLock = 0;

                     if (!(desc.attr & 0x0010)) { // kein memory segment (gate bzw. nicht da)
                         hGlobalx = 0;
                         strcpy(str2,"???");
                     } else {
                         globalentry.dwSize = sizeof(GLOBALENTRY);
                         if (GlobalEntryHandle(&globalentry,hGlobal)) {
                             hGlobalx = hGlobal;
                             switch (globalentry.wType) {
                                 case GT_UNKNOWN:
                                   pStr = "PRIV";
                                   break;
                                 case GT_DGROUP:
                                   pStr = "DGRP";
                                   break;
                                 case GT_DATA:
                                   pStr = "DATA";
                                   break;
                                 case GT_CODE:
                                   pStr = "CODE";
                                   break;
                                 case GT_TASK:
                                   pStr = "TD";
                                   break;
                                 case GT_RESOURCE:
                                   pStr = "RES";
                                   break;
                                 case GT_MODULE:
                                   pStr = "MD";
                                   break;
                                 case GT_FREE:
                                   pStr = "FREE";
                                   break;
                                 case GT_INTERNAL:
                                   pStr = "ITRN";
                                   break;
                                 case GT_SENTINEL:
                                   pStr = "SEN";
                                   break;
                                 case GT_BURGERMASTER:
                                   pStr = "BM";
                                   break;
                                 default:
                                   pStr = "?";
                                   break;
                             }
                         } else {
                             pStr = "???";
                             hGlobalx = 0;
                         }
                         strcpy(str2,pStr);

                         if (memflags & GMEM_DISCARDED)
                             strcat(str2,",discarded");
                         else
                         	if (memflags & GMEM_DISCARDABLE) {
                            	strcat(str2,",discardable");
                         	} else
                         		if (globalentry.wcLock) {
	                            	strcat(str2,",fixed");
    	                        	if (globalentry.wcLock != (memflags & 0xFF))
        	                    		strcat(str2,",DOS");
            	             	} else
                	            	strcat(str2,",moveable");

                         if (memflags & GMEM_DISCARDABLE)
                             SetWindowText(GetDlgItem(hDlg,ID_SETRESETDISCARDABLEMEM),
                                           "!Dis&cardable");
                         else
                             SetWindowText(GetDlgItem(hDlg,ID_SETRESETDISCARDABLEMEM),
                                           "Dis&cardable");

                         if (desc.attr & 0x0008)
                             pszStr2 = "CODE";
                         else
                             pszStr2 = "DATA";
                         wsprintf(str1,
                                  ",(%s,%lX,%04X)",
                                  (LPSTR)pszStr2,
                                  GetSelectorLimit(tsel),
                                  desc.attr
                                 );
                         strcat(str2,str1);
                     }

                     dflag = 0;
                     if (memflags & GMEM_DISCARDABLE)
                         if (!(memflags & GMEM_DISCARDED) && !(memflags & 0x00FF))
                             dflag = 1;
                     wLocks = memflags & 0xFF;
                     EnableWindow(GetDlgItem(hDlg,ID_DISCMEM),dflag);
                     EnableWindow(GetDlgItem(hDlg,ID_LOCKMEM),(WORD)hGlobalx && (memflags & 0x0100));
                     EnableWindow(GetDlgItem(hDlg,ID_UNLKMEM),(WORD)hGlobalx && (memflags & 0x0100) && wLocks);
                     EnableWindow(GetDlgItem(hDlg,ID_PLCKMEM),(WORD)hGlobalx);
                     EnableWindow(GetDlgItem(hDlg,ID_PULKMEM),(WORD)hGlobalx && globalentry.wcPageLock);
                     EnableWindow(GetDlgItem(hDlg,ID_FIXMEM),(WORD)hGlobalx);
                     EnableWindow(GetDlgItem(hDlg,ID_UNFIXMEM),(WORD)hGlobalx && wLocks);
                     EnableWindow(GetDlgItem(hDlg,ID_SETRESETDISCARDABLEMEM),(WORD)hGlobalx);
                     if (hGlobal && !hGlobalx)	// zwischenzeitlich ungültig?
	                     EnableWindow(GetDlgItem(hDlg,ID_SUBDLG1),0);
					 else
     	                EnableWindow(GetDlgItem(hDlg,ID_SUBDLG1),		// Anzeigen
                                  (WORD)hGlobalx || (hflags != GT_INTERNAL && hflags != GT_SENTINEL)
                                 );
                     EnableWindow(GetDlgItem(hDlg,ID_SUBDLG2),hGlobalx && IsValidLocalHeap((HGLOBAL)tsel));

                     wsprintf(str,
                              "Handle:\t %X\tTyp:\t %s\tLocks:\t L%d,P%d",
                              hGlobal,
                              (LPSTR)str2,
                              wLocks,
                              globalentry.wcPageLock
                             );
                     SetDlgItemText(hDlg,ID_STATUS2,str);
                     break;
                  case LBN_DBLCLK:
                     PostMessage(hDlg,WM_COMMAND,ID_SUBDLG2,0);
                     break;
                  case XLBN_RBUTTONDOWN:
                     TrackPopup(hDlg,BtnTab);
                     break;
                 }
              break;
         case ID_REFRESH:
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
//              SendMessage(hWnd,WM_SETREDRAW,0,0);
              ListBox_ResetContent(hWnd);

              wSortPos = 0xFFFF;       /* keine Sortierung */

              if (pgmf->wGmemtype == GLOBAL_LRU)
                 ;
              else
              if (pgmf->fSort == 4)             /* sortierung nach Name */
                 wSortPos = 30;
              else
              if (pgmf->fSort == 3)             /* sortierung nach Groesse */
                 wSortPos = 14;
              else
              if (pgmf->fSort == 2)             /* Sortierung nach Addresse */
                 wSortPos = 5;
              else
              if (pgmf->fSort == 1)
                 wSortPos = 0;            /* Sortierung nach Handle */

              SetCursor(hCursorWait);

              SendMessage(hWnd,XLB_SETSORTSTART,wSortPos,0);

              SendMessage(hWnd,XLB_SETSORTDIR,pgmf->fSortdir,0);

              globalinfo.dwSize = sizeof(globalinfo);
              globalinfo.wcItems = 0;

              GlobalInfo(&globalinfo);

              if (!(hHeapw = GlobalAlloc(GMEM_FIXED,((DWORD)globalinfo.wcItems+10) * sizeof(KGMEM))))
                  {
                   CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRMEM5),(LPCSTR)0,MB_OK);
                   SetCursor(hCursor);
                   break;
                  }

              lpvoid = GlobalLock(hHeapw);
              hpkgmem = (KGMEM _huge *)lpvoid;

              globalentry.dwSize = sizeof(globalentry);
              y = GlobalFirst(&globalentry,pgmf->wGmemtype);
              entries = 0;
              while (y)
                 {
                  if (globalentry.hBlock != hHeapw)
                     {
                      int select = 1;
                      if (!globalentry.hBlock)           /* discarded filter */
                          if (pgmf->discardfilter)
                             select = 2;
                          else
                             select = 0;
                      else
                      if (GlobalFlags(globalentry.hBlock) & GMEM_DISCARDABLE)  /* discardable */
                          if (pgmf->discardablefilter)
                             ;
                          else
                             select = 0;
                      else
                          if (pgmf->nondiscardablefilter)    /* nondiscardable */
                             ;
                          else
                             select = 0;

                      if (select == 1)
                          if (globalentry.wcPageLock
                          &&  pgmf->pagelockedfilter)
                                 ;
                          else
                          if ((!globalentry.wcPageLock)
                          &&  pgmf->notpagelockedfilter)
                                 ;
                          else
                              select = 0;

                      if (select == 1)
                          if (globalentry.wType == GT_CODE)
                              if (pgmf->codefilter)              /* codefilter */
                                 ;
                              else
                                 select = 0;
                          else
                          if (globalentry.wType == GT_RESOURCE)
                              if (pgmf->resourcefilter)          /* resourcefilter */
                                 ;
                              else
                                 select = 0;
                          else
                          if (globalentry.wType == GT_UNKNOWN
                          ||  globalentry.wType == GT_DGROUP
                          ||  globalentry.wType == GT_DATA)
                              if (pgmf->datafilter)              /* datafilter */
                                 ;
                              else
                                 select = 0;
                          else
                              if (pgmf->internfilter)              /* internfilter */
                                 ;
                              else
                                 select = 0;

                      if (select)
                         {
                          if (globalentry.dwAddress < pgmf->memvonfilter)
                              select = 0;
                          else
                          if (globalentry.dwAddress > pgmf->membisfilter)
                              select = 0;
                         }

                      if (select)
                         {
                          hpkgmem->dwBase = globalentry.dwAddress;
                          hpkgmem->dwSize = globalentry.dwBlockSize;
                          hpkgmem->hBlock = globalentry.hBlock;
                          hpkgmem->hOwner = globalentry.hOwner;
                          hpkgmem->wType  = globalentry.wType;
                          hpkgmem->wData  = globalentry.wData;

                          hpkgmem->wcLock       = globalentry.wcLock;
                          hpkgmem->wcPageLock   = globalentry.wcPageLock;
                          hpkgmem->wFlags       = globalentry.wFlags;
                          hpkgmem->wHeapPresent = globalentry.wHeapPresent;

                          hpkgmem++;
                          entries++;
                         }
                     }
                  y = GlobalNext(&globalentry,pgmf->wGmemtype);
                 }

              hpkgmem2 = hpkgmem;
              maxmem      = 0;
              usedmem     = 0;
              discablemem = 0;
              discmem     = 0;
              hpkgmem = (KGMEM _huge *)lpvoid;
              while (hpkgmem < hpkgmem2)
                 {                           /* owner -> str1 */
                  strcpy(szStr4,"  ");
                  str1[0] = 0;
                  switch ((int)hpkgmem->hOwner)
                    {
                     case GO_FREE:
                        pStr = "<frei>";
                        break;
                     case GO_SENTINEL:
                        pStr = "<sentinel>";
                        break;
                     case GO_BURGERMASTER:
                        pStr = "<burgermaster>";
                        break;
                     case GO_NOT_THERE:
                        pStr = "<not_there>";
                        break;
                     case GO_PHANTOM:
                        pStr = "<phantom>";
                        break;
                     case GO_WRAITH:
                        pStr = "<wraith>";
                        break;
                     case GO_BOGUS:
                        pStr = "<bogus>";
                        break;
                     default:
                        pStr = "<?>";
                        if (GlobalEntryHandle(&globalentry,hpkgmem->hOwner))
                           {
                            if (globalentry.wType == GT_MODULE)
                               {
                                GetModuleName((HMODULE)hpkgmem->hOwner,str1,sizeof(str1));
                                strcpy(szStr4,",M");
                               }
                            else
                            if (globalentry.wType == GT_TASK)
                               {
                                GetModuleName((HMODULE)TaskGetModule(hpkgmem->hOwner),str1,sizeof(str1));
                                strcpy(szStr4,",T");
                               }
                           }
                        break;
                    }
                  if (!str1[0])
                      strcpy(str1,pStr);
                                             /* flags -> memflags */
                  if (hpkgmem->hBlock)
                      memflags = GlobalFlags(hpkgmem->hBlock);
                  else
                      memflags = 0;
                                             /* sonstiges -> str2 */
                  wsprintf(szStr3,"%X",hpkgmem->wData);
                  pszStr2 = szStr3;

                  switch (hpkgmem->wType)
                     {
                      case GT_UNKNOWN:
                        pStr = "priv";
                        break;
                      case GT_DGROUP:
                        pStr = "dgrp";
                        break;
                      case GT_DATA:
                        pStr = "data";
                        break;
                      case GT_CODE:
                        pStr = "code";
                        break;
                      case GT_TASK:
                        pStr = "td";
                        break;
                      case GT_RESOURCE:
                        pStr = "res";
                        switch (hpkgmem->wData)
                          {
                           case GD_USERDEFINED:
                             pszStr2 = "user";
                             break;
                           case GD_CURSORCOMPONENT:
                             pszStr2 = "cursorc";
                             break;
                           case GD_BITMAP:
                             pszStr2 = "bitmap";
                             break;
                           case GD_ICONCOMPONENT:
                             pszStr2 = "iconc";
                             break;
                           case GD_MENU:
                             pszStr2 = "menu";
                             break;
                           case GD_DIALOG:
                             pszStr2 = "dialog";
                             break;
                           case GD_STRING:
                             pszStr2 = "string";
                             break;
                           case GD_FONTDIR:
                             pszStr2 = "fontdir";
                             break;
                           case GD_FONT:
                             pszStr2 = "font";
                             break;
                           case GD_ACCELERATORS:
                             pszStr2 = "acc";
                             break;
                           case GD_RCDATA:
                             pszStr2 = "rcdata";
                             break;
                           case GD_ERRTABLE:
                             pszStr2 = "errtab";
                             break;
                           case GD_CURSOR:
                             pszStr2 = "cursor";
                             break;
                           case GD_ICON:
                             pszStr2 = "icon";
                             break;
                           case GD_NAMETABLE:
                             pszStr2 = "namtab";
                             break;
                           default:
                             break;
                          }
                        break;
                      case GT_MODULE:
                        pStr = "md";
                        break;
                      case GT_FREE:
                        pStr = "free";
                        break;
                      case GT_INTERNAL:
                        pStr = "int";
                        break;
                      case GT_SENTINEL:
                        pStr = "sen";
                        break;
                      case GT_BURGERMASTER:
                        pStr = "bm";
                        break;
                      default:
                        pStr = "?";
                        break;
                     }
                  if (IsValidLocalHeap(hpkgmem->hBlock))
                      pszStr2 = "Heap";

                  c = ' ';
                  if (hpkgmem->hBlock)
                     if (hpkgmem->wcLock != (memflags & 0xFF))
                         c = 'F';
                     else
                     if (memflags & 0x0100)
                         c = 'D';

                  wsprintf(str2,
                           "%s,%s",
                           (LPSTR)pStr,
                           (LPSTR)pszStr2
//                           hpkgmem->wcLock,
//                           hpkgmem->wcPageLock
//                           hpkgmem->wFlags,
//                           hpkgmem->wHeapPresent
                          );
                                             /* selektion */
                  if (pgmf->hOwner && pgmf->hOwner != hpkgmem->hOwner)
                     ;
                  else
                  if (pgmf->szOwner[0] && strcmp(str1,pgmf->szOwner))
                      ;
                  else
                     {
                                             /* summenfelder update */
                      maxmem = maxmem + hpkgmem->dwSize;
                      if (!hpkgmem->hBlock)
                          discmem = discmem + hpkgmem->dwSize;
                      else
                         {
                          usedmem = usedmem + hpkgmem->dwSize;
                          if (memflags & GMEM_DISCARDED)
                              discmem = discmem + hpkgmem->dwSize;
                          else
                          if (memflags & GMEM_DISCARDABLE)
                              discablemem = discablemem + hpkgmem->dwSize;
                         }
                                             /* owner evtl. kuerzen (nicht bei internals) */
                      if ((str1[0] != '<') && strlen(str1) > 8)
                          strcpy(&str1[8],"...");
                      pStr = szStr3;
                      *pStr = 0;
                      if (hpkgmem->hBlock)
                          if (memflags & 0xFF)
                             pStr = pStr + wsprintf(pStr,"L%d ",memflags & 0xFF);
                          if (hpkgmem->wcPageLock)
                             pStr = pStr + wsprintf(pStr,"P%d",hpkgmem->wcPageLock);

                                             /* jetzt zeile fuellen */
                      wsprintf(str,
                               "%04X\t%08lX\t%8.1lX\t\t%04X%s\t%s\t%s\t%c\t%s",
                               hpkgmem->hBlock,
                               hpkgmem->dwBase,
                               hpkgmem->dwSize,
                               hpkgmem->hOwner,
                               (LPSTR)szStr4,
                               (LPSTR)str1,
                               (LPSTR)szStr3,
                               c,
                               (LPSTR)str2
                              );
                      x = (int)SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                      SendMessage(hWnd,LB_SETITEMDATA,x,MAKELONG(hpkgmem->hBlock,hpkgmem->wType));
                     }
                  hpkgmem++;
                 }                /* end while */

              GlobalUnlock(hHeapw);
              GlobalFree(hHeapw);

              PostMessage(hWnd,LB_SETCURSEL, 0, 0);

              wsprintf(str,
                       pszStatMem1,
                       maxmem,
                       usedmem,
                       discablemem,
                       discmem
                      );
              SetDlgItemText(hDlg,ID_STATUS1,str);

              SetCursor(hCursor);

              break; // ende ID_REFRESH
         } // ende switch (wParam)
      rc = TRUE;
      break; // ende WM_COMMAND
    default:
      break;
    } // ende switch (message)
 return rc;
}
