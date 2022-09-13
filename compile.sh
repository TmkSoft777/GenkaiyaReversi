#!/bin/bash
arm-mingw32ce-windres reversi.rc -o reversi.o
arm-mingw32ce-g++  -std=gnu++2a -march=armv5tej -mcpu=arm926ej-s -static -s -o AppMain.exe window.cpp board.cpp win.rc.o -lcommctrl