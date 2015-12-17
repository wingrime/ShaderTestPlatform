del CMakeCache.txt
del Build\main.exe
del Build\tests.exe
cmake -G "MinGW Makefiles" 
mingw32-make -j8