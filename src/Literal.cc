//
// Created by liz3 on 08/07/2020.
//

#include "../include/Literal.h"
#include "../include/types/TNumber.h"

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
  if(isDouble) {
    return new OperatorPart(LITERAL, NUMBER, new TNumber(std::stod(base)), true);
  } else {
    return new OperatorPart(LITERAL, NUMBER, new TNumber(std::stoi(base)), false);

  }
}
