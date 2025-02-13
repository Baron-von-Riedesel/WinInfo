
name = WinUtil1
model= S
MSVC = \msvc
CC   = $(MSVC)\bin\cl
LINK = $(MSVC)\bin\link
RC   = $(MSVC)\bin\rc
OBJS = $(model)windll
LIBPATH = \msvc\lib;\lib\win;\lib\omf;
LIBS = libw toolhelp $(model)dllcew $(model)userw userw $(model)user user

COPTS = -c -A$(model)w -Zp1 -G3Ds -Oxz -W3 -nologo -I$(MSVC)\include -I..\include
LOPTS=/NOLOGO/NOD/A:16/NON/NOE/MAP:FULL/FAR/ONE:NOE

mod1 = creatmsg
mod2 = opendlg

ODIR = Build

all: $(ODIR) $(ODIR)\$(name).dll

$(ODIR):
	@mkdir $(ODIR)

$(ODIR)\$(name).dll: $(ODIR)\$(name).obj $(ODIR)\$(mod1).obj $(ODIR)\$(mod2).obj $(ODIR)\$(name).res $(name).def Makefile
	@set LIB=$(LIBPATH)
	@$(LINK) $(LOPTS) @<<
$* + $(OBJS) + $(ODIR)\$(mod1) + $(ODIR)\$(mod2),
$*.dll,
$*.map,
$(LIBS),
$(name).def;
<<
	@$(RC) -t -31 $*.res $*.dll
	@$(MSVC)\bin\implib /NOLOGO /nowep $*.lib $(name).def

$(ODIR)\$(name).obj: $(name).c
	@$(CC) $(COPTS) -Fa$* -Fo$* $(name).c

$(ODIR)\$(mod1).obj: $(mod1).c
	@$(CC) $(COPTS) -Fo$* $(mod1).c

$(ODIR)\$(mod2).obj: $(mod2).c $(mod2).h
	@$(CC) $(COPTS) -Fo$* $(mod2).c

$(ODIR)\$(name).res: $(name).rc $(name).ver $(mod2).ico
	@$(RC) -r -i$(MSVC)\include -i..\include -fo$*.res $(name).rc

clean:
	@del $(ODIR)\*.obj
	@del $(ODIR)\*.res
	@del $(ODIR)\*.dll
