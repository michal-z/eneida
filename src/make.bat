@echo off

set CFLAGS=/Od /D_DEBUG
::set CFLAGS=/Ox /GL /Oi /fp:fast
::set LFLAGS=/LTCG

set CC=..\compiler\cl.exe
set HLSL=..\compiler\fxc.exe /Ges /O3 /WX /nologo /Qstrip_reflect /Qstrip_debug /Qstrip_priv
set ASM=..\compiler\ml64.exe

::%HLSL% /D_VS /E main /Fo ..\bin\data\test_triangle.vso /T vs_5_1 test_triangle.hlsl & if errorlevel 1 goto :end
::%HLSL% /D_PS /E main /Fo ..\bin\data\test_triangle.pso /T ps_5_1 test_triangle.hlsl & if errorlevel 1 goto :end

%ASM% /c /nologo /Fo asmlib.obj asmlib.asm

%CC% /Zi %CFLAGS% /MP /Gm- /nologo /WX /W4 /wd4201 /wd4221 /wd4152 /wd4204 /GS- /Gs999999 /Gy /Gw /EHa- ^
    demo01.c windows.c library.c asmlib.obj ^
    /link ..\compiler\kernel32.lib ^
    /OPT:REF /WX /INCREMENTAL:NO %LFLAGS% /SUBSYSTEM:WINDOWS /ENTRY:start /NODEFAULTLIB /OUT:..\bin\demo01.exe

:end
if exist *.pdb del *.pdb
if exist *.obj del *.obj
