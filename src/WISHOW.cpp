
/* resourcen interpretieren */

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

#define COMPONENT 0

extern int fOwnerDrawWarning;

typedef struct tagRESOBJECT {
  WORD    typ;
  WORD    id;
  HGLOBAL handle;
  HMODULE hModule;
  } RESOBJECT;


int WINAPI interruptcallback(void);
extern CATCHBUF cb;

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ XDlg - Dummy Dialogfunktion                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int CALLBACK EXPORTED XDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
 static fOwner = 1;
 RECT rect;
 LPMEASUREITEMSTRUCT lpmi;
 BOOL rc = FALSE;

   switch(message)
     {
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
      case WM_MEASUREITEM:      /* falls ownerdrawn controls */
         if (fOwner)
            {
             if (fOwnerDrawWarning)
                {
                 fOwner = 0;
                 CreateMessage(hDlg,
                               MAKEINTRESOURCE(IDS_ERRSHW1),
                               MAKEINTRESOURCE(IDS_WARNING),
                               MB_OK);
                }
            }
         lpmi = (LPMEASUREITEMSTRUCT)lParam;
         lpmi->itemWidth  = 20;
         lpmi->itemHeight = 20;
         rc = TRUE;
         break;
      case WM_DRAWITEM:      /* falls ownerdrawn controls */
         rc = TRUE;
         break;
      case WM_DELETEITEM:    /* falls ownerdrawn controls */
         rc = TRUE;
         break;
      case WM_INITDIALOG:
         if (GetWindowLong(hDlg,GWL_STYLE) & WS_CHILD)
            {
             GetWindowRect(hDlg,&rect);
             AdjustWindowRect(&rect,WS_OVERLAPPED | WS_CAPTION,0);
             SetWindowPos(GetParent(hDlg),
                          0,0,0,
                          rect.right-rect.left,
                          rect.bottom-rect.top,
                          SWP_NOMOVE | SWP_NOZORDER);
            }
         if (fOwner)
             ShowWindow(hDlg,SW_SHOWNORMAL);
         else
             ShowWindow(hDlg,SW_SHOWNOACTIVATE);
         fOwner = 1;
         rc = TRUE;
         break;
      case WM_CLOSE:
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
     }
 return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ShowResDlg - Resource anzeigen                       บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL CALLBACK EXPORTED ShowResDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
  LPSTR lpstr;
  RESENTRY FAR * lpres;
  RESOBJECT * pres;
  PAINTSTRUCT ps;
  char str[60];
  BYTE volatile fOk;
  HMENU hMenu;
  LPINT lpint;
  HDC hsDC;
  BITMAP bitmap;
  RECT   rect;
