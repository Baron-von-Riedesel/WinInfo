
// User/Gdi Status anzeigen

// #define _TRACE_

#include "stdlib.h"
#include "stddef.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "wininfo.h"
#include "wininfox.h"

#define strcmp lstrcmp
#define strlen lstrlen

//BOOL CALLBACK DisDrvDlg(HWND ,UINT,WPARAM,LPARAM);

// globale variable

HWND   hWndWinStat = 0;
int xStatpos  = 100;
int yStatpos  = 100;

typedef struct WINSTATSTRUCT {
  HMODULE hUser;
  WORD    wUserTotal;
  WORD    wUserFree;
  HMODULE hGDI;
  WORD    wGDITotal;
  WORD    wGDIFree;
  DWORD   dwFree;
  WORD    wTasks;
  HWND    hWnd;
  HTASK   hTask;
  HINSTANCE hInstance;
 } WINSTATSTRUCT;

// statische variable

static WNDPROC fpWinStatWndProc;
static char szMonText[20];
static BOOL bIconPainted = FALSE;

static WINSTATSTRUCT ws;

static DDENTRY fstr[] = {
                  0,offsetof(WINSTATSTRUCT,hUser),      "%04X",DDT_WORD,
                  0,offsetof(WINSTATSTRUCT,wUserTotal), "%u",  DDT_WORD,
                  0,offsetof(WINSTATSTRUCT,wUserFree),  "%u",  DDT_WORD,
                  0,offsetof(WINSTATSTRUCT,hGDI),       "%04X",DDT_WORD,
                  0,offsetof(WINSTATSTRUCT,wGDITotal),  "%u",  DDT_WORD,
                  0,offsetof(WINSTATSTRUCT,wGDIFree),   "%u",  DDT_WORD,
                  0,offsetof(WINSTATSTRUCT,dwFree),     "%u kB",DDT_WORD,
                  0,offsetof(WINSTATSTRUCT,wTasks),     "%X",  DDT_WORD,
                  0,offsetof(WINSTATSTRUCT,hTask),      "%04X",DDT_WORD,
                  0,offsetof(WINSTATSTRUCT,hInstance),  "%04X",DDT_WORD,
                  0,offsetof(WINSTATSTRUCT,hWnd),       "%04X",DDT_WORD,
                  (PSTR)0};

static STRLOADENTRY CODESEG strloadtab[] = {
                       &fstr[0].name,  IDS_XWIN00,
                       &fstr[1].name,  IDS_XWIN01,
                       &fstr[2].name,  IDS_XWIN02,
                       &fstr[3].name,  IDS_XWIN03,
                       &fstr[4].name,  IDS_XWIN04,
                       &fstr[5].name,  IDS_XWIN05,
                       &fstr[6].name,  IDS_XWIN06,
                       &fstr[7].name,  IDS_XWIN07,
                       &fstr[8].name,  IDS_XWIN08,
                       &fstr[9].name,  IDS_XWIN09,
                       &fstr[10].name, IDS_XWIN10,
                       (PSTR *)(0)};


static DDENTRY ddtab[] = {
                  "X DlgBaseUnits",                    0x00,"%u",DDT_WORD,
                  "Y DlgBaseUnits",                    0x02,"%u",DDT_WORD,
                  (PSTR)0};

