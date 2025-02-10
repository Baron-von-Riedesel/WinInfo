
#define _TRACE_

#define UA_USETAB 0x20

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "dos.h"
#include "windows.h"
#include "windowsx.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"


// #define strcmp lstrcmp
// #define strlen lstrlen
// #define strcat lstrcat
// #define sscanf lsscanf

/* extern DWORD (FAR PASCAL *MyUnassemble)(FWORD,LPSTR,WORD);
*/
extern "C" {
LPSTR FAR PASCAL _convstrhex(LPSTR,LPSTR,WORD);
};

extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern int  unassLines;
extern int  fShowBytes;
extern int  _lastException;
extern BYTE fShow;
extern WORD wRegistered;
extern HFONT hFontAlt;

typedef struct tagUAENTRY {
   DWORD offset;
   WORD  selector;
   struct {
      unsigned fShowBytes;
     } flags;
  } UAENTRY;

int  xUnassPos = 100;
int  yUnassPos = 100;

static char szFStrWd[] = {" %u %*c"};

BOOL CALLBACK UnassembleDlg  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Unassemble32Dlg(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ViewDlg        (HWND, UINT, WPARAM, LPARAM);

CATCHBUF cb;

int WINAPI interruptcallback(void)
{
    _asm {
        pusha
        push es
        call GetTaskDS
        mov dx,ax
        mov ax,[bp+6+2] // interrupt nummer
        cmp al,12               // stack fault?
        jz  exc12
        cmp al,13               // gp fault?
        jz  exc13
        cmp al,14
        jnz myexit
    exc12:
    exc13:
        push dx
        push offset cb
        push ax
        call Throw
    myexit:
        pop es
        popa
    }
    return 0;
}
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Dialogfunktion "Parametereingabe"                                 บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int EXPORTED WINAPI UnassParmsDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
 char szStr[20];
 char str[80];
 char str1[80];
 static char szFindString[20] = {0};
 WORD wLines;
 int i;
 HWND hWnd;
 FWORD faddr;
 UAENTRY * pentry;
 BOOL rc = FALSE;

   switch(message)
     {
      case WM_INITDIALOG:
         EnableDlgItem(GetParent(hDlg),ID_SUBDLG1,0);
         SendDlgItemMessage(hDlg,ID_EDITUNASSLINES,EM_LIMITTEXT,5,0);
         wsprintf(szStr,"%u",unassLines);
         SetDlgItemText(hDlg,ID_EDITUNASSLINES,szStr);
         SendDlgItemMessage(hDlg,ID_EDITUNASSADDR,EM_LIMITTEXT,14,0);
         pentry = (UAENTRY *)lParam;
         wsprintf(szStr,"%X:%lX",pentry->selector,pentry->offset);
         SetDlgItemText(hDlg,ID_EDITUNASSADDR,szStr);
         SendDlgItemMessage(hDlg,ID_EDITUNASSBTN1,BM_SETCHECK,pentry->flags.fShowBytes,0);

         SendDlgItemMessage(hDlg,ID_EDITUNASSSTR,EM_LIMITTEXT,sizeof(szFindString)-1,0);
         SetDlgItemText(hDlg,ID_EDITUNASSSTR,szFindString);
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
            case ID_EDITUNASSSTR:
               if (HIWORD(lParam) == EN_UPDATE)
                  {
                   GetDlgItemText(hDlg,ID_EDITUNASSSTR,szStr,2);
                   EnableDlgItem(hDlg,ID_EDITUNASSFIND,*szStr);
                   EnableDlgItem(hDlg,ID_EDITUNASSREFIND,0);
                  }
               break;
            case ID_EDITUNASSREFIND:
            case ID_EDITUNASSFIND:
               GetDlgItemText(hDlg,ID_EDITUNASSSTR,szFindString,sizeof(szFindString));
               if (szFindString[0]) {
                   hWnd = GetParent(hDlg);
                   i = (int)SendDlgItemMessage(hWnd,ID_LISTBOX1,LB_GETCURSEL,0,0);
                   if ((wParam == ID_EDITUNASSFIND) && (i != 0xFFFF))
                           i--;
                   SetCursor(hCursorWait);
                   i = (int)SendDlgItemMessage(hWnd,ID_LISTBOX1,LB_FINDSTRING,i,(LONG)(LPSTR)szFindString);
                   SetCursor(hCursor);
                   if (i != 0xFFFF) {
                       SendDlgItemMessage(hWnd,ID_LISTBOX1,LB_SETCURSEL,i,0);
                       EnableDlgItem(hDlg,ID_EDITUNASSREFIND,1);
                   } else {
                       LoadString(hInst,IDS_ERRVIEB,str1,sizeof(str1));
                       wsprintf(str,str1,(LPSTR)szFindString);
                       CreateMessage(hDlg,str,0,MB_OK);
                   }
               } else
                   MessageBeep(0);
               break;
            case IDOK:
               GetDlgItemText(hDlg,ID_EDITUNASSSTR,szFindString,sizeof(szFindString));
               GetDlgItemText(hDlg,ID_EDITUNASSLINES,szStr,sizeof(szStr));
               if (sscanf(szStr,szFStrWd,&wLines) != 1) {
                   MessageBeep(0);
                   break;
               }
               if (wLines > 0x7FFF) {
                   CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRUNA1),0,MB_OK);
                   break;
               }
               GetDlgItemText(hDlg,ID_EDITUNASSADDR,szStr,sizeof(szStr));
               faddr.selector = 0;
               faddr.offset   = 0;
               sscanf(szStr," %X:%lX",&faddr.selector,&faddr.offset);

               if (wLines)
                   unassLines = wLines;
               else {
                   MessageBeep(0);
                   break;
               }

               fShowBytes = (int)SendDlgItemMessage(hDlg,ID_EDITUNASSBTN1,BM_GETCHECK,0,0);

               SendMessage(GetParent(hDlg),
                           WM_COMMAND,
                           ID_REFRESH,
                           (LONG)(LPSTR)&faddr
                          );
               if (fShow)
