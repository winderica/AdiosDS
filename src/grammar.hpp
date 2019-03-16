#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "../lib/json.hpp"

using namespace std;
using json = nlohmann::json;

/**
 * Retrieve key from a map
 */
struct RetrieveKey {
    template<typename T>
    /**
     * reload "()" operator to return the first element of a key-value pair
     * @tparam T
     * @param pair
     * @return
     */
    typename T::first_type operator()(T pair) const {
        return pair.first;
    }
};

/**
 * Compare helper class
 */
struct Compare {
    /**
     * return whether the length of first string is greater than the second
     * @param first
     * @param second
     * @return
     */
    bool operator()(const string &first, const string &second) {
        return first.size() > second.size();
    }
};

/**
 * basic grammars
 */
struct Grammar {

    /**
     * precedence of operators
     */
    map<string, int> precedence = {
            {"=",   1},
            {"+=",  1},
            {"-=",  1},
            {"*=",  1},
            {"/=",  1},
            {"%=",  1},
            {">>=", 1},
            {"<<=", 1},
            {"&=",  1},
            {"^=",  1},
            {"|=",  1},
            {"?",   2},
            {":",   2},
            {"||",  3},
            {"&&",  4},
            {"|",   5},
            {"^",   6},
            {"&",   7},
            {"<",   8},
            {">",   8},
            {"<=",  8},
            {">=",  8},
            {"==",  8},
            {"!=",  8},
            {">>",  9},
            {"<<",  9},
            {"+",   10},
            {"-",   10},
            {"*",   11},
            {"/",   11},
            {"%",   11},
            {".",   13},
            {"->",  13}
    };

    /**
     * operators got by anonymous function
     */
    json operators = [this]() -> json {
        json operators;
        transform(precedence.begin(), precedence.end(), back_inserter(operators), RetrieveKey());
        Compare compare;
        sort(operators.begin(), operators.end(), compare);
        return operators;
    }();

    /**
     * conversion of escapes
     */
    map<char, char> escapes = {
            {'a',  '\a'},
            {'b',  '\b'},
            {'f',  '\f'},
            {'n',  '\n'},
            {'r',  '\r'},
            {'t',  '\t'},
            {'v',  '\v'},
            {'\\', '\\'},
            {'\'', '\''},
            {'"',  '"'},
            {'?',  '\?'}
    };

    /**
     * basic types
     */
    json typeNames = {
            "void",
            "char",
            "short",
            "int",
            "long",
            "float",
            "double"
    };

    /**
     * type modifiers
     */
    json typeModifiers = {
            "auto",
            "extern",
            "register",
            "static",
            "signed",
            "unsigned",
            "short",
            "long",
            "const",
            "struct",
            "enum"
    };

    /**
     * Whether current char is number
     * @param ch - current char
     * @return - result
     */
    bool isNumber(char ch);

    /**
     * Whether current char is float
     * @param ch - current char
     * @return - result
     */
    bool isFloat(char ch);

    /**
     * Whether current char is hexadecimal
     * @param ch - current char
     * @return - result
     */
    bool isHex(char ch);

    /**
     * Whether current char is octal
     * @param ch - current char
     * @return - result
     */
    bool isOct(char ch);

    /**
     * Whether current char is start of identifier
     * @param ch - current char
     * @return - result
     */
    bool isIdentifierStart(char ch);

    /**
     * Whether current char is body of identifier
     * @param ch - current char
     * @return - result
     */
    bool isIdentifierBody(char ch);

    /**
     * Whether str is identifier
     * @param str - current string
     * @return - result
     */
    bool isIdentifier(string str);

    /**
     * Whether current char is space
     * @param ch - current char
     * @return - result
     */
    bool isSpace(char ch);
};

#endif // GRAMMAR_H
