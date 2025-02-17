
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

int xAtompos,yAtompos = 100;
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ AtomDlg - Atom Dialog                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK AtomDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 char  str[100];
 char  str1[80];
 int   i,j,x,y;
 HWND  hWnd;
 HGLOBAL hSegm;
 LPINT lpint;
 LPATOMENTRY lpatomentry;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      SetWindowLong(hDlg,DLGWINDOWEXTRA,lParam);
      LoadTabs(IDUS_53,str);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
      LoadTabs(IDUS_52,str);
      SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      SetWindowPos(hDlg,0,xAtompos,yAtompos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_SHOWNORMAL);
      rc = TRUE;
      break;
    case WM_CLOSE:
      PostMessage(GetParent(hDlg),WM_COMMAND,ID_DESTROY,(LPARAM)(LPVOID)hDlg);
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xAtompos,&yAtompos);
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_REFRESH:
              hWnd  = GetDlgItem(hDlg,ID_LISTBOX1);
              hSegm = (HANDLE)GetWindowWord(hDlg,DLGWINDOWEXTRA);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);
              if (!IsValidLocalHeap(hSegm))
                 break;
              lpint = (LPINT)MAKELONG(8,GlobalHandleToSel(hSegm));
              if (!BasedLocalLock((HGLOBAL)HIWORD(lpint),(HLOCAL)*lpint))
                 break;
              BasedLocalUnlock((HGLOBAL)HIWORD(lpint),(HLOCAL)*lpint);
              lpint = (LPINT)MAKELONG(*lpint,HIWORD(lpint));
              for (i=*lpint,lpint++,x=0,y=1;i;i--,lpint++,y++)
                 if (*lpint)
                    {
                     lpatomentry = (LPATOMENTRY)MAKELONG(*lpint,HIWORD(lpint));
                     do
                        {
                         x++;
                         j = min(sizeof(str1)-1,lpatomentry->length);
                         _fmemcpy(str1,
                                  (LPSTR)MAKELONG(lpatomentry->string,HIWORD(lpint)),
                                  j);
                         str1[j] = 0;
                         wsprintf(str,
                                  "%u\t%04X\t%04X\t%04X\t%s",
                                  y,
                                  0xC000+(LOWORD(lpatomentry)>>2),
                                  LOWORD(lpatomentry),
                                  lpatomentry->count,
                                  (LPSTR)str1
                                 );
                         SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                         lpatomentry = (LPATOMENTRY)MAKELONG(lpatomentry->pNext,HIWORD(lpint));
                        }
                     while (LOWORD(lpatomentry));
                    }  // end if
              wsprintf(str,"Atome\t%u\tEntries\t%u\t",x,y-1);
              SetDlgItemText(hDlg,ID_STATUS1,str);
              break;
         }
      rc = TRUE;
      break;
   }
 return rc;
}
