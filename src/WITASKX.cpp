
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
#include "resource.h"

static const char szcFStr1X[] = "%04X";
static const char szcFStr2X[] = "%04X:%04X";

static DDENTRY tskstrtab[] = {
        0,offsetof(TASKSTRUC,wPriority),   szcFStr1X,DDT_WORD,
        0,offsetof(TASKSTRUC,wSemaphor),   szcFStr1X,DDT_WORD,
        0,offsetof(TASKSTRUC,dwInt0vec),   szcFStr2X,DDT_DWORD,
        0,offsetof(TASKSTRUC,dwInt2vec),   szcFStr2X,DDT_DWORD,
        0,offsetof(TASKSTRUC,dwInt4vec),   szcFStr2X,DDT_DWORD,
        0,offsetof(TASKSTRUC,dwInt6vec),   szcFStr2X,DDT_DWORD,
        0,offsetof(TASKSTRUC,dwInt7vec),   szcFStr2X,DDT_DWORD,
        0,offsetof(TASKSTRUC,dwInt3Evec),  szcFStr2X,DDT_DWORD,
        0,offsetof(TASKSTRUC,dwInt75vec),  szcFStr2X,DDT_DWORD,
        0,offsetof(TASKSTRUC,wPSP),        szcFStr1X,DDT_WORD,
        0,offsetof(TASKSTRUC,wAlias),      szcFStr1X,DDT_WORD,
        0,offsetof(TASKSTRUC,wInstance),   szcFStr1X,DDT_WORD,
        0,offsetof(TASKSTRUC,wModule),     szcFStr1X,DDT_WORD,
        0,offsetof(TASKSTRUC,wParent),     szcFStr1X,DDT_WORD,
        0,offsetof(TASKSTRUC,wThunkNext),  szcFStr1X,DDT_WORD,
        0,offsetof(TASKSTRUC,dwSSSP),      szcFStr2X,DDT_DWORD,
        0,offsetof(TASKSTRUC,wFlags),      szcFStr1X,DDT_WORD,
        0,offsetof(TASKSTRUC,dwDTA),       szcFStr2X,DDT_DWORD,
        0,offsetof(TASKSTRUC,wQueue),      szcFStr1X,DDT_WORD,
        (PSTR)0};

static STRLOADENTRY CODESEG strloadtab[] = {
        &tskstrtab[ 0].name, IDS_XTASK00,
        &tskstrtab[ 1].name, IDS_XTASK12,
        &tskstrtab[ 2].name, IDS_XTASK01,
        &tskstrtab[ 3].name, IDS_XTASK02,
        &tskstrtab[ 4].name, IDS_XTASK03,
        &tskstrtab[ 5].name, IDS_XTASK04,
        &tskstrtab[ 6].name, IDS_XTASK05,
        &tskstrtab[ 7].name, IDS_XTASK06,
        &tskstrtab[ 8].name, IDS_XTASK07,
        &tskstrtab[ 9].name, IDS_XTASK08,
        &tskstrtab[10].name, IDS_XTASK09,
        &tskstrtab[11].name, IDS_XTASK0A,
        &tskstrtab[12].name, IDS_XTASK0B,
        &tskstrtab[13].name, IDS_XTASK0C,
        &tskstrtab[14].name, IDS_XTASK0D,
        &tskstrtab[15].name, IDS_XTASK0E,
        &tskstrtab[16].name, IDS_XTASK0F,
        &tskstrtab[17].name, IDS_XTASK10,
        &tskstrtab[18].name, IDS_XTASK11,
        (PSTR *)(0)};
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ViewTask  - 1 Task  anschauen                        บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK ViewTaskDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
 static int iXPos,iYPos = 0;
 HTASK  hTask;
 HWND   hWnd;
 DDENTRY * pddentry;
 TASKSTRUC FAR * lptsk;
 TASKENTRY taskentry;
 LPSTR lpstr;
 char str[128];
 char str1[40];
 int  tabpos[2];
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      hTask = (HTASK)LOWORD(lParam);
      SetWindowLong(hDlg,DWL_USER,lParam);
      if (!tskstrtab[0].name)
          LoadStringTab(hInst,strloadtab);
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
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      if (iXPos)
          SetWindowPos(hDlg,0,iXPos,iYPos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_SHOWNORMAL);
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
         case ID_REFRESH:
              hTask = (HTASK)GetWindowLong(hDlg,DWL_USER);
              taskentry.dwSize = sizeof(taskentry);
              if (!TaskFindHandle(&taskentry,hTask))
                  break;
              tabpos[0] = 108;
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              ListBox_SetTabStops(hWnd,1,&tabpos);
              lptsk = (TASKSTRUC FAR *)MAKELONG(0,hTask);
              lpstr = (LPSTR)lptsk;
              pddentry = tskstrtab;
              ListBox_ResetContent(hWnd);
              while (pddentry->name)
                 {
                  xsprintf(pddentry,str1,lpstr);

                  wsprintf(str,"%s\t%s",
                           (LPSTR)pddentry->name,
                           (LPSTR)str1
                          );
                  ListBox_AddString(hWnd,str);
                  pddentry++;
                 }
              break;
        }
      rc = TRUE;
      break;
   }
 return rc;
}
