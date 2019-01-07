@echo off

set CFLAGS=/Od /D_DEBUG
::set CFLAGS=/Od /Zi /D_DEBUG
::set CFLAGS=/O2 /fp:fast

set CC=..\compiler\cl.exe
set HLSLC=..\compiler\dxc.exe /Ges /O3 /WX /nologo

set NAME=test01
 
%HLSLC% /D VS_01 /E VertexMain /Fo data\01.vs.cso /T vs_6_0 %NAME%.hlsl & if ERRORLEVEL 1 goto :end
%HLSLC% /D PS_01 /E PixelMain /Fo data\01.ps.cso /T ps_6_0 %NAME%.hlsl & if ERRORLEVEL 1 goto :end

if exist %NAME%.exe del %NAME%.exe

%CC% %CFLAGS% /Gm- /nologo /GS- /Gs999999 /Gy /Gw /EHa- /Bt /WX /Wall /wd4710 /wd4711 /wd5045 /wd4820 /wd4201 /wd4221 /wd4152 /wd4204 %NAME%.c ^
     /link ..\compiler\kernel32.lib /OPT:REF /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /ENTRY:Start /NODEFAULTLIB

if exist *.obj del *.obj

:end

if "%1" == "run" if exist %NAME%.exe (.\%NAME%.exe)
