//
// Created by liz3 on 08/07/2020.
//

#ifndef SKX_LITERAL_H
#define SKX_LITERAL_H

#include "TreeCompiler.h"
#include <string>

namespace skx {
class Literal {
 public:
  static std::string* extractQuoteValue(std::string base, size_t start);
  static OperatorPart* extractNumber(std::string& base);
};
}


#endif //SKX_LITERAL_H