//                   PostMessage(hDlg,WM_CLOSE,0,0);
                   ;
               else
                   MessageBeep(0);

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
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ CheckCall  Dialog                                    บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
static BOOL PASCAL CheckCall(HWND hDlg,WORD wMode)
{
 char  str[80];
 char  str2[80];
 char c;
 int   y,i,j,k;
 FARPROC paddr;
 FWORD faddr;
 UAENTRY * pentry;
 IDTENTRY idtentry;
 BOOL fUseFaddr = FALSE;
 LPBYTE lpproc;

  y =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
  if (y == LB_ERR)
      return FALSE;

  pentry = (UAENTRY *)GetWindowWord(hDlg,DWL_USER);
  fShowBytes = pentry->flags.fShowBytes;
  paddr   = 0;
  str2[0] = 0;
  c       = 0;
  i       = 0;
  SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETTEXT,y,(LPARAM)(LPSTR)str);
  if (pentry->flags.fShowBytes)
      k = sscanf(str,"%04X:%04X %*s %*s %s",&i,&j,str2);
  else
      k = sscanf(str,"%04X:%04X %*s %s",&i,&j,str2);

  if (k != 3)
      return FALSE;

  lpproc = (LPBYTE)MAKELONG(j,i);
  if (!IsValidAddress(lpproc))
     return FALSE;

  if ((*lpproc == 0x9A) || (*lpproc == 0xEA))
      paddr = (FARPROC)MAKELONG(*(LPWORD)(lpproc+1),*(LPWORD)(lpproc+3));
  else
  if ((*lpproc == 0x66) && ((*(lpproc+1) == 0x9A) || (*(lpproc+1) == 0xEA)))
     {
      faddr.offset   = *(LPDWORD)(lpproc+2);
      faddr.selector = *(LPWORD)(lpproc+6);
      fUseFaddr = TRUE;
     }
  else
  if ((*lpproc == 0xCD))
     {
      idtentry.id = (WORD)*(lpproc+1) - 1;
      idtentry.wLimit = 0;  // aktuelle IDT lesen
      if (!IDTNext(&idtentry))
          return FALSE;
      if (idtentry.attr & 0x0800)
         {
          faddr.selector = idtentry.selector;
          faddr.offset   = idtentry.offset;
          fUseFaddr = TRUE;
         }
      else
          paddr = (FARPROC)MAKELONG(idtentry.offset,idtentry.selector);
     }
  else
     {
      y = sscanf(str2,"%04X%c%04X ",&i,&c,&j);
      if (y > 1)
          paddr = (FARPROC)MAKELONG(j,i);
      else
      if (y && (!c))
         {
          sscanf(str,"%04X:",&j);
          paddr = (FARPROC)MAKELONG(i,j);
         }
     }

  if (fUseFaddr)
     {
      if (!wMode)
         CreateDialogParam(hInst,
                           MAKEINTRESOURCE(IDD_UNASSEM32),
                           hDlg,
                           Unassemble32Dlg,
                           (DWORD)(LPSTR)&faddr
                          );
      return TRUE;
     }
  else
  if (paddr)
     {
      if (!IsReadableSelector(HIWORD(paddr)))
         return FALSE;
      else
        {
         if (!wMode)
             CreateDialogParam(hInst,
                               MAKEINTRESOURCE(IDD_UNASSEM),
                               hDlg,
                               UnassembleDlg,
                               (DWORD)paddr
                              );
         return TRUE;
        }
     }
  else
      return FALSE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ CheckCall32                                          บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
