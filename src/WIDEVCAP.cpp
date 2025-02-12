
/* device caps */

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
#include "print.h"

extern BYTE fShow;

typedef int (WINAPI *LPEXTDEVICEMODE)(HWND,HANDLE,LPDEVMODE,
                                      LPSTR,LPSTR,LPDEVMODE,LPSTR,WORD);

static PSTR fstr[29] = {0};

static KEYENTRY BrStyle[] = {
             "BS_DIBPATTERN",BS_DIBPATTERN,
             "BS_HATCHED",BS_HATCHED,
             "BS_HOLLOW",BS_HOLLOW,
             "BS_PATTERN",BS_PATTERN,
             "BS_NULL",BS_NULL,
             "BS_SOLID",BS_SOLID,
             (PSTR)0};

static KEYENTRY PenStyle[] = {
             "PS_SOLID",PS_SOLID,
             "PS_DASH",PS_DASH,
             "PS_DOT",PS_DOT,
             "PS_DASHDOT",PS_DASHDOT,
             "PS_DASHDOTDOT",PS_DASHDOTDOT,
             "PS_NULL",PS_NULL,
             "PS_INSIDEFRAME",PS_INSIDEFRAME,
             (PSTR)0};

static int itab[] = {
             DRIVERVERSION,
             TECHNOLOGY   ,
             HORZSIZE     ,
             VERTSIZE     ,
             HORZRES      ,
             VERTRES      ,
             BITSPIXEL    ,
             PLANES       ,
             NUMBRUSHES   ,
             NUMPENS      ,
             NUMMARKERS   ,
             NUMFONTS     ,
             NUMCOLORS    ,
             PDEVICESIZE  ,
             CURVECAPS    ,
             LINECAPS     ,
             POLYGONALCAPS,
             TEXTCAPS     ,
             CLIPCAPS     ,
             RASTERCAPS   ,
             ASPECTX      ,
             ASPECTY      ,
             ASPECTXY     ,
             LOGPIXELSX   ,
             LOGPIXELSY   ,
             SIZEPALETTE  ,
             NUMRESERVED  ,
             COLORRES     ,
             0xFFFF
                      };

