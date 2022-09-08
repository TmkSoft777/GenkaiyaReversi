@cls
@SET DIR=%~dp0
arm-mingw32ce-g++  -std=gnu++2a -march=armv5tej -mcpu=arm926ej-s -static -s -o "%DIR%AppMain.exe" "%DIR%window.cpp" "%DIR%board.cpp" "%DIR%win.rc.o" -L "%DIR:~0,-1%" -lcommctrl