#include "../include/ParserState.h"

#include <iostream>

#include "../include/Function.h"
#include "../include/RegexUtils.h"
#include "../include/types/TBoolean.h"
#include "../include/types/TNumber.h"
#include "../include/types/TString.h"
#include "../include/api/Json.h"
#include "../include/api/NativeCall.h"
#include "../include/utils.h"

namespace skx {
ParserState::ParserState(std::string content) : content(content), pos(0) {
    setup();
}
uint16_t ParserState::nextWhiteSpace() {
    uint16_t remaining = content.length() - pos;
    for (uint16_t i = 0; i < remaining; i++) {
        if (isWhiteSpace(content[pos + i])) return i;
    }
    return remaining;
}
std::string ParserState::consume(uint16_t amount) {
    uint16_t remaining = content.length() - pos;
    uint16_t end = amount > remaining ? remaining : amount;
    if (end == remaining && content[content.length() - 1] == ':') {
        if (type == IF_STATEMENT || type == ELSE_IF_STATEMENT ||
            type == LOOP_STMT)
            end -= 1;
    }
    std::string str = content.substr(pos, end);
    pos += end;
    return str;
}
void ParserState::skip(uint16_t amount) {
    uint16_t remaining = content.length() - pos;
    pos += amount > remaining ? remaining : amount;
}
bool ParserState::isWhiteSpace(char search) {
    return search == ' ' || search == '\n' || search == '\t';
}

uint16_t ParserState::nextNonWhiteSpace() {
    uint16_t remaining = content.length() - pos;
    for (uint16_t i = 0; i < remaining; i++) {
        if (!isWhiteSpace(content[pos + i])) return i;
    }
    return remaining;
}
FunctionInvoker* ParserState::getNextAsFunctionInvoke(Context* ctx) {
    skip(nextNonWhiteSpace());
    std::string name = "";
    name += content[pos];
    while (true) {
        char n = next();
        if (n == ' ' || n == '(') break;
        name += n;
    }
    auto* f = ctx->global->functions[name];
    if (!f) return nullptr;
    FunctionInvoker* invoker = new FunctionInvoker();
    invoker->function = f;
    skip(1);
    skip(nextNonWhiteSpace());
    if (content[pos] == ')') return invoker;
    while (true) {
        invoker->dependencies.push_back(getNextAsValue(ctx));
        skip(1);
        skip(nextNonWhiteSpace());
        if (content[pos] != ',') break;
        skip(1);
    }
    return invoker;
}
bool ParserState::nextIs(ParserContextState what) {
    uint16_t current = pos;
    bool res = false;
    skip(nextNonWhiteSpace());
    switch (what) {
        case ENTRY:
            res = pos == 0;
            break;
        case DEFINITION:
            res = content[pos] == '{';
            break;
        case STRING_LITERAL:
            res = content[pos] == '"';
            break;
        case NUMBER_LITERAL:
            res = isNumber(content[pos]);
            break;
        case BOOLEAN_LITERAL: {
            std::string next = consume(nextWhiteSpace());
            res = next == "true" || next == "false";
            break;
        }
        case OPERATOR: {
            std::string next = consume(nextWhiteSpace());
            res = isOperator(next);
            break;
        }
        case FUNCTION_CALL: {
            auto funcCallMatches = skx::RegexUtils::getMatches(
                skx::functionCallPattern, content.substr(pos));
            res = !funcCallMatches.empty() && funcCallMatches[0].pos == 0;
            break;
        }
        default:
            res = false;
            break;
    }

    pos = current;
    return res;
}
bool ParserState::isOperator(std::string in) {
    return in == "+" || in == "-" || in == "*" || in == "/" || in == "plus" ||
           in == "minus" || in == "multiply" || in == "divided" || in == "is";
}
OperatorPart* ParserState::getNextAsValue(Context* ctx) {
    skip(nextNonWhiteSpace());
    if (content[pos] != '{' && content.substr(pos).find("player") != 0) {
        auto* descriptor = getNextAsVariable();
        if (descriptor == nullptr) return nullptr;
        Variable* var = nullptr;
        if (descriptor->type == STATIC || descriptor->type == GLOBAL) {
            var = skx::Utils::searchRecursive(descriptor->name, ctx->global);
        } else {
            var = skx::Utils::searchRecursive(descriptor->name, ctx);
        }
        delete descriptor;
        if (var)
            return new OperatorPart(VARIABLE, var->type, var, var->isDouble);
        return nullptr;
    } else if (isNumber(content[pos])) {
        return getNextAsNumber();
    } else if (content[pos] == '"') {
        return new OperatorPart(LITERAL, STRING, new TString(getNextAsString()),
                                false);
    }
    auto funcCallMatches = skx::RegexUtils::getMatches(skx::functionCallPattern,
                                                       content.substr(pos));
    if (!(funcCallMatches).empty()) {
        auto entry = funcCallMatches[0];
        if (entry.pos == 0)
            return new OperatorPart(EXECUTION, UNDEFINED,
                                    getNextAsFunctionInvoke(ctx), false);
    }
    return getNextAsBoolean();
}
OperatorPart* ParserState::getNextAsNumber() {
    skip(nextNonWhiteSpace());
    if (!isNumber(content[pos])) return nullptr;
    return Literal::extractNumber(consume(nextWhiteSpace()));
}
OperatorPart* ParserState::getNextAsBoolean() {
    skip(nextNonWhiteSpace());
    std::string next = consume(nextWhiteSpace());
    if (next != "true" && next != "false") return nullptr;
    return new OperatorPart(LITERAL, BOOLEAN, new TBoolean(next == "true"),
                            false);
}
OperatorResult ParserState::getNextAsOperator() {
    OperatorResult result;
    result.success = true;
    skip(nextNonWhiteSpace());
    std::string start = consume(nextWhiteSpace());
    if (start != "is") {
        if (start == "+" || start == "plus")
            result.op = ADD;
        else if (start == "-" || start == "minus")
            result.op = SUBTRACT;
        else if (start == "*" || start == "multiply")
            result.op = MULTIPLY;
        else if (start == "/" || start == "divide")
            result.op = DIVIDE;
        else
            result.success = false;
        return result;
    }
    result.op = EQUAL;
    uint16_t saved = pos;
    skip(1);
    std::string entry = consume(nextWhiteSpace());

    if (entry == "greater" || entry == "bigger") {
        result.op = BIGGER;
    } else if (entry == "smaller") {
        result.op = SMALLER;
    } else if (entry == "not") {
        result.inverted = true;
        saved = pos;
        skip(1);
        std::string next = consume(nextWhiteSpace());
        if (next == "greater" || next == "bigger") {
            result.op = BIGGER;
        } else if (next == "smaller") {
            result.op = SMALLER;
        } else {
            pos = saved;
        }
    } else {
        pos = saved;
    }
    if (result.op == BIGGER || result.op == SMALLER) {
        skip(1);
        std::string next = consume(nextWhiteSpace());

        if (next == "or") {
            skip(1);
            next = consume(nextWhiteSpace());
            if (next == "equal") {
                skip(1);
                next = consume(nextWhiteSpace());
                if (next == "to" || next == "then") {
                    if (result.op == BIGGER) result.op = BIGGER_OR_EQUAL;
                    if (result.op == SMALLER) result.op = SMALLER_OR_EQUAL;
                }
            } else {
                pos = saved;
            }
        } else if (next != "then" && next != "to") {
            pos = saved;
        }
    }
    return result;
}
VariableDescriptor* ParserState::getNextAsVariable() {
    skip(nextNonWhiteSpace());
    if (content[pos] != '{' && content.substr(pos).find("player") != 0) {
        return nullptr;
    }
    size_t level = 0;
    std::string name;
    name += content[pos];
    while (true) {
        char n = next();
        name += n;
        if (n == '}') {
            if (level == 0)
                break;
            else
                level--;
        } else if (n == '{') {
            level++;
        }
    }
    return Variable::extractNameSafe(name, false);
}
std::string ParserState::getNextAsString() {
    skip(nextNonWhiteSpace());
    if (content[pos] != '"') return "";

    // skip the initial "
    std::string str = "";

    while (true) {
        char n = next();
        //  std::cout << "next: " << n << "\n";
        if (n == '"' && (str.length() == 0 || str[str.length() - 1] != '\\')) {
            break;
        } else if (str.length() > 0 && str[str.length() - 1] == '\\') {
            switch (n) {
                case '"': {
                    str[str.length() - 1] = '"';
                    break;
                }
                case '\\': {
                    break;
                }
                case 't': {
                    str[str.length() - 1] = '\t';
                    break;
                }
                case 'n': {
                    str[str.length() - 1] = '\r';
                    break;
                }
                case 'r': {
                    str[str.length() - 1] = '\r';
                    break;
                }
                case 'b': {
                    str[str.length() - 1] = '\b';
                    break;
                }
                default:
                    str += n;
                    break;
            }
        } else {
            str += n;
        }
    }
    return str;
}
bool ParserState::isNumber(char c) { return c >= '0' && c <= '9'; }
char ParserState::next() {
    if (pos + 1 > content.length()) return '\0';
    return content[++pos];
}
void ParserState::setup() {
    std::string entry = consume(nextWhiteSpace());
    if (entry == "if") {
        type = IF_STATEMENT;
    } else if (entry == "else") {
        skip(1);
        std::string nextWord = consume(nextWhiteSpace());
        if (nextWord == "if") {
            type = ELSE_IF_STATEMENT;
        } else {
            type = OTHER;
        }
    } else if (entry == "set") {
        type = ASSIGMENT;
    } else if (entry == "return") {
        type = RETURN_STMT;
    } else if (entry == "loop") {
        type = LOOP_STMT;
    } else if (entry == "add" || entry == "subtract" || entry == "divide" ||
               entry == "multiply") {
        type = MATH_OPERATION;
        mathOperation = entry;
    } else {
        type = OTHER;
    }
}
}  // namespace skx
