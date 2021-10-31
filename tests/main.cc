//
// Created by liz3 on 29/06/2020.
//

#include <fstream>
#include <string>
#include <iostream>
#include <chrono>
#include <vector>
#include "../include/Script.h"
#include "../include/Executor.h"
#include "../include/Assembler.h"
void printTest(std::string data, int stream) {
  std::cout << "[" << stream << "] >> " << data;
}

int main(int argc, char** argv) {
    std::vector<std::string> argsVec;
    if(argc > 2) {
      for(size_t i = 2; i < argc; i++) {
        argsVec.push_back(std::string(argv[i]));
      }

    }
    std::fstream stream(argv[1]);
    stream.seekg (0, stream.end);
    int length = stream.tellg();
    stream.seekg (0, stream.beg);

    // allocate memory:
    char * buffer = new char [length + 1];
    buffer[length ] = '\0';
    // read data as a block:
    stream.read (buffer,length);
    std::cout << ">> Parsing script into tree:\n";
    std::chrono::high_resolution_clock::time_point parseStart = std::chrono::high_resolution_clock::now();
    auto result = skx::Script::parse(buffer, argsVec.size(), argsVec);
    result->setPrintFunc(&printTest);
    std::chrono::high_resolution_clock::time_point parseEnd = std::chrono::high_resolution_clock::now();
    if(argc == 3) {
      if(std::string(argv[2]) == "assemble")
        std::cout << skx::Assembler::assemble(result) << "\n";
    }
    std::cout << ">> Parse time: " << (std::chrono::duration_cast<std::chrono::microseconds>(parseEnd - parseStart).count()) << " micro seconds\n";
    delete result->preParseResult;
    for(const auto& entry : result->signals) {
        if(entry.first->signalType == skx::TriggerSignal::LOAD) {
            std::chrono::high_resolution_clock::time_point onloadStart;
            std::chrono::high_resolution_clock::time_point onLoadEnd;
            std::cout << ">> Running onload:\n\n";
            onloadStart = std::chrono::high_resolution_clock::now();
            skx::Executor::executeStandalone(entry.second);
            onLoadEnd = std::chrono::high_resolution_clock::now();
            std::cout << "\n" << ">> Onload Execution time: " << (std::chrono::duration_cast<std::chrono::microseconds>(onLoadEnd - onloadStart).count()) << " micro seconds\n";
        }
    }
    for(const auto& entry : result->signals) {
        if(entry.first->signalType == skx::TriggerSignal::UN_LOAD) {
            std::chrono::high_resolution_clock::time_point unloadStart;
            std::chrono::high_resolution_clock::time_point unloadEnd;
            std::cout << ">> Running unload:\n\n";
            unloadStart = std::chrono::high_resolution_clock::now();
            skx::Executor::executeStandalone(entry.second);
            unloadEnd = std::chrono::high_resolution_clock::now();
            std::cout << "\n" << ">> Unload Execution time: " << (std::chrono::duration_cast<std::chrono::microseconds>(unloadEnd - unloadStart).count()) << " micro seconds\n";
        }
    }


    //clean up
    delete[] buffer;
    delete result;
    return 0;
}
