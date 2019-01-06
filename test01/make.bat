@echo off

set CFLAGS=/Od /D_DEBUG
::set CFLAGS=/O2 /GL /Oi /fp:fast

set CC=..\compiler\cl.exe
set HLSL=..\compiler\fxc.exe /Ges /O3 /WX /nologo /Qstrip_reflect /Qstrip_debug /Qstrip_priv
set ASM=..\compiler\fasm.exe
set APPNAME=test01

if exist %APPNAME%.exe del %APPNAME%.exe

::%HLSL% /D VS_E01_TRANSFORM /E vs_e01_transform /Fo vs_e01_transform.cso /T vs_5_1 experiment01.hlsl & if errorlevel 1 goto :end
::%HLSL% /D PS_E01_SHADE /E ps_e01_shade /Fo ps_e01_shade.cso /T ps_5_1 experiment01.hlsl & if errorlevel 1 goto :end

::%ASM% asmlib.asm & if errorlevel 1 goto :end

%CC% %CFLAGS% /Gm- /nologo /GS- /Gs999999 /Gy /Gw /EHa- ^
    /WX /Wall /Bt /wd4710 /wd4711 /wd5045 /wd4820 /wd4201 /wd4221 /wd4152 /wd4204 ^
    %APPNAME%.c ^
    /link ..\compiler\kernel32.lib ^
    /OPT:REF /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /ENTRY:Start /NODEFAULTLIB /OUT:%APPNAME%.exe

:end
if exist *.obj del *.obj
if exist *.cso del *.cso

if "%1" == "run" if exist %APPNAME%.exe (.\%APPNAME%.exe)
::if "%1" == "debug" if exist %APPNAME%.exe (WinDbgX /y .\ /srcpath .\ .\%APPNAME%.exe)
if "%1" == "debug" if exist %APPNAME%.exe (WinDbgX .\%APPNAME%.exe)
