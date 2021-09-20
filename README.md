# Skx
This is a prototype and can easily break!

## Whats Skx
Skx is a compiler & runtime for the [Skript lang](https://github.com/SkriptLang/Skript), written in c++ 14.
Im currently starting a bridging plugin to work within a mc server, see the plugin folder.

## Concepts
Skx isnt finished design wise, as with many things i do it started as a total side thing.
But here are the base concepts as list.
1. Syntax tree should be created once and NOT be edited at runtime, ever.
2. Runtime should only change values and NEVER create vars. Values yes. var containers should be known at compile time.
3. Variables and state are hold in Context`s, the instructions themselfs are in CompileItems

Skx is a static tree compiler, so it reads the skript into AST and then executes that AST as a "runtime".
The variables and structures are allocated compile time and get their values mutated at runtime, some structures like json can change size runtime but thats because they are custom pointers.

## State
The runtime and parser are mostly functional, though its not anywhere the same design as the actual plugin.

## APIS
Variables and functions work very similar to the original skript plugin.
To work with the system theres a `nativecall` api which allows things like file access, stream writing, math actions and other things.
For now use `src/api/NativeCall.cc` file, it has all native calls.
An example is:
```
set {content} to nativecall readfile "some/foo/bar/file.txt"
```

## Current issues
The Tree compiler has some issues with the way that its not a token based parser, which did add some limitations.

## Building
The CMake file has two variables, one builds a executable which can take a file as a argument and runs it.
The second is to build the *Java plugin*, which is a simple way to use some basic events.

On Unix and unix likes(MacOS/GNU/Linux) you can simply run:
```
build.sh
```
On windows you can run:
```
build.bat
```

### Building the Plugin
```
cd plugin
gradlew shadowJar
```
to use the server with the plugin you need to rebuild the library and then set `SKXLIBPATH` env var to the path to the library file, when starting the server to load the native file.


## License
This project does not have a actual license yet. but you can treat my software as Free software, free as in libre.
