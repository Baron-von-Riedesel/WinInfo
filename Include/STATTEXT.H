
                                          /* statustext window styles */
#define STS_SEPARATOR     0x0001L
#define STS_NOFRAMES      0x0002L
#define STS_DEFALTFONT    0x0004L
#define STS_WINDOWBK      0x0008L
#define STS_SIDEBORDER    0x0010L
#define STS_TRANSPARENT   0x0020L
#define STS_CAPTIONBORDER STS_SEPARATOR | STS_SIDEBORDER
                                          /* messages fuer statustext */
#define ST_SETTABSTOPS        (WM_USER+1)
#define ST_SETSEPARATOR       (WM_USER+2)
#define ST_SETALTFONT         (WM_USER+3)
#define ST_SETTEXTFIELD       (WM_USER+4)

#define CTLCOLOR_STATTEXT 8

#ifdef __cplusplus
extern "C" {
#endif

BOOL WINAPI StatustextRegisterClass(HINSTANCE, LPSTR, WORD);
BOOL WINAPI StatustextUnregisterClass(HINSTANCE, LPSTR);

#ifdef __cplusplus
}
#endif

