#ifndef PARSER_FORMATTER_HPP
#define PARSER_FORMATTER_HPP

#include <fstream>
#include "grammar.hpp"

class Formatter {

    json src;
    stringstream stream;

    /**
     * Format code with indentation
     * @param indentLevel - level of indentation
     */
    void indent(int indentLevel);

public:
    /**
     * Constructor of class
     * @param src - source code
     */
    explicit Formatter(const string &src);

    /**
     * Format source code
     * @param source - source code
     * @param indentLevel - level of indentation
     */
    void format(const json &source, int indentLevel = 0);

    /**
     * Save result to file
     * @param filename - file to be saved
     */
    void save(const string &filename);

    /**
     * Format program
     * @param source - source code
     */
    void formatProgram(const json &source);

    /**
     * Format type
     * @param source - source code
     */
    void formatType(const json &source);

    /**
     * Format function
     * @param source - source code
     * @param indentLevel - level of indentation
     * @param kind - statement kind
     */
    void formatFunction(const json &source, int indentLevel, const string &kind);

    /**
     * Format declaration/definition
     * @param source - source code
     * @param kind - statement kind
     */
    void formatDeclaration(const json &source, const string &kind);

    /**
     * Format number
     * @param source - source code
     */
    void formatNumber(const json &source);

    /**
     * Format char
     * @param source - source code
     */
    void formatChar(const json &source);

    /**
     * Format string
     * @param source - source code
     */
    void formatString(const json &source);

    /**
     * Format array
     * @param source - source code
     */
    void formatArray(const json &source);

    /**
     * Format binary expression
     * @param source - source code
     */
    void formatBinary(const json &source);

    /**
     * Format index expression
     * @param source - source code
     */
    void formatIndex(const json &source);

    /**
     * Format call expression
     * @param source - source code
     */
    void formatCall(const json &source);

    /**
     * Format parentheses expression
     * @param source - source code
     */
    void formatParentheses(const json &source);

    /**
     * Format identifier
     * @param source - source code
     */
    void formatIdentifier(const json &source);

    /**
     * Format expression statement
     * @param source - source code
     */
    void formatExpression(const json &source);

    /**
     * Format body
     * @param source - source code
     * @param indentLevel - level of indentation
     */
    void formatBody(const json &source, int indentLevel);

    /**
     * Format if statement
     * @param source - source code
     * @param indentLevel - level of indentation
     */
    void formatIf(const json &source, int indentLevel);

    /**
     * Format for statement
     * @param source - source code
     * @param indentLevel - level of indentation
     */
    void formatFor(const json &source, int indentLevel);

    /**
     * Format while statement
     * @param source - source code
     * @param indentLevel - level of indentation
     */
    void formatWhile(const json &source, int indentLevel);

    /**
     * Format do-while statement
     * @param source - source code
     * @param indentLevel - level of indentation
     */
    void formatDoWhile(const json &source, int indentLevel);

    /**
     * Format return statement
     * @param source - source code
     */
    void formatReturn(const json &source);

    /**
     * Format break statement
     * @param source - source code
     */
    void formatBreak(const json &source);

    /**
     * Format continue statement
     * @param source - source code
     */
    void formatContinue(const json &source);

    /**
     * Format include statement
     * @param source - source code
     */
    void formatInclude(const json &source);

    /**
     * Format predefine statement
     * @param source - source code
     */
    void formatPredefine(const json &source);

    /**
     * Format typedef statement
     * @param source - source code
     */
    void formatTypedef(const json &source);

    /**
     * Format comment
     * @param source - source code
     * @param isInline - is inline comment
     */
    void formatComment(const json &source, bool isInline);
};

#endif //PARSER_FORMATTER_HPP