//  HRSRC  hRes;
  BOOL   fShow;
  BOOL  rc = FALSE;

 pres    = (RESOBJECT *)GetWindowWord(hDlg,DLGWINDOWEXTRA);

 switch (message)
   {
    case WM_INITDIALOG:
        fShow = TRUE;
        if (!(pres = (RESOBJECT *)LocalAlloc(LMEM_FIXED,sizeof(RESOBJECT)))) {
			PostMessage(hDlg,WM_CLOSE,0,0);
            break;
		}            
        lpres = (RESENTRY FAR *)lParam;
        pres->handle  = lpres->handle;
        pres->id      = lpres->id;
        pres->hModule = lpres->hModule;
        pres->typ     = lpres->typ;
        SetWindowWord(hDlg,DLGWINDOWEXTRA,(WORD)pres);

        switch (pres->typ)
           {
            case GD_CURSOR:  
				pres->handle = LoadCursor(pres->hModule,MAKEINTRESOURCE(pres->id));
				SetWindowPos(hDlg,0,0,0,64,
                           64 + GetSystemMetrics(SM_CYCAPTION),
                           SWP_NOMOVE | SWP_NOZORDER);
				break;
            case GD_ICON:
				pres->handle = LoadIcon(pres->hModule,MAKEINTRESOURCE(pres->id));
				SetWindowPos(hDlg,0,0,0,64,
                           64 + GetSystemMetrics(SM_CYCAPTION),
                           SWP_NOMOVE | SWP_NOZORDER);
				break;
#if COMPONENT
            case GD_ICONCOMPONENT:
				SetWindowPos(hDlg,0,0,0,64,
                           64 + GetSystemMetrics(SM_CYCAPTION),
                           SWP_NOMOVE | SWP_NOZORDER);
				break;
            case GD_CURSORCOMPONENT:
				SetWindowPos(hDlg,0,0,0,64,
                           64 + GetSystemMetrics(SM_CYCAPTION),
                           SWP_NOMOVE | SWP_NOZORDER);
				break;
#endif
            case GD_BITMAP:                /* bitmap */
              if (pres->id < 0x8000)
                  pres->handle = LoadBitmap(pres->hModule,MAKEINTRESOURCE(pres->id));
              else
                  pres->handle = LoadBitmap(pres->hModule,lpres->idname);
              if (pres->handle)
                 {
                  GetObject(pres->handle,sizeof(bitmap),&bitmap);
                  rect.left   = 0;
                  rect.top    = 0;
                  rect.right  = max(bitmap.bmWidth,100);
                  rect.bottom = max(bitmap.bmHeight,100);
                  AdjustWindowRect(&rect,WS_POPUPWINDOW | WS_CAPTION,0);
                  SetWindowPos(hDlg,0,0,0,
                               rect.right - rect.left,
                               rect.bottom - rect.top,
                               SWP_NOMOVE | SWP_NOZORDER);
                 }
              else
                  rc = IDS_ERRSHW6;
              break;
            case GD_DIALOG:
              fOk = TRUE;
              InterruptRegister(0,(FARPROC)interruptcallback);
              if (!Catch(cb)) {
                  if  (lpstr = (LPSTR)LockResource(pres->handle))
                      if (CreateDialogIndirectParam((HINSTANCE)GetModuleDgroup(pres->hModule),
                                                    lpstr,
                                                    GetParent(hDlg),
                                                    XDlg,
                                                    0)) {
                          PostMessage(hDlg,WM_CLOSE,0,0);
                          fShow = FALSE;
                      } else
                          rc = IDS_ERRSHW5;
                  else
                      rc = IDS_ERRSHW3;
              } else {
                  fOk = FALSE;
                  rc = IDS_ERRSHW7;
              }
              
              InterruptUnRegister(0);
              break;
            case GD_MENU:
              if  (lpstr = (LPSTR)LockResource(pres->handle))
                 {
                  SetWindowPos(hDlg,0,0,0,
                              400,
                              80,
                              SWP_NOMOVE | SWP_NOZORDER);
                  hMenu = LoadMenuIndirect(lpstr);
                  SetMenu(hDlg,hMenu);
                 }
              else
                  rc = IDS_ERRSHW3;
              break;
            default:
              rc = IDS_ERRSHW2;
           }

        if  (rc) {
			LoadString(hInst,rc,str,sizeof(str));
            MessageBox(hDlg,str,0,MB_OK);
            pres->typ = 0;
        }
        if (fShow)
            ShowWindow(hDlg,SW_NORMAL);
        rc = TRUE;
        break;
    case WM_CLOSE:
        if (pres) {
	        switch (pres->typ) {
	        case GD_BITMAP:
    	        DeleteObject(pres->handle);
    	        break;
    	    case GD_CURSOR:
    	    	DestroyCursor((HCURSOR)pres->handle);    
			    break;
    	    case GD_ICON:
    	    	DestroyIcon((HICON)pres->handle);    
			    break;
#if COMPONENT
    	    case GD_CURSORCOMPONENT:
    	    case GD_ICONCOMPONENT:
				break;
#endif
			}    
        	LocalFree((HLOCAL)pres);
		}	
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
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
            PostMessage(hDlg,WM_CLOSE,0,0);
        break;
    case WM_PAINT:
        BeginPaint(hDlg,(LPPAINTSTRUCT)&ps);
        switch (pres->typ) {
#if COMPONENT
          case GD_CURSORCOMPONENT:
          case GD_ICONCOMPONENT:
#endif
          case GD_CURSOR:
          case GD_ICON:
            DrawIcon(ps.hdc,0,0,(HICON)pres->handle);
          	break;
			break;
          case GD_BITMAP:     /* bitmap */
            hsDC = CreateCompatibleDC(ps.hdc);
            SelectObject(hsDC,pres->handle);
            BitBlt(ps.hdc,0,0,200,200,hsDC,0,0,SRCCOPY);
            DeleteDC(hsDC);
            break;
        }
        EndPaint(hDlg,(LPPAINTSTRUCT)&ps);
        rc = TRUE;
        break;
   }
 return rc;
}
