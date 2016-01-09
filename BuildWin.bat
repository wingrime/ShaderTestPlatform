del CMakeCache.txt
del Build\main.exe
del Build\tests.exe
mingw32-make clean
cmake -G "MinGW Makefiles" 
mingw32-make