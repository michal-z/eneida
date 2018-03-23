# fractals

### Project goals:<br />
* Create beautiful fractal images from low-level code.

### Project rules:<br />
* The most important rule is: Write only the code that you need and make it as simple as possible.
* Code is fully standalone, it compiles by running <b>make.bat</b> on any Windows machine (no Visual Studio or any other software is needed).
* No external headers or libs are required (not even system or libc headers).
* Only external functions used are <b>LoadLibrary</b> and <b>GetProcAddress</b>. Every other syscall is loaded at runtime.
* All needed compilers are present in <b>compiler</b> folder.
* <b>Vim</b> is used to write code, <b>WinDbg Preview</b> to debug it.
* <b>Intel VTune</b> and <b>Intel Architecture Code Analyzer</b> programs are used to profile and optimize the code.
