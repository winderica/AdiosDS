#ifndef PARSER_H
#define PARSER_H

#include "grammar.hpp"

namespace ParserNamespace {

    struct Program {
        string kind;
        json body;
    };

    struct ProgramItem {
        string kind;
        int position;
    };

    struct IncludeStatement : ProgramItem {
        string file;
    };

    struct Comment : ProgramItem {
        string content;
    };

    struct Type : ProgramItem {
        json modifiers;
        string name;
    };

    struct Identifier : ProgramItem {
        string name;
    };

    struct PredefineStatement : ProgramItem {
        Identifier identifier;
        json arguments;
        json value;
    };

    struct Declaration : ProgramItem {
        Type type;
        Identifier identifier;
    };

    struct Definition : Declaration {
        json length;
        json value;
    };

    struct FunctionDeclaration : Declaration {
        json parameters;
    };

    struct FunctionDefinition : FunctionDeclaration {
        json body;
    };

    template<class T>
    struct Literal : ProgramItem {
        T value;
    };

    struct IndexExpression : ProgramItem {
        json array;
        json indexes;
    };

    struct CallExpression : ProgramItem {
        json arguments;
        json callee;
    };

    struct ParenthesesExpression : ProgramItem {
        json expression;
    };

    struct BinaryExpression : ProgramItem {
        string op;
        json left;
        json right;
    };

    struct BodyStatement : ProgramItem {
        json body;
    };

    struct IfStatement : BodyStatement {
        json condition;
        json elseBody;
    };

    struct WhileStatement : BodyStatement {
        json condition;
    };

    struct ForStatement : BodyStatement {
        json init;
        json condition;
        json step;
    };

    struct ReturnStatement : ProgramItem {
        json value;
    };

    struct InterruptStatement : ProgramItem {
        json label;
    };

    struct ExpressionStatement : ProgramItem {
        json expression;
    };

    void to_json(json &j, const Program &p) {
        j = json{
                {"kind", p.kind},
                {"body", p.body},
        };
    }

    void to_json(json &j, const IncludeStatement &p) {
        j = json{
                {"kind",     p.kind},
                {"position", p.position},
                {"file",     p.file},
        };
    }

    void to_json(json &j, const Comment &p) {
        j = json{
                {"kind",     p.kind},
                {"position", p.position},
                {"content",  p.content},
        };
    }

    void to_json(json &j, const Type &p) {
        j = json{
                {"kind",      p.kind},
                {"position",  p.position},
                {"name",      p.name},
                {"modifiers", p.modifiers},
        };
    }

    void to_json(json &j, const Identifier &p) {
        j = json{
                {"kind",     p.kind},
                {"position", p.position},
                {"name",     p.name},
        };
    }

    void to_json(json &j, const Declaration &p) {
        j = json{
                {"kind",       p.kind},
                {"position",   p.position},
                {"identifier", p.identifier},
                {"type",       p.type},
        };
    }

    void to_json(json &j, const Definition &p) {
        j = json{
                {"kind",       p.kind},
                {"position",   p.position},
                {"identifier", p.identifier},
                {"type",       p.type},
        };
        if (!p.length.empty()) {
            j["length"] = p.length;
        }
        if (!p.value.is_null()) {
            j["value"] = p.value;
        }
    }

    void to_json(json &j, const FunctionDeclaration &p) {
        j = json{
                {"kind",       p.kind},
                {"position",   p.position},
                {"identifier", p.identifier},
                {"type",       p.type},
                {"parameters", p.parameters},
        };
    }

    void to_json(json &j, const FunctionDefinition &p) {
        j = json{
                {"kind",       p.kind},
                {"position",   p.position},
                {"identifier", p.identifier},
                {"type",       p.type},
                {"parameters", p.parameters},
                {"body",       p.body},
        };
    }

    template<typename T>
    void to_json(json &j, const Literal<T> &p) {
        j = json{
                {"kind",     p.kind},
                {"position", p.position},
                {"value",    p.value},
        };
    }

    void to_json(json &j, const IndexExpression &p) {
        j = json{
                {"kind",     p.kind},
                {"position", p.position},
                {"array",    p.array},
                {"indexes",  p.indexes},
        };
    }

    void to_json(json &j, const CallExpression &p) {
        j = json{
                {"kind",      p.kind},
                {"position",  p.position},
                {"callee",    p.callee},
                {"arguments", p.arguments},
        };
    }

    void to_json(json &j, const ParenthesesExpression &p) {
        j = json{
                {"kind",       p.kind},
                {"position",   p.position},
                {"expression", p.expression},
        };
    }

    void to_json(json &j, const BodyStatement &p) {
        j = json{
                {"kind",     p.kind},
                {"position", p.position},
                {"body",     p.body},
        };
    }

    void to_json(json &j, const IfStatement &p) {
        j = json{
                {"kind",      p.kind},
                {"position",  p.position},
                {"body",      p.body},
                {"condition", p.condition},
                {"elseBody",  p.elseBody},
        };
    }

