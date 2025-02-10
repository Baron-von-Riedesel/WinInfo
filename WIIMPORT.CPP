
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT   hFontAlt;
extern int fNativeimport;
extern BYTE fShow;

BOOL CALLBACK UnassembleDlg  (HWND, UINT, WPARAM, LPARAM);

int xImppos   = 100;
int yImppos   = 100;

static fSort = 0;
static char * pszImport = {"Importe"};
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ CheckModul - pruefen ob Modul noch da                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
static BOOL CheckModul(HWND hDlg,HMODULE * phandle,MODULEENTRY * pmoduleentry,WORD wMode)
{
  *phandle = (HMODULE)GetWindowWord(hDlg,DLGWINDOWEXTRA);
  (*pmoduleentry).dwSize = sizeof(MODULEENTRY);
  if (ModuleFindHandle(pmoduleentry,*phandle))
     return TRUE;

  EnableDlgItem(hDlg,ID_SUBDLG2,0);
  EnableDlgItem(hDlg,ID_SUBDLG1,0);
  EnableDlgItem(hDlg,ID_REFRESH,0);

  if (wMode)
      CreateMessage(hDlg,errorstr1,0,MB_OK);
  return FALSE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SetSortParms                                         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
static void SetSortParms(HWND hDlg)
{
 WORD wSortPos;
 DWORD dw;
 HWND hWnd;
 char szStr[80];
 char str1[20];

  SetCursor(hCursorWait);
  if (fSort)
     wSortPos = 16;
  else
     wSortPos = 0;
  hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
  SendMessage(hWnd,XLB_SETSORTSTART,wSortPos,0);
  SendMessage(hWnd,XLB_SORT,0,0);
  InvalidateRect(hWnd,0,1);
  dw = SendMessage(hWnd,XLB_GETWINDOWDATA,0,0);

  GetWindowText(hWnd,szStr,sizeof(szStr));
  GetTextField(szStr,fSort,str1,sizeof(str1));
  wsprintf(szStr,
           "%s\t%u\tsortiert nach\t%s\tundefiniert\t%d\t\"by Name\"\t%d\t",
           (LPSTR)pszImport,
           (WORD)SendMessage(hWnd,LB_GETCOUNT,0,0),
           (LPSTR)str1,
           LOWORD(dw),
           HIWORD(dw)
          );
  SetDlgItemText(hDlg,ID_STATUS1,szStr);
  SetCursor(hCursor);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ImportDlg - Imports auflisten                        บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK ImportDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_SUBDLG1,0};
 WORD wZeilen;
 WORD wUndefs;
 WORD wByName;
 WORD y,i,wMode;
 char szStr[128];
 char str1[80];
 char str2[40];
 BOOL fUndef;
 FARPROC paddr;
 HWND hWnd;
 MODULEENTRY moduleentry;
 IMPORTENTRY importentry;
 HMODULE hModule;
 int hFile;
 PROCSTRUC procstruc;
 HMODULE lmod;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      SetWindowLong(hDlg,DLGWINDOWEXTRA,lParam);

      LoadTabs(IDUS_10,szStr);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      LoadTabs(IDUS_11,szStr);
      SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));

      SetWindowPos(hDlg,0,xImppos,yImppos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      if (fShow)
          ShowWindow(hDlg,SW_SHOWNORMAL);
      else
          ShowWindow(hDlg,SW_SHOWNOACTIVATE);

      SetCursor(hCursor);
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xImppos,&yImppos);
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_SNAPSHOT:
              GetTempFileName(0,"IMP",0,szStr);
              hFile = _lcreat(szStr,0);
              GetWindowText(hDlg,str1,sizeof(str1));
              _lwriteline(hFile,str1);
              WriteLBSnapShot(hFile,hDlg,ID_LISTBOX1,(LPSTR)0,0);
              GetDlgItemText(hDlg,ID_STATUS1,str1,sizeof(str1));
              _lwriteline(hFile,str1);
              _lclose(hFile);
              PutFiletoClipboard(szStr,hDlg);
              break;
         case ID_SUBDLG2:           /* sortieren */
              fSort = 3 - fSort;
              SetSortParms(hDlg);
              break;
         case ID_SUBDLG1:           /* disassembler */
              if (!CheckModul(hDlg,&hModule,&moduleentry,1))
                  break;
              y =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
              if (y == LB_ERR)
                  paddr = 0;
              else
                 {
                  paddr = (FARPROC)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,y,0);
                  paddr = (FARPROC)MAKELONG(LOWORD(paddr),GetSegmentHandle(hModule,HIWORD(paddr)));
                 }
              if (paddr)
                  if (!IsReadableSelector(HIWORD(paddr)))
                     MessageBeep(0);
                  else
                    {
                     _asm {
                         les bx,paddr
                         cmp bx,1
                         jb  lm1
                         cmp byte ptr es:[bx-1],66h
                         jnz lm1
                         dec bx
                         mov word ptr paddr,bx
                        lm1:
                         }
                     CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_UNASSEM), hDlg, UnassembleDlg,
                                              (DWORD)paddr
                                             );
                    }
              break;
         case ID_REFRESH:
              fShow = TRUE;
              if (!CheckModul(hDlg,&hModule,&moduleentry,1))
                 {
                  fShow = FALSE;
                  break;
                 }
              SetCursor(hCursorWait);
              GetModuleName(hModule,str1,sizeof(str1));
              wsprintf(szStr,"%s %s",(LPSTR)pszImport,(LPSTR)str1);
              SetWindowText(hDlg,szStr);
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);

              if (fNativeimport)
                  wMode = IL_SORT | IL_ALL;
              else
                  wMode = IL_SORT;

              y = ImportFirst(hModule,&importentry,wMode);
              lmod = 0;
              wZeilen = 0;
              wUndefs = 0;
              wByName = 0;

              while (y)
                  {
                   fUndef = FALSE;
                   if (fNativeimport)
                      {
                       if ((importentry.bRelocType & 0x03 == 0x01)
                       ||  (importentry.bRelocType & 0x03 == 0x02))
                           GetModuleName(importentry.hModule,str2,sizeof(str2));
                       else
                           str2[0] = 0;
                       wsprintf(str1,"%s(%X)",
                                (LPSTR)str2,
                                importentry.hModule
                               );
                       wsprintf(str2,"%X",importentry.wEntry);  /* index/offs */
                      }
                   else
                      {
                       if (lmod != importentry.hModule)            /* wenn modul wechselt */
                          {
                           if (lmod && ((int)procstruc.hMem != 0xFFFF))  /* altes abschliessen */
                              {
                               GlobalUnlock(procstruc.hMem);
                               GlobalFree(procstruc.hMem);
                              }

                           if (!GetModuleName(importentry.hModule,str1,sizeof(str1)))
                               wsprintf(str1,"%X (undef)",importentry.hModule);

                           procstruc.hMem = (HGLOBAL)0xFFFF;
                           lmod = importentry.hModule;
                          }
                             /* import by name? */
                             /* dann steht name in importierendem modul */
                       if ((importentry.bRelocType & 0x03) == 0x02)
                          {
                           if (!GetImportedName(hModule,
                                                importentry.wEntry,
                                                str2,
                                                sizeof(str2)
                                               ))
                               fUndef = TRUE;
                           wByName++;
                          }
                       else
                       if ((importentry.bRelocType & 0x03) == 0x01)
                         {
                          if (!GetProcName(importentry.hModule,
                                           importentry.wEntry,
                                           str2,
                                           sizeof(str2),
                                           &procstruc))
                              fUndef = TRUE;
                         }
                      }
                   if (fUndef)
                      {
                       wsprintf(str2,"%X (undef)",importentry.wEntry); /* index */
                       wUndefs++;
                      }

                   wsprintf(szStr,
                            "%04X:%04X\t%02X\t%02X\t%s.%s",
                            importentry.wSegment,        /* segmentnr */
                            LOWORD(importentry.fpAddr),  /* offset */
                            importentry.bRelocType,      /* reloc typ */
                            importentry.bAddrType,       /* addr typ */
                            (LPSTR)str1,                 /* modulname */
                            (LPSTR)str2                  /* entrynr|entryname */
                           );
                   if ((y = (WORD)SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)szStr)) == LB_ERRSPACE)
                      {
                       UpdateWindow(hWnd);
                       CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRIMP4),0,MB_OK);
                       fShow = FALSE;
                       i = 1;
                       break;
                      }
                   else
                       SendMessage(hWnd,LB_SETITEMDATA,y,(LONG)importentry.fpAddr - 1);

                   wZeilen++;
                   y = ImportNext(hModule,&importentry);

                  }                                             /* endfor i */
              if  (lmod && ((int)procstruc.hMem != 0xFFFF))
                  {
                   GlobalUnlock(procstruc.hMem);
                   GlobalFree(procstruc.hMem);
                  }

              ImportClose(hModule,&importentry);
              if (importentry.wError)
                 {
                  if (importentry.wError == IMPERROR_OPEN)
                      i = IDS_ERRIMP1;
                  else
                  if (importentry.wError == IMPERROR_ALLOC)
                      i = IDS_ERRIMP2;
                  else
                  if (importentry.wError == IMPERROR_READ)
                      i = IDS_ERRIMP3;
                  else
                      i = IDS_ERRIMP5;
                  CreateMessage(hDlg,(LPSTR)i,0,MB_OK);
                  fShow = FALSE;
                 }
              SendMessage(hWnd,XLB_SETWINDOWDATA,0,MAKELONG(wUndefs,wByName));

              SetSortParms(hDlg);

              EnableDlgItem(hDlg,ID_SUBDLG1,0);
              SetCursor(hCursor);

              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                 {
                  case LBN_SELCHANGE:                       /* selektion geaendert */
                     if (!CheckModul(hDlg,&hModule,&moduleentry,0))
                         break;
                     y =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
                     if (y == LB_ERR)
                         paddr = 0;
                     else
                        {
                         paddr = (FARPROC)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,y,0);
                         if (!IsReadableSelector((WORD)GetSegmentHandle(hModule,HIWORD(paddr))))
                             paddr = 0;
                        }
                     EnableDlgItem(hDlg,ID_SUBDLG1,paddr != 0);
                     break;
                  case LBN_DBLCLK:
                     PostMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                     break;
                  case XLBN_RBUTTONDOWN:
                     TrackPopup(hDlg,BtnTab);
                     break;
                 }
              break;
         }                                               /* end switch wParam */
        rc = TRUE;
        break;
   }
 return rc;
}
