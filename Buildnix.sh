rm CMakeCache.txt
rm Build\main.exe
rm Build\tests.exe
cmake -G "Unix Makefiles"
make
