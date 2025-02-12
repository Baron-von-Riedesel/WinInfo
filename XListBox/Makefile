
NAME = xlistbox

MSVC = \msvc
MODEL= S
ODIR=Build

LOPT = /NOLOGO/NOD/A:16/NON/NOE/MAP:FULL/FAR/ONERROR:NOEXE
LIBS = libw shell $(MODEL)dllcew userw user386 $(MODEL)user user
CC   = $(MSVC)\bin\cl -Fo$* -c -A$(model)w -G3Ds -Oxz -W3 -DSTRICT -I$(MSVC)\include -I..\include
LINK = $(MSVC)\bin\link
RC   = $(MSVC)\bin\rc
LIBPATH = $(MSVC)\Lib;..\lib;

all: $(ODIR) $(ODIR)\$(NAME).dll

$(ODIR):
	@mkdir $(ODIR)

$(ODIR)\$(NAME).dll: $*.obj $*.res $(NAME).def Makefile
	@set LIB=$(LIBPATH)
	@$(LINK) $(LOPT) @<<
$* nocidll1 winheap,
$*.dll,
$*.map,
$(LIBS),
$(NAME).def
<<
	@$(RC) -t -31 $*.res $*.dll
	@$(MSVC)\bin\implib /NOLOGO /nowep $*.lib $(NAME).def

$(ODIR)\$(NAME).obj: $(NAME).cpp Resource.h
	@$(CC) $(NAME).cpp

$(ODIR)\$(NAME).res: $(NAME).rc Resource.h $(NAME).ver
	@$(RC) -r -i$(MSVC)\include -fo$*.res $(NAME).rc

clean:
	@del $(ODIR)\*.obj
	@del $(ODIR)\*.res
	@del $(ODIR)\*.dll
