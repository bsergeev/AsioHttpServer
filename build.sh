mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
cd ..

ls -Al build/server
