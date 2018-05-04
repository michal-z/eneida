@echo off

set CFLAGS=/Od
::set CFLAGS=/O2 /GL /Oi /fp:fast
::set LFLAGS=/LTCG

set CC=..\compiler\cl.exe
set HLSL=..\compiler\fxc.exe /Ges /O3 /WX /nologo /Qstrip_reflect /Qstrip_debug /Qstrip_priv
set ASM=..\compiler\fasm.exe

%HLSL% /D VS_TRIANGLE /E vs_triangle /Fo vs_triangle.cso /T vs_5_1 triangle.hlsl & if errorlevel 1 goto :end
%HLSL% /D PS_TRIANGLE /E ps_triangle /Fo ps_triangle.cso /T ps_5_1 triangle.hlsl & if errorlevel 1 goto :end

%ASM% asmlib.asm & if errorlevel 1 goto :end

%CC% /Zi %CFLAGS% /MP /Gm- /nologo /WX /W4 /wd4201 /wd4221 /wd4152 /wd4204 /GS- /Gs999999 /Gy /Gw /EHa- ^
    demo01.c main.c library.c renderer.c asmlib.obj ^
    /link ..\compiler\kernel32.lib ^
    /OPT:REF /WX /INCREMENTAL:NO %LFLAGS% /SUBSYSTEM:WINDOWS /ENTRY:start /NODEFAULTLIB /OUT:demo01.exe

:end
if exist vc140.pdb del vc140.pdb
if exist *.obj del *.obj
if exist *.cso del *.cso
