
//#define STRICT
               
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "custcntl.h"
#include "toolhelp.h"
#include "user386.h"
#include "user.h"
#include "userw.h"
#include "hexdump.h"
#include "resource.h"

extern "C" {
HANDLE __export WINAPI HexdumpInfo(void);
BOOL   __export WINAPI HexdumpStyle(HWND,HANDLE,FARPROC,FARPROC);
WORD   __export WINAPI HexdumpFlags(WORD,LPSTR,WORD);
BOOL   __export WINAPI HexdumpDlgFn(HWND,UINT,WPARAM,LPARAM);
BOOL   __export WINAPI WEP(WORD);
}

extern CSTRC szClassName[];

////////////////////////////////////////////////////////
// diverse routinen fuer dialogbox-editor (veraltet)   
////////////////////////////////////////////////////////
HANDLE __export WINAPI HexdumpInfo(void)
{
 HANDLE hGlobal;
 LPCTLINFO lpctlinfo;

 if (hGlobal = GlobalAlloc(GMEM_MOVEABLE,sizeof(CTLINFO)))
     if (lpctlinfo = (LPCTLINFO)GlobalLock(hGlobal))
        {
         lpctlinfo->wVersion = 0x0100;
         lpctlinfo->wCtlTypes = 1;
         lstrcpy(lpctlinfo->szClass,szClassName);
         lstrcpy(lpctlinfo->szTitle,szClassName);
         lpctlinfo->Type[0].wType   = 0;
         lpctlinfo->Type[0].wWidth  = 100;
         lpctlinfo->Type[0].wHeight =  60;
         lpctlinfo->Type[0].dwStyle = WS_CHILD;
         lpctlinfo->Type[0].szDescr[0] = 0;
        }
     else
        {
         GlobalFree(hGlobal);
         hGlobal = 0;
        }
 return hGlobal;
}
BOOL __export WINAPI HexdumpStyle(HWND hWnd, HANDLE hCtlStyle,
                             FARPROC lpstrtoid, FARPROC lpidtostr)
{
 return 0;
}
WORD __export WINAPI HexdumpFlags(WORD flags, LPSTR lpszStr, WORD MaxString)
{
 *lpszStr = 0;
 return 0;
}
BOOL __export WINAPI HexdumpDlgFn(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 return 0;
}
BOOL __export WINAPI WEP(WORD wCode)
{
 return 1;
}
