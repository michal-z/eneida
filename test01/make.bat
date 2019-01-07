@echo off

set CFLAGS=/Od /D_DEBUG
::set CFLAGS=/Od /Zi /D_DEBUG
::set CFLAGS=/O2 /fp:fast

set CC=..\compiler\cl.exe

set APPNAME=test01

if exist %APPNAME%.exe del %APPNAME%.exe

%CC% %CFLAGS% /Gm- /nologo /GS- /Gs999999 /Gy /Gw /EHa- /Bt /WX /Wall /wd4710 /wd4711 /wd5045 /wd4820 /wd4201 /wd4221 /wd4152 /wd4204 %APPNAME%.c ^
     /link ..\compiler\kernel32.lib /OPT:REF /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /ENTRY:Start /NODEFAULTLIB /OUT:%APPNAME%.exe

if exist *.obj del *.obj

:end

if "%1" == "run" if exist %APPNAME%.exe (.\%APPNAME%.exe)
