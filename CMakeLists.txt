cmake_minimum_required(VERSION 3.16)
project(skx)

set(CMAKE_CXX_STANDARD 14)

add_library(skx SHARED src/Script.cc include/Script.h src/Context.cc include/Context.h src/Variable.cc include/Variable.h src/Step.cc include/Step.h src/PreParser.cc include/PreParser.h src/utils.cc include/utils.h include/BaseTypeParser.h src/Function.cc include/Function.h src/Instruction.cc include/Instruction.h src/Array.cc include/Array.h src/TreeCompiler.cc include/TreeCompiler.h src/Executor.cc include/Executor.h include/stdlib.h src/StdLib.cpp)

add_executable(skx-example tests/main.cc)

target_link_libraries(skx-example skx)