@echo off

if "%1" == "fmt" (
  "build\clang-format.exe" -i *.c *.h tests\*.c ^
  -style="{ColumnLimit: 100, BasedOnStyle: LLVM}"
  goto :end
)

set CFLAGS=/Zi /Od /D_DEBUG
::set CFLAGS=/O2 /Oi /fp:fast

set CC=build\clang-cl.exe
set APPNAME=eneida

if exist %APPNAME%.exe del %APPNAME%.exe

%CC% -fuse-ld=lld -nobuiltininc %CFLAGS% /nologo /GS- /Gs16384 /X /Gy /Gw /GR- /EHa- /I. /W4 ^
  %APPNAME%.c libs.c ^
  /link build\kernel32.lib /opt:ref /incremental:no /subsystem:windows /entry:start /nodefaultlib ^
  /OUT:%APPNAME%.exe

:end

if "%1" == "run" (
  if exist %APPNAME%.exe (
    %APPNAME%.exe
  )
)
