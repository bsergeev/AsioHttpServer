#!/bin/bash

BUILD_DIRECTORY=build
BUILD_CONFIGURATION=Debug
if [ "$1" = "Release" ]; then
  BUILD_CONFIGURATION="$1"
fi
if [ ! -d "$BUILD_DIRECTORY" ]; then
  mkdir build
fi

cd $BUILD_DIRECTORY
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="$BUILD_CONFIGURATION" ..
make
cd ..

$BUILD_DIRECTORY/tests
