
// Hexdumpanzeige Dialog

#ifdef _DEBUG
#define _TRACE_
#endif

#define SELINVIEW 1
#define ADDRINVIEW 1
#define DEB16FW 0 /* doesn't exist anymore */

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"

#define DESCRIPTORBASE(x) (DWORD)MAKELONG(x.base1,x.base2 | ((WORD)x.base3<<8))

// externals

extern LPSTR lpszHint;

BOOL WINAPI CheckDlgBoxIcon(HWND);

BOOL CALLBACK UnassembleDlg  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Unassemble32Dlg(HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK ViewDlgTemplate(HWND, UINT, WPARAM, LPARAM);
BOOL EXPORTED CALLBACK ViewDlg        (HWND, UINT, WPARAM, LPARAM);

typedef void (FAR PASCAL *FPINTERPRET)(LPSTR);

// globals

int xSelpos   = 100,ySelpos   = 100;
int xLinpos   = 100,yLinpos   = 100;

// locals

static char * pszUeb  = "Sel:\t %X\t Base:\t%lX\t Limit:\t%lX\t Attr:\t%04X\t\t%s\t";
static char szFStrDW[] = {" %lX %*c"};
static char szFStrW[]  = {" %X %*c"};
static char * restype[] = {
                        "Userdefined",
                        "Cursorcomponent",
                        "Bitmap",
                        "Iconcomponent",
                        "Menu",
                        "Dialog",
                        "String",
                        "Fontdir",
                        "Font",
                        "Accelerators",
                        "RCdata",
                        "Errtable",
                        "Cursor",
                        "???",
                        "Icon",
                        "Nametable"
                       };

#if SELINVIEW == 0
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Dialogfunktion "Selektoreingabe"                                  บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int EXPORTED CALLBACK SelektorDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
 static char str[20] = {"0"};
 char szStr[40];
 DWORD dw[2];
 WORD selektor,y;
 HWND hWnd;
 HEXDUMPPARM hdp;
 BOOL rc = FALSE;

   switch(message)
     {
      case WM_INITDIALOG:
         EnableWindow(GetParent(hDlg),0);
         SendDlgItemMessage(hDlg,ID_EDITSEL,EM_LIMITTEXT,4,0);
         SetDlgItemText(hDlg,ID_EDITSEL,str);
         SetWindowPos(hDlg,0,xSelpos,ySelpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
         ShowWindow(hDlg,SHOW_OPENWINDOW);
         rc = TRUE;
         break;
      case WM_CLOSE:
         EnableWindow(GetParent(hDlg),1);
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
      case WM_MOVE:
         SaveWindowPos(hDlg,&xSelpos,&ySelpos);
         break;
      case WM_COMMAND:
         switch(wParam)
           {
            case IDOK:
               GetDlgItemText(hDlg,ID_EDITSEL,str,sizeof(str));
               if (sscanf(str,szFStrW,&selektor) != 1)
                  {
                   MessageBeep(0);
                   break;
                  }
               if (!IsValidAddress((LPVOID)MAKELONG(0,selektor)))
                  {
                   MessageBeep(0);
                   break;
                  }
               y = selektor;
               hdp.dwOffset = 0;
               hdp.hGlobal  = y;
               hdp.dwLength = 0;
               hdp.wType    = 0;
               if (hWnd = CreateDialogParam(hInst,
                                            MAKEINTRESOURCE(IDD_VIEW),
                                            GetParent(hDlg),
                                            ViewDlg,
                                            (DWORD)&hdp
                                           ))
                  {
                   wsprintf(szStr,"Segment: %X",y);
                   SetWindowText(hWnd,szStr);
                   PostMessage(hDlg,WM_CLOSE,0,0);
                  }
               break;
            case IDCANCEL:
               PostMessage(hDlg,WM_CLOSE,0,0);
               break;
            default:
               break;
           }
         rc = TRUE;     /* WM_COMMAND immer RC TRUE */
         break;
      default:
         break;
     }
 return rc;
}
#endif
#if ADDRINVIEW == 0
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Dialogfunktion "lineare Addresse eingeben"                        บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int EXPORTED CALLBACK AddressDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
 static char str[20] = {"0"};
 char szStr[40];
 HEXDUMPPARM hdp;
 DWORD addr;
 DWORD dw[2];
 HWND  hWnd;
 BOOL rc = FALSE;

   switch(message)
     {
      case WM_INITDIALOG:
         EnableWindow(GetParent(hDlg),0);
         SendDlgItemMessage(hDlg,ID_EDITADDR,EM_LIMITTEXT,8,0);
         SetDlgItemText(hDlg,ID_EDITADDR,str);
         SetWindowPos(hDlg,0,xLinpos,yLinpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
         ShowWindow(hDlg,SHOW_OPENWINDOW);
         rc = TRUE;
         break;
      case WM_CLOSE:
         EnableWindow(GetParent(hDlg),1);
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
      case WM_MOVE:
         SaveWindowPos(hDlg,&xLinpos,&yLinpos);
         break;
      case WM_COMMAND:
         switch(wParam)
           {
            case IDOK:
               GetDlgItemText(hDlg,ID_EDITADDR,str,sizeof(str));
               if (sscanf(str,szFStrDW,&addr) != 1)
                  {
                   MessageBeep(0);
                   break;
                  }
               hdp.dwOffset = addr;
               hdp.dwLength = 0;
               hdp.wType    = HDP_LINEAR;
               if (hWnd = CreateDialogParam( hInst,
                                             MAKEINTRESOURCE(IDD_VIEW),
                                             GetParent(hDlg),
                                             ViewDlg,
                                             (DWORD)&hdp
                                            ))
                  {
                   wsprintf(szStr,"Lineare Adresse: %lX",addr);
                   SetWindowText(hWnd,szStr);
                  }
               PostMessage(hDlg,WM_CLOSE,0,0);
               break;
            case IDCANCEL:
               PostMessage(hDlg,WM_CLOSE,0,0);
               break;
            default:
               break;
           }
         rc = TRUE;     /* WM_COMMAND immer RC TRUE */
         break;
      default:
         break;
     }
 return rc;
}
#endif
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ View Options Dialog                                  บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK ViewOptionsDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 char szStr[20];
 char str[80];
 char str1[80];
 static char szFindString[20] = {0};
 HWND hWnd;
 DWORD dwParm;
 WORD i;
 WORD j,k;
 LPSTR lpStr;
 BOOL rc = FALSE;

   switch(message)
     {
      case WM_INITDIALOG:
         i = (WORD)lParam & 7;
         for (j=0,k=i;k;k=k>>1,j++);
            CheckRadioButton(hDlg,ID_RBTN1,ID_RBTN4,ID_RBTN1 + j);
         SetWindowWord(hDlg,DWL_USER,i);
//       SendDlgItemMessage(hDlg,ID_RBTN1 + j,BM_SETCHECK,1,0);
         SendDlgItemMessage(hDlg,ID_SUBDLG1,EM_LIMITTEXT,sizeof(szFindString)-1,0);
         SetDlgItemText(hDlg,ID_SUBDLG1,szFindString);
         ShowWindow(hDlg,SW_SHOWNORMAL);
         rc = TRUE;
         break;
      case WM_CLOSE:
         EnableDlgItem(GetParent(hDlg),ID_SUBDLG1,1);
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
      case WM_COMMAND:
         switch(wParam)
           {
            case ID_SUBDLG1: /* FindString */
               if (HIWORD(lParam) == EN_UPDATE)
                  {
                   GetDlgItemText(hDlg,ID_SUBDLG1,szStr,2);
                   EnableDlgItem(hDlg,ID_SUBDLG2,*szStr);
                   EnableDlgItem(hDlg,ID_SUBDLG3,0);
                  }
               break;
            case ID_SUBDLG3: /* Weitersuchen */
            case ID_SUBDLG2: /* Suchen */
               GetDlgItemText(hDlg,ID_SUBDLG1,szFindString,sizeof(szFindString));
               if (!szFindString[0])
                  {
                   MessageBeep(0);
                   break;
                  }
               hWnd = GetParent(hDlg);
               dwParm = SendDlgItemMessage(hWnd,ID_LISTBOX1,HD_SEARCHSTRING,wParam-ID_SUBDLG2,(LONG)(LPSTR)szFindString);
               if (dwParm == 0xFFFFFFFF)
                   ;
               else
               if (dwParm)
                  {
                   SendDlgItemMessage(hWnd,ID_LISTBOX1,HD_SETPOSITION,0,(LONG)dwParm-1);
                   EnableDlgItem(hDlg,ID_SUBDLG3,1);
                  }
               else
                  {
                   LoadString(hInst,IDS_ERRVIEB,str,sizeof(str));
                   wsprintf(str1,str,(LPSTR)szFindString);
                   CreateMessage(hDlg,str1,lpszHint,MB_OK);
                  }
               break;
            case ID_SUBDLG4:         /* speichern */
              lpStr = (LPSTR)SendDlgItemMessage(GetParent(hDlg),ID_LISTBOX1,HD_SAVE,0,0);
              if (lpStr)
                 {
                  wsprintf(str1,"Gespeichert unter\n%s",lpStr);
                  CreateMessage(hDlg,str1,lpszHint,MB_OK);
                 }
              break;
            case IDOK:               /* refresh */
               i = GetWindowWord(hDlg,DWL_USER);
               GetDlgItemText(hDlg,ID_SUBDLG1,szFindString,sizeof(szFindString));
               SendMessage(GetParent(hDlg),
                           WM_COMMAND,
                           ID_SUBDLG2,
                           i
                          );
               break;
            case IDCANCEL:
               PostMessage(hDlg,WM_CLOSE,0,0);
               break;
            case ID_RBTN5:
               PostMessage(GetParent(hDlg),WM_COMMAND,ID_SUBDLG6,0);
               PostMessage(hDlg,WM_COMMAND,IDCANCEL,0);
               break;
            case ID_RBTN1:
            case ID_RBTN2:
            case ID_RBTN3:
            case ID_RBTN4:
               switch (HIWORD(lParam))
                 {
                  case BN_CLICKED:
                    j = wParam - ID_RBTN1;
                    if (j)
                       i = 1 << (j-1);
                    else
                       i = 0;
                    CheckRadioButton(hDlg,ID_RBTN1,ID_RBTN4,wParam);
                    SetWindowWord(hDlg,DWL_USER,i);
                    PostMessage(hDlg,WM_COMMAND,IDOK,0);
                    break;
                 }
               break;
            default:
               break;
           }
         rc = TRUE;     /* WM_COMMAND immer RC TRUE */
         break;
      default:
         break;
     }
 return rc;
}
////////////////////////////////////////////////////////
// ViewDlg - xxxxxxx auflisten                          
////////////////////////////////////////////////////////
BOOL EXPORTED CALLBACK ViewDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
  static int tabpos[2] = {8,-8};
  static BOOL fFirst;
  static int wXViewpos,wYViewpos = 0;
  static int BtnTab[] = {
                         ID_SUBDLG7 | 0x8000,
                         IDS_BYTES,
                         ID_SUBDLG8 | 0x8000,
                         IDS_WORDS,
                         ID_SUBDLG9 | 0x8000,
                         IDS_DWORDS,
                         ID_SUBDLG10 | 0x8000,
                         IDS_TEXT,
                         -1,-1,
                         ID_SUBDLG6 | 0x8000,
                         IDS_DISASM,
                         ID_SUBDLG1,
                         ID_SUBDLG3,
//                         ID_SUBDLG5,
                         0};
  int i;
