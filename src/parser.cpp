#include <map>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <memory>
#include "../lib/json.hpp"

using namespace std;
using json = nlohmann::json;

struct RetrieveKey {
    template<typename T>
    typename T::first_type operator()(T pair) const {
        return pair.first;
    }
};

struct Compare {
    bool operator()(const string &first, const string &second) {
        return first.size() > second.size();
    }
};

class Parser {
    string source;
    char curr;
    int index;
    int lineNumber;

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

    vector<string> operators = [this]() -> vector<string> {
        vector<string> operators;
        transform(precedence.begin(), precedence.end(), back_inserter(operators), RetrieveKey());
        Compare compare;
        sort(operators.begin(), operators.end(), compare);
        return operators;
    }();

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
    vector<string> typeNames = {
            "void",
            "char",
            "short",
            "int",
            "long",
            "float",
            "double"
    };
    vector<string> typeModifiers = {
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


    bool isNumber(char ch) {
        return (ch >= '0' && ch <= '9');
    }

    bool isFloat(char ch) {
        return isNumber(ch) || ch == '.';
    }

    bool isHex(char ch) {
        return isNumber(ch)
               || (ch >= 'A' && ch <= 'F')
               || (ch >= 'a' && ch <= 'f');
    }

    bool isOct(char ch) {
        return (ch >= '0' && ch <= '7');
    }

    bool isIdentifierStart(char ch) {
        return (ch == '_')
               || (ch >= 'a' && ch <= 'z')
               || (ch >= 'A' && ch <= 'Z');
    }

    bool isIdentifierBody(char ch) {
        return isIdentifierStart(ch) || isNumber(ch);
    }

    bool isIdentifier(string str) {
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

    bool isSpace(char ch) {
        return ch == ' '
               || ch == '\f'
               || ch == '\n'
               || ch == '\r'
               || ch == '\t'
               || ch == '\v';
    }

    json parseParameters() {
        json params = json::array();
        while (definitionIncoming()) {
            json definition = parseDefinition();
            definition["kind"] = "ParameterDeclaration";
            params.push_back(definition);

            if (lookahead(")")) {
                return params;
            }
            consume(",");
        }
        consume(")");
        return params;
    }

    json parseBody(bool shouldBeBlock = false) {
        json statements = json::array();
        if (curr == '{' || shouldBeBlock) {
            json block;
            block["kind"] = "BlockStatement";
            block["position"] = lineNumber;
            consume("{");

            while (curr && curr != '}') {
                statements.push_back(parseStatement());
            }

            consume("}");
            block["body"] = statements;
            return block;
        } else {
            json line;
            line["kind"] = "InlineStatement";
            line["position"] = lineNumber;
            if (!lookahead(";")) {
                statements.push_back(parseStatement());
            }
            line["body"] = statements;
            return line;
        }
    }

    json parseStatement() {
        json statement;
        if (lookahead("if")) {
            statement["kind"] = "IfStatement";
            statement["position"] = lineNumber;
            consume("(");
            statement["condition"] = parseExpression(")");
            statement["body"] = parseBody();
            if (lookahead("else")) {
                statement["elseBody"] = parseBody();
            } else {
                statement["elseBody"] = nullptr;
            }
            return statement;
        } else if (lookahead("while")) {
            statement["kind"] = "WhileStatement";
            statement["position"] = lineNumber;
            consume("(");
            statement["condition"] = parseExpression(")");
            statement["body"] = parseBody();
            return statement;
        } else if (lookahead("do")) {
            statement["kind"] = "DoWhileStatement";
            statement["position"] = lineNumber;
            statement["body"] = parseBody();
            consume("while");
            consume("(");
            statement["condition"] = parseExpression(")");
            consume(";");
            return statement;
        } else if (lookahead("for")) {
            statement["kind"] = "ForStatement";
            statement["position"] = lineNumber;
            consume("(");
            json init = parseStatement();
            string kind = init["kind"];
            if (kind == "VariableDefinition" || kind == "VariableDeclaration") {
                init["kind"] = "For" + kind;
            }
            statement["init"] = init;
            statement["condition"] = parseExpression(";");
            statement["step"] = parseExpression(")");
            statement["body"] = parseBody();
            return statement;
        } else if (lookahead("return")) {
            statement["kind"] = "ReturnStatement";
            statement["position"] = lineNumber;
            statement["value"] = parseExpression(";");
            return statement;
        } else if (lookahead("break")) {
            statement["kind"] = "BreakStatement";
            statement["position"] = lineNumber;
            statement["label"] = parseExpression(";");
            return statement;
        } else if (lookahead("continue")) {
            statement["kind"] = "ContinueStatement";
            statement["position"] = lineNumber;
            statement["label"] = parseExpression(";");
            return statement;
        } else if (definitionIncoming()) {
            json definition = parseDefinition();
            json length;
            while (lookahead("[")) {
                if (!lookahead("]")) {
                    length.push_back(parseExpression());
                    consume("]");
                }
            }
            if (!length.empty()) {
                definition["length"] = length;
                if (lookahead("=")) {
                    definition["kind"] = "ArrayDefinition";
                    definition["value"] = parseExpression(";");
                } else {
                    definition["kind"] = "ArrayDeclaration";
                    consume(";");
                }
            } else {
                if (lookahead("=")) {
                    definition["kind"] = "VariableDefinition";
                    definition["value"] = parseExpression(";");
                } else {
                    definition["kind"] = "VariableDeclaration";
                    consume(";");
                }
            }
            return definition;
        } else {
            statement["kind"] = "ExpressionStatement";
            statement["position"] = lineNumber;
            statement["expression"] = parseExpression(";");
            return statement;
        }
    }

    string unexpected(const string &expected) {
        return "Line number " + to_string(lineNumber) + ": Expect " + expected;
    }

    json parseExpression(const string &end = "") {
        json expr = parseBinary(parseUnary(), 0);
        if (!end.empty()) {
            consume(end);
        }
        return expr;
    }

    string scanBinaryOperator() {
        int _index = index;
        for (const string &op: operators) {
            if (lookahead(op)) {
                index = _index;
                curr = source[index];
                return op;
            }
        }
        return "";
    }

    json parseBinary(json left, int minPrecedence) {
        string ahead = scanBinaryOperator();
        while (!ahead.empty() && precedence[ahead] >= minPrecedence) {
            string op = ahead;
            int position = lineNumber;
            consume(op);
            auto right = parseUnary();
            if (right.is_null()) {
                throw unexpected("right value");
            }
            ahead = scanBinaryOperator();

            while (!ahead.empty() && precedence[ahead] > precedence[op]) {
                right = parseBinary(right, precedence[ahead]);
                if (right.is_null()) {
                    throw unexpected("right value");
                }
                ahead = scanBinaryOperator();
            }

            json newExpression;
            newExpression["kind"] = "BinaryExpression";
            newExpression["position"] = position;
            newExpression["left"] = left;
            newExpression["right"] = right;
            newExpression["operator"] = op;
            left = newExpression;
        }
        return left;
    }

    json parseLiteral() {
        json expression;
        if (lookahead("{")) {
            expression["kind"] = "ArrayLiteral";
            expression["position"] = lineNumber;
            json entries;
            while (curr) {
                entries.push_back(parseExpression());

                if (!lookahead(",")) {
                    break;
                }
            }
            consume("}");
            expression["value"] = entries;
            return expression;
        } else if (lookahead("'")) {
            expression["kind"] = "CharLiteral";
            expression["position"] = lineNumber;
            char ch = curr;
            if (curr == '\\') {
                ch = parseEscape();
            } else {
                next(true, true);
            }
            consume("'");
            expression["value"] = ch;
            return expression;
        } else if (curr == '"') {
            expression["kind"] = "StringLiteral";
            expression["position"] = lineNumber;
            expression["value"] = parseString();
            return expression;
        } else if (isFloat(curr)) {
            expression["kind"] = "NumberLiteral";
            expression["position"] = lineNumber;
            expression["value"] = parseNumber();
            return expression;
        } else if (isIdentifierStart(curr)) {
            return parseIdentifier();
        } else {
            return expression;
        }
    }

    json parseUnary() {
        json expression = parseLiteral();

        json indexExpression;
        indexExpression["kind"] = "IndexExpression";
        indexExpression["position"] = lineNumber;
        json indexes;
        while (lookahead("[")) {
            indexes.push_back(parseExpression());
            consume("]");
        }
        if (!indexes.empty()) {
            indexExpression["array"] = expression;
            indexExpression["indexes"] = indexes;
            return indexExpression;
        } else if (lookahead("(")) {
            json callExpression;
            callExpression["kind"] = "CallExpression";
            callExpression["position"] = lineNumber;
            json arguments;

            while (curr) {
                arguments.push_back(parseExpression());

                if (!lookahead(",")) {
                    break;
                }
            }
            consume(")");
            callExpression["arguments"] = arguments;
            callExpression["callee"] = expression;
            return callExpression;
        } else {
            return expression;
        }
    }

    bool definitionIncoming() {
        int prevIndex = index;
        for (const string &modifier: typeModifiers) {
            if (lookahead(modifier)) {
                index = prevIndex;
                curr = source[index];
                return true;
            }
        }
        for (const string &name: typeNames) {
            if (lookahead(name)) {
                index = prevIndex;
                curr = source[index];
                return true;
            }
        }
        return false;
    }

    json parseDefinition() {
        json modifiers = json::array();
        bool hasModifier;
        json type;
        do {
            hasModifier = false;
            for (const string &modifier: typeModifiers) {
                if (lookahead(modifier)) {
                    modifiers.push_back(modifier);
                    hasModifier = true;
                }
            }
        } while (hasModifier);
        type["kind"] = "Type";
        type["position"] = lineNumber;
        type["modifiers"] = modifiers;
        for (const string &name: typeNames) {
            if (lookahead(name)) {
                type["name"] = name;
                json definition;
                definition["identifier"] = parseIdentifier();
                definition["position"] = lineNumber;
                definition["type"] = type;
                return definition;
            }
        }
        ostringstream stream;
        copy(typeNames.begin(), typeNames.end(), ostream_iterator<string>(stream, ", "));
        throw unexpected(stream.str());
    }

    json parseInclude() {
        json statement;
        statement["kind"] = "IncludeStatement";
        string str;
        if (curr == '<') {
            while (curr && curr != '>') {
                str.push_back(curr);
                next(true);
            }
        } else if (curr == '"') {
            while (curr && curr != '"') {
                str.push_back(curr);
                next(true);
            }
        } else {
            throw unexpected("\" or <");
        }
        str.push_back(curr);
        next(true);
        statement["file"] = str;
        return statement;
    }

    string parseString(bool keepBlanks = false) {
        string str;
        next(true, true);
        while (curr && curr != '"') {
            if (curr == '\\') {
                next(true, true);
                str.push_back(parseEscape());
            } else {
                str.push_back(curr);
                next(true, true);
            }
        }
        if (!lookahead("\"", keepBlanks)) {
            throw unexpected("double quote");
        }
        return str;
    }

    char parseEscape() {
        if (curr == 'x') {
            next(true, true);
            int code = 0;

            for (int i = 0; i < 2; i++) {
                if (isHex(curr)) {
                    code = code * 16 + (int) string("0123456789abcdef").find((char) tolower(curr));
                    next(true, true);
                }
            }
            return (char) code;
        } else if (isOct(curr)) {
            int code = 0;

            for (int i = 0; i < 3; i++) {
                if (isOct(curr)) {
                    code = code * 8 + (int) string("01234567").find((char) tolower(curr));
                    next(true, true);
                }
            }
            return (char) code;
        } else if (char escape = escapes[curr]) {
            next(true, true);
            return escape;
        } else {
            throw unexpected("escape sequence");
        }
    }

    json parseIdentifier(bool keepBlanks = false) {
        if (!isIdentifierStart(curr)) {
            throw unexpected("Identifier");
        }
        json identifier;
        identifier["kind"] = "Identifier";
        identifier["position"] = lineNumber;
        string name = string(1, curr);
        next(true);
        while (curr && isIdentifierBody(curr)) {
            name.push_back(curr);
            next(true);
        }
        if (!keepBlanks) {
            skipSpaces();
        }
        identifier["name"] = name;
        return identifier;
    }

    string parseNumber(bool keepBlanks = false) {
        if (!isFloat(curr)) {
            throw unexpected("Number");
        }

        string number = string(1, curr);
        next(true);
        while (curr && isFloat(curr)) {
            number.push_back(curr);
            next(true);
        }
        if (!keepBlanks) {
            skipSpaces();
        }
        return number;
    }

    bool lookahead(const string &str, bool keepBlanks = false) {
        int _index = index;
        for (char ch: str) {
            if (curr != ch) {
                index = _index;
                curr = source[index];
                return false;
            }
            next(true);
        }

        if (isIdentifierStart(curr) && isIdentifier(str)) {
            index = _index;
            curr = source[index];
            return false;
        }

        if (!keepBlanks) {
            skipSpaces();
        }
        return true;
    }

    void consume(string str) {
        for (char ch: str) {
            if (curr != ch) {
                throw unexpected(str);
            }
            next();
        }
    }

    void skipSpaces() {
        if (isSpace(curr)) {
            next();
        }
    }

    bool skipSpaces(bool withSpaces) {
        if (withSpaces) {
            return false;
        }
        if (isSpace(curr)) {
            while (curr && isSpace(curr)) {
                if (curr == '\n') {
                    lineNumber++;
                }
                index++;
                curr = source[index];
            }
            return true;
        }
        return false;
    }

    // TODO: parse comment
    bool skipComments(bool withComments) {
        if (withComments) {
            return false;
        }
        if (curr && curr == '/' && source[index + 1] == '/') {
            while (curr != '\n') {
                index++;
                curr = source[index];
            }
            return true;
        }
        if (curr && curr == '/' && source[index + 1] == '*') {
            while (curr != '*' || source[index + 1] != '/') {
                if (curr == '\n') {
                    lineNumber++;
                }
                index++;
                curr = source[index];
            }
            index += 2;
            curr = source[index];
            return true;
        }
        return false;
    }

    void next(bool withSpaces = false, bool withComments = false) {
        if (curr == '\n') {
            lineNumber++;
        }
        index++;
        curr = source[index];
        bool skipped;
        do {
            skipped = skipComments(withComments) || skipSpaces(withSpaces);
        } while (skipped);
    };
public:
    explicit Parser(string src) : source(move(src)), curr(), lineNumber(1), index(-1) {}

    json parse() {
        next();
        json statements;
        while (curr) {
            skipSpaces();
            if (lookahead("#include")) {
                statements.push_back(parseInclude());
            } else if (definitionIncoming()) {
                json definition = parseDefinition();
                json length;
                while (lookahead("[")) {
                    if (!lookahead("]")) {
                        length.push_back(parseExpression());
                        consume("]");
                    }
                }
                if (!length.empty()) {
                    json arrayDefinition = definition;
                    arrayDefinition["length"] = length;
                    if (lookahead("=")) {
                        arrayDefinition["kind"] = "ArrayDefinition";
                        arrayDefinition["value"] = parseExpression(";");
                    } else {
                        arrayDefinition["kind"] = "ArrayDeclaration";
                        consume(";");
                    }
                    statements.push_back(arrayDefinition);
                } else if (lookahead("(")) {
                    if (lookahead(";")) {
                        json functionDeclaration = definition;
                        functionDeclaration["kind"] = "FunctionDeclaration";
                        functionDeclaration["parameters"] = parseParameters();
                        statements.push_back(functionDeclaration);
                    } else {
                        json functionDefinition = definition;
                        functionDefinition["kind"] = "FunctionDefinition";
                        functionDefinition["parameters"] = parseParameters();
                        functionDefinition["body"] = parseBody(true);
                        statements.push_back(functionDefinition);
                    }
                } else {
                    json globalVariableDefinition = definition;
                    if (lookahead("=")) {
                        globalVariableDefinition["kind"] = "GlobalVariableDefinition";
                        globalVariableDefinition["value"] = parseExpression(";");
                    } else {
                        globalVariableDefinition["kind"] = "GlobalVariableDeclaration";
                        consume(";");
                    }
                    statements.push_back(globalVariableDefinition);
                }
            } else if (lookahead("typedef")) {
                json definition = parseDefinition();
                definition["kind"] = "TypeDefinition";
                consume(";");
                statements.push_back(definition);
            } else if (lookahead("struct")) {
                throw "struct is not supported";
            } else if (lookahead("enum")) {
                throw "enum is not supported";
            } else {
                throw unexpected("definition");
            }
        }

        json program;
        program["kind"] = "Program";
        program["body"] = statements;
        return program;
    }
};