static int dditab[10];

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ OutputIconic                                         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
static void OutputIconic(HWND hWnd,HDC hDC, WINSTATSTRUCT FAR * lpws)
{
    char str[80];
    HBRUSH hBrush,oldBrush;
    WORD wProz1;
    TEXTMETRIC tm;
    RECT rect;

    bIconPainted = TRUE;
    GetClientRect(hWnd,&rect);
    hBrush = CreateSolidBrush(GetSysColor(COLOR_BACKGROUND));
    oldBrush = (HBRUSH)SelectObject(hDC,hBrush);
    Rectangle(hDC,0,0,rect.right,rect.bottom);
    SetBkMode(hDC,TRANSPARENT);
    //SetMapMode( hDC, MM_TEXT );

    GetTextMetrics(hDC,&tm);

    if (lpws->wUserTotal)
        wProz1 = MulDivWords(lpws->wUserFree,100,lpws->wUserTotal);
    else
        wProz1 = 0;
    wsprintf(str,"%u%%",wProz1);
    TextOut(hDC,2,0,str,strlen(str));

    if (lpws->wGDITotal)
        wProz1 = MulDivWords(lpws->wGDIFree,100,lpws->wGDITotal);
    else
        wProz1 = 0;
    wsprintf(str,"%u%%",wProz1);
    TextOut(hDC,2,tm.tmHeight-2,str,strlen(str));

    SelectObject(hDC,oldBrush);
    DeleteObject(hBrush);

#ifdef _TRACE_
    wsprintf(str,"%u %u %u %u %u %u",
             lpws->wUserFree,
             lpws->wUserTotal,
             lpws->wGDIFree,
             lpws->wGDITotal,
             MulDiv(lpws->wUserFree,100,lpws->wUserTotal),
             MulDiv(lpws->wGDIFree,100,lpws->wGDITotal)
            );
#endif
    TRACEOUT(str);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WinStatWndProc - window subclass                     บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
LRESULT EXPORTED CALLBACK WinStatWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    PAINTSTRUCT ps;
    LRESULT rc;

    rc = FALSE;

    switch (message) {
    //case WM_ICONERASEBKGND:
    //    break;
    case WM_ERASEBKGND:
        rc = 1;
        break;
    case WM_SYSCOMMAND:
        if (wParam == SC_MINIMIZE)
            SetWindowText(hWnd,"User/GDI");
        else if (wParam == SC_RESTORE)
            SetWindowText(hWnd,szMonText);

        rc = CallWindowProc(fpWinStatWndProc,hWnd,message,wParam,lParam);
        break;
    case WM_PAINT:
        if (!IsIconic(hWnd)) {
            rc = CallWindowProc(fpWinStatWndProc,hWnd,message,wParam,lParam);
            break;
        }
        /* fall thru */
    case WM_PAINTICON:
        BeginPaint(hWnd,&ps);
        OutputIconic(hWnd,ps.hdc,&ws);
        EndPaint(hWnd,&ps);
        break;
    default:
        rc = CallWindowProc(fpWinStatWndProc,hWnd,message,wParam,lParam);
        break;
    }
    return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WinStatusDlg - User/GDI Status Dialog                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK WinStatusDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
    char str[80];
    char str1[80];
    int x;
    DWORD ul;
    // WORD tx;
    DDENTRY * pddentry;
    int tabpos[2];
    BOOL rc = FALSE;

    switch (message)  {
    case WM_INITDIALOG:
        hWndWinStat = hDlg;
        GetWindowText(hDlg,szMonText,sizeof(szMonText));
        fpWinStatWndProc = (WNDPROC)GetWindowLong(hDlg,GWL_WNDPROC);
        SetWindowLong(hDlg,GWL_WNDPROC,(LONG)(WNDPROC)WinStatWndProc);
        if (!fstr[0].name)
            LoadStringTab(hInst,strloadtab);
        SetTimer(hDlg,1,2000,0);
        PostMessage(hDlg,WM_TIMER,0,0);
        for (x=0;x<11;x++) {
            tabpos[0] = 60;
            tabpos[1] = 110;
            SendDlgItemMessage(hDlg,x + ID_STATS,ST_SETTABSTOPS,2,(LPARAM)(LPVOID)tabpos);
            SendDlgItemMessage(hDlg,x + ID_STATS,ST_SETALTFONT,-1,0);
        }
        SetWindowPos(hDlg,0,xStatpos,yStatpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
        ShowWindow(hDlg,SW_NORMAL);
        rc = TRUE;
        break;
    case WM_DESTROY:
        KillTimer(hDlg,1);
        hWndWinStat = 0;
        break;
    case WM_CLOSE:
        DestroyWindow(hDlg);
        rc = TRUE;
        break;
    case WM_PARENTNOTIFY:
        rc = TRUE;
        break;
    case WM_COMMAND:
        switch (wParam) {
        case IDCANCEL:
            PostMessage(hDlg,WM_CLOSE,0,0);
            break;
        }
        rc = TRUE;
        break;
    case WM_MOVE:
        SaveWindowPos(hDlg,&xStatpos,&yStatpos);
        break;
    case WM_TIMER:
        ws.hGDI = GetModuleHandle("GDI");          // GDI handle
        ul = GetHeapSpaces(ws.hGDI);
        ws.wGDITotal = HIWORD(ul);                 // GDI total
        ws.wGDIFree  = LOWORD(ul);                 // GDI frei
        ws.hUser = GetModuleHandle("USER");        // USER Handle
        ul = GetHeapSpaces(ws.hUser);
        ws.wUserTotal = HIWORD(ul);                // USER total
        ws.wUserFree  = LOWORD(ul);                // USER frei
        ws.dwFree = GetFreeSpace(0);               // Global frei
        ws.dwFree = ws.dwFree>>10;
        ws.wTasks = GetNumTasks();                 // Tasks
        ws.hWnd  = GetActiveWindow();              // aktuelles Fenster
        ws.hTask = GetWindowTask(ws.hWnd);         // aktuelle Task
        ws.hInstance = GetInstanceFromTask(ws.hTask);

        if (IsIconic(hDlg)) {
            InvalidateRect(hDlg,0,1);
            if (bIconPainted == FALSE) {
                wsprintf(str,"U/G=%u%%/%u%%",
                         ws.wUserTotal ? MulDivWords(ws.wUserFree,100,ws.wUserTotal ) : 0,
                         ws.wGDITotal  ? MulDivWords(ws.wGDIFree,100,ws.wGDITotal) : 0 );
                SetWindowText(hDlg,str);
            }
        } else
            for(pddentry = fstr,x=0;pddentry->name;pddentry++,x++) {
                xsprintf(pddentry,str1,(LPSTR)&ws);
                wsprintf(str,"%s\t%s",
                         (LPSTR)pddentry->name,
                         (LPSTR)str1
                        );
                GetDlgItemText(hDlg,x+ID_STATS,str1,sizeof(str1));
                if (strcmp(str,str1))
                    SetDlgItemText(hDlg,x+ID_STATS,str);
            }

        rc = TRUE;
        break;
    }
    return rc;
}
