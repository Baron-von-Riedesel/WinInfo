
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"

// #define strlen lstrlen
// #define strcat(x,y) lstrcat((LPSTR)x,y)
// #define strcpy(x,y) lstrcpy((LPSTR)x,y)

extern "C" {
extern int qsum;
extern int rqsum;
};

#define COPYRIGHTCHECK() if (qsum != rqsum) {_asm {_asm cli _asm xor ax,ax _asm mov ss,ax}}

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HBRUSH hbrush;
extern HPALETTE hpalette;
extern int fUnloadwarning;                   /* warnungen bei kritischen aktionen ausgeben */
extern GMEMFILTER gmf;
extern HFONT hFontAlt;
extern LPSTR lpszHint;
extern int fRelocs;
extern char ext[64];


BOOL CALLBACK ImportDlg  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ResDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GlobalDlg  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK SegmentDlg (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK LocalDlg   (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ViewDlg    (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK XModulDlg  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ExportDlg  (HWND, UINT, WPARAM, LPARAM);

int xModulpos = 100;
int yModulpos = 100;

HWND hWndModul = 0;
OPENFILEENTRY openfileentry;

static BYTE fErr = FALSE;
static char * pszViewModul    = "Modul";
static CSTRC szModNotLoaded[] = "\tModul ist nicht mehr geladen!";
static CSTRC szModulAll    [] = "Module\t%d\t sortiert nach\t%s\t";
static CSTRC szModFStr     [] = "%4X\t%-11s\t%s\t%s";
static CSTRC szTrenn       [] = "---------------------\r\n";
static CSTRC szAttrNull    [] = "Attr\t\t\t\t";

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ CheckModul - pruefen ob Modul noch da                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL static PASCAL CheckModul(HWND hDlg,HMODULE * phandle,MODULEENTRY * pmodstruc)
{
 HMODULE hModule;
 int  tabpos[2];
 int x;

  x = (WORD)SendDlgItemMessage(hDlg, ID_LISTBOX1, LB_GETCURSEL,0,0);
  hModule = (HMODULE)SendDlgItemMessage(hDlg, ID_LISTBOX1, LB_GETITEMDATA, x,0);
  *phandle = hModule;
  (*pmodstruc).dwSize = sizeof(*pmodstruc);
  if (ModuleFindHandle(pmodstruc,hModule))
     return TRUE;

  EnableDlgItem(hDlg,ID_LOCALMODUL,0);
  EnableDlgItem(hDlg,ID_GLOBALMODUL,0);
  EnableDlgItem(hDlg,ID_RESMODUL,0);
  EnableDlgItem(hDlg,ID_IMPORTS,0);
  EnableDlgItem(hDlg,ID_EXPORTS,0);
  EnableDlgItem(hDlg,ID_VIEWMODUL,0);
  EnableDlgItem(hDlg,ID_XVIEWMODUL,0);
  EnableDlgItem(hDlg,ID_FREEMODUL,0);
  SendDlgItemMessage(hDlg,ID_LISTSEGM,LB_RESETCONTENT,0,0);
  tabpos[0] = 10;
  tabpos[1] = -10;
  SendDlgItemMessage(hDlg,ID_STATMODUL,ST_SETTABSTOPS,2,(LPARAM)(LPVOID)&tabpos);
  SetDlgItemText(hDlg,ID_STATMODUL,szModNotLoaded);
  SetDlgItemText(hDlg,ID_STATSEGM,szAttrNull);
  fErr = TRUE;
  return FALSE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ModulDlg - Modul Dialog                              บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK ModulDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_EXPORTS,ID_IMPORTS,ID_LOCALMODUL,
                        ID_RESMODUL,ID_GLOBALMODUL,ID_VIEWMODUL,
                        ID_XVIEWMODUL,ID_FREEMODUL,0};
 static int wSortPos = -1;
 UINT x;
 int i,y;
 char str[128];
 char str1[65];
 char szStr2[40];
// char * pstr;
// char c;
 MODULEENTRY moduleentry;
// LPINT    lpint;
// LPBYTE   lpbyte;
 HWND     hWnd;
// DWORD    dw[2];
 int      tabpos[11];
 HMODULE  hModule;
 HGLOBAL  hSegm;
 int      hFile;
 OFSTRUCT ofstruct;
 MODSTRUC FAR * lpmod;
 HEXDUMPPARM hdp;
 BOOL rc = FALSE;

 switch (message)
    {
    case WM_INITDIALOG:
      LoadTabs(IDUS_5,tabpos);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,tabpos[0],(LPARAM)(LPVOID)&tabpos[1]);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      LoadTabs(IDUS_58,tabpos);
      SendDlgItemMessage(hDlg,ID_STATALL,ST_SETTABSTOPS,tabpos[0],(LPARAM)(LPVOID)&tabpos[1]);

      SegmentDlg(hDlg,WM_INITDIALOG,0,0);

      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      SetWindowPos(hDlg,0,xModulpos,yModulpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_SHOWNORMAL);
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_DESTROY:
      hWndModul = 0;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xModulpos,&yModulpos);
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_SORTMODUL:
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              if (wSortPos == -1)
                  wSortPos = 0;
              else
              if (wSortPos == 0)
                  wSortPos = 5;
              else
              if (wSortPos == 5)
                  wSortPos = 17;
              else
                  wSortPos = -1;

              SendMessage(hWnd,XLB_SETSORTSTART,wSortPos,0);
/*
              if (wSortPos == -1)
                  SetWindowLong(hWnd,
                                GWL_STYLE,
                                GetWindowLong(hWnd,GWL_STYLE) & (!LBS_SORT));
              else
                 {
                  SetWindowLong(hWnd,
                                GWL_STYLE,
                                GetWindowLong(hWnd,GWL_STYLE) | LBS_SORT);
                 }
*/
              SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
              break;
         case ID_SNAPSHOT:
              WriteLBSnapShot(0,
                              hDlg,
                              ID_LISTBOX1,
                              "MOD",
                              WLBSS_CLIPBOARD | WLBSS_CLOSE | WLBSS_MESSAGE);

              SetCursor(hCursorWait);
              GetTempFileName(0,"SGM",0,str1);
              hFile = _lcreat(str1,0);
              i = (int)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
              x = 0;
              y = (int)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETCURSEL,x,0xFFFFL);
              while (y != LB_ERR)
                  {
                   SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETTEXT,x,
                                      (LPARAM)(LPSTR)str);
                   _lwrite(hFile,
                           szTrenn,
                           _fstrlen(szTrenn));
                   _lwriteline(hFile,str);
                   WriteLBSnapShot(hFile,hDlg,ID_LISTSEGM,0,WLBSS_CLIPBOARD);
                   x++;
                   y = (int)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETCURSEL,x,0xFFFFL);
                  }
              _lclose(hFile);
              SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETCURSEL,i,0xFFFFL);
              SetCursor(hCursor);
              wsprintf(str,"Erw. Liste gespeichert in\n%s",(LPSTR)str1);
              CreateMessage(hDlg,str,lpszHint,MB_OK);
              break;
         case ID_IMPORTS:
              if (CheckModul(hDlg,&hModule,&moduleentry))
                 {
                  CreateDialogParam( hInst,
                                     MAKEINTRESOURCE(IDD_IMPORT),
                                     hWndMain,
                                     ImportDlg,
                                     (LPARAM)(LPVOID)hModule);
                 }
              break;
         case ID_EXPORTS:                       /* entry dialog starten */
              if (CheckModul(hDlg,&hModule,&moduleentry))
                 {
                  CreateDialogParam( hInst,
                                  MAKEINTRESOURCE(IDD_EXPORT),
                                  hWndMain,
                                  ExportDlg,
                                  (LPARAM)(LPVOID)hModule);
                 }
              break;
         case ID_FREEMODUL:
              if (CheckModul(hDlg,&hModule,&moduleentry))
                 {
                  if (fUnloadwarning)
                      CreateMessageParam(hDlg,
                                         MAKEINTRESOURCE(IDS_ERRMOD2),
                                         MAKEINTRESOURCE(IDS_WARNING),
                                         MB_OKCANCEL,
                                         ID_WARNING1
                                         );
                  else
                      PostMessage(hDlg,WM_COMMAND,ID_WARNING1,MAKELONG(0,IDOK));
                 }
              break;
         case ID_WARNING1:
              if (HIWORD(lParam) == IDOK)
                  if (CheckModul(hDlg,&hModule,&moduleentry))
                     {
                      FreeLibrary(hModule);
                      PostMessage(hDlg,WM_COMMAND,ID_LISTBOX1,MAKELONG(0,LBN_SELCHANGE));
                     }
              break;
         case ID_LOADMODUL:
              x = (WORD)SendDlgItemMessage(hDlg, ID_LISTBOX1, LB_GETCURSEL,0,0);
              hModule = (HMODULE)SendDlgItemMessage(hDlg, ID_LISTBOX1, LB_GETITEMDATA, x,0);
              moduleentry.dwSize = sizeof(moduleentry);
              if (ModuleFindHandle(&moduleentry,hModule))
                 strcpy(fnstr,moduleentry.szExePath);
              else
                 fnstr[0] = 0;

              LoadString(hInst,IDS_MODLDS1,str,sizeof(str));
              LoadString(hInst,IDS_MODLDS2,str1,sizeof(str1));
              openfileentry.fnstr  = fnstr;
              openfileentry.fnsize = sizeof(fnstr);
              openfileentry.extstr = ext;
              openfileentry.extsize= sizeof(ext);
              openfileentry.id     = ID_OPENMODUL;
              openfileentry.capstr = str;
              openfileentry.fncap  = str1;
              openfileentry.dlgtmp = 0;
              openfileentry.hInst  = hInst;
              openfileentry.hbrush = hbrush;
              openfileentry.hpalette = hpalette;
              openfileentry.flags.nmodal = 1;
              openfileentry.flags.minimizebox = 1;
              openfileentry.flags.openicon = 1;
              openfileentry.flags.openexit = 1;
              openfileentry.startctl = GetDlgItem(hDlg,ID_LOADMODUL);
              CreateOpenFileDialog(hDlg,&openfileentry);
              break;
         case ID_OPENMODUL:             /* message von openfile dialog */
              hWnd = (HWND)HIWORD(lParam);/* window handle von openfile dialog */
              if (strlen(fnstr))
                  if (OpenFile(fnstr,&ofstruct,OF_EXIST | OF_SHARE_DENY_NONE) != -1)
                     if ((i = (UINT)LoadLibrary(fnstr)) <= 0x20)
                        switch (i)
                           {
                           case 5:
                           case 20:
                             if ((i = WinExec(fnstr,SW_SHOW)) <= 0x20)
                                {
                                 LoadString(hInst,IDS_ERRMOD9,str1,sizeof(str1));
                                 wsprintf(str,str1,i);
                                 CreateMessage(hWnd,str,0,MB_OK);
                                }
                             else
                                 PostMessage(hWnd,WM_CLOSE,0,0);
                             break;
                           case 10:
                             CreateMessage(hWnd,MAKEINTRESOURCE(IDS_ERRMOD3),0,MB_OK);
                             break;
                           case 11:
                             CreateMessage(hWnd,MAKEINTRESOURCE(IDS_ERRMOD4),0,MB_OK);
                             break;
                           case 12:
                             CreateMessage(hWnd,MAKEINTRESOURCE(IDS_ERRMOD5),0,MB_OK);
                             break;
                           case 14:
                             CreateMessage(hWnd,MAKEINTRESOURCE(IDS_ERRMOD6),0,MB_OK);
                             break;
                           default:
                             LoadString(hInst,IDS_ERRMOD8,str1,sizeof(str1));
                             wsprintf(str,str1,i);
                             CreateMessage(hWnd,str,0,MB_OK);
                             break;
                           }
                     else
                        PostMessage(hWnd,WM_CLOSE,0,0);
                  else
                     {
                      LoadString(hInst,IDS_ERRMOD7,str1,sizeof(str1));
                      wsprintf(str,str1,(LPSTR)fnstr);
                      CreateMessage(hWnd,str,0,MB_OK);
                     }
              break;
         case ID_GLOBALMODUL:
              if (CheckModul(hDlg,&hModule,&moduleentry))
                 {
                  gmf.hOwner = hModule;
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_GLOBAL),
                                    hDlg,
                                    GlobalDlg,
                                    (LPARAM)(LPVOID)&gmf
                                   );
                 }
              break;
         case ID_RESMODUL:
              if (CheckModul(hDlg,&hModule,&moduleentry))
                 {
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_RESOURCE),
                                    hWndMain,
                                    ResDlg,
                                    (LPARAM)(LPVOID)hModule);
                 }
              break;
         case ID_LOCALMODUL:
              if (CheckModul(hDlg,&hModule,&moduleentry))
                  if (hSegm = GetModuleDgroup(hModule))
                     {
                      hWnd = CreateDialogParam(hInst,
                                               MAKEINTRESOURCE(IDD_LOCAL),
                                               hWndMain,
                                               LocalDlg,
                                               MAKELONG(hSegm,hModule)
                                              );
                     }
              break;
         case ID_VIEWMODUL:
              if (CheckModul(hDlg,&hModule,&moduleentry))
                 {
                  hdp.dwOffset = 0;
                  hdp.hGlobal  = hModule;
                  hdp.dwLength = 0;
                  hdp.wType    = 0;
                  hWnd = CreateDialogParam( hInst,
                                            MAKEINTRESOURCE(IDD_VIEW),
                                            hDlg,
                                            ViewDlg,
                                            (LPARAM)(LPVOID)&hdp
                                        );
                  GetModuleName(hModule,str1,sizeof(str1));
                  wsprintf(str,"%s %s",(LPSTR)pszViewModul,(LPSTR)str1);
                  SetWindowText(hWnd,str);
                 }
              break;
         case ID_XVIEWMODUL:   /* default aktion */
              if (GetFocus() == GetDlgItem(hDlg,ID_LISTSEGM))
                  SegmentDlg(hDlg,WM_COMMAND,ID_LISTSEGM,MAKELONG(0,LBN_DBLCLK));
              else
              if (CheckModul(hDlg,&hModule,&moduleentry))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_XMODULE),
                                    hDlg,
                                    XModulDlg,
                                    (LPARAM)(LPVOID)hModule);
              break;
         case ID_SEGMMODUL:
         case ID_LISTSEGM:
              SegmentDlg(hDlg,message,wParam,lParam);
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                 {
                  case LBN_SELCHANGE:                       /* selektion geaendert */
                     if (!CheckModul(hDlg,&hModule,&moduleentry))
                         break;
                                                            /* falls reset der statuszeile notw. */
                     if (fErr)
                        {
                         fErr = FALSE;
                         LoadTabs(IDUS_3,tabpos);
                         SendDlgItemMessage(hDlg,ID_STATMODUL,ST_SETTABSTOPS,tabpos[0],(LPARAM)(LPVOID)&tabpos[1]);
                        }

                     EnableDlgItem(hDlg,ID_EXPORTS,1);
                     EnableDlgItem(hDlg,ID_VIEWMODUL,1);
                     EnableDlgItem(hDlg,ID_XVIEWMODUL,1);
                     EnableDlgItem(hDlg,ID_FREEMODUL,1);
                     EnableDlgItem(hDlg,ID_GLOBALMODUL,1);

                     hModule = (HMODULE)GlobalHandleToSel(hModule);
                     lpmod = (MODSTRUC FAR *)MAKELONG(0,hModule);

                     hSegm = GetModuleDgroup(hModule);
                     EnableDlgItem(hDlg,ID_LOCALMODUL,IsValidLocalHeap((HGLOBAL)hSegm));
                     EnableDlgItem(hDlg,ID_RESMODUL,lpmod->offsresname - lpmod->offsrctab);

                     SegmentDlg(hDlg,WM_COMMAND,ID_REFRESH,
                                MAKELONG(hModule,0));

                     EnableDlgItem(hDlg,ID_IMPORTS,fRelocs);

                     COPYRIGHTCHECK();
                     break;
                  case LBN_DBLCLK:
                     if (!CheckModul(hDlg,&hModule,&moduleentry))
                         CreateMessage(hDlg,errorstr1,0,MB_OK);
                     else
                         PostMessage(hDlg,WM_COMMAND,ID_XVIEWMODUL,0);
                     break;
                  case XLBN_RBUTTONDOWN:
                     TrackPopup(hDlg,BtnTab);
                     break;
                 }
              break;
         case ID_REFRESH:
              SetCursor(hCursorWait);
              ListBox_ResetContent(hWnd = GetDlgItem(hDlg,ID_LISTBOX1));
              moduleentry.dwSize = sizeof(moduleentry);
              y = ModuleFirst(&moduleentry);
              i = 0;
              while (y)
                 {
                  i ++;
                  lpmod = (MODSTRUC FAR *)MAKELONG(0,moduleentry.hModule);
                  if (lpmod->flags & 0x8000)
                      strcpy(szStr2,"Lib");
                  else
                      strcpy(szStr2,"Prg");
                  if (GetModuleName(moduleentry.hModule,str1,sizeof(str1)) > 8)
                      strcpy(&str1[8],"...");

                  wsprintf(str,
                           szModFStr,
                           moduleentry.hModule,
                           (LPSTR)str1,
                           (LPSTR)szStr2,
                           (LPSTR)moduleentry.szExePath);
                  x = ListBox_AddString(hWnd,str);
                  ListBox_SetItemData(hWnd,x,(LPARAM)(LPVOID)moduleentry.hModule);
                  y = ModuleNext(&moduleentry);
                  }
              ListBox_SetCurSel(hWnd,0);

              if (wSortPos == -1)
                 strcpy(str1,"unsortiert");
              else
              if (wSortPos == 0)
                 strcpy(str1,"Handle");
              else
              if (wSortPos == 5)
                 strcpy(str1,"Name");
              else
                 strcpy(str1,"Typ");
              wsprintf(str,
                       szModulAll,
                       i,
                       (LPSTR)str1
                      );
              SetDlgItemText(hDlg,ID_STATALL,str);
              COPYRIGHTCHECK();
              SetCursor(hCursor);
              break;
            }                 /* end switch wParam */
        rc = TRUE;            /* end case WM_COMMAND */
        break;
    default:
        break;
    }
 return rc;
}
