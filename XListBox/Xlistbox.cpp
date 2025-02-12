
/* Aenderungen:
   24.11.99: Umstellung auf Visual C 1.52, Copyrightcheck entfernt
    8.12.94: ???
    7.01.04: Umstellung auf .cpp
   27.12.93: XLB_GETWINDOWDATA und XLB_SETWINDOWDATA
   27.12.93: andere Sortprozedur _hqsortlp()
   08.12.93: Unterstuetzung LB_INSERTSTRING, LB_DELETESTRING, LB_GETCOUNT
   31.08.93: NULL2LF-Konvertierung integriert (_hconvert())
   26.05.93: Messages XLB_SAVEBOX und XLB_LOADBOX(Dummy) eingebaut
   26.05.93: bei Message LB_SETCURSEL wird Message LBN_SELCHANGE
             falls LOWORD(lParam) == 0xFFFF synchron an Parent geschickt!
   26.05.93: Extended Listbox Style neu eingefuehrt:
      XLBES_SCROLLNOTIFY: Parent ueber scrollvorgang informieren (ID XLBN_SCROLLNOTIFY)
*/

// #define _TRACE_

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "windows.h"
#include "windowsx.h"
#include "shellapi.h"
#include "toolhelp.h"
#include "custcntl.h"
#include "dragdrop.h"
#include "user.h"
#include "userw.h"
#include "resource.h"
#include "xlistbox.h"
                                /* WS_BORDER umdefinieren (WS_BORDER für Childs ist kritisch) */
#define WS_BORDER WS_DLGFRAME
                                // fuer windows 3.1 anwendungen!
                                // durch subtraction von -1 auch win30 kompat
#if 1
#define GlobalLock(x) (LPVOID)MAKELP((WORD)x-1,0)
#define GlobalUnlock(x)
#endif


#define SORTPROC NEARPROC
                                /* modus fuer invalidaterect */
#define INVALMODE 0
                                /* ausgabeposition */
#define XSTART 4
                                /* caption offset 9.12.99: von 3 auf 4 geไndert */
#define YCAPOFFS 4			
                                /* owner zeichnet und hat normalerweise auch strings */
#define LBS_OWNERDRAW (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE)
                                /* listbox zeichnet, owner liefert strings dynamisch */
#define OWNERHASSTRINGS (style & (XLBS_OWNERHASSTRINGS | XLBS_OWNERHASALL))
#define OWNERHASALL     (style & XLBS_OWNERHASALL)

// #define COPYRIGHTCHK() if (qsum != rqsum) {_asm {_asm leave _asm jmp word ptr ss:[0]}}
                               /* copyright */

extern "C" {
HANDLE  __export WINAPI XListBoxInfo(void);
BOOL    __export WINAPI XListBoxStyle(HWND,HANDLE,FARPROC,FARPROC);
WORD    __export WINAPI XListBoxFlags(WORD,LPSTR,WORD);
LRESULT __export WINAPI XListBoxWndFn(HWND,UINT,WPARAM,LPARAM);
BOOL    __export WINAPI XListBoxDlgFn(HWND,WORD,WORD,LONG);
BOOL    __export WINAPI WEP(WORD);
//extern int qsum;
//extern int rqsum;
};

///////////////////////////////////////////////
// zentrale Klasse XLISTBOX
///////////////////////////////////////////////

class XLISTBOX {
 private:
   HWND   hWnd;                /* window handle */
   WORD   wID;                 /* window id */
   WORD   wNumItems;           /* anzahl strings (=anzahl zeilen insgesamt) */
   WORD   wActItem;            /* aktuell selektierte zeile (oder 0xFFFF) */
   HGLOBAL hStrings;           /* globales handle der strings */
   DWORD  dwStringSize;        /* aktuelle  groesse des stringblocks */
   HGLOBAL hData;              /* globales handle der itemdaten */
   HANDLE hText;               /* window Text */
   WORD   wDataSize;           /* groesstes Item mit 32-Bit-Data */
   WORD   wVPos;               /* aktuelle  oberste "sichtbare" zeile */
   DWORD  dwOffset;            /* offset der "obersten" zeile (wVPos) */
   SORTPROC sortproc;          /* adresse der Sortierroutine (nicht verwendet) */
   HFONT  hFont;               /* aktueller font */
   HFONT  hFontAlt;            /* alternate font */
   WORD   wChrHgt;             /* aktuelle  zeichenhoehe */
   WORD   wChrWdt;             /* aktuelle  zeichenbreite */
   WORD   wSortPos;            /* position in Zeile ab der sortiert werden soll */
   WORD   wNumRows;            /* hoehe des windows in textzeilen */
   WORD   wCaptionHeight;      /* hoehe der ueberschriftzeile (NCAREA) */
   WORD   wExtStyle;           /* extended Window Style */
   WORD   wlrest;              /* restpixelzeilen */
   DWORD  dwWndData;           /* windowspezifische Daten vom user */
   struct
     {
      unsigned caption:1;        // ist caption da?
      unsigned focus:1;          // haben wir den focus?
      unsigned sorted:1;         // sind die strings sortiert?
      unsigned mousecap:1;       // mouse capture ein?
      unsigned shadowborder:1;   // rand mit schatten zeichnen (3D)?
      unsigned vscroll:1;        // vscroll control da?
      unsigned vscrollvisible:1; // vscroll control sichtbar?
      unsigned vscrollrange:1;   // range muss neu gesetzt werden?
      unsigned vscrollpaint:1;   // vscroll muss neu gezeichnet werden
      unsigned hscroll:1;
      unsigned hscrollvisible:1;
      unsigned hscrollrange:1;
      unsigned hscrollpaint:1;
      unsigned rightaligned:1;
      unsigned noinput:1;        // readonly control
//      unsigned integerrows:1;    // nicht verwendet
      unsigned sortdesc;         // abwaerts sortieren?
     } flags;
   WORD   wNumTabs;            /* anzahl tabulatoren */
   WORD   wTabPos[0];          /* tabulatorwerte (dynamisches array) */
 public:
         XLISTBOX(HWND,LPCREATESTRUCT);
         ~XLISTBOX(void);
   int   GetScreenZeile(WORD);
   void  GetXClientRect(LPRECT prect);
   BOOL  GetCharHeight (void);
   int   PaintClient(HDC, LPRECT);
   WORD  SelChange(WORD, WORD);
   int   VertScroll(WORD, WORD);
   void  SetScrollBarPos(WORD,WORD,int);
   DWORD GetStringPos(WORD);
   BOOL  AdjustWindow(void);
   int   ShowFocus(void);
   int   SortStrings(void);
   void  PaintBorder(void);
   void  PaintCaption(void);
   WORD  SetItemData(WORD,DWORD);
   DWORD GetItemData(WORD wParam);
   void  ShowZeile(WORD,int);
   WORD  FindString(LPSTR,WORD,WORD,WORD);
   int   AddString(LPSTR);
   WORD  GetString(WORD,LPSTR);
   WORD  ReplaceString(WORD,LPSTR,WORD);
   void  SetScrollBarRange(WORD,WORD);
   void  ShowVScrollBar(int);
   BOOL  DoKeys(WORD);
   BOOL  DoKeys1(WORD);
   long  DoCommand(unsigned, WORD, LONG);
   BOOL  SetSelLine(UINT,int,int);
   void  InsertDroppedString(HDROP,DWORD,WORD);
   LPSTR SaveLBContent(WORD,LPSTR);
   void  ResetContent(void);
   WORD  AddInsertItem(UINT,WORD,LONG);
   WORD  DeleteString(WORD);
   void  CalcSize(NCCALCSIZE_PARAMS FAR *);
   WORD  SetText(LPSTR);
   LRESULT Dispatch(UINT,WPARAM,LPARAM);
   };

typedef XLISTBOX NEAR * PXLISTBOX;

typedef struct tagSSENTRY {
        HPSTR  lpstr;
        WORD   wLength;
        WORD   wIndex;
       } SSENTRY;

static WORD     wCount = 0;
static HPEN     wpen   = 0;
static HPEN     bpen   = 0;
static HBRUSH   hbrush = 0;
static HBRUSH   hbrush1= 0;
static HBRUSH   hbrush2= 0;
static WORD     wCharw = 0;
static WORD     wCharh = 0;
static WORD     charoffs;
static BYTE     fRegistered = 0;
static CSTRC    szClassName[] = "xlistbox";
static char     szFilename[128];
static COLORREF ccfSysColWindow;
static COLORREF ccfSysColBtnFace;
static COLORREF ccfSysColBtnShadow;

void DeleteBrushes()
{
	if (hbrush) DeleteObject(hbrush);
	if (hbrush1) DeleteObject(hbrush1);
	if (hbrush2) DeleteObject(hbrush2);
	hbrush = hbrush1 = hbrush2 = 0;
}
void CreateBrushes(void)
{
	
	ccfSysColWindow = GetSysColor(COLOR_WINDOW);
	ccfSysColBtnFace = GetSysColor(COLOR_BTNFACE);
	ccfSysColBtnShadow = GetSysColor(COLOR_BTNSHADOW);

	hbrush  = CreateSolidBrush(ccfSysColWindow);
	hbrush1 = CreateSolidBrush(ccfSysColBtnFace);
	hbrush2 = CreateSolidBrush(ccfSysColBtnShadow);
}     
CheckBrushes(void)
{
	if ((ccfSysColWindow != GetSysColor(COLOR_WINDOW))
	|| (ccfSysColBtnFace != GetSysColor(COLOR_BTNFACE))
	|| (ccfSysColBtnShadow != GetSysColor(COLOR_BTNSHADOW))) {
	 	DeleteBrushes();
	 	CreateBrushes();
	}
}

