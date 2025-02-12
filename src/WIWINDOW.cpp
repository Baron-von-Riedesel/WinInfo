
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

// #define strlen lstrlen
// #define strcpy lstrcpy
// #define strcmp lstrcmp

extern FARPROC lpprocWindow;
extern HFONT   hFontAlt;

// int  FAR PASCAL WndChildCB (HWND, DWORD);
// int  FAR PASCAL WndPropCB (HWND, LPSTR, HANDLE);
BOOL CALLBACK UnassembleDlg (HWND, UINT, WPARAM, LPARAM);

static HWND tDlg;

static DWORD wmsg[] = {
               WS_MINIMIZE,
               WS_VISIBLE,
               WS_DISABLED,
               WS_CLIPSIBLINGS,
               WS_CLIPCHILDREN,
               WS_MAXIMIZE,
               WS_CAPTION,
               WS_BORDER,
               WS_DLGFRAME,
               WS_VSCROLL,
               WS_HSCROLL,
               WS_SYSMENU,
               WS_THICKFRAME,
               0
              };

static char * wmsgstr[] = {
                  "WS_MINIMIZE",
                  "WS_VISIBLE",
                  "WS_DISABLED",
                  "WS_CLIPSIBLINGS",
                  "WS_CLIPCHILDREN",
                  "WS_MAXIMIZE",
                  "WS_CAPTION",
                  "WS_BORDER",
                  "WS_DLGFRAME",
                  "WS_VSCROLL",
                  "WS_HSCROLL",
                  "WS_SYSMENU",
                  "WS_THICKFRAME",
                 };

static DWORD wbmsg[] = {
               WS_MINIMIZEBOX,
               WS_MAXIMIZEBOX,
               0
              };

static char * wbmsgstr[] = {
                  "WS_MINIMIZEBOX",
                  "WS_MAXIMIZEBOX"
                 };

static DWORD lbstyle[] = {
                  WS_GROUP,
                  WS_TABSTOP,
                  LBS_NOTIFY,             /* 0x0001L */
                  LBS_SORT,               /* 0x0002L */
                  LBS_NOREDRAW,           /* 0x0004L */
                  LBS_MULTIPLESEL,        /* 0x0008L */
                  LBS_OWNERDRAWFIXED,     /* 0x0010L */
                  LBS_OWNERDRAWVARIABLE,  /* 0x0020L */
                  LBS_HASSTRINGS,         /* 0x0040L */
                  LBS_USETABSTOPS,        /* 0x0080L */
                  LBS_NOINTEGRALHEIGHT,   /* 0x0100L */
                  LBS_MULTICOLUMN,        /* 0x0200L */
                  LBS_WANTKEYBOARDINPUT,  /* 0x0400L */
                  LBS_EXTENDEDSEL,        /* 0x0800L */
                  0
                 };
static char * lbsstr[] = {
                  "WS_GROUP",
                  "WS_TABSTOP",
                  "LBS_NOTIFY",
                  "LBS_SORT",
                  "LBS_NOREDRAW",
                  "LBS_MULTIPLESEL",
                  "LBS_OWNERDRAWFIXED",
                  "LBS_OWNERDRAWVARIABLE",
                  "LBS_HASSTRINGS",
                  "LBS_USETABSTOPS",
                  "LBS_NOINTEGRALHEIGHT",
                  "LBS_MULTICOLUMN",
                  "LBS_WANTKEYBOARDINPUT",
                  "LBS_EXTENDEDSEL",
                 };

static DWORD cbstyle[] = {
                   WS_GROUP,
                   WS_TABSTOP,
                   CBS_SIMPLE,           /* 0x0001L */
                   CBS_DROPDOWN,         /* 0x0002L */
                   CBS_DROPDOWNLIST,     /* 0x0003L */
                   CBS_OWNERDRAWFIXED,   /* 0x0010L */
                   CBS_OWNERDRAWVARIABLE,/* 0x0020L */
                   CBS_AUTOHSCROLL,      /* 0x0040L */
                   CBS_OEMCONVERT,       /* 0x0080L */
                   CBS_SORT,             /* 0x0100L */
                   CBS_HASSTRINGS,       /* 0x0200L */
                   CBS_NOINTEGRALHEIGHT, /* 0x0400L */
                   0
                 };
