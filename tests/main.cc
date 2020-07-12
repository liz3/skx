//
// Created by liz3 on 29/06/2020.
//

#include <fstream>
#include <string>
#include <iostream>
#include <chrono>
#include "../include/Script.h"
#include "../include/Executor.h"
#include "../include/AsyncExecutor.h"



int main() {

    std::fstream stream("Vanish.sk");
    stream.seekg (0, stream.end);
    int length = stream.tellg();
    stream.seekg (0, stream.beg);

    // allocate memory:
    char * buffer = new char [length + 1];
    buffer[length ] = '\0';
    // read data as a block:
    stream.read (buffer,length);

    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    auto result = skx::Script::parse(buffer);
    delete result->preParseResult;
    for(const auto& entry : result->signals) {
        if(entry.first->signalType == skx::TriggerSignal::LOAD) {
            skx::Executor::executeStandalone(entry.second);
        }
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "everything took: " << t << " ms";

    //clean up
    delete[] buffer;
    delete result;
    return 0;
}