//////////////////////////////
// WEP  
//////////////////////////////
BOOL __export WINAPI WEP(WORD wCode)
{

 if (wCount) {
      DeleteObject(wpen);
      DeleteObject(bpen);
      DeleteBrushes();
 }

 return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ diverse routinen fuer dialogbox-editor (z.Z. dummy)  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
HANDLE __export WINAPI XListBoxInfo(void)
{
 HGLOBAL hMem;
 LPCTLINFO lpctlinfo;

 if (hMem = GlobalAlloc(GMEM_MOVEABLE,sizeof(CTLINFO)))
     if (lpctlinfo = (LPCTLINFO)GlobalLock(hMem))
        {
         lpctlinfo->wVersion = 0x0100;
         lpctlinfo->wCtlTypes = 1;
         lstrcpy(lpctlinfo->szClass,"xlistbox");
         lstrcpy(lpctlinfo->szTitle,"extended Listbox");
         lpctlinfo->Type[0].wType   = 0;
         lpctlinfo->Type[0].wWidth  = 100;
         lpctlinfo->Type[0].wHeight =  60;
         lpctlinfo->Type[0].dwStyle = WS_CHILD | XLBS_STANDARD;
         lpctlinfo->Type[0].szDescr[0] = 0;
        }
     else
        {
         GlobalFree(hMem);
         hMem = 0;
        }
 return hMem;
}
BOOL __export WINAPI XListBoxStyle(HWND hWnd, HANDLE hCtlStyle,
                             FARPROC lpstrtoid, FARPROC lpidtostr)
{
 return 0;
}
WORD __export WINAPI XListBoxFlags(WORD flags, LPSTR lpszStr, WORD MaxString)
{
 *lpszStr = 0;
 return 0;
}
BOOL __export WINAPI XListBoxDlgFn(HWND hDlg, WORD wMsg, WORD wParam, LONG lParam)
{
 return 0;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ konvertieren                                                       บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL _hconvert(HFILE hFile,HPSTR hpStr,DWORD dwSize)
{
 PSTR pStr,pStr2;
 WORD i;

 if (pStr = (PSTR)LocalAlloc(LMEM_FIXED,160))
    {
     while (dwSize)
       {
        if (dwSize > 80L)
            i = 80;
        else
            i = (WORD)dwSize;
        dwSize = dwSize - i;
        for (pStr2=pStr;i;pStr2++,hpStr++,i--)
            if (*hpStr == 0)
               {
                *pStr2 = 0x0D;
                pStr2++;
                *pStr2 = 0x0A;
               }
            else
                *pStr2 = *hpStr;
        _lwrite(hFile,pStr,pStr2-pStr);
       }
     LocalFree((HLOCAL)pStr);
    }
 return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ tatsaechliche client-area ermitteln                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void XLISTBOX::GetXClientRect(LPRECT prect)
{
  GetClientRect(hWnd,prect);
  if (flags.vscrollvisible)
      prect->right = prect->right - GetSystemMetrics(SM_CXVSCROLL);
  if (flags.hscrollvisible)
      prect->bottom = prect->bottom - GetSystemMetrics(SM_CXHSCROLL);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ aus string-index position auf screen ermitteln                     บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int XLISTBOX::GetScreenZeile(WORD zeile)
{
  if (zeile < wVPos)
      return -1;
  else
  if (zeile >= wVPos + wNumRows)
      return -1;
  else
      return zeile - wVPos;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ char height des aktuellen fonts holen                              บ
บ output: hobj->wChrHgt                                              บ
บ         hobj->wChrWdt                                              บ
บ         hobj->wNumRows                                             บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL XLISTBOX::GetCharHeight()
{
  TEXTMETRIC metrics;
  HDC hdc;
  BOOL rc;

  rc = FALSE;
  hdc = GetDC(hWnd);
  if (hFont)
      SelectObject(hdc,hFont);
  GetTextMetrics(hdc,&metrics);
  ReleaseDC(hWnd,hdc);
  if (wChrHgt != metrics.tmHeight)
     {
      wChrHgt = metrics.tmHeight;
      rc = TRUE;
     }
  wChrWdt = metrics.tmAveCharWidth + charoffs;
  return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ AdjustWindow (nach Fontaenderung)                                  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL XLISTBOX::AdjustWindow()
{
  int i;
  RECT rect;
  DWORD style;

  GetXClientRect(&rect);
  if (flags.hscrollvisible)
      i = GetSystemMetrics(SM_CXHSCROLL);
  else
      i = 0;

  wNumRows = (rect.bottom - rect.top - i) / wChrHgt;

  style = GetWindowStyle(hWnd);
  if (style & LBS_NOINTEGRALHEIGHT)
      AdjustClientSize(hWnd,0,0,0);
  else
//  if ((rect.bottom - rect.top - i) % wChrHgt)
     {
      AdjustClientSize(hWnd,0,0,0);
//      flags.integerrows = 1; /* ??? */
//      AdjustClientSize(hWnd,0,wNumRows * wChrHgt + i,1);
     }

  return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ShowFocus                                                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int XLISTBOX::ShowFocus()
{
  RECT rect;
  int scrzeile;

   if (wActItem == 0xFFFF)
       if (!wVPos)
          scrzeile = 0;
       else
          return 0;
   else                                  /* selektierte zeile auf screen? */
   if ((scrzeile = GetScreenZeile(wActItem)) == -1)
       return 0;

   GetXClientRect(&rect);

   rect.top    = scrzeile * wChrHgt;
   rect.bottom = rect.top + wChrHgt;

   InvalidateRect(hWnd,&rect,0);

   return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ cmpelem - fuer sortprozedur                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
//int FAR cmpelem(SSENTRY FAR * lpssentry1,SSENTRY FAR * lpssentry2)
//int cmpelem(SSENTRY FAR * lpssentry1,SSENTRY FAR * lpssentry2)
// {
// return _hstrcmp(lpssentry1->lpstr,lpssentry2->lpstr);
// }
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Sortroutine                                                        บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int XLISTBOX::SortStrings()
{
  HGLOBAL handle,handle2;
  HPBYTE lpstr;
  SSENTRY FAR * lpssentry;
  SSENTRY FAR * lpssentry1;
  WORD i,wLen;
  DWORD FAR * lpdw;
  DWORD FAR * lpdw1;

  if (!(handle = GlobalAlloc(GMEM_MOVEABLE,(DWORD)wNumItems * sizeof(SSENTRY))))
      return 0;
  lpssentry1 = (SSENTRY FAR *)GlobalLock(handle);
  lpstr = (HPBYTE)GlobalLock(hStrings);
                                                           /* stringpointerarray erzeugen */
  for (i=0,lpssentry=lpssentry1;i<wNumItems;i++,lpssentry++)
      {
       wLen = _hstrlen((HPCSTR)lpstr)+1;
       lpssentry->lpstr   = (HPSTR)lpstr + wSortPos;
       lpssentry->wLength = wLen - wSortPos;
       lpssentry->wIndex  = i;
       lpstr = lpstr + wLen;
      }

//  if (!sortproc)
//      sortproc = (SORTPROC)cmpelem;
//  lqsort(lpssentry1,wNumItems,sizeof(SSENTRY),sortproc);
//  lqsorts(lpssentry1,wNumItems,sizeof(SSENTRY),sortproc);
  _hqsortlp(lpssentry1,wNumItems);

                                                   /* strings wurden ueber hilfsarray sortiert */

  if (handle2 = GlobalAlloc(GMEM_MOVEABLE,dwStringSize))
     {
      lpstr  = (HPBYTE)GlobalLock(handle2);
      if (flags.sortdesc)
        for (i=0,lpssentry=lpssentry1+wNumItems-1;i<wNumItems;i++,lpssentry--)
           {
            wLen = lpssentry->wLength + wSortPos;
            _hmemcpy(lpstr,lpssentry->lpstr - wSortPos,wLen);
            lpstr = lpstr + wLen;
           }
      else
        for (i=0,lpssentry=lpssentry1;i<wNumItems;i++,lpssentry++)
           {
            wLen = lpssentry->wLength + wSortPos;
            _hmemcpy(lpstr,lpssentry->lpstr - wSortPos,wLen);
            lpstr = lpstr + wLen;
           }
      GlobalUnlock(handle2);
      GlobalUnlock(hStrings);
      GlobalFree(hStrings);
      hStrings = handle2;

      if (hData)
         {
          if (handle2 = GlobalAlloc(GMEM_MOVEABLE,wNumItems*4))
             {
              lpdw = (LPDWORD)GlobalLock(handle2);
              lpdw1 = (LPDWORD)GlobalLock(hData);
              if (flags.sortdesc)
                for (i=0,lpssentry=lpssentry1+wNumItems-1;i<wNumItems;i++,lpssentry--,lpdw++)
                  *lpdw = *(lpdw1 + lpssentry->wIndex);
              else
                for (i=0,lpssentry=lpssentry1;i<wNumItems;i++,lpssentry++,lpdw++)
                  *lpdw = *(lpdw1 + lpssentry->wIndex);
              GlobalUnlock(handle2);
              GlobalUnlock(hData);
              GlobalFree(hData);
              hData = handle2;
             }
         }
     }
  else
      GlobalUnlock(hStrings);

  GlobalUnlock(handle);
  GlobalFree(handle);

  return 1;
}
//////////////////////////////////////////////////////////////////////
// PaintClient                                                       
//////////////////////////////////////////////////////////////////////
int XLISTBOX::PaintClient(HDC hdc,LPRECT prect)
{
  UINT i,j,width;
  char szStr[128];
  int  pos,xpos;
  UINT len;
  HPSTR lpstr,lpstr2;
  DWORD style,dwOffsetX;
  RECT rect;
  RECT rect1;
  HWND hWndParent;
  COLORREF bkcolor;
  COLORREF txtcolor;
  COLORREF sbkcolor;
  COLORREF stxtcolor;

  style = GetWindowStyle(hWnd);
  hWndParent = GetParent(hWnd);

  if (hFontAlt)
      SelectObject(hdc,hFontAlt);
  else
  if (hFont)
      SelectObject(hdc,hFont);

  if (style & XLBS_PLAST) {
      bkcolor  = GetSysColor(COLOR_BTNFACE);
      txtcolor = GetSysColor(COLOR_BTNTEXT);
      sbkcolor = bkcolor;
      stxtcolor= txtcolor;
  } else {
      bkcolor  = GetSysColor(COLOR_WINDOW);
      txtcolor = GetSysColor(COLOR_WINDOWTEXT);
      sbkcolor = GetSysColor(COLOR_HIGHLIGHT);
      stxtcolor= GetSysColor(COLOR_HIGHLIGHTTEXT);
  }
  SetBkColor(hdc,bkcolor);
  SetTextColor(hdc,txtcolor);

  GetXClientRect(&rect);

  IntersectClipRect(hdc,rect.left,rect.top,rect.right,rect.bottom);

//j = min(wVPos + wNumRows + flags.integerrows,wNumItems);
  j = min(wVPos + wNumRows,wNumItems);
  xpos = XSTART;

  if (style & LBS_OWNERDRAW)
     {
      DRAWITEMSTRUCT distruct;

      pos = 0;
      if (hData)
         {
          lpstr = (HPSTR)GlobalLock(hData);

          distruct.CtlType    = ODT_LISTBOX;
          distruct.CtlID      = wID;
          distruct.itemAction = ODA_DRAWENTIRE;
          distruct.hwndItem   = hWnd;
          distruct.hDC        = hdc;
          distruct.rcItem.left  = xpos;
          distruct.rcItem.right = rect.right;
          for (i=wVPos;i < j;i++,pos = pos + wChrHgt)
             {
              if ((pos + wChrHgt - 1) < prect->top)
                 continue;
              else
              if ((pos >= prect->bottom) || (i>=wNumItems))
                 break;
                                                    /* falls rechter rand vorhanden */
              if (XSTART)
                  TabbedTextOut(hdc,xpos-XSTART,pos,"\t",1,1,
                                (int FAR *)&xpos,0);
              if (i == wActItem)
                  if (flags.focus)
                      distruct.itemState  = ODS_SELECTED | ODS_FOCUS;
                  else
                      distruct.itemState  = ODS_SELECTED;
              else
                 distruct.itemState  = 0;
              distruct.itemID  = i;
              distruct.rcItem.top    = pos;
              distruct.rcItem.bottom = pos + wChrHgt;
              distruct.itemData = *((LPDWORD)lpstr+i);
              SendMessage(hWndParent,WM_DRAWITEM,0,(LONG)(LPSTR)&distruct);
             }
          GlobalUnlock(hData);
         }
     }
  else
     {
      if (OWNERHASSTRINGS)
         {
          SendMessage(hWndParent,
                      WM_COMMAND,
                      wID,
                      MAKELONG(hWnd,XLBN_STARTDRAW));
          if (OWNERHASALL)
             {
              dwOffsetX = dwOffset;
              i = 1;                     /* 0 fuer i vermeiden wg focus */
              j = wNumRows + 1;
             }
          else
             {
              lpstr = (HPSTR)GlobalLock(hData) + wVPos * 4;
              i = wVPos;
             }
         }
      else
         {
          lpstr = (HPSTR)GlobalLock(hStrings) + dwOffset;
          i = wVPos;
         }

      pos = 0;
      if (lpstr)
      for (;(i < j) && (pos < prect->bottom);i++,pos = pos + wChrHgt)
         {
          lpstr2 = lpstr;
          if (OWNERHASSTRINGS)
             if (OWNERHASALL)
                dwOffsetX++;
             else
                lpstr = lpstr + 4;
          else
             {
              len = _hstrlen(lpstr);
              lpstr = lpstr + len + 1;
             }

          if ((pos + wChrHgt - 1) < prect->top)
             continue;

          if (i == wActItem)
             {
              if (style & XLBS_PLAST)
                {
                 xpos++;
                }
              else
                {
                 SetBkColor(hdc,sbkcolor);
                 SetTextColor(hdc,stxtcolor);
                }
             }
                                                    /* fuehrende blanks ausgeben */
          if (XSTART)
              TabbedTextOut(hdc,xpos-XSTART,pos,"\t",1,1,
                            (int FAR *)&xpos,0);

          if (OWNERHASSTRINGS)
              if (OWNERHASALL)
                  lpstr2 = (HPSTR)SendMessage(hWndParent,WM_DRAWITEM,wID,dwOffsetX);
              else
                  lpstr2 = (HPSTR)SendMessage(hWndParent,WM_DRAWITEM,wID,*(LPDWORD)lpstr2);
          else
          if ((DWORD)LOWORD(lpstr2)+len & 0xFFFF0000L)
             {
              _hmemcpy(szStr,lpstr2,sizeof(szStr));
              lpstr2 = szStr;
             }

          if (!lpstr2)
              break;

          if (flags.rightaligned)
              width = (WORD)TabbedTextOutEx(hdc,xpos,pos,lpstr2,len,wNumTabs,
                      (int FAR *)wTabPos,xpos);
          else
              width = (WORD)TabbedTextOut(hdc,xpos,pos,lpstr2,len,wNumTabs,
                      (int FAR *)wTabPos,xpos);

          if (width < prect->right)
              TabbedTextOut(hdc,width+xpos,pos,"\t",1,1,
                            (int FAR *)&prect->right,0);

                                                    /* rahmen fuer selektierte zeile zeichnen */
          if (!flags.noinput)
             {
              if (i == wActItem)
                 {
                  if (style & XLBS_PLAST)
                     {
                      xpos--;
                      rect1.top    = pos;
                      rect1.bottom = rect1.top + wChrHgt;
                      rect1.left   = rect.left;
                      rect1.right  = rect.right;
                      if (flags.vscrollvisible)
                          rect1.right  = rect1.right - 2;
                      MoveTo(hdc,rect1.left,rect1.bottom-1);
                      SelectObject(hdc,bpen);
                      LineTo(hdc,rect1.left,rect1.top);
                      LineTo(hdc,rect1.right-1,rect1.top);
                      SelectObject(hdc,wpen);
                      LineTo(hdc,rect1.right-1,rect1.bottom-1);
                      LineTo(hdc,rect1.left,rect1.bottom-1);
                     }
                  else
                     {
                      SetBkColor(hdc,bkcolor);
                      SetTextColor(hdc,txtcolor);
                     }
                 }
                                                      /* focus zeichnen */
              if ((i == wActItem)
               || ((!i) && (wActItem == 0xFFFF)) )
                  if (flags.focus)
                     {
                      rect1.top    = pos;
                      rect1.bottom = rect1.top + wChrHgt;
                      rect1.left   = rect.left;
                      rect1.right  = rect.right;
                      if (style & XLBS_PLAST)
                         {
                          MoveTo(hdc,rect1.left+2,rect1.bottom-3);
                          SelectObject(hdc,wpen);
                          LineTo(hdc,rect1.left+2,rect1.bottom-5);
                          LineTo(hdc,rect1.left+4,rect1.bottom-5);
                          SelectObject(hdc,bpen);
                          LineTo(hdc,rect1.left+4,rect1.bottom-3);
                          LineTo(hdc,rect1.left+2,rect1.bottom-3);
                         }
                      else
                         {
                          FrameRect(hdc,&rect1,(HBRUSH)GetStockObject(GRAY_BRUSH));
                         }
                     }
             }
         }
      if (OWNERHASSTRINGS)
         {
          SendMessage(hWndParent,WM_COMMAND,wID,MAKELONG(hWnd,XLBN_ENDDRAW));
          if (OWNERHASALL)
              ;
          else
              GlobalUnlock(hData);
         }
      else
          GlobalUnlock(hStrings);
     }

  if (pos < prect->bottom)
     {
      rect.top = pos;
      rect.bottom = prect->bottom;
      if (style & XLBS_PLAST)
          FillRect(hdc,&rect,hbrush1);
      else
          FillRect(hdc,&rect,hbrush);
     }

  return 1;
}
//////////////////////////////////////////////////////////////////////
// PaintBorder (NC-Area malen)                                       
//////////////////////////////////////////////////////////////////////
void XLISTBOX::PaintBorder()
{
 RECT rect;
 RECT rect1;
 int x,y,i;
 HDC  hdc;
 HRGN hrgn;
 DWORD style;

	style = GetWindowStyle(hWnd);
	if ((style & (WS_BORDER | WS_DLGFRAME)) == 0)
		return;
		
	hdc = GetWindowDC(hWnd);
	GetChildWindowRect(hWnd,&rect);
	rect.bottom = rect.bottom - wlrest;

	if (flags.shadowborder) {
		i = GetSystemMetrics(SM_CXDLGFRAME);

		rect1.right  = rect.right;
		rect1.bottom = rect.bottom;

		rect1.left   = i;
		rect1.top    = rect.bottom - i;
		FillRect(hdc,&rect1,hbrush2);
		rect1.left   = rect.right - i;
		rect1.top    = i;
		FillRect(hdc,&rect1,hbrush2);

		rect.right  = rect.right - i;
		rect.bottom = rect.bottom - i;
	}

	if (style & WS_BORDER) {
		i = GetSystemMetrics(SM_CXBORDER);
		if (style & XLBS_PLAST) {
			MoveTo(hdc,rect.left,rect.bottom - i);
			SelectObject(hdc,wpen);
			LineTo(hdc,rect.left,rect.top);
			LineTo(hdc,rect.right - 1,rect.top);
			SelectObject(hdc,GetStockObject(BLACK_PEN));
			LineTo(hdc,rect.right-1,rect.bottom-1);
			LineTo(hdc,rect.left,rect.bottom-1);
		}
		if (style & XLBS_EDGEBORDER) {
#if 0         
			MoveTo(hdc,rect.left,rect.bottom-1);
			SelectObject(hdc,bpen);
			LineTo(hdc,rect.left,rect.top);
			LineTo(hdc,rect.right-1,rect.top);
			SelectObject(hdc,wpen);
			LineTo(hdc,rect.right-1,rect.bottom-1);
			LineTo(hdc,rect.left-1,rect.bottom-1);
#else 
			SelectObject(hdc,bpen);

			MoveTo(hdc,rect.left,rect.bottom - i);        // unten links
			LineTo(hdc,rect.left,rect.top);             // unten links nach oben links
			LineTo(hdc,rect.right,rect.top);            // oben links nach oben rechts


			MoveTo(hdc,rect.left + i,rect.bottom - 2*i - 1);  // unten links
			LineTo(hdc,rect.left + i,rect.top+i);         // unten links nach oben links
			LineTo(hdc,rect.right - 2*i-1,rect.top + i);    // oben links nach oben rechts

			SelectObject(hdc,(HGDIOBJ)wpen);

			MoveTo(hdc,rect.right - i,rect.top);          // oben rechts
			LineTo(hdc,rect.right - i,rect.bottom - i);     // oben rechts nach unten rechts
			LineTo(hdc,rect.left,rect.bottom - i);        // unten rechts nach unten links

			MoveTo(hdc,rect.right - 2*i,rect.top + i);      // oben rechts
			LineTo(hdc,rect.right - 2*i,rect.bottom - 2*i); // oben rechts nach unten rechts
			LineTo(hdc,rect.left + i,rect.bottom - 2*i);    // unten rechts nach unten links

			rect.left++;   // 2 mal
			rect.top++;
			rect.right--;
			rect.bottom--;
#endif
		}
		if (!(style & (XLBS_PLAST | XLBS_EDGEBORDER)))
			FrameRect(hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));

		rect.left++;
		rect.top++;
		rect.right--;
		rect.bottom--;
	}

	if (style & WS_DLGFRAME) {
		hrgn = CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
		x = GetSystemMetrics(SM_CXDLGFRAME);
		if (style & XLBS_PLAST)
			FrameRgn(hdc,hrgn,hbrush1,x,x);
		else
			FrameRgn(hdc,hrgn,hbrush,x,x);
		DeleteObject(hrgn);
	}

	ReleaseDC(hWnd,hdc);
	return;
}
//////////////////////////////////////////////////////////////////////
// PaintCaption - Spaltenüberschriften zeichnen                                                      
//////////////////////////////////////////////////////////////////////
void XLISTBOX::PaintCaption()
{
 HDC hdc;
 PSTR pStr;
 int width;
 DWORD style;
 RECT rect;
 int x,dx,dxe;
 int xoffset;
 int yoffset;
 int dy;
 COLORREF bkcolor;
 COLORREF txtcolor;

  hdc = GetWindowDC(hWnd);
  if (hFont)
      SelectObject(hdc,hFont);
  style = GetWindowStyle(hWnd);
  if (style & XLBS_PLAST) {
      bkcolor  = GetSysColor(COLOR_BTNFACE);
      txtcolor = GetSysColor(COLOR_BTNTEXT);
  } else {
      bkcolor  = GetSysColor(COLOR_WINDOW);
      txtcolor = GetSysColor(COLOR_WINDOWTEXT);
  }

  SetBkColor(hdc,bkcolor);
  SetTextColor(hdc,txtcolor);

  xoffset = XSTART;
  yoffset = 0;
  if (style & WS_BORDER) {
      xoffset = xoffset + GetSystemMetrics(SM_CXBORDER);
      yoffset = yoffset + GetSystemMetrics(SM_CYBORDER);
  }
  
  if (style & WS_DLGFRAME) {
      xoffset = xoffset + GetSystemMetrics(SM_CXDLGFRAME);
      yoffset = yoffset + GetSystemMetrics(SM_CYDLGFRAME);
  }

  if (XSTART) {
      rect.top    = yoffset;
      rect.left   = xoffset - XSTART;
      rect.bottom = rect.top + wChrHgt;
      rect.right  = xoffset;
      if (style & XLBS_PLAST)
          FillRect(hdc,&rect,hbrush1);
      else
          FillRect(hdc,&rect,hbrush);
  }

  GetWindowRect(hWnd,&rect);
  x = rect.right - rect.left - (xoffset - XSTART);
  if (flags.shadowborder)
      x = x - GetSystemMetrics(SM_CXDLGFRAME);

  if (hText && (pStr = (PSTR)LocalLock(hText)))
     {
      if (flags.rightaligned)
          width = (WORD)TabbedTextOutEx(hdc,
                                        xoffset,
                                        yoffset,
                                        pStr,
                                        strlen(pStr),
                                        wNumTabs,
                                        (int FAR *)wTabPos,
                                        xoffset
                                       );
      else
          width = (WORD)TabbedTextOut(hdc,
                                      xoffset,
                                      yoffset,
                                      pStr,
                                      strlen(pStr),
                                      wNumTabs,
                                      (int FAR *)wTabPos,
                                      xoffset
                                     );
      if (width < x)
          TabbedTextOut(hdc,width+xoffset,yoffset,"\t",1,1,
                        (int FAR *)&x,0);
      LocalUnlock(hText);
     }

  if (!(style & XLBS_SHADOWBORDER)) {
      dx = 0;
      dxe = 0;
  } else {
      dx = xoffset;
      dxe = xoffset - XSTART;
  }

	if (style & XLBS_EDGEBORDER) {
		SelectObject(hdc,wpen);
		MoveTo(hdc,dxe + 2,wChrHgt + yoffset);
	} else	{
		SelectObject(hdc,bpen);
		MoveTo(hdc,dxe + 1,wChrHgt + yoffset);
	}	
		
  LineTo(hdc,x+xoffset-dx,wChrHgt + yoffset);
                                                   /* linie in ruhe lassen */
  rect.top    = yoffset + wChrHgt + 1;
  dy = YCAPOFFS - 1;

	if (style & XLBS_EDGEBORDER) {
		MoveTo(hdc,dxe + 2,wChrHgt + yoffset + 1);
  		LineTo(hdc,x+xoffset-dx,wChrHgt + yoffset + 1);
		SelectObject(hdc,bpen);
		MoveTo(hdc,dxe + 2,wChrHgt + yoffset + 2);
  		LineTo(hdc,x+xoffset-dx,wChrHgt + yoffset + 2);
		MoveTo(hdc,dxe + 2,wChrHgt + yoffset + 3);
  		LineTo(hdc,x+xoffset-dx,wChrHgt + yoffset + 3);
		rect.top = rect.top + 3;
		if (dy >= 3)
			dy = dy - 3;
		else
			dy = 0;	
	}

	if (style & XLBS_PLAST) {
		rect.top++;
		dy--;
		SelectObject(hdc,wpen);
		MoveTo(hdc,dxe,wChrHgt + yoffset + 1);
		LineTo(hdc,x+xoffset-dx,wChrHgt + yoffset + 1);
	}
	if (dy > 0) {
		rect.left   = dxe;
		if (style & XLBS_EDGEBORDER)		// 1.6.2002: only increment with EDGEBORDER
			rect.left++;					// 8.12.99: dxe -> dxe + 1
		rect.bottom = rect.top + dy;
		rect.right  = x;    // 8.12.99: x + xoffset - dx -> x + xoffset - dx 
		if (style & XLBS_PLAST)
			FillRect(hdc,&rect,hbrush1);
		else
			FillRect(hdc,&rect,hbrush);
	}
  ReleaseDC(hWnd,hdc);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SetItemData                                                        บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD XLISTBOX::SetItemData(WORD wParam,DWORD lParam)
{
  if (GetWindowStyle(hWnd) & XLBS_OWNERHASSTRINGS)
      ;
  else
  if  (wParam >= wNumItems)
       return (WORD)LB_ERR;

  if  (wParam >= wDataSize)
      {
       if (hData)
           hData = GlobalReAlloc(hData,(wParam+1) * 4,GMEM_MOVEABLE);
       else
           hData = GlobalAlloc(GMEM_MOVEABLE,(wParam+1) * 4);
       wDataSize = wParam + 1;
      }
  if (!hData)
      return (WORD)LB_ERR;

  *((DWORD FAR *)GlobalLock(hData)+wParam) = lParam;
  GlobalUnlock(hData);

 return 0;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ GetItemData                                                        บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
DWORD XLISTBOX::GetItemData(WORD wParam)
{
  LPSTR lpstr;
  DWORD rc;

  if  (wParam >= wDataSize)
      return (DWORD)LB_ERR;
  if (lpstr = (LPSTR)GlobalLock(hData))
     {
      rc = *((DWORD FAR *)lpstr+wParam);
      GlobalUnlock(hData);
     }
  else
      rc = 0;
  return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ShowZeile - zeile ausgeben                                         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void XLISTBOX::ShowZeile(WORD zeile,int mode)
{
  int scrzeile;
  RECT rect;
  HDC hdc;

  if ((scrzeile = GetScreenZeile(zeile)) == -1)
     return;

  GetXClientRect(&rect);
  rect.top    = scrzeile * wChrHgt;
  rect.bottom = rect.top + wChrHgt - 1;
  if (mode)
     {
      hdc = GetDC(hWnd);
      PaintClient(hdc,&rect);
      ReleaseDC(hWnd,hdc);
     }
  else
      InvalidateRect(hWnd,&rect,0);

  return;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SelChange - selektierte Zeile wechselt                             บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
 */
WORD XLISTBOX::SelChange(WORD selzeile,WORD mode) // mode == 2/3: SETCURSEL
{                                                 // mode == 1: SCROLL msgs keys
                                                  // mode == 0: SCROLL msgs mouse
  int scrzeile;
  RECT rect;
  HDC hdc;
  DWORD lParam;
  HWND hWndParent;

   hWndParent = GetParent(hWnd);
   if ((selzeile != 0xFFFF) && (selzeile >= wNumItems))
      {
       if (mode)
           PostMessage(hWndParent,
                       WM_COMMAND,
                       wID,
                       MAKELONG(hWnd,LBN_SELCHANGE)
                      );
       return (WORD)LB_ERR;
      }
   if (mode == 3)
      {
       wActItem = selzeile;
       SendMessage(hWndParent,
                   WM_COMMAND,
                   wID,
                   MAKELONG(hWnd,LBN_SELCHANGE)
                  );
       return wActItem;
      }

   if (flags.noinput)
      {
       wActItem = selzeile;
       return wActItem;
      }

   GetXClientRect(&rect);
                                // screenzeile der alten selektierten zeile
   if (wActItem != 0xFFFF)
      scrzeile = GetScreenZeile(wActItem);
   else
                                // zwar keine selektierte zeile, aber focus da
   if (!wVPos)
      scrzeile = 0;
   else
      scrzeile = -1;

   wActItem = selzeile;
                                // screenpos der alten selektierten zeile
                                // und diese ausgeben sofern sichtbar
   hdc = GetDC(hWnd);
   if (scrzeile != -1)
      {
       rect.top    = scrzeile * wChrHgt;
       rect.bottom = rect.top + wChrHgt;
       if (mode == 2)
           InvalidateRect(hWnd,&rect,INVALMODE);
       else
          {
           PaintClient(hdc,&rect);
           ValidateRect(hWnd,&rect);
          }
      }
                                // und jetzt neue ausgeben
   if (wActItem != 0xFFFF)
      {
       scrzeile = GetScreenZeile(wActItem);
       if (scrzeile == -1)
          {
           if (wActItem < wVPos)
               VertScroll(SB_THUMBPOSITION,wActItem);
           else
               VertScroll(SB_THUMBPOSITION,wActItem - wNumRows + 1);
          }
       else
          {
           rect.top    = scrzeile * wChrHgt;
           rect.bottom = rect.top + wChrHgt;
           if (mode == 2)
               InvalidateRect(hWnd,&rect,INVALMODE);
           else
              {
               PaintClient(hdc,&rect);
               ValidateRect(hWnd,&rect);
              }
          }
      }
   ReleaseDC(hWnd,hdc);
                             // stil xlbs_seltrack verschickt LBN_SELCHANGE
                             // meldungen ohne loslassen der maustaste
   if (mode)
       lParam = MAKELONG(hWnd,LBN_SELCHANGE);
   else
   if (GetWindowStyle(hWnd) & XLBS_SELTRACK)
       lParam = MAKELONG(hWnd,LBN_SELCHANGE);
   else
       lParam = MAKELONG(hWnd,LBN_SELTRACK);
                                         /* parent benachrichtigen */
// PostMessage(hWndParent,
   SendMessage(hWndParent,
               WM_COMMAND,
               wID,
               lParam
              );

   return wActItem;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ GetStringPos - Input: Index,  Output: Offset         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
DWORD XLISTBOX::GetStringPos(WORD index)
{
 HPSTR lpstr,lpstr2;
 WORD i;
 DWORD rc;

  if (GetWindowStyle(hWnd) & LBS_OWNERDRAW)
      rc = 0;
  else
  if (index)
     {
      lpstr = (LPSTR)GlobalLock(hStrings);
      lpstr2 = lpstr;
      lpstr = lpstr + dwOffset;
      if (index < wVPos)
         {
          lpstr = lpstr - 2;
          for (i=wVPos;i > index;i--,lpstr--)
              lpstr = (HPSTR)_hmemchrd(lpstr,0,lpstr - lpstr2);
          lpstr = lpstr + 2;
         }
      else
          for (i=wVPos;i < index;i++,lpstr++)
              lpstr = (HPSTR)_hmemchr(lpstr,0,dwStringSize - dwOffset);

      GlobalUnlock(hStrings);
      rc = lpstr - lpstr2;
     }
  else
      rc = 0;
  return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ FindString - Input: String, Output: Index            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD XLISTBOX::FindString(LPSTR lpFindString,WORD size,WORD wIndex,WORD wType)
{
 HPSTR lpstr1,lpstr2;
 WORD i,j;
 char uc,lc;
 BOOL fFound;

 lpstr1 = (HPSTR)GlobalLock(hStrings) + GetStringPos(wIndex);
 i = wIndex;
 fFound = FALSE;
 if (wType)                 /* soll nur in Key-Feld positioniert werden? */
    {
     uc = lc = *lpFindString;
     AnsiUpperBuff(&uc,1);
     AnsiLowerBuff(&lc,1);
     while (i < wNumItems)
        {
         if ((*(lpstr1 + wSortPos) == uc) || (*(lpstr1 + wSortPos) == lc))
            {
             fFound = TRUE;
             break;
            }
         lpstr1 = lpstr1 + _hstrlen(lpstr1) + 1;
         i++;
        }
    }
 else
     while (i < wNumItems && (!fFound))
        {
         lpstr2 = lpstr1;
         j = _hstrlen(lpstr1);
         lpstr1 = lpstr1 + j + 1;
         while (j)
            if (lpstr2 = (HPSTR)_hmemchri(lpstr2,*lpFindString,j))
               if (!_hmemicmp(lpstr2,lpFindString,size))
                  {
                   fFound = TRUE;
                   break;
                  }
               else
                  {
                   lpstr2++;
                   j = _hstrlen(lpstr2);
                  }
            else
                break;

         i++;
        }

 GlobalUnlock(hStrings);

 if (fFound)
    return i-1;
 else
    return 0xFFFF;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ AddString - String ans Ende der Liste anfuegen       บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int XLISTBOX::AddString(LPSTR lpszStr)
{
 WORD len;

  len = _hstrlen(lpszStr) + 1;
//  if ((DWORD)dwStringSize + len > 0xFFFFL)
//      return LB_ERRSPACE;
  if (!(hStrings = GlobalReAlloc(hStrings,dwStringSize + len,GMEM_MOVEABLE)))
      return LB_ERRSPACE;
  _hmemcpy((HPSTR)GlobalLock(hStrings) + dwStringSize,lpszStr,len);
  GlobalUnlock(hStrings);
  dwStringSize = dwStringSize + len;
  return wNumItems;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ GetString                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD XLISTBOX::GetString(WORD index,LPSTR str)
{
 HPSTR lpstr;
 WORD len;
 if (index >= wNumItems)
    return (WORD)LB_ERR;
 lpstr = (HPSTR)GlobalLock(hStrings) + GetStringPos(index);
 len = _hstrlen(lpstr);
 _hmemcpy(str,lpstr,len+1);
 GlobalUnlock(hStrings);
 return len;
}
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ReplaceString (falls str == NULL, auch DeleteString())บ
บ               (falls wMode == 1, InsertString())      บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD XLISTBOX::ReplaceString(WORD index,LPSTR str,WORD wMode)
{
 DWORD dwOffsetX;
 HPBYTE lpstr;
 WORD  len1,len2;
 DWORD nsize;
                                        /* eingefuegt kann auch bei leerer */
  if (wMode)                            /* Liste werden */
     {
      if (index > wNumItems)
          return LB_ERR;
     }
  else
      if (index >= wNumItems)
          return LB_ERR;

  lpstr = (HPBYTE)GlobalLock(hStrings);
  if (index)
     {
      dwOffsetX = GetStringPos(index);
      lpstr = lpstr + dwOffsetX;
     }
  else
      dwOffsetX = 0;

  if (wMode)
     {
      len1 = _hstrlen(str) + 1;      /* laenge des einzufuegenden strings */
      len2 = 0;
     }
  else
  if (str)
     {
      if (!_hstrcmp(str,(HPSTR)lpstr))      /* wenn strings identisch */
         {                                  /* hier bereits fertig    */
          GlobalUnlock(hStrings);
          return 1;
         }
      len1 = _hstrlen((HPCSTR)str);         /* neue laenge */
      len2 = _hstrlen((HPCSTR)lpstr);       /* alte laenge */
     }
  else
     {
      len1 = 0;                            /* neue laenge */
      len2 = _hstrlen((HPCSTR)lpstr) + 1;  /* alte laenge */
     }

  nsize = dwStringSize + len1 - len2;
  if (nsize > dwStringSize)
     {                                  /* string hat sich vergroessert */
      GlobalUnlock(hStrings);
      hStrings = GlobalReAlloc(hStrings,nsize,GMEM_MOVEABLE);
      lpstr = (HPBYTE)GlobalLock(hStrings) + dwOffsetX;
      _hmemcpyd(lpstr+len1,lpstr+len2,dwStringSize - dwOffsetX - len2);
     }

  if (str)
      _hmemcpy(lpstr,str,len1+1-wMode);

  if (nsize < dwStringSize)
     {
      _hmemcpy(lpstr+len1,lpstr+len2,dwStringSize - dwOffsetX - len2);
      GlobalUnlock(hStrings);
      if (nsize)
          hStrings = GlobalReAlloc(hStrings,nsize,GMEM_MOVEABLE);
      else
          hStrings = GlobalReAlloc(hStrings,1,GMEM_MOVEABLE);
     }
  else
      GlobalUnlock(hStrings);

  dwStringSize = nsize;
  return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SetScrollBarPos                                      บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void XLISTBOX::SetScrollBarPos(WORD type,WORD wParam,int mode)
{
 if (type == SB_VERT)
    {
     if (!flags.vscroll)
         return;
     flags.vscrollpaint = 1;
     if (mode)
        if (SendMessage(GetDlgItem(hWnd,1),SB_SETPOSITION,wParam,1))
            flags.vscrollpaint = 0;
    }
 if (type == SB_HORZ)
    {
     if (!flags.hscroll)
         return;
     flags.hscrollpaint = 1;
     if (mode)
        if (SendMessage(GetDlgItem(hWnd,2),SB_SETPOSITION,wParam,1))
            flags.hscrollpaint = 0;
    }
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SetScrollBarRange (nicht verwendet)                  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void XLISTBOX::SetScrollBarRange(WORD type,WORD wParam)
{
   flags.vscrollrange = 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ShowVScrollBar                                                     บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void XLISTBOX::ShowVScrollBar(int mode)
{
 RECT rect;
 HWND hWndX;
 HINSTANCE hInstance;
 int i;

 if (wNumItems <= wNumRows)
    {
     if (flags.vscrollvisible)
        {
         ShowWindow(GetDlgItem(hWnd,1),SW_HIDE);
         flags.vscrollvisible = 0;
        }
    }
 else
    {
     if (!flags.vscroll)
        {
         flags.vscroll = 1;
         GetClientRect(hWnd,&rect);
         i = GetSystemMetrics(SM_CXVSCROLL);
         hInstance = GetWindowInstance(hWnd);
         hWndX = CreateWindow("scrollbar",0,WS_CHILD | SBS_VERT,
                             rect.right-i,rect.top,i,rect.bottom,
                             hWnd,(HMENU)1,hInstance,(LPSTR)0L);
         flags.vscrollvisible = 0;
         flags.vscrollrange   = 1;
         flags.vscrollpaint   = 1;
         SetScrollBarPos(SB_VERT,wVPos,mode);
        }
     else
         hWndX = GetDlgItem(hWnd,1);
     if (!flags.vscrollvisible)
        {
         flags.vscrollvisible = 1;
         ShowWindow(hWndX,SW_SHOWNOACTIVATE);
        }
     if (flags.vscrollrange)
        {
         flags.vscrollrange = 0;
         SendMessage(hWndX,SB_SETRANGE,0,MAKELONG(0,wNumItems - wNumRows));
//         PrintfAsciiz("Items: %X, Zeilen: %X",wNumItems,wNumRows);
        }
     if (flags.vscrollpaint)
         SetScrollBarPos(SB_VERT,wVPos,mode);
    }
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ VertScroll - vertikal scrollen                       บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int XLISTBOX::VertScroll(WORD wParam,WORD posNew)
{
 int  vpos;
 int  vmax;
 int pagesize;
 RECT rect;
 DWORD style;


    vmax = wNumItems - wNumRows;
    if (vmax < 0)
        vmax = 0;

    pagesize = max(wNumRows - 1,1);

    vpos    = wVPos;

    switch (wParam)
    {
        case SB_LINEUP:
            vpos--;
            break;
        case SB_LINEDOWN:
            vpos++;
            break;
        case SB_PAGEUP:
            vpos = vpos - pagesize;
            break;
        case SB_PAGEDOWN:
            vpos = vpos + pagesize;
            break;
        case SB_TOP:
            vpos = 0;
            break;
        case SB_BOTTOM:
            vpos = vmax;
            break;
        case SB_THUMBPOSITION:
            vpos = posNew;
            break;
        case SB_THUMBTRACK:
            if (!(posNew % 3))
               vpos = posNew;
            else
               return FALSE;
            break;
        default:
            return FALSE;
    }
    if (vpos < 0)
        vpos = 0;
    else
    if (vpos > vmax)
        vpos = vmax;

    GetXClientRect(&rect);
    ScrollWindow(hWnd,0,(wVPos - vpos) * wChrHgt,&rect,&rect);

    if (wParam != SB_THUMBTRACK)
       SetScrollBarPos(SB_VERT,vpos,TRUE);
                                                   /* nicht mehr setzen */
    flags.vscrollpaint = 0;

    style = GetWindowStyle(hWnd);

    dwOffset = GetStringPos(vpos);
    wVPos = vpos;
    if (wExtStyle & XLBES_SCROLLNOTIFY)
        PostMessage(GetParent(hWnd),
                    WM_COMMAND,
                    wID,
                    MAKELONG(hWnd,XLBN_SCROLLNOTIFY)
                   );
    return vpos;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ DoKeys - Keyboardeingaben verarbeiten                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL XLISTBOX::DoKeys(WORD wParam)
{
 DWORD dw;
 int zeile;

    if (flags.noinput)
       {
        switch (wParam)
           {
            case VK_UP:
              wParam = SB_LINEUP;
              break;
            case VK_DOWN:
              wParam = SB_LINEDOWN;
              break;
            case VK_PRIOR:
              wParam = SB_PAGEUP;
              break;
            case VK_NEXT:
              wParam = SB_PAGEDOWN;
              break;
            case VK_HOME:
              wParam = SB_TOP;
              break;
            case VK_END:
              wParam = SB_BOTTOM;
              break;
            default:
              return FALSE;
           }
        VertScroll(wParam,0);
        return TRUE;
       }

    switch (wParam)
        {
        case VK_UP:
            if (!wActItem)
                break;
            if (wActItem == 0xFFFF)
                zeile = 0;
            else
                zeile = wActItem - 1;

            SelChange(zeile,1);
            break;
        case VK_DOWN:
            if (wActItem == 0xFFFF)
                zeile = 0;
            else
                zeile = wActItem + 1;
            if (zeile < (int)wNumItems)
                SelChange(zeile,1);
            break;
        case VK_PRIOR:
            if (!wActItem)
                break;
            zeile = wActItem - wNumRows + 1;
            if (zeile < 0)
                zeile = 0;
            SelChange(zeile,1);
            break;
        case VK_NEXT:
            zeile = wActItem + wNumRows - 1;
            if (zeile >= (int)wNumItems)
                zeile = wNumItems - 1;
            SelChange(zeile,1);
            break;
        case VK_HOME:
            SelChange(0,1);
            break;
        case VK_END:
            SelChange(wNumItems - 1,1);
            break;
        default:
            return FALSE;
        }
    return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ DoKeys1 - weitere Keyboardeingaben verarbeiten       บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL XLISTBOX::DoKeys1(WORD wParam)
{
 WORD i;

 AnsiUpperBuff((LPSTR)&wParam,1);
 if ((wParam >= 'A' && wParam <= 'Z') || (wParam >= '0' && wParam <= '9'))
     if ((i = FindString((LPSTR)&wParam,1,0,1)) != 0xFFFF)
         SelChange(i,1);
 return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ DoCommand - WM_COMMAND verarbeiten                                 บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
long XLISTBOX::DoCommand(UINT message,WORD wParam,LONG lParam)
{

  switch (wParam)
    {
    }
 return 0;
}

// void CopyrightCheck()
//{
// COPYRIGHTCHK();
//}

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SetSelLine - Verarbeitung WM_LBUTTONDOWN,WM_RBUTTONDOWN,           บ
บ              WM_MOUSEMOVE,WM_SYSTIMER messages                     บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL XLISTBOX::SetSelLine(UINT message, int x,int y)
{
  int  zzeile;
  WORD anfselzeile;

  if (!wNumItems)
      return FALSE;
                                    // zeile auf schirm errechnen
                                    // fall 1: y-position negativ -> scrollen
  if (y < 0)
     {
      if (wActItem)
         SelChange(wActItem - 1,0);
     }
  else
     {
      zzeile = y / wChrHgt;
      if ((zzeile >= wNumRows) & (wNumItems > wVPos + wNumRows))
          SelChange(wVPos + wNumRows,0);   // scrollen nach unten
      else
         {
          anfselzeile = min(wVPos + zzeile,wNumItems - 1);
          if ( (message == WM_LBUTTONDOWN)
            || (message == WM_RBUTTONDOWN)
            || (anfselzeile != wActItem) )
               SelChange(anfselzeile,0);
         }
     }
  return TRUE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Drag&Drop Unterstuetzung - String einfuegen                        บ
บ sollte spaeter geaendert werden -> Parent benachrichtigen          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void XLISTBOX::InsertDroppedString(HDROP wParam, DWORD lParam, WORD wMode)
{
  WORD i,j;
  HINSTANCE hInstance;
  LPSTR lpStr;
  DWORD dwMousePos;
  WORD  wYPos;
  char szStr[128];

  dwMousePos = GetMessagePos();
  ScreenToClient(hWnd,(LPPOINT)&dwMousePos);
  wYPos = HIWORD(dwMousePos);
  if (wYPos > 0x7FFF)
      wYPos = 0;
  i = wYPos / wChrHgt + wVPos;
  if (i > wNumItems)
      i = wNumItems;
  if (wMode)
     {
      j = DragQueryFile(wParam,-1,0,0);
      for (;j;j--)
         {
          DragQueryFile(wParam,j-1,szStr,sizeof(szStr));
          SendMessage(hWnd,LB_INSERTSTRING,i,(LONG)(LPSTR)szStr);
         }
      DragFinish(wParam);
     }
  else
     {
      hInstance = GetWindowInstance((HWND)wParam);
      lpStr = (LPSTR)GlobalLock(hInstance) + (WORD)((LPDRAGINFO)lParam)->szList;
      lstrcpy(szStr,lpStr);
      GlobalUnlock(hInstance);
      SendMessage(hWnd,LB_INSERTSTRING,i,(LONG)(LPSTR)szStr);
     }
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Listbox abspeichern                                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
LPSTR XLISTBOX::SaveLBContent(WORD wParam, LPSTR lParam)
{
  LPSTR lpstr;
  HFILE hFile;

  if (lpstr = (LPSTR)GlobalLock(hStrings))
      {
       if (!wParam)
          {
           if (!lParam)
               {
                GetTempFileName(0,"XLB",0,szFilename);
                lParam = szFilename;
               }
           hFile = _lcreat((LPSTR)lParam,0);
          }
       else
          {
           hFile = wParam;
           lParam = 0;
          }

       if (hFile != -1)
          {
//           _hwrite(hFile,lpstr,dwStringSize);
           _hconvert(hFile,lpstr,dwStringSize);
           if (!wParam)
               _lclose(hFile);
          }
       else
          lParam = 0;

       GlobalUnlock(hStrings);
      }
  else
      lParam = 0;

  return lParam;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ResetContent - zuruecksetzen                                       บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void XLISTBOX::ResetContent()
{
  wVPos        = 0;
  dwStringSize = 0;
  wDataSize    = 0;
  dwOffset     = 0;
  wNumItems    = 0;
  wActItem     = 0xFFFF;
  GlobalReAlloc(hStrings,1,GMEM_MOVEABLE);
  if (hData)
      GlobalReAlloc(hData,1,GMEM_MOVEABLE);
  flags.vscrollpaint = 1;
  flags.vscrollrange = 1;
  InvalidateRect(hWnd,0,1);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ AddInsertItem - zeile einfuegen/anhaengen                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD XLISTBOX::AddInsertItem(UINT message,WORD wParam,LONG lParam)
{
  WORD  rc;
  DWORD style;
  RECT rect;

   style = GetWindowStyle(hWnd);
   if (message == LB_ADDSTRING)
      {
       flags.sorted = 0;
       if (style & LBS_HASSTRINGS)
           rc = AddString((LPSTR)lParam);
       else
       if (style & LBS_OWNERDRAW)
           rc = SetItemData(wParam,lParam);
       else
           rc = AddString((LPSTR)lParam);
       wParam = wNumItems;
      }
   else       // LB_INSERTSTRING
      {
       if (!flags.sorted)
           {
            flags.sorted = 1;
            if (style & LBS_SORT)
                SortStrings();
           }
       if (wParam == 0xFFFF)
           wParam = wNumItems;
       if (style & LBS_HASSTRINGS)
           rc = ReplaceString(wParam,(LPSTR)lParam,1);
       else
       if (style & LBS_OWNERDRAW)
           rc = SetItemData(wParam,lParam);
       else
           rc = ReplaceString(wParam,(LPSTR)lParam,1);
      }
   if ((rc == LB_ERRSPACE) || (rc == LB_ERR))
       return rc;

   wNumItems++;
   flags.vscrollrange = 1;
   if (wActItem != 0xFFFF)
       if (wParam <= wActItem)
//           SelChange(wActItem+1,3); // nicht notwendig
           wActItem++;

   if (wParam >= wVPos)
      {
       if (wParam < wVPos + wNumRows)
          {
           GetXClientRect(&rect);
           rect.top    = (wParam - wVPos) * wChrHgt;
           InvalidateRect(hWnd,&rect,0);
          }
      }
   else
      {
       dwOffset = GetStringPos(wVPos + 1);
       wVPos++;
      }
   return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ DeleteString - zeile loeschen                                      บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD XLISTBOX::DeleteString(WORD wParam)
{
  WORD rc;
  DWORD style;
  RECT rect;

    if (!wNumItems)
        return 0;
    style = GetWindowStyle(hWnd);
    if (style & LBS_HASSTRINGS)
        rc = ReplaceString(wParam,0,0);
    else
    if (style & LBS_OWNERDRAW)
        rc = SetItemData(wParam,0);
    else
        rc = ReplaceString(wParam,0,0);
    if (rc != LB_ERR)
       {
        wNumItems--;
        flags.vscrollrange = 1;
        if (wParam >= wVPos)
           {
            if (wParam < wVPos + wNumRows)
               {
                GetXClientRect(&rect);
                rect.top    = (wParam - wVPos) * wChrHgt;
                InvalidateRect(hWnd,&rect,0);
               }
           }
        else
           {
            dwOffset = GetStringPos(wVPos - 1);
            wVPos--;
           }

        if (wActItem != 0xFFFF)
            if (wParam < wActItem)
//                SelChange(wActItem-1,3); // keine msg
                wActItem--;
            else
            if (wParam == wActItem)
                SelChange(0xFFFF,3);
        rc = wNumItems;
       }
    else
        rc = 0;

    return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ CalcSize -                                                         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void XLISTBOX::CalcSize(NCCALCSIZE_PARAMS FAR * lpnccsp)
{
  DWORD style;
  WORD offset,i,j;
  char str[80];
  HFONT xFont;

    if (!hFont)
        hFont = (HFONT)SendMessage(GetParent(hWnd),WM_GETFONT,0,0);

    style = GetWindowStyle(hWnd);
    offset = 0;
    i = GetSystemMetrics(SM_CXDLGFRAME);
    j = GetSystemMetrics(SM_CYDLGFRAME);
    if (style & WS_BORDER)
        offset = offset + GetSystemMetrics(SM_CXBORDER);
    if (style & WS_DLGFRAME)
        offset = offset + i;
    lpnccsp->rgrc[0].left   = lpnccsp->rgrc[0].left   + offset;
    lpnccsp->rgrc[0].right  = lpnccsp->rgrc[0].right  - offset;
    lpnccsp->rgrc[0].top    = lpnccsp->rgrc[0].top    + offset;
    lpnccsp->rgrc[0].bottom = lpnccsp->rgrc[0].bottom - offset;
/*
    if (style & WS_VSCROLL)
       {
        offset = GetSystemMetrics(SM_CXVSCROLL);
        lpnccsp->rgrc[0].right = lpnccsp->rgrc[0].right - offset;
       }
*/
    GetCharHeight();
    if (flags.caption)
        lpnccsp->rgrc[0].top = lpnccsp->rgrc[0].top + wChrHgt + YCAPOFFS;
#ifdef _TRACE_
    wsprintf(str,"wChrHgt: %X",wChrHgt);
    DEBOUT(str);
#endif
    if (flags.shadowborder)
       {
        lpnccsp->rgrc[0].right  = lpnccsp->rgrc[0].right  - i;
        lpnccsp->rgrc[0].bottom = lpnccsp->rgrc[0].bottom - j;
       }
    if (flags.hscrollvisible)
        i = GetSystemMetrics(SM_CXHSCROLL);
    else
        i = 0;

    wNumRows = (lpnccsp->rgrc[0].bottom - lpnccsp->rgrc[0].top - i) / wChrHgt;
    if (style & LBS_NOINTEGRALHEIGHT)
       ;
    else
    if (wlrest = (lpnccsp->rgrc[0].bottom - lpnccsp->rgrc[0].top - i) % wChrHgt)
       {
        lpnccsp->rgrc[0].bottom = lpnccsp->rgrc[0].bottom - wlrest;
       }

    if (flags.vscroll)
       {
        i = GetSystemMetrics(SM_CXVSCROLL);
        MoveWindow(GetDlgItem(hWnd,1),
                   lpnccsp->rgrc[0].right - i,
                   0,
                   i,
// geaendert am 10.11.93     lpnccsp->rgrc[0].bottom,
                   lpnccsp->rgrc[0].bottom - lpnccsp->rgrc[0].top,
                   1
                  );
        flags.vscrollrange = 1;            /* range kann veraendert sein */
        ShowVScrollBar(1);
       }
     wCaptionHeight = lpnccsp->rgrc[0].top;
//   rc = DefWindowProc(hWnd, message, wParam, lParam);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SetText                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD XLISTBOX::SetText(LPSTR lpStr)
{
  WORD j;
  PSTR pszText;

   j = _hstrlen(lpStr);
   if (hText)
      {
       LocalFree(hText);
       hText = 0;
      }
   flags.caption = FALSE;
   if (!j)
       return 0;

   if (!(hText = LocalAlloc(LMEM_MOVEABLE,j+1)))
       return LB_ERRSPACE;

   flags.caption = TRUE;
   pszText = (PSTR)LocalLock(hText);
   _hmemcpy((LPSTR)pszText,lpStr,j+1);
   LocalUnlock(hText);
   PaintCaption();
   return j;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ XLISTBOX Konstruktor                                               บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
XLISTBOX::XLISTBOX(HWND hWndUser,LPCREATESTRUCT lpCS)
{
  TEXTMETRIC metrics;
  PSTR  pszText;
  WORD  j;
  DWORD style;
  HDC   hdc;
  COLORREF color1;

  TRACEOUT("entry konstruktor xlistbox");

	if (!wCount) {
		if (LOWORD(GetVersion()) == 3)
			{
			charoffs = 0;
			color1 = RGB(255,255,255);
			}
		else
			{
			charoffs = 1;
			color1 = GetSysColor(COLOR_BTNHIGHLIGHT);
			}

//     GetCopyRightName();
		hdc = GetDC(0);
		GetTextMetrics(hdc,&metrics);
		ReleaseDC(0,hdc);
		wCharh  = metrics.tmHeight;
		wCharw  = metrics.tmAveCharWidth + charoffs;

		CreateBrushes();
		wpen    = CreatePen(PS_SOLID,1,color1);
		bpen    = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
	}
	wCount++;

  memset(this,0,sizeof(XLISTBOX));
  hWnd     = hWndUser;
  wActItem = 0xFFFF;
  wID      = GetWindowWord(hWnd,GWW_ID);
  wChrHgt  = wCharh;
  wChrWdt  = wCharw;
  style    = GetWindowStyle(hWnd);
  
  if (style & XLBS_SHADOWBORDER)
      flags.shadowborder = 1;
  if (style & XLBS_NOINPUT)
      flags.noinput = 1;
  hStrings = GlobalAlloc(GMEM_MOVEABLE,0);
  SetWindowWord(hWnd,0,(WORD)this);
  if (lpCS->lpszName)
     {
      j = _hstrlen(lpCS->lpszName);
      if (j && (hText = LocalAlloc(LMEM_MOVEABLE,j+1)))
         {
          flags.caption = 1;
          pszText = (PSTR)LocalLock(hText);
          _hmemcpy(pszText,(HPVOID)lpCS->lpszName,j+1);
          LocalUnlock(hText);
         }
     }
//  CopyrightCheck();

  TRACEOUT("exit konstruktor xlistbox");

}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ~XLISTBOX Destruktor                                               บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
XLISTBOX::~XLISTBOX()
{
  TRACEOUT("entry destruktor xlistbox");

  GlobalFree(hStrings);
  if (hData)
      GlobalFree(hData);
  if (hText)
      LocalFree(hText);
  wCount--;
  SetWindowWord(hWnd,0,0);
  if (!wCount)
     {
      DeleteObject(wpen);
      DeleteObject(bpen);
      DeleteObject(hbrush);
      DeleteObject(hbrush1);
      DeleteObject(hbrush2);
     }
  TRACEOUT("exit destruktor xlistbox");
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Dispatch - Dispatcher                                              บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
LRESULT XLISTBOX::Dispatch(UINT message,WPARAM wParam,LPARAM lParam)
{
  PAINTSTRUCT ps;
  PXLISTBOX   hObj;
  LOGFONT     lf;
  PSTR  pszText;
  WORD  i,j,offset;
  HDC   hdc;
  long  rc;
  LPSTR lpstr;
  LPINT lpint;
  DWORD style;
  POINT point;

    rc = 0;

    switch (message)
       {
        case WM_NCCALCSIZE:
            TRACEOUT("WM_NCCALCSIZE received");
            CalcSize((NCCALCSIZE_PARAMS FAR *)lParam);
            break;
        case WM_GETTEXTLENGTH:
            if (hText)
               {
                pszText = (PSTR)LocalLock(hText);
                rc = strlen(pszText);
                LocalUnlock(hText);
               }
            break;
        case WM_GETTEXT:
            if (hText)
               {
                pszText = (PSTR)LocalLock(hText);
                j = min(strlen(pszText)+1,wParam);
                _hmemcpy((LPSTR)lParam,(LPSTR)pszText,j);
                LocalUnlock(hText);
                rc = j;
               }
            break;
        case WM_SETTEXT:
            rc = SetText((LPSTR)lParam);
            break;
        case WM_WINDOWPOSCHANGED:
            break;
        case WM_ERASEBKGND:
//            rc = DefWindowProc(hWnd, message, wParam, lParam);
            break;
        case WM_NCPAINT:
			CheckBrushes();
            PaintBorder();
            if (flags.caption)
                PaintCaption();
//            rc = DefWindowProc(hWnd, message, wParam, lParam);
            break;
        case WM_PAINT:
            style = GetWindowStyle(hWnd);
                                                    /* muss zuvor sortiert werden? */
           if (!flags.sorted)
              {
               flags.sorted = 1;
               if (style & LBS_SORT)
                   SortStrings();
              }
            ShowVScrollBar(1);
            hdc = BeginPaint(hWnd,(LPPAINTSTRUCT)&ps);
            PaintClient(ps.hdc,&ps.rcPaint);
            EndPaint(hWnd,(LPPAINTSTRUCT)&ps);
            break;
        case WM_GETDLGCODE:
            rc = DLGC_WANTARROWS;   /* direction keys selber verarbeiten */
//            rc = DLGC_WANTCHARS | DLGC_WANTARROWS;
            break;
        case WM_SETREDRAW:          /* zur kompatibilitaet mit "listbox" */
            break;
        case WM_GETFONT:
            rc = (LONG)hFont;
            break;
        case WM_SETFONT:
            TRACEOUT("WM_SETFONT received");
            hFont = (HFONT)wParam;
            GetCharHeight();
            AdjustWindow();
            break;
        case XLB_SETFONTALT:
            hFontAlt = (HFONT)wParam;
            break;
        case WM_COMMAND:
            rc = DoCommand(message,wParam,lParam);
            break;
        case WM_SETFOCUS:
            flags.focus = 1;
            ShowFocus();
            PostMessage(GetParent(hWnd),
                        WM_COMMAND,
                        wID,
                        MAKELONG(hWnd,LBN_SETFOCUS)
                       );
            break;
        case WM_KILLFOCUS:
            flags.focus = 0;
            ShowFocus();
            PostMessage(GetParent(hWnd),
                        WM_COMMAND,
                        wID,
                        MAKELONG(hWnd,LBN_KILLFOCUS)
                       );
            break;
//      case WM_SYSTIMER:
        case WM_TIMER:
            GetCursorPos(&point);
            ScreenToClient(hWnd,&point);
            lParam = MAKELONG(point.x,point.y);
            wParam = MK_LBUTTON;
        case WM_MOUSEMOVE:
            if (!flags.mousecap)
                break;
            if (!(wParam & MK_LBUTTON))
                break;
            SetSelLine(message,LOWORD(lParam),HIWORD(lParam));
            break;
// neu 23.12.93
        case WM_QUERYDROPOBJECT:
            if (wExtStyle & XLBES_DROPACCEPT)
                rc = TRUE;
            else
                rc = DefWindowProc(hWnd, message, wParam, lParam);
            break;
        case WM_DROPOBJECT:
            if (wExtStyle & XLBES_DROPOBJECT)
               {
                InsertDroppedString((HDROP)wParam,lParam,0);
                rc = TRUE;
               }
            else
                rc = DRAG_FILE;
            break;
// ende
// neu 19.12.93
        case WM_DROPFILES:
            InsertDroppedString((HDROP)wParam,lParam,1);
            PostMessage(GetParent(hWnd),WM_COMMAND,wID,
                        MAKELONG(hWnd,XLBN_DROPFILES)
                       );
            break;
// ende
        case WM_LBUTTONDOWN:
// neu 18.12.93
            if ((wParam & MK_RBUTTON) && (wExtStyle & XLBES_DRAGDETECT))
               {
                if (wActItem == 0xFFFF)
                    break;
               }
            else
               if (!(SetSelLine(message,LOWORD(lParam),HIWORD(lParam))))
                   break;

            if (wExtStyle & XLBES_DRAGDETECT)
               {
                if ((wParam & MK_RBUTTON) || DragDetect(hWnd,(LPPOINT)&lParam))
                    SendMessage(GetParent(hWnd),WM_BEGINDRAG,wParam,lParam);
                else
                    PostMessage(hWnd,WM_LBUTTONUP,wParam,lParam);
               }
            else
// ende
            if (!flags.mousecap)
               {
                flags.mousecap = TRUE;
                SetCapture(hWnd);
//                SetSystemTimer(hWnd,1,100,0);
                SetTimer(hWnd,1,100,0);
               }

            break;
        case WM_LBUTTONUP:
            if (!(GetWindowStyle(hWnd) & XLBS_SELTRACK))
                PostMessage(GetParent(hWnd),
                            WM_COMMAND,
                            wID,
                            MAKELONG(hWnd,LBN_SELCHANGE)
                           );
            if (flags.mousecap)
               {
                ReleaseCapture();
                flags.mousecap = FALSE;
//              KillSystemTimer(hWnd,1);
                KillTimer(hWnd,1);
               }
            break;
        case WM_LBUTTONDBLCLK:
//            style = GetWindowStyle(hWnd);
//            if (style & LBS_NOTIFY)
                PostMessage(GetParent(hWnd),
                            WM_COMMAND,
                            wID,
                            MAKELONG(hWnd,LBN_DBLCLK)
                           );
            break;
        case WM_MOUSEACTIVATE:
            SetFocus(hWnd);
            rc = DefWindowProc(hWnd, message, wParam, lParam);
            break;
        case WM_RBUTTONDOWN:
            if (wParam & MK_LBUTTON)
                break;
            if (wExtStyle & XLBES_RBUTTONTRACK)
                SetSelLine(message,LOWORD(lParam),HIWORD(lParam));
            SendMessage(GetParent(hWnd),WM_COMMAND,wID,MAKELONG(hWnd,XLBN_RBUTTONDOWN));
            break;
        case WM_SHOWWINDOW:
            if (!wParam)
               if (flags.vscroll)
                 {
                  DestroyWindow(GetDlgItem(hWnd,1));
                  flags.vscroll = 0;
                 }
            rc = DefWindowProc(hWnd, message, wParam, lParam);
            break;
        case WM_CHAR:
            style = GetWindowStyle(hWnd);
            if (style & LBS_WANTKEYBOARDINPUT)
                SendMessage(GetParent(hWnd),WM_CHARTOITEM,wParam,MAKELONG(hWnd,wActItem));
//            DoKeys1(wParam);
            break;
        case WM_KEYDOWN:
            style = GetWindowStyle(hWnd);
            if (style & LBS_WANTKEYBOARDINPUT)
                if ((SendMessage(GetParent(hWnd),WM_VKEYTOITEM,wParam,MAKELONG(hWnd,wActItem))) == -2)
                    break;
//            if (wParam == VK_SPACE)
//               {
//                SendMessage(GetParent(hWnd),WM_COMMAND,wID,MAKELONG(hWnd,XLBN_RBUTTONDOWN));
//                break;
//               }
            DoKeys(wParam);
            break;
        case WM_VSCROLL:
            VertScroll(wParam,LOWORD(lParam));
            break;
        case LB_RESETCONTENT:
            ResetContent();
            break;
        case LB_INSERTSTRING:
        case LB_ADDSTRING:
            rc = AddInsertItem(message,wParam,lParam);
            break;
        case XLB_ADDMULTSTRINGS:
            style = GetWindowStyle(hWnd);
                           /* bei ownerdraws wird kein string mitgegeben */
                           /* sondern dword wie LB_SETITEMDATA */
            if (style & LBS_HASSTRINGS)
               ;
            else
            if (style & LBS_OWNERDRAW)
               {
                flags.sorted = 0;
                wNumItems = wNumItems + wParam;
                rc = SetItemData(wNumItems-1,0);
                flags.vscrollrange = 1;
               }
            break;
        case LB_DELETESTRING:              /* item data muss noch versorgt werden */
            rc = DeleteString(wParam);
            break;
        case XLB_REPLACESTRING:            /* entspricht LB_SETTEXT */
            style = GetWindowStyle(hWnd);
            if (style & LBS_HASSTRINGS)
                rc = ReplaceString(wParam,(LPSTR)lParam,0);
            else
            if (style & LBS_OWNERDRAW)
                rc = SetItemData(wParam,lParam);
            else
                rc = ReplaceString(wParam,(LPSTR)lParam,0);
            if (rc)
               ShowZeile(wParam,0);
            break;
        case XLB_CHANGECURPOS:                        /* wenn an listenanfang eingefuegt wird */
            offset = wParam - wVPos;
            wVPos = wParam;
            if (wActItem != 0xFFFF);
                wActItem = wActItem + offset;
            SetScrollBarPos(SB_VERT,wVPos,1);
            break;
        case LB_GETTEXT:
            style = GetWindowStyle(hWnd);
            if (style & LBS_HASSTRINGS)
                rc = GetString(wParam,(LPSTR)lParam);
            else
            if (style & LBS_OWNERDRAW)
                *(DWORD FAR *)lParam = GetItemData(wParam);
            else
                rc = GetString(wParam,(LPSTR)lParam);
            break;
        case LB_GETITEMDATA:
            rc = GetItemData(wParam);
            break;
        case LB_SETITEMDATA:
            rc = SetItemData(wParam,lParam);
            break;
        case LB_GETCURSEL:
            rc = wActItem;
            break;
        case LB_GETCOUNT:
            rc = wNumItems;
            break;
        case LB_SETCURSEL:
//            rc = SelChange(wParam,1);
            if (LOWORD(lParam) == 0xFFFF)
                rc = SelChange(wParam,3);
            else
                rc = SelChange(wParam,2);
            break;
        case LB_SETTABSTOPS:
            hObj = this;
            if (wParam != wNumTabs)
               {
                offset = sizeof(XLISTBOX) + wParam * sizeof(int);
                if (hObj = (PXLISTBOX)LocalReAlloc((HANDLE)this,offset,LMEM_ZEROINIT | LMEM_MOVEABLE))
                   {
                    hObj->wNumTabs = wParam;
                    SetWindowWord(hObj->hWnd,0,(WORD)hObj);
                   }
                else
                    break;
               }
            for (i=0,lpint=(LPINT)lParam;i<hObj->wNumTabs;i++,lpint++)
               {
                hObj->wTabPos[i] = ((*lpint & 0x7FFF) * hObj->wChrWdt) / 4 + (*lpint & 0x8000);
                if (hObj->wTabPos[i] & 0x8000)
                    hObj->flags.rightaligned = TRUE;
               }
            break;
        case XLB_GETWINDOWDATA:
            rc = dwWndData;
            break;
        case XLB_SETWINDOWDATA:
            rc = dwWndData;
            dwWndData = lParam;
            break;
        case XLB_GETLINES:
            rc = wNumRows;
            break;
        case XLB_SETSORTSTART:
            style = GetWindowStyle(hWnd);
            if (wParam == 0xFFFF)
              {
               style = style & (~LBS_SORT);
               wSortPos = 0;
              }
            else
              {
               style = style | LBS_SORT;
               wSortPos = wParam;
              }
            flags.sorted = FALSE;
            SetWindowLong(hWnd,GWL_STYLE,style);
            wActItem = 0xFFFF;
            wVPos    = 0;
            dwOffset = 0;
            flags.vscrollpaint = 1;
            break;
        case XLB_SETSORTPROC:
            sortproc = (SORTPROC)lParam;
            break;
        case XLB_SETSORTDIR:
            flags.sortdesc = wParam;
            break;
        case XLB_SETEXTSTYLE:
            wExtStyle = wParam;
            if (LOWORD(lParam))
                SendMessage(hWnd,XLB_SETFONTALT,LOWORD(lParam),0);
            break;
        case XLB_SORT:
            SortStrings();
            wActItem = 0xFFFF;
            wVPos    = 0;
            dwOffset = 0;
            flags.vscrollpaint = 1;
        case XLB_SETSORTFLAG:
            flags.sorted = 1;
            break;
        case LB_FINDSTRING:
            rc = FindString((LPSTR)lParam,_hstrlen((LPSTR)lParam),wParam+1,0);
            break;
        case XLB_SAVEBOX:
            rc = (LONG)SaveLBContent(wParam,(LPSTR)lParam);
            break;
        case XLB_LOADBOX:
            break;
        case XLB_GETCAPHEIGHT:
            rc = wCaptionHeight;
            break;
        case LB_GETTOPINDEX:
            rc = wVPos;
            break;
        case LB_SETTOPINDEX:
            VertScroll(SB_THUMBPOSITION,wParam);
            break;
        default:
            rc = DefWindowProc(hWnd, message, wParam, lParam);
            break;
       }
    return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WndProc - Windowroutine                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
LRESULT __export WINAPI XListBoxWndFn(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
  PXLISTBOX hObj;

//  OutputMonoString("XlistboxWndFn\r\n");

  if (message == WM_NCCREATE)
      return (LONG)new XLISTBOX(hWnd,(LPCREATESTRUCT)lParam);
  else
  if (hObj = (PXLISTBOX)GetWindowWord(hWnd,0))
     {
      if (message == WM_DESTROY)
         {
          delete hObj;
          return 0;
         }
      else
          return hObj->Dispatch(message,wParam,lParam);
     }
  else
      return DefWindowProc(hWnd, message, wParam, lParam);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ XListBoxRegisterClass                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL __export WINAPI XListBoxRegisterClass(HINSTANCE hInstance, LPSTR lpstr, WORD mode)
{
    WNDCLASS wc;
    BOOL rc;

//	OutputMonoString("XlistboxRegisterClass\r\n");

    wc.style = CS_PARENTDC | CS_DBLCLKS;

    if (mode)
        if (fRegistered && (!lpstr))
            return TRUE;
        else
           {
            wc.style = CS_GLOBALCLASS | CS_PARENTDC | CS_DBLCLKS;
            hInstance = GetExePtr((HGLOBAL)LOWORD(GlobalHandle(GetMyDS())));
           }

    wc.lpfnWndProc   = (WNDPROC)XListBoxWndFn;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(PXLISTBOX);
    wc.hInstance     = hInstance;
    wc.hIcon         = 0;
    wc.hCursor       = LoadCursor(0,IDC_ARROW);
    wc.lpszMenuName  = (LPCSTR)NULL;
    wc.hbrBackground = 0;
    if (lpstr)
       wc.lpszClassName = lpstr;
    else
       wc.lpszClassName = szClassName;

    rc = RegisterClass(&wc);

    if (rc && mode && (!lpstr))
        fRegistered = 1;

    return rc;
}
