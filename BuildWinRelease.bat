del CMakeCache.txt
del Build\main.exe
del Build\tests.exe
cmake -G "MinGW Makefiles" -DDEBUG_BUILD=OFF
mingw32-make -j8