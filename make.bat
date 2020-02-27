@echo off

::set CFLAGS=/Od /D_DEBUG
set CFLAGS=/O2 /Oi /fp:fast

set CC=build\clang-cl.exe
set ASM=build\fasm.exe
set APPNAME=eneida

if exist %APPNAME%.exe del %APPNAME%.exe

%ASM% alib.asm & if errorlevel 1 goto :end

%CC% /Zi %CFLAGS% /nologo /GS- /Gs999999 /Gy /Gw /EHa- /WX /W4 ^
    %APPNAME%.c alib.obj ^
    /link build\kernel32.lib /OPT:REF /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /ENTRY:start /NODEFAULTLIB ^
    /OUT:%APPNAME%.exe

:end
if exist *.obj del *.obj
