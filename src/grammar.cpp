#include "grammar.hpp"

/**
 * Whether current char is number
 * @param ch - current char
 * @return - result
 */
bool Grammar::isNumber(char ch) {
    return (ch >= '0' && ch <= '9');
}

/**
 * Whether current char is float
 * @param ch - current char
 * @return - result
 */
bool Grammar::isFloat(char ch) {
    return isNumber(ch) || ch == '.';
}

/**
 * Whether current char is hexadecimal
 * @param ch - current char
 * @return - result
 */
bool Grammar::isHex(char ch) {
    return isNumber(ch)
           || (ch >= 'A' && ch <= 'F')
           || (ch >= 'a' && ch <= 'f');
}

/**
 * Whether current char is octal
 * @param ch - current char
 * @return - result
 */
bool Grammar::isOct(char ch) {
    return (ch >= '0' && ch <= '7');
}

/**
 * Whether current char is start of identifier
 * @param ch - current char
 * @return - result
 */
bool Grammar::isIdentifierStart(char ch) {
    return (ch == '_')
           || (ch >= 'a' && ch <= 'z')
           || (ch >= 'A' && ch <= 'Z');
}

/**
 * Whether current char is body of identifier
 * @param ch - current char
 * @return - result
 */
bool Grammar::isIdentifierBody(char ch) {
    return isIdentifierStart(ch) || isNumber(ch);
}

/**
 * Whether str is identifier
 * @param str - current string
 * @return - result
 */
bool Grammar::isIdentifier(basic_string<char> str) {
    if (!isIdentifierStart(str[0])) {
        return false;
    }
    for (char ch: str.substr(1)) {
        if (!isIdentifierBody(ch)) {
            return false;
        }
    }
    return true;
}

/**
 * Whether current char is space
 * @param ch - current char
 * @return - result
 */
bool Grammar::isSpace(char ch) {
    return ch == ' '
           || ch == '\f'
           || ch == '\n'
           || ch == '\r'
           || ch == '\t'
           || ch == '\v';
}

/**
 * Whether current char is illegal
 * @param ch - current char
 * @return - result
 */
bool Grammar::isIllegal(char ch) {
    if (isSpace(ch) || ch == 0) {
        return false;
    }
    return ch <= 31 || ch == 36 || ch == 64 || ch == 92 || ch == 96 || ch >= 127;
}
