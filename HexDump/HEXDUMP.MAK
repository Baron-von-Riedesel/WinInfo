# Microsoft Visual C++ generated build script - Do not modify

PROJ = HEXDUMP
DEBUG = 0
PROGTYPE = 1
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = \Projects\WinInfo\HEXDUMP\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC =             
FIRSTCPP = HEXDUMP.CPP 
RC = \msvc\bin\rc
CFLAGS_D_WDLL = /nologo /G2 /Zp1 /W3 /Zi /ASw /Od /D "_DEBUG" /Fa /FR /GD /Fd"HEXDUMP.PDB"
CFLAGS_R_WDLL = /nologo /G3 /Zp1 /W3 /ASw /Ox /D "NDEBUG" /Fa /FR /GD 
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /MAP:FULL
LIBS_D_WDLL = oldnames libw sdllcew user386 userw user suser commdlg.lib toolhelp.lib 
LIBS_R_WDLL = oldnames libw sdllcew user386 userw user suser commdlg.lib toolhelp.lib 
RCFLAGS = /nologo 
RESFLAGS = /nologo /31 
RUNFLAGS = 
DEFFILE = HEXDUMP.DEF
OBJS_EXT = \LIB\WIN\NOCIDLL1.OBJ 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WDLL)
LFLAGS = $(LFLAGS_D_WDLL)
LIBS = $(LIBS_D_WDLL)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WDLL)
LFLAGS = $(LFLAGS_R_WDLL)
LIBS = $(LIBS_R_WDLL)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = HEXDUMP.SBR \
		DEHLP.SBR


NOCIDLL1_DEP = 

HEXDUMP_DEP = d:\csrc\inc16\user386.h \
	d:\csrc\inc16\user.h \
	d:\csrc\inc16\userw.h \
	d:\csrc\inc16\hexdump.h \
	conmenu.h


HEXDUMP_RCDEP = d:\csrc\inc16\hexdump.h \
	conmenu.rc


DEHLP_DEP = d:\csrc\inc\user386.h \
	d:\csrc\inc16\user.h \
	d:\csrc\inc16\userw.h \
	d:\csrc\inc16\hexdump.h


all:	$(PROJ).DLL $(PROJ).BSC

HEXDUMP.OBJ:	HEXDUMP.CPP $(HEXDUMP_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c HEXDUMP.CPP

HEXDUMP.RES:	HEXDUMP.RC $(HEXDUMP_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r HEXDUMP.RC

DEHLP.OBJ:	DEHLP.CPP $(DEHLP_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DEHLP.CPP


$(PROJ).DLL::	HEXDUMP.RES

$(PROJ).DLL::	HEXDUMP.OBJ DEHLP.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
HEXDUMP.OBJ +
DEHLP.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
\lib\win\+
\lib\omf\+
\MSVC\LIB\+
\MSVC\MFC\LIB\+
$(LIBS)
$(DEFFILE);
<<
	\msvc\bin\link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) HEXDUMP.RES $@
	@copy $(PROJ).CRF MSVC.BND
	\msvc\bin\implib /nowep $(PROJ).LIB $(PROJ).DLL

$(PROJ).DLL::	HEXDUMP.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) HEXDUMP.RES $@

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	\msvc\bin\bscmake @<<
/o$@ $(SBRS)
<<
