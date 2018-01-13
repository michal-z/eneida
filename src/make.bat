@echo off

set CONFIG=/Od /D_DEBUG
::set CONFIG=/Ox /DNDEBUG /GL

set CC=..\compiler\cl.exe
set ASM=..\compiler\ml64.exe

if exist ..\bin\*.exe del ..\bin\*.exe
if exist ..\bin\*.pdb del ..\bin\*.pdb
if exist *.obj del *.obj

%ASM% /c /nologo /Fo asmlib.obj library.asm

%CC% /Zi %CONFIG% /MP /Gm- /nologo /WX /W4 /wd4201 /wd4152 /wd4204 /GS- /Gs999999 /Gy /Gw /EHa- ^
    demo01.c windows.c ^
    /link asmlib.obj ..\compiler\kernel32.lib ^
    /OPT:REF /WX /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /ENTRY:start /NODEFAULTLIB /OUT:..\bin\demo01.exe

if exist *.pdb del *.pdb
if exist *.obj del *.obj