    void to_json(json &j, const WhileStatement &p) {
        j = json{
                {"kind",      p.kind},
                {"position",  p.position},
                {"body",      p.body},
                {"condition", p.condition},
        };
    }

    void to_json(json &j, const ForStatement &p) {
        j = json{
                {"kind",      p.kind},
                {"position",  p.position},
                {"body",      p.body},
                {"init",      p.init},
                {"condition", p.condition},
                {"step",      p.step},
        };
    }

    void to_json(json &j, const ReturnStatement &p) {
        j = json{
                {"kind",     p.kind},
                {"position", p.position},
                {"value",    p.value},
        };
    }

    void to_json(json &j, const InterruptStatement &p) {
        j = json{
                {"kind",     p.kind},
                {"position", p.position},
                {"label",    p.label},
        };
    }

    void to_json(json &j, const ExpressionStatement &p) {
        j = json{
                {"kind",       p.kind},
                {"position",   p.position},
                {"expression", p.expression},
        };
    }

    void to_json(json &j, const PredefineStatement &p) {
        j = json{
                {"kind",       p.kind},
                {"position",   p.position},
                {"identifier", p.identifier},
                {"arguments",  p.arguments},
                {"value",      p.value},
        };
    }

    void to_json(json &j, const BinaryExpression &p) {
        j = json{
                {"kind",     p.kind},
                {"position", p.position},
                {"operator", p.op},
                {"left",     p.left},
                {"right",    p.right},
        };
    }

    /**
     * parser class
     */
    class Parser : Grammar {
        string source;
        char curr;
        int index;
        int lineNumber;
        json comments;

        /**
         * Error on unexpected character
         * @param expected - expecting characters
         * @return - error message
         */
        string unexpected(const string &expected);

        /**
         * Parse parameters of function
         * @return - JSON tree of parameters
         */
        json parseParameters();

        /**
         * Parse body of sub-statements
         * @param shouldBeBlock - should sub-statements be block statements
         * @return - JSON tree of body
         */
        json parseBody(bool shouldBeBlock = false);

        /**
         * Parse statement
         * @return - JSON tree of statement
         */
        json parseStatement();

        /**
         * Parse definition
         * @param declaration - original declaration
         * @return - JSON tree of definition
         */
        json parseDefinition(Declaration declaration, bool isGlobal = false);

        /**
         * Parse expression
         * @param end - end character
         * @return - JSON tree of expression
         */
        json parseExpression(const string &end = "");

        /**
         * Get incoming operator
         * @return - operator
         */
        string scanBinaryOperator();

        /**
         * Parse binary expression
         * @param left - left value of binary expression
         * @param minPrecedence - minimum precedence of operator
         * @return - JSON tree of binary expression
         */
        json parseBinary(json left, int minPrecedence);

        /**
         * Parse unary expression
         * @return  - JSON tree of unary expression
         */
        json parseUnary();

        /**
         * Parse literal value
         * @return - JSON tree of literal
         */
        json parseLiteral();

        /**
         * Determine incoming declaration
         * @return - whether incoming string is declaration
         */
        bool declarationIncoming();

        /**
         * Parse declaration
         * @return - JSON tree of declaration
         */
        Declaration parseDeclaration(string kind = "");

        json parseFunction(Declaration declaration);

        /**
         * Parse #include statement
         * @return - JSON tree of #include statement
         */
        json parseInclude();

        /**
         * Parse #define statement
         * @return - JSON tree of #define statement
         */
        json parsePredefine();

        /**
         * Parse string literal
         * @param keepBlanks - should keep spaces
         * @return - string literal
         */
        string parseString(bool keepBlanks = false);

        /**
         * Parse escaped char
         * @return - unescaped char
         */
        char parseEscape();

        /**
         * Parse identifier
         * @param keepBlanks - should keep spaces
         * @return - JSON tree of identifier
         */
        Identifier parseIdentifier(bool keepBlanks = false);

        /**
         * Parse number literal
         * @param digits - valid digits
         * @return - JSON tree of number literal
         */
        Literal<string> parseNumber(int digits);

        /**
         * Parse comment
         * @return - JSON tree of comment
         */
        json parseComment();

        /**
         * Match string ahead
         * @param str - string to match
         * @param keepBlanks - should keep spaces
         * @return - result of matching
         */
        bool lookahead(const string &str, bool keepBlanks = false);

        /**
         * Consume rest characters
         * @param str - characters to be skipped
         */
        void consume(string str);

        /**
         * Skip all spaces
         */
        void skipSpaces();

        /**
         * Go to next valid character
         * @param withSpaces - should keep spaces
         */
        void next(bool withSpaces = false);

    public:
        /**
         * Constructor of class
         * @param src - source code
         */
        explicit Parser(string src);

        /**
         * Parse source code
         * @return
         */
        json parse();
    };
}

#endif // PARSER_H
