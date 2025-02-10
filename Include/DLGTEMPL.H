
typedef struct tagDLGTEMPLATE
  {
    DWORD dtStyle;
    BYTE  dtItemCount;
    WORD  dtX;
    WORD  dtY;
    WORD  dtCX;
    WORD  dtCY;
    char  dtMenuName[1];
  } DLGTEMPLATE;

typedef DLGTEMPLATE FAR *LPDLGTEMPLATE;

typedef struct tagDLGITEMTEMPLATE
  {
    WORD  dtilX;
    WORD  dtilY;
    WORD  dtilCX;
    WORD  dtilCY;
    WORD  dtilID;
    DWORD dtilStyle;
    char  dtilClass[1];
  } DLGITEMTEMPLATE;

typedef DLGITEMTEMPLATE FAR *LPDLGITEMTEMPLATE;

