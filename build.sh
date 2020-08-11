#!/bin/sh
git submodule update --init
cd plugin
./gradlew shadowJar
cd ..
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make