
// #define _TRACE_

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

// #define strcpy _fstrcpy

BOOL CALLBACK AtomDlg    (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ViewDlg    (HWND, UINT, WPARAM, LPARAM);

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT   hFontAlt;
extern BYTE fShow;
extern LPSTR lpszHint;

int xLocpos   = 100,yLocpos   = 100;

static char * pszAtome = {"Atome"};
static char * pszViewLocal = {"Local Heap Objekt"};

static KEYENTRY gditypetab[] = {
                  "Pen"       ,LT_GDI_PEN,
                  "Brush"     ,LT_GDI_BRUSH,
                  "Font"      ,LT_GDI_FONT,
                  "Palette"   ,LT_GDI_PALETTE,
                  "Bitmap"    ,LT_GDI_BITMAP,
                  "Region"    ,LT_GDI_RGN,
                  "DC"        ,LT_GDI_DC,
                  "DisabledDC",LT_GDI_DISABLED_DC,
                  "MetaDC"    ,LT_GDI_METADC,
                  "MetaFile"  ,LT_GDI_METAFILE,
                  (PSTR)0};

static KEYENTRY usertypetab[] = {
               "Class"             ,LT_USER_CLASS              ,
               "Wnd"               ,LT_USER_WND                ,
               "String"            ,LT_USER_STRING             ,
               "Menu"              ,LT_USER_MENU               ,
               "Clip"              ,LT_USER_CLIP               ,
               "CBox"              ,LT_USER_CBOX               ,
               "Palette"           ,LT_USER_PALETTE            ,
               "ED"                ,LT_USER_ED                 ,
               "BWL"               ,LT_USER_BWL                ,
               "Ownerdraw"         ,10, // LT_USER_OWNERDRAW          ,
               "SPB"               ,LT_USER_SPB                ,
               "Checkpoint"        ,LT_USER_CHECKPOINT         ,
               "DCE"               ,LT_USER_DCE                ,
               "MWP"               ,LT_USER_MWP                ,
               "Prop"              ,LT_USER_PROP               ,
               "LBiv"              ,LT_USER_LBIV               ,
               "Misc"              ,LT_USER_MISC               ,
               "Atoms"             ,LT_USER_ATOMS              ,
               "Lockinputstate"    ,LT_USER_LOCKINPUTSTATE     ,
               "Hooklist"          ,LT_USER_HOOK	           ,
               "Userseeuserdoalloc",LT_USER_USERSEEUSERDOALLOC ,
               "Hotkeylist"        ,LT_USER_HOTKEYLIST         ,
               "Popupmenu"         ,LT_USER_POPUPMENU          ,
               "Handletable"       ,LT_USER_HANDLETABLE        ,
               (PSTR)0};
////////////////////////////////////////////////////////
// CheckLocalHeap                                       
////////////////////////////////////////////////////////
int CheckLocalHeap(HWND hDlg, HGLOBAL hLocal)
{
 int rc;

 if (!(rc = IsValidLocalHeap(hLocal)))
     CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRLOC1),0,MB_OK);
  return rc;
}
////////////////////////////////////////////////////////
// Ueberpruefen ob bestimmtes modul                     
////////////////////////////////////////////////////////
BOOL modulename(HMODULE handle,PSTR pstr)
{
 MODULEENTRY modentry;
 modentry.dwSize = sizeof(modentry);
 if (ModuleFindName(&modentry,pstr))
     if (GetModuleDgroup(modentry.hModule) == (HANDLE)GlobalHandleToSel(handle))
        return TRUE;
 return FALSE;
}
////////////////////////////////////////////////////////
// LocalDlg - Local Heap anzeigen                       
////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK LocalDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
  static int BtnTab[] = {ID_VIEWLOCAL,0};
  static fSort = 1;
  HEXDUMPPARM hdp;
  char str[80];
  char str1[100];
  char szStr2[40];
  int    hFile;
  HANDLE hAtom;
  PSTR pstr;
  int i,x,y,anzahl;
  HWND hWnd;
  WORD wSortPos;
  DWORD dx;
  DWORD * p1;
  LOCALENTRY localentry;
  HGLOBAL hLocal;
