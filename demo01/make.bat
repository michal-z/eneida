@echo off

set CFLAGS=/Od /D_DEBUG
::set CFLAGS=/O2 /GL /Oi /fp:fast
::set LFLAGS=/LTCG

set CC=..\compiler\cl.exe
set HLSL=..\compiler\fxc.exe /Ges /O3 /WX /nologo /Qstrip_reflect /Qstrip_debug /Qstrip_priv
set ASM=..\compiler\fasm.exe
set APPNAME=demo01

if exist %APPNAME%.exe del %APPNAME%.exe

%HLSL% /D VS_E01_TRIANGLE /E vs_e01_triangle /Fo vs_e01_triangle.cso /T vs_5_1 experiment01.hlsl & if errorlevel 1 goto :end
%HLSL% /D PS_E01_TRIANGLE /E ps_e01_triangle /Fo ps_e01_triangle.cso /T ps_5_1 experiment01.hlsl & if errorlevel 1 goto :end
%HLSL% /D VS_E02_TRANSFORM /E vs_e02_transform /Fo vs_e02_transform.cso /T vs_5_1 experiment02.hlsl & if errorlevel 1 goto :end
%HLSL% /D PS_E02_SHADE /E ps_e02_shade /Fo ps_e02_shade.cso /T ps_5_1 experiment02.hlsl & if errorlevel 1 goto :end

%ASM% asmlib.asm & if errorlevel 1 goto :end

%CC% /Zi /Fd%APPNAME% %CFLAGS% /MP /Gm- /nologo /GS- /Gs999999 /Gy /Gw /EHa- ^
    /WX /Wall /wd5045 /wd4820 /wd4201 /wd4221 /wd4152 /wd4204 ^
    experiment01.c experiment02.c demo01.c library.c renderer.c asmlib.obj ^
    /link ..\compiler\kernel32.lib ^
    /OPT:REF /WX /INCREMENTAL:NO %LFLAGS% /SUBSYSTEM:WINDOWS /ENTRY:start /NODEFAULTLIB /OUT:%APPNAME%.exe

:end
if exist *.obj del *.obj
if exist *.cso del *.cso

if "%1" == "run" if exist %APPNAME%.exe (.\%APPNAME%.exe)
if "%1" == "debug" if exist %APPNAME%.exe (WinDbgX /y .\ /srcpath .\ .\%APPNAME%.exe)
