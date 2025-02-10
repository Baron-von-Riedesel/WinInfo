
# WinInfo

## 1. About

 It's a cool 16-bit Windows App, written in the early 1990s. Display a lot of
 information about Windows internals. Compatible with Windows 3.1, 9x and Me.
 Also works in NTVDM, but due to the nature of this emulation the usefulness is
 perhaps limited there.

 Catch: user interface is in German. Also, all comments in the source are in
 this language.

 The app consists of an executable and **6** dlls. A Windows help file is also
 included.


## 2. Creating the Binaries

 One needs MS VC 1.52 and Masm/JWasm. The dlls all have their own makefile, so
 one has to run nmake 7 times, for each dll and the executable itself.

 Some low-level assembly code isn't included as source yet, it's in .obj or
 .lib format currently. To be fixed...

