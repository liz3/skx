//
// Created by liz3 on 29/06/2020.
//

#include "../include/Context.h"
#include "../include/Function.h"


skx::Context::Context(uint16_t level, uint32_t steps, uint32_t stepPointer, skx::Context *parent) : level(level),
  steps(steps),
  stepPointer(
    stepPointer),
  parent(parent) {
  if(parent) {
    parent->childContexts.push_back(this);
  }

}

skx::Context::~Context() {
  for (auto & childContext : childContexts) {
    delete childContext;
  }
  childContexts.clear();
  if(parent == nullptr) {
    for (auto& func : functions) {
      delete func.second;
    }
    functions.clear();
  }
  for (auto& var : vars) {
    delete var.second;
  }
  vars.clear();
}
