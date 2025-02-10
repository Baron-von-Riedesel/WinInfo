
/* dialog template  + strings auflisten */

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
#include "dlgtempl.h"

//#define strlen lstrlen
//#define strcpy lstrcpy
//#define strcat lstrcat

extern HFONT hFontAlt;
HMENU WINAPI TrackPopup(HWND, PINT);

static WORD maxlen;
static WORD limit;

typedef MENUITEMTEMPLATE FAR *LPMENUITEMTEMPLATE;

static KEYENTRY keytab1[] = {
                  "button",   0X80,
                  "edit",     0X81,
                  "static",   0X82,
                  "listbox",  0X83,
                  "scrollbar",0X84,
                  "combobox", 0X85,
                  (PSTR)0};

static DDENTRY modstrtab[] = {
                  "dtStyle",                   0x00,"%04X%04X",2,
                  "dtItemCount",               0x04,"%u",0,
                  "dtX",                       0x05,"%u",1,
                  "dtY",                       0x07,"%u",1,
                  "dtCX",                      0x09,"%u",1,
                  "dtCY",                      0x0b,"%u",1,
                  "dtMenuName",                0x0d,"%s",0x0005,
                  "dtClassName",               0x0d,"%s",0x0105,
                  "dtCaption",                 0x0d,"%s",0x0205,
                  (PSTR)0};

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Liste Dialogbox fuellen                              บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int FillDlgTmp(HWND hDlg,LPVOID lpvoid, RESENTRY FAR * lpres)
{
 PSTR   pstr;
 LPSTR  lpstr,lpstr1,lpstr2,lpstr3,lpstr4,lpstr5;
 char str[256];
 char str1[10];
 char str2[10];
 DLGITEMTEMPLATE FAR * lpdlgientry;
 LPDLGTEMPLATE lpdlgtmp;
 WORD  i,j,tabs,tabpos[7];

 tabpos[0] = 24;       /* style */
 tabpos[1] = 64;       /* class */
 tabpos[2] = 104;      /* x */
 tabpos[3] = 122;      /* y */
 tabpos[4] = 140;      /* lx */
 tabpos[5] = 158;      /* ly */
 tabpos[6] = 176;      /* text */

 SendDlgItemMessage(hDlg,ID_DLGINFO,LB_SETTABSTOPS,7,(LPARAM)(LPVOID)&tabpos);
 LoadString(hInst,IDS_RESVIEW1,str,sizeof(str));
 SetDlgItemText(hDlg,ID_DLGINFO,str);

 lpdlgtmp = (LPDLGTEMPLATE)lpvoid;
                                            /* 1. statuszeile aufbereiten */
 tabpos[0] = 28;
 tabpos[1] = 68;
 tabpos[2] = 94;
 tabpos[3] = 114;
 tabpos[4] = 144;
 tabpos[5] = 194;
 SendDlgItemMessage(hDlg,ID_DLGSTAT1,ST_SETTABSTOPS,6,(LPARAM)(LPVOID)&tabpos);
 wsprintf(str,
          "Style\t%lX\tItems\t%u\tKoord\t%u,%u,%u,%u\t",
          lpdlgtmp->dtStyle,
          lpdlgtmp->dtItemCount,
          lpdlgtmp->dtX,
          lpdlgtmp->dtY,
          lpdlgtmp->dtCX,
          lpdlgtmp->dtCY
         );
 SetDlgItemText(hDlg,ID_DLGSTAT1,str);

 lpstr = (LPSTR)&lpdlgtmp->dtMenuName;
 lpstr5 = lpstr;
 lpstr = lpstr + lstrlen(lpstr) + 1;          /* menu name ueberspringen */

 lpstr1 = str;
 if (*lpstr)
    wsprintf(lpstr1,"Klasse\t%s\t",(LPSTR)lpstr);
 else
    lstrcpy(lpstr1,"Klasse\t<Std>\t");

 tabs = 2;
 tabpos[0] = 34;
 tabpos[1] = 92;
 tabpos[2] = 120;
 tabpos[3] = 180;

 lpstr1 = lpstr1 + lstrlen(lpstr1);
 lpstr = lpstr + lstrlen(lpstr) + 1;          /* class name ueberspringen */
 lpstr2 = lpstr;                              /* caption merken */
 lpstr = lpstr + lstrlen(lpstr) + 1;          /* caption ueberspringen */
                                              /* font ausgeben in 2. statuszeile */
 if (*(LONG FAR *)lpvoid & DS_SETFONT)
    {
     wsprintf(lpstr1,
              "Font\t%s,%u\t",
              (LPSTR)lpstr+2,
              *(LPWORD)lpstr
             );
     lpstr1 = lpstr1 + lstrlen(lpstr1);
     lpstr = lpstr + 2;
     lpstr = lpstr + lstrlen(lpstr) + 1;
     tabs = tabs + 2;
    }
                                            /* eventuelles menu ausgeben */
 if (*lpstr5)
    {
     if (*(LPBYTE)lpstr5 == 0xFF)
        {
         lpstr5++;
         wsprintf(lpstr1,"Menü\t%u",*(LPWORD)lpstr5);
        }
     else
         wsprintf(lpstr1,
                  "Menü\t%s",
                  (LPSTR)lpstr5
                 );
     tabpos[tabs] = tabpos[tabs-1] + 30;
     tabs++;
     tabpos[tabs] = tabpos[tabs-1] + (lstrlen(lpstr1) - 4) * 6;
     tabs++;
     lpstr1 = lpstr1 + lstrlen(lpstr1);
    }

 SendDlgItemMessage(hDlg,ID_DLGSTAT2,ST_SETTABSTOPS,tabs,(LPARAM)(LPVOID)&tabpos);
 SetDlgItemText(hDlg,ID_DLGSTAT2,str);
                                              /* den caption text in die ueberschrift */
 if (*lpstr2)
    wsprintf(str,"Dialog %s - \"%s\"",(LPSTR)lpres->idname,(LPSTR)lpstr2);
 else
    wsprintf(str,"Dialogtemplate %s",(LPSTR)lpres->idname);

 SetWindowText(hDlg,str);
                                            /* jetzt die einzelnen zeilen aufbereiten */
 for (i=0;i <  *((LPBYTE)lpvoid+4);i++)
    {
     lpdlgientry = (LPDLGITEMTEMPLATE)lpstr;
     if (lpdlgientry->dtilClass[0] & 0x80)
        {
         lpstr = lpstr + 15;                     /* classname ist hier nur 1 byte */
         if (pstr = SearchKeyTable((char *)keytab1,(WORD)(BYTE)lpdlgientry->dtilClass[0]))
             lpstr4 = pstr;
         else
            {
             wsprintf(str1,"%X",(WORD)(BYTE)lpdlgientry->dtilClass[0]);
             lpstr4 = str1;
            }
        }
     else
        {
         lpstr = lpstr + 14;
         lpstr4 = lpstr;
         lpstr = lpstr + lstrlen(lpstr) + 1;     /* hinter classenname springen */
        }
                                                 /* hinter windowtext springen */
     if (*(LPBYTE)lpstr == 0xFF)
        {
         lpstr++;
         wsprintf(str2,"%d",*(LPWORD)lpstr);
         lpstr3 = str2;
         lpstr++;
         lpstr++;
        }
     else
        {
         lpstr3 = lpstr;
         lpstr = lpstr + lstrlen(lpstr) + 1;
        }

     wsprintf(str,
              "%d\t%lX\t%s\t%u\t%u\t%u\t%u\t%s",
              lpdlgientry->dtilID,
              lpdlgientry->dtilStyle,
              (LPSTR)lpstr4,                    /* classe */
              lpdlgientry->dtilX,
              lpdlgientry->dtilY,
              lpdlgientry->dtilCX,
              lpdlgientry->dtilCY,
              (LPSTR)lpstr3                      /* text */
             );

     j = strlen(str) + 30;
     if (j > maxlen)
         maxlen = j;

     SendDlgItemMessage(hDlg,ID_DLGINFO,LB_ADDSTRING,0,(LONG)(LPSTR)str);
     lpstr = lpstr + *(LPBYTE)lpstr + 1;         /* hinter infobytes springen */
    }

 return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Liste Strings fuellen                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int FillStrings(HWND hDlg,LPVOID lpdlgtmp,RESENTRY FAR * lpres)
{
 LPBYTE lpbyte;
 PSTR   pstr;
 char str[256];
 WORD i,j,maxstr;
 WORD tabpos[2];

 tabpos[0] = 24;

 SendDlgItemMessage(hDlg,ID_DLGINFO,LB_SETTABSTOPS,1,(LPARAM)(LPVOID)&tabpos);
 LoadString(hInst,IDS_RESVIEW2,str,sizeof(str));
 SetDlgItemText(hDlg,ID_DLGINFO,str);

 lpbyte = (LPBYTE)lpdlgtmp;
 for (i = 0;i < 16; i++)
    {
     if (*lpbyte)
        {
         wsprintf(str,"%X\t",i + (lpres->id - 1) * 16);
         pstr = str + strlen(str);
         maxstr = 0;
         for (j = *lpbyte,lpbyte++;j;j--,lpbyte++)
              if (LOWORD(lpbyte) > limit)
                 {
                  CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRDLG1),0,MB_OK);
                  return -1;
                 }
              else
              if (*lpbyte == 13)
                 ;
              else
              if (*lpbyte != 10)
                 {
                  *pstr = *lpbyte;
                  pstr++;
                  maxstr++;
                 }
              else
                 {
                  if (maxstr > maxlen)
                      maxlen = maxstr;
                  maxstr = 0;
                  *pstr = 0;
                  SendDlgItemMessage(hDlg,ID_DLGINFO,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                  pstr = str;
                  *pstr = 9;
                  pstr++;
                 }

         if (maxstr > maxlen)
             maxlen = maxstr;
         *pstr = 0;
         SendDlgItemMessage(hDlg,ID_DLGINFO,LB_ADDSTRING,0,(LONG)(LPSTR)str);
        }
     else
        lpbyte++;
    }
 wsprintf(str,
          "Strings %u-%u(%X-%X)",
          (lpres->id - 1) * 16,
          (lpres->id - 1) * 16 + 15,
          (lpres->id - 1) * 16,
          (lpres->id - 1) * 16 + 15
         );
 SetWindowText(hDlg,str);

 tabpos[0] = 68;
 tabpos[1] = 88;
 SendDlgItemMessage(hDlg,ID_DLGSTAT1,ST_SETTABSTOPS,2,(LPARAM)(LPVOID)&tabpos);
 wsprintf(str,
          "max. Stringgr๖฿e\t%u\t",
          maxlen
         );
 SetDlgItemText(hDlg,ID_DLGSTAT1,str);

 return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Liste Menubox fuellen                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int FillMenu(HWND hDlg,LPVOID lpvoid, RESENTRY FAR * lpres)
{
 LPBYTE lpbyte;
 char str[100];
 char str1[100];
 char tabstr[8];
 char * pstr;
 int offs;
 int stufe;
 WORD tabpos[8];
 LPMENUITEMTEMPLATE lpmenuitem;

 tabpos[0] = 28;
 tabpos[1] = 56;
 tabpos[2] = 64;
 tabpos[3] = 72;
 tabpos[4] = 80;
 tabpos[5] = 88;
 tabpos[6] = 96;
 tabpos[7] = 104;
 SendDlgItemMessage(hDlg,ID_DLGINFO,LB_SETTABSTOPS,8,(LPARAM)(LPVOID)&tabpos);
 LoadString(hInst,IDS_RESVIEW3,str,sizeof(str));
 SetDlgItemText(hDlg,ID_DLGINFO,str);

 if (*(LPINT)lpvoid)                      /* alte menues nicht anzeigen */
   {
    CreateMessageParam(hDlg,MAKEINTRESOURCE(IDS_ERRDLG2),0,MB_OK,IDOK);
    return 0;
   }
 offs   = LOWORD((LPINT)lpvoid+1);
 lpmenuitem = (LPMENUITEMTEMPLATE)((LPBYTE)lpvoid+offs+2);
 stufe = 1;
 pstr = tabstr;
 while (stufe)
    {
     *pstr = 0;
     if (lpmenuitem->mtOption & MF_POPUP)
        {
         lpbyte = (LPBYTE)lpmenuitem + 2;
         wsprintf(str1,
                  "%X\t\t%s%s",
                  lpmenuitem->mtOption,
                  (LPSTR)tabstr,
                  lpbyte
                 );
         stufe++;
         *pstr = 9;
         pstr++;
        }
     else
        {
         lpbyte = (LPBYTE)lpmenuitem + 4;
         wsprintf(str1,
                  "%X\t%u\t%s%s",
                  lpmenuitem->mtOption,
                  lpmenuitem->mtID,
                  (LPSTR)tabstr,
                  lpbyte
                 );
         if (lpmenuitem->mtOption & MF_END)
             pstr = tabstr + stufe - 2;
        }
     if (lpmenuitem->mtOption & MF_END)
         stufe--;

     _fstrchng(str,str1,"\x8","\\a");
     SendDlgItemMessage(hDlg,ID_DLGINFO,LB_ADDSTRING,0,(LONG)(LPSTR)str);

     lpbyte = lpbyte + lstrlen((LPSTR)lpbyte) + 1;
     lpmenuitem = (LPMENUITEMTEMPLATE)lpbyte;
    }

 wsprintf(str,"Menu \"%s\"",(LPSTR)lpres->idname);
 SetWindowText(hDlg,str);

 return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Liste Trans   fuellen                                บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int FillTrans(HWND hDlg,LPVOID lpdlgtmp,RESENTRY FAR * lpres)
{
 LPBYTE lpbyte;
 char str[256];
 WORD i;
 WORD tabpos[4];

 tabpos[0] = 24;
 tabpos[1] = 48;
 tabpos[2] = 72;

 SendDlgItemMessage(hDlg,ID_DLGINFO,LB_SETTABSTOPS,3,(LPARAM)(LPVOID)&tabpos);
 LoadString(hInst,IDS_RESVIEW4,str,sizeof(str));
 SetDlgItemText(hDlg,ID_DLGINFO,str);

 lpbyte = (LPBYTE)lpdlgtmp;

 i = 0;
 while (*(LPINT)lpbyte)
     {
      wsprintf(str,
          "%X\t%u\t%s",
          *(LPINT)(lpbyte+2),
          *(LPINT)(lpbyte+4) & 0x7FFF,
          (LPSTR)(lpbyte+7)
         );
     SendDlgItemMessage(hDlg,ID_DLGINFO,LB_ADDSTRING,0,(LONG)(LPSTR)str);
     lpbyte = lpbyte + *(LPINT)lpbyte;
     i++;
     }

 wsprintf(str,"%s",(LPSTR)lpres->idname);
 SetWindowText(hDlg,str);

 tabpos[0] = 68;
 tabpos[1] = 88;
 SendDlgItemMessage(hDlg,ID_DLGSTAT1,ST_SETTABSTOPS,2,(LPARAM)(LPVOID)&tabpos);
 wsprintf(str,"Anzahl\t%u",i);
 SetDlgItemText(hDlg,ID_DLGSTAT1,str);

 return 1;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ ViewModul - 1 Modul anschauen                        บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK ViewDlgTemplate(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 HANDLE handle;
 WORD   typ;
 RESENTRY FAR * lpres;
 HDC hDC;
 TEXTMETRIC tm;
 WORD charw;
 LPVOID lpdlgtmp;
 BOOL rc = FALSE;
 int xrc;
 DWORD dw,wsize;
 RECT rect;
 HWND hWnd;
 int tmaxlen;


 switch (message)
   {
    case WM_COMMAND:
      switch (wParam)
        {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_LISTBOX1:
              switch (HIWORD(lParam))
                 {
                  case XLBN_RBUTTONDOWN:
//                    TrackPopup(hDlg,BtnTab);
                    break;
                 }
              break;
        }
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_INITDIALOG:
      lpres = (RESENTRY FAR *)lParam;
      handle = lpres->handle;
      typ    = lpres->typ;
      maxlen = 0;

      if (!(lpdlgtmp = LockResource(handle)))
          {
           CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRDLG3),0,MB_OK);
           break;
          }

      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      limit = (WORD)GetSelectorLimit(HIWORD(lpdlgtmp));
      SendDlgItemMessage(hDlg,ID_DLGINFO,LB_RESETCONTENT,0,0);

      switch (typ)
         {
          case GD_MENU:                           /* menus */
            xrc = FillMenu(hDlg,lpdlgtmp,lpres);
            break;
          case GD_DIALOG:                         /* dialogboxen */
            xrc = FillDlgTmp(hDlg,lpdlgtmp,lpres);
            break;
          case GD_STRING:                         /* strings */
            xrc = FillStrings(hDlg,lpdlgtmp,lpres);
            break;
          case GD_NAMETABLE:                      /* "translate" tabelle */
            xrc = FillTrans(hDlg,lpdlgtmp,lpres);
            break;
          default:
            xrc = FALSE;
         }

      hDC = GetDC(hDlg);
      GetTextMetrics(hDC,&tm);
      charw = tm.tmAveCharWidth;
      ReleaseDC(hDlg,hDC);
      UnlockResource(handle);
      if  (maxlen > 30)
         {
          maxlen = maxlen + 6;
          tmaxlen = min(80,maxlen);
          GetClientRect(hDlg,&rect);
          rect.right = rect.left + tmaxlen * charw;
          AdjustWindowRect(&rect,WS_CAPTION | WS_BORDER,0);
          SetWindowPos(hDlg,0,0,0,
                       rect.right - rect.left,
                       rect.bottom - rect.top,
                       SWP_NOZORDER | SWP_NOMOVE);

          hWnd = GetDlgItem(hDlg,ID_DLGINFO);
          GetWindowRect(hWnd,&rect);
          SetWindowPos(hWnd,0,0,0,
                       (tmaxlen-2) * charw,
                       rect.bottom - rect.top,
                       SWP_NOZORDER | SWP_NOMOVE);

          if (typ == GD_STRING)
              if (maxlen > 80)
                  SendMessage(hWnd,LB_SETHORIZONTALEXTENT,maxlen * charw,0);

//          hWnd = GetDlgItem(hDlg,ID_DLGTMPS1);
//          GetWindowRect(hWnd,&rect);
//          SetWindowPos(hWnd,0,0,0,
//                       (tmaxlen-2) * charw,
//                       rect.bottom - rect.top,
//                       SWP_NOZORDER | SWP_NOMOVE);

          hWnd = GetDlgItem(hDlg,ID_DLGSTAT1);
          GetWindowRect(hWnd,&rect);
          SetWindowPos(hWnd,0,0,0,
                       tmaxlen * charw,
                       rect.bottom - rect.top,
                       SWP_NOZORDER | SWP_NOMOVE);
          if (typ == GD_DIALOG)
             {
              hWnd = GetDlgItem(hDlg,ID_DLGSTAT2);
              GetWindowRect(hWnd,&rect);
              SetWindowPos(hWnd,0,0,0,
                           tmaxlen * charw,
                           rect.bottom - rect.top,
                           SWP_NOZORDER | SWP_NOMOVE);
             }
          hWnd = GetDlgItem(hDlg,IDOK);
          dw = GetClientSize(hDlg);
          wsize = GetWindowSize(hWnd);
          SetWindowPos(hWnd,0,
                       LOWORD(dw) - LOWORD(wsize) - 4,
                       HIWORD(dw) - HIWORD(wsize) - 0,
                       0,
                       0,
                       SWP_NOZORDER | SWP_NOSIZE);
         }

      if (typ == GD_DIALOG)
         {
          hWnd = GetDlgItem(hDlg,ID_DLGSTAT2);
          dw = GetWindowSize(hWnd);
          GetClientRect(hDlg,&rect);
          rect.bottom = rect.bottom + HIWORD(dw);
          AdjustWindowRect(&rect,WS_CAPTION | WS_BORDER,0);
          SetWindowPos(hDlg,0,0,0,
                       rect.right - rect.left,
                       rect.bottom - rect.top,
                       SWP_NOZORDER | SWP_NOMOVE);
         }

      if (xrc == -1)
         ShowWindow(hDlg,SW_SHOWNOACTIVATE);
      else
      if (xrc)
         ShowWindow(hDlg,SW_SHOWNORMAL);

      rc = TRUE;
      break;
   }
 return rc;
}
