
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

BOOL CALLBACK UnassembleDlg  (HWND, UINT, WPARAM, LPARAM);

extern HFONT hFontAlt;
extern HWND  hWndClass;

int xClasspos = 100,yClasspos = 100;

// beschreibung WNDCLASS struktur

static DDENTRY clsstrtab[] = {
                     0, offsetof(WNDCLASS,style),      "%X",   DDT_WORD,
                     "",offsetof(WNDCLASS,style),      "",     0x00FF,
                     0, offsetof(WNDCLASS,lpfnWndProc),"%X:%X",DDT_DWORD,
                     0, offsetof(WNDCLASS,cbClsExtra), "%u",   DDT_WORD,
                     0, offsetof(WNDCLASS,cbWndExtra), "%u",   DDT_WORD,
                     0, offsetof(WNDCLASS,hInstance),  "%X",   DDT_WORD,
                     0, offsetof(WNDCLASS,hInstance),  "%s",   0x01FF,
                     0, offsetof(WNDCLASS,hIcon),      "%X",   DDT_WORD,
                     0, offsetof(WNDCLASS,hCursor),    "%X",   DDT_WORD,
                     0, offsetof(WNDCLASS,hbrBackground),"%X", DDT_WORD,
                  (PSTR)0};

static STRLOADENTRY CODESEG strloadtab[] = {
                       &clsstrtab[0].name,  IDS_XCLASS00,
                       &clsstrtab[2].name,  IDS_XCLASS01,
                       &clsstrtab[3].name,  IDS_XCLASS02,
                       &clsstrtab[4].name,  IDS_XCLASS03,
                       &clsstrtab[5].name,  IDS_XCLASS04,
                       &clsstrtab[6].name,  IDS_XCLASS05,
                       &clsstrtab[7].name,  IDS_XCLASS06,
                       &clsstrtab[8].name,  IDS_XCLASS07,
                       &clsstrtab[9].name,  IDS_XCLASS08,
                       (PSTR *)(0)};


static KEYENTRY csstr[] = {
                  "CS_BYTEALIGNCLIENT",CS_BYTEALIGNCLIENT,
                  "CS_BYTEALIGNWINDOW",CS_BYTEALIGNWINDOW,
                  "CS_CLASSDC",        CS_CLASSDC,
                  "CS_DBLCLKS",        CS_DBLCLKS,
                  "CS_GLOBALCLASS",    CS_GLOBALCLASS,
                  "CS_HREDRAW",        CS_HREDRAW,
                  "CS_NOCLOSE",        CS_NOCLOSE,
                  "CS_OWNDC",          CS_OWNDC,
                  "CS_PARENTDC",       CS_PARENTDC,
                  "CS_SAVEBITS",       CS_SAVEBITS,
                  "CS_VREDRAW",        CS_VREDRAW,
                  0};