#if DEB16FW
  HINSTANCE hDeb16fw;
  FPINTERPRET fpInterpret = 0;
#endif
  HEXDUMPPARM FAR *lpw;
  HEXDUMPPARM hdp;
  GLOBALENTRY globalentry;
//  LPSTR lpstr;
  DWORD dwParm;
//  DWORD dwLength;
  WORD sel;
  char str[80];
  char szStr2[40];
  DESCRIPTOR desc;
//  SELECTOR tsel;
//  WORD rights;
  BOOL rc = FALSE;

 switch (message) {
 case WM_INITDIALOG:
#if DEB16FW
     fpInterpret = 0;
     if (hDeb16fw = GetModuleHandle("DEB16FWD"))
         fpInterpret = (FPINTERPRET)GetProcAddress(hDeb16fw,(LPSTR)7);
     EnableDlgItem(hDlg,ID_SUBDLG3,HIWORD(fpInterpret));
#else
     EnableDlgItem(hDlg,ID_SUBDLG3,0);
#endif
     TRACEOUT("ViewDlg: WM_INITDIALOG received");
     lpw = (LPHEXDUMPPARM)lParam;
     if (lpw->wType & HDP_RESHDL)
         LockResource(lpw->hGlobal);
     if (lpw->wType & (HDP_FREEHDL | HDP_RESHDL | HDP_DISABLESELEDIT))
     {
         ShowWindow(GetDlgItem(hDlg,ID_SUBDLG4),SW_HIDE);
         ShowWindow(GetDlgItem(hDlg,ID_SELINVIEW),SW_HIDE);
         ShowWindow(GetDlgItem(hDlg,ID_SUBDLG5),SW_HIDE);
         ShowWindow(GetDlgItem(hDlg,ID_ADDRINVIEW),SW_HIDE);
         //            EnableDlgItem(hDlg,ID_SUBDLG4,0);
     }
     fFirst = TRUE;
#if SELINVIEW
     SendDlgItemMessage(hDlg,ID_SUBDLG4,EM_LIMITTEXT,4,0);
     if (lpw->wType & HDP_LINEAR)
         str[0] = 0;
     else
         wsprintf(str,"%X",lpw->hGlobal);
     SetDlgItemText(hDlg,ID_SUBDLG4,str);
#endif
#if ADDRINVIEW
     SendDlgItemMessage(hDlg,ID_SUBDLG5,EM_LIMITTEXT,8,0);
     wsprintf(str,"%lX",lpw->dwOffset);
     SetDlgItemText(hDlg,ID_SUBDLG5,str);
#endif
     SendMessage(hDlg,WM_COMMAND,ID_REFRESH,lParam);

     //        SetParent(hDlg,hWndMain);        // vorsicht!!!!!!
     SetWindowWord(hDlg,GWW_HWNDPARENT,(WORD)hWndMain);

     if (wXViewpos)
         SetWindowPos(hDlg,
                      0,
                      wXViewpos,
                      wYViewpos,
                      0,0,
                      SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
     else
         ShowWindow(hDlg,SW_NORMAL);

     TRACEOUT("ViewDlg: WM_INITDIALOG handled normal");
     rc = TRUE;
     break;
 case WM_DESTROY:
     dwParm = GetWindowLong(hDlg,DWL_USER);
     if (HIWORD(dwParm) & HDP_RESHDL)
         UnlockResource((HGLOBAL)LOWORD(dwParm));
     if (HIWORD(dwParm) & HDP_FREEHDL)
         GlobalFree((HGLOBAL)LOWORD(dwParm));
     break;
 case WM_CLOSE:
     TRACEOUT("ViewDlg: WM_CLOSE received");
     DestroyWindow(hDlg);
     rc = TRUE;
     break;
 case WM_PAINT:
     rc = CheckDlgBoxIcon(hDlg);
     break;
 case WM_MOVE:
     SaveWindowPos(hDlg,&wXViewpos,&wYViewpos);
     break;
 case WM_COMMAND:
     switch (wParam) {
     case IDCANCEL:
     case IDOK:
         PostMessage(hDlg,WM_CLOSE,0,0);
         break;
     case ID_REFRESH:
         lpw = (LPHEXDUMPPARM)lParam;
         SetWindowLong(hDlg,DWL_USER,MAKELONG(lpw->hGlobal,lpw->wType));

         if (!SendDlgItemMessage(hDlg,ID_LISTBOX1,HD_RESETCONTENT,0,(LPARAM)(LPSTR)lpw))
         {
             rc = TRUE;
             break;
         }
         LoadTabs(IDUS_36,str);
         SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));

         if (lpw->wType & HDP_SETWINDOWTEXT)
         {
             if (lpw->wType & HDP_LINEAR)
                 wsprintf(str,"Lineare Adresse: %lX",lpw->dwOffset);
             else
             {
                 szStr2[0] = 0;
                 globalentry.dwSize = sizeof(GLOBALENTRY);
                 if (GlobalEntryHandle(&globalentry,(HGLOBAL)lpw->hGlobal) &&
                     globalentry.hOwner && (globalentry.hOwner < (HGLOBAL)0xFFF0))
                 {
                     if (globalentry.wFlags & GF_PDB_OWNER)
                         GetModuleName((HMODULE)GetModuleFromTask((HMODULE)globalentry.hOwner),
                                       szStr2,sizeof(szStr2));
                     else
                         GetModuleName((HMODULE)globalentry.hOwner,szStr2,sizeof(szStr2));
                     if (globalentry.wType == GT_RESOURCE)
                     {
                         strcat(szStr2,".Resource");
                         if (globalentry.wData <= GD_MAX_RESOURCE)
                         {
                             strcat(szStr2,".");
                             strcat(szStr2,restype[globalentry.wData]);
                         }
                     }
                     wsprintf(str,"Segment: %X (%s.%u)",
                              lpw->hGlobal,
                              (LPSTR)szStr2,
                              globalentry.wData
                             );
                 }
                 else
                     wsprintf(str,"Segment: %X",lpw->hGlobal);
             }
             SetWindowText(hDlg,str);
         }

         if (lpw->wType & HDP_LINEAR)
         {
             wsprintf(str,
                      pszUeb,
                      0,
                      lpw->dwOffset,
                      lpw->dwLength,
                      0,
                      (LPSTR)"Ok"
                     );
         } else {
             sel = GlobalHandleToSel(lpw->hGlobal);
             GetDescriptor(sel,&desc);
             wsprintf(str,
                      pszUeb,
                      sel,
                      DESCRIPTORBASE(desc),
                      GetSelectorLimit(sel),
                      desc.attr & 0xF0FF,
                      (LPSTR)"Ok"
                     );
         }
         SetDlgItemText(hDlg,ID_STATUS1,str);
         EnableDlgItem(hDlg,ID_SUBDLG1,1);
         //EnableDlgItem(hDlg,ID_SUBDLG3,1);

         //EnableDlgItem(hDlg,ID_SUBDLG6,!(lpw->wType & HDP_LINEAR));
         fFirst = FALSE;
         rc = TRUE;
         break;
     case ID_SUBDLG2:         /* moduswechsel */
         SendDlgItemMessage(hDlg,ID_LISTBOX1,HD_SETMODE,(WORD)lParam,0);
         break;
     case ID_SUBDLG7:         /* format byte */
         SendDlgItemMessage(hDlg,ID_LISTBOX1,HD_SETMODE,HDT_BYTE,0);
         break;
     case ID_SUBDLG8:         /* format word */
         SendDlgItemMessage(hDlg,ID_LISTBOX1,HD_SETMODE,HDT_WORD,0);
         break;
     case ID_SUBDLG9:         /* format dword */
         SendDlgItemMessage(hDlg,ID_LISTBOX1,HD_SETMODE,HDT_DWORD,0);
         break;
     case ID_SUBDLG10:        /* format text */
         SendDlgItemMessage(hDlg,ID_LISTBOX1,HD_SETMODE,HDT_TEXT,0);
         break;
     case ID_SUBDLG1:         /* optionen dialog */
         i = (int)SendDlgItemMessage(hDlg,ID_LISTBOX1,HD_GETMODE,0,0);
         CreateDialogParam(hInst,
                           MAKEINTRESOURCE(IDD_VIEWOPT),
                           hDlg,
                           ViewOptionsDlg,
                           i
                          );
         EnableDlgItem(hDlg,ID_SUBDLG1,0);
         break;
     case ID_SUBDLG4:    // ID_SELMEM
#if SELINVIEW
         if (fFirst)
             break;
         if (HIWORD(lParam) == EN_UPDATE)
         {
             dwParm = GetWindowLong(hDlg,DWL_USER);
             hdp.wType   = HIWORD(dwParm);
             GetDlgItemText(hDlg,ID_SUBDLG4,str,sizeof(str));
             sel = 0;
             if (sscanf(str,szFStrW,&sel) > 1)
             {
                 MessageBeep(0);
                 break;
             }
             if (sel)
             {
                 if (!IsValidAddress((LPVOID)MAKELONG(0,sel)))
                     break;
                 hdp.wType = hdp.wType & ~HDP_LINEAR;
                 hdp.dwLength = 0;
             }
             else
             {
                 hdp.wType = HDP_LINEAR;
                 hdp.dwLength = -1;
             }
             hdp.wType = hdp.wType | HDP_SETWINDOWTEXT;
             SetDlgItemText(hDlg,ID_SUBDLG5,"0");
             hdp.dwOffset = 0;
             hdp.hGlobal = (HGLOBAL)sel;
             SendMessage(hDlg,WM_COMMAND,ID_REFRESH,(LPARAM)(LPVOID)&hdp);
             InvalidateRect(GetDlgItem(hDlg,ID_LISTBOX1),0,1);
         }
#else
         CreateDialogParam(hInst,
                           MAKEINTRESOURCE(IDD_SELECTOR),
                           hDlg,
                           SelektorDlg,
                           (LONG)0
                          );
#endif
         break;
     case ID_SUBDLG5:  // ID_ADDRMEM
#if ADDRINVIEW
         if (fFirst)
             break;
         if (HIWORD(lParam) == EN_UPDATE)
         {
             dwParm = GetWindowLong(hDlg,DWL_USER);
             hdp.hGlobal = (HGLOBAL)LOWORD(dwParm);
             hdp.wType   = HIWORD(dwParm);
             GetDlgItemText(hDlg,ID_SUBDLG5,str,sizeof(str));
             if (sscanf(str,szFStrDW,&hdp.dwOffset) != 1)
             {
                 MessageBeep(0);
                 break;
             }
             if (hdp.wType & HDP_LINEAR)
             {
                 hdp.dwLength = 0 - hdp.dwOffset - 1;
                 hdp.wType = hdp.wType | HDP_SETWINDOWTEXT;
             }
             else
                 hdp.dwLength = 0;

             SendMessage(hDlg,WM_COMMAND,ID_REFRESH,(LPARAM)(LPVOID)&hdp);
             InvalidateRect(GetDlgItem(hDlg,ID_LISTBOX1),0,1);
         }
#else
         CreateDialogParam(hInst,
                           MAKEINTRESOURCE(IDD_ADDRESS),
                           hDlg,
                           AddressDlg,
                           (LONG)0
                          );
#endif
         break;
     case ID_SUBDLG3:  // EDIT
#if DEB16FW
         if (hDeb16fw = GetModuleHandle("DEB16FWD"))
             if (fpInterpret = (FPINTERPRET)GetProcAddress(hDeb16fw,(LPSTR)7)) {
                 dwParm = GetWindowLong(hDlg,DWL_USER);
                 if (HIWORD(dwParm) & HDP_LINEAR)
                     wsprintf(str,
                              "ty %%%lX,%lX ;q",
                              SendDlgItemMessage(hDlg,
                                                 ID_LISTBOX1,
                                                 HD_GETOFFSET,
                                                 0,0),
                              SendDlgItemMessage(hDlg,
                                                 ID_LISTBOX1,
                                                 HD_GETSIZE,
                                                 0,0)
                             );
                 else
                     wsprintf(str,"ty %X:0 ;q",LOWORD(dwParm));
                 fpInterpret((LPSTR)str);
                 DebugBreak();
                 break;
             }
         CreateMessage(hDlg,"DEB16FWD.7 kann nicht aufgerufen werden",0,MB_OK);
#endif
         break;
     case ID_SUBDLG6:  // ID_DISASS
         FWORD fw;
         UINT wAttr;
         dwParm = GetWindowLong(hDlg,DWL_USER);
         fw.offset = SendDlgItemMessage(hDlg,
                                        ID_LISTBOX1,
                                        HD_GETOFFSET,
                                        0,
                                        0);
         fw.selector = LOWORD(dwParm);
         //wsprintf(str,"Addr=%X:%lX",fw.selector,fw.offset);
         //MessageBox(0,str,0,MB_OK);
         GetAttrofSelector(fw.selector,&wAttr);
         if (HIWORD(dwParm) & HDP_LINEAR)
             fw.selector = 0;
         if ((!fw.selector) || (wAttr & 0x4000)) {
             //MessageBox(hDlg,"32 Bit",0,MB_OK);
             CreateDialogParam(hInst,
                               MAKEINTRESOURCE(IDD_UNASSEM),
                               hDlg,
                               Unassemble32Dlg,
                               (LONG)(LPFWORD)&fw
                              );
         } else {
             //MessageBox(hDlg,"16 Bit",0,MB_OK);
             CreateDialogParam(hInst,
                               MAKEINTRESOURCE(IDD_UNASSEM),
                               hDlg,
                               UnassembleDlg,
                               (LONG)MAKELONG(LOWORD(fw.offset),fw.selector)
                              );
         }
         break;
     case ID_SUBDLG11:  // Resource
         RESENTRY res;
         res.handle = (HANDLE)GetWindowWord(hDlg,DWL_USER);
         globalentry.dwSize = sizeof(GLOBALENTRY);
         if (!GlobalEntryHandle(&globalentry,(HGLOBAL)res.handle))
             break;
         if (globalentry.wType != GT_RESOURCE)
             break;
         res.typ    = globalentry.wData;
         res.hModule = GetExePtr((HGLOBAL)res.handle);
         res.idname[0] = 0;
         res.id      = 1;
         CreateDialogParam(hInst,
                           MAKEINTRESOURCE(IDD_DLGTMP),
                           hDlg,
                           ViewDlgTemplate,
                           (LONG)(LPSTR)&res
                          );
         break;
     case ID_LISTBOX1: /* this is the ID of the hexdump control! */
#ifdef _TRACE_
         wsprintf(str,"ViewDlg.ID_ListBox1: lParam=%lX\r\n",lParam);
         OutputDebugString(str);
#endif
         i = 0;
         switch (HIWORD(lParam))
         {
         case HEXN_ERROR1:
             i = IDS_ERRVIE1;
             break;
         case HEXN_ERROR9:
             i = IDS_ERRVIE2;
             break;
         case HEXN_ERROR2:
             i = IDS_ERRVIE3;
             break;
         case HEXN_ERROR3:
             i = IDS_ERRVIE4;
             break;
         case HEXN_ERROR4:
             i = IDS_ERRVIE5;
             break;
         case HEXN_ERROR5:
             i = IDS_ERRVIE6;
             break;
         case HEXN_ERROR8:
         case HEXN_ERROR6:
             sel = GetWindowWord(hDlg,DWL_USER);
             if (sel) {
                 sel = GlobalHandleToSel((HGLOBAL)sel);
                 DPMIGetDescriptor(sel,&desc);
             }
             if (LOWORD(lParam) == 0x000D)
                 LoadString(hInst,IDS_ERRVIE8,szStr2,sizeof(szStr2));
             else
                 LoadString(hInst,IDS_ERRVIE7,szStr2,sizeof(szStr2));

             if (HIWORD(lParam) == HEXN_ERROR6) {
                 SendDlgItemMessage(hDlg,ID_STATUS1,
                                    ST_SETTEXTFIELD,
                                    9,
                                    (LONG)(LPSTR)szStr2);
                 /* wsprintf(str, pszUeb, sel, DESCRIPTORBASE(desc), GetSelectorLimit(sel), desc.attr, (LONG)(LPSTR)szStr2 );
                  * SetDlgItemText(hDlg,ID_STATUS1,str);
                  */
             }
             else
                 CreateMessage(hDlg,szStr2,0,MB_OK);
             break;
         case HEXN_ERROR7:
             i = IDS_ERRVIEA;
             break;
         case HEXN_RBUTTONDOWN:
             BOOL fRes = FALSE;
             sel = GetWindowWord(hDlg,DWL_USER);
             globalentry.dwSize = sizeof(GLOBALENTRY);
             if (GlobalEntryHandle(&globalentry,(HGLOBAL)sel))
                 if (globalentry.wType == GT_RESOURCE)
                     if (globalentry.wData == GD_STRING ||
                         globalentry.wData == GD_DIALOG ||
                         globalentry.wData == GD_MENU)
                         fRes = TRUE;
             if (fRes)
             {
                 BtnTab[8] = ID_SUBDLG11 | 0x8000;
                 BtnTab[9] = IDS_RESOURCE;
             }
             else
             {
                 BtnTab[8] = -1;
                 BtnTab[9] = -1;
             }
             TrackPopup(hDlg,BtnTab);
             break;
         }
         if (i)
         {
             SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,2,(LONG)(LPVOID)tabpos);
             str[0] = '\t';
             i = LoadString(hInst,i,str+1,sizeof(str)-2) + 1;
             str[i] = '\t';
             str[i+1] = 0;
             SetDlgItemText(hDlg,ID_STATUS1,str);
             MessageBeep(0);
             EnableDlgItem(hDlg,ID_SUBDLG1,0);
             //                  EnableDlgItem(hDlg,ID_SUBDLG3,0);
             EnableDlgItem(hDlg,ID_SUBDLG6,0);
         }
         TRACEOUT("ViewDlg: Errormessage from Hexdump");
         break;
     }
     rc = TRUE;
     break;
 }
 return rc;
}
