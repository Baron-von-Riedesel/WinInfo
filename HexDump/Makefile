
# tools used: MSVC 1.52

MSVC  = \msvc
ODIR  = Build
name  = hexdump
model = S
lopt  = /NOD/A:16/NON/NOE/FAR/MAP:FULL/ONERROR:NOEXE
LIBPATH=$(MSVC)\lib;..\lib;
LIBS  = libw toolhelp commdlg sdllcew user386 userw suser user

all: $(ODIR) $(ODIR)\$(name).dll

$(ODIR):
	@mkdir $(ODIR)

$(ODIR)\$(name).dll: $*.obj $(name).def $*.res Makefile
	@set LIB=$(LIBPATH)
	@$(MSVC)\bin\link @<<
$* + nocidll1 + dataseg $(lopt),
$*.dll,
$*.map,
$(LIBS)
$(name).def
<<
	@$(MSVC)\bin\rc -t -31 $*.res $*.dll
	@$(MSVC)\bin\implib /NOLOGO /nowep $*.lib $(name).def

$(ODIR)\$(name).obj: $(name).cpp
	@$(MSVC)\bin\cl -c -A$(model)w -G3 -GDs -Oxz -W3 -I$(MSVC)\include -I..\include -Fo$* -DSTRICT $(name).cpp

$(ODIR)\$(name).res: $(name).rc $(name).ver
	@$(MSVC)\bin\rc -r -i$(MSVC)\include;..\include -fo$(ODIR)\$(name).res $(name).rc

clean:
	@del $(ODIR)\$(name).dll
	@del $(ODIR)\$(name).obj
	@del $(ODIR)\$(name).res
