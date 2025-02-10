
name  = winutil3
name1 = wutil286
LIBS  = libw toolhelp swinrt slibc libh suserw userw suser user
LOPTS = /FAR/MAP:FULL/A:16/NON

$(name1).dll: $(name1).obj $(name).def wutil286.mak $(name).res
    link @<<
$(name1),
$(name1).dll,
$(name1).map,
$(LIBS),
$(name).def $(LOPTS)
<<
    rc -t -30 $(name).res $(name1).dll

$(name1).obj: $*.asm
     masm -Zi -l $*.asm;

$(name).res: $*.rc
    rc -r $*.rc

