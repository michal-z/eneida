@echo off

::set CFLAGS=/Od /D_DEBUG
set CFLAGS=/O2 /Oi /fp:fast

set CC=..\compiler\cl.exe
set APPNAME=Demo01

if exist %APPNAME%.exe del %APPNAME%.exe

%CC% /Zi %CFLAGS% /Gm- /nologo /GS- /Gs999999 /Gy /Gw /EHa- ^
    /WX /Wall /wd4710 /wd4711 /wd5045 /wd4820 /wd4201 /wd4221 /wd4152 /wd4204 ^
    demo01.c ^
    /link ..\compiler\kernel32.lib ^
    /OPT:REF /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /ENTRY:Start /NODEFAULTLIB /OUT:%APPNAME%.exe

:end
if exist *.obj del *.obj

if "%1" == "run" if exist %APPNAME%.exe (.\%APPNAME%.exe)
::if "%1" == "debug" if exist %APPNAME%.exe (WinDbgX /y .\ /srcpath .\ .\%APPNAME%.exe)
::if "%1" == "debug" if exist %APPNAME%.exe (WinDbgX .\%APPNAME%.exe)
