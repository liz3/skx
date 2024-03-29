cmake_minimum_required(VERSION 3.16)
project(skx)

option(SKX_BUILD_API "Build Java" OFF)
option(SKX_BUILD_EXAMPLE "Build Example Executable" ON)

if (SKX_BUILD_API)
    add_definitions(-DSKX_BUILD_API)
endif ()

set(CMAKE_CXX_STANDARD 14)

set(SKX_SOURCES
        src/Script.cc
        src/Context.cc
        src/Variable.cc
        src/PreParser.cc
        src/utils.cc
        src/Function.cc
        src/Instruction.cc
        src/TreeCompiler.cc
        src/Executor.cc
        src/BasicInstructions.cc
        src/StringTemplateCompiler.cc
        src/RegexUtils.cc
        src/Literal.cc
        src/types/TString.cc
        src/types/TBoolean.cc
        src/types/TNumber.cc
        src/types/TCharacter.cc
        src/types/TArray.cc
        src/api/Http.cpp
        src/api/Json.cpp
        src/api/NativeCall.cc
        src/Assembler.cc
        src/Disassembler.cc
        src/types/TMap.cc
        include/types/TMap.h
        src/api/MapEffects.cc
        include/api/MapEffects.h)
if (SKX_BUILD_API)
    include_directories(plugin/build)
    list(APPEND SKX_SOURCES
            src/AsyncExecutor.cc
            src/ApiBridge.cc
            src/types/TPlayerRef.cc
            src/api/McEventsBaseEffects.cc
            src/api/RuntimeMcEventValues.cc
            src/api/McEventPlayerEffects.cc
            src/api/EventPreconditions.cc
            src/api-compiler/EventValuesCompiler.cc
            src/api-compiler/EventBaseEffectCompiler.cc
            )
endif ()

add_library(skx SHARED
        ${SKX_SOURCES})
include_directories(third-party/json/single_include)
if (SKX_BUILD_API)
   if(APPLE)
       include_directories(/Library/Java/JavaVirtualMachines/jdk-11.0.9.jdk/Contents/Home/include /Library/Java/JavaVirtualMachines/jdk-11.0.9.jdk/Contents/Home/include/darwin)
   elseif(UNIX)
    include_directories(/usr/lib/jvm/java-11-openjdk/include /usr/lib/jvm/java-11-openjdk/include/linux)
   elseif(WIN32)
        include_directories("C:/Program Files/Java/jdk-11.0.8/include" "C:/Program Files/Java/jdk-11.0.8/include/win32")
    endif ()
endif()
if (NOT WIN32 AND NOT APPLE)
    #target_link_libraries(skx curl)
    target_link_libraries(skx pthread)
endif ()
if (SKX_BUILD_EXAMPLE)
    add_executable(skx-example tests/main.cc)
    configure_file(assets/Vanish.sk . COPYONLY)
    target_link_libraries(skx-example skx)
endif ()

if(NOT WIN32)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
