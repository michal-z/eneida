@echo off
set DXC=build\dxc.exe /Ges /O3 /WX /nologo

if exist *.cso del *.cso
%DXC% /E main_vs /Fo data\shaders\test.vs.cso /T vs_6_0 shaders\test.hlsl & if errorlevel 1 goto :end
%DXC% /E main_ps /Fo data\shaders\test.ps.cso /T ps_6_0 shaders\test.hlsl & if errorlevel 1 goto :end

:end
