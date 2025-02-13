
# nmake will create wininfo.exe
# to create a debug version enter "nmake debug=1"
#
# - tools used: MSVC v1.5 and jwasm
# - adjust path for MSVC below
# - jwasm must be located in PATH

!ifndef DEBUG
DEBUG=0
!endif

NAME=WinInfo
MSVC=\msvc

!if $(DEBUG)
COPTD=-D "_DEBUG" -Zi -Od -G2
AOPTD=-D_DEBUG -Zi
LOPTD=/CO
OUTDIR=DEBUG
!else
COPTD=-D "NDEBUG" -O1 -G3 
AOPTD=
LOPTD=
OUTDIR=RELEASE
!endif

SRCMODS = \
!include Modules.inc

OBJNAMES = $(SRCMODS:.cpp=.obj)
!if $(DEBUG)
OBJMODS = $(OBJNAMES:.\=DEBUG\)
!else
OBJMODS = $(OBJNAMES:.\=RELEASE\)
!endif

ASM =jwasm.exe
CC  =$(MSVC)\bin\cl.exe
LINK=$(MSVC)\bin\link.exe
LIB =$(MSVC)\bin\lib.exe
RC  =$(MSVC)\bin\rc.exe
HC  =$(MSVC)\bin\hc31.exe

AOPT = -c -nologo -Sg -Cp $(AOPTD)
COPT = -c -nologo -Gs -GA -Zp1 -W3 -I$(MSVC)\include -IInclude -AM -D "STRICT" $(COPTD)
LOPTS= /NOLOGO/MAP:FULL/ONE:NOE/NOD/A:16/NOE/ST:8192
LIBS = $(OUTDIR)\WinInfo.lib libw.lib toolhelp hexdump stattext winutil1 winutil2 winutil3 xlistbox mlibcew user386 userw user oldnames
LIBPATH= $(MSVC)\lib;Lib;

.SUFFIXES: .asm .obj .cpp

.asm{$(OUTDIR)}.obj:
	@$(ASM) $(AOPT) -Fl$* -Fo$* $<

{src}.cpp{$(OUTDIR)}.obj:
	@$(CC) $(COPT) -Fo$* $<

ALL: $(OUTDIR) $(OUTDIR)\$(NAME).exe $(OUTDIR)\$(NAME).hlp

$(OUTDIR):
	@mkdir $(OUTDIR)
    
$(OUTDIR)\$(NAME).exe: $*.lib $*.res Makefile
	@set LIB=$(LIBPATH)
	@$(LINK) @<<
$*.obj $(LOPTS) $(LOPTD),
$*.exe,
$*.map,
$(LIBS),
$(NAME).def
<<
	@$(RC) /nologo /31 $*.res $*.exe 
 
$(OUTDIR)\$(NAME).lib: $(OBJMODS) Makefile
	@cd $(OUTDIR)
	@erase $(NAME).lib
	@$(LIB) /nologo $(NAME).lib $(OBJNAMES:.\=+);
	@cd ..

$(OUTDIR)\$(NAME).res: $(NAME).rc Res\Witabpos.bin
	@$(RC) -r -iinclude;$(MSVC)\include -fo $*.res $(NAME).rc

Res\WItabpos.bin: WItabpos.asm
	@$(ASM) $(AOPT) -bin -Fl$* -Fo$*.bin WItabpos.asm

$(OBJMODS): WinInfo.h Resource.h WinInfoX.h

$(OUTDIR)\$(NAME).hlp: $(NAME).rtf $(NAME).hpj
	@cd $(OUTDIR)
	@$(HC) ..\$(NAME).hpj
	@cd ..

clean:
	@del $(OUTDIR)\*.obj
	@del $(OUTDIR)\*.exe
	@del $(OUTDIR)\*.res
	@del $(OUTDIR)\*.hlp
	@del $(OUTDIR)\*.lib