//  HDC hDC;
  BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
        TRACEOUT("LocalDlg: WM_INITDIALOG received");
        SetWindowWord(hDlg,DLGWINDOWEXTRA,LOWORD(lParam));
        LoadTabs(IDUS_21,str);
        SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
        SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                           (LPARAM)(LPVOID)hFontAlt);
        LoadTabs(IDUS_20,str);
        SendDlgItemMessage(hDlg,ID_STATLOCAL,ST_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
        SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
        if (fShow)
           {
            SetWindowPos(hDlg,0,xLocpos,yLocpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
            ShowWindow(hDlg,SW_NORMAL);
           }
        else
            PostMessage(hDlg,WM_CLOSE,0,0);
        if (HIWORD(lParam))
           {
            GetModuleName((HMODULE)HIWORD(lParam),str1,sizeof(str1));
            wsprintf(str,"Local Heap: %s",(LPSTR)str1);
            SetWindowText(hDlg,str);
           }
        TRACEOUT("LocalDlg: WM_INITDIALOG successfully handled");
        rc = TRUE;
        break;
    case WM_CLOSE:
        PostMessage(GetParent(hDlg),WM_COMMAND,ID_DESTROY,(LPARAM)(LPVOID)hDlg);
        DestroyWindow(hDlg);
        rc = TRUE;
        break;
    case WM_MOVE:
        SaveWindowPos(hDlg,&xLocpos,&yLocpos);
        break;
    case WM_COMMAND:
        hLocal = (HGLOBAL)GetWindowWord(hDlg,DLGWINDOWEXTRA);
        switch (wParam)
          {
           case ID_SNAPSHOT:
              SetCursor(hCursorWait);
              GetTempFileName(0,"LCH",0,str);
              hFile = _lcreat(str,0);
              GetDlgItemText(hDlg,ID_LISTBOX1,str1,sizeof(str1));
              _lwriteline(hFile,str1);
              SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SAVEBOX,hFile,0);
              GetDlgItemText(hDlg,ID_STATLOCAL,str1,sizeof(str1));
              _lwriteline(hFile,str1);
              _lclose(hFile);
              PutFiletoClipboard(str,hDlg);
              SetCursor(hCursor);
              wsprintf(str1,"Gespeichert unter\n%s",(LPSTR)str);
              CreateMessage(hDlg,str1,lpszHint,MB_OK);
              break;
           case IDCANCEL:
           case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
           case ID_REFRESH:
              TRACEOUT("LocalDlg: enter refreshing Listbox");
              fShow = FALSE;
              if (!(anzahl = CheckLocalHeap(hDlg,hLocal)))
                  break;

              SetCursor(hCursorWait);
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);


//              SendMessage(hWnd,WM_SETREDRAW,0,0);
              ListBox_ResetContent(hWnd);

              if (fSort)
                  wSortPos = 5;
              else
                  wSortPos = 0;
              SendMessage(hWnd,XLB_SETSORTSTART,wSortPos,0);

              i = 0;
              x = 0;
              TRACEOUT("LocalDlg: will do LocalFirst()");
              localentry.dwSize = sizeof(localentry);
              y = LocalFirst(&localentry,hLocal);
              TRACEOUT("LocalDlg: enter LocalNext() loop");
              while (y) {
                  str1[0] = 0;
                  if (localentry.wFlags == LF_FREE)
                      strcpy(szStr2,"frei");
                  else {
                      x = x + localentry.wSize;
                      if (localentry.wFlags == LF_FIXED)
                         strcpy(szStr2,"fixed");
                      else
                      if (localentry.wFlags == LF_MOVEABLE)
                         strcpy(szStr2,"movbl");
                      else
                         wsprintf(szStr2,"%X",localentry.wFlags);

                      if (localentry.wHeapType == GDI_HEAP)
                         {
                          if (pstr = SearchKeyTable((PSTR)&gditypetab,localentry.wType))
                              strcpy(str1,pstr);
                         }
                      else
                      if (localentry.wHeapType == USER_HEAP)
                          if (pstr = SearchKeyTable((PSTR)&usertypetab,localentry.wType))
                              strcpy(str1,pstr);
                          else
                          if (IsWindow((HWND)localentry.hHandle))
                              strcpy(str1,"Wnd");
                          else
                          if (IsClass(localentry.hHandle))
                              strcpy(str1,"Class");
                  } // end if (localentry.wFlags == LF_FREE)

                  if (str1[0] == 0)
                      wsprintf(str1,"%X",localentry.wType);

                  wsprintf(str,
                           "%04X\t%04X\t%u\t\t%s\t%X\t%s",
                           localentry.hHandle,
                           localentry.wAddress,
                           localentry.wSize,           /* size  */
                           (LPSTR)szStr2,              /* flags (im Klartext) */
                           localentry.wcLock,          /* locks */
                           (LPSTR)str1                 /* typ (GDI) */
//                           localentry.hHeap            /* ? */
//                           localentry.wHeapType,       /* ? */
//                           localentry.wNext
                          );          /* ? */
                  y = ListBox_AddString(hWnd, str);
                  ListBox_SetItemData(hWnd,y,MAKELONG(localentry.wAddress,localentry.wSize));
                  y = LocalNext(&localentry);
                  i++;
              }				// end while (y)
              TRACEOUT("LocalDlg: LocalNext() loop ended");
              ListBox_SetCurSel(hWnd,0);
              GetWindowText(hWnd,str,sizeof(str));
              GetTextField(str,fSort,str1,sizeof(str1));
              wsprintf(str,"Objekte\t%u\tbelegt\t%u\tsortiert nach\t%s\t",i,x,(LPSTR)str1);
              SetDlgItemText(hDlg,ID_STATLOCAL,str);
              EnableDlgItem(hDlg,ID_VIEWLOCAL,i);
              TRACEOUT("LocalDlg: Checking existance of atom table");
              hAtom = (HANDLE)*(LPINT)MAKELONG(8,hLocal);
              if (hAtom)
                 {
                  if (i = (int)BasedLocalLock(hLocal,hAtom))
                      BasedLocalUnlock(hLocal,hAtom);
                 }
              else
                  i = 0;
              EnableDlgItem(hDlg,ID_ATOMLOCAL,i);
              SetCursor(hCursor);
              fShow = TRUE;
              TRACEOUT("LocalDlg: exit refreshing Listbox");
              break;
           case ID_VIEWLOCAL:
              if (!CheckLocalHeap(hDlg,hLocal))
                  break;

              x = (WORD)SendDlgItemMessage(hDlg, ID_LISTBOX1,LB_GETCURSEL,0,0);
              dx = (LONG)SendDlgItemMessage(hDlg, ID_LISTBOX1, LB_GETITEMDATA,x,0);

              hdp.dwOffset = LOWORD(dx);
              hdp.hGlobal = hLocal;
              hdp.dwLength = HIWORD(dx);
              hdp.wType   = 1;
              hWnd = CreateDialogParam( hInst,
                                        MAKEINTRESOURCE(IDD_VIEW),
                                        hDlg,
                                        ViewDlg,
                                        (LPARAM)(LPVOID)&hdp
                                        );
              wsprintf(str,"%s: %X",(LPSTR)pszViewLocal,hdp.dwOffset);
              SetWindowText(hWnd,str);
              break;
         case ID_SORTLOCAL:
              fSort = 1 - fSort;
              PostMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
              break;
         case ID_COMPLOCAL:
              if (!CheckLocalHeap(hDlg,hLocal))
                  break;
              i = BasedLocalCompact((HGLOBAL)GlobalHandleToSel(hLocal),0xFFFF);
              LoadString(hInst,IDS_LOCSTR1,str1,sizeof(str1));
              wsprintf(str,str1,i);
              CreateMessage(hDlg,
                            str,
                            MAKEINTRESOURCE(IDS_HINT),
                            MB_OK);
              break;
         case ID_ATOMLOCAL:
              if (!CheckLocalHeap(hDlg,hLocal))
                  break;

              hWnd = (HWND)GetWindowWord(hDlg,DLGWINDOWEXTRA+2);
              if (hWnd)
                  ShowWindow(hWnd,SW_RESTORE);
              else
                 {
                  hWnd = CreateDialogParam(hInst,
                                           MAKEINTRESOURCE(IDD_ATOMBOX),
                                           hDlg,
                                           AtomDlg,
                                           (LPARAM)(LPVOID)hLocal);
                  SetWindowWord(hDlg,DLGWINDOWEXTRA+2,(WORD)hWnd);
                  wsprintf(str,"%s %X",(LPSTR)pszAtome,hLocal);
                  SetWindowText(hWnd,str);
                 }
              break;
         case ID_DESTROY:
              hWnd = (HWND)GetWindowWord(hDlg,DLGWINDOWEXTRA+2);
              if (hWnd == (HWND)LOWORD(lParam))
                  SetWindowWord(hDlg,DLGWINDOWEXTRA+2,0);
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                 {
                  case LBN_DBLCLK:                       /* doppelclick auf item */
                     PostMessage(hDlg,WM_COMMAND,ID_VIEWLOCAL,0);
                     break;
                  case XLBN_RBUTTONDOWN:
                     TrackPopup(hDlg,BtnTab);
                     break;
                 }
              break;
           }
        rc = TRUE;
        break;
   }
 return rc;
}
