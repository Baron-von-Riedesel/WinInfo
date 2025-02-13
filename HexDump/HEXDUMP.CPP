
// Achtung: Byte alignment auf 1 setzen
// kritische Funktionen bspw: GetDescriptor()
               
//#define STRICT
               
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
//#include "custcntl.h"
#include "commdlg.h"
#include "toolhelp.h"
#include "user386.h"
#include "user.h"
#include "userw.h"
#include "hexdump.h"
#include "resource.h"
#include "conmenu.h"

#define XPOS 0

extern "C" {
long   __export WINAPI HexdumpWndFn(HWND,UINT,WPARAM,LPARAM);
extern NEAR _WINFLAGS;
}

typedef struct tagHDFLAGS
     {
      unsigned words:1;         // 01 format Words
      unsigned dwords:1;        // 02 format DWords
      unsigned strings:1;       // 04 format Strings
      unsigned reserved1:1;		// 08
      unsigned reserved2:1;		// 10
      unsigned reserved3:1;		// 20
      unsigned reserved4:1;		// 40
      unsigned reserved5:1;		// 80
      unsigned usestdcontextmenu:1;		// 100
     } HDFLAGS;

class HEXDUMP {
public:
   WORD   wID;                 // window id 
   WORD   font;                // aktueller font
   WORD   charh;               // aktuelle  zeichenhoehe 
   WORD   charw;               // aktuelle  zeichenbreite 
   DWORD  dwVPos;              // aktuelle  oberste zeile
   DWORD  vmax;                // anzahl zeilen insgesamt (Limit/ (Bytes/Zeile))
   HWND   hWnd;
   BYTE   sFac;                // schiebefaktor 
   WORD   anzZeilen;           // hoehe des windows in textzeilen 
   FWORD  fAddr;                // lineare adresse 
   DWORD  dwMaxSize;
   DWORD  dwViewOffset;         // nur fuer Anzeige "Adresse"
   WORD   wType;
   WORD   whSize;              // Size horizontal in Bytes 
   DWORD  bkcolor;
   DWORD  fgcolor;
   union {
     HDFLAGS flags;
     WORD    wflags;
    };
	HEXDUMP(HWND hWndSrc, WPARAM wParam, LPARAM lParam);
	~HEXDUMP();
	BOOL PaintProc (HDC, LPRECT);
	int  VertScroll(WORD, WORD);
	void DoKeys    (WORD);
	DWORD SearchString(LPSTR lpFindString,FWORD fwPtr,DWORD dwLaenge);
	void PaintBorder(DWORD style);
	void SetSizeParms();
	int  GetCharHeight();
	LRESULT OnSave(WPARAM wParam, LPARAM lParam);
	LRESULT OnSaveEx();
	LRESULT OnResetContent(WPARAM wParam, LPARAM lParam);
	int OnSetMode(WPARAM wParam);
   };

typedef HEXDUMP __near * PHEXDUMP;


static WORD   wCount = 0;
static int    tabposb[18] = {0};
static int    tabposw[12] = {0};
static int    tabposd[7] = {0};
static int    tabposs[2] = {0};
static COLORREF color1;
static COLORREF color2;
static chkorr   = 1;             /* korrektur fuer charh */
static charoffs = 0;
static BYTE fRegistered = 0;

CSTRC szClassName[] = "Hexdump";
static CSTRC szFStr1[]  = "%.8lX:\t";
static CSTRC szFStr2[]  = "%08lX\t%08lX\t%08lX\t%08lX\t";
static CSTRC szFStr3[]  = "%04X\t%04X\t%04X\t%04X\t%04X\t%04X\t%04X\t%04X\t";
static CSTRC szFStr4[]  = "%.8lX:\t";
static CSTRC szHDS[]    = "HDS";

static CATCHBUF cb;
static HINSTANCE hAppInstance;

// CS ermitteln

static WORD GetCS()
{
 WORD rc;
 _asm {mov rc,cs}
return rc;
}

int __export interruptcallback(void)
{
    _asm {
        pusha
        push es
        mov ax,[bp+6+2] // interrupt nummer
        cmp ax,13       // gp fault?
        jz  exc13
        cmp ax,14
        jnz myexit
    exc13:
        push ds
        push offset cb
        push ax
        call Throw
    myexit:
        pop es
        popa
    }
    return 0;
}
////////////////////////////////////////////////////////
// ValidateSegment                                      
////////////////////////////////////////////////////////
static WORD ValidateSegment(HANDLE hSegm)
{
 int i = 1;
 WORD hSel;
 LPBYTE lpByte = 0;

 hSel = GlobalHandleToSel(hSegm);
 if (!IsValidAddress((LPVOID)MAKELONG(0,hSel)))
     return 0;

 return hSel;
}
////////////////////////////////////////////////////////
// VertScroll - vertikal scrollen                      
////////////////////////////////////////////////////////
int HEXDUMP::VertScroll(WORD wParam,WORD posNew)
{
 LONG  i;
 LONG  vpos;
 LONG  lVMax;
 RECT  rect;

    if (vmax <= anzZeilen) // zurück falls alle zeilen in window passen
        return 0;

    vpos = dwVPos;

    lVMax = vmax - anzZeilen;

    switch (wParam) {
    case SB_LINEUP:
            vpos--;
            break;
    case SB_LINEDOWN:
            vpos++;
            break;
    case SB_PAGEUP:
            vpos = vpos - anzZeilen;
            break;
    case SB_PAGEDOWN:
            vpos = vpos + anzZeilen;
            break;
    case SB_TOP:
            vpos = 0;
            break;
    case SB_BOTTOM:
            vpos = lVMax;
            break;
    case SB_THUMBPOSITION:
            vpos = (LONG)posNew<<sFac;
            break;
    case SB_THUMBTRACK:
            if (!(posNew % 3))
               vpos = (LONG)posNew<<sFac;
            else
               return FALSE;
            break;
    default:
            return FALSE;
    }
    if (vpos < 0)
        vpos = 0;
    else
    if (vpos > lVMax)
        vpos = lVMax;

    GetClientRect(hWnd,&rect);
    i = dwVPos - vpos;
    dwVPos = vpos;
    if (wParam != SB_THUMBTRACK)
       SetScrollPos(hWnd,SB_VERT,(int)(dwVPos>>sFac),TRUE);

    if (i > 4 || i < -4)
       InvalidateRect(hWnd,&rect,0);
    else {
       ScrollWindow(hWnd,0,(int)i * charh,&rect,&rect);
       UpdateWindow(hWnd);
    }
 return 1;
}
////////////////////////////////////////////////////////
// DoKeys - Keyboardeingaben verarbeiten                
////////////////////////////////////////////////////////
void HEXDUMP::DoKeys(WORD wParam)
{
    switch (wParam) {
        case VK_UP:
            VertScroll(SB_LINEUP,0);
            break;
        case VK_DOWN:
            VertScroll(SB_LINEDOWN,0);
            break;
        case VK_PRIOR:
            VertScroll(SB_PAGEUP,0);
            break;
        case VK_NEXT:
            VertScroll(SB_PAGEDOWN,0);
            break;
        case VK_HOME:
            VertScroll(SB_TOP,0);
            break;
        case VK_END:
            VertScroll(SB_BOTTOM,0);
            break;
        }
}
////////////////////////////////////////////////////////
// SearchString - String in Memoryblock suchen          
////////////////////////////////////////////////////////
DWORD HEXDUMP::SearchString(LPSTR lpFindString,FWORD fwPtr,DWORD dwLaenge)
{
  int wLen;
  DWORD dw;
  DWORD rc;
  FWORD fwPtr2;
  int iExc;

  if (!(wLen = lstrlen(lpFindString)))
     return 0;

  if (!dwLaenge)
     return 0;

  rc = 0;

  fwPtr2.offset = LOWORD(lpFindString);
  fwPtr2.selector = HIWORD(lpFindString);

  hAppInstance = GetTaskDS();

  InterruptRegister(NULL,(FARPROC)interruptcallback);

  if (!(iExc = Catch(cb))) {
      while (dwLaenge - fwPtr.offset) {
          dw = _hmemchr32i(fwPtr,*lpFindString,dwLaenge - fwPtr.offset);
          if (dw == 0xFFFFFFFF)
             break;
          fwPtr.offset = dw;
          if (!_hmemicmp32(fwPtr,fwPtr2,wLen)) {
              rc = fwPtr.offset + 1;
              break;
          } else
              fwPtr.offset++;
      }
  }

  InterruptUnRegister(NULL);

  if (iExc) {
      PostMessage(GetParent(hWnd), WM_COMMAND, wID, MAKELONG(iExc,HEXN_ERROR8) );
      rc = 0xFFFFFFFF;
  }

  return rc;
}
////////////////////////////////////////////////////////
// PaintProc - Paint Routine                            
////////////////////////////////////////////////////////
BOOL HEXDUMP::PaintProc(HDC hdc,LPRECT prect)
{
 DWORD style;
 int iExc;
 HANDLE handle;
 DWORD x;
 long lAnzZeilen;
 DWORD dwSize;
 WORD lByte;
 UINT wAttr;
 BYTE buffer[16];
 BYTE zeile[80];
 int i,j,k,pos;
 LPBYTE pstr;
 LPBYTE pstr1;
 LPBYTE pstr2;
 FWORD fwZiel;
 FWORD fwQuelle;
 WORD wTabs;
 WORD wChars;
 WORD fErr = 0;
 LPINT piTabpos;
 HBRUSH hBrush;
 BOOL rc;
                                                       /* font setzen */
   if (font)
      SelectObject(hdc,(HGDIOBJ)font);

   IntersectClipRect(hdc,prect->left,prect->top,prect->right,prect->bottom);
                                                         /* farben setzen */
	style = GetWindowStyle(hWnd);
	if (style & HDS_WINDOWBK) {
		bkcolor = GetSysColor(COLOR_WINDOW);
		fgcolor = GetSysColor(COLOR_WINDOWTEXT);
	} else {
		bkcolor = GetSysColor(COLOR_BTNFACE);
		fgcolor = GetSysColor(COLOR_BTNTEXT);
	}
   SetTextColor(hdc,fgcolor);
   SetBkColor(hdc,bkcolor);

                                                         /* selektor ueberpruefen */
   handle = (HANDLE)fAddr.selector;
   if (!(fwQuelle.selector = ValidateSegment(handle))) {
       fErr = HEXN_ERROR1;
       x = 0;
   } else                                                /* limit ueberpruefen und -> x */
       if (x = GetSelectorLimit(fwQuelle.selector)) {
           if (x + 1)
               x++;
           GetAttrofSelector(fAddr.selector,&wAttr);
           if (0x04 == (wAttr & 0x0C)) {                 // expand down segment?
               if (wAttr & 0x4000)
                   x = 0xFFFFFFFF;
               else
                   x = 0x10000;
           }
       }
                                                         /* offset beruecksichtigen */
   fwQuelle.offset = fAddr.offset;
   x        = x - fAddr.offset;
                                                         /* falls explizit laenge mitgegeben */
   if ((dwMaxSize) && (dwMaxSize < x))
       dwSize = dwMaxSize;
   else
       dwSize = x;
                                                         /* vertikalen scrollfaktor bercksichtigen */
   fwQuelle.offset = fwQuelle.offset + (DWORD)dwVPos * whSize;
                                                         /* die aktuelle position */
                                                         /* muá kleiner als das limit sein */
   if (dwSize <= (dwVPos * whSize)) {
       dwSize = 0;
       if (!fErr)
           fErr = HEXN_ERROR9;
   } else
       dwSize = dwSize - dwVPos * whSize;

                                                         /* anzahl restzeilen errechnen */
   lAnzZeilen = dwSize / whSize;
   if (lByte = (WORD)(dwSize % whSize))
      lAnzZeilen++;
   else
      lByte = whSize;

   if ((long)anzZeilen < lAnzZeilen) {
       lAnzZeilen = anzZeilen;
       lByte = whSize;
   }

   fwZiel.selector = HIWORD((LPVOID)&buffer);
   fwZiel.offset   = LOWORD((LPVOID)&buffer);
   k = whSize;


   if (flags.dwords) {
       wTabs = 4 + 2;
       wChars = 10 + 4 * 9 + 1;
       piTabpos = tabposd;
       tabposd[5]  = prect->right;
   } else
       if (flags.words) {
           wTabs = 8 + 2;
           wChars = 10 + 5 * 8 + 1;
           piTabpos = tabposw;
           tabposw[9]  = prect->right;
       } else
           if (flags.strings) {
               fwZiel.offset = LOWORD((LPVOID)&zeile)+10;
               wTabs = 2;
               wChars = 10 + 64 + 1;
               piTabpos = tabposs;
               tabposs[1] = prect->right;
           } else {
               wTabs = 16 + 2;
               wChars = 10 + 3 * 16 + 16 + 1;
               piTabpos = tabposb;
               tabposb[17] = prect->right;
           }


   pos = 0;
   i = 0;

   hAppInstance = GetTaskDS();
   j = InterruptRegister(NULL,(FARPROC)interruptcallback);

#if 0
   char szStr[80];
   wsprintf(szStr,"InterruptRegister returned %u\r\n",j);
   OutputDebugString(szStr);
#endif

   if (!(iExc = Catch(cb))) {
       for (;i<lAnzZeilen;i++,fwQuelle.offset = fwQuelle.offset + whSize,pos = pos + charh) {
           if ((int)(pos + charh - 1) < prect->top)
               continue;
           else
               if (pos > prect->bottom)
                   break;

           if (i == (lAnzZeilen - 1))
               k = lByte;

           wsprintf((LPSTR)zeile,
                    szFStr1,
                    fwQuelle.offset - fAddr.offset  + dwViewOffset);
//			_asm int 3;                    
           _hmemcpy32(fwZiel,fwQuelle,k);
           pstr = zeile + 10;
           if (flags.dwords) {
               pstr1 = buffer;
               wsprintf((LPSTR)zeile+10,
                        szFStr2,
                        *(LPDWORD)pstr1,
                        *(LPDWORD)(pstr1+4),
                        *(LPDWORD)(pstr1+8),
                        *(LPDWORD)(pstr1+12)
                       );
               pstr2 = zeile + 10 + 4 * 9;
           } else {
               if (flags.words) {
                   pstr1 = buffer;
                   wsprintf((LPSTR)zeile+10,
                        szFStr3,
                        *(LPWORD)pstr1,
                        *(LPWORD)(pstr1+2),
                        *(LPWORD)(pstr1+4),
                        *(LPWORD)(pstr1+6),
                        *(LPWORD)(pstr1+8),
                        *(LPWORD)(pstr1+10),
                        *(LPWORD)(pstr1+12),
                        *(LPWORD)(pstr1+14)
                       );
                   pstr2 = zeile + 10 + 8 * 5;
               } else {
                   if (flags.strings) {
                       for (j=0,pstr2=zeile+10;j < k;j++,pstr2++) {
                           if (*pstr2 == 9)
                               *pstr2 = '.';
                       }
                   } else {
                       pstr2 = zeile + 10 + 16 * 3;
                       for (j = 0,pstr1 = buffer;j < k;j++,pstr1++,pstr=pstr+3,pstr2++) {
                           _ConvByteHex((LPSTR)pstr,*pstr1);
                            *(pstr+2) = 9;
                           if (*pstr1 != 9)
                               *pstr2 = *pstr1;
                           else
                               *pstr2 = '.';
                       }
                       while ((WORD)k < whSize) {   // rest mit blanks auffuellen
                           *pstr = ' ';
                           *(pstr+1) = ' ';
                           *(pstr+2) = 9;
                           *pstr2 = ' ';
                           pstr = pstr + 3;
                           pstr2++;
                           k++;
                       }
                   }
               }
           }
           *pstr2 = 9;
           TabbedTextOutEx(hdc,XPOS,pos,(LPSTR)zeile,wChars,wTabs,piTabpos,0);
       }  // end for
   } else {
       for (;i < lAnzZeilen;i++,fwQuelle.offset = fwQuelle.offset + 16,pos = pos + charh) {
           wsprintf((LPSTR)zeile,
                    szFStr4,
                    fwQuelle.offset - fAddr.offset + dwViewOffset);
           pstr = zeile + 10;
           pstr2 = zeile + 58;
           for (j = 0;j<16;j++,pstr=pstr+3,pstr2++) {
                *pstr     = '?';
                *(pstr+1) = '?';
                *(pstr+2) = 9;
                *pstr2 = '?';
           }
           *pstr2 = 9;
           TabbedTextOutEx(hdc,XPOS,pos,(LPSTR)zeile,75,18,tabposb,0);
       }
   }    

   InterruptUnRegister(NULL);
// OutputDebugString("InterruptUnRegister called\r\n");

   if (pos < prect->bottom) {
       prect->top = pos;
       hBrush = CreateSolidBrush(bkcolor);
       FillRect(hdc,prect,hBrush);
       DeleteObject(hBrush);
   }

   rc = TRUE;

   if  (fErr)
       PostMessage(GetParent(hWnd),
                   WM_COMMAND,
                   wID,
                   MAKELONG(0,fErr) );
   else
       if (iExc)
           PostMessage(GetParent(hWnd),
                   WM_COMMAND,
                   wID,
                   MAKELONG(iExc,HEXN_ERROR6) );


   return rc;
}
////////////////////////////////////////////////////////
// PaintBorder (NC-Area malen)                                         
////////////////////////////////////////////////////////
void HEXDUMP::PaintBorder(DWORD style)
{
 RECT rect;
// RECT rect1;
 int x,i;
 HDC  hdc;
 HRGN hrgn;
 HBRUSH   hbrush;
 HBRUSH   hbrush1;
 HPEN wpen,bpen,apen;


  hdc = GetWindowDC(hWnd);
  GetChildWindowRect(hWnd,&rect);
  if (style & WS_VSCROLL)
      rect.right = rect.right - GetSystemMetrics(SM_CXVSCROLL);
  i = GetSystemMetrics(SM_CXBORDER);

  if (style & WS_BORDER)
     if (style & HDS_PLAST) {
         wpen = CreatePen(PS_SOLID,i,color1);
         bpen = CreatePen(PS_SOLID,i,color2);

         apen = (HPEN)SelectObject(hdc,wpen);

         MoveTo(hdc,rect.left,rect.bottom-i);        // unten links
         LineTo(hdc,rect.left,rect.top);             // unten links nach oben links
         LineTo(hdc,rect.right,rect.top);            // oben links nach oben rechts

         MoveTo(hdc,rect.right-2*i,rect.top+i);      // oben rechts
         LineTo(hdc,rect.right-2*i,rect.bottom-2*i); // oben rechts nach unten rechts
         LineTo(hdc,rect.left+i,rect.bottom-2*i);    // unten rechts nach unten links

         SelectObject(hdc,(HGDIOBJ)bpen);

         MoveTo(hdc,rect.left+i,rect.bottom-2*i-1);  // unten links
         LineTo(hdc,rect.left+i,rect.top+i);         // unten links nach oben links
         LineTo(hdc,rect.right-2*i-1,rect.top+i);    // oben links nach oben rechts

         MoveTo(hdc,rect.right-i,rect.top);          // oben rechts
         LineTo(hdc,rect.right-i,rect.bottom-i);     // oben rechts nach unten rechts
         LineTo(hdc,rect.left,rect.bottom-i);        // unten rechts nach unten links

         SelectObject(hdc,(HGDIOBJ)apen);

         DeleteObject(wpen);
         DeleteObject(bpen);

         rect.left++;rect.left++;
         rect.top++;rect.top++;
         rect.right--;rect.right--;
         rect.bottom--;rect.bottom--;
     } else {
         FrameRect(hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
         rect.left++;
         rect.top++;
         rect.right--;
         rect.bottom--;
     }

  if (style & WS_DLGFRAME) {
      hbrush  = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
      hbrush1 = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
//	  hbrush2 = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
      hrgn = CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
      x = GetSystemMetrics(SM_CXDLGFRAME);
      if (style & HDS_PLAST)
          FrameRgn(hdc,hrgn,hbrush1,x,x);
      else
          FrameRgn(hdc,hrgn,hbrush,x,x);
      DeleteObject(hrgn);
      DeleteObject(hbrush);
      DeleteObject(hbrush1);
//	  DeleteObject(hbrush2);
  }

  ReleaseDC(hWnd,hdc);
}
////////////////////////////////////////////////////////
// char height des aktuellen fonts holen                              
////////////////////////////////////////////////////////
int HEXDUMP::GetCharHeight(void)
{
    TEXTMETRIC metrics;
    HDC hdc;
    RECT rect;

    hdc = GetDC(hWnd);
    if (font)
        SelectObject(hdc,(HGDIOBJ)font);
    GetTextMetrics(hdc,&metrics);
    charh = metrics.tmHeight - chkorr;
    charw = metrics.tmAveCharWidth + charoffs;
    GetClientRect(hWnd,&rect);
    anzZeilen = (rect.bottom - rect.top) / charh;
    if ((rect.bottom - rect.top) % charh)
        AdjustClientSize(hWnd,0,anzZeilen * charh + chkorr,0);

    ReleaseDC(hWnd,hdc);
    return 1;
}
////////////////////////////////////////////////////////
// Size Parameter setzen                                              
////////////////////////////////////////////////////////
void HEXDUMP::SetSizeParms()
{
  DWORD x;
  UINT  wAttr;

  dwVPos = 0;

  GetAttrofSelector(fAddr.selector,&wAttr);
  x = GetSelectorLimit(GlobalHandleToSel((HANDLE)fAddr.selector));
  if (x + 1)
      x++;
  if (0x04 == (wAttr & 0x0C))          // expand down segment?
     {
      if (!fAddr.offset)
         fAddr.offset = x;
      if (wAttr & 0x4000)
         x = 0 - x;
      else
         x = 0x10000 - x;
     }                                  /* falls explizit laenge mitgegeben */
  else
  if (fAddr.offset < x)
      x = x - fAddr.offset;

  if ((dwMaxSize) && (dwMaxSize < x))
     x = dwMaxSize;

  vmax = x / whSize;
  if (x % whSize)
      vmax++;

  x = vmax;
  sFac = 0;
  while (x > 0x7FFF)
     {
      x = x>>1;
      sFac++;
     }

  if (vmax > anzZeilen)
     {
      SetScrollRange(hWnd,SB_VERT,0,(int)(x - anzZeilen),TRUE);
      SetScrollPos(hWnd,SB_VERT,(int)dwVPos,TRUE);
     }
    return;
}

LRESULT HEXDUMP::OnSave(WPARAM wParam, LPARAM lParam)
{
    LRESULT rc = 0;
    int    hFile;
    HANDLE handle;
    LPSTR lpstr;
    DWORD x;

    if (HIWORD(fAddr.offset))
        return rc;
    if (!(fAddr.selector & 0x0004))	// schreiben geht nur bei LDT selektoren!
        return rc;
    handle = (HANDLE)fAddr.selector;
    lpstr  = (LPSTR)MAKELONG(LOWORD(fAddr.offset),handle);
    if (ValidateSegment(handle)) {
        if (dwMaxSize)
            x = dwMaxSize;
        else {
            x = GetSelectorLimit(fAddr.selector) + 1;
        }

        if (x) {
            if (!wParam) {
                char szStr[80];
                if (!lParam) {
                    GetTempFileName(0,szHDS,0,szStr);
                    lParam = (LPARAM)(LPVOID)szStr;
                }
                hFile = _lcreat((LPSTR)lParam,0);
            } else
                hFile = wParam;

            if (hFile != -1) {
                rc = _hwrite(hFile,lpstr,x);
                if (!wParam)
                    _lclose(hFile);
            }
        }
    }
    if (!rc)
        PostMessage(GetParent(hWnd),WM_COMMAND,wID,MAKELONG(0,HEXN_ERROR7));

    return rc;
}

// vor save Filename holen

LRESULT HEXDUMP::OnSaveEx()
{
    DWORD x;
    LRESULT rc = 0;

    OPENFILENAME ofn;
    char szStr1[260];
    char szStr2[280];
    char * pStr;

    strcpy(szStr1,"");

    memset(szStr2,0,sizeof(szStr2));
    strcpy(szStr2,"Alle Dateien");
    strcpy(szStr2 + strlen(szStr2) + 1,"*.*");
    
    memset((void *)&ofn,0,sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;

    ofn.lpstrFilter = szStr2;
    ofn.lpstrCustomFilter = 0;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = szStr1;
    ofn.nMaxFile = sizeof(szStr1);
    ofn.Flags = OFN_EXPLORER;
    if (GetSaveFileName(&ofn)) {
        rc = (LRESULT)OnSave(0,(LPARAM)(LPVOID)szStr1);
        if (dwMaxSize)
            x = dwMaxSize;
        else
            if (fAddr.selector & 0x0004)
                x = GetSelectorLimit(fAddr.selector) + 1;
            else
                x = 0;
        if (!x) x--;
        if ((DWORD)rc < x) {
            pStr = 0;
            if (rc == 0)
                strcpy(szStr2,"Es konnte nichts geschrieben werden!");
            else
                wsprintf(szStr2,"Es konnten nur %lu von %lu\nBytes geschrieben werden!",rc,x);
        } else {
            pStr = "Hinweis";
            wsprintf(szStr2,"Es wurden %lu Bytes auf\n%s\ngeschrieben",rc,(LPSTR)szStr1);
        }
        MessageBox(hWnd,szStr2,pStr,MB_OK);
    }
    return rc;
}


// OnResetContent

LRESULT HEXDUMP::OnResetContent(WPARAM wParam, LPARAM lParam)
{            
    HANDLE handle;
    LPHEXDUMPPARM lphdp;
    WORD sel;
    DESCRIPTOR desc;
    
    if (wType & 0x8000) {       // wurde beim Vorgänger Selektor allokiert (Linear)?
        SetSelectorLimit(fAddr.selector,0);
        FreeSelector(fAddr.selector);
    }
    lphdp  = (LPHEXDUMPPARM)lParam;
    fAddr.offset   = lphdp->dwOffset;
    fAddr.selector = (WORD)lphdp->hGlobal;
    dwMaxSize      = lphdp->dwLength;
    wType          = lphdp->wType & 0x7FFF;

    if (wType & HDP_LINEAR)
        if (fAddr.selector = AllocSelector(GetCS())) {
            SetSelectorBase(fAddr.selector,fAddr.offset);
            dwViewOffset = fAddr.offset;
            fAddr.offset = 0;
            SetSelectorLimit(fAddr.selector,dwMaxSize - 1);
            wType = wType | 0x8000;
        }
    handle = (HANDLE)fAddr.selector;

    sel = GlobalHandleToSel(handle);
    GetDescriptor(sel,&desc);
    if (!(desc.attr & 0x0080))             /* NOT PRESENT + DATA? */
        if (!(desc.attr & 0x0008)) {
            /* not present exception return */
            PostMessage(GetParent(hWnd),WM_COMMAND,wID,MAKELONG(0,HEXN_ERROR5));
            return FALSE;
        }
                                               /* schauen ob selector gueltig */
    if (!(ValidateSegment(handle))) {
        PostMessage(GetParent(hWnd),WM_COMMAND,wID,MAKELONG(0,HEXN_ERROR1));
        return FALSE;
    }

    SetSizeParms();

    return 1;
        
}        

// OnSetMode

int HEXDUMP::OnSetMode(WPARAM wParam)
{
    WORD x;

    x = whSize;
    wflags = wParam;
    if (flags.strings)
        whSize = 64;
    else
        whSize = 16;
    if (x != whSize)
        SetSizeParms();

    InvalidateRect(hWnd,0,1);

    return 1;
}

// konstruktor

HEXDUMP::HEXDUMP(HWND hWndSrc, WPARAM wParam, LPARAM lParam)
{
    WORD i;

    memset(this,0,sizeof(HEXDUMP));
    if (!wCount)
        if (LOWORD(GetVersion()) == 3) {
            charoffs = 0;
            color1 = RGB(255,255,255);
        } else {
            charoffs = 1;
            color1 = GetSysColor(COLOR_BTNHIGHLIGHT);
        }
    color2 = GetSysColor(COLOR_BTNSHADOW);
    hWnd = hWndSrc;
    wID = GetWindowWord(hWnd,GWW_ID);
    whSize = 16;
    GetCharHeight();
                                                         /* erster aufruf? */
    if (!wCount) {
        tabposb[0] = charw * 9;
        tabposw[0] = charw * 9;
        tabposd[0] = charw * 9;
        tabposs[0] = charw * 9;
        for (i=1;i<17;i++)
            tabposb[i] = tabposb[i-1] + MulDiv(charw,5,2);
        for (i=1;i<9;i++)
            tabposw[i] = tabposw[i-1] + 6 * charw;
        for (i=1;i<6;i++)
            tabposd[i] = tabposd[i-1] + 10 * charw;
    }
    wCount++;
    SetWindowWord(hWnd,0,(WORD)this);

    return;
}

// destruktor

HEXDUMP::~HEXDUMP()
{
    if (wType & 0x8000) {
        SetSelectorLimit(fAddr.selector,0);
        FreeSelector(fAddr.selector);
    }
    wCount--;
    return;
}
////////////////////////////////////////////////////////
// WndProc - Window Methode                             
////////////////////////////////////////////////////////
long __export CALLBACK HexdumpWndFn(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam )
{
    PAINTSTRUCT ps;
    DWORD x;
    DWORD style;
    FWORD fw1;
    WORD i,j,offset;
    PHEXDUMP pHD;
    HMENU hMenu;
    POINT pt;
    NCCALCSIZE_PARAMS FAR * lpnccsp;
    // char szStr[80];
    long rc = 0;

    pHD = (PHEXDUMP)GetWindowWord(hWnd,0);

    switch (message) {
    case WM_CREATE:
        pHD = new HEXDUMP(hWnd,wParam,lParam);
        if (pHD)
            rc = 0;
        else
            rc = -1;
        break;
    case WM_DESTROY:
        if (pHD)
            delete pHD;
        break;
    case WM_SETFONT:
        pHD->font = wParam;
        pHD->GetCharHeight();
        break;
    case WM_COMMAND:
        pHD->wflags = pHD->wflags & 0xFF00;
        switch (wParam) {
        case IDM_BYTEFORMAT:
            break;
        case IDM_WORDFORMAT:
            pHD->flags.words = 1;
            break;
        case IDM_DWORDFORMAT:
            pHD->flags.dwords = 1;
            break;
        case IDM_TEXTFORMAT:
            pHD->flags.strings = 1;
            break;
        case IDM_SAVEHEXDUMP:
            return pHD->OnSaveEx();
            break;
        }     
        pHD->OnSetMode(pHD->wflags);
        break;
    case HD_SAVE:
        rc = pHD->OnSave(wParam,lParam);
        break;
    case HD_SEARCHSTRING:
        if (!lParam)
            break;
        if (ValidateSegment((HANDLE)pHD->fAddr.selector)) {
            if (pHD->dwMaxSize)
                x = pHD->dwMaxSize;
            else {
                x = GetSelectorLimit(pHD->fAddr.selector);
                if (!(x++)) x--;
            }

            fw1 = pHD->fAddr;
            if (pHD->dwVPos) {
                fw1.offset = fw1.offset + pHD->dwVPos * pHD->whSize;
                x   = x - pHD->dwVPos * pHD->whSize;
            }
            if (wParam) {
                fw1.offset = fw1.offset + pHD->whSize;
                x   = x - pHD->whSize;
            }
/*
            wsprintf(szStr,
                     "fptr=%X:%lX,size=%lX",
                     fw1.selector,
                     fw1.offset,
                     x
                    );
            MessageBox(0,szStr,"",MB_OK);
*/
            rc = pHD->SearchString((LPSTR)lParam,
                              fw1,
                              x);
            if (rc)
                rc = rc - pHD->fAddr.offset;
        }
        break;
    case HD_SETPOSITION:
        pHD->dwVPos = lParam / pHD->whSize;
        SetScrollPos(hWnd,SB_VERT,(int)(pHD->dwVPos>>pHD->sFac),TRUE);
        InvalidateRect(hWnd,0,1);
        break;
    case HD_SETMODE:
        pHD->OnSetMode(wParam);
        break;
    case HD_GETMODE:
        rc = pHD->wflags;
        break;
    case HD_GETOFFSET:
//        rc = GetSelectorBase(pHD->fAddr.selector);
        rc = pHD->fAddr.offset;
        break;
    case HD_SETVIEWOFFSET:
        pHD->dwViewOffset = lParam;
        break;
    case HD_GETSIZE:
        rc = pHD->dwMaxSize;
        break;
    case HD_RESETCONTENT:
        rc = pHD->OnResetContent(wParam,lParam);
        break;
    case WM_GETDLGCODE:
        return DLGC_WANTARROWS;   /* direction keys selber verarbeiten */
    case WM_VSCROLL:
        pHD->VertScroll(wParam, LOWORD(lParam));
        break;
    case WM_KEYDOWN:
        pHD->DoKeys(wParam);
        break;
    case WM_RBUTTONDOWN:
        if (wParam & MK_LBUTTON)
            break;
        if (pHD->flags.usestdcontextmenu) {
            hMenu = LoadMenu((HINSTANCE)GetMyDS(),MAKEINTRESOURCE(IDR_MENUHEXDUMP));
            if (hMenu) {
                GetCursorPos(&pt);
                TrackPopupMenu(GetSubMenu(hMenu,0),TPM_LEFTALIGN | TPM_LEFTBUTTON,
                               pt.x,pt.y,0,hWnd,NULL);
                DestroyMenu(hMenu);
            }
        } else
            SendMessage(GetParent(hWnd),WM_COMMAND,pHD->wID,MAKELONG(hWnd,HEXN_RBUTTONDOWN));
        break;
    case WM_PAINT:
        BeginPaint(hWnd,(LPPAINTSTRUCT)&ps);
        pHD->PaintProc(ps.hdc,(LPRECT)&ps.rcPaint);
        EndPaint(hWnd,(LPPAINTSTRUCT)&ps);
        break;
    case WM_NCPAINT:
        style = GetWindowStyle(hWnd);
        SetWindowStyle(hWnd,style & ~WS_BORDER);
        rc = DefWindowProc( hWnd, message, wParam, lParam );
        SetWindowStyle(hWnd,style);
        if (style & (WS_BORDER | WS_DLGFRAME))
            pHD->PaintBorder(style);
        break;
    case WM_MOUSEACTIVATE:
        SetFocus(hWnd);
        rc = DefWindowProc(hWnd, message, wParam, lParam);
        break;
    case WM_NCCALCSIZE:
        style = GetWindowStyle(hWnd);
        SetWindowStyle(hWnd,style & ~WS_BORDER);
        rc = DefWindowProc( hWnd, message, wParam, lParam );
        SetWindowStyle(hWnd,style);
        offset = 0;
        i = GetSystemMetrics(SM_CXDLGFRAME);
        j = GetSystemMetrics(SM_CYDLGFRAME);
        if (style & WS_BORDER)
           {
            offset = offset + GetSystemMetrics(SM_CXBORDER);
            if (style & HDS_PLAST)
                offset = offset + GetSystemMetrics(SM_CXBORDER);
           }
        lpnccsp = (NCCALCSIZE_PARAMS FAR *)lParam;
        lpnccsp->rgrc[0].left   = lpnccsp->rgrc[0].left   + offset;
        lpnccsp->rgrc[0].right  = lpnccsp->rgrc[0].right  - offset;
        lpnccsp->rgrc[0].top    = lpnccsp->rgrc[0].top    + offset;
        lpnccsp->rgrc[0].bottom = lpnccsp->rgrc[0].bottom - offset;

        break;
    default:
        rc = DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }
    return rc;
}
////////////////////////////////////////////////////////
// HexDumpRegisterClass                                 
////////////////////////////////////////////////////////
BOOL __export WINAPI HexDumpRegisterClass(HINSTANCE hInstance, LPSTR lpstr, WORD mode)
{
    BOOL rc;
    WNDCLASS wc;

//    wc.style = CS_PARENTDC;
    wc.style = 0;
    if (mode) {
        if (fRegistered && (!lpstr))
            return TRUE;
        wc.style = CS_GLOBALCLASS | wc.style;
        hInstance = (HINSTANCE)GetExePtr((HGLOBAL)GlobalHandle(GetMyDS()));
    }

    wc.lpfnWndProc    = (WNDPROC)HexdumpWndFn;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = sizeof(PHEXDUMP);
    wc.hInstance      = (HINSTANCE)hInstance;
    wc.hIcon          = 0;
    wc.hCursor        = LoadCursor( 0, IDC_ARROW);
//    wc.hbrBackground  = (HBRUSH)COLOR_BTNFACE + 1;
    wc.hbrBackground  = NULL;
    wc.lpszMenuName   = (LPSTR)0;
    if (lpstr)
        wc.lpszClassName  = lpstr;
    else
        wc.lpszClassName  = szClassName;

    rc = RegisterClass(&wc);
    if (rc && mode && (!lpstr))
        fRegistered = TRUE;

    return rc;
}
