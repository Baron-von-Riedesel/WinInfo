
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"

// #define strlen lstrlen
// #define strcat(x,y) lstrcat((LPSTR)x,y)
// #define strcpy(x,y) lstrcpy((LPSTR)x,y)

extern HFONT hFontAlt;
extern HCURSOR hCursor;
extern HCURSOR hCursorWait;

BOOL CALLBACK ViewDlg    (HWND, UINT, WPARAM, LPARAM);

int fRelocs;

static CSTRC szSegFStr     [] = "%u\t%04X\t%04X\t%04X\t%04X\t";
static CSTRC szModulStat   [] = "Segm\t%d\t total\t%lX\tdiscrdbl\t%lX\t Cnt\t%d\t Auto\t%d\t";
static CSTRC szSegStat     [] = "Attr\t%s\t\t%s\t";
static CSTRC szSegStatFStr [] = "Base=%lX,Limit=%lX";
static CSTRC szSegmentFStr [] = "Segment: %X";
static CSTRC szData        [] = "data";
static CSTRC szCode        [] = "code";
static CSTRC szPreload     [] = ",preload";
static CSTRC szLoadoncall  [] = ",loadoncall";
static CSTRC szDiscardable [] = ",discardable";
static CSTRC szMoveable    [] = ",moveable";
static CSTRC szFixed       [] = ",fixed";
static CSTRC szShared      [] = ",shared";
static CSTRC szRO          [] = ",r/o";
static CSTRC szEO          [] = ",e/o";
static CSTRC szNotPresent  [] = "not present";
static CSTRC sz32bit       [] = ",32bit";

/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Dialogfunktion Segmentflags aendern                               บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK SegFlagsDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
 BOOL rc = FALSE;
 SEGMFLAGS FAR * lpSegF;
 SEGMFLAGS sf;

   switch(message)
     {
      case WM_INITDIALOG:
         EnableWindow(GetParent(hDlg),0);
         SetWindowLong(hDlg,DWL_USER,lParam);
         sf = *(SEGMFLAGS FAR *)lParam;
         SendDlgItemMessage(hDlg,ID_SEGFLG1,BM_SETCHECK,sf.moveable,0);
         SendDlgItemMessage(hDlg,ID_SEGFLG2,BM_SETCHECK,sf.shared,0);
         SendDlgItemMessage(hDlg,ID_SEGFLG3,BM_SETCHECK,sf.readonly,0);
         SendDlgItemMessage(hDlg,ID_SEGFLG4,BM_SETCHECK,sf.conforming,0);
         SendDlgItemMessage(hDlg,ID_SEGFLG5,BM_SETCHECK,sf.discardable,0);
         ShowWindow(hDlg,SW_NORMAL);
         rc = TRUE;
         break;
      case WM_CLOSE:
         EnableWindow(GetParent(hDlg),1);
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
      case WM_COMMAND:
         switch(wParam)
           {
            case IDCANCEL: // schliessen
               PostMessage(hDlg,WM_CLOSE,0,0);
               break;
           case ID_SEGFLG1:
           case ID_SEGFLG2:
           case ID_SEGFLG3:
           case ID_SEGFLG4:
           case ID_SEGFLG5:
               lpSegF = (SEGMFLAGS FAR *)GetWindowLong(hDlg,DWL_USER);
               sf = *lpSegF;
               if (HIWORD(lParam) == BN_CLICKED)
                   switch (wParam)
                     {
                      case ID_MEMFIL1:
                        sf.moveable  = 1 - sf.moveable;
                        break;
                      case ID_MEMFIL2:
                        sf.shared    = 1 - sf.shared;
                        break;
                      case ID_MEMFIL3:
                        sf.readonly  = 1 - sf.readonly;
                        break;
                      case ID_MEMFIL4:
                        sf.conforming = 1 - sf.conforming;
                        break;
                      case ID_MEMFIL5:
                        sf.discardable = 1 - sf.discardable;
                        break;
                     }
               *lpSegF = sf;
               break;
           }
         rc = TRUE;     /* WM_COMMAND immer RC TRUE */
         break;
      default:
         break;
     }
 return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ FindSegment - Segment in NE suchen                   บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
