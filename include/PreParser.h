//
// Created by liz3 on 29/06/2020.
//

#ifndef SKX_PREPARSER_H
#define SKX_PREPARSER_H

#include <string>
#include <vector>

namespace skx {
    class PreParserItem {
    public:
        std::string itemRaw;
        PreParserItem* parent;
        uint32_t pos;
        uint16_t level;
        std::string actualContent;
        std::vector<PreParserItem*> children;
        bool isComment = false;
    };
    class PreParseResult {
    public:
        std::vector<PreParserItem*> rootItems;
    };
    struct PreParserState {
        uint32_t pos;
    };
    class PreParser {
    private:
        static void advance(PreParserItem* baseItem, PreParserState* state, std::vector<std::string> items);
        static uint16_t countTabs(std::string item);
    public:
        static PreParseResult* preParse(std::vector<std::string> base);
    };
}


#endif //SKX_PREPARSER_H
