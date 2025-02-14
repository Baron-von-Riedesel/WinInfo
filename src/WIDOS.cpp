
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "dpmi.h"
#include "dosx.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

#define strlen lstrlen
#define strcpy lstrcpy
#define strcat lstrcat
#define strncpy _fstrncpy
#define strncmp _fstrncmp

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT hFontAlt;

BOOL FAR PASCAL McbDlg (HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL DevDlg (HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL CDSDlg (HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL DPBDlg (HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL EMSDlg (HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL XMSDlg (HWND, UINT, WPARAM, LPARAM);

extern BOOL EMSCheck(void);

int xDOSpos  = 100;
int yDOSpos  = 100;
HWND hWndDOS = 0;
HWND hWndMCB = 0;
HWND hWndDEV = 0;
HWND hWndDPB = 0;
HWND hWndCDS = 0;
HWND hWndEMS = 0;
HWND hWndXMS = 0;

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ DosDlg - DOS Dialog                                  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK DosDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 char  str[80];
 char  str1[20];
 char  str3[40];
 char  * pstr;
 int   y,x,i,j;
 HTASK hTask;
 SFTENTRY sftentry;
 TASKENTRY taskentry;
 LPBYTE lpfindex;
 LPWORD lpPSP;
 WORD sel,wVersion;
 WORD wKrnlPSP;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      LoadTabs(IDUS_45,str);
      SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
      LoadTabs(IDUS_44,str);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,0,(LPARAM)(LPVOID)hFontAlt);
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      SetWindowPos(hDlg,0,xDOSpos,yDOSpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_SHOWNORMAL);
      rc = TRUE;
      break;
    case WM_CLOSE:
//      PostMessage(hWndMenu,WM_COMMAND,ID_DESTROY,(LPARAM)hDlg);
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_DESTROY:
      hWndDOS = 0;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xDOSpos,&yDOSpos);
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
/*
         case ID_FLUSHDOS:
              x = HIWORD((DWORD)GetModuleHandle((LPSTR)(LONG)hInst));
              while (x)
                 {
                  FlushCachedFileHandle(x); // windows 3.1 funktion
                  x = *(LPINT)MAKELONG(6,x);
                 }
              break;
*/
         case ID_REFRESH:
              SetCursor(hCursorWait);
              EnableDlgItem(hDlg,ID_EMSDOS,EMSCheck());
              SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_RESETCONTENT,0,0);

              sel = AllocSelector(GlobalHandleToSel(hInst));
              SetSelectorLimit(sel,0x100);
              wKrnlPSP = (GetSelectorBase(GetKernelPDB())>>4);
              y = SFTFirst(&sftentry,1);
              x = 1;
              while (y && (x < 256))
                 {
                  strncpy(str1,sftentry.name,8);
                  str1[8] = 0;
                  _fstrip(str1,' ');
                  if (strncmp(sftentry.ext,"   ",3))
                     {
                      str[0] = '.';
                      strncpy(str+1,sftentry.ext,3);
                      str[4] = 0;
                      strcat(str1,str);
                     }

                  taskentry.dwSize = sizeof(taskentry);
                  if (hTask = GethTaskFromPSP(sftentry.psp))
                     {
                      TaskFindHandle(&taskentry,hTask);
                      lpPSP = (LPWORD)MAKELONG(0,*(LPWORD)MAKELONG(0x60,hTask));
                     }
                  else
                  if (sftentry.psp == wKrnlPSP)
                     {
                      strcpy(taskentry.szModule,"<kernel>");
                      lpPSP = (LPWORD)MAKELONG(0,GetKernelPDB());
                     }
                  else
                     {
                      strcpy(taskentry.szModule,"<msdos>");
                      strcpy(str3,"?");
                      lpPSP = 0;
                     }
                  if (lpPSP)
                     {
                      j = *(lpPSP+0x19);
                      SetSelectorBase(sel,(DWORD)*(lpPSP+0x1B)*0x10);
                      lpfindex = (LPBYTE)MAKELONG(*(lpPSP+0x1A),sel);
                      pstr = str3;
                      *pstr = 0;
                      for (i=0;i<j;i++)
                          if (*(lpfindex+i) == LOBYTE(sftentry.index))
                             {
                              if (pstr != str3)
                                  *pstr++ = ',';
                              wsprintf(pstr,"%u",i);
                              pstr = pstr + strlen(pstr);
                             }
                     }

                  wsprintf(str,
                           "%u\t%04X\t%s\t%s\t%u\t%s\t%X",
                           sftentry.index,
                           sftentry.psp,
                           (LPSTR)taskentry.szModule,
                           (LPSTR)str1,
                           sftentry.handles,
                           (LPSTR)str3,
                           sftentry.open_mode
                           );

                  SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                  x++;
                  y = SFTNext(&sftentry);
                 }
              SFTClose(&sftentry);

              FreeSelector(sel);
              wVersion = HIWORD(GetVersion());
              wsprintf(str,
                       "Version\t%u.%u\t File Handles\t%d\t",
                       (WORD)HIBYTE(wVersion),
                       (WORD)LOBYTE(wVersion),
                       ++sftentry.index
                      );
              SetDlgItemText(hDlg,ID_STATUS1,str);
              SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETCURSEL,0,0);
              SetCursor(hCursor);
              break;
         case ID_MCBDOS:
              if (hWndMCB)
                  ShowWindow(hWndMCB,SW_RESTORE);
              else
                  hWndMCB = CreateDialogParam(hInst,
                                              MAKEINTRESOURCE(IDD_MCB),
                                              hDlg,
                                              McbDlg,
                                              (LONG)0
                                             );
              break;
         case ID_DEVDOS:
              if (hWndDEV)
                  ShowWindow(hWndDEV,SW_RESTORE);
              else
                  hWndDEV = CreateDialogParam(hInst,
                                              MAKEINTRESOURCE(IDD_DOSDEV),
                                              hDlg,
                                              DevDlg,
                                              (LONG)0
                                             );
              break;
         case ID_EMSDOS:
              if (hWndEMS)
                  ShowWindow(hWndEMS,SW_RESTORE);
              else
                  hWndEMS = CreateDialogParam(hInst,
                                              MAKEINTRESOURCE(IDD_EMS),
                                              hDlg,
                                              EMSDlg,
                                              (LONG)0
                                             );
              break;
         case ID_CDSDOS:
              if (hWndCDS)
                  ShowWindow(hWndCDS,SW_RESTORE);
              else
                  hWndCDS = CreateDialogParam(hInst,
                                              MAKEINTRESOURCE(IDD_CDS),
                                              hDlg,
                                              CDSDlg,
                                              (LONG)0
                                             );
              break;
         case ID_DPBDOS:
              if (hWndDPB)
                  ShowWindow(hWndDPB,SW_RESTORE);
              else
                  hWndDPB = CreateDialogParam(hInst,
                                              MAKEINTRESOURCE(IDD_DPB),
                                              hDlg,
                                              DPBDlg,
                                              (LONG)0
                                             );
              break;
         case ID_XMSDOS:
              if (hWndXMS)
                  ShowWindow(hWndXMS,SW_RESTORE);
              else
                  hWndXMS = CreateDialogParam(hInst,
                                              MAKEINTRESOURCE(IDD_XMS),
                                              hDlg,
                                              XMSDlg,
                                              (LONG)0
                                             );
              break;
         }
      rc = TRUE;
      break;
    }
 return rc;
}