static BOOL PASCAL CheckCall32(HWND hDlg,WORD wMode)
{
  char  str[80];
  char  str1[80];
  char  str2[80];
  int   y,i,k;
  char c;
  DWORD j;
  UAENTRY * pentry;
  IDTENTRY idtentry;
  FWORD entry,faddr;

  y =  (WORD)SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETCURSEL,0,0);
  if (y == LB_ERR)
      return FALSE;
  pentry = (UAENTRY *)GetWindowWord(hDlg,DWL_USER);
  fShowBytes = pentry->flags.fShowBytes;
  str2[0] = 0;
  SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_GETTEXT,y,(LPARAM)(LPSTR)str);
  entry.selector = 0;
  if (pentry->flags.fShowBytes)
      k = sscanf(str,"%X:%lX %*s %s %s",&entry.selector,&entry.offset,str1,str2);
  else
      k = sscanf(str,"%X:%lX %s %s",&entry.selector,&entry.offset,str1,str2);
  if (k != 4)
      return FALSE;
       	
  y = sscanf(str2,"%X:%lX ",&i,&j);
  if (!strcmp(str1,"int"))
     {
      if (wMode)
          return TRUE;
      idtentry.id = i - 1;
	  idtentry.wLimit = 0;	// aktuelle IDT lesen
      if (!IDTNext(&idtentry))
          return FALSE;
      faddr.selector = idtentry.selector;
      faddr.offset   = idtentry.offset;
      CreateDialogParam(hInst,
                        MAKEINTRESOURCE(IDD_UNASSEM32),
                        hDlg,
                        Unassemble32Dlg,
                        (DWORD)(LPSTR)&faddr
                       );
      return TRUE;
     }
  else
  if (y > 2)
     {
      entry.selector = i;
      entry.offset   = j;
     }
  else
     {
      if ((y = sscanf(str2,"%lX%c",&j,&c)) == 1)
          entry.offset = j;
      else
         {
          entry.selector = 0;
         }
     }

  if (entry.selector)
     {
      if (!wMode)
           CreateDialogParam(hInst,
                             MAKEINTRESOURCE(IDD_UNASSEM32),
                             hDlg,
                             Unassemble32Dlg,
                             (DWORD)(LPSTR)&entry
                            );
      return TRUE;
     }
  else
      return FALSE;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Unassemble Dialog                                    บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK UnassembleDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 char  str[128];
 char  str1[80];
 char  str2[80];
 int  y;
 UINT x,i,j,wAttr;
 int tabpos[5];
 HWND hWnd;
 LPSTR lpstr;
 BYTE volatile fOk;
 LPBYTE lpproc;
 FWORD faddr;
 LPFWORD lpentry;
 HGLOBAL hOwner;
 BOOL rc = FALSE;
 GLOBALENTRY globalentry;
 UAENTRY * pentry;
 HEXDUMPPARM hdp;
 HANDLE hHandler;
