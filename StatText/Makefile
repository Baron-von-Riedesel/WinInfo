
name  = StatText
MODEL = S
LOPTS = /NOLOGO/NOD/A:16/NON/NOE/FAR/MAP:FULL
LIBS  = libw $(MODEL)dllcew
MSVC  = \msvc
CC    = $(MSVC)\bin\cl
LINK  = $(MSVC)\bin\link
RC    = $(MSVC)\bin\rc
LIBPATH=$(MSVC)\lib;..\lib;

COPT  = -c -A$(model)w -G3Ds -Oxz -W3 -DSTRICT -I$(MSVC)\include -I..\include
COPTD =

ODIR = Build

all: $(ODIR) $(ODIR)\$(name).dll

$(ODIR):
	@mkdir $(ODIR)

$(ODIR)\$(name).dll: $*.obj $(name).def $*.res Makefile
    @set LIB=$(LIBPATH)
	@$(LINK) $(LOPTS) @<<
$* + nocidll1 + winheap,
$*.dll,
$*,
$(LIBS),
$(name).def
<<
	@$(RC) -t -31 $*.res $*.dll
	@$(MSVC)\bin\implib /NOLOGO /nowep $*.lib $(name).def

$(ODIR)\$(name).obj: $(name).cpp Resource.h
	@$(CC) $(COPT) $(COPTD) -Fa$* -Fo$* $(name).cpp

$(ODIR)\$(name).res: $(name).rc Resource.h $(name).ver
	@$(RC) -r -i$(MSVC)\include -fo$*.res $(name).rc

clean:
	@del $(ODIR)\*.obj
	@del $(ODIR)\*.map
	@del $(ODIR)\*.dll
	@del $(ODIR)\*.res
