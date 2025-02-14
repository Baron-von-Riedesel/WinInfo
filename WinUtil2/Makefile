
name = WinUtil2

MSVC = \msvc
CC   = $(MSVC)\bin\cl
LINK = $(MSVC)\bin\link
RC   = $(MSVC)\bin\rc

LOPTS=/NOLOGO/NOE/NON/A:16/NOD/MAP:FULL/FAR/ONERROR:NOEXE
LIBS=libw toolhelp dpmi user386 cwinrt userw cuser user
mods = Lib\anlshl Lib\strcpy Lib\output

ODIR = Build

all: $(ODIR) $(ODIR)\$(name).dll

$(ODIR):
	@mkdir $(ODIR)

$(ODIR)\$(name).dll: $(ODIR)\$(name).obj $(name).def $(ODIR)\$(name).res Makefile
	@set lib=..\lib;Lib;$(MSVC)\lib;
	@$(LINK) $(LOPTS) @<<
$* $(mods),
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
	@del $(ODIR)\$(name).obj
	@del $(ODIR)\$(name).dll
	@del $(ODIR)\$(name).res
