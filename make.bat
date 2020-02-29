@echo off

if "%1" == "fmt" (
    "build\clang-format.exe" -i *.c *.h ^
    -style="{IndentWidth: 4, TabWidth: 4, ColumnLimit: 105}"
    goto :end
)

set CFLAGS=/Od /D_DEBUG
::set CFLAGS=/O2 /Oi /fp:fast

set CC=build\clang-cl.exe
set APPNAME=eneida

if exist %APPNAME%.exe del %APPNAME%.exe

%CC% /Zi %CFLAGS% /nologo /GS- /Gs999999 /Gy /Gw /EHa- /W4 ^
    %APPNAME%.c ^
    /link build\kernel32.lib /OPT:REF /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /ENTRY:start /NODEFAULTLIB ^
    /OUT:%APPNAME%.exe

:end
if exist *.obj del *.obj
