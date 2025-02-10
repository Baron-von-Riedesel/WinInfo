/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ dialogfeld "Datei laden"                                          บ
บ RC:       File Handle                                             บ
บ ID_FILENAME : edit control fuer filename                          บ
บ ID_LISTDIR  : listbox control fuer direktory listing              บ
บ ID_LISTFIL  : listbox control fuer files listing                  บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/

#include "windows.h"
#include "user.h"
#include "userw.h"
#include "string.h"
#include "direct.h"
#include "dos.h"
#include "winutil1.h"
#include "opendlg.h"
// #include "chkdebug.h"

extern HINSTANCE hInst;
static HICON hIcon = 0;
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ  IsDirectory                                                             *บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL IsDirectory(LPSTR lpstr)
{
 WORD len;
 LPSTR lpstr2;
 BOOL rc = FALSE;

 if (*lpstr != '[')
    return rc;
 len = _fstrlen(lpstr);
 lpstr2 = lpstr+len-1;
 if (*lpstr2 != ']')
    return rc;
 rc = TRUE;
 if ((len == 5) && (*(lpstr+1) == '-') && (*(lpstr2-1) == '-'))
    {
     *lpstr = *(lpstr+2);
     *(lpstr+1) = ':';
     *(lpstr+2) = 0;
     return rc;
    }
 *lpstr2 = '\\';
 _fstrcpy(lpstr,lpstr+1);
 return rc;
}
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ  scstr() - check ob wildcard in filename                                 *บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL scstr(const LPSTR lpstr)
{
  LPSTR p;

  for (p = lpstr + _fstrlen(lpstr) - 1;p >= lpstr;p--)
      if ((*p == '\\') || (*p == ':'))
         return FALSE;
      else
      if ((*p == '*') || (*p == '?'))
         return TRUE;

  return FALSE;
}
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ  newpath() - Neues Verzeichnis                                          บ
บ  a) pruefen ob wildcard in filenamen                                    บ
บ  b) wenn ja, den neuen filenamen setzen                                 บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL newpath(const LPSTR dirname,LPSTR filename,LPSTR lpext,int extsize)
{
 char  temp[MAXFN];
 LPSTR lpstr;

   _fstrcpy(temp,dirname);
   if (scstr(filename))
      {
      for (lpstr = filename + _fstrlen(filename) - 1;lpstr >= filename;lpstr--)
          if ((*lpstr == '\\') || (*lpstr == ':'))
              break;
      lpstr++;
      _fstrcat(temp,lpstr);
      if (_fstrlen(lpstr) > extsize)
          return FALSE;
      else
          _fstrcpy(lpext,lpstr);
      }
   else                             /* ansonsten die letzte extension nehmen */
      _fstrcat(temp,lpext);

   _fstrcpy(filename,temp);
   return(TRUE);
}
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ  changepath() - in anderes Verzeichnis/Drive wechseln                   บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL changepath(LPSTR filename,LPSTR dirname)
{
 LPSTR lpstr;
 int i;
 unsigned drives;

   _fstrcpy(dirname,filename);
   i = _fstrlen(dirname);
   for (lpstr = dirname + i - 1;i;i--,lpstr--)
       if (*lpstr == '\\')
          {
           _fstrcpy(filename,lpstr+1);
           *lpstr = 0;
           DosChangeDir(dirname);
           break;
          }
       else
       if (*lpstr == ':')
          {
           _fstrcpy(filename,lpstr+1);
           *lpstr = 0;
           drives = SetDrive((*dirname | 0x20) - 'a' + 1);
           break;
          }
   return(TRUE);
}
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ  OpenFileDlg                                                            บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int __export CALLBACK OpenFileDlg(HWND hDlg,unsigned message,WPARAM wParam,LPARAM lParam)
{
   BOOL  result;
   DWORD style;
   HANDLE tHandle;
   HCURSOR xCursor;
   char filename[MAXFN];
   char dirname[MAXFN];
   OFSTRUCT ofstruct;
   char str[MAXFN+20];
   char str1[80];
   OPENFILEENTRY FAR * lpopenfileentry;
   unsigned x;
   unsigned drive;
   BOOL rc = FALSE;

   lpopenfileentry = GetWindowLong(hDlg,DWL_USER);

   switch(message)
     {
      case WM_INITDIALOG:
         lpopenfileentry = (OPENFILEENTRY FAR *)lParam;
         SetWindowLong(hDlg,DWL_USER,(LONG)(LPSTR)lpopenfileentry);
         if (lpopenfileentry->capstr)
             SetWindowText(hDlg,lpopenfileentry->capstr);
         if (lpopenfileentry->fncap)
             SetDlgItemText(hDlg,ID_FNCAP,lpopenfileentry->fncap);
         SendDlgItemMessage(hDlg,ID_FILENAME,EM_LIMITTEXT,MAXFN,0L);
         SetDlgItemText(hDlg,ID_FILENAME,lpopenfileentry->extstr);
         drive = GetDrive();
         x = AddFileList(hDlg,lpopenfileentry->extstr,ID_LISTFIL,0x0001); /* normale files + r/o files */
         if (x)
            x = 0xC010;                                       /* drives + subdirs */
         else
           {
            drive = SetDrive(drive);
            x = 0xC000;                                       /* nur drives */
           }
         AddFileList(hDlg,"*.*",ID_LISTDIR,x);

         DosGetCurrentDir(str);
         SetDlgItemText(hDlg,ID_PATH,str);

         if (!lpopenfileentry->flags.nmodal)
             EnableWindow(GetParent(hDlg),0);
         else
         if (lpopenfileentry->startctl)             // nur 1 mal starten?
             EnableWindow(lpopenfileentry->startctl,0);
         if (!lpopenfileentry->flags.minimizebox)
             SetWindowLong(hDlg,GWL_STYLE,GetWindowLong(hDlg,GWL_STYLE) & (~WS_MINIMIZEBOX));

         ShowWindow(hDlg,SW_NORMAL);
         rc = TRUE;
         break;
      case WM_CLOSE:
         if (!lpopenfileentry->flags.nmodal)
             EnableWindow(GetParent(hDlg),1);
         else
         if (lpopenfileentry->startctl)         // nur 1 mal starten?
             EnableWindow(lpopenfileentry->startctl,1);
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
      case WM_QUERYDRAGICON:
         if (lpopenfileentry && lpopenfileentry->hIcon)
            rc = lpopenfileentry->hIcon;
         break;
      case WM_ERASEBKGND:
         if (IsIconic(hDlg))
            {
             DefWindowProc(hDlg,WM_ICONERASEBKGND,wParam,lParam);
             rc = 1;
            }
         break;
      case WM_PAINT:
         if (IsIconic(hDlg))
            {
             if (lpopenfileentry && lpopenfileentry->hIcon)
                {
                 PAINTSTRUCT ps;
                 HWND hWnd;
                 RECT rect;

                 BeginPaint(hDlg,(LPPAINTSTRUCT)&ps);
                 DrawIcon(ps.hdc,0,0,lpopenfileentry->hIcon);
                 EndPaint( hDlg,(LPPAINTSTRUCT)&ps);
                 rc = TRUE;
                }
            }
         break;
      case WM_COMMAND:
         switch(wParam)
           {
            case IDOK:
               GetDlgItemText(hDlg,ID_FILENAME,filename,sizeof(filename));
               if (scstr(filename))                          /* falls wildcard in filename */
                  {
                   if (newpath("",filename,lpopenfileentry->extstr,lpopenfileentry->extsize))
                       PostMessage(hDlg,WM_COMMAND,ID_LISTDIR,MAKELONG(0,LBN_DBLCLK));
                   else
                       MessageBeep(0);
                  }
               else
                   if (lpopenfileentry->flags.openexit)
                      {                                /* file check ganz dem client ueberlassen */
                       _fstrncpy(lpopenfileentry->fnstr,filename,lpopenfileentry->fnsize);
                       PostMessage(GetParent(hDlg),WM_COMMAND,
                                   lpopenfileentry->id,MAKELONG(0,hDlg));
                      }
                   else                                /* ueberpruefen ob file da */
                   if ((tHandle = OpenFile(filename,&ofstruct,OF_EXIST | OF_SHARE_DENY_NONE)) != -1)
                      {
                       _fstrncpy(lpopenfileentry->fnstr,filename,lpopenfileentry->fnsize);
                       if (SendMessage(GetParent(hDlg),WM_COMMAND,
                                       lpopenfileentry->id,MAKELONG(tHandle,hDlg)))
                           PostMessage(hDlg,WM_CLOSE,0,0);
                      }
                   else
                      {
                       if (LoadString(hInst,IDS_ERRNR + ofstruct.nErrCode,str1,sizeof(str1)))
                           wsprintf(str,str1,(LPSTR)filename);
                       else
                          {
                           LoadString(hInst,IDS_ERRNR,str1,sizeof(str1));
                           wsprintf(str, str1, ofstruct.nErrCode, (LPSTR)filename);
                          }
                       CreateMessage(hDlg,str,0,MB_OK);
                      }
               break;
            case IDCANCEL:
               PostMessage(hDlg,WM_CLOSE,0,0);
               break;
            case ID_LISTDIR:                 /* directory-liste */
               switch(HIWORD(lParam))
                 {
                  case LBN_SELCHANGE:
//                     if (DlgDirSelect(hDlg,dirname,ID_LISTDIR))
                     x = SendDlgItemMessage(hDlg,ID_LISTDIR,LB_GETCURSEL,0,0);
                     if (x == LB_ERR)
                         break;
                     SendDlgItemMessage(hDlg,ID_LISTDIR,LB_GETTEXT,x,(LONG)(LPSTR)dirname);
                     if (IsDirectory(dirname))
                        {
                         GetDlgItemText(hDlg,ID_FILENAME,filename,sizeof(filename));
                         if (newpath(dirname,filename,lpopenfileentry->extstr,lpopenfileentry->extsize))
                             SetDlgItemText(hDlg,ID_FILENAME,filename);
                         else
                             MessageBeep(0);
                        }
                     break;
                  case LBN_DBLCLK:           /* verzeichnis oder drive wechselt */
                     xCursor = SetCursor(LoadCursor(0,IDC_WAIT));
                     GetDlgItemText(hDlg,ID_FILENAME,filename,sizeof(filename));
                     drive = GetDrive();
                     changepath(filename,dirname);
                     SetDlgItemText(hDlg,ID_FILENAME,filename);
                     SendDlgItemMessage(hDlg,ID_LISTFIL,LB_RESETCONTENT,0,0);
                     SendDlgItemMessage(hDlg,ID_LISTDIR,LB_RESETCONTENT,0,0);
                     x = AddFileList(hDlg,filename,ID_LISTFIL,0x0001);
                     if (x)
                        x = 0xC010;
                     else
                       {
                        drive = SetDrive(drive);
                        x = 0xC000;
                       }
                     AddFileList(hDlg,"*.*",ID_LISTDIR,x);
                     str[0] = 0;
                     DosGetCurrentDir(str);
                     SetDlgItemText(hDlg,ID_PATH,str);
                     SetFocus(GetDlgItem(hDlg,ID_FILENAME));
                     SetCursor(xCursor);
                     break;
                  default:
                     break;
                 }
               break;
            case ID_LISTFIL:                 /* file liste */
               switch(HIWORD(lParam))
                 {
                  case LBN_SELCHANGE:
                     x = SendDlgItemMessage(hDlg,ID_LISTFIL,LB_GETCURSEL,0,0);
                     if (x == LB_ERR)
                         break;
                     SendDlgItemMessage(hDlg,ID_LISTFIL,LB_GETTEXT,x,(LONG)(LPSTR)filename);
                     SetDlgItemText(hDlg,ID_FILENAME,filename);
                     break;
                  case LBN_DBLCLK:
                     PostMessage(hDlg,WM_COMMAND,IDOK,0);
                     break;
                  default:
                     break;
                 }
               break;
            case ID_FILENAME:
               if(HIWORD(lParam) == EN_CHANGE)
                  EnableWindow(GetDlgItem(hDlg, IDOK),
                     (BOOL) SendDlgItemMessage(hDlg, ID_FILENAME, WM_GETTEXTLENGTH, 0, 0L));
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
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ  CreateOpenFileDialog                                                   บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL __export WINAPI CreateOpenFileDialog(HWND hDlg,OPENFILEENTRY FAR * lpopenfileentry)
{
  HANDLE hresinfo;
  HANDLE hresdata;
  LPVOID lpvoid;
  HANDLE hInstance;
  WORD   dlgtmp;
  BOOL   rc;


  if (lpopenfileentry->dlgtmp)
     {
      dlgtmp    = lpopenfileentry->dlgtmp;
      hInstance = lpopenfileentry->hInst;
     }
  else
     {
      dlgtmp    = IDD_OPENFILE;
      hInstance = hInst;
     }

  if (!hIcon)
       hIcon = LoadIcon(hInst,MAKEINTRESOURCE(IDI_OPENICON));

  if (lpopenfileentry->flags.openicon)
      lpopenfileentry->hIcon = hIcon;

#if 1
  hresinfo = FindResource(hInstance, MAKEINTRESOURCE(dlgtmp), MAKEINTRESOURCE(RT_DIALOG));
  hresdata = LoadResource(hInstance,hresinfo);
  lpvoid   = LockResource(hresdata);

  rc = CreateDialogIndirectParam(lpopenfileentry->hInst,
                                 lpvoid,
                                 hDlg,
                                 OpenFileDlg,
                                 (LONG)(LPSTR)lpopenfileentry);
  UnlockResource(hresdata);
  FreeResource(hresdata);
#else
  rc = CreateDialogParam(hInstance,MAKEINTRESOURCE(dlgtmp),hDlg,OpenFileDlg,
                         (LPARAM)(LPSTR)lpopenfileentry);
#endif
  return rc;
}
