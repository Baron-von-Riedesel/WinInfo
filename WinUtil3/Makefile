
name   = winutil3
name1  = winutil3
LOPTS  = /NOLOGO/FAR/MAP:FULL/A:16/NON/ONERROR:NOEXE
LIBS   = libw toolhelp user386 libc32vx suserw userw suser user
MSVC   = \msvc
LIBPATH= $(MSVC)\lib;..\lib;Lib;

LINK   = $(MSVC)\bin\link.exe
RC     = $(MSVC)\bin\rc.exe
IMPLIB = $(MSVC)\bin\implib.exe

ODIR = Build

all: $(ODIR) $(ODIR)\$(name1).dll

$(ODIR)\$(name1).dll: $*.obj $(ODIR)\unassem.obj $(name).def $*.res Makefile
	@set LIB=$(LIBPATH)
	@$(LINK) $(LOPTS) @<<
$* Lib\disasm.obj $(ODIR)\unassem.obj,
$*.dll,
$*.map,
$(LIBS),
$(name);
<<
	@$(RC) -t -31 $*.res $*.dll
	@$(IMPLIB) /nowep $*.lib $(name).def

$(ODIR)\$(name).obj: $(name).asm
	@jwasm -nologo -c -Fl$(ODIR)\ -Fo$(ODIR)\ -Cp -Sg $(name).asm

$(ODIR)\unassem.obj: unassem.asm
	@jwasm -nologo -c -Fl$(ODIR)\ -Fo$(ODIR)\ -Sg unassem.asm

$(ODIR)\$(name).res: $(name).rc $(name).ver
	@$(RC) -r -fo$*.res -i$(MSVC)\include;..\include $(name).rc

clean:
	@del $(ODIR)\*.obj
	@del $(ODIR)\*.lst
	@del $(ODIR)\$(name).res
	@del $(ODIR)\$(name).dll
	@del $(ODIR)\$(name).lib