static char * cbsstr[] = {
                  "WS_GROUP",
                  "WS_TABSTOP",
                  "CBS_SIMPLE",
                  "CBS_DROPDOWN",
                  "CBS_DROPDOWNLIST",
                  "CBS_OWNERDRAWFIXED",
                  "CBS_OWNERDRAWVARIABLE",
                  "CBS_AUTOHSCROLL",
                  "CBS_OEMCONVERT",
                  "CBS_SORT",
                  "CBS_HASSTRINGS",
                  "CBS_NOINTEGRALHEIGHT"
                 };
static DWORD btnstyle[] = {
                  WS_GROUP,
                  WS_TABSTOP,
                  BS_PUSHBUTTON,      /* 0x00L */
                  BS_DEFPUSHBUTTON,   /* 0x01L */
                  BS_CHECKBOX,        /* 0x02L */
                  BS_AUTOCHECKBOX,    /* 0x03L */
                  BS_RADIOBUTTON,     /* 0x04L */
                  BS_3STATE,          /* 0x05L */
                  BS_AUTO3STATE,      /* 0x06L */
                  BS_GROUPBOX,        /* 0x07L */
                  BS_USERBUTTON,      /* 0x08L */
                  BS_AUTORADIOBUTTON, /* 0x09L */
                  BS_PUSHBOX,         /* 0x0AL */
                  BS_OWNERDRAW,       /* 0x0BL */
                  BS_LEFTTEXT,        /* 0x20L */
                  0xFFFFFFFF
                 };

static char * btsstr[] = {
                  "WS_GROUP",
                  "WS_TABSTOP",
                  "BS_PUSHBUTTON",
                  "BS_DEFPUSHBUTTON",
                  "BS_CHECKBOX",
                  "BS_AUTOCHECKBOX",
                  "BS_RADIOBUTTON",
                  "BS_3STATE",
                  "BS_AUTO3STATE",
                  "BS_GROUPBOX",
                  "BS_USERBUTTON",
                  "BS_AUTORADIOBUTTON",
                  "BS_PUSHBOX",
                  "BS_OWNERDRAW",
                  "BS_LEFTTEXT"
                 };
static DWORD ststyle[] = {
                  WS_GROUP,
                  WS_TABSTOP,
                  SS_LEFT,            /* 0x00L */
                  SS_CENTER,          /* 0x01L */
                  SS_RIGHT,           /* 0x02L */
                  SS_ICON,            /* 0x03L */
                  SS_BLACKRECT,       /* 0x04L */
                  SS_GRAYRECT,        /* 0x05L */
                  SS_WHITERECT,       /* 0x06L */
                  SS_BLACKFRAME,      /* 0x07L */
                  SS_GRAYFRAME,       /* 0x08L */
                  SS_WHITEFRAME,      /* 0x09L */
                  SS_USERITEM,        /* 0x0AL */
                  SS_SIMPLE,          /* 0x0BL */
                  SS_LEFTNOWORDWRAP,  /* 0x0CL */
                  SS_NOPREFIX,        /* 0x80L Don't do "&" character translation */
                  0xFFFFFFFF
                 };
static char * stsstr[] = {
                  "WS_GROUP",
                  "WS_TABSTOP",
                  "SS_LEFT",
                  "SS_CENTER",
                  "SS_RIGHT",
                  "SS_ICON",
                  "SS_BLACKRECT",
                  "SS_GRAYRECT",
                  "SS_WHITERECT",
                  "SS_BLACKFRAME",
                  "SS_GRAYFRAME",
                  "SS_WHITEFRAME",
                  "SS_USERITEM",
                  "SS_SIMPLE",
                  "SS_LEFTNOWORDWRAP",
                  "SS_NOPREFIX"
                  };

static DWORD esstyle[] = {
                  WS_GROUP,
                  WS_TABSTOP,
                  ES_LEFT,             /* 0x0000L */
                  ES_CENTER,           /* 0x0001L */
                  ES_RIGHT,            /* 0x0002L */
                  ES_MULTILINE,        /* 0x0004L */
                  ES_UPPERCASE,        /* 0x0008L */
                  ES_LOWERCASE,        /* 0x0010L */
                  ES_PASSWORD,         /* 0x0020L */
                  ES_AUTOVSCROLL,      /* 0x0040L */
                  ES_AUTOHSCROLL,      /* 0x0080L */
                  ES_NOHIDESEL,        /* 0x0100L */
                  ES_OEMCONVERT,       /* 0x0400L */
                  0xFFFFFFFF
                 };
