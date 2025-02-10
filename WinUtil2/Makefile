
name = winutil2
#LIBS=libw toolhelp user386 cwinrt dpmi cuserw userw cuser user
LOPTS=/NOE/NON/A:16/NOD/MAP:FULL/FAR/ONERROR:NOEXE
LIBS=libw toolhelp dpmi user386 cwinrt userw cuser user
mods = Lib\anlshl Lib\strcpy Lib\output

MSVC = \msvc
CC   = $(MSVC)\bin\cl
LINK = $(MSVC)\bin\link
RC   = $(MSVC)\bin\rc

ODIR = Build

all: $(ODIR) $(ODIR)\$(name).dll

$(ODIR):
	@mkdir $(ODIR)

$(ODIR)\$(name).dll: $(ODIR)\$(name).obj $(name).def $(ODIR)\$(name).res Makefile
	@set lib=$(MSVC)\lib;..\lib;Lib;
	@$(LINK) @<<
$* $(mods) $(LOPTS),
$*.dll,
$*.map,
$(LIBS),
$(name).def;
<<
	@$(RC) -t -31 $*.res $*.dll
	@$(MSVC)\bin\implib /NOLOGO /nowep $*.lib $(name).def

$(ODIR)\$(name).res: $(name).rc $(name).ver
	@$(RC) -r -fo$*.res -i$(MSVC)\include;..\include $(name).rc

$(ODIR)\$(name).obj: $(name).asm
	@jwasm -c -nologo -Fl$(ODIR)\ -Fo$(ODIR)\ $(name).asm

clean:
	@del $(ODIR)\winutil2.obj
	@del $(ODIR)\*.dll
	@del $(ODIR)\*.res
