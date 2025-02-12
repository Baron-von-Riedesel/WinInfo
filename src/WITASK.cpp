
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

// #define strcat lstrcat
// #define strcpy lstrcpy

// externals

extern "C" BOOL WINAPI PostEvent(HTASK);

extern GMEMFILTER gmf;
extern HFONT hFontAlt;

// Prototypen

void WINAPI EnableItems(HWND, PINT, WORD);

// globale Variable

HWND hWndTask = 0;
int  xTaskpos = 100;
int  yTaskpos = 100;

BOOL EXPORTED CALLBACK WindowDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK GlobalDlg     (HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK UnassembleDlg (HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK LocalDlg      (HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK ViewDlg       (HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK AtomDlg       (HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK StackDlg      (HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK PSPDlg        (HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK ViewTaskDlg   (HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK TaskDlg       (HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK XModulDlg     (HWND, UINT, WPARAM, LPARAM);

// statische Variable

static HWND zWnd;
static BYTE fErr;

static int  BtnTab[] = {
  ID_DETAILTASK, ID_STACKTASK,
  ID_LOCALTASK,  ID_GLOBALTASK,
  ID_MODULTASK,
  ID_VIEWTASK,   ID_AUTOTASK, ID_PSPTASK, ID_QUEUTASK,
  ID_SWTTASK,
  ID_TRMTASK,    ID_KILLTASK,
  0};
static int  BtnTab2[] = {ID_WINDTASK | 0x8000,IDS_VIEW,0};

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WindowCB - Window Callback                           บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK WindowCB(HWND hWnd, LPARAM lparam)
{
 char str1[80];
 char str2[40];
 char str[160];
 HWND hWnd2;
 HANDLE hParent;
 DWORD  wStyle;
 WORD x;
 BOOL rc;

 rc = TRUE;

  switch (lparam)
    {
    case 0:                                                    /* terminate task */
      if (!GetParent(hWnd))                                    /* nur fenster ohne owner beachten */
         {
          if (IsWindowVisible(hWnd))
             zWnd = hWnd;
          else
          if (!zWnd)
             zWnd = hWnd;
         }
      break;
    case 1:                                                    /* switch to task */
      if (!GetParent(hWnd))
           if (IsWindowVisible(hWnd))
              {
               zWnd = hWnd;
               rc = FALSE;
              }
           else
           if (!zWnd)
               zWnd = hWnd;
      break;
    default:                                                   /* windows auflisten */
      GetWindowText(hWnd,str1,sizeof(str1));
      GetClassName(hWnd,str2,sizeof(str2));                    /* Class des Windows ermitteln */
      hParent = GetParent(hWnd);
      wStyle  = GetWindowStyle(hWnd);
      wsprintf(str,
               "%X\t%X\t%X%04X\t%s\t%s",
               hWnd,                 /* Window handle */
               hParent,              /* dessen Parent/owner */
               HIWORD(wStyle),       /* window style */
               LOWORD(wStyle),
               (LPSTR)str2,          /* class name  */
               (LPSTR)str1           /* dessen text */
               );

      hWnd2 = (HWND)LOWORD(lparam);
      x = ListBox_AddString(hWnd2,str);
      ListBox_SetItemData(hWnd2,x,(LPARAM)(LPVOID)hWnd);
    }

 return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ CheckTask - pruefen ob Task noch da                  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL CheckTask(HWND hDlg,HTASK * phandle,TASKENTRY * ptaskentry)
{
 HTASK hTask;
 HWND hWnd;
 int x;
 int  tabpos[2];

  hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
  x = ListBox_GetCurSel(hWnd);
  hTask = (HTASK)ListBox_GetItemData(hWnd,x);
  *phandle = hTask;
  (*ptaskentry).dwSize = sizeof(*ptaskentry);
  if (TaskFindHandle(ptaskentry,hTask))
     return TRUE;
  EnableItems(hDlg,BtnTab,0);
  SendDlgItemMessage(hDlg,ID_LISTWINDOWS,LB_RESETCONTENT,0,0);
  tabpos[0] = 8;
  tabpos[1] = -8;
  SendDlgItemMessage(hDlg,ID_STATTASK,ST_SETTABSTOPS,2,(LPARAM)(LPVOID)&tabpos);
  SetDlgItemText(hDlg,ID_STATTASK,"\tTask ist inzwischen beendet!\t");
  fErr = TRUE;
  return FALSE;
}
void GetCSIP(DWORD * pdw)
{
   _asm
      {
       mov bx,pdw
       mov word ptr [bx+2],cs
       mov ax,[bp+2]
       mov word ptr [bx+0],ax
      }
}

BOOL TaskOnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int x;
	UINT y;
	char  str[80];
	int    tabpos[10];
	TASKENTRY taskentry;
	HTASK  hTask;
	DWORD  dw[2];
	HWND   hWnd;
	RECT   rect;
	HDC    hDC;
	LPINT  lpint;
	HEXDUMPPARM hdp;
	int    wStackTop,wStackMin,wStackBot;

	switch (wParam) {
	case ID_REFRESH:
		hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
		ListBox_ResetContent(hWnd);
		taskentry.dwSize = sizeof(taskentry);
		x = TaskFirst(&taskentry);
		while (x) {
			wsprintf(str,
					"%04X\t%s\t%04X\t%04X\t%04X",
					taskentry.hTask,
					(LPSTR)taskentry.szModule,
					taskentry.hInst,
					taskentry.wPSPOffset,
					taskentry.hModule
//                         taskentry.hQueue
					);

			y = ListBox_AddString(hWnd,str);
			ListBox_SetItemData(hWnd,y,MAKELONG(taskentry.hTask,0));
			x = TaskNext(&taskentry);
		}
		SendMessage(hWnd,LB_SETCURSEL,0,0);
		break;
	case ID_LISTBOX1:
		switch(HIWORD(lParam)) {
                case LBN_SELCHANGE:                       /* selektion geaendert */
                     if (!CheckTask(hDlg,&hTask,&taskentry))
                         break;
                     EnableItems(hDlg,BtnTab,1);
                                                          /* DGROUP pointer holen */
                     lpint     = 0;
                     wStackTop = 0;
                     wStackMin = 0;
                     wStackBot = 0;
                     if (taskentry.wSS)
                        {
                         lpint = (LPINT)MAKELONG(0,taskentry.wSS);
                         if (!*lpint)                     /* dgroup sollte mit 0000 beginnen */
                            {                             /* local heap + atomtabelle da? */
                             EnableDlgItem(hDlg,ID_LOCALTASK,IsValidLocalHeap((HGLOBAL)taskentry.wSS));
                             wStackTop = *(lpint+5);
                             wStackMin = *(lpint+6);
                             wStackBot = *(lpint+7);
                            }
                        }
                                                         /* listbox leeren */
                     hWnd = GetDlgItem(hDlg,ID_LISTWINDOWS);
                     ListBox_ResetContent(hWnd);
                     EnumTaskWindows(hTask,WindowCB,(LPARAM)(LPVOID)hWnd);
                                                         /* statuszeile setzen */
                     if (fErr)
                        {
                         fErr = FALSE;
                         tabpos[0] = 18;
                         tabpos[1] = 40;
                         tabpos[2] = 70;
                         tabpos[3] = 104;
                         tabpos[4] = 144;
                         tabpos[5] = 166;
                         tabpos[6] = 186;
                         tabpos[7] = 208;
                         tabpos[8] = 228;
                         tabpos[9] = 250;
                         SendDlgItemMessage(hDlg,ID_STATTASK,ST_SETTABSTOPS,10,(LPARAM)(LPVOID)&tabpos);
                        }
                     if (hTask == GetCurrentTask())
                         GetCSIP(&dw[0]);
                     else
                         dw[0] = TaskGetCSIP(hTask);
                     wsprintf(str,
                              "SP\t%X\tCS:IP\t%X:%X\tStack-Top\t%X\tMin\t%X\tBot\t%X\t",
                              taskentry.wSP,
                              HIWORD(dw[0]),
                              LOWORD(dw[0]),
                              wStackTop,
                              wStackMin,
                              wStackBot
                             );
                     SetDlgItemText(hDlg,ID_STATTASK,str);
                     break;
                case LBN_DBLCLK:
                     if (!CheckTask(hDlg,&hTask,&taskentry))
                        CreateMessage(hDlg,errorstr2,0,MB_OK);
                     else
                        PostMessage(hDlg,WM_COMMAND,ID_DETAILTASK,0);
                     break;
                case XLBN_RBUTTONDOWN:
                     TrackPopup(hDlg,BtnTab);
                     break;
               }
            break;
	case ID_STACKTASK:
            if (CheckTask(hDlg,&hTask,&taskentry))
               {
                CreateDialogParam(hInst,
                                  MAKEINTRESOURCE(IDD_STACKTRACE),
                                  hDlg,
                                  StackDlg,
                                  (LPARAM)(LPVOID)hTask
                                 );
               }
            break;
	case ID_MODULTASK:
		if (CheckTask(hDlg,&hTask,&taskentry)) {
			CreateDialogParam(hInst,
							MAKEINTRESOURCE(IDD_XMODULE),
							hDlg,
							XModulDlg,
							(LPARAM)(LPVOID)taskentry.hModule);
		}
		break;
	case ID_DETAILTASK:
		if (CheckTask(hDlg,&hTask,&taskentry)) {
                CreateDialogParam(hInst,
                                  MAKEINTRESOURCE(IDD_XTASK),
                                  hDlg,
                                  ViewTaskDlg,
                                  (LPARAM)(LPVOID)hTask
                                 );
		}
		break;
	case ID_VIEWTASK:
		if (CheckTask(hDlg,&hTask,&taskentry)) {
			if (GetFocus() == GetDlgItem(hDlg,ID_LISTWINDOWS)) {
                    PostMessage(hDlg,WM_COMMAND,ID_LISTWINDOWS,MAKELONG(0,LBN_DBLCLK));
                    break;
			}
			hdp.dwOffset = 0;
			hdp.hGlobal  = hTask;
			hdp.dwLength = 0;
			hdp.wType    = 0;
			hWnd = CreateDialogParam( hInst,
							MAKEINTRESOURCE(IDD_VIEW),
							hDlg,
							ViewDlg,
							(LPARAM)(LPVOID)&hdp);
			strcpy(str,"Task: ");
			strcat(str,taskentry.szModule);
			SetWindowText(hWnd,str);
		}
		break;
	case ID_AUTOTASK:
            if (CheckTask(hDlg,&hTask,&taskentry))
               {
                hdp.dwOffset = 0;
//                hdp.hGlobal  = taskentry.wSS;
                hdp.hGlobal  = taskentry.hInst;
                hdp.dwLength = 0;
                hdp.wType    = 0;
                hWnd = CreateDialogParam( hInst,
                                          MAKEINTRESOURCE(IDD_VIEW),
                                          hDlg,
                                          ViewDlg,
                                          (LPARAM)(LPVOID)&hdp
                                          );
                strcpy(str,"Automatic Data Segment: ");
                strcat(str,taskentry.szModule);
                SetWindowText(hWnd,str);
               }
            break;
	case ID_PSPTASK:
            if (CheckTask(hDlg,&hTask,&taskentry))
                CreateDialogParam( hInst,
                                   MAKEINTRESOURCE(IDD_PSP),
                                   hDlg,
                                   PSPDlg,
                                   (DWORD)MAKELONG(taskentry.wPSPOffset,hTask));
            break;
	case ID_GLOBALTASK:
            if (CheckTask(hDlg,&hTask,&taskentry))
               {
                gmf.hOwner = taskentry.hTask;
                CreateDialogParam(hInst,
                                  MAKEINTRESOURCE(IDD_GLOBAL),
                                  hDlg,
                                  GlobalDlg,
                                  (LPARAM)(LPVOID)&gmf
                                 );
               }
            break;
	case ID_QUEUTASK:                           /* task queue */
            if (CheckTask(hDlg,&hTask,&taskentry))
               {
                hdp.dwOffset = 0;
                hdp.hGlobal  = taskentry.hQueue;
                hdp.dwLength = 0;
                hdp.wType    = 0;
                hWnd = CreateDialogParam( hInst,
                                          MAKEINTRESOURCE(IDD_VIEW),
                                          hDlg,
                                          ViewDlg,
                                          (LPARAM)(LPVOID)&hdp
                                          );
                strcpy(str,"Task Queue: ");
                strcat(str,taskentry.szModule);
                SetWindowText(hWnd,str);
               }
            break;
	case ID_LOCALTASK:
            if (CheckTask(hDlg,&hTask,&taskentry))
               {
                hWnd = CreateDialogParam(hInst,
                                         MAKEINTRESOURCE(IDD_LOCAL),
                                         hWndMain,
                                         LocalDlg,
                                         MAKELONG(taskentry.wSS,
                                                  taskentry.hModule)
                                        );
               }
            break;
	case ID_TRMTASK:                                  /* task beenden */
		if (CheckTask(hDlg,&hTask,&taskentry)) {
			zWnd = 0;
			EnumTaskWindows(hTask,WindowCB,0);
			if (!zWnd) {
				PostEvent(hTask);					// zusไtzlich
				if (PostAppMessage(hTask,WM_QUIT,0x00FF,0)) {
					DirectedYield(hTask);
				} else
					CreateMessage(hDlg,
								MAKEINTRESOURCE(IDS_ERRTSK3),
								MAKEINTRESOURCE(IDS_HINT),
								MB_OK);
			} else {
//				PostMessage(zWnd,WM_CLOSE,0,0);
				PostMessage(zWnd,WM_SYSCOMMAND,SC_CLOSE,0);
				DirectedYield(hTask);
			}
		}
		break;
	case ID_KILLTASK:
            CreateMessageParam(hDlg,
                               MAKEINTRESOURCE(IDS_ERRTSK2),
                               MAKEINTRESOURCE(IDS_WARNING),
                               MB_OKCANCEL,
                               ID_WARNING1
                              );
            break;
	case ID_WARNING1:
            if (HIWORD(lParam) == IDOK)
                if (CheckTask(hDlg,&hTask,&taskentry))
                    TerminateApp(hTask,UAE_BOX);
            break;
	case ID_SWTTASK:
            if (CheckTask(hDlg,&hTask,&taskentry))
               {
                zWnd = 0;
                EnumTaskWindows(hTask,WindowCB,1);
                if (zWnd)
                   {
                    SetActiveWindow(zWnd);
                    ShowWindow(zWnd,SW_SHOWNORMAL);
                   }
               }
            break;
	case ID_WINDTASK:
            if (CheckTask(hDlg,&hTask,&taskentry))
               {
                x =  (WORD)SendDlgItemMessage(hDlg, ID_LISTWINDOWS, LB_GETCURSEL,0,0);
                if (x == LB_ERR)
                    break;
                hWnd = (HWND)SendDlgItemMessage(hDlg, ID_LISTWINDOWS, LB_GETITEMDATA,x,0);
                if (GetWindowTask(hWnd) == hTask)
                    CreateDialogParam(hInst,
                                      MAKEINTRESOURCE(IDD_WINDOW),
                                      hDlg,
                                      WindowDlg,
                                      (LPARAM)(LPVOID)hWnd);
                else
                    CreateMessage(hDlg,
                                  MAKEINTRESOURCE(IDS_ERRTSK4),
                                  0,
                                  MB_OK
                                 );
               }
            break;
	case ID_LISTWINDOWS:
            if (CheckTask(hDlg,&hTask,&taskentry))
               {
                switch(HIWORD(lParam))
                  {
                   case LBN_SELCHANGE:
                     x = (int)SendDlgItemMessage(hDlg,ID_LISTWINDOWS,LB_GETCURSEL,0,0);
                     hWnd = (HWND)SendDlgItemMessage(hDlg, ID_LISTWINDOWS, LB_GETITEMDATA,x,0);
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
                     break;
                   case LBN_DBLCLK:
                     PostMessage(hDlg,WM_COMMAND,ID_WINDTASK,0);
                     break;
                   case XLBN_RBUTTONDOWN:
                     TrackPopup(hDlg,BtnTab2);
                     break;
                  }
               }
            break;
	case IDCANCEL:
	case IDOK:
            PostMessage(hDlg,WM_CLOSE,0,0);
            break;
	}
	return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ TaskDlg - Task Dialog                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK TaskDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
// char  zstr[10];
// PSTR  pstr;
// long  lx;
 int    tabpos[10];
 BOOL   rc = FALSE;

	switch (message) {
	case WM_INITDIALOG:
		fErr = TRUE;
		SendDlgItemMessage(hDlg,ID_STATTASK,ST_SETALTFONT,-1,0);
		tabpos[0] = 28;
		tabpos[1] = 72;
		tabpos[2] = 96;
		tabpos[3] = 120;
		tabpos[4] = 144;
		SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,5,(LPARAM)(LPINT)&tabpos);
		SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
		tabpos[0] = 22;
		tabpos[1] = 44;
		tabpos[2] = 84;
		tabpos[3] = 134;
		SendDlgItemMessage(hDlg,ID_LISTWINDOWS,LB_SETTABSTOPS,4,(LPARAM)(LPVOID)&tabpos);
		SendDlgItemMessage(hDlg,ID_LISTWINDOWS,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
		SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
		SetWindowPos(hDlg,0,xTaskpos,yTaskpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
		ShowWindow(hDlg,SW_NORMAL);
		rc = TRUE;
		break;
	case WM_CLOSE:
//		PostMessage(hWndMenu,WM_COMMAND,ID_DESTROY,(LPARAM)hDlg);
		DestroyWindow(hDlg);
		rc = TRUE;
		break;
    case WM_DESTROY:
		hWndTask = 0;
		break;
	case WM_MOVE:
		SaveWindowPos(hDlg,&xTaskpos,&yTaskpos);
		break;
	case WM_COMMAND:
		rc = TaskOnCommand(hDlg,wParam,lParam);
		break;
	default:
      break;
    }
 return rc;
}
