//
// Created by liz3 on 12/07/2020.
//

#ifndef SKX_ASYNCEXECUTOR_H
#define SKX_ASYNCEXECUTOR_H

#include "Executor.h"
#include <vector>
#include <thread>
#include <functional>
#include <deque>

namespace skx {
using CallingTask = std::function<void()>;

    class AsyncTask {
    public:
       CallingTask task;
    };
    class AsyncExecutor {
    public:
        void interrupt();
        AsyncExecutor();
        void queueTask(AsyncTask* task);
        void queueFunc(const CallingTask& task);
        bool isRunning() const;

        virtual ~AsyncExecutor();

    private:
        bool stopped = false;
        bool running = false;
        std::deque<AsyncTask*> tasks;
        void runLoop();
        std::thread* thread = nullptr;
    };
}


#endif //SKX_ASYNCEXECUTOR_H
