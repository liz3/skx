//
// Created by liz3 on 29/06/2020.
//

#include "../include/PreParser.h"
#include "../include/utils.h"


skx::PreParseResult *skx::PreParser::preParse(std::vector<std::string> base) {
    PreParseResult* result = new PreParseResult();
    PreParserState* state = new PreParserState();
    state->pos = 0;
    while (true) {
        if(state->pos >= base.size() -1)
            break;
        std::string item = base[state->pos];
        PreParserItem* entry = new PreParserItem();
        entry->level = countTabs(item);
        entry->pos = state->pos + 1;
        entry->parent = nullptr;
        entry->isComment = skx::Utils::ltrim(item).rfind('#', 0) == 0;
        entry->actualContent = skx::Utils::trim(item);
        entry->itemRaw = std::move(item);
        if(state->pos < base.size() && countTabs(base[state->pos + 1]) > 0 && !entry->isComment) {
            advance(entry, state, base);
        }
        state->pos += 1;
        if(!entry->isComment && entry->actualContent.length() > 0)
           if(entry->level == 0) {
               result->rootItems.push_back(entry);
           } else {
               result->rootItems[result->rootItems.size() - 1]->children.push_back(entry);
           }
        else
            delete entry;
    }
    return result;
}

uint16_t skx::PreParser::countTabs(std::string item) {
    uint16_t count = 0;

        for (char i : item) {
            if(i != '\t') break;
            count++;
        }

    return count;
}

void skx::PreParser::advance(skx::PreParserItem *baseItem, skx::PreParserState *state, std::vector<std::string> items) {
    while (true) {
        if(state->pos >= items.size() - 1) return;
        if(countTabs(items[state->pos + 1]) <= baseItem->level) {
            uint16_t tabs = countTabs(items[state->pos + 1]);
            if(skx::Utils::trim(items[state->pos + 1]).length() > 0 && items[state->pos + 1].rfind('#', tabs) != 0) {
                return;
            }
        }
        state->pos += 1;
        std::string item = items[state->pos];
        PreParserItem* entry = new PreParserItem();
        entry->level = countTabs(item);
        entry->pos = state->pos + 1;
        entry->parent = baseItem;
        entry->isComment = skx::Utils::ltrim(item).rfind('#', 0) == 0;
        entry->actualContent = skx::Utils::trim(item);
        entry->itemRaw = std::move(item);

        if(state->pos < items.size() - 1) {
            uint16_t nextTabs = countTabs(items[state->pos + 1]);
            if(items[state->pos + 1].rfind('#', nextTabs) != 0) {
                if(nextTabs > baseItem->level + 1 && !entry->isComment) {
                    advance(entry, state, items);
                }
            }

        }
        if(!entry->isComment && entry->actualContent.length() > 0)
             baseItem->children.push_back(entry);
        else
            delete entry;

    }
}
