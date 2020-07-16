cmake_minimum_required(VERSION 3.16)
project(skx)

option(SKX_BUILD_API "Build Java" ON)
option(SKX_BUILD_EXAMPLE "Build Example Executable" ON)

if (SKX_BUILD_API)
    add_definitions(-DSKX_BUILD_API)
endif ()

set(CMAKE_CXX_STANDARD 14)

set(SKX_SOURCES
        src/Script.cc
        src/Context.cc
        src/Variable.cc
        src/Step.cc
        src/PreParser.cc
        src/utils.cc
        src/Function.cc
        src/Instruction.cc
        src/TreeCompiler.cc
        src/Executor.cc
        src/BasicInstructions.cc
        src/RegexUtils.cc
        src/Literal.cc
        src/types/TString.cc
        src/types/TBoolean.cc
        src/types/TNumber.cc
        src/types/TCharacter.cc
        src/types/TArray.cc
        src/api/Http.cpp
        src/api/Json.cpp
        src/Assembler.cc
        src/Disassembler.cc)
if (SKX_BUILD_API)
    include_directories(plugin/build)
    list(APPEND SKX_SOURCES
            src/AsyncExecutor.cc
            src/ApiBridge.cc
            src/api/McEvents.cc
            src/api/McEventValues.cc
            src/api/PlayerEffects.cc
            )
endif ()

add_library(skx SHARED
        ${SKX_SOURCES})
include_directories(third-party/json/single_include)
if (SKX_BUILD_API)
   if(APPLE)
       include_directories(/Library/Java/JavaVirtualMachines/jdk-12.0.2.jdk/Contents/Home/include /Library/Java/JavaVirtualMachines/jdk-12.0.2.jdk/Contents/Home/include/darwin)
   elseif(UNIX)
    include_directories(/usr/lib/jvm/java-11-openjdk/include /usr/lib/jvm/java-11-openjdk/include/linux)
   elseif(WIN32)

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
