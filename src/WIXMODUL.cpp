

#include "stddef.h"
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

extern char szNo[];
extern char szYes[];
extern BYTE fShow;
extern HFONT hFontAlt;

GMEMFILTER gmf = {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,
                         TRUE,FALSE,FALSE,FALSE,FALSE,
                         0x00000000,0xFFFFFFFF,GLOBAL_ALL,0,"\0",0
                        };

BOOL CALLBACK ImportDlg      (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK ResDlg         (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK RefModuleDlg   (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK ImportedFromDlg(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK GlobalDlg      (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK UnassembleDlg  (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK LocalDlg       (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK SegmentDlg     (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK ViewDlg        (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK ModulEntriesDlg(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK XModulDlg      (HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK ExportDlg      (HWND,UINT,WPARAM,LPARAM);

HMODULE WINAPI CheckModul(HWND,MODULEENTRY *,PINT,WORD);

static int idtab1[] = {
                       ID_SUBDLG1,ID_SUBDLG2,ID_SUBDLG3,
                       ID_SUBDLG5,ID_SUBDLG6,ID_SUBDLG7,
                       ID_SUBDLG8,ID_SUBDLG9,ID_SUBDLG10,
                       0};

static KEYENTRY keytab3[] = {
                  "NONE",0X00,
                  "SHARED",0X01,
                  "NONSHARED",0X02,
                  "???",0x03,
                  (PSTR)0};

static KEYENTRY keytab4[] = {
                  "0",0X00,
                  "NOTWINDOWCOMPAT",0X01,
                  "WINDOWCOMPAT",0X02,
                  "WINDOWAPI",0x03,
                  (PSTR)0};

static KEYENTRY keytab2[] = {
                  "Library",0x80,
                  "Program",0x00,
                  (PSTR)0};

static KEYENTRY keytab1[] = {
                  "Windows",2,
                  "OS/2",1,
                  (PSTR)0};

static KEYENTRY keytab5[] = {
                  szNo,0,
                  szYes,8,
                  (PSTR)0};

static DDENTRY modstrtab[] = {
                  0,offsetof(MODSTRUC,flags),      "%04X",DDT_WORD,
                  0,offsetof(MODSTRUC,flags)+1,    (PSTR)&keytab2,0x8000 | DDT_KBITS,
                  0,offsetof(MODSTRUC,flags)+1,    (PSTR)&keytab5,0x0800 | DDT_KBITS,
                  0,offsetof(MODSTRUC,flags)+1,    (PSTR)&keytab4,0x0300 | DDT_KBITS,
                  0,offsetof(MODSTRUC,flags),      (PSTR)&keytab3,0x0300 | DDT_KBITS,
                  0,offsetof(MODSTRUC,initcsip),   "%X:%X",DDT_DWORD,
                  0,offsetof(MODSTRUC,initsssp),   "%X:%X",DDT_DWORD,
                  0,offsetof(MODSTRUC,heapsize),   "%X",DDT_WORD,
                  0,offsetof(MODSTRUC,stacksize),  "%X",DDT_WORD,
                  0,offsetof(MODSTRUC,segs),       "%d",DDT_WORD,
                  0,offsetof(MODSTRUC,autods),     "%d",DDT_WORD,
                  0,offsetof(MODSTRUC,offssegtab), "%X",DDT_WORD,
                  0,offsetof(MODSTRUC,offsrctab),  "%X",DDT_WORD,
                  0,offsetof(MODSTRUC,offsresname),"%X",DDT_WORD,
                  0,offsetof(MODSTRUC,offsmodref), "%X",DDT_WORD,
                  0,offsetof(MODSTRUC,offsimpname),"%X",DDT_WORD,
                  0,offsetof(MODSTRUC,offsentries),"%X",DDT_WORD,
                  0,offsetof(MODSTRUC,moventrys),  "%d",DDT_WORD,
                  0,offsetof(MODSTRUC,pfileinfo),  "%X",DDT_WORD,
                  0,offsetof(MODSTRUC,pfileinfo),  "%u.%u.%02u, %u:%02u",DDT_USER,
                  0,offsetof(MODSTRUC,swaparea),   "%X",DDT_WORD,
                  0,offsetof(MODSTRUC,os),         (PSTR)&keytab1,DDT_KBYTE,
                  0,offsetof(MODSTRUC,version),    "%u.%u",DDT_USER,
                  0,offsetof(MODSTRUC,nextmod),    "%X",DDT_WORD,
                  (PSTR)0};

static STRLOADENTRY CODESEG strloadtab[] = {
                       &modstrtab[00].name, IDS_XMODUL00,
                       &modstrtab[01].name, IDS_XMODUL01,
                       &modstrtab[02].name, IDS_XMODUL22,
                       &modstrtab[03].name, IDS_XMODUL02,
                       &modstrtab[04].name, IDS_XMODUL03,
                       &modstrtab[05].name, IDS_XMODUL04,
                       &modstrtab[06].name, IDS_XMODUL05,
                       &modstrtab[07].name, IDS_XMODUL06,
                       &modstrtab[ 8].name, IDS_XMODUL07,
                       &modstrtab[ 9].name, IDS_XMODUL08,
                       &modstrtab[10].name, IDS_XMODUL09,
                       &modstrtab[11].name, IDS_XMODUL10,
                       &modstrtab[12].name, IDS_XMODUL11,
                       &modstrtab[13].name, IDS_XMODUL12,
                       &modstrtab[14].name, IDS_XMODUL13,
                       &modstrtab[15].name, IDS_XMODUL14,
                       &modstrtab[16].name, IDS_XMODUL15,
                       &modstrtab[17].name, IDS_XMODUL21,
                       &modstrtab[18].name, IDS_XMODUL16,
                       &modstrtab[19].name, IDS_XMODUL23,
                       &modstrtab[20].name, IDS_XMODUL17,
                       &modstrtab[21].name, IDS_XMODUL18,
                       &modstrtab[22].name, IDS_XMODUL19,
                       &modstrtab[23].name, IDS_XMODUL20,
                       (PSTR *)(0)};
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ RefModuleDlg - Nur die referenzierten Module auflisten บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK RefModuleDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_SUBDLG11 | 0x8000,IDS_VIEW,
                        0};
 int  x,y;
 char szStr[80];
 REFMODULEENTRY refmoduleentry;
 HMODULE hModule;
 HWND  hWnd;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      hWnd = GetDlgItem(hDlg,ID_SUBDLG4);
      hModule = (HMODULE)LOWORD(lParam);
      SendMessage(hWnd,XLB_SETWINDOWDATA,0,(LPARAM)(LPVOID)hModule);
      LoadTabs(IDUS_14,szStr);
      SendMessage(hWnd,LB_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
      SendMessage(hWnd,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                  (LPARAM)(LPVOID)hFontAlt);
//    SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
//    GetModuleName(hModule,szStr1,sizeof(szStr1));
//    wsprintf(szStr,"Modulreferenzen Modul: %s",(LPSTR)szStr1);
//    SetWindowText(hDlg,szStr);
//    ShowWindow(hDlg,SW_NORMAL);
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case IDOK:
              break;
         case ID_REFRESH:
              hWnd = GetDlgItem(hDlg,ID_SUBDLG4);
              hModule = (HMODULE)SendMessage(hWnd,XLB_GETWINDOWDATA,0,0);
              ListBox_ResetContent(hWnd);
              refmoduleentry.dwSize = sizeof(refmoduleentry);
              refmoduleentry.hModule = hModule;
              y = RefModuleFirst(&refmoduleentry);
              while (y)
                 {
                  wsprintf(szStr,"%s",(LPSTR)refmoduleentry.szRefModule);
                  x = ListBox_AddString(hWnd,szStr);
                  ListBox_SetItemData(hWnd,x,(LPARAM)(LPVOID)refmoduleentry.hRefModule);
                  y = RefModuleNext(&refmoduleentry);
                 }
              break;
         case ID_SUBDLG11:
              hWnd = GetDlgItem(hDlg,ID_SUBDLG4);
              x = ListBox_GetCurSel(hWnd);
              if (x == LB_ERR)
                  break;
              hModule = (HMODULE)ListBox_GetItemData(hWnd,x);
              CreateDialogParam(hInst,MAKEINTRESOURCE(IDD_XMODULE),hDlg,
                                XModulDlg,(LPARAM)(LPVOID)hModule);
              break;
         case ID_SUBDLG4:
              hWnd = (HWND)LOWORD(lParam);
              switch (HIWORD(lParam))
                {
                 case LBN_DBLCLK:
                    PostMessage(hDlg,WM_COMMAND,ID_SUBDLG11,0);
                    break;
                 case XLBN_RBUTTONDOWN:
	 	            x = ListBox_GetCurSel(hWnd);
                    if (x != LB_ERR)
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
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ UserForm - spezielles Format aufbereiten             บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL PASCAL UserForm(DDENTRY * pddentry,PSTR pStr,LPSTR lpStr)
{
 LPINT lpInt;
 UINT  wDate;
 UINT  wTime;

 switch (pddentry->addr)
   {
    case offsetof(MODSTRUC,pfileinfo):
      lpInt = (LPINT)MAKELONG(pddentry->addr,HIWORD(lpStr));
      lpInt = (LPINT)MAKELONG(*lpInt,HIWORD(lpInt));
      wDate = *(lpInt+2);
      wTime = *(lpInt+3);
      wsprintf(pStr,
               pddentry->format,
               (wDate & 0x1F),
               (wDate >> 5) & 0xF,
               (wDate >> 9) + 80,
               (wTime >> 11),
               (wTime >> 5) & 0x3F
              );
      break;
    case offsetof(MODSTRUC,version):
      wsprintf(pStr,
               pddentry->format,
               (WORD)*(LPBYTE)MAKELONG(pddentry->addr+1,HIWORD(lpStr)),
               (WORD)*(LPBYTE)MAKELONG(pddentry->addr+0,HIWORD(lpStr))
              );
      break;
    default:
      *pStr = 0;
      break;
    }
 return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ViewModul - 1 Modul anschauen                        บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED WINAPI XModulDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int iXPos,iYPos = 0;
 HMODULE hModule;
 HGLOBAL hSegm;
 HWND   hWnd;
 DDENTRY * pddentry;
 MODSTRUC FAR * lpmod;
 MODULEENTRY moduleentry;
 HEXDUMPPARM hdp;
 LPSTR lpstr;
 char str[128];
 char str1[40];
 char str2[20];
 int  tabpos[2];
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      SetWindowLong(hDlg,DLGWINDOWEXTRA,lParam);
      if (!modstrtab[0].name)
          LoadStringTab(hInst,strloadtab);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,0,
                         (LPARAM)(LPVOID)hFontAlt);
      RefModuleDlg(hDlg,WM_INITDIALOG,0,lParam);
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      if (fShow)
         {
          if (iXPos)
              SetWindowPos(hDlg,0,iXPos,iYPos,0,0,SWP_NOSIZE | SWP_NOZORDER);
          ShowWindow(hDlg,SW_NORMAL);
         }
      else
         {
          CreateMessage(GetParent(hDlg),errorstr1,0,MB_OK);
          PostMessage(hDlg,WM_CLOSE,0,0);
         }
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&iXPos,&iYPos);
      break;
    case WM_COMMAND:
      switch (wParam)
        {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_SUBDLG1: // disassembler
              if (!(hModule = CheckModul(hDlg,&moduleentry,idtab1,1)))
                  break;
              lpmod = (MODSTRUC FAR *)MAKELONG(0,hModule);
              if (lpmod->initcsip)
                 {
                  hSegm = GetSegmentHandle(hModule,HIWORD(lpmod->initcsip));
                  lpstr = (LPSTR)MAKELONG(LOWORD(lpmod->initcsip),hSegm);
                  if (IsValidAddress(lpstr))
                      CreateDialogParam(hInst,
                                        MAKEINTRESOURCE(IDD_UNASSEM),
                                        hDlg,
                                        UnassembleDlg,
                                        (LPARAM)lpstr
                                       );
                 }
              break;
         case ID_SUBDLG3: //exporte
              if (hModule = CheckModul(hDlg,&moduleentry,idtab1,1))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_EXPORT),
                                    hDlg,
                                    ExportDlg,
                                    (LPARAM)(LPVOID)hModule);
              break;
         case ID_SUBDLG2: // importe
              if (hModule = CheckModul(hDlg,&moduleentry,idtab1,1))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_IMPORT),
                                    hDlg,
                                    ImportDlg,
                                    (LPARAM)(LPVOID)hModule);
              break;
         case ID_SUBDLG4: // referenzierte Module
         case ID_SUBDLG11:
              RefModuleDlg(hDlg,message,wParam,lParam);
/*
              if (hModule = CheckModul(hDlg,&moduleentry,idtab1,1))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_REFMOD),
                                    hDlg,
                                    RefModuleDlg,
                                    (LPARAM)hModule
                                   );
*/
              break;
         case ID_SUBDLG5: // resourcen
              if (hModule = CheckModul(hDlg,&moduleentry,idtab1,1))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_RESOURCE),
                                    hDlg,
                                    ResDlg,
                                    (LPARAM)(LPVOID)hModule);
              break;
         case ID_SUBDLG6: // importiert von
              if (hModule = CheckModul(hDlg,&moduleentry,idtab1,1))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_IMPORTEDFROM),
                                    hDlg,
                                    ImportedFromDlg,
                                    (LPARAM)MAKELONG(0,hModule)
                                   );
              break;
         case ID_SUBDLG7: // segmente
              if (hModule = CheckModul(hDlg,&moduleentry,idtab1,1))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_SEGMENT),
                                    hDlg,
                                    SegmentDlg,
                                    (LPARAM)MAKELONG(hModule,1)
                                   );
              break;
/*
         case ID_SUBDLG7: // entries
              if (hModule = CheckModul(hDlg,&moduleentry,idtab1,1))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_ENTRY),
                                    hDlg,
                                    ModulEntriesDlg,
                                    (LPARAM)MAKELONG(hModule,0)
                                   );
              break;
*/
         case ID_SUBDLG8: // local heap
              if (hModule = CheckModul(hDlg,&moduleentry,idtab1,1))
                  if (hSegm = GetModuleDgroup(hModule))
                      CreateDialogParam(hInst,
                                        MAKEINTRESOURCE(IDD_LOCAL),
                                        hDlg,
                                        LocalDlg,
                                        (LPARAM)MAKELONG(hSegm,hModule)
                                       );
              break;
         case ID_SUBDLG9: // anzeigen
              if (hModule = CheckModul(hDlg,&moduleentry,idtab1,1))
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
                  wsprintf(str,"Modul %s",(LPSTR)str1);
                  SetWindowText(hWnd,str);
                 }
              break;
         case ID_SUBDLG10: // Global heap
              if (hModule = CheckModul(hDlg,&moduleentry,idtab1,1))
                 {
//                  GetModuleName(hModule,gmf.szOwner,sizeof(gmf.szOwner));
                  gmf.hOwner = hModule;
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_GLOBAL),
                                    hDlg,
                                    GlobalDlg,
                                    (LPARAM)(LPVOID)&gmf
                                   );
                 }
              break;
         case ID_REFRESH:
              fShow = FALSE;
              if (!(hModule = CheckModul(hDlg,&moduleentry,idtab1,0)))
                  break;
              tabpos[0] = 108;
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              ListBox_SetTabStops(hWnd,1,&tabpos);
              GetWindowText(hDlg,str1,sizeof(str1));
              GetModuleName(hModule,str2,sizeof(str2));
              wsprintf(str,str1,(LPSTR)str2);
              SetWindowText(hDlg,str);
              lpmod = (MODSTRUC FAR *)MAKELONG(0,hModule);
              lpstr = (LPSTR)lpmod;
              pddentry = modstrtab;
              ListBox_ResetContent(hWnd);
              while (pddentry->name)
                 {
                  if (pddentry->typ == DDT_USER)
                      UserForm(pddentry,str1,lpstr);
                  else
                      xsprintf(pddentry,str1,lpstr);

                  wsprintf(str,"%s\t%s",
                           (LPSTR)pddentry->name,
                           (LPSTR)str1
                          );
                  ListBox_AddString(hWnd,str);
                  pddentry++;
                 }
              hSegm = GetSegmentHandle(hModule,HIWORD(lpmod->initcsip));
              lpstr = (LPSTR)MAKELONG(LOWORD(lpmod->initcsip),hSegm);
              EnableDlgItem(hDlg,ID_SUBDLG1,IsValidAddress(lpstr));
              EnableDlgItem(hDlg,
                            ID_SUBDLG8,
                            IsValidLocalHeap(GetModuleDgroup(hModule))
                           );
              EnableDlgItem(hDlg,ID_SUBDLG7,lpmod->segs);
              RefModuleDlg(hDlg,WM_COMMAND,ID_REFRESH,0);
              fShow = TRUE;
              break;
        }
      rc = TRUE;
      break;
   }
 return rc;
}
