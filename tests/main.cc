//
// Created by liz3 on 29/06/2020.
//

#include <fstream>
#include <string>
#include <iostream>
#include <chrono>
#include "../include/Script.h"
#include "../include/Executor.h"



int main() {

    std::fstream stream("/home/liz3/Downloads/Vanish.sk");
    stream.seekg (0, stream.end);
    int length = stream.tellg();
    stream.seekg (0, stream.beg);

    // allocate memory:
    char * buffer = new char [length + 1];
    buffer[length +1] = '\0';
    // read data as a block:
    stream.read (buffer,length);
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();

    auto result = skx::Script::parse(buffer);
    skx::Executor::executeStandalone(result->compiledPreRuntimeEvents[0]);
//    auto var1 = result->compiledPreRuntimeEvents[0]->children[0]->children[0]->ctx->vars["test"];
//    auto var2 = result->compiledPreRuntimeEvents[0]->children[0]->children[0]->ctx->vars["test2"];
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    auto t = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    std::cout << "everything took: " << t << " microseonds";
    return 0;
}