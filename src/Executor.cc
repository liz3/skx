//
// Created by liz3 on 30/06/2020.
//

#include "../include/Executor.h"
#include "../include/utils.h"


void skx::Executor::executeStandalone(skx::CompileItem *item) {
    auto *exec = new Executor();
    exec->isLoop = item->isLoop;
    skx::Utils::updateVarState(item->ctx, RUNTIME_CR);
    exec->root = item;
    for (int i = 0; i < item->children.size(); ++i) {
        auto current = item->children[i];
        exec->walk(current);
    }
    skx::Utils::updateVarState(item->ctx, SPOILED);
    delete exec;
}

void skx::Executor::walk(skx::CompileItem *item) {
    skx::Utils::updateVarState(item->ctx, RUNTIME_CR);
    if(item->isElse && lastFailed && lastFailLevel == item->level) {
        for (auto child : item->children) {
            walk(child);
        }
        lastFailed = false;
        return;
    }
    if(item->isBreak && isLoop) {
        if(!stopLoop)
            stopLoop = true;
        return;
    } else if(stopLoop) {
        return;
    }
    if(item->level > lastFailLevel && lastFailed) {
        lastFailed = false;
    }
    if (item->triggers.empty() && item->assignments.size() == 0 && item->comparisons.size() > 0) {
        for (int i = 0; i < item->comparisons.size(); ++i) {
            auto current = item->comparisons[i];
            bool failed = !current->execute(item->ctx);
            if (failed) {
                lastFailed = true;
                lastFailLevel = item->level;
                skx::Utils::updateVarState(item->ctx, SPOILED);
                return;
            }
        }
        for (auto child : item->children) {
            if(stopLoop) return;
            walk(child);
        }

        skx::Utils::updateVarState(item->ctx, SPOILED);
        return;
    }
    if (item->triggers.empty() && item->assignments.size() > 0 && item->comparisons.size() == 0) {
        for (int i = 0; i < item->assignments.size(); ++i) {
            auto current = item->assignments[i];
            bool failed = !current->execute(item->ctx);
            if (failed) {
                skx::Utils::updateVarState(item->ctx, SPOILED);
                return;
            }
        }
        //assignments should not have children
    }
    if(!item->executions.empty()) {
        for (int i = 0; i < item->executions.size(); ++i) {
            auto current = item->executions[i];
           current->execute(item->ctx);
        }
    }
    skx::Utils::updateVarState(item->ctx, SPOILED);

}

void skx::Executor::execute(skx::CompileItem *item) {
    isLoop = item->isLoop;
    skx::Utils::updateVarState(item->ctx, RUNTIME_CR);
    root = item;
    for (int i = 0; i < item->children.size(); ++i) {
        auto current = item->children[i];
        if(stopLoop) break;
        walk(current);
    }
    skx::Utils::updateVarState(item->ctx, SPOILED);
}
