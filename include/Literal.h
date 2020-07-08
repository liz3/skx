//
// Created by liz3 on 08/07/2020.
//

#ifndef SKX_LITERAL_H
#define SKX_LITERAL_H

#include <string>

namespace skx {
    class Literal {
        static std::string* extractQuoteValue(std::string base, size_t start);
    };
}


#endif //SKX_LITERAL_H