// MODULEENTRY moduleentry;

 switch (message)
    {
    case WM_INITDIALOG:
      if (!(pentry = (UAENTRY *)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,sizeof(UAENTRY)))) {
          PostMessage(hDlg,WM_CLOSE,0,0);
          break;
         }
      SetWindowWord(hDlg,DWL_USER,(WORD)pentry);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,0, (LPARAM)(LPVOID)hFontAlt);
      pentry->offset   = LOWORD(lParam);
      pentry->selector = HIWORD(lParam);
      SendMessage(hDlg,WM_COMMAND,ID_REFRESH,(LONG)(LPSTR)pentry);
//      SetWindowPos(hDlg,0,xUnassPos,yUnassPos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_SHOWNORMAL);
      LoadTabs(IDUS_43,str);
      SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LONG)(LPINT)(str+2));
      EnableDlgItem(hDlg,ID_SNAPSHOT,wRegistered == 1);
      rc = TRUE;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_DESTROY:
      HLOCAL hMem;
      if (hMem = (HLOCAL)GetWindowWord(hDlg,DWL_USER))
          LocalFree(hMem);
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xUnassPos,&yUnassPos);
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case ID_SNAPSHOT:
              WriteLBSnapShot(0,
                              hDlg,
                              ID_LISTBOX1,
                              "UAS",
                              WLBSS_CLOSE | WLBSS_CLIPBOARD | WLBSS_MESSAGE);
              break;
         case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_SUBDLG1:
              pentry = (UAENTRY *)GetWindowWord(hDlg,DWL_USER);
              CreateDialogParam(hInst,
                                MAKEINTRESOURCE(IDD_UNASSPARMS),
                                hDlg,
                                UnassParmsDlg,
                                (LPARAM)(LPVOID)pentry
                               );
              break;
         case ID_SUBDLG3:
              pentry = (UAENTRY *)GetWindowWord(hDlg,DWL_USER);
              hdp.dwLength = 0;
              hdp.dwOffset = 0;
              hdp.hGlobal  = (HGLOBAL)pentry->selector;
              hdp.wType    = HDP_NORMAL | HDP_SETWINDOWTEXT;
              CreateDialogParam(hInst,
                                MAKEINTRESOURCE(IDD_VIEW),
                                hDlg,
                                ViewDlg,
                                (LPARAM)(LPVOID)&hdp
                               );
              break;
         case IDOK:
         case ID_SUBDLG2:
              if (!CheckCall(hDlg,0))
                  MessageBeep(0);
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam)) {
              case LBN_SELCHANGE:
                  EnableDlgItem(hDlg,ID_SUBDLG2,CheckCall(hDlg,1));
                  x = (WORD)SendMessage((HWND)LOWORD(lParam),LB_GETCURSEL,0,0) + 1;
                  wsprintf(str,"Zeile\t%u",x);
                  SetDlgItemText(hDlg,ID_STATUS1,str);
                  break;
              case LBN_DBLCLK:
                  PostMessage(hDlg,WM_COMMAND,ID_SUBDLG2,0);
                  break;
              }
              break;
         case ID_REFRESH:
              fShow = FALSE;
              pentry = (UAENTRY *)GetWindowWord(hDlg,DWL_USER);
              lpentry = (LPFWORD)lParam;
              GetAttrofSelector(lpentry->selector,&wAttr);
//              if (HIWORD(lpentry->offset)) {
              if (wAttr & 0x4000) {
                  CreateDialogParam(hInst,
                                    MAKEINTRESOURCE(IDD_UNASSEM32),
                                    GetParent(hDlg),
                                    Unassemble32Dlg,
                                    (DWORD)(LPSTR)lpentry
                                   );
                  PostMessage(hDlg,WM_CLOSE,0,0);
                  break;
              }
              if (!IsValidAddress((LPSTR)MAKELONG(LOWORD(lpentry->offset),lpentry->selector))) {
                  MessageBox(0,"Invalid Address specified",0,MB_OK);
                  break;
              }
              SetCursor(hCursorWait);
              pentry->offset   = lpentry->offset;
              pentry->selector = lpentry->selector;
              pentry->flags.fShowBytes = fShowBytes;
              lpproc = (LPBYTE)MAKELONG(pentry->offset,pentry->selector);
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);
              if (pentry->flags.fShowBytes) {
                  tabpos[0] = 44;
                  tabpos[1] = 102;
                  tabpos[2] = 126;
                  tabpos[3] = 146;
              } else {
                  tabpos[0] = 52;
                  tabpos[1] = 82;
                  tabpos[2] = 106;
                  tabpos[3] = 126;
              }
              ListBox_SetTabStops(hWnd,4,tabpos);

