
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
 
BOOL  WINAPI CheckMessage(MSG *,WORD,HWND);

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT hFontAlt;


////////////////////////////////////////////////////////
// EnvironmentDlg 
////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK EnvironmentDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 char  szStr[128];
 int  i;
 HWND hWnd;
 HGLOBAL hEnv;
 LPSTR lpEnv;
// int  tabpos[1];
 BOOL rc = FALSE;

	switch (message) {
	case WM_COMMAND:
		switch (wParam) {
		case ID_REFRESH:
				
			hEnv = (HGLOBAL)GetWindowWord(hDlg,DWL_USER);
			lpEnv = (LPSTR)MAKELONG(0,hEnv);
			hWnd = GetDlgItem(hDlg,ID_LISTBOX1);			
			ListBox_ResetContent(hWnd);
			if (!(MemoryRead(HIWORD(lpEnv),0,szStr,1))) {
				MessageBox(hDlg,"Zugriff auf Environment nicht möglich",0,MB_OK);
				PostMessage(hDlg,WM_CLOSE,0,0);
				break;
			}	
			while (*lpEnv) {
				i = _fstrlen(lpEnv);
				ListBox_AddString(hWnd,lpEnv);
				lpEnv = lpEnv + i + 1;
			}
			break;
		case IDCANCEL:
		case IDOK:
			PostMessage(hDlg,WM_CLOSE,0,0);
			break;
		}
		rc = TRUE;
		break;
	case WM_CLOSE:
		DestroyWindow(hDlg);
		rc = TRUE;
		break;
	case WM_INITDIALOG:
		SetWindowWord(hDlg,DWL_USER,LOWORD(lParam));

		SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);

		wsprintf(szStr,"Environment %X",LOWORD(lParam));
		SetWindowText(hDlg,szStr);
		ShowWindow(hDlg,SW_NORMAL);
		rc = TRUE;
		break;
	}

	return rc;
}
