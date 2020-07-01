//
// Created by liz3 on 29/06/2020.
//

#include "../include/Context.h"

skx::Context::Context(uint16_t level, uint32_t steps, uint32_t stepPointer, skx::Context *parent) : level(level),
                                                                                                    steps(steps),
                                                                                                    stepPointer(
                                                                                                            stepPointer),
                                                                                                    parent(parent) {

}