static STRLOADENTRY CODESEG strloadtab[] = {
                       &fstr[0],  IDS_DRIVERVERSION,
                       &fstr[1],  IDS_TECHNOLOGY   ,
                       &fstr[2],  IDS_HORZSIZE     ,
                       &fstr[3],  IDS_VERTSIZE     ,
                       &fstr[4],  IDS_HORZRES      ,
                       &fstr[5],  IDS_VERTRES      ,
                       &fstr[6],  IDS_BITSPIXEL    ,
                       &fstr[7],  IDS_PLANES       ,
                       &fstr[8],  IDS_NUMBRUSHES   ,
                       &fstr[9],  IDS_NUMPENS      ,
                       &fstr[10], IDS_NUMMARKERS   ,
                       &fstr[11], IDS_NUMFONTS     ,
                       &fstr[12], IDS_NUMCOLORS    ,
                       &fstr[13], IDS_PDEVICESIZE  ,
                       &fstr[14], IDS_CURVECAPS    ,
                       &fstr[15], IDS_LINECAPS     ,
                       &fstr[16], IDS_POLYGONALCAPS,
                       &fstr[17], IDS_TEXTCAPS     ,
                       &fstr[18], IDS_CLIPCAPS     ,
                       &fstr[19], IDS_RASTERCAPS   ,
                       &fstr[20], IDS_ASPECTX      ,
                       &fstr[21], IDS_ASPECTY      ,
                       &fstr[22], IDS_ASPECTXY     ,
                       &fstr[23], IDS_LOGPIXELSX   ,
                       &fstr[24], IDS_LOGPIXELSY   ,
                       &fstr[25], IDS_SIZEPALETTE  ,
                       &fstr[26], IDS_NUMRESERVED  ,
                       &fstr[27], IDS_COLORRES     ,
                       (PSTR *)(0)};

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ObjectsProc                                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL __export CALLBACK EnumObjProc(LPVOID lpvoid,LPARAM lParam)
{
 HWND hWnd;
 WORD wType;
 char szStr[80];
 LPLOGBRUSH lb;
 LPLOGPEN   lp;

 hWnd = (HWND)LOWORD(lParam);
 wType = HIWORD(lParam);
 if (wType == 0) {
     lb = (LPLOGBRUSH)lpvoid;
     wsprintf(szStr,
              "%s\t%06lX\t%d",
              (LPSTR)SearchKeyTable((char *)BrStyle,lb->lbStyle),
              lb->lbColor,
              lb->lbHatch
             );
 } else {
     lp = (LPLOGPEN)lpvoid;
     wsprintf(szStr,
              "%s\t%06lX\t%d",
              (LPSTR)SearchKeyTable((char *)PenStyle,lp->lopnStyle),
              lp->lopnColor,
              lp->lopnWidth.x
             );
 }
 SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)szStr);

 return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ObjectsDlg                                           บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK ObjectsDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 HGLOBAL hDev;
 HWND hWnd;
 char szStr[80];
 HDC hdc;
 int i,j;
 LPSTR lpDev,lpDevice,lpDriver,lpPort;
 BOOL rc = FALSE;

 switch (message) {
    case WM_INITDIALOG:
      SetWindowLong(hDlg,DLGWINDOWEXTRA,lParam);
      hDev = (HGLOBAL)LOWORD(lParam);
      if (!(lpDev = (LPSTR)GlobalLock(hDev)))
          break;
      lpDevice = lpDev;
      lpDriver = lpDev + _fstrlen(lpDev) + 1;
      lpPort   = lpDriver + _fstrlen(lpDriver) + 1;
      if (hdc = CreateDC(lpDriver,lpDevice,lpPort,(LPVOID)0))
         {
          hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
          SendMessage(hWnd,LB_RESETCONTENT,0,0);
          LoadTabs(IDUS_55,szStr);
          SendMessage(hWnd,LB_SETTABSTOPS,*(LPINT)szStr,(LPARAM)(LPINT)(szStr+2));
          EnumObjects(hdc,OBJ_BRUSH,EnumObjProc,MAKELONG(hWnd,0));
          i = (int)SendMessage(hWnd,LB_GETCOUNT,0,0);
          hWnd = GetDlgItem(hDlg,ID_SUBDLG1);
          SendMessage(hWnd,LB_RESETCONTENT,0,0);
          LoadTabs(IDUS_55,szStr);
          SendMessage(hWnd,LB_SETTABSTOPS,*(LPINT)szStr,(LPARAM)(LPINT)(szStr+2));
          EnumObjects(hdc,OBJ_PEN,EnumObjProc,MAKELONG(hWnd,1));
          j = (int)SendMessage(hWnd,LB_GETCOUNT,0,0);
          DeleteDC(hdc);
         }
      GlobalUnlock(hDev);

      hWnd = GetDlgItem(hDlg,ID_STATUS1);
      LoadTabs(IDUS_54,szStr);
      SendMessage(hWnd,ST_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
      wsprintf(szStr,"Brushes\t%u\tPens\t%u\t",i,j);
      SetDlgItemText(hDlg,ID_STATUS1,szStr);

      ShowWindow(hDlg,SW_NORMAL);
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_COMMAND:
      switch (wParam)
        {
         case IDOK:
         case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
        }
      rc = TRUE;
      break;
 }
 return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Command1Dlg                                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK DevCapDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 WORD i,j;
 char str[80];
 BOOL rc = FALSE;
 LPEXTDEVICEMODE lpExtDeviceMode;
 int iSize;
 HGLOBAL hDev;
 HINSTANCE hLib;
 HGLOBAL hMem;
 LPSTR lpDev,lpDevice,lpDriver,lpPort;
 LPDEVMODE lpDevMode;
 HWND hWnd;
 HDC hdc;
 int tabpos[2];

 switch (message)
    {
    case WM_INITDIALOG:
      SetWindowLong(hDlg,DLGWINDOWEXTRA,lParam);
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      if (fShow)
          ShowWindow(hDlg,SW_NORMAL);
      else
          PostMessage(hDlg,WM_CLOSE,0,0);
      rc = TRUE;
      break;
    case WM_CLOSE:
      if (hDev = (HGLOBAL)GetWindowWord(hDlg,DLGWINDOWEXTRA))
          GlobalFree(hDev);
      DestroyWindow(hDlg);
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
              if (!(hDev = (HGLOBAL)GetWindowWord(hDlg,DLGWINDOWEXTRA)))
                  break;
              lpDev = (LPSTR)GlobalLock(hDev);
              lpDriver = lpDev + _fstrlen(lpDev) + 1;
              lpPort   = lpDriver + _fstrlen(lpDriver) + 1;
              wsprintf(str,"%s.DRV",(LPSTR)lpDriver);
              if ((int)(hLib = LoadLibrary(str)) > 0x20)
                 {
                  if (lpExtDeviceMode = (LPEXTDEVICEMODE)GetProcAddress(hLib,"EXTDEVICEMODE"))
                     {
                      iSize = lpExtDeviceMode((HWND)hDlg,
                                      (HANDLE)hLib,
                                      (LPDEVMODE)0,
                                      (LPSTR)lpDev,
                                      (LPSTR)lpPort,
                                      (LPDEVMODE)0,
                                      (LPSTR)0,
                                      (WORD)0);
                      if (hMem = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,iSize))
                         {
                          lpDevMode = (LPDEVMODE)GlobalLock(hMem);
                          lpExtDeviceMode((HWND)hDlg,
                                          (HANDLE)hLib,
                                          (LPDEVMODE)lpDevMode,
                                          (LPSTR)lpDev,
                                          (LPSTR)lpPort,
                                          (LPDEVMODE)0,
                                          (LPSTR)0,
                                          (WORD)DM_OUT_BUFFER);
                          lpExtDeviceMode((HWND)hDlg,
                                          (HANDLE)hLib,
                                          (LPDEVMODE)lpDevMode,
                                          (LPSTR)lpDev,
                                          (LPSTR)lpPort,
                                          (LPDEVMODE)lpDevMode,
                                          (LPSTR)0,
                                          (WORD)DM_IN_PROMPT);
                          GlobalFree(hMem);
                         }
                     }
                  else
                      CreateMessage(hDlg,"EXTDEVICEMODE() nicht vorhanden",0,MB_OK);
                  FreeLibrary(hLib);
                 }
              else
                  CreateMessage(hDlg,"Error loading driver",0,MB_OK);
              GlobalUnlock(hDev);
              break;
         case ID_SUBDLG2:
              if (!(hDev = (HGLOBAL)GetWindowWord(hDlg,DLGWINDOWEXTRA)))
                  break;
              CreateDialogParam(hInst,
                                MAKEINTRESOURCE(IDD_OBJECTS),
                                hDlg,
                                ObjectsDlg,
                                (LPARAM)(LPVOID)hDev
                               );
              break;
         case ID_REFRESH:
              fShow = FALSE;
              if (!(hDev = (HGLOBAL)GetWindowWord(hDlg,DLGWINDOWEXTRA)))
                  break;
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              lpDev = (LPSTR)GlobalLock(hDev);
              lpDevice = lpDev;
              lpDriver = lpDevice + _fstrlen(lpDevice) + 1;
              lpPort   = lpDriver + _fstrlen(lpDriver) + 1;
              wsprintf(str,"Device %s",(LPSTR)lpDevice);
              SetWindowText(hDlg,str);
              if (!fstr[0])
                  LoadStringTab(hInst,strloadtab);
              if (!(hdc = CreateDC(lpDriver,lpDevice,lpPort,(LPVOID)0)))
                 {
                  CreateMessage(GetParent(hDlg),
                                "DC kann nicht erzeugt werden",
                                0,
                                MB_OK);
                  break;
                 }
              GlobalUnlock(hDev);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);
              tabpos[0] = 90;
              tabpos[1] = 110 | 0x8000;
              SendMessage(hWnd,LB_SETTABSTOPS,2,(LPARAM)(LPVOID)&tabpos);
              i = 0;
              while (1)
                 {
                  if (itab[i] == 0xFFFF)
                     break;
                  j = GetDeviceCaps(hdc,itab[i]);
                  wsprintf(str,"%s\t%u",(LPSTR)fstr[i],j);
                  SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                  i++;
                 }
              DeleteDC(hdc);
              fShow = TRUE;
              break;
        }
      rc = TRUE;
      break;
    }
 return rc;
}
