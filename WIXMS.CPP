
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
#include "dpmi.h"
#include "xms.h"

BOOL WINAPI CheckMessage(MSG *,WORD,HWND);
                     
typedef struct tagXMSHA {
	BYTE Flags;
	BYTE Locks;
	DWORD dwAddress;  // in kB
	DWORD dwLength;   // in kB
	} XMSHA;
typedef XMSHA FAR * LPXMSHA;

typedef struct tagXMSHT {
	BYTE Version;
	BYTE SizeDesc;
	WORD NumItems;
	DWORD dwHandleArray;
	} XMSHT;
typedef XMSHT FAR * LPXMSHT;
                     
extern HCURSOR hCursor;
extern HCURSOR hCursorWait;
extern HFONT hFontAlt;
extern HWND hWndXMS;

BOOL CALLBACK ViewDlg   (HWND, UINT, WPARAM, LPARAM);
BOOL WINAPI XMSDlg      (HWND, UINT, WPARAM, LPARAM);

int  xXMSpos = 50;
int  yXMSpos = 50;

////////////////////////////////////////////////////////
// XMSDlg - XMS Dialog                                 
////////////////////////////////////////////////////////
BOOL EXPORTED WINAPI XMSDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam )
{
 static int BtnTab[] = {ID_SUBDLG1,
                        0};
 char  szStr[80];
 char  szStr1[60];
 int   x,i;
 static DWORD fpXMSProc = 0;
 WORD  wHandleAlt;
 HWND  hWnd;
 RMCSTRUCT rmcstruct;
 HEXDUMPPARM hdp;
 LPXMSHA lpXMSHA;
 LPXMSHT lpXMSHT;
 BOOL rc = FALSE;

 switch (message)
    {
    case WM_INITDIALOG:
      LoadTabs(IDUS_40,szStr);
      SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,*(LPINT)szStr,(LPARAM)(LPINT)(szStr+2));
      SendDlgItemMessage(hDlg,ID_LISTBOX1,XLB_SETEXTSTYLE,XLBES_RBUTTONTRACK,(LPARAM)(LPVOID)hFontAlt);
      LoadTabs(IDUS_42,szStr);
      SendDlgItemMessage(hDlg,ID_STATUS2,ST_SETTABSTOPS,*(LPINT)szStr,(LPARAM)(LPINT)(szStr+2));
      SetWindowPos(hDlg,0,xXMSpos,yXMSpos,0,0,SWP_NOSIZE | SWP_NOZORDER);
      ShowWindow(hDlg,SW_NORMAL);
      PostMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
      rc = TRUE;
      break;
    case WM_DESTROY:
      hWndXMS = 0;
      break;
    case WM_CLOSE:
      DestroyWindow(hDlg);
      rc = TRUE;
      break;
    case WM_MOVE:
      SaveWindowPos(hDlg,&xXMSpos,&yXMSpos);
      break;
    case WM_COMMAND:
      switch (wParam)
         {
         case IDCANCEL:
              PostMessage(hDlg,WM_CLOSE,0,0);
              break;
         case ID_SUBDLG1:
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              if ((x = (WORD)SendMessage(hWnd,LB_GETCURSEL,0,0)) == LB_ERR) {
                  MessageBeep(0);
                  break;
              }
              SendMessage(hWnd,LB_GETTEXT,x,(LPARAM)(LPSTR)szStr);
              memset(&rmcstruct,0,sizeof(rmcstruct));

              /* to be fixed: address may be more than 8 hex digits! */
              i = sscanf(szStr,"%X %lX %lu %u", &wHandleAlt, &hdp.dwOffset, &rmcstruct.rEDX, &x );
              if ( i < 4 || wHandleAlt == 0 ) {
                  //MessageBox(0,"XMS","1",MB_OK);
                  MessageBeep(0);
                  break;
              }
              /* size (in kB) must be > 0 */
              if ( rmcstruct.rEDX == 0) {
                  //MessageBox(0,"XMS","2",MB_OK);
                  MessageBeep(0);
                  break;
              }
              /* don't map more than 64 kB */
              if ( rmcstruct.rEDX > 64 )
                  hdp.dwLength = (DWORD)64 * 1024;
              else
                  hdp.dwLength = rmcstruct.rEDX * 1024;
              wsprintf(szStr,"XMB: %X L\344nge:%lu kB)", wHandleAlt, rmcstruct.rEDX );
              hdp.dwOffset = DPMIMapPhysToLinear(hdp.dwOffset,hdp.dwLength);
              if (!hdp.dwOffset) {
                  //MessageBox(0,"XMS","3",MB_OK);
                  MessageBeep(0);
                  break;
              }
              hdp.wType = HDP_LINEAR;
              hWnd = CreateDialogParam(hInst,
                                        MAKEINTRESOURCE(IDD_VIEW),
                                        hDlg,
                                        ViewDlg,
                                        (LPARAM)(LPVOID)&hdp
                                        );
              SetWindowText(hWnd,szStr);
              break;
         case ID_LISTBOX1:
              switch(HIWORD(lParam))
                {
                 case LBN_DBLCLK:
                   SendMessage(hDlg,WM_COMMAND,ID_SUBDLG1,0);
                   break;
                  case XLBN_RBUTTONDOWN:
                    TrackPopup(hDlg,BtnTab);
                    break;
                }
              break;
         case ID_REFRESH:
              hWnd = GetDlgItem(hDlg,ID_LISTBOX1);
              SendMessage(hWnd,LB_RESETCONTENT,0,0);

              memset(&rmcstruct,0,sizeof(rmcstruct));
              /* Int2F, ax=4300h - check XMS host presence */
              rmcstruct.rAX  = 0x4300;
              DPMISimRMInterrupt(0x002F,0,&rmcstruct);
              if ( rmcstruct.rAL != 0x80 ) {
                  CreateMessage(GetParent(hDlg),MAKEINTRESOURCE(IDS_ERRXMS1),0,MB_OK);
                  PostMessage(hDlg,WM_CLOSE,0,0);
              }
              /* Int2F, ax=4310h - get XMS entry */
              rmcstruct.rAX  = 0x4310;
              DPMISimRMInterrupt(0x002F,0,&rmcstruct);
              fpXMSProc = MAKELONG(rmcstruct.rBX,rmcstruct.rES);

              /* Int 2Fh, ax=4309: get XMS handle table */
              rmcstruct.rAX  = 0x4309;
              DPMISimRMInterrupt(0x002F,0,&rmcstruct);
              if (rmcstruct.rAL != 0x43) {
                  CreateMessage(GetParent(hDlg),MAKEINTRESOURCE(IDS_ERRXMS2),0,MB_OK);
                  PostMessage(hDlg,WM_CLOSE,0,0);
                  break;
              }
              lpXMSHT = (LPXMSHT)MAKELONG(rmcstruct.rBX,DPMISegToDescriptor(rmcstruct.rES));
              lpXMSHA = (LPXMSHA)MAKELONG(LOWORD(lpXMSHT->dwHandleArray),
                                          DPMISegToDescriptor(HIWORD(lpXMSHT->dwHandleArray)));

              for (i = lpXMSHT->NumItems;i;i--,lpXMSHA++) {
                  char *p;
                  if (lpXMSHA->Flags & 2)
                      p = "allokiert";
                  else if (lpXMSHA->Flags & 1)
                      p = "frei";
                  else
                      p = "hdl frei";
                  if ( lpXMSHA->dwAddress >> 22 )
                      wsprintf(szStr,
                            "%X\t%X%08lX\t%lu\t%u\t%2X - %s",
                            LOWORD(lpXMSHA),
                            (WORD)(lpXMSHA->dwAddress >> 22),
                            lpXMSHA->dwAddress << 10,
                            lpXMSHA->dwLength,
                            lpXMSHA->Locks,
                            lpXMSHA->Flags,
                            (LPSTR)p );
                  else
                      wsprintf(szStr,
                            "%X\t%08lX\t%lu\t%u\t%2X - %s",
                            LOWORD(lpXMSHA),
                            lpXMSHA->dwAddress << 10,
                            lpXMSHA->dwLength,
                            lpXMSHA->Locks,
                            lpXMSHA->Flags,
                            (LPSTR)p );
                  SendMessage(hWnd,LB_ADDSTRING,0,(LONG)(LPSTR)szStr);
              }
              LoadTabs(IDUS_41,szStr);
              SendDlgItemMessage(hDlg,ID_STATUS1,ST_SETTABSTOPS,*(LPINT)szStr,(LONG)(LPINT)(szStr+2));

              rmcstruct.rAX = 0x0000; /* Get XMS Version Number */
              DPMICallRMFarProc(fpXMSProc,&rmcstruct,0,0);
              LoadString(hInst,IDS_XMS2,szStr1,sizeof(szStr1));
              wsprintf(szStr,
                       szStr1,
                       (WORD)rmcstruct.rAH,             /* version major */
                       (WORD)rmcstruct.rAL,             /* version minor */
                       rmcstruct.rBX,                   /* interne revisionsnummer */
                       HIWORD(fpXMSProc),               /* Entry */
                       LOWORD(fpXMSProc)
                      );
              SetDlgItemText(hDlg,ID_STATUS1,szStr);
              LoadString(hInst,IDS_XMS3,szStr1,sizeof(szStr1));

              x = rmcstruct.rAH;
              if ( x > 3 || ( x == 3 && rmcstruct.rAL > 7 )) {  /* use 3.x functions only if driver is > 3.07 */
                  rmcstruct.rAX = 0x8800; /* XMS v3 query free memory */
                  DPMICallRMFarProc(fpXMSProc,&rmcstruct,0,0);
              } else {
                  rmcstruct.rAX = 0x0800; /* XMS v2 query free memory */
                  DPMICallRMFarProc(fpXMSProc,&rmcstruct,0,0);
                  rmcstruct.rEAX &= 0xffff;
                  rmcstruct.rEDX &= 0xffff;
              }
              wsprintf(szStr, szStr1, rmcstruct.rEDX, rmcstruct.rEAX );
              SetDlgItemText(hDlg,ID_STATUS2,szStr);
              break;
         }
      rc = TRUE;
      break;
    default:
      break;
    }
 return rc;
}
