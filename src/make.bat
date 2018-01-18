@echo off

set CFLAGS=/Od /D_DEBUG
::set CFLAGS=/Ox /GL
::set LFLAGS=/LTCG

set CC=..\compiler\cl.exe
set HLSL=..\compiler\fxc.exe /Ges /O3 /WX /nologo /Qstrip_reflect /Qstrip_debug /Qstrip_priv

%HLSL% /D_VS /E vs_main /Fo ..\bin\data\vs_triangle.cso /T vs_5_1 triangle.hlsl
if errorlevel 1 goto :end
%HLSL% /D_PS /E ps_main /Fo ..\bin\data\ps_triangle.cso /T ps_5_1 triangle.hlsl
if errorlevel 1 goto :end

if exist ..\bin\*.exe del ..\bin\*.exe
if exist ..\bin\*.pdb del ..\bin\*.pdb
if exist *.obj del *.obj

%CC% /Zi %CFLAGS% /MP /Gm- /nologo /WX /W4 /wd4201 /wd4152 /wd4204 /GS- /Gs999999 /Gy /Gw /EHa- ^
    triangle.c windows.c library.c ^
    /link ..\compiler\kernel32.lib ^
    /OPT:REF /WX /INCREMENTAL:NO %LFLAGS% /SUBSYSTEM:WINDOWS /ENTRY:start /NODEFAULTLIB /OUT:..\bin\demo01.exe

:end
if exist *.pdb del *.pdb
if exist *.obj del *.obj
