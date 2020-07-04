//
// Created by liz3 on 03/07/2020.
//

#include "../include/RegexUtils.h"

std::vector<skx::RegexResult> skx::RegexUtils::getMatches(const std::regex &regex, const std::string &content) {
    std::vector<RegexResult> matches;

    for (std::sregex_iterator i = std::sregex_iterator(content.begin(), content.end(), regex);
         i != std::sregex_iterator();
         ++i) {
        std::smatch m = *i;
        RegexResult entry{
            .pos = static_cast<uint32_t>(m.position()),
            .content = m.str()
        };
        matches.push_back(entry);
    }
    return matches;
}
