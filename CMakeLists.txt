cmake_minimum_required(VERSION 3.16)
project(skx)

set(CMAKE_CXX_STANDARD 14)

add_library(skx SHARED
        src/Script.cc
        include/Script.h
        src/Context.cc
        include/Context.h
        src/Variable.cc
        include/Variable.h
        src/Step.cc
        include/Step.h
        src/PreParser.cc
        include/PreParser.h
        src/utils.cc
        include/utils.h
        include/BaseTypeParser.h
        src/Function.cc
        include/Function.h
        src/Instruction.cc
        include/Instruction.h
        src/TreeCompiler.cc
        include/TreeCompiler.h
        src/Executor.cc
        include/Executor.h
        include/BasicInstructions.h
        src/BasicInstructions.cpp
        src/RegexUtils.cc
        include/RegexUtils.h
        src/Literal.cc
        include/Literal.h
        src/types/TString.cc include/types/TString.h src/types/TBoolean.cc include/types/TBoolean.h src/types/TNumber.cc include/types/TNumber.h src/types/TCharacter.cc include/types/TCharacter.h src/types/TArray.cc include/types/TArray.h)

add_executable(skx-example tests/main.cc)
configure_file(assets/Vanish.sk . COPYONLY)
target_link_libraries(skx-example skx)