////////////////////////////////////////////////////////
// ViewClassDlg - 1 Klasse anschauen                    
// Parameter: &WNDCLASS                                 
////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK ViewClassDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
 LPWNDCLASS lpwndclass;
 DDENTRY * pddentry;
 HWND hWnd;
 char  str[80];
 char  str1[40];
 int tabpos[3];
 WORD i;
 KEYENTRY * pkeyentry;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      if (!clsstrtab[0].name)
          LoadStringTab(hInst,strloadtab);
      hWnd = GetDlgItem(hDlg,ID_VIEWCLSLB);
      lpwndclass = (LPWNDCLASS)lParam;
      pddentry = clsstrtab;
      tabpos[0] = 68;
      tabpos[1] = 130;
      ListBox_SetTabStops(hWnd,2,tabpos);
      ListBox_ResetContent(hWnd);
      while (pddentry->name)
         {
          if (LOBYTE(pddentry->typ) == 0xFF)
             {
              if (HIBYTE(pddentry->typ) == 0)
                 {
                  pkeyentry = csstr;
                  i = *(LPWORD)lpwndclass;
                  while (pkeyentry->name)
                     {
                      if (pkeyentry->wert & i)
                         {
                          wsprintf(str,"\t%s",(LPSTR)pkeyentry->name);
                          SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                         }
                      pkeyentry++;
                     }
                 }
              else
              if (HIBYTE(pddentry->typ) == 1)
                 {
                  if (GetModuleName(GetExePtr((HANDLE)*(LPINT)((LPSTR)lpwndclass+pddentry->addr)),str1,sizeof(str1)))
                     {
                      wsprintf(str,"%s\t%s",
                               (LPSTR)pddentry->name,
                               (LPSTR)str1
                              );
                      SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                     }
                 }
             }
          else
             {
              xsprintf(pddentry,str1,(LPSTR)lpwndclass);

              wsprintf(str,"%s\t%s",
                       (LPSTR)pddentry->name,
                       (LPSTR)str1
                      );
              SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
             }
          pddentry++;
         }
      wsprintf(str,"Klasse %s",(LPSTR)lpwndclass->lpszClassName);
      SetWindowText(hDlg,str);
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
////////////////////////////////////////////////////////
// ClassDlg - Class Dialog                              
////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK ClassDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_VIEWCLASS,
                        ID_UNASSCLASS,
                        0};
 CLASSENTRY classentry;
 char  str[200];
 char  str1[40];
 PSTR  pstr;
 int   i,y;
 WNDCLASS wndclass;
 DWORD paddr;
 HWND hWnd;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      LoadTabs(IDUS_15,str);
      SendDlgItemMessage(hDlg,ID_LISTCLASS,LB_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
      SendDlgItemMessage(hDlg,ID_LISTCLASS,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      SendMessage(hDlg,WM_COMMAND,ID_NEWCLASS,0);
      SetWindowPos(hDlg,0,xClasspos,yClasspos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_NORMAL);
      rc = TRUE;
      break;
    case WM_DESTROY:
      hWndClass = 0;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xClasspos,&yClasspos);
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDOK:
         case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_UNASSCLASS:
              hWnd = GetDlgItem(hDlg,ID_LISTCLASS);
              y =  (WORD)SendMessage(hWnd,LB_GETCURSEL,0,0);
              if (y == LB_ERR)
                  break;
              paddr = SendMessage(hWnd,LB_GETITEMDATA,y,0);
              if (paddr && IsReadableSelector(HIWORD(paddr)))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_UNASSEM),
                                    hDlg,
                                    UnassembleDlg,
                                    (DWORD)paddr
                                   );
              else
                  CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRCLS1),0,MB_OK);
              break;
         case ID_NEWCLASS:
              hWnd = GetDlgItem(hDlg,ID_LISTCLASS);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);
              classentry.dwSize = sizeof(classentry);
              y = ClassFirst(&classentry);
              while (y)
                  {
                  GetClassInfo(classentry.hInst,classentry.szClassName,(LPWNDCLASS)&wndclass);
                  if (GetModuleName(classentry.hInst,str1,sizeof(str1)) > 8)
                      strcpy(&str1[8],"...");
                  wsprintf(str,
                           "%04X\t%s\t%04X\t%X:%X\t%s",
                           classentry.hInst,
                           (LPSTR)str1,
                           wndclass.style,
                           HIWORD(wndclass.lpfnWndProc),
                           LOWORD(wndclass.lpfnWndProc),
                           (LPSTR)classentry.szClassName
                           );
                  y = SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM)(LPSTR)str);
                  SendMessage(hWnd,LB_SETITEMDATA,y,(LPARAM)wndclass.lpfnWndProc);
                  y = ClassNext(&classentry);
                  }
              SendMessage(hWnd,LB_SETCURSEL,0,0);
              break;
         case ID_VIEWCLASS:
              hWnd = GetDlgItem(hDlg,ID_LISTCLASS);
              i = (WORD)SendMessage(hWnd, LB_GETCURSEL,0,0);
              if (i == LB_ERR)
                 break;
              SendMessage(hWnd,LB_GETTEXT,i,(LONG)(LPSTR)str);
              sscanf(str,"%X",&classentry.hInst);
              for (i=4,pstr=str;i;i--,pstr++)
                  pstr = strchr(pstr,9);
              strcpy(classentry.szClassName,pstr);
              if (GetClassInfo(classentry.hInst,classentry.szClassName,(LPWNDCLASS)&wndclass))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_VIEWCLS),
                                    hDlg,
                                    ViewClassDlg,
                                    (LPARAM)(LPVOID)&wndclass
                                   );
              else
                  CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRCLS1),0,MB_OK);
              break;
         case ID_LISTCLASS:
              switch(HIWORD(lParam))
                 {
                  case LBN_DBLCLK:
                    PostMessage(hDlg,WM_COMMAND,ID_VIEWCLASS,0);
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
