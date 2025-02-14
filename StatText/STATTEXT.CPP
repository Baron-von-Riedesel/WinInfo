
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "windows.h"
#include "custcntl.h"
#include "user.h"
#include "userw.h"
#include "stattext.h"
#include "resource.h"

extern "C" {
HANDLE __export WINAPI StatusTextInfo(void);
BOOL   __export WINAPI StatusTextStyle(HWND,HANDLE,FARPROC,FARPROC);
WORD   __export WINAPI StatusTextFlags(WORD,LPSTR,WORD);
LONG   __export WINAPI StatusTextWndFn(HWND,UINT,WORD,LONG);
BOOL   __export WINAPI StatusTextDlgFn(HWND,WORD,WORD,LONG);
BOOL   __export WINAPI WEP(WORD);
};

class STATUSTEXT {
   HANDLE hText;            // Handle Fenstertext
   WORD   wChrHgt;          // zeichenhoehe
   WORD   wChrWth;          // zeichenbreite
   HFONT  hFontEx;          // alternativer Font
   DWORD  dwBkColor;        // Farbe
   struct
     {
      unsigned bNewTabs:1;  // tabs neu gesetzt?
     } flags;
   HFONT  hFont;            // Standard Font
   HFONT  hFontAlt;         // alternativer Font
   WORD   wNumTabs;         // Anzahl Tabs
   int    iTabPos[0];       // Tabs
 public:

        STATUSTEXT(HWND,LPCREATESTRUCT);
        ~STATUSTEXT(void);
   int  GetCharHeight(HWND);
   BOOL PaintProc(HWND,HDC,LPPAINTSTRUCT);
   WORD SetText(HWND,LPSTR,WORD);
   WORD SetTextField(HWND,LPSTR,int);
   int  GetFieldLength(PSTR);
   PSTR SearchField(PSTR,int);
   WORD GetText(LPSTR,WORD);
   WORD GetTextLength();
   WORD EraseBkGnd(HWND,HDC);
   void SetTabStops(HWND,WORD,LPINT);
   void SetFont(HFONT,WORD);
   };

typedef STATUSTEXT NEAR * PSTATUSTEXT;

static HPEN   hPenBl  = 0;
static HPEN   hPenWt  = 0;
static HBRUSH hBrush1 = 0;
static HBRUSH hBrush2 = 0;

static WORD  wInitCount  = 0;
static WORD  wCharOfs    = 0;
static BYTE  fRegistered = 0;

static CSTRC szClassName[] = {"statustext"};
static CSTRC szStZeile[] = {"Status\tZeile"};

