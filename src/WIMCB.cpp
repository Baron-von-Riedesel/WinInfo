
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dosx.h"
#include "wininfo.h"
#include "wininfox.h"

BOOL  WINAPI CheckMessage(MSG *,WORD,HWND);

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT hFontAlt;
extern HWND  hWndMCB;

BOOL CALLBACK MCBDlg  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ViewDlg (HWND, UINT, WPARAM, LPARAM);

static CSTRC szFStr1[] = {"%c%u\t%lX\t%lX\t%X\t%s"};

int  xMCBpos = 100;
int  yMCBpos = 100;

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ MCBDlg - MCB Dialog                                  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK MCBDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_SUBDLG1,
                        0};
 char  szStr[80];
 char  szStr2[20];
// DWORD dw[2];
 int   y,x,i;
 HWND hWnd;
 MCBENTRY mcbentry;
 MCBENTRY mcbsubentry;
 HGLOBAL hMem;
 long memsize,memoffs;
 MSG msg;
 int zflag;
 HEXDUMPPARM hdp;
 BOOL rc = FALSE;

 switch (message)
    {
    case WM_COMMAND:
      switch (wParam)
         {
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                {
                 case LBN_DBLCLK:
                   SendMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                   break;
                  case XLBN_RBUTTONDOWN:
                    TrackPopup(hDlg,BtnTab);
                    break;
                }
              break;
         case ID_SUBDLG1:
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              if ((y = (WORD)SendMessage(hWnd,LB_GETCURSEL,0,0)) == LB_ERR)
                 {
                  MessageBeep(0);
                  break;
                 }
              SendMessage(hWnd,LB_GETTEXT,y,(LPARAM)(LPSTR)szStr);
              if (sscanf(szStr,"%*c%X %lX %lX ",&i,&hdp.dwOffset,&hdp.dwLength) >= 3)
                 {
                  hdp.dwOffset = hdp.dwOffset + 0x10;
                  wsprintf(szStr,"DOS Speicherblock: %0X",(WORD)(hdp.dwOffset>>4));
                  hdp.wType = HDP_LINEAR | HDP_DISABLESELEDIT;
                  hWnd = CreateDialogParam( hInst,
                                        MAKEINTRESOURCE(IDD_VIEW),
                                        hDlg,
                                        ViewDlg,
                                        (LPARAM)(LPVOID)&hdp
                                        );
                  SetWindowText(hWnd,szStr);
                 }
              break;
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_REFRESH:
              SetCursor(hCursorWait);
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              ListBox_ResetContent(hWnd);

              x = 1;
              y = McbFirst(&mcbentry,1);
              while (y)
                 {
                  wsprintf(szStr,
                           szFStr1,
                           ' ',
                           x,
                           mcbentry.addr,
                           (DWORD)mcbentry.size<<4,
                           mcbentry.psp,
                           (LPSTR)mcbentry.name
                           );

                  ListBox_AddString(hWnd,szStr);
                  if ((mcbentry.psp == 8) && (mcbentry.wSK == 'DS'))
                     {
                      memcpy(&mcbsubentry,&mcbentry,sizeof(mcbsubentry));
                      y = McbSubFirst(&mcbsubentry);
                      while (y)
                         {
                          switch (mcbsubentry.bKB)
                            {
                             case 'F':
                               strcpy(szStr2,"Files");
                               break;
                             case 'I':
                               strcpy(szStr2,"IFS");
                               break;
                             case 'B':
                               strcpy(szStr2,"Buffers");
                               break;
                             case 'S':
                               strcpy(szStr2,"Stacks");
                               break;
                             case 'L':
                               strcpy(szStr2,"Lastdrive");
                               break;
                             case 'X':
                               strcpy(szStr2,"FCBs");
                               break;
                             default:
                               strcpy(szStr2,mcbsubentry.name);
                               break;
                            }
                          wsprintf(szStr,
                                   szFStr1,
                                   '*',
                                   x,
                                   mcbsubentry.addr,
                                   (DWORD)mcbsubentry.size<<4,
                                   mcbsubentry.psp,
                                   (LPSTR)szStr2
                                   );

                          ListBox_AddString(hWnd,szStr);
                          y = McbSubNext(&mcbsubentry);
                         }
                     }
                  x++;
                  y = McbNext(&mcbentry);
                 }
              McbClose(&mcbentry);
              ListBox_SetCurSel(hWnd,0);

                                       /* freien DOS speicher errechnen */
              for (memsize = 0,zflag = 1,memoffs = 0xFFFFF;
                   memoffs && CheckMessage(&msg,PM_REMOVE,hDlg);
                   memoffs = memoffs >> 1)
                 {
                  memsize = memsize + memoffs * zflag;
                  if (hMem = (HGLOBAL)LOWORD(GlobalDosAlloc(memsize)))
                     {
                      GlobalDosFree((UINT)hMem);
                      zflag = 1;
                     }
                  else
                      zflag = -1;
                 }

              wsprintf(szStr,
                       "DOS-Memory allokierbar\t%lu\t",
                       memsize
                      );
              SetDlgItemText(hDlg,ID_STATUS1,szStr);
              SetCursor(hCursor);
              break;
         }
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xMCBpos,&yMCBpos);
      break;
    case WM_DESTROY:
      hWndMCB = 0;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_INITDIALOG:
      LoadTabs(IDUS_19,szStr);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)szStr,(LPARAM)(LPSTR)(szStr+2));
      SendDlgItemMessage(hDlg,
                         ID_LISTBOX1,
                         XLB_SETEXTSTYLE,
                         XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      LoadTabs(IDUS_23,szStr);
      SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)szStr,(LPARAM)(LPSTR)(szStr+2));
      PostMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      SetWindowPos(hDlg,0,xMCBpos,yMCBpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_NORMAL);
      rc = TRUE;
      break;
    default:
      break;
    }

 return rc;
}
