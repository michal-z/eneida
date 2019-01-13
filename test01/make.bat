@echo off
setlocal enableextensions
setlocal enabledelayedexpansion

::set CFLAGS=/Od /D_DEBUG
::set CFLAGS=/Od /Zi /D_DEBUG
set CFLAGS=/O2 /fp:fast

set CC=..\compiler\cl.exe
set HLSLC=..\compiler\dxc.exe /Ges /O3 /WX /nologo

set NAME=test01
set SHADER_BEGIN=0
set SHADER_END=0
 
for /L %%G in (%SHADER_BEGIN%,1,%SHADER_END%) do (
    if exist data\%%G.vs.cso del data\%%G.vs.cso
    if exist data\%%G.ps.cso del data\%%G.ps.cso
    %HLSLC% /D VS_%%G /E VertexMain /Fo data\%%G.vs.cso /T vs_6_0 %NAME%.hlsl & if !ERRORLEVEL! neq 0 (goto :end)
    %HLSLC% /D PS_%%G /E PixelMain /Fo data\%%G.ps.cso /T ps_6_0 %NAME%.hlsl & if !ERRORLEVEL! neq 0 (goto :end)
)

if exist %NAME%.exe del %NAME%.exe

%CC% %CFLAGS% /Gm- /nologo /GS- /Gs999999 /Gy /Gw /EHa- /Bt /WX /Wall /wd4710 /wd4711 /wd5045 /wd4820 /wd4201 /wd4221 /wd4152 /wd4204 %NAME%.c ^
     /link ..\compiler\kernel32.lib /OPT:REF /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /ENTRY:Start /NODEFAULTLIB

if exist *.obj del *.obj

if "%1" == "run" if exist %NAME%.exe (.\%NAME%.exe)

:end
