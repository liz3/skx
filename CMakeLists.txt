cmake_minimum_required(VERSION 3.16)
project(skx)

option(SKX_BUILD_API "Build Java" ON)
option(SKX_BUILD_EXAMPLE "Build Example Executable" ON)

if (SKX_BUILD_API)
    add_definitions(-DSKX_BUILD_API)
endif ()

set(CMAKE_CXX_STANDARD 14)

set(SKX_SOURCES src/Script.cc
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
        src/types/TString.cc
        include/types/TString.h
        src/types/TBoolean.cc
        include/types/TBoolean.h
        src/types/TNumber.cc
        include/types/TNumber.h
        src/types/TCharacter.cc
        include/types/TCharacter.h
        src/types/TArray.cc
        include/types/TArray.h
        src/api/Http.cpp
        include/api/Http.h
        src/api/Json.cpp
        include/api/Json.h
        src/AsyncExecutor.cc
        include/AsyncExecutor.h
        src/ApiBridge.cc
        include/ApiBridge.h )
if (SKX_BUILD_API)
    list(APPEND ${SKX_SOURCES} plugin/build/net_liz3_skx_SkxApi.h)
endif ()

add_library(skx SHARED
        ${SKX_SOURCES})

if (SKX_BUILD_API)
    include_directories(/usr/lib/jvm/java-11-openjdk/include /usr/lib/jvm/java-11-openjdk/include/linux)
endif ()

if (NOT WIN32)
    #target_link_libraries(skx curl)
    target_link_libraries(skx pthread)
endif ()
if (SKX_BUILD_EXAMPLE)
    add_executable(skx-example tests/main.cc)
    configure_file(assets/Vanish.sk . COPYONLY)
    target_link_libraries(skx-example skx)
endif ()