#!/bin/bash

BUILD_DIRECTORY=build

if [ ! -d "$BUILD_DIRECTORY" ]; then
  mkdir build
fi

cd $BUILD_DIRECTORY
cmake -G "Unix Makefiles" ..
make
cd ..

$BUILD_DIRECTORY/tests
