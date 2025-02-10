
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

// #define sscanf lsscanf

extern HICON hIcon;

BOOL WINAPI CheckDlgBoxIcon(HWND);

int xFiltpos  = 100,yFiltpos  = 100;

static char szFStrDW[] = {" %lX %*c"};
static char szFStrW[]  = {" %X %*c"};

/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SetSortButtons                                                    บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
static void SetSortButtons(HWND hDlg,WORD wMode)
{
  BOOL fEna;
  if (wMode == GLOBAL_LRU)
      fEna = FALSE;
  else
      fEna = TRUE;
  EnableDlgItem(hDlg,ID_RBTN1,fEna);
  EnableDlgItem(hDlg,ID_RBTN2,fEna);
  EnableDlgItem(hDlg,ID_RBTN3,fEna);
  EnableDlgItem(hDlg,ID_RBTN4,fEna);
  EnableDlgItem(hDlg,ID_RBTN5,fEna);
}
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Dialogfunktion "Filtereingabe"                                    บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK MemFilterDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
 LPGMEMFILTER lpmf;
 char str[20];
 DWORD dw;
 BOOL rc = FALSE;
// BOOL fEna;

   lpmf = (LPGMEMFILTER)GetWindowLong(hDlg,DWL_USER);

   switch(message)
     {
      case WM_INITDIALOG:
         SetWindowLong(hDlg,DWL_USER,lParam);
         lpmf = (LPGMEMFILTER)lParam;
         SendDlgItemMessage(hDlg,ID_MEMFIL1,BM_SETCHECK,lpmf->discardfilter,0);
         SendDlgItemMessage(hDlg,ID_MEMFIL2,BM_SETCHECK,lpmf->discardablefilter,0);
         SendDlgItemMessage(hDlg,ID_MEMFIL3,BM_SETCHECK,lpmf->nondiscardablefilter,0);
         SendDlgItemMessage(hDlg,ID_MEMFIL9,BM_SETCHECK,lpmf->pagelockedfilter,0);
         SendDlgItemMessage(hDlg,ID_MEMFIL0,BM_SETCHECK,lpmf->notpagelockedfilter,0);
         SendDlgItemMessage(hDlg,ID_MEMFIL4,BM_SETCHECK,lpmf->codefilter,0);
         SendDlgItemMessage(hDlg,ID_MEMFIL5,BM_SETCHECK,lpmf->datafilter,0);
         SendDlgItemMessage(hDlg,ID_MEMFIL7,BM_SETCHECK,lpmf->resourcefilter,0);
         SendDlgItemMessage(hDlg,ID_MEMFIL8,BM_SETCHECK,lpmf->internfilter,0);
         SendDlgItemMessage(hDlg,ID_MEMFIL6,EM_LIMITTEXT,sizeof(lpmf->szOwner)-1,0);
         SetDlgItemText(hDlg,ID_MEMFIL6,lpmf->szOwner);
         EnableWindow(GetDlgItem(hDlg,ID_MEMFIL6),!(lpmf->fProtectOwner));
         SendDlgItemMessage(hDlg,ID_MEMVON,EM_LIMITTEXT,8,0);
         wsprintf(str,"%lX",lpmf->memvonfilter);
         SetDlgItemText(hDlg,ID_MEMVON,str);
         SendDlgItemMessage(hDlg,ID_MEMBIS,EM_LIMITTEXT,8,0);
         wsprintf(str,"%lX",lpmf->membisfilter);
         SetDlgItemText(hDlg,ID_MEMBIS,str);
         SendDlgItemMessage(hDlg,ID_RBTN1 + lpmf->fSort,BM_SETCHECK,1,0);
         SendDlgItemMessage(hDlg,ID_SORTDIR,BM_SETCHECK,lpmf->fSortdir,0);
         SendDlgItemMessage(hDlg,ID_RBTNA + (lpmf->wGmemtype - GLOBAL_ALL),BM_SETCHECK,1,0);
         SetSortButtons(hDlg,lpmf->wGmemtype);
         EnableDlgItem(hDlg,ID_SORTDIR,lpmf->fSort && !lpmf->wGmemtype);

         if (xFiltpos || yFiltpos)
            SetWindowPos(hDlg,0,xFiltpos,yFiltpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
         ShowWindow(hDlg,SW_NORMAL);
         rc = TRUE;
         break;
      case WM_CLOSE:
         PostMessage(GetParent(hDlg),WM_COMMAND,ID_DESTROY,(LPARAM)(LPVOID)hDlg);
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
      case WM_MOVE:
         SaveWindowPos(hDlg,&xFiltpos,&yFiltpos);
         break;
      case WM_PAINT:
         rc = CheckDlgBoxIcon(hDlg);
         break;
      case WM_ERASEBKGND:
         if (IsIconic(hDlg))
             rc = (BOOL)DefWindowProc(hDlg,WM_ICONERASEBKGND, wParam, lParam);
         break;
      case WM_QUERYDRAGICON:
         rc = (BOOL)hIcon;
         break;
      case WM_COMMAND:
         switch(wParam)
           {
            case ID_SNAPSHOT:
               PostMessage(GetParent(hDlg),WM_COMMAND,ID_SNAPSHOT,0);
               break;
            case ID_SUBDLG1:
               PostMessage(GetParent(hDlg),WM_COMMAND,ID_COMPMEM,0);
               break;
            case IDOK: // refresh
            case IDCANCEL: // schliessen
               if (lpmf->fErrMVF)
                  {
                   MessageBeep(0);
                   SetFocus(GetDlgItem(hDlg,ID_MEMVON));
                  }
               else
               if (lpmf->fErrMBF)
                  {
                   MessageBeep(0);
                   SetFocus(GetDlgItem(hDlg,ID_MEMBIS));
                  }
               else
               if (wParam == IDCANCEL)
                   PostMessage(hDlg,WM_CLOSE,0,0);
               else
                   PostMessage(GetParent(hDlg),WM_COMMAND,ID_REFRESH,0);
               break;
           case ID_RBTNA:            // normal
           case ID_RBTNB:            // lru
           case ID_RBTNC:
               switch (HIWORD(lParam))
                 {
                  case BN_CLICKED:
                    lpmf->wGmemtype = wParam - ID_RBTNA;
                    CheckRadioButton(hDlg,ID_RBTNA,ID_RBTNC,wParam);
                    SetSortButtons(hDlg,lpmf->wGmemtype);
                    EnableDlgItem(hDlg,ID_SORTDIR,lpmf->fSort && !lpmf->wGmemtype);
                    break;
                 }
               break;
           case ID_RBTN1:            // sortierung
           case ID_RBTN2:
           case ID_RBTN3:
           case ID_RBTN4:
           case ID_RBTN5:
               switch (HIWORD(lParam))
                 {
                  case BN_CLICKED:
                    lpmf->fSort = wParam - ID_RBTN1;
                    CheckRadioButton(hDlg,ID_RBTN1,ID_RBTN5,wParam);
                    EnableDlgItem(hDlg,ID_SORTDIR,lpmf->fSort && !lpmf->wGmemtype);
                    break;
                 }
               break;
           case ID_SORTDIR:
               lpmf->fSortdir = 1 - lpmf->fSortdir;
               break;
           case ID_MEMFIL0:
           case ID_MEMFIL1:
           case ID_MEMFIL2:
           case ID_MEMFIL3:
           case ID_MEMFIL4:
           case ID_MEMFIL5:
           case ID_MEMFIL7:
           case ID_MEMFIL8:
           case ID_MEMFIL9:
               if (HIWORD(lParam) == BN_CLICKED)
                   switch (wParam)
                     {
                      case ID_MEMFIL0:
                        lpmf->notpagelockedfilter  = 1 - lpmf->notpagelockedfilter;
                        break;
                      case ID_MEMFIL1:
                        lpmf->discardfilter  = 1 - lpmf->discardfilter;
                        break;
                      case ID_MEMFIL2:
                        lpmf->discardablefilter = 1 - lpmf->discardablefilter;
                        break;
                      case ID_MEMFIL3:
                        lpmf->nondiscardablefilter = 1 - lpmf->nondiscardablefilter;
                        break;
                      case ID_MEMFIL4:
                        lpmf->codefilter = 1 - lpmf->codefilter;
                        break;
                      case ID_MEMFIL5:
                        lpmf->datafilter = 1 - lpmf->datafilter;
                        break;
                      case ID_MEMFIL7:
                        lpmf->resourcefilter = 1 - lpmf->resourcefilter;
                        break;
                      case ID_MEMFIL8:
                        lpmf->internfilter = 1 - lpmf->internfilter;
                        break;
                      case ID_MEMFIL9:
                        lpmf->pagelockedfilter = 1 - lpmf->pagelockedfilter;
                        break;
                     }
               break;
           case ID_MEMFIL6:
               GetDlgItemText(hDlg,ID_MEMFIL6,lpmf->szOwner,sizeof(lpmf->szOwner));
               AnsiUpper(lpmf->szOwner);
               break;
           case ID_MEMVON:
               if (HIWORD(lParam) == EN_UPDATE)
                  {
                   GetDlgItemText(hDlg,ID_MEMVON,str,sizeof(str));
                   if (sscanf(str,szFStrDW,&dw) == 1)
                      {
                       lpmf->memvonfilter = dw;
                       lpmf->fErrMVF = FALSE;
                      }
                   else
                      {
                       MessageBeep(0);
                       lpmf->fErrMVF = TRUE;
                      }
                  }
               break;
           case ID_MEMBIS:
               if (HIWORD(lParam) == EN_UPDATE)
                  {
                   GetDlgItemText(hDlg,ID_MEMBIS,str,sizeof(str));
                   if (sscanf(str,szFStrDW,&dw) == 1)
                      {
                       lpmf->membisfilter = dw;
                       lpmf->fErrMBF = FALSE;
                      }
                   else
                      {
                       MessageBeep(0);
                       lpmf->fErrMBF = TRUE;
                      }
                  }
               break;
           }
         rc = TRUE;     /* WM_COMMAND immer RC TRUE */
         break;
      default:
         break;
     }
 return rc;
}
