#!/bin/sh
git submodules update --init
cd plugin
./gradlew shadowJar
cd ..
mkdir build
cd build
cmake ..
make