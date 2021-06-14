#!/bin/sh
git submodule update --init
cd plugin
./gradlew shadowJar
cd ..
mkdir Debug
cd Debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
