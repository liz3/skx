//
// Created by liz3 on 30/06/2020.
//

#ifndef SKX_EXECUTOR_H
#define SKX_EXECUTOR_H

#include "TreeCompiler.h"

namespace skx {
    class Executor {
    public:
        static void executeStandalone(CompileItem* item);

    private:
        CompileItem* root = nullptr;
        void walk(CompileItem* item);
        bool lastFailed = false;
        uint16_t lastFailLevel = 0;
    };
}

#endif //SKX_EXECUTOR_H
