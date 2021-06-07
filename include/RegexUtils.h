//
// Created by liz3 on 03/07/2020.
//

#ifndef SKX_REGEXUTILS_H
#define SKX_REGEXUTILS_H

#include <regex>

namespace skx {
const std::regex functionCallPattern("[^\\>\\[( {]+\\(.*\\)(?!.*\")");
struct RegexResult {
  uint32_t pos;
  std::string content;
};
class RegexUtils {
 public:
  static std::vector<RegexResult> getMatches(const std::regex& regex, const std::string& content);
};

}

#endif //SKX_REGEXUTILS_H
