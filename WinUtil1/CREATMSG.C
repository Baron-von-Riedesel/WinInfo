
#include "string.h"
#include "windows.h"
#include "user.h"
#include "userw.h"
#include "dlgtempl.h"

#define ID_CMSGTEXT 51

#define OKPOS1 0x1E
#define OKCX1  0x32
#define OKPOS2 0x32
#define OKCX2  0x3C

extern HINSTANCE hInst;

struct {
    DLGTEMPLATE dt;
    BYTE dtClassName;
    BYTE dtCaptionText;
    WORD PointSize;
    char szTypeFace[5];
    DLGITEMTEMPLATE dlgitmp2;
    char dtil2Text1[1];
    char dtil2Info;
    DLGITEMTEMPLATE dlgitmp1;
    BYTE dtil1Text1;
    WORD dtil1Text2;
    char dtil1Info;
    DLGITEMTEMPLATE dlgitmp3;
    char dtil3Text[1];
    char dtil3Info;
    DLGITEMTEMPLATE dlgitmp4;
    BYTE dtil4Text[1];
    char dtil4Info;
} dlgtemp = {
    {0x80C801C0,4,20,20,0xA0,0x32,0},
    0,0,8,"Helv",
    {0x8,6,0x90,0x18,ID_CMSGTEXT,0x50000001,0x82}, /* fehlertext */
    "", 0,
    {4,8,0,0,-1,0x50000003,0x82}, /* ICON */
    0xFF, 0x7F03, 0,
    {0x00,0x1F,0x00,0x0e,IDOK,0x50010000,0x80}, /* OK */
    "", 0,
    {0x5a,0x1F,0x32,0x0e,IDCANCEL,0x50010000,0x80}, /* Cancel */
    "", 0
};

/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ Dialogfunktion                                                    บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int __export CALLBACK CreateMsgDlg(HWND hDlg,unsigned message,WORD wParam,LONG lParam)
{
    LPSTR lpstr1;
    LPSTR lpstr2;
    WORD id;
    WORD type;
    char str[80];
    BOOL rc;

    rc = FALSE;

    switch(message) {
    case WM_INITDIALOG:
        lpstr1  = *((LPSTR FAR *)lParam);
        lpstr2  = *(((LPSTR FAR *)lParam)+1);
        type    = LOWORD(*(((LPSTR FAR *)lParam)+2));
        id      = HIWORD(*(((LPSTR FAR *)lParam)+2));
        if (lpstr2)
            if (HIWORD(lpstr2))
                SetWindowText(hDlg,lpstr2);
            else
            {
                if (!LoadString(hInst,LOWORD(lpstr2),str,sizeof(str)))
                    _fstrcpy(str,"???");
                SetWindowText(hDlg,str);
            }
        else
            SetWindowText(hDlg,GetStdStringAddress(IDS_ERROR));

        SetWindowWord(hDlg,DWL_USER,id);

        if (HIWORD(lpstr1))
            SetDlgItemText(hDlg,ID_CMSGTEXT,lpstr1);
        else
        {
            if (!LoadString(GetTaskDS(),LOWORD(lpstr1),str,sizeof(str)))
                _fstrcpy(str,"???");
            SetDlgItemText(hDlg,ID_CMSGTEXT,str);
        }
        EnableWindow(GetParent(hDlg),0);

        SetDlgItemText(hDlg,IDCANCEL,GetStdStringAddress(IDS_CANCEL));
        SetDlgItemText(hDlg,IDOK,GetStdStringAddress(IDS_OK));

        ShowWindow(hDlg,SHOW_OPENWINDOW);
        rc = TRUE;
        break;
    case WM_CLOSE:
        EnableWindow(GetParent(hDlg),1);
        DestroyWindow(hDlg);
        rc = TRUE;
        break;
    case WM_COMMAND:
        id = GetWindowWord(hDlg,DWL_USER);
        switch(wParam) {
        case IDCANCEL:
        case IDOK:
            if (id)
                PostMessage(GetParent(hDlg),
                            WM_COMMAND,
                            id,
                            MAKELONG(0,wParam));
            PostMessage(hDlg,WM_CLOSE,0,0);
            break;
        }
        rc = TRUE;         /* WM_COMMAND immer RC TRUE */
        break;
    }
    return rc;
}
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ asynchrone MessageBox                                             บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int __export WINAPI CreateMessageParam(HWND hDlg,LPSTR lptext,LPSTR lpcap,WORD type,WORD id)
{
    DWORD lstr[3];

    lstr[0] = (DWORD)lptext;
    lstr[1] = (DWORD)lpcap;
    lstr[2] = MAKELONG(type,id);

    if (type == MB_OKCANCEL) {
        dlgtemp.dt.dtItemCount  = 4;
        dlgtemp.dlgitmp3.dtilX  = OKPOS1;
        dlgtemp.dlgitmp3.dtilCX = OKCX1;
    } else {
        dlgtemp.dt.dtItemCount  = 3;
        dlgtemp.dlgitmp3.dtilX  = OKPOS2;
        dlgtemp.dlgitmp3.dtilCX = OKCX2;
    }
    return CreateDialogIndirectParam(hInst,(LPSTR)&dlgtemp,hDlg,CreateMsgDlg,(LONG)(LPSTR)&lstr);
}
/*
ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ asynchrone MessageBox                                             บ
ศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
int __export WINAPI CreateMessage(HWND hDlg,LPSTR lptext,LPSTR lpcap,WORD type)
{
    return CreateMessageParam(hDlg,lptext,lpcap,type,0);
}