static char * esstr[] = {
                  "WS_GROUP",
                  "WS_TABSTOP",
                  "ES_LEFT",
                  "ES_CENTER",
                  "ES_RIGHT",
                  "ES_MULTILINE",
                  "ES_UPPERCASE",
                  "ES_LOWERCASE",
                  "ES_PASSWORD",
                  "ES_AUTOVSCROLL",
                  "ES_AUTOHSCROLL",
                  "ES_NOHIDESEL",
                  "ES_OEMCONVERT"
                 };
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WndChildCB - Window Child Callback                   บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int EXPORTED CALLBACK WndChildCB(HWND hWnd, LPARAM lParam)
{
 char  str[128];
 char  str1[40];
 char  str2[40];
 DWORD wStyle;
 int  x;

  if  (!lParam)
      ;
  else                                                 /* nur richtige childs, keine Enkel */
  if (GetParent(hWnd) == (HWND)HIWORD(lParam))
     {
      GetClassName(hWnd,str1,sizeof(str1));            /* Class des Windows ermitteln */
      GetWindowText(hWnd,str2,sizeof(str2));
      wStyle  = GetWindowStyle(hWnd);

      wsprintf(str,
               "%X\t%X\t%X%04X\t%s\t%s",
               hWnd,                                  /* Window handle */
               GetWindowWord(hWnd,GWW_ID),            /* Child ID */
               HIWORD(wStyle),
               LOWORD(wStyle),
               (LPSTR)str1,                           /* class name   */
               (LPSTR)str2                            /* window text  */
               );

      x = (int)SendMessage((HWND)LOWORD(lParam),LB_ADDSTRING,0,(LPARAM)(LPSTR)str);
      SendMessage((HWND)LOWORD(lParam),LB_SETITEMDATA,x,(LPARAM)(LPVOID)hWnd);
     }
 return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WndPropCB - Window Property Callback                 บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK WndPropCB(HWND hWnd, LPCSTR lpstr,HANDLE hData)
{
 char str[128];
 int x;

   if (HIWORD(lpstr))
      wsprintf(str,
               "\"%s\",%X",
               (LPSTR)lpstr,
               hData
               );
   else
      wsprintf(str,
               "%lX,%X",
               (LONG)lpstr,
               hData
               );

   x = (int)SendDlgItemMessage(tDlg,
                          ID_WNDPROP,
                          CB_ADDSTRING,
                          0,
                          (LPARAM)(LPSTR)str);
   SendDlgItemMessage(tDlg,
                      ID_WNDPROP,
                      CB_SETITEMDATA,
                      x,
                      (LPARAM)(LPVOID)hWnd);

 return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WindowDlg - Window Dialog                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK WindowDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_WNDVIEW,0};
 char str[128];
 char str1[80];
 HWND hWnd;
 HWND hWndStyle;
 HWND hWndChild;
 DWORD wStyle;
 int x;
 RECT rect;
 FARPROC paddr;
 HDC  hDC;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      hWnd = (HWND)LOWORD(lParam);
      GetWindowText(hWnd,str1,sizeof(str1));
      wsprintf(str,
               "Window %X,\"%s\"",
               hWnd,
               (LPSTR)str1
              );
      SetWindowText(hDlg,str);

      LoadTabs(IDUS_16,str);
      SendDlgItemMessage(hDlg,ID_WNDPARENT,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
      GetWindowRect(hWnd,&rect);
      wsprintf(str,
               "Owner\t%X\t Koord.\t%d,%d,%d,%d\tInst\t%04X",
               GetWindowWord(hWnd,GWW_HWNDPARENT),
               rect.left,
               rect.top,
               rect.right,
               rect.bottom,
               GetWindowWord(hWnd,GWW_HINSTANCE)
               );
      SetDlgItemText(hDlg,ID_WNDPARENT,str);

      LoadTabs(IDUS_17,str);
      SendDlgItemMessage(hDlg,ID_WNDSTAT1,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
      wsprintf(str,
               "WndProc\t%X:%X\t ExStyle\t%lX\tID\t%04X",
               HIWORD(GetWindowLong(hWnd,GWL_WNDPROC)),
               LOWORD(GetWindowLong(hWnd,GWL_WNDPROC)),
               GetWindowLong(hWnd,GWL_EXSTYLE),
               GetWindowWord(hWnd,GWW_ID)
               );
      SetDlgItemText(hDlg,ID_WNDSTAT1,str);

      hWndChild = GetDlgItem(hDlg,ID_WNDCHILD);
      SendMessage(hWndChild,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,(LPARAM)(LPVOID)hFontAlt);
      SendMessage(hWndChild,LB_RESETCONTENT,0,0);
      SendMessage(hWndChild,XLB_SETWINDOWDATA,0,(LPARAM)(LPVOID)hWnd);
      LoadTabs(IDUS_46,str);
      SendMessage(hWndChild,LB_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
      EnumChildWindows(hWnd,WndChildCB,MAKELONG(hWndChild,hWnd));
      SendMessage(hWndChild,LB_SETCURSEL,0,0);

      hWndStyle = GetDlgItem(hDlg,ID_WNDSTYLE);
      SendMessage(hWndStyle,CB_RESETCONTENT,0,0);
      wStyle  = GetWindowLong(hWnd,GWL_STYLE);
      GetClassName(hWnd,str1,sizeof(str1));            /* Class des Windows ermitteln */

      if (wStyle & 0x40000000)
          strcpy(str,"WS_CHILD");
      else
      if (wStyle & 0x80000000)
          strcpy(str,"WS_POPUP");
      else
          strcpy(str,"WS_OVERLAPPED");

      SendMessage(hWndStyle,CB_ADDSTRING,0,(LONG)(LPSTR)str);

      for (x=0;wmsg[x];x++)
         if ((wStyle & wmsg[x]) == wmsg[x])
            SendMessage(hWndStyle,CB_ADDSTRING,0,(LONG)(LPSTR)wmsgstr[x]);

      AnsiUpper(str1);
      if (!strcmp(str1,"LISTBOX"))
         {
          for (x=0;lbstyle[x];x++)
              if ((wStyle & lbstyle[x]) == lbstyle[x])
                 SendMessage(hWndStyle,CB_ADDSTRING,0,(LONG)(LPSTR)lbsstr[x]);
         }
      else
      if (!strcmp(str1,"COMBOBOX"))
         {
          for (x=0;cbstyle[x];x++)
              if ((wStyle & cbstyle[x]) == cbstyle[x])
                 SendMessage(hWndStyle,CB_ADDSTRING,0,(LONG)(LPSTR)cbsstr[x]);
         }
      else
      if (!strcmp(str1,"BUTTON"))
         {
          for (x=0;btnstyle[x] != 0xFFFFFFFF;x++)
             {
              if (btnstyle[x] < 0x20)
                 {
                  if (btnstyle[x] == (wStyle & 0x001F))
                      SendMessage(hWndStyle,CB_ADDSTRING,0,(LONG)(LPSTR)btsstr[x]);
                 }
              else
              if ((wStyle & btnstyle[x]) == btnstyle[x])
                  SendMessage(hWndStyle,CB_ADDSTRING,0,(LONG)(LPSTR)btsstr[x]);
             }
         }
      else
      if (!strcmp(str1,"STATIC"))
         {
          for (x=0;ststyle[x] != 0xFFFFFFFF;x++)
             {
              if (ststyle[x] < 0x20)
                 {
                 if (ststyle[x] == (wStyle & 0x001F))
                     SendMessage(hWndStyle,CB_ADDSTRING,0,(LONG)(LPSTR)stsstr[x]);
                 }
              else
              if ((wStyle & ststyle[x]) == ststyle[x])
                  SendMessage(hWndStyle,CB_ADDSTRING,0,(LONG)(LPSTR)stsstr[x]);
             }
         }
      else
      if (!strcmp(str1,"EDIT"))
         {
          for (x=0;esstyle[x] != 0xFFFFFFFF;x++)
             {
              if (esstyle[x] < 0x20)
                 {
                 if (esstyle[x] == (wStyle & 0x001F))
                     SendMessage(hWndStyle,CB_ADDSTRING,0,(LONG)(LPSTR)esstr[x]);
                 }
              else
              if ((wStyle & esstyle[x]) == esstyle[x])
                  SendMessage(hWndStyle,CB_ADDSTRING,0,(LONG)(LPSTR)esstr[x]);
             }
         }
      else
          for (x=0;wbmsg[x];x++)
              if ((wStyle & wbmsg[x]) == wbmsg[x])
                 SendMessage(hWndStyle,CB_ADDSTRING,0,(LONG)(LPSTR)wbmsgstr[x]);
      AdjustCBSize(hWndStyle);
      SendMessage(hWndStyle,CB_SETCURSEL,0,0);

      tDlg = hDlg;
      SendDlgItemMessage(hDlg,ID_WNDPROP,CB_RESETCONTENT,0,0);
//      tabpos[0] = 80;
//      SendDlgItemMessage(hDlg,ID_WNDPROP,LB_SETTABSTOPS,1,(LONG)tabpos);
      if (EnumProps(hWnd,WndPropCB) == -1)
         {
          SendDlgItemMessage(hDlg,
                             ID_WNDPROP,
                             CB_ADDSTRING,
                             0,
                             (LONG)(LPSTR)"***");
          EnableWindow(GetDlgItem(hDlg,ID_WNDPROP),FALSE);
         }
      else
         {
          EnableWindow(GetDlgItem(hDlg,ID_WNDPROP),TRUE);
         }
      SendDlgItemMessage(hDlg,ID_WNDPROP,CB_SETCURSEL, 0, 0);
      SendDlgItemMessage(hDlg,ID_WNDPROP,CB_SHOWDROPDOWN,FALSE,0);
//      SetDlgItemText(hDlg,ID_WNDPROPT,"Property List");
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
         case ID_WNDCHILD:
              switch(HIWORD(lParam))
                 {
                  case LBN_SELCHANGE:
                     x = (int)SendDlgItemMessage(hDlg,ID_WNDCHILD,LB_GETCURSEL,0,0);
                     hWnd = (HWND)SendDlgItemMessage(hDlg, ID_WNDCHILD, LB_GETITEMDATA,x,0);
                     if (IsWindow(hWnd))
                        {
                         hDC = GetWindowDC(hWnd);
                         GetWindowRect(hWnd,&rect);
                         rect.right  = rect.right  - rect.left;
                         rect.bottom = rect.bottom - rect.top;
                         rect.left = 0;
                         rect.top  = 0;
                         DrawFocusRect(hDC,&rect);
                         ReleaseDC(hWnd,hDC);
                        }
                     EnableDlgItem(hDlg,
                                    ID_WNDVIEW,
                                    LB_ERR != (WORD)SendMessage((HWND)LOWORD(lParam),LB_GETCURSEL,0,0)
                                   );
                     break;
                  case LBN_DBLCLK:
                      PostMessage(hDlg,WM_COMMAND,ID_WNDVIEW,0);
                      break;
                  case XLBN_RBUTTONDOWN:
                      TrackPopup(hDlg,BtnTab);
                      break;
                 }
              break;
         case ID_WNDVIEW:
              x =  (int)SendDlgItemMessage(hDlg, ID_WNDCHILD, LB_GETCURSEL,0,0);
              if (x == LB_ERR)
                 {
                  MessageBeep(0);
                  break;
                 }
              hWnd = (HWND)SendDlgItemMessage(hDlg, ID_WNDCHILD, LB_GETITEMDATA,x,0);
              if (IsWindow(hWnd))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_WINDOW),
                                    hDlg,
                                    WindowDlg,
                                    (LPARAM)(LPVOID)hWnd);
              else
                  CreateMessage(hDlg,
                                MAKEINTRESOURCE(IDS_ERRWND1),
                                0,
                                MB_OK
                               );
              break;
         case ID_WNDCHGST:
              hWnd = (HWND)SendDlgItemMessage(hDlg,ID_WNDCHILD,XLB_GETWINDOWDATA,0,0);
         	  SetWindowStyle(hWnd,GetWindowStyle(hWnd) & (~ES_PASSWORD));
			  sprintf(str,"Style geaendert von %X",hWnd);
			  MessageBox(hDlg,str,0,MB_OK);
         	  break;	
         case ID_WNDUNASS:
              hWnd = (HWND)SendDlgItemMessage(hDlg,ID_WNDCHILD,XLB_GETWINDOWDATA,0,0);
              paddr = (FARPROC)GetWindowLong(hWnd,GWL_WNDPROC);
              if (paddr && IsReadableSelector(HIWORD(paddr)))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_UNASSEM),
                                    hDlg,
                                    UnassembleDlg,
                                    (DWORD)paddr
                                   );
              else
                  MessageBeep(0);
              break;
         case IDOK:
              PostMessage(hDlg,WM_COMMAND,ID_WNDCHILD,MAKELONG(0,LBN_DBLCLK));
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
