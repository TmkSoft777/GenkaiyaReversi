#!/bin/bash
arm-mingw32ce-windres resource.rc -o resource.o
arm-mingw32ce-g++  -std=gnu++2a -march=armv5tej -mcpu=arm926ej-s -static -s -o AppMain.exe window.cpp board.cpp resource.o -lcommctrl
rm resource.o