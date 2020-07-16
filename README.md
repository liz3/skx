# Skx
This is a prototype and not functional!

## Whats Skx
Skx is a compiler & runtime for the [Skript lang](https://github.com/SkriptLang/Skript), written in c++ 14.  
Im currently starting a bridging plugin to work within a mc server, see the plugin folder.

## Concepts
Skx isnt finished design wise, as with many things i do it started as a total side thing.  
But here are the base concepts as list.
1. Syntax tree should be created once and NOT be edited at runtime, ever.
2. Runtime should only change values and NEVER create vars. Values yes. var containers should be known at compile time.
3. Variables and state are hold in Context`s, the instructions themselfs are in CompileItems

Skx does not produce a byte code. it compiles the skript into a set of in memory types, name: assigment, comparison & execution.  
Execution is probably was ressembles a normal skript effect the most.  
Comparisons & assigments are more restricted. by that i mean that anything implementing the execute function from Execution can basically do whatever they want to the Context. Assigment and comparisons are way more controlled. Though that might change to allow inlining of executions into assignments & comparisons.  
Currently those can only operate on variables OR literals.

 ## Current issues i know of/things which need to be done for a MVP
 * i dont think `else if` works.
 * Effects. theres the print effect and the JSON api. but thats it
 * A couple of events are working without conditions(i discovered earlier that on click and a couple of others do have that). Commands are not implemented but im honestly not too worried about these.
 
 ## Building
 
You will need cmake and the JDK 11 with the headers.

### macOS & gnu/linux
1. In the CmakeLists replace line 53 with the appropiate paths so Cmkake can include the JDK headers.
2. run ```build.sh```

### windows
1. same as for macOS & gnu/linux.
2. either use the `git bash` to run the script or recreate what it does. for the `make` command use `cmake --build build`

## Running
### Running the example. 
The building generated a `skx-example` executable which can be used for simple testing.
### Running the plugin
in `plugin/build/libs` is the plugin.  
**Note**: Right now it doesnt do anything but since im working on it when you are reading this it might be working.  
ATM:  
1. Copy the plugin into a **test** server.
2. when starting the server pass a `SKXLIBPATH` ENV var with the value being the path to the libskx.(dylib/so/dll) from the build folder. i.e: `SKXLIBPATH=/home/liz3/CLionProjects/skx/build/libskx.so java -jar spigot-1.12.2.jar`

## License
This project does not have a actual license yet. but you can treat my software as Free software, free as in libre.