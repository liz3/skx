#!/bin/sh
git submodule update --init
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
