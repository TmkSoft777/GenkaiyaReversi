@cls
arm-mingw32ce-g++  -std=gnu++2a -march=armv5tej -mcpu=arm926ej-s -static -s -lcommctrl -lcommdlg -o "%~dp0AppMain.exe" "%~dp0window.cpp" "%~dp0board.cpp"