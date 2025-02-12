
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

extern HFONT hFontAlt;

BOOL EXPORTED CALLBACK UnassembleDlg (HWND, UINT, WPARAM, LPARAM);

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ StackDlg - Stack einer Task                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK StackDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int iXPos,iYPos = 0;
 static int  BtnTab[] = {ID_SUBDLG1,0};
 UINT  x,y;
 char  str[80];
 char  str1[40];
 char  str2[40];
 DWORD  dw;
 STACKTRACEENTRY stackentry;
 TASKENTRY taskentry;
 int    tabpos[5];
 HTASK  hTask;
 HWND   hWnd;
 WORD   wSS,wCS,wIP,wBP;
 BOOL   rc = FALSE;
 FARPROC fpActProc;

 switch (message)
   {
    case WM_INITDIALOG:
      hTask = (HTASK)lParam;
      SetWindowLong(hDlg,DWL_USER,lParam);
      taskentry.dwSize = sizeof(taskentry);
      if (TaskFindHandle(&taskentry,hTask))
         {
          GetWindowText(hDlg,str1,sizeof(str1));
          wsprintf(str,
                   str1,
                   (LPSTR)taskentry.szModule
                  );
          SetWindowText(hDlg,str);
         }
      tabpos[0] = 50;
      tabpos[1] = 100;
      tabpos[2] = 144;
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,3,(LPARAM)(LPVOID)&tabpos);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      if (iXPos)
          SetWindowPos(hDlg,0,iXPos,iYPos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_NORMAL);
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
       case ID_REFRESH:
            hTask = (HTASK)GetWindowLong(hDlg,DWL_USER);
            taskentry.dwSize = sizeof(taskentry);
            if (!TaskFindHandle(&taskentry,hTask))
               {
                EnableDlgItem(hDlg,ID_REFRESH,0);
                EnableDlgItem(hDlg,ID_SUBDLG1,0);
                MessageBeep(0);
                break;
               }
            hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
            ListBox_ResetContent(hWnd);
            stackentry.dwSize = sizeof(stackentry);
            if (hTask == GetCurrentTask())
               {
//                GetStackParms(&sbci);
                _asm
                   {
                    mov wSS,ss
                    mov wCS,cs
                    mov ax,offset StackDlg
                    mov wIP,ax
                    mov wBP,bp
                   }
                x = StackTraceCSIPFirst(&stackentry,
                                        wSS,
                                        wCS,
                                        wIP,
                                        wBP
                                        );
               }
            else
                x = StackTraceFirst(&stackentry,hTask);
            while (x)
               {
                GetModuleName(stackentry.hModule,str1,sizeof(str1));
                _asm {
                      xor ax,ax
                      mov word ptr fpActProc+0,ax
                      mov word ptr fpActProc+2,ax
                      mov es,stackentry.wCS
                      mov bx,stackentry.wIP
                      cmp bx,5
                      jb  short lab1
                      cmp byte ptr es:[bx-5],09Ah
                      jnz short lab1
                      mov ax,es:[bx-4]
                      mov word ptr fpActProc+0,ax
                      mov ax,es:[bx-2]
                      mov word ptr fpActProc+2,ax
                    lab1:
                     }
                if (fpActProc)
                   {
                    GetSymbolicNameEx(fpActProc,str2);
                   }
                else
                   str2[0]=0;
                wsprintf(str,
                         "%04X:%04X\t%04X:%04X\t%s\t%s",
                         stackentry.wSS,
                         stackentry.wBP,
                         stackentry.wCS,
                         stackentry.wIP,
                         (LPSTR)str1,
                         (LPSTR)str2
                        );

                y = ListBox_AddString(hWnd,str);
                ListBox_SetItemData(hWnd,y,MAKELONG(stackentry.wIP,stackentry.wCS));
                x = StackTraceNext(&stackentry);
               }
            ListBox_SetCurSel(hWnd,0);
            break;
       case ID_SUBDLG1:
            hTask = (HTASK)GetWindowLong(hDlg,DWL_USER);
            taskentry.dwSize = sizeof(taskentry);
            if (!TaskFindHandle(&taskentry,hTask))
               {
                EnableDlgItem(hDlg,ID_REFRESH,0);
                EnableDlgItem(hDlg,ID_SUBDLG1,0);
                MessageBeep(0);
                break;
               }
            x =  (WORD)SendDlgItemMessage(hDlg, ID_LISTBOX1, LB_GETCURSEL,0,0);
            if (x == LB_ERR)
                break;
            dw = (LONG)SendDlgItemMessage(hDlg, ID_LISTBOX1, LB_GETITEMDATA,x,0);
            CreateDialogParam(hInst,
                              MAKEINTRESOURCE(IDD_UNASSEM),
                              hDlg,
                              UnassembleDlg,
                              dw
                             );

            break;
       case ID_LISTBOX1:
            switch(HIWORD(lParam))
               {
                case LBN_SELCHANGE:                       /* selektion geaendert */
                  break;
                case LBN_DBLCLK:
                  PostMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                  break;
                case XLBN_RBUTTONDOWN:
                     TrackPopup(hDlg,BtnTab);
                     break;
               }
            break;
       case IDOK:
       case IDCANCEL:
            PostMessage(hDlg,WM_CLOSE,0,0);
            break;
       }
      rc = TRUE;
      break;
    default:
      break;
    }
 return rc;
}
