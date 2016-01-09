del CMakeCache.txt
del Build\main.exe
del Build\tests.exe
cmake clean
cmake -G "MinGW Makefiles" 
mingw32-make