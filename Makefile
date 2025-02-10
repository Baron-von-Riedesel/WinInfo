
# nmake will create wininfo.exe
# to create a debug version enter "nmake debug=1"
#
# - tools used: MSVC v1.5 and jwasm
# - adjust path for MSVC below
# - jwasm must be located in PATH

!ifndef DEBUG
DEBUG=0
!endif

NAME=wininfo
MSVC=\msvc

!if $(DEBUG)
COPTD=-D "_DEBUG" -Zi -Od
AOPTD=-D_DEBUG -Zi
LOPTD=/DEBUG
OUTDIR=DEBUG
!else
COPTD=-D "NDEBUG" -O1 
AOPTD=
LOPTD=
OUTDIR=RELEASE
!endif

SRCMODS = \
!include modules.inc

OBJNAMES = $(SRCMODS:.cpp=.obj)
!if $(DEBUG)
OBJMODS = $(OBJNAMES:.\=DEBUG\)
!else
OBJMODS = $(OBJNAMES:.\=RELEASE\)
!endif

ASM=jwasm.exe -c -nologo -Sg -Cp -Fl$* -Fo$* $(AOPTD)
CC=$(MSVC)\bin\cl.exe -c -nologo -Gs -GA -G3 -Zp1 -W3 -I$(MSVC)\include -IInclude -AM -D "STRICT" -Fo$* $(COPTD)

LOPTS=/NOLOGO/MAP:FULL/ONE:NOE/NOD/A:16/NOE/ST:8192
LIBS=$(OUTDIR)\wininfo.lib libw.lib toolhelp hexdump stattext winutil1 winutil2 winutil3 xlistbox mlibcew user386 userw user oldnames
LINK=$(MSVC)\bin\link.exe
LIB=$(MSVC)\bin\lib.exe
RC=$(MSVC)\bin\rc.exe
HC=$(MSVC)\bin\hc31.exe
LIBPATH=$(MSVC)\lib;Lib;

.SUFFIXES: .asm .obj .cpp

.asm{$(OUTDIR)}.obj:
	@$(ASM) $<

.cpp{$(OUTDIR)}.obj:
	@$(CC) $<


ALL: $(OUTDIR) $(OUTDIR)\$(NAME).exe $(OUTDIR)\$(NAME).hlp

$(OUTDIR):
	@mkdir $(OUTDIR)
    
$(OUTDIR)\$(NAME).exe: $(OUTDIR)\$(NAME).lib $(OUTDIR)\$(NAME).res makefile
	set LIB=$(LIBPATH)
	@$(LINK) @<<
$(OUTDIR)\$(NAME) $(LOPTS) $(LOPTD),
$*,
$*,
$(LIBS),
$(NAME).def
<<
	@$(RC) /nologo /31 $*.res $*.exe 
 
$(OUTDIR)\$(NAME).lib: $(OBJMODS) makefile
	@cd $(OUTDIR)
	@erase $(NAME).lib
	@$(LIB) $(NAME).lib $(OBJNAMES:.\=+);
    @cd ..

$(OUTDIR)\$(NAME).res: $(NAME).rc Res\Witabpos.bin Res\Wiabout1.res
	@$(RC) -r -iinclude;$(MSVC)\include -fo $*.res $(NAME).rc

Res\wiabout1.res: wiabout1.rc
	@$(RC) -r -iinclude;$(MSVC)\include -fo $*.res wiabout1.rc

Res\Witabpos.bin: Witabpos.asm
	@jwasm -bin -Fo$*.bin Witabpos.asm

$(OBJMODS): wininfo.h resource.h wininfox.h

$(OUTDIR)\$(NAME).hlp: $(NAME).rtf
	@cd $(OUTDIR)
	@$(HC) ..\$(NAME).hpj
	@cd ..

clean:
	@del $(OUTDIR)\*.obj
	@del $(OUTDIR)\*.exe
	@del $(OUTDIR)\*.res
