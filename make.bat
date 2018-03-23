@echo off

::set CFLAGS=/Od /D_DEBUG
set CFLAGS=/Ox /Oi /fp:fast

set CC=compiler\cl.exe

%CC% /Zi %CFLAGS% /MP /Gm- /nologo /WX /W4 /wd4201 /wd4221 /wd4152 /wd4204 /GS- /Gs999999 /Gy /Gw /EHa- ^
    qjulia.c ^
    /link compiler\kernel32.lib ^
    /OPT:REF /WX /INCREMENTAL:NO %LFLAGS% /SUBSYSTEM:WINDOWS /ENTRY:start /NODEFAULTLIB /OUT:qjulia.exe

:end
if exist *.pdb del *.pdb
if exist *.obj del *.obj
