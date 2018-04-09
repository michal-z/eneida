# demo01

### Project goals:<br />
* Create interesting, real-time animations by writing simple, low-level code.
* Explore math, procedural generation techniques and audio programming.
* Eventually, create a demoscene production which could be submitted to demo party.

### Project rules:<br />
* The most important rule is: Write only the code that you need and make it as simple as possible.
* Code is fully standalone, it compiles by running <b>make.bat</b> from <b>src</b> folder on any Windows machine (no Visual Studio or any other software is needed).
* No external headers or libs are required (not even system or libc headers).
* Only external functions used are <b>LoadLibrary</b> and <b>GetProcAddress</b>. Everything else is loaded at runtime.
* All needed compilers are present in <b>compiler</b> folder.
* <b>C99</b>, <b>ASM</b> and <b>HLSL</b> languages are used.
* Header files cannot include other header files (except very simple types.h).
* <b>Vim</b> is used to write code, <b>WinDbg Preview</b> and <b>Windows PIX</b> to debug it.
* <b>Intel VTune</b> and <b>Intel Architecture Code Analyzer</b> programs are used to profile and optimize the code.