BOOL __export WINAPI WEP(WORD wCode)
////////////////////////////////////
{
 if (wInitCount)
    {
     DeleteObject(hPenBl);
     DeleteObject(hPenWt);
     DeleteObject(hBrush1);
     DeleteObject(hBrush2);
    }
 return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ diverse routinen fuer dialogbox-editor               บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
HANDLE __export WINAPI StatusTextInfo(void)
{
    HANDLE hObj;
    LPCTLINFO lpctlinfo;

    if (hObj = GlobalAlloc(GMEM_MOVEABLE,sizeof(CTLINFO)))
        if (lpctlinfo = (LPCTLINFO)GlobalLock(hObj))
        {
            lpctlinfo->wVersion = 0x0100;
            lpctlinfo->wCtlTypes = 1;
            lstrcpy(lpctlinfo->szClass,szClassName);
            lstrcpy(lpctlinfo->szTitle,szStZeile);
            lpctlinfo->Type[0].wType   = 0;
            lpctlinfo->Type[0].wWidth  = 100;
            lpctlinfo->Type[0].wHeight = 13;
            lpctlinfo->Type[0].dwStyle = WS_CHILD | STS_SEPARATOR | STS_DEFALTFONT;
            lpctlinfo->Type[0].szDescr[0] = 0;
        }
        else
        {
            GlobalFree(hObj);
            hObj = 0;
        }
    return hObj;
}
BOOL __export WINAPI StatusTextStyle(HWND hWnd, HANDLE hCtlStyle,
                             FARPROC lpstrtoid, FARPROC lpidtostr)
{
    return 0;
}
WORD __export WINAPI StatusTextFlags(WORD flags, LPSTR lpszStr, WORD MaxString)
{
    *lpszStr = 0;
    return 0;
}
BOOL __export WINAPI StatusTextDlgFn(HWND hDlg, WORD wMsg, WORD wParam, LONG lParam)
{
    return 0;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Zeichenhoehe des aktuellen fonts holen                           บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int STATUSTEXT::GetCharHeight(HWND hWnd)
{
    TEXTMETRIC metrics;
    HDC hdc;

    hdc = GetDC(hWnd);
    if (hFont)
        SelectObject(hdc,hFont);
    GetTextMetrics(hdc,&metrics);
    wChrHgt = metrics.tmHeight;
    wChrWth = metrics.tmAveCharWidth + wCharOfs;
    ReleaseDC(hWnd,hdc);
    return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ PaintProc - Paint Routine                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL STATUSTEXT::PaintProc(HWND hWnd,HDC hDC,LPPAINTSTRUCT lpps)
{
    HFONT  hFonts[2];
    WORD   wETMode;
    char   * pstr;
    char   * apstr;
    WORD   i;
    int    pos,npos,ypos;
    WORD   wTabOfs;
    RECT   rect;
    LOGFONT logfont;
    WORD   slen;
    DWORD  style;
    WORD   offs;

    GetClientRect(hWnd,&rect);
    style = GetWindowStyle(hWnd);

    if (style & STS_TRANSPARENT)
    {
        SetBkMode(hDC,TRANSPARENT);
        wETMode = ETO_CLIPPED;
    }
    else
    {
        SetBkColor(hDC,dwBkColor);
        wETMode = ETO_CLIPPED | ETO_OPAQUE;
    }

    SetTextColor(hDC,GetSysColor(COLOR_BTNTEXT));

    if (hFont)
        SelectObject(hDC,hFont);

    if (style & STS_DEFALTFONT)
        if (!hFontAlt)
            hFontAlt = (HFONT)-1;

    hFonts[0] = hFont;
    hFonts[1] = hFontAlt;
    wTabOfs = (wChrWth>1) + (wChrWth>>2);

    offs = (rect.bottom - wChrHgt + 1) >> 2;  /* ueber die zeichenhoehe hinausgehende hoehe erm */
    ypos = offs * 2;
    if (style & STS_SEPARATOR)
        ypos++;

    if (!hText)
        ;
    else
    {
        pstr= (PSTR)LocalLock(hText);
        if (hFonts[0] && hFonts[1])                   /* wenn 2 fonts gesetzt */
        {
            if (hFonts[1] == (HFONT)-1)               /* 2. font holen */
            {
                if (hFontEx == (HFONT)0)
                {
                    GetObject(hFont,sizeof(LOGFONT),(LPSTR)&logfont);
                    if (logfont.lfWeight == 400)
                        logfont.lfWeight = 700;
                    else
                        logfont.lfWeight = 400;
                    hFontEx = CreateFontIndirect(&logfont);
                }
                hFonts[1] = hFontEx;
            }
        }

        rect.top  = ypos;
        rect.bottom = rect.top + wChrHgt;

        for (i=0,pos=0;i < wNumTabs;i++)
        {
            if (hFonts[1])
                SelectObject(hDC,hFonts[i & 1]);
            rect.left = 3+pos;
            rect.right = iTabPos[i] - wTabOfs;
            apstr = pstr;
            if (!pstr)
                slen  = 0;
            else
                if (pstr = strchr(pstr,9))
                {
                    slen = (WORD)(pstr - apstr);
                    pstr++;
                }
                else
                    slen = strlen(apstr);

            ExtTextOut(hDC,3+pos,ypos,wETMode,&rect,(LPSTR)apstr,slen,0);

            pos = iTabPos[i];
        }                     // end for(;;)
        if (*pstr)
            ExtTextOut(hDC,
                       pos,
                       ypos,
                       wETMode,
                       &rect,
                       (LPSTR)pstr,
                       strlen(pstr),
                       0);

        LocalUnlock(hText);
    }

    return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ EraseBkGnd - Fensterhintergrund loeschen             บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD STATUSTEXT::EraseBkGnd(HWND hWnd,HDC hDC)
{
 WORD   i;
 WORD   wTabOfs;
 DWORD  style;
 RECT   rect;
 HBRUSH hBrush;
 int    offs,uoffs,lpos;

  flags.bNewTabs = FALSE;
  style = GetWindowStyle(hWnd);

  if (style & STS_TRANSPARENT)
     {
      if (!(hBrush = (HBRUSH)SendMessage(GetParent(hWnd),
                                 WM_CTLCOLOR,
                                 (WPARAM)hDC,
                                 (LPARAM)MAKELONG(hWnd,CTLCOLOR_STATTEXT)
                                )))
          hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
     }
  else
  if (style & STS_WINDOWBK)
      hBrush = hBrush2;
  else
      hBrush = hBrush1;

  GetClientRect(hWnd,&rect);
  FillRect(hDC,&rect,hBrush);
                                    // die obere Trennlinie hier zeichnen
  if (style & STS_SEPARATOR)
     {
      SelectObject(hDC,hPenBl);
      MoveTo(hDC,0,0);
      LineTo(hDC,rect.right+1,0);
      SelectObject(hDC,hPenWt);
      MoveTo(hDC,0,1);
      LineTo(hDC,rect.right+1,1);
     }

 // rahmen zeichnen
                      // ueber die zeichenhoehe hinausgehende hoehe ermitteln
                      // und das Ergebnis durch 2 dividieren
 offs = (rect.bottom - wChrHgt + 1) >> 2;
 uoffs = rect.bottom - offs;              /* unteren offset (fuer Linie) */

 wTabOfs = (wChrWth>1) + (wChrWth>2);

  if (!(style & STS_NOFRAMES))
      for (i=0,lpos=2;i<wNumTabs;i++)
         {
          if (i & 1)
             {
              SelectObject(hDC,hPenBl);
              MoveTo(hDC,lpos,uoffs);
              LineTo(hDC,lpos,offs+1);
              LineTo(hDC,iTabPos[i] - wTabOfs,offs+1);
              SelectObject(hDC,hPenWt);
              LineTo(hDC,iTabPos[i] - wTabOfs,uoffs);
              LineTo(hDC,lpos,uoffs);
             }
          lpos = iTabPos[i] - wTabOfs;
         }

  return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SetText - Fenstertext setzen                         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD STATUSTEXT::SetText(HWND hWnd, LPSTR lpWndText, WORD wMode)
{
    WORD i;
    DWORD style;
    PSTR pszText;
    RECT rect;
    HDC hDC;

#ifdef _DEBUG
    OutputDebugString("statustext.SetText enter\r\n");
#endif
    i = _fstrlen(lpWndText);
    if (hText)
        LocalFree(hText);
    if (!(hText = LocalAlloc(LMEM_MOVEABLE,i+1)))
        return LB_ERRSPACE;

    pszText = (PSTR)LocalLock(hText);
    _fmemcpy((LPSTR)pszText,lpWndText,i+1);
    LocalUnlock(hText);

    style = GetWindowStyle(hWnd);
                                // bei transparent immer bkndg loeschen
    if (style & STS_TRANSPARENT)
        InvalidateRect(hWnd,0,1);
    else
        if ((!GetUpdateRect(hWnd,&rect,0)) || wMode) {
            hDC = GetDC(hWnd);
            PaintProc(hWnd,hDC,(LPPAINTSTRUCT)0);
            ReleaseDC(hWnd,hDC);
        }
        else
            InvalidateRect(hWnd,0,1);

#ifdef _DEBUG
    OutputDebugString("statustext.SetText exit\r\n");
#endif
    return i;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ GetText - Fenstertext holen                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD STATUSTEXT::GetText(LPSTR lpWndText, WORD wSize)
{
  WORD i;
  PSTR pszText;

  if (hText)
     {
      pszText = (PSTR)LocalLock(hText);
      i = min(strlen(pszText)+1,wSize);
      _fmemcpy(lpWndText,pszText,i);
      LocalUnlock(hText);
      return i;
     }
  else
      return 0;
}

int STATUSTEXT::GetFieldLength(PSTR pStr)
/////////////////////////////////////////
{
 int i;

 for (i=0;(*pStr != 0) && (*pStr != 9);pStr++,i++);

 return i;
}

PSTR STATUSTEXT::SearchField(PSTR pStr, int iTab)
/////////////////////////////////////////////////
{
 while (iTab)
    {
     if (*pStr == 0)
        break;
     if (*pStr == 9)
         iTab--;
     pStr++;
    }
 return pStr;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SetTextField - Text eines Feldes setzen              บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD STATUSTEXT::SetTextField(HWND hWnd, LPSTR lpWndText, int wField)
{
    WORD i,j,k,l;
    DWORD style;
    PSTR pszText,pszText1,pszText2;
    RECT rect;
    HDC hDC;
    HLOCAL xText;

#ifdef _DEBUG
    OutputDebugString("statustext.SetTextField enter\r\n");
#endif
    i = _fstrlen(lpWndText);
    j = 0;
    k = 0;

    if (pszText = (PSTR)LocalLock(hText)) {
        l = strlen(pszText);
        pszText1 = SearchField(pszText,wField);
        j = GetFieldLength(pszText1);
        k = l - j + i + 1;
    }

    if (!k)
        return LB_ERRSPACE;

    if (!(xText = LocalAlloc(LMEM_MOVEABLE,k)))
        return LB_ERRSPACE;

    k = pszText1 - pszText;

    if (pszText2 = (PSTR)LocalLock(xText)) {
        strncpy(pszText2,pszText,k);
        *(pszText2+k) = 0;
        _fstrcat(pszText2,lpWndText);
        strcat(pszText2,pszText1+j);
    }

    LocalUnlock(hText);
    LocalFree(hText);
    hText = xText;
    LocalUnlock(hText);

    style = GetWindowStyle(hWnd);
                                // bei transparent immer bkndg loeschen
    if (style & STS_TRANSPARENT)
        InvalidateRect(hWnd,0,1);
    else
        if ((!GetUpdateRect(hWnd,&rect,0))) {
            hDC = GetDC(hWnd);
            PaintProc(hWnd,hDC,(LPPAINTSTRUCT)0);
            ReleaseDC(hWnd,hDC);
        }
        else
            InvalidateRect(hWnd,0,1);

#ifdef _DEBUG
    OutputDebugString("statustext.SetTextField exit\r\n");
#endif
  return i;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ GetTextLength - Laenge Fenstertext holen             บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD STATUSTEXT::GetTextLength()
{
  WORD rc;

  if (hText)
     {
      rc = strlen((PSTR)LocalLock(hText));
      LocalUnlock(hText);
     }
  else
      rc = 0;

  return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SetTabStops - Tabulatorpositionen setzen             บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void STATUSTEXT::SetTabStops(HWND hWnd,WORD wReqTabs,LPINT lpInt)
{
    WORD i;
    int  j;
    PSTATUSTEXT hObj;
    RECT rect;

#ifdef _DEBUG
    OutputDebugString("statustext.SetTabStops enter\r\n");
#endif
   hObj = this;
   if (wNumTabs != wReqTabs) {
       flags.bNewTabs = TRUE;
       if (hObj = (PSTATUSTEXT)LocalReAlloc((HANDLE)this,
                                            sizeof(STATUSTEXT)+wReqTabs*sizeof(int),
                                            LMEM_MOVEABLE | LMEM_ZEROINIT))
           SetWindowWord(hWnd,0,(WORD)hObj);
       else
           return;
       hObj->wNumTabs = wReqTabs;
   }
   GetClientRect(hWnd,&rect);
   for ( i=0; i < wReqTabs; i++) {
       j = (*(lpInt+i) * (int)hObj->wChrWth) / 4;
       if (j < 0)
           j = rect.right + j;

       if (hObj->iTabPos[i] != j)
           hObj->flags.bNewTabs = TRUE;
       hObj->iTabPos[i] = j;
      }
   if (hObj->flags.bNewTabs)
       InvalidateRect(hWnd,0,1);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SetFont - Fonts setzen                               บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void STATUSTEXT::SetFont(HFONT hNewFont, WORD wMode)
{
#ifdef _DEBUG
    OutputDebugString("statustext.SetFont enter\r\n");
#endif
    if (wMode == 0)
        hFont    = hNewFont;
    else
        hFontAlt = hNewFont;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Konstruktor                                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
STATUSTEXT::STATUSTEXT(HWND hWnd,LPCREATESTRUCT lpCS)
{
    WORD i;
    PSTR pszText;
    DWORD color1;

#ifdef _DEBUG
    OutputDebugString("statustext.statustext enter\r\n");
#endif
    if (!wInitCount) {
        if (LOWORD(GetVersion()) == 3) {
            color1 = RGB(255,255,255);
            wCharOfs = 0;
        } else {
            color1 = GetSysColor(COLOR_BTNHIGHLIGHT);
            wCharOfs = 1;
        }
        hBrush1 = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        hBrush2 = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        hPenWt  = CreatePen(PS_SOLID,1,color1);
        hPenBl  = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
    }

    wInitCount++;
    memset(this,0,sizeof(STATUSTEXT));

  if (lpCS->lpszName) {
      i = _fstrlen(lpCS->lpszName)+1;
      if (hText = LocalAlloc(LMEM_MOVEABLE,i)) {
          pszText = (PSTR)LocalLock(hText);
          _fmemcpy(pszText,lpCS->lpszName,i);
          LocalUnlock(hText);
      }
  }
  GetCharHeight(hWnd);
  SetWindowWord(hWnd,0,(WORD)this);
  if (GetWindowStyle(hWnd) & STS_WINDOWBK)
      dwBkColor = GetSysColor(COLOR_WINDOW);
  else
      dwBkColor = GetSysColor(COLOR_BTNFACE);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Destruktor                                           บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
STATUSTEXT::~STATUSTEXT()
{
 if (hFontEx)
     DeleteObject(hFontEx);
 if (hText)
     LocalFree(hText);
 wInitCount--;
 if (!wInitCount)
    {
     DeleteObject(hPenBl);
     DeleteObject(hPenWt);
     DeleteObject(hBrush1);
     DeleteObject(hBrush2);
    }
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WndProc - Window Methode                             บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
long __export WINAPI StatusTextWndFn(HWND hWnd,unsigned message,WORD wParam,LONG lParam )
{
    PAINTSTRUCT ps;
    PSTATUSTEXT hObj;
    long rc;

    rc = FALSE;

    hObj = (PSTATUSTEXT)GetWindowWord(hWnd,0);

    switch (message)
    {
    case WM_NCCREATE:
        rc = (LONG)new STATUSTEXT(hWnd,(LPCREATESTRUCT)lParam);
        break;
    case WM_DESTROY:
        if (hObj)
            delete hObj;
        break;
    case ST_SETALTFONT:                    /* alternate font setzen */
        hObj->SetFont((HFONT)wParam,1);
        break;
    case ST_SETSEPARATOR:                  /* flag fuer separator setzen */
        SetWindowStyle(hWnd,GetWindowStyle(hWnd) | STS_SEPARATOR);
        break;
    case ST_SETTABSTOPS:
        hObj->SetTabStops(hWnd,wParam,(LPINT)lParam);
        break;
    case WM_GETTEXTLENGTH:
        rc = hObj->GetTextLength();
        break;
    case WM_GETTEXT:
        rc = hObj->GetText((LPSTR)lParam,wParam);
        break;
    case WM_SETTEXT:
        rc = hObj->SetText(hWnd,(LPSTR)lParam,wParam);
        break;
    case ST_SETTEXTFIELD:
        rc = hObj->SetTextField(hWnd,(LPSTR)lParam,wParam);
        break;
    case WM_SETFONT:
        hObj->SetFont((HFONT)wParam,0);
        hObj->GetCharHeight(hWnd);
        break;
    case WM_ERASEBKGND:
        rc = hObj->EraseBkGnd(hWnd,(HDC)wParam);
        break;
    case WM_NCPAINT:
        break;
    case WM_PAINT:
        BeginPaint(hWnd,(LPPAINTSTRUCT)&ps);
        hObj->PaintProc(hWnd,ps.hdc,(LPPAINTSTRUCT)&ps);
        EndPaint(hWnd,(LPPAINTSTRUCT)&ps);
        break;
    default:
        rc = DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ StatusTextRegisterClass                              บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL __export WINAPI StatustextRegisterClass(HINSTANCE hInstance, LPSTR lpstr, WORD mode)
{
    WNDCLASS wc;

    wc.style          = CS_PARENTDC;
    wc.lpfnWndProc    = (WNDPROC)StatusTextWndFn;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = sizeof(PSTATUSTEXT);
    wc.hInstance      = hInstance;
    wc.hIcon          = 0;
    wc.hCursor        = LoadCursor(0,IDC_ARROW);
    wc.hbrBackground  = 0;
    wc.lpszMenuName   = (LPCSTR)0;
    if (lpstr)
        wc.lpszClassName  = lpstr;
    else
        wc.lpszClassName  = szClassName;

    if (!GetClassInfo(hInstance,wc.lpszClassName,&wc))
        return RegisterClass(&wc);
    else
        return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ StatusTextUnregisterClass                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL WINAPI StatustextUnregisterClass(HINSTANCE hInstance,LPSTR lpstr)
{
 if (lpstr)
     return UnregisterClass(lpstr,hInstance);
 else
     return UnregisterClass(szClassName,hInstance);
}
