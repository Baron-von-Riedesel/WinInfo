
#include "string.h"
#include "windows.h"
#include "toolhelp.h"
#include "user.h"
#include "userw.h"
#include "user386.h"
#include "dpmi.h"
#include "wininfo.h"
#include "wininfox.h"
#include "vmm.h"

#define strcat lstrcat
#define strcpy lstrcpy

extern HWND hWndENHVxD;

static KEYENTRY vxdtab[] = {
    "VMM"       ,1,
    "Debug"     ,2,
    "VPICD"     ,3,
    "VDMAD"     ,4,
    "VTD"       ,5,
    "V86MMGR"   ,6,
    "PageSwap"  ,7,
    "Parity"    ,8,
    "ReBoot"    ,9,
    "VDD"       ,10,
    "VSD"       ,11,
    "VMD"       ,12,
    "VKD"       ,13,
    "VCD"       ,14,
    "VPD"       ,15,
    "VHD"       ,0x010,
    "VMCPD"     ,0x011,
    "EBIOS"     ,0x012,
    "BIOSXLAT"  ,0x013,
    "VNETBIOS"  ,0x014,
    "DOSMGR"    ,0x015,
    "WINLOAD"   ,0x016,
    "Shell"     ,0x017,
    "VMPoll"    ,0x018,
    "VPROD"     ,0x019,
    "DOSNET"    ,0x01A,
    "VFD"       ,0x01B,
    "VDD2"      ,0x01C,
    "WINDEBUG"  ,0x01D,
    "TSRLOAD"   ,0x01E,
    "BiosHook"  ,0x01F,  // Bios interrupt hooker VxD
    "Int13"     ,0x020,  //
    "PageFile"  ,0x021,  // Paging File device
    "SCSI"      ,0x022,  // SCSI device
    "MCA_POS"   ,0x023,  // MCA_POS device
    "SCSIFD"    ,0x024,  // SCSI FastDisk device
    "VPEND"     ,0x025,  // Pen device
    "APM"       ,0x026,  // Power Management device
    "Vxdldr"    ,0x027,
    "NDIS"      ,0x028,
    "VWIN32"    ,0x02A,
    "VCOMM"     ,0x02B,
    "WIN32S"    ,0x02D,
    "DEBUGCMD"  ,0x02E,
    "NETBEUI"   ,0x031,	// nur bis wfw 311
    "Server"    ,0x032,	// nur bis wfw 311
    "Configmg"  ,0x033,	// ab w95
    "DWCFGMG"   ,0x034,	// ab w95
    "SCSIPORT"  ,0x035,
    "VFBACKUP"  ,0x036,
    "ENABLE"    ,0x037,	// ab w95
    "VCOND"     ,0x038,	// ab w95
    "VPMTD"     ,0x03A,
    "DblSpace"  ,0x03B,
    "ISAPNP"    ,0x03C,	// ab w95
    "IFSMgr"    ,0x040,	// ab w95
    "VCDFSD"    ,0x041,	// ab w95
    "MRCI2"     ,0x042,	// ab w95
    "PCI"       ,0x043,	// ab w95
    "PELOADER"  ,0x044,	// ab w95
    "EISA"      ,0x045,	// ab w95
    "DRAGCLI"   ,0x046,	// ab w95
    "DRAGSRV"   ,0x047,	// ab w95
    "PERF"      ,0x048,	// ab w95
    "AWREDIR"   ,0x049,	// ab w95
    "VIPX"      ,0x200,
    "WinICE"    ,0x202,
    "VClient"   ,0x203,
    "BCW"       ,0x205,
    "VCommute"  ,0x234,
    "VTDAPI"    ,0x442,
    "VSBD"      ,0x445,
    "VflatD"    ,0x45D,
    "VNETSUP"   ,0x480,
    "VREDIR"    ,0x481,
    "VSHARE"    ,0x483,
    "IFSMGR"    ,0x484,
    "VFAT"      ,0x486,
    "VCACHE"    ,0x48B,
    (PSTR)0     ,0};


/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ VxDDlg - VXD Entries ausgeben                        บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL EXPORTED CALLBACK VxDDlg(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
  char str[60],str1[60];
  PSTR pstr;
  INT2FSTRUC int2f1;
  INT2FSTRUC int2f2;
  int tabpos[6];
  char vxdname[10];
  char xbytes[12];
  FARPROC lpproc;
  KEYENTRY * pKE;
  BOOL rc = FALSE;

  switch (message)
     {
      case WM_INITDIALOG:
         SendMessage(hDlg,WM_COMMAND,ID_REFRESH,0);
         ShowWindow(hDlg,SW_SHOWNORMAL);
         rc = TRUE;
         break;
      case WM_COMMAND:
         switch (wParam)
           {
           case IDOK:
           case IDCANCEL:
               PostMessage(hDlg,WM_CLOSE,0,0);
               break;
           case ID_REFRESH:
               SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_RESETCONTENT,0,0);
               tabpos[0] = 20;
               tabpos[1] = 60;
               tabpos[2] = 90;
               tabpos[3] = 150;
               tabpos[4] = 180;
               tabpos[5] = 210;
               SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETTABSTOPS,6,(LPARAM)(LPVOID)&tabpos);

               for (pKE = vxdtab;pKE->wert;pKE++)
                  {
                   int2f1.rEAX = 0x1684;
                   int2f1.rEBX = pKE->wert;
                   int2f1.rECX = 0;
                   int2f1.rEDX = 0;
                   int2f1.rEDI = 0;    /* es:di = 0000:0000 */
                   int2f1.rES  = 0;
                   if ( Int2FCall(&int2f1,&int2f2) )
                      if (int2f2.rES)
                         {
                          _fmemchr(xbytes,0,sizeof(xbytes));
                          lpproc = (FARPROC)MAKELONG(LOWORD(int2f2.rEDI),int2f2.rES);
                          int2f1.rEAX = 0;            /* funktion 0 aufrufen */
                          int2f1.rEBX = (WORD)&xbytes;
                          int2f1.rECX = 0;
                          int2f1.rEDX = 0;
                          int2f1.rESI = 0;
                          int2f1.rEDI = LOWORD(&vxdname);
                          int2f1.rES  = HIWORD(&vxdname);
                          vxdname[0] = 0;
                          vxdname[8] = 0;
//                          VxDPMCall(lpproc,&int2f1,&int2f2);
                          str1[0] = 0;
//                          if (pstr = SearchKeyTable((PSTR)&vxdtab,x))
                          pstr = pKE->name;
                          strcpy(str1,pstr);
                          wsprintf(str,
//                                   "%04X\t%s\t%X:%X\t%lX,%lX,%lX,%lX,%s",
                                   "%04X\t%s\t%X:%X",
                                   pKE->wert,
                                   (LPSTR)str1,
                                   HIWORD(lpproc),
                                   LOWORD(lpproc)
/*
                                   int2f2.rEAX,
                                   int2f2.rEBX,
                                   int2f2.rECX,
                                   int2f2.rEDX,
                                   (LPSTR)vxdname
*/
                                   );
                          SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_ADDSTRING,0,(LPARAM)(LPSTR)str);
                         }
                  }             // end for (pKE...)

               SendDlgItemMessage(hDlg,ID_LISTBOX1,LB_SETCURSEL,0,0);
               break;
           }                    // end switch wParam
         rc = TRUE;             // end WM_COMMAND
         break;
      case WM_DESTROY:
         hWndENHVxD = 0;
         break;
      case WM_CLOSE:
         DestroyWindow(hDlg);
         rc = TRUE;
         break;
     }                          /* end switch message */

  return rc;
}