//              GetCodeHandle((FARPROC)lpproc);  // segment in speicher laden
              IsValidAddress(lpproc);
              globalentry.dwSize = sizeof(GLOBALENTRY);
              if (GlobalEntryHandle(&globalentry,(HGLOBAL)HIWORD(lpproc))) {
                  hOwner = globalentry.hOwner;
                  if (!IsModule(hOwner))
                      hOwner = GetModuleFromTask(hOwner);

                  if (x = GetEntryID((HMODULE)hOwner,HIWORD(lpproc),LOWORD(lpproc))) {
                     if (GetProcedureName((HMODULE)hOwner,x,str2,sizeof(str2)))
                        ;
                     else
                         wsprintf(str2,"@%d",x);
                  } else
                     wsprintf(str2,"%X.%X",
                              GetSegmentID((HMODULE)hOwner,HIWORD(lpproc)),
                              LOWORD(lpproc)
                             );
//                  moduleentry.dwSize = sizeof(MODULEENTRY);
//                  ModuleFindHandle(&moduleentry,globalentry.hOwner);
//                  wsprintf(str,"%s.%s",(LPSTR)moduleentry.name,(LPSTR)str2);
                  GetModuleName((HMODULE)hOwner,str1,sizeof(str1));
                  LoadString(hInst,IDS_DISASSSTR,str,sizeof(str));
                  wsprintf(str+strlen(str),": %s.%s",(LPSTR)str1,(LPSTR)str2);
                  SetWindowText(hDlg,str);
//                  strcatc(str,':');
//                  SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                 }
              else
                 {
                  hOwner = 0;
                  wsprintf(str,"%X.%X",HIWORD(lpproc),LOWORD(lpproc));
                  LoadString(hInst,IDS_DISASSSTR,str1,sizeof(str1));
                  wsprintf(str2,"%s: %s",(LPSTR)str1,(LPSTR)str);
                  SetWindowText(hDlg,str2);
                 }
              faddr.offset   = LOWORD(lpproc);
              faddr.selector = HIWORD(lpproc);
              x = GetNextEntryAddress((HMODULE)hOwner,faddr.selector,(WORD)faddr.offset);
              GetAttrofSelector(faddr.selector,&wAttr);
              if (wAttr & 0x4000)
                  wAttr = 3;
              else
                  wAttr = 0;
//              wsprintf(str,"Attr: %X",wAttr);
//              TRACEOUT(str);
              wAttr = wAttr | UA_USETAB;
              fOk = TRUE;
              unassLines = unassLines & 0x7FFF;
			  
			  InterruptRegister(0,(FARPROC)interruptcallback);
			  if (!Catch(cb)) {
                  for (y = 0 ; y < unassLines ; y++) {
                      if (x <= faddr.offset) {
                          if (i = GetEntryID((HMODULE)hOwner,faddr.selector,x)) {
                              if (!GetProcedureName((HMODULE)hOwner,i,str,sizeof(str)-1))
                                  wsprintf(str,"@%d",i);
                              i = strlen(str);
                              str[i] = ':';
                              str[i+1] = 0;
                              SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
                          }
                          faddr.offset = x;
                          x = GetNextEntryAddress((HMODULE)hOwner,faddr.selector,x+1);
                      }
                      wsprintf(str,"%04X:%04X\t",pentry->selector,LOWORD(faddr.offset));
                      j = LOWORD(faddr.offset);
                      if (pentry->flags.fShowBytes) {
                          faddr.offset = Unassemble(faddr,str1,wAttr);
                          if (!(i = min(LOWORD(faddr.offset) - j,8)))
                              break;
                          lpstr = _convstrhex(str+10,(LPSTR)MAKELONG(j,faddr.selector),i);
                          *lpstr = 9;
                          *(lpstr+1) = 0;
                          strcat(str,str1);
                      } else {
                          faddr.offset = Unassemble(faddr,str+10,wAttr);
                          if (j == LOWORD(faddr.offset))
                             break;
                      }

                      if (SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str) == LB_ERRSPACE) {
                          CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRIMP4),0,MB_OK);
                          break;
                      }
                  }
              } else {
				  fOk = FALSE;
                  wsprintf(str,"%04X:%04X\t???",HIWORD(lpproc),LOWORD(faddr.offset));
                  SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)str);
              }

              InterruptUnRegister(0);
              
              EnableDlgItem(hDlg,ID_SUBDLG2,0);
              SendMessage(hWnd,LB_SETCURSEL,0,0);
              SetCursor(hCursor);
              fShow = TRUE;
              break;
         }
      rc = TRUE;
      break;
    default:
      break;
    }
 return rc;
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Unassemble Dialog 32 Bit                             บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK Unassemble32Dlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
    char  str[128];
    char  str1[80];
    char  str2[80];
    DWORD dwBase;
    DWORD dwLimit;
    UINT wAttr;
    LPSTR lpstr;
    WORD qsel;
    int   y,x;
    int tabpos[5];
    HWND hWnd;
    BYTE volatile fOk;
    LPBYTE lpproc;
    BOOL rc = FALSE;
    LPFWORD lpentry;
    UAENTRY * pentry;
    FWORD entry,xaddr,yaddr;


    switch (message)
    {
    case WM_INITDIALOG:
        if (!(pentry = (UAENTRY *)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,sizeof(FWORD)))) {
            PostMessage(hDlg,WM_CLOSE,0,0);
            break;
        }
        SetWindowWord(hDlg,DWL_USER,(WORD)pentry);
        SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,0, (LPARAM)(LPVOID)hFontAlt);
        SendMessage(hDlg,WM_COMMAND,ID_REFRESH,lParam);
        if (fShow) {
            //          SetWindowPos(hDlg,0,xUnassPos,yUnassPos,0,0,SWP_NOSIZE | SWP_NOZORDER);
            ShowWindow(hDlg,SW_SHOWNORMAL);
            LoadTabs(IDUS_43,str);
            SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)str,(LPARAM)(LPINT)(str+2));
        } else
            PostMessage(hDlg,WM_CLOSE,0,0);
        rc = TRUE;
        break;
    case WM_CLOSE:
        DestroyWindow(hDlg);
        rc = TRUE;
        break;
    case WM_DESTROY:
        HLOCAL hMem;
        if (hMem = (HLOCAL)GetWindowWord(hDlg,DWL_USER))
            LocalFree(hMem);
        break;
    case WM_COMMAND:
        switch (wParam)
        {
        case ID_SNAPSHOT:
            WriteLBSnapShot(0,hDlg,ID_LISTBOX1,"UAS",3);
            break;
        case IDCANCEL:
            PostMessage(hDlg,WM_CLOSE,0,0);
            break;
        case ID_SUBDLG1:
            pentry = (UAENTRY *)GetWindowWord(hDlg,DWL_USER);
            CreateDialogParam(hInst,
                              MAKEINTRESOURCE(IDD_UNASSPARMS),
                              hDlg,
                              UnassParmsDlg,
                              (LPARAM)(LPVOID)pentry
                             );
            break;
        case IDOK:
        case ID_SUBDLG2:
            if (!CheckCall32(hDlg,0))
                MessageBeep(0);
            break;
        case ID_LISTBOX1:
            switch(HIWORD(lParam))
            {
            case LBN_SELCHANGE:
                EnableDlgItem(hDlg,ID_SUBDLG2,CheckCall32(hDlg,1));
                x = (int)ListBox_GetCurSel((HWND)LOWORD(lParam)) + 1;
                wsprintf(str,"Zeile\t%u",x);
                SetDlgItemText(hDlg,ID_STATUS1,str);
                break;
            case LBN_DBLCLK:
                PostMessage(hDlg,WM_COMMAND,ID_SUBDLG2,0);
                break;
            }
            break;
        case ID_REFRESH:
            fShow = FALSE;
            lpentry = (LPFWORD)lParam;
            /* dgroup holen */
            lpproc  = (LPBYTE)str2;
            xaddr.selector = HIWORD(lpproc);
            xaddr.offset   = LOWORD(lpproc);
            pentry = (UAENTRY *)GetWindowWord(hDlg,DWL_USER);

            if (lpentry->selector) {
                if (!IsMemoryDescriptor(lpentry->selector))
                    break;
                if (!GetBaseofSelector(lpentry->selector,&dwBase))
                    break;
            } else
                dwBase = 0;

            if (!(qsel = DPMIAllocDescriptor(1)))  /* nicht ueber windows! */
                break;

            pentry->flags.fShowBytes = fShowBytes;
            fShow = TRUE;
            SetCursor(hCursorWait);
            hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
            if (pentry->flags.fShowBytes) {
                tabpos[0] = 60;
                tabpos[1] = 132;
                tabpos[2] = 154;
                tabpos[3] = 174;
            } else {
                tabpos[0] = 64;
                tabpos[1] = 94;
                tabpos[2] = 116;
                tabpos[3] = 136;
            }
            ListBox_SetTabStops(hWnd,4,tabpos);
            if (lpentry->selector) {
                GetLimitofSelector(lpentry->selector,&dwLimit);
                GetAttrofSelector(lpentry->selector,&wAttr);
            } else {
                dwLimit = -1;
                wAttr = 0xC0FB;
            }
            wAttr = AdjustDescriptorAttr(wAttr);

            SetDescriptor(qsel,dwBase,dwLimit,wAttr);

            if (wAttr & 0x4000)
                wAttr = 3;
            else
                wAttr = 0;

            entry.offset   = lpentry->offset;
            entry.selector = qsel;
            unassLines = unassLines & 0x7FFF;
            wAttr = wAttr | UA_USETAB;		// ???

            fOk = TRUE;
            InterruptRegister(0,(FARPROC)interruptcallback);

            if ((x = Catch(cb)) == 0) {
                for (y = 0 ; y < unassLines ; y++ ) {

                    wsprintf(str,"%04X:%08lX\t",lpentry->selector,entry.offset);
                    if (pentry->flags.fShowBytes) {
                        yaddr = entry;
                        entry.offset = Unassemble(entry,str1,wAttr);
                        if (!(x = (int)__min(entry.offset - yaddr.offset,10)))
                            break;
                        _hmemcpy32(xaddr,yaddr,x);
                        lpstr = _convstrhex(str+14,str2,x);
                        *lpstr = 9;
                        *(lpstr+1) = 0;
                        strcat(str,str1);
                    } else
                        entry.offset = Unassemble(entry,str+14,wAttr);

                    if (!y)
                        ListBox_ResetContent(hWnd);
                    if (ListBox_AddString(hWnd,str) == LB_ERRSPACE) {
                        CreateMessage(hDlg,MAKEINTRESOURCE(IDS_ERRIMP4),0,MB_OK);
                        break;
                    }
                }
            } else {
                fOk = FALSE;
                if (y) {
                    wsprintf(str,"%04X:%08lX\t???",lpentry->selector,entry.offset);
                    ListBox_AddString(hWnd,str);
                } else {
                    //MessageBeep(0);
                    if (x == 0x000D)
                        LoadString(hInst,IDS_ERRVIE5,str,sizeof(str));
                    else
                        LoadString(hInst,IDS_ERRVIE4,str,sizeof(str));
                    MessageBox(GetParent(hDlg),str,0,MB_OK);
                    fShow = FALSE;
                }
            }

            if (fShow) {
                pentry->offset   = lpentry->offset;
                pentry->selector = lpentry->selector;
            }
            DPMIFreeDescriptor(qsel);
              
            InterruptUnRegister(0);
              
            LoadString(hInst,IDS_DISASSSTR,str1,sizeof(str1));
            wsprintf(str,"%s: %X:%lX",(LPSTR)str1,pentry->selector,pentry->offset);
            SetWindowText(hDlg,str);
            EnableDlgItem(hDlg,ID_SUBDLG2,0);
            ListBox_SetCurSel(hWnd,0);
            SetCursor(hCursor);
            break;
        }
        rc = TRUE;
        break;
    case WM_MOVE:
        SaveWindowPos(hDlg,&xUnassPos,&yUnassPos);
        break;
    default:
        break;
    }
    return rc;
}