SEGMDESC FAR * FindSegment(HMODULE hModule,HGLOBAL hSegm)
{
 SEGMDESC FAR * lpsegm;
 LPMODSTRUC lpmod;
 int i;

 lpmod = (LPMODSTRUC)MAKELONG(0,hModule);
 lpsegm = (SEGMDESC FAR *)MAKELONG(lpmod->offssegtab,hModule);
 for (i=lpmod->segs;i;i--,lpsegm++)
     if (lpsegm->handle == hSegm)
         return lpsegm;
 return 0;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Segmentflags in Klartext umwandeln                   บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
void TranslateSegmentFlags(SEGMFLAGS flags,char * str1)
{
   if (flags.data)
       lstrcpy(str1,szData);
   else
       lstrcpy(str1,szCode);
   if (flags.preload)
       lstrcat(str1,szPreload);
   else
       lstrcat(str1,szLoadoncall);

   if (flags.discardable)
       lstrcat(str1,szDiscardable);

   if (flags.moveable)
       lstrcat(str1,szMoveable);
   else
       lstrcat(str1,szFixed);

   if (flags.shared)
       lstrcat(str1,szShared);

   if (flags.readonly)
       if (flags.data)
           lstrcat(str1,szRO);
       else
           lstrcat(str1,szEO);

   if (flags.mixed1632)
       lstrcat(str1,sz32bit);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ SegmentDlg - Segment Dialog                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK SegmentDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab2[] = {ID_SEGMMODUL | 0x8000,IDS_VIEW,ID_SUBDLG2,
                        0};
 DWORD dx,dwXMemlen,dwXDisclen;
 char str[128];
 char str1[60];
 char szStr2[40];
 UINT y,i,j,wMode;
 int x;
 HEXDUMPPARM hdp;
 int  tabpos[11];
 HWND hWnd,hWndStat;
 SEGMDESC FAR * lpsegm;
 DESCRIPTOR desc;
 HGLOBAL hSegm;
 HMODULE hModule;
 MODSTRUC FAR * lpmod;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      LoadTabs(IDUS_4,tabpos);
      SendDlgItemMessage(hDlg,ID_LISTSEGM,LB_SETTABSTOPS,tabpos[0],(LPARAM)(LPINT)&tabpos[1]);
      SendDlgItemMessage(hDlg,ID_LISTSEGM,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      LoadTabs(IDUS_3,tabpos);
      SendDlgItemMessage(hDlg,ID_STATMODUL,ST_SETTABSTOPS,tabpos[0],(LPARAM)(LPVOID)&tabpos[1]);
      LoadTabs(IDUS_6,tabpos);
      SendDlgItemMessage(hDlg,ID_STATSEGM,ST_SETTABSTOPS,tabpos[0],(LPARAM)(LPVOID)&tabpos[1]);

      if (LOWORD(lParam))
         {
          SendMessage(hDlg,WM_COMMAND,ID_REFRESH,lParam);
//          SetWindowPos(hDlg,0,xModulpos,yModulpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
          ShowWindow(hDlg,SW_SHOWNORMAL);
          GetModuleName((HMODULE)LOWORD(lParam),szStr2,sizeof(szStr2));
          wsprintf(str," Segmente Modul %s",szStr2);
          SetWindowText(hDlg,str);
         }
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_REFRESH:
              hWnd = GetDlgItem(hDlg,ID_LISTSEGM);
              hModule = (HMODULE)LOWORD(lParam);
              wMode = HIWORD(lParam);
              SendMessage(hWnd,XLB_SETWINDOWDATA,0,lParam);
                                                 /* anzahl segmente holen */
              SendMessage(hWnd,LB_RESETCONTENT,0,0);
              lpmod = (MODSTRUC FAR *)MAKELONG(0,hModule);
              lpsegm = (SEGMDESC FAR *)MAKELONG(lpmod->offssegtab,hModule);
              i = lpmod->segs;
              j = 1;
              dwXMemlen = 0;
              dwXDisclen = 0;
              fRelocs = 0;
              while (i)
                  {
                   if (lpsegm->flags.relocs)        /* relocs vorhanden? */
                       fRelocs = 1;
                   wsprintf(str,
                           szSegFStr,
                           j,                       /* Segmentnr */
                           lpsegm->handle,          /* handle */
                           lpsegm->flags,           /* flags */
                           lpsegm->memlen,          /* ursprungsgroesse im memory */
                           lpsegm->filelen          /* groesse im file */
                          );
                   if (wMode)
                      {
                       TranslateSegmentFlags(lpsegm->flags,str1);
                       strcat(str,str1);
                      }
                   dwXMemlen = dwXMemlen + lpsegm->memlen;
                   if (lpsegm->flags.discardable)
                      dwXDisclen = dwXDisclen + lpsegm->memlen;
                   x = (int)SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM)(LPSTR)str);
                   SendMessage(hWnd, LB_SETITEMDATA, x, MAKELONG(y,lpsegm->handle));
                   lpsegm++;
                   i--;j++;
                  }
              wsprintf(str,
                       szModulStat,
                       lpmod->segs,
                       dwXMemlen,
                       dwXDisclen,
                       lpmod->count,
                       lpmod->autods
                      );
              SetDlgItemText(hDlg,ID_STATMODUL,str);
              SendMessage(hWnd,LB_SETCURSEL,0,0);
              break;
         case ID_SUBDLG2:
              hWnd = GetDlgItem(hDlg,ID_LISTSEGM);
              hModule = (HMODULE)SendMessage(hWnd,XLB_GETWINDOWDATA,0,0);
              x =  (int)SendMessage(hWnd, LB_GETCURSEL,0,0);
              if (x == LB_ERR)
                  break;
              dx = (LONG)SendMessage(hWnd, LB_GETITEMDATA,x,0);
              hSegm = (HGLOBAL)HIWORD(dx);
              if (lpsegm = FindSegment(hModule,hSegm))
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_SEGMFLAGS),
                                    hDlg,
                                    SegFlagsDlg,
                                    (DWORD)&lpsegm->flags
                                   );
              break;
         case ID_SUBDLG1:
         case ID_SEGMMODUL:
              x =  (int)SendDlgItemMessage(hDlg, ID_LISTSEGM, LB_GETCURSEL,0,0);
              if (x == LB_ERR)
                  break;
              dx = (LONG)SendDlgItemMessage(hDlg, ID_LISTSEGM, LB_GETITEMDATA,x,0);
              hSegm = (HGLOBAL)HIWORD(dx);
              if (hSegm)
                 {
                  hdp.dwOffset = 0;
                  hdp.hGlobal  = hSegm;
                  hdp.dwLength = 0;
                  hdp.wType    = 1 | HDP_SETWINDOWTEXT;
                  hWnd = CreateDialogParam(hInst,
                                           MAKEINTRESOURCE(IDD_VIEW),
                                           hDlg,
                                           ViewDlg,
                                           (LPARAM)(LPVOID)&hdp
                                           );
//                  PostMessage(hDlg,WM_COMMAND,ID_LISTSEGM,MAKELONG(0,LBN_SELCHANGE));
                 }
              else
                  CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRMOD10),0,MB_OK);
              break;
         case ID_LISTSEGM:       // Listbox
              hWnd     = GetDlgItem(hDlg,ID_LISTSEGM);
              hWndStat = GetDlgItem(hDlg,ID_STATSEGM);
              hModule = (HMODULE)SendMessage(hWnd,XLB_GETWINDOWDATA,0,0);
              x =  (int)SendMessage(hWnd,LB_GETCURSEL,0,0);
              if (x == LB_ERR)           /* es ist kein segment ausgewaehlt */
                 {
                  if (hWndStat)
                      SetWindowText(hWndStat,"Attr\t\t\t\t");
                  break;
                 }
              dx = (LONG)SendMessage(hWnd,LB_GETITEMDATA,x,0);
              hSegm = (HGLOBAL)HIWORD(dx);
              x     = LOWORD(dx);                        /* flags */
              switch(HIWORD(lParam))
                {
                 case LBN_SELCHANGE:                    /* anderes segment */
                   if (!(lpsegm = FindSegment(hModule,hSegm)))
                     {
                      if (hWndStat)
                          SetWindowText(hWnd,"Attr\t?\t\tSegment nicht gefunden\t");
                      break;
                     }
                   TranslateSegmentFlags(lpsegm->flags,str1);

                   hSegm = (HGLOBAL)GlobalHandleToSel(hSegm);
                   DPMIGetDescriptor((WORD)hSegm,&desc);

//                   if (!(lpsegm->flags.isloaded))
                   if (!(desc.attr & 0x0080))
                       lstrcpy(szStr2,szNotPresent);
                   else
                       wsprintf(szStr2,
                                szSegStatFStr,
                                GetSelectorBase((UINT)hSegm),
                                GetSelectorLimit((UINT)hSegm)
                               );

                   wsprintf(str,
                            szSegStat,
                            (LPSTR)str1,
                            (LPSTR)szStr2
                           );
                   if (hWndStat)
                       SetWindowText(hWndStat,str);

                   break;
                 case LBN_DBLCLK:
                   PostMessage(hDlg,WM_COMMAND,ID_SEGMMODUL,0);
                   break;
                 case XLBN_RBUTTONDOWN:
                   TrackPopup(hDlg,BtnTab2);
                   break;
                }         /* end switch HIWORD(lParam) */
              break;          /* end case ID_LISTSEGM */
         }
      rc = TRUE;
      break;
    default:
      break;
    }

 return rc;
}
