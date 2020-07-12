//
// Created by liz3 on 12/07/2020.
//

#include "../include/AsyncExecutor.h"
#include <iostream>

void skx::AsyncExecutor::interrupt() {
    if (stopped || thread == nullptr) return;
    stopped = true;
    thread->join();
}

void skx::AsyncExecutor::runLoop() {
    while (!tasks.empty()) {
        auto *entry = tasks[0];
        entry->task();
        tasks.pop_front();
        delete entry;
    }

    running = false;
}

skx::AsyncExecutor::AsyncExecutor() {}

void skx::AsyncExecutor::queueTask(skx::AsyncTask *task) {
    if(stopped) return;
    tasks.push_back(task);
    if(!running) {
        if(thread != nullptr) {
            thread->join();
            delete thread;
            thread = nullptr;
        }
        running = true;
        thread = new std::thread([this]() {
           runLoop();
        });

    }
}

void skx::AsyncExecutor::queueFunc(const skx::CallingTask &task) {
    AsyncTask *job = new AsyncTask();
    job->task = task;
    queueTask(job);
}

bool skx::AsyncExecutor::isRunning() const {
    return stopped;
}

skx::AsyncExecutor::~AsyncExecutor() {
    delete thread;
}
