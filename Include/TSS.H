
typedef struct tagTSSENTRY
   {
     WORD link;
     WORD res1;
     FWORD ring0sp;
     WORD res2;
     FWORD ring1sp;
     WORD res3;
     FWORD ring2sp;
     WORD res4;
     DWORD cr3;
     DWORD eip;
     DWORD eflags;
     DWORD eax;
     DWORD ecx;
     DWORD edx;
     DWORD ebx;
     DWORD esp;
     DWORD ebp;
     DWORD esi;
     DWORD edi;
     WORD  es;
     WORD res5;
     WORD  cs;
     WORD res6;
     WORD  ss;
     WORD res7;
     WORD  ds;
     WORD res8;
     WORD  fs;
     WORD res9;
     WORD  gs;
     WORD res10;
     WORD  ldt;
     WORD res11;
     DWORD  parm;
     WORD iobitmapoffs;
   } TSSENTRY;

typedef TSSENTRY FAR * LPTSSENTRY;

#ifdef __cplusplus
extern "C" {
#endif
LPTSSENTRY FAR PASCAL GetTSS(WORD);
#ifdef __cplusplus
}
#endif

