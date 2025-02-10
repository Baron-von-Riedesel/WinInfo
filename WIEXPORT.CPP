
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

#define strlen lstrlen
#define strcpy lstrcpy
#define strcat lstrcat

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT   hFontAlt;
extern BYTE fShow;

BOOL CALLBACK UnassembleDlg  (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK Unassemble32Dlg(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK ImportedFromDlg(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK ModulEntriesDlg(HWND,UINT,WPARAM,LPARAM);

typedef struct tagEXPSTATUS {
   WORD wEntries;
   WORD wResName;
   WORD wNoName;
   int  iSort;
  } EXPSTATUS;

int xExppos   = 100,yExppos   = 100;

static EXPSTATUS es;

static char * pszExport = {"Exporte"};
static int idtab[] = {ID_SUBDLG4,ID_SUBDLG3,ID_SUBDLG1,ID_SUBDLG2,ID_REFRESH,0};
static int fSort = 0;

////////////////////////////////////////////////////////
// CheckModul - prufen ob modul noch da                 
////////////////////////////////////////////////////////
HMODULE WINAPI CheckModul(HWND hDlg,MODULEENTRY * pmodentry,PINT pint,WORD wMode)
{
//  char str[10];
  HMODULE hModule;

  hModule = (HMODULE)GetWindowWord(hDlg,DLGWINDOWEXTRA);
  (*pmodentry).dwSize = sizeof(MODULEENTRY);
  if (ModuleFindHandle(pmodentry,hModule))
      return hModule;

  if (pint)
      for (;*pint;pint++)
          EnableDlgItem(hDlg,*pint,0);

  if (wMode)
      CreateMessage(hDlg,errorstr1,0,MB_OK);
  return FALSE;
}
////////////////////////////////////////////////////////
// SetSortParms                                         
////////////////////////////////////////////////////////
static void SetSortParms(HWND hDlg)
{
 WORD wSortPos;
 HWND hWnd;
 EXPSTATUS * pes;
 char szStr[80];
 char str1[20];

  SetCursor(hCursorWait);
  hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
  pes = (EXPSTATUS *)GetWindowWord(hDlg,DLGWINDOWEXTRA+2);
  if (pes->iSort == 3)
     wSortPos = 17;
  else
  if (pes->iSort == 2)
     wSortPos = 15;
  else
  if (pes->iSort == 1)
     wSortPos = 5;
  else
     wSortPos = 0;
  SendMessage(hWnd,XLB_SETSORTSTART,wSortPos,0);
  InvalidateRect(hWnd,0,1);

  GetWindowText(hWnd,szStr,sizeof(szStr));
  GetTextField(szStr,pes->iSort,str1,sizeof(str1));
  wsprintf(szStr,
           "%s\t%u\tsortiert nach\t%s\tresident\t%u\to. Namen\t%u\t",
           (LPSTR)pszExport,
           pes->wEntries,
           (LPSTR)str1,
           pes->wResName,
           pes->wNoName
           );
  SetDlgItemText(hDlg,ID_STATUS1,szStr);

  SetCursor(hCursor);
}
////////////////////////////////////////////////////////
// ExportDlg - exportierte Entries auflisten            
////////////////////////////////////////////////////////
BOOL CALLBACK ExportDlg(HWND hDlg,unsigned message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_SUBDLG1,ID_SUBDLG2,0};
 WORD  y,i;
 EXPSTATUS * pes;
 char szStr[80];
 char szStr1[128];
 char cRes;
 PSTR pStr,pStr2;
 FARPROC paddr;
 WORD  wSortPos;
 HWND  hWnd;
 MODULEENTRY moduleentry;
 EXPORTENTRY exportentry;
 ENTRYENTRY  entryentry;
 LPMODSTRUC lpmodstruc;
 HMODULE hModule;
 HANDLE hMem;
 BOOL rc = FALSE;

 switch (message)
    {
    case WM_INITDIALOG:
      if (!(pes = (EXPSTATUS *)LocalAlloc(LMEM_FIXED,sizeof(EXPSTATUS))))
          pes = &es;
      pes->iSort = fSort;
      SetWindowLong(hDlg,DLGWINDOWEXTRA,MAKELONG(LOWORD(lParam),pes));
      LoadTabs(IDUS_12,szStr);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      LoadTabs(IDUS_13,szStr);
      SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
      SetWindowPos(hDlg,0,xExppos,yExppos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      if (fShow)
          ShowWindow(hDlg,SW_SHOWNORMAL);
      else
          ShowWindow(hDlg,SW_SHOWNOACTIVATE);

      rc = TRUE;
      break;
    case WM_CLOSE:
      LocalFree((HLOCAL)GetWindowWord(hDlg,DLGWINDOWEXTRA+2));
      SetCursor(hCursor);
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xExppos,&yExppos);
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_SUBDLG3:     /* Umsortieren */
              if (!(hModule = CheckModul(hDlg,&moduleentry,idtab,1)))
                  break;
              pes = (EXPSTATUS *)GetWindowWord(hDlg,DLGWINDOWEXTRA+2);
              pes->iSort++;
              if (pes->iSort == 2)
                  pes->iSort++;
              else
              if (pes->iSort > 3)
                  pes->iSort = 0;
              fSort = pes->iSort;
              SetSortParms(hDlg);
              break;
         case ID_SUBDLG1:     /* Disassembler */
              if (!(hModule = CheckModul(hDlg,&moduleentry,idtab,1)))
                  break;
              y =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
              if (y == LB_ERR)
                  break;
              y = (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,y,0);
              paddr = (FARPROC)GetEntryAddress(hModule,y);
              if (paddr)
                 {
                  if (!IsReadableSelector(HIWORD(paddr)))
                     break;
                  else
                     hWnd = CreateDialogParam(hInst,
                                              MAKEINTRESOURCE(IDD_UNASSEM),
                                              hDlg,
                                              UnassembleDlg,
                                              (DWORD)paddr
                                             );
                 }
              break;
         case ID_SUBDLG2:     /* Importiert von */
              if (!(hModule = CheckModul(hDlg,&moduleentry,idtab,1)))
                  break;
              y =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
              if (y == LB_ERR)
                  break;
              y = (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,y,0);
//              y = GetEntryID(hModule,HIWORD(paddr),LOWORD(paddr));
              hWnd = CreateDialogParam(hInst,
                                       MAKEINTRESOURCE(IDD_IMPORTEDFROM),
                                       hDlg,
                                       ImportedFromDlg,
                                       MAKELONG(y,hModule)
                                      );
              break;
         case ID_SUBDLG4:     /* alle entries */
              if (!(hModule = CheckModul(hDlg,&moduleentry,idtab,1)))
                  break;
              y =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
              if (y == LB_ERR)
                  break;
              y = (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,y,0);
              hWnd = CreateDialogParam(hInst,
                                       MAKEINTRESOURCE(IDD_ENTRY),
                                       hDlg,
                                       ModulEntriesDlg,
                                       (LPARAM)(LPVOID)hModule
                                      );
              break;
         case ID_REFRESH:
              fShow = TRUE;
              pes = (EXPSTATUS *)GetWindowWord(hDlg,DLGWINDOWEXTRA+2);
              if (!(hModule = CheckModul(hDlg,&moduleentry,idtab,1)))
                 {
                  fShow = FALSE;
                  break;
                 }
              SetCursor(hCursorWait);
              EnableDlgItem(hDlg,ID_SUBDLG1,0);
              lpmodstruc = (LPMODSTRUC)MAKELONG(0,GlobalHandleToSel(hModule));
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              GetModuleName(hModule,szStr1,sizeof(szStr1));
              wsprintf(szStr,"%s %s",(LPSTR)pszExport,(LPSTR)szStr1);
              SetWindowText(hDlg,szStr);
//              SendMessage(hWnd,WM_SETREDRAW,0,0);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);

              pes->wEntries = 0;
              pes->wNoName  = 0;
              pes->wResName = 0;
#ifdef _NAME_
              y = ExportFirst(hModule,&exportentry);
              while (y)
                  {
                   if (exportentry.wType == 1)
                       strcpy(szStr,"(R)");
                   else
                       szStr[0] = 0;
                   wsprintf(szStr1,
                            "%04X\t%04X:%04X\t%s%s",
                            exportentry.wEntry,           /* index */
                            HIWORD(exportentry.fpAddr),   /* proc addr */
                            LOWORD(exportentry.fpAddr),
                            (LPSTR)exportentry.szProcName,/* entry name */
                            (LPSTR)szStr                  /* (R) */
                           );

                   y = SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)szStr1);
                   SendMessage(hWnd,LB_SETITEMDATA,y,(LONG)exportentry.wEntry);

                   pes->wEntries++;
                   y = ExportNext(hModule,&exportentry);
                  }
              ExportClose(hModule,&exportentry);
              if (exportentry.wError)
                 {
                  if (exportentry.wError == EXPERROR_OPEN)
                      i = IDS_ERREXP1;
                  else
                  if (exportentry.wError == EXPERROR_ALLOC)
                      i = IDS_ERREXP2;
                  else
                      i = IDS_ERREXP3;
                  CreateMessage(hDlg,MAKEINTRESOURCE(i),0,MB_OK);
                  fShow = FALSE;
                 }
#else
              hMem = GetNResNames(hModule);
              y = GetFirstEntry(hModule,&entryentry);
              while (y)
                 {
                  if (entryentry.flags & 1)
                     {
                      paddr = (FARPROC)GetEntryAddress(hModule,entryentry.id);
                      szStr[0] = 0;
                      i = GetProcedureNameEx(hModule,entryentry.id,szStr,sizeof(szStr)-4,hMem);
                      if (i & 0x8000)
                         {
                          cRes = 'R';
                          pes->wResName++;
                         }
                      else
                         {
                          cRes = ' ';
                          if (!i)
                             pes->wNoName++;
                         }

                      wsprintf(szStr1,
                               "%04X\t%04X:%04X\t%c\t%s",
                               entryentry.id,
                               HIWORD(paddr),
                               entryentry.offset,
                               cRes,
                               (LPSTR)szStr
                          );
                      y = SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)szStr1);
                      SendMessage(hWnd,LB_SETITEMDATA,y,entryentry.id);
                      pes->wEntries++;
                     }
                  y = GetNextEntry(hModule,&entryentry);
                 }
              GlobalFree(hMem);
#endif
              SetSortParms(hDlg);

              SendMessage(hWnd,LB_SETCURSEL, 0, 0);

              SetCursor(hCursor);
              break;
         case ID_LISTBOX1:
              hModule = CheckModul(hDlg,&moduleentry,idtab,0);
              switch(HIWORD(lParam))
                 {
                  case LBN_SELCHANGE:                       /* selektion geaendert */
                     if (!hModule)
                         break;
                     y =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
                     if (y == LB_ERR)
                         paddr = 0;
                     else
                        {
                         y = (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETITEMDATA,y,0);
                         paddr = (FARPROC)GetEntryAddress(hModule,y);
                         if (!IsReadableSelector(HIWORD(paddr)))
                             paddr = 0;
                        }
                     EnableDlgItem(hDlg,ID_SUBDLG1,paddr != 0);
                     EnableDlgItem(hDlg,ID_SUBDLG2,paddr != 0);
                     break;
                  case LBN_DBLCLK:
                     PostMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                     break;
                  case XLBN_RBUTTONDOWN:
                     TrackPopup(hDlg,BtnTab);
                     break;
                 }
              break;
         }
      rc = TRUE;
      break;
    default:
      break;
    }
 return rc;
}
