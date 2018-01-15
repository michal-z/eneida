@echo off

set CFLAGS=/Od /D_DEBUG
::set CFLAGS=/Ox /GL
::set LFLAGS=/LTCG

set CC=..\compiler\cl.exe

if exist ..\bin\*.exe del ..\bin\*.exe
if exist ..\bin\*.pdb del ..\bin\*.pdb
if exist *.obj del *.obj

%CC% /Zi %CFLAGS% /MP /Gm- /nologo /WX /W4 /wd4201 /wd4152 /wd4204 /GS- /Gs999999 /Gy /Gw /EHa- ^
    demo01.c windows.c library.c ^
    /link ..\compiler\kernel32.lib ^
    /OPT:REF /WX /INCREMENTAL:NO %LFLAGS% /SUBSYSTEM:WINDOWS /ENTRY:start /NODEFAULTLIB /OUT:..\bin\demo01.exe

if exist *.pdb del *.pdb
if exist *.obj del *.obj
