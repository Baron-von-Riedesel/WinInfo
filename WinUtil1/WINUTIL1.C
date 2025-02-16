
#include "windows.h"
#include "user.h"
#include "userw.h"
#include "string.h"

HANDLE hInst = 0;

static BOOL   fInit = FALSE;

static NPSTR pszStd[8];

static STRLOADENTRY CODESEG strloadtab[] = {
                       &pszStd[IDS_OK],       IDS_OK,
                       &pszStd[IDS_CANCEL],   IDS_CANCEL,
                       &pszStd[IDS_ERROR],    IDS_ERROR,
                       &pszStd[IDS_WARNING],  IDS_WARNING,
                       &pszStd[IDS_HINT],     IDS_HINT,
                       &pszStd[IDS_YES],      IDS_YES,
                       &pszStd[IDS_NO],       IDS_NO,
                       (PSTR *)(0)};


/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ GetStdString                                         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
WORD __export WINAPI GetStdString(WORD id,LPSTR lpStr,WORD wSize)
/////////////////////////////////////////////////////////////////
{
 PSTR *ppStr;
 PSTR pStr;

 if (!fInit)
    {
     fInit = TRUE;
     LoadStringTab(hInst,strloadtab);
    }
 ppStr = &pszStd;
 ppStr = ppStr + id;
 pStr  = *ppStr;
 _fstrcpy(lpStr,pStr);
 return strlen(pStr);
}

LPSTR __export WINAPI GetStdStringAddress(WORD wID)
///////////////////////////////////////////////////
{
 if (!fInit)
    {
     fInit = TRUE;
     LoadStringTab(hInst,strloadtab);
    }
 return (LPSTR)(NPSTR)*(pszStd+wID);
}
/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ WEP - Windows Exit Procedure                         บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
BOOL __export WINAPI WEP(WORD code)
{
  return TRUE;
}
/* must be far if just MSVC sdllcew.lib is used;
 * must be near if module sdllwin.obj is added.
 *
 */
int FAR PASCAL LibMain(HANDLE hInstance,WORD hPrevInstance,WORD x,LPSTR y)
{
    hInst = hInstance;
    return TRUE;
}
