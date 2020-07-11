#!/bin/sh
git submodule update --init
cd plugin
./gradlew shadowJar
cd ..
mkdir build
cd build
cmake ..
make