@echo off
git submodule update --init
mkdir Release
cd Release
cmake ..
cmake --build . --config Release
