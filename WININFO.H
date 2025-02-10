
#include <stattext.h>
#include <xlistbox.h>
#include <hexdump.h>

typedef struct tagGMEMFILTER {
   unsigned discardfilter:1;
   unsigned discardablefilter:1;
   unsigned nondiscardablefilter:1;
   unsigned pagelockedfilter:1;
   unsigned notpagelockedfilter:1;
   unsigned codefilter:1;
   unsigned datafilter:1;
   unsigned resourcefilter:1;
   unsigned internfilter:1;
   unsigned fProtectOwner:1;       // owner nicht eingebbar
   unsigned fStatic:1;             // aenderungen zurueckkopieren?
   unsigned fErrMBF:1;             // membisfilter wert ungueltig
   unsigned fErrMVF:1;             // memvonfilter wert ungueltig
   unsigned fSortdir:1;            // sortierung absteigend?
   DWORD memvonfilter;
   DWORD membisfilter;
   int   wGmemtype;
   int   fSort;                    // welche sortierung ist aktiv?
   char  szOwner[20];
   HGLOBAL hOwner;
    } GMEMFILTER,FAR * LPGMEMFILTER;

extern PSTR winName;
extern PSTR errorstr1;
extern PSTR errorstr2;
extern char fnstr[128];
extern char grech[14];

extern HINSTANCE hInst;        /* Instanz der Task */
extern HICON  hIcon;
extern HWND   hWndMain;        /* Handle des Hauptfensters */
extern HWND   hWndMenu;

extern int xpos,ypos;

HMENU WINAPI TrackPopup(HWND, PINT);
