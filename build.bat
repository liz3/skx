@echo off
git submodule update --init
cd plugin
call gradlew.bat shadowJar
cd ..
mkdir Release
cd Release
cmake ..
cmake --build . --config Release