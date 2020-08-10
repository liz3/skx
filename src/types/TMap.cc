//
// Created by Yann Holme Nielsen on 11/08/2020.
//

#include "../../include/types/TMap.h"

#include <utility>

std::string skx::TMap::getStringValue() {
    return "";
}

skx::TMap::TMap() {
    type = MAP;
}

skx::TMap::TMap(std::vector<MapEntry>) : value(std::move(value)) {
    type = MAP;
}

skx::TMap::~TMap() {

}

skx::VariableValue *skx::TMap::copyValue() {
    return new TMap(value);
}
