
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

BOOL CALLBACK StatusDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK WindowDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ClassDlg      (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK LocalDlg      (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GlobalDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ViewClassDlg  (HWND, UINT, WPARAM, LPARAM);

extern HFONT hFontAlt;
extern HWND hWndMon;

int xUserpos  = 100;
int yUserpos  = 100;

HWND hWndUser       = 0;
HWND hWndClass      = 0;
HWND hWndWin        = 0;
HWND hWndUserLocal  = 0;
HWND hWndUserGlobal = 0;

static GMEMFILTER gmf = {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,
                         TRUE,TRUE,FALSE,FALSE,FALSE,
                         0x00000000,0xFFFFFFFF,GLOBAL_ALL,0,"\0",0
                        };

static PSTR fstr[32] = {0};

static int itab[32] = {SM_CXSCREEN,
                       SM_CYSCREEN,
                       SM_CXFRAME,
                       SM_CYFRAME,
                       SM_CXVSCROLL,
                       SM_CYVSCROLL,
                       SM_CXHSCROLL,
                       SM_CYHSCROLL,
                       SM_CYCAPTION,
                       SM_CXBORDER,
                       SM_CYBORDER,
                       SM_CXDLGFRAME,
                       SM_CYDLGFRAME,
                       SM_CXHTHUMB,
                       SM_CYVTHUMB,
                       SM_CXICON,
                       SM_CYICON,
                       SM_CXCURSOR,
                       SM_CYCURSOR,
                       SM_CYMENU,
                       SM_CXFULLSCREEN,
                       SM_CYFULLSCREEN,
                       SM_CYKANJIWINDOW,
                       SM_CXMINTRACK,
                       SM_CYMINTRACK,
                       SM_CXMIN,
                       SM_CYMIN,
                       SM_CXSIZE,
                       SM_CYSIZE,
                       SM_MOUSEPRESENT,
                       SM_DEBUG,
                       SM_SWAPBUTTON
                      };

static STRLOADENTRY CODESEG strloadtab[] = {
                       &fstr[0],  IDS_SPSTR1,
                       &fstr[1],  IDS_SPSTR2,
                       &fstr[2],  IDS_SPSTR3,
                       &fstr[3],  IDS_SPSTR4,
                       &fstr[4],  IDS_SPSTR5,
                       &fstr[5],  IDS_SPSTR6,
                       &fstr[6],  IDS_SPSTR7,
                       &fstr[7],  IDS_SPSTR8,
                       &fstr[8],  IDS_SPSTR9,
                       &fstr[9],  IDS_SPSTR10,
                       &fstr[10], IDS_SPSTR11,
                       &fstr[11], IDS_SPSTR12,
                       &fstr[12], IDS_SPSTR13,
                       &fstr[13], IDS_SPSTR14,
                       &fstr[14], IDS_SPSTR15,
                       &fstr[15], IDS_SPSTR16,
                       &fstr[16], IDS_SPSTR17,
                       &fstr[17], IDS_SPSTR18,
                       &fstr[18], IDS_SPSTR19,
                       &fstr[19], IDS_SPSTR20,
                       &fstr[20], IDS_SPSTR21,
                       &fstr[21], IDS_SPSTR22,
                       &fstr[22], IDS_SPSTR23,
                       &fstr[23], IDS_SPSTR24,
                       &fstr[24], IDS_SPSTR25,
                       &fstr[25], IDS_SPSTR26,
                       &fstr[26], IDS_SPSTR27,
                       &fstr[27], IDS_SPSTR28,
                       &fstr[28], IDS_SPSTR29,
                       &fstr[29], IDS_SPSTR30,
                       &fstr[30], IDS_SPSTR31,
                       &fstr[31], IDS_SPSTR32,
                       (PSTR *)(0)};

////////////////////////////////////////////////////////
// WinCB - Window Callback                              
////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK WinCB(HWND hWnd,LONG lParam)
{
 HWND hLBWnd;
 HWND hParent;
 HTASK hTask;
 DWORD dwStyle;
 char str1[60];
 char str2[60];
 char str[140];
 int x;
 TASKENTRY taskentry;
 BOOL rc = TRUE;

  GetWindowText(hWnd,str1,sizeof(str1));
  GetClassName(hWnd,str2,sizeof(str2));
  hParent = GetParent(hWnd);
  if (HIWORD(lParam) == 1)
     {
      strcpy(taskentry.szModule,"<desktop>");
      hTask = 0;
     }
  else
     {
      hTask = GetWindowTask(hWnd);
      taskentry.dwSize = sizeof(TASKENTRY);
      TaskFindHandle(&taskentry,hTask);
     }
  dwStyle  = GetWindowStyle(hWnd);
  wsprintf(str,
           "%X\t%X\t%X\t%s\t%lX\t%s\t%s",
           hWnd,                      /* Window handle */
           hParent,                   /* dessen Parent/owner */
           hTask,                     /* zug. Task */
           (LPSTR)taskentry.szModule, /* Task text */
           dwStyle,                   /* window style */
           (LPSTR)str2,               /* class name  */
           (LPSTR)str1                /* dessen text */
           );
  hLBWnd = (HWND)LOWORD(lParam);
  x = (int)SendMessage(hLBWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
  SendMessage(hLBWnd,LB_SETITEMDATA,x,MAKELONG(hWnd,hTask));

 return rc;
}
////////////////////////////////////////////////////////
// CheckWindow                                          
////////////////////////////////////////////////////////
static HWND CheckWindow(HWND hDlg)
{
 WORD x;
 char szStr[80];
 HWND hWnd;
 int  tabpos[2];
 HTASK hTask;
 DWORD dw;
 int  nErr;

  x = (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
  if (x == LB_ERR)
      return 0;
  dw   = SendDlgItemMessage(hDlg,ID_LISTBOX1, LB_GETITEMDATA,x,0);
  hWnd = (HWND)LOWORD(dw);
  hTask = (HTASK)HIWORD(dw);
  if (!IsWindow(hWnd))
      nErr = IDS_ERRWND1;
  else
  if (hTask && (GetWindowTask(hWnd) != hTask))
      nErr = IDS_ERRWND1;
  else
      nErr = 0;

  if (nErr)
     {
      szStr[0] = '\t';
      nErr = LoadString(hInst,nErr,szStr+1,sizeof(szStr)-2);
      szStr[nErr+1] = '\t';
      szStr[nErr+2] = 0;
      hWnd = 0;
      tabpos[0] = 8;
      tabpos[1] = -8;
      SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,2,(LONG)(LPSTR)tabpos);
      SetDlgItemText(hDlg,ID_STATUS1,szStr);
     }

  return hWnd;
}
////////////////////////////////////////////////////////
// WindowDlg - Alle Windows auflisten                   
////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK AllWindowDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_SUBDLG3,ID_SUBDLG2,ID_SUBDLG1,0};
 int  x;
 HWND xWnd;
 HWND hWnd;
 DWORD dw,dw1;
 WNDCLASS wc;
 HMENU hMenu1;
 HMODULE hModule;
 HINSTANCE hInstance;
 char szStr[60];
 char szStr2[80];
 BOOL rc = FALSE;

 switch (message)
    {
    case WM_INITDIALOG:
      LoadTabs(IDUS_18,szStr);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      ShowWindow(hDlg,SW_NORMAL);
      rc = TRUE;
      break;
    case WM_DESTROY:
      hWndWin = 0;
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
              PostMessage(hDlg,WM_COMMAND,ID_LISTBOX1,MAKELONG(0,LBN_DBLCLK));
              break;
         case ID_SUBDLG1:  // WM_CLOSE nachricht an Window schicken
              if (hWnd = CheckWindow(hDlg))
                  SendMessage(hWnd,WM_CLOSE,0,0);
              break;
         case ID_SUBDLG2:  // Klasse des Fensters
              if (!(hWnd = CheckWindow(hDlg)))
                  break;
              hModule = (HMODULE)GetClassWord(hWnd,GCW_HMODULE);
              hInstance = GetInstanceFromTask(GetWindowTask(hWnd));
              GetClassName(hWnd,szStr,sizeof(szStr));
              if (!(x = GetClassInfo(hInstance,szStr,&wc)))
                   if (!(x = GetClassInfo(hModule,szStr,&wc)))
                       x = GetClassInfo(0,szStr,&wc);
              if (x)
                 {
                  wc.hInstance = (HINSTANCE)GetClassWord(hWnd,GCW_HMODULE);
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_VIEWCLS),
                                    hDlg,
                                    ViewClassDlg,
                                    (LPARAM)(LPVOID)&wc
                                   );
                 }
              else
                 {
                  wsprintf (szStr2,"bei\nGetClassInfo(%X,\"%s\")",
                            hInstance,
                            szStr);
                  CreateMessage(hDlg,szStr2,"Fehler",MB_OK);
                 }
              break;
         case ID_SUBDLG3:
              if (!(hWnd = CheckWindow(hDlg)))
                  break;
              CreateDialogParam(hInst,
                                MAKEINTRESOURCE(IDD_WINDOW),
                                hDlg,
                                WindowDlg,
                                (LPARAM)(LPVOID)hWnd
                               );
              break;
         case ID_REFRESH:
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);
              EnumWindows(WinCB,MAKELONG(hWnd,0));
              WinCB(GetDesktopWindow(),MAKELONG(hWnd,1));
              SendMessage(hWnd,LB_SETCURSEL,0,0);
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                {
                 case LBN_SELCHANGE:
                   xWnd = CheckWindow(hDlg);
                   EnableDlgItem(hDlg,ID_SUBDLG1,x);
                   EnableDlgItem(hDlg,ID_SUBDLG2,x);
                   EnableDlgItem(hDlg,ID_SUBDLG3,x);
                   if (xWnd)
                      {
                       LoadTabs(IDUS_56,szStr);
                       SendDlgItemMessage(hDlg,
                                          ID_STATUS1,
                                          ST_SETTABSTOPS,
                                          *(LPINT)szStr,
                                          (LONG)(LPINT)(szStr+2));
                        dw = GetWindowLong(xWnd,GWL_WNDPROC);
                        dw1 = GetClassLong(xWnd,GCL_WNDPROC);
                        hMenu1 = GetMenu(xWnd);
                        wsprintf(szStr,
                                 "WndProc\t%04X:%04X\tClassProc\t%04X:%04X\tMenu\t%04X\t",
                                 HIWORD(dw),
                                 LOWORD(dw),
                                 HIWORD(dw1),
                                 LOWORD(dw1),
                                 hMenu1
                                );
                        SetDlgItemText(hDlg,ID_STATUS1,szStr);
                      }
                   break;
                 case LBN_DBLCLK:
                   SendMessage(hDlg,WM_COMMAND,ID_SUBDLG3,0);
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
////////////////////////////////////////////////////////
// UserDlg - User Dialog                                
////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK UserDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 HMODULE hModule;
 HGLOBAL hSegm;
 int i,j;
 int tabpos[4];
 char str[80];
 BOOL rc = FALSE;

 switch (message)
    {
    case WM_INITDIALOG:
      if (!fstr[0])
          LoadStringTab(hInst,strloadtab);
      SendDlgItemMessage(hDlg,ID_SYSPARMLB,LB_RESETCONTENT,0,0);
      tabpos[0] = 90;
      tabpos[1] = 110 | 0x8000;
      SendDlgItemMessage(hDlg,ID_SYSPARMLB,LB_SETTABSTOPS,2,(LPARAM)(LPVOID)&tabpos);
      for (i=0;i<32;i++)
         {
          j = GetSystemMetrics(itab[i]);
          wsprintf(str,"%s\t%u\t ",(LPSTR)fstr[i],j);
          SendDlgItemMessage(hDlg,ID_SYSPARMLB,LB_ADDSTRING,0,(LONG)(LPSTR)str);
         }
      SetWindowPos(hDlg,0,xUserpos,yUserpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_NORMAL);
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_DESTROY:
      hWndUser = 0;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xUserpos,&yUserpos);
      break;
    case WM_COMMAND:
      switch (wParam)
        {
         case ID_WINDOWS:
              if (hWndWin)
                  ShowWindow(hWndWin,SW_NORMAL);
              else
                  hWndWin = CreateDialog(hInst,
                                         MAKEINTRESOURCE(IDD_ALLWIN),
                                         hDlg,
                                         AllWindowDlg);
              break;
         case ID_CLASS:
              if (hWndClass)
                  ShowWindow(hWndClass,SW_NORMAL);
              else
                  hWndClass = CreateDialog(hInst,
                                           MAKEINTRESOURCE(IDD_CLASS),
                                           hDlg,
                                           ClassDlg);
              break;
         case ID_USERLOCAL:
              if (IsWindow(hWndUserLocal))
                  ShowWindow(hWndUserLocal,SW_NORMAL);
              else
                 {
                  hModule = GetModuleHandle("USER");
                  hSegm   = GetModuleDgroup(hModule);
                  hWndUserLocal = CreateDialogParam(hInst,
                                                    MAKEINTRESOURCE(IDD_LOCAL),
                                                    hDlg,
                                                    LocalDlg,
                                                    MAKELONG(hSegm,hModule)
                                                   );
                 }
              break;
         case ID_USERGLOBAL:
              if (IsWindow(hWndUserGlobal))
                  ShowWindow(hWndUserGlobal,SW_NORMAL);
              else
                 {
                  gmf.hOwner = GetModuleHandle("USER");
                  hWndUserGlobal = CreateDialogParam(hInst,
                                           MAKEINTRESOURCE(IDD_GLOBAL),
                                           hDlg,
                                           GlobalDlg,
                                           (LPARAM)(LPVOID)&gmf
                                          );
                 }
              break;
         case ID_WINMON:
              if (hWndMon)
                  ShowWindow(hWndMon,SW_NORMAL);
              else
                  hWndMon = CreateDialog(hInst,
                                         MAKEINTRESOURCE(IDD_WINSTATUS),
                                         0,
                                         StatusDlg);
              break;
         case ID_DESTROY:
              if ((HWND)LOWORD(lParam) == hWndUserGlobal)
                  hWndUserGlobal = 0;
              else
              if ((HWND)LOWORD(lParam) == hWndUserLocal)
                  hWndUserLocal  = 0;
              break;
         case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
        }
      rc = TRUE;
      break;
    }
 return rc;
}
