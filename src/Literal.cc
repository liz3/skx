//
// Created by liz3 on 08/07/2020.
//

#include "../include/Literal.h"

std::string *skx::Literal::extractQuoteValue(std::string base, size_t start) {
    size_t n = 0;
    while (true) {
        n = base.find_first_of('"', start + n);
        if(base[n -1] != '\\') break;
    }
    return new std::string (base.substr(start, n));
}

skx::OperatorPart *skx::Literal::extractNumber(std::string &base) {
    bool isDouble = base.find('.') != std::string::npos;
    if(isDouble){
        auto *f = new double (std::stod(base));
       return new OperatorPart(LITERAL, NUMBER, f, true);
    } else {
        auto *f = new int32_t (std::stoi(base));
       return new OperatorPart(LITERAL, NUMBER, f, false);

    }
}
