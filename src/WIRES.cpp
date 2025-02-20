
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
//#define strcpy lstrcpy
//#define strcat lstrcat
//#define strlen lstrlen

BOOL CALLBACK ViewDlgTemplate  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ViewDlg          (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ShowResDlg       (HWND, UINT, WPARAM, LPARAM);

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT   hFontAlt;
extern int fUnloadwarning;                   /* warnungen bei kritischen aktionen ausgeben */
extern BYTE fShow;
extern LPSTR lpszHint;

int xRespos   = 100,yRespos   = 100;

static char * pszResourcen = {"Resourcen"};

static char * presstr[] = {   "Userdef",                  /* 0000 = UNDEF */
                       "CsrComp",                         /* 8001 */
                       "Bitmap",                          /* 8002 */
                       "IconComp",                        /* 8003 */
                       "Menu",                            /* 8004 */
                       "Dialog",
                       "Strings",
                       "Fontdir",
                       "Font",                            /* 8008 */
                       "AccelTab",
                       "RCData",
                       "Errtab",                          /* 800B */
                       "Cursor",                          /* 800C */
                       "<800D>",
                       "Icon",
                       "NameTab",                         /* 800F */
                       "<user>"};

/*
浜様様様様様様様様様様様様様様様様様様様様様様様様様様融
� CheckModul - prufen ob modul noch da                 �
藩様様様様様様様様様様様様様様様様様様様様様様様様様様夕
*/
BOOL static CheckModul(HWND hDlg,HMODULE * pModul,MODULEENTRY * pmodentry)
{
  int tabpos[2];

  *pModul = (HMODULE)GetWindowWord(hDlg,DLGWINDOWEXTRA);
  (*pmodentry).dwSize = sizeof(MODULEENTRY);
  if (ModuleFindHandle(pmodentry,*pModul))
      return TRUE;

  EnableDlgItem(hDlg,ID_SUBDLG3,0);    /* detail view */
  EnableDlgItem(hDlg,ID_SUBDLG1,0);    /* view */
  EnableDlgItem(hDlg,ID_SUBDLG2,0);    /* show */
  EnableDlgItem(hDlg,ID_SUBDLG4,0);    /* free */
  EnableDlgItem(hDlg,ID_REFRESH,0);

  tabpos[0] = 10;
  tabpos[1] = 200;
  SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,2,(LPARAM)(LPVOID)&tabpos);
  SetDlgItemText(hDlg,ID_STATUS1,"\tModul ist nicht mehr geladen!");

  return FALSE;
}
/*
浜様様様様様様様様様様様様様様様様様様様様様様様様様様融
� ResDlg - Resource Dialog                             �
藩様様様様様様様様様様様様様様様様様様様様様様様様様様夕
*/
BOOL EXPORTED CALLBACK ResDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
 static int BtnTab[] = {ID_SUBDLG1,ID_SUBDLG2,ID_SUBDLG3,ID_SUBDLG4,0};
 char   szStr[100];
 char   str[80];
 WORD   j,x,y,typ,id,wHandles;
 int	i;
 HWND   hWnd;
 DWORD  dw[2],dwLength,dwLength1;
 MODULEENTRY moduleentry;
 HEXDUMPPARM hdp;
 RESENTRY res;
 HMODULE hModule;
 HRSRC  hRes;
 int    hFile;
 LPSTR  lpstr,lpstr2;
 RECT   rect;
 BOOL rc = FALSE;

 switch (message)
   {
    case WM_INITDIALOG:
      SetWindowLong(hDlg,DLGWINDOWEXTRA,lParam);
      LoadTabs(IDUS_7,str);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,
                         (LPARAM)(LPVOID)hFontAlt);
      LoadTabs(IDUS_8,str);
      SendDlgItemMessage(hDlg,ID_STATUS2,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
      LoadTabs(IDUS_9,str);
      SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));

      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      if (fShow)
         {
          SetWindowPos(hDlg,0,xRespos,yRespos,0,0,SWP_NOSIZE | SWP_NOZORDER);
          ShowWindow(hDlg,SW_NORMAL);
         }
      else
          PostMessage(hDlg,WM_CLOSE,0,0);
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xRespos,&yRespos);
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
         case IDOK:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_SNAPSHOT:
              SetCursor(hCursorWait);
              GetTempFileName(0,"RES",0,szStr);
              hFile = _lcreat(szStr,0);
              GetWindowText(hDlg,str,sizeof(str));
              _lwriteline(hFile,str);
              GetDlgItemText(hDlg,ID_LISTBOX1,str,sizeof(str));
              _lwriteline(hFile,str);
              SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SAVEBOX,hFile,0);
              GetDlgItemText(hDlg,ID_STATUS2,str,sizeof(str));
              _lwriteline(hFile,str);
              _lclose(hFile);
              PutFiletoClipboard(szStr,hDlg);
              wsprintf(str,"Gespeichert in\n%s",(LPSTR)szStr);
              CreateMessage(hDlg,str,lpszHint,MB_OK);
              SetCursor(hCursor);
              break;
         case ID_REFRESH:
              fShow = FALSE;
              if (!CheckModul(hDlg,&hModule,&moduleentry))
                  break;

              SetCursor(hCursorWait);
              GetModuleName(hModule,str,sizeof(str));
              wsprintf(szStr,"%s %s",(LPSTR)pszResourcen,(LPSTR)str);
              SetWindowText(hDlg,szStr);

              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);
              x = 0;
              wHandles = 0;
              dwLength = 0;
              dwLength1 = 0;
              y = ResourceFirst(hModule,&res);
              while (y)
                 {
                  if (res.typ < 0x0010)
                      i = res.typ;
                  else                   /* typen < 0x100 sind reserviert */
                  if (res.typ < 0x0100)
                      i = 0;
                  else                   /* alle andern sind user typen */
                      i = 0x10;

                  wsprintf(szStr,
                           "%s(%s)\t%s\t%X\t%X\t%X\t%X\t%X",
                           (LPSTR)presstr[i],    /* resource typ Klartext */
                           (LPSTR)res.typname,   /* resource typ Wert */
                           (LPSTR)res.idname,    /* resource id name*/
                           res.handle,           /* handle */
                           res.count,            /* counts */
                           res.flags,            /* flags */
                           res.length<<4,        /* laenge in paras */
                           res.offs<<4           /* offset */
                          );
                  i = (int)SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM)(LPSTR)szStr);
                  SendMessage(hWnd, LB_SETITEMDATA, i, MAKELONG(res.typ,res.id) );
                  x++;
                  dwLength = dwLength + res.length * 0x0010;
                  if (res.handle)
                    {
                     wHandles++;
                     dwLength1 = dwLength1 + res.length * 0x0010;
                    }
                  y = ResourceNext(hModule,&res);
                 }
              SendMessage(hWnd,LB_SETCURSEL,0,0);
              wsprintf(str,
                       "Objekte\t%u\tHandles\t%u\tBytes(gel)\t%lX(%lX)\t",
                       x,
                       wHandles,
                       dwLength,
                       dwLength1
                      );
              SetDlgItemText(hDlg,ID_STATUS2,str);
              SetCursor(hCursor);
              fShow = TRUE;
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                 {
                  case LBN_SELCHANGE:                       /* selektion geaendert */
                     if (!CheckModul(hDlg,&hModule,&moduleentry))
                         break;

                     hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
                     i = (int)SendMessage(hWnd, LB_GETCURSEL,0,0);
                     dw[0] = SendMessage(hWnd, LB_GETITEMDATA, i,0);
                     typ = LOWORD(dw[0]);
                     id = HIWORD(dw[0]);
                     res.id = id;
                     res.typ = typ;

                     if (!ResourceFindID(hModule,&res))
                        {
//                         MessageBox(hDlg,"???",0,MB_OK);
                         break;
                        }

                     if (res.typ < 0x0010)
                         j = res.typ;
                     else                   /* typen < 0x100 sind reserviert */
                     if (res.typ < 0x0100)
                         j = 0;
                     else                   /* alle andern sind user typen */
                         j = 0x10;

                     wsprintf(szStr,
                              "%s(%s)\t%s\t%X\t%X\t%X\t%X\t%X",
                              (LPSTR)presstr[j],   /* dessen Klartext */
                              (LPSTR)res.typname,  /* resource typ */
                              (LPSTR)res.idname,   /* resource name */
                              res.handle,          /* handle */
                              res.count,           /* counts */
                              res.flags,           /* flags */
                              res.length<<4,       /* laenge */
                              res.offs<<4          /* offset */
                             );

                     SendMessage(hWnd,XLB_REPLACESTRING,i,(LONG)(LPSTR)szStr);
                     SendMessage(hWnd,LB_SETITEMDATA,i,dw[0]);
//                     ReplaceItem(hWnd,i,szStr,dw[0]);

                     EnableWindow(GetDlgItem(hDlg,ID_SUBDLG2), (typ == GD_CURSOR)
                                                            || (typ == GD_BITMAP)
                                                            || (typ == GD_MENU)
                                                            || (typ == GD_DIALOG)
#if COMPONENT
                                                            || (typ == GD_CURSORCOMPONENT)
                                                            || (typ == GD_ICONCOMPONENT)
#endif
                                                            || (typ == GD_ICON) );
                     EnableWindow(GetDlgItem(hDlg,ID_SUBDLG3),  (typ == GD_MENU)
                                                            || (typ == GD_DIALOG)
                                                            || (typ == GD_STRING)
                                                            || (typ == GD_NAMETABLE)
                                                            );
                     EnableWindow(GetDlgItem(hDlg,ID_SUBDLG4),(int)res.handle);
                     if (!GetFocus())                      /* falls focus auf deaktivem window */
                         SetFocus(GetDlgItem(hDlg,IDOK));


                     if (res.flags.preload)
                         strcpy(szStr,"preload");
                     else
                         strcpy(szStr,"loadoncall");

                     if (res.flags.moveable)
                         strcat(szStr,",moveable");
                     else
                         strcat(szStr,",fixed");

                     if (res.flags.discardable)
                         strcat(szStr,",discardable");

                     if  (res.handle)
                         if (GlobalFlags(res.handle) & GMEM_DISCARDED)
                             strcat(szStr,"\t\tdiscarded");
                         else
                            {
                             lpstr = szStr + strlen(szStr);
                             wsprintf(lpstr,"\t\tBase=%lX,Limit=%lX",
                                      GetSelectorBase(GlobalHandleToSel(res.handle)),
                                      GetSelectorLimit(GlobalHandleToSel(res.handle))
                                     );
                            }
                     wsprintf(str,
                              "Handler\t%X:%X\tAttr\t%s",
                              HIWORD(res.handler),
                              LOWORD(res.handler),
                              (LPSTR)szStr
                             );
                     SetDlgItemText(hDlg,ID_STATUS1,str);
                     break;
                  case LBN_DBLCLK:
                     if (!CheckModul(hDlg,&hModule,&moduleentry))
                         CreateMessage(hDlg,errorstr1,0,MB_OK);
                     else
                     if (IsWindowEnabled(GetDlgItem(hDlg,ID_SUBDLG3)))
                         PostMessage(hDlg,WM_COMMAND,ID_SUBDLG3,0);
                     else
                     if (IsWindowEnabled(GetDlgItem(hDlg,ID_SUBDLG2)))
                         PostMessage(hDlg,WM_COMMAND,ID_SUBDLG2,0);
                     else
                         PostMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                     break;
                  case XLBN_RBUTTONDOWN:
                     TrackPopup(hDlg,BtnTab);
                     break;
                 }
              break;
         case ID_SUBDLG3:    /* detail */
         case ID_SUBDLG1:    /* view (hexdump) */
         case ID_SUBDLG2:    /* show (interpret) */
         case ID_SUBDLG4:    /* free */
         case ID_WARNING1:
              if (!CheckModul(hDlg,&hModule,&moduleentry))
                  break;

              i = (int)SendDlgItemMessage(hDlg, ID_LISTBOX1, LB_GETCURSEL,0,0);
              dw[0] = SendDlgItemMessage(hDlg, ID_LISTBOX1, LB_GETITEMDATA, i,0);
              typ = LOWORD(dw[0]);
              id  = HIWORD(dw[0]);
              res.id = id;
              res.typ = typ;
              if (!ResourceFindID(hModule,&res))
                  break;

              i = HDP_RESHDL;
              if ((wParam == ID_SUBDLG4) && (res.handle))
                 {
                  if (fUnloadwarning)
                     {
                      CreateMessageParam(hDlg,
                                 MAKEINTRESOURCE(IDS_ERRRES1),
                                 MAKEINTRESOURCE(IDS_WARNING),
                                 MB_OKCANCEL,
                                 ID_WARNING1
                                 );
                      break;
                     }
                  else
                     {
                      PostMessage(hDlg,WM_COMMAND,ID_WARNING1,MAKELONG(0,IDOK));
                      break;
                     }
                 }
              else
              if ((wParam == ID_WARNING1) && (HIWORD(lParam) == IDOK))
                 {
                  if (res.handle)
                      FreeResource(res.handle);
                 }
              else
              if (!res.handle)                  /* falls resource erst zu finden ist */
                 {
                  if (id > 0x7FFF)
                      lpstr = res.idname;
                  else
                      lpstr = (LPSTR)MAKELONG(id,0);

                  if (typ > 0x7FFF)
                     {
                      GetResourceName(hModule,typ & 0x7FFF,str,sizeof(str));
                      lpstr2 = str;
                     }
                  else
                      lpstr2 = (LPSTR)MAKELONG(typ,0);

                  hRes = FindResource(hModule,lpstr,lpstr2);

                  if (hRes)
                     if (res.handle = LoadResource(hModule,hRes))
                         i = i | 0x8000;
                     else
                         CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRRES2),0,MB_OK);
                  else
                     CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRRES3),0,MB_OK);
                 }

              if (res.handle)
                  if (wParam == ID_SUBDLG1)
                     {
                      hdp.dwOffset = 0;
                      hdp.hGlobal  = res.handle;
                      hdp.dwLength = 0;
                      hdp.wType    = i;
                      hWnd = CreateDialogParam( hInst,
                                                MAKEINTRESOURCE(IDD_VIEW),
                                                hDlg,
                                                ViewDlg,
                                                (LPARAM)(LPVOID)&hdp
                                                );
                      wsprintf(str,"Resource %s",(LPSTR)res.idname);
                      SetWindowText(hWnd,str);
                     }
                  else
                  if (wParam == ID_SUBDLG2)
                     {
                      GetWindowRect(hDlg,&rect);
                      if (hWnd = CreateDialogParam(hInst,
                                                   MAKEINTRESOURCE(IDD_SHOWRES),
                                                   hDlg,
                                                   ShowResDlg,
                                                   (DWORD)(LPSTR)&res
                                                   ))
                         {
                          wsprintf(str,"%s",(LPSTR)res.idname);
                          SetWindowText(hWnd,str);
                         }
                      else
                          CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRRES4),0,MB_OK);
                     }
                  else
                  if (wParam == ID_SUBDLG3)
                     CreateDialogParam( hInst,
                                        MAKEINTRESOURCE(IDD_DLGTMP),
                                        hDlg,
                                        ViewDlgTemplate,
                                        (LONG)(LPSTR)&res
                                       );

                  PostMessage(hDlg,
                  			WM_COMMAND,
                  			ID_LISTBOX1,
                  			MAKELONG(GetDlgItem(hDlg,ID_LISTBOX1),LBN_SELCHANGE));

              break;
         }
        rc = TRUE;
        break;
    default:
      break;
    }
 return rc;
}
