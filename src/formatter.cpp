#include "formatter.hpp"

/**
 * Constructor of class
 * @param src - source code
 */
Formatter::Formatter(const string &src) {
    this->src = json::parse(src);
}

/**
 * Format code with indentation
 * @param indentLevel - level of indentation
 */
void Formatter::indent(int indentLevel) {
    if (stream.str().back() == '\n') {
        for (int i = 0; i < indentLevel; i++) {
            stream << "    ";
        }
    }
}

/**
 * Save result to file
 * @param filename - file to be saved
 */
void Formatter::save(const string &filename) {
    format(src);
    ofstream file(filename);
    file << stream.str();
}

/**
 * Format source code
 * @param source - source code
 * @param indentLevel - level of indentation
 */
void Formatter::format(const json &source, int indentLevel) {
    indent(indentLevel);
    string kind = source["kind"];
    if (kind == "Program") {
        formatProgram(source);
    } else if (kind == "Type") {
        formatType(source);
    } else if (kind == "FunctionDefinition" || kind == "FunctionDeclaration") {
        formatFunction(source, indentLevel, kind);
    } else if (kind == "GlobalVariableDeclaration" || kind == "GlobalVariableDefinition"
               || kind == "GlobalArrayDefinition" || kind == "GlobalArrayDeclaration"
               || kind == "ArrayDefinition" || kind == "ArrayDeclaration"
               || kind == "VariableDefinition" || kind == "VariableDeclaration"
               || kind == "ForVariableDefinition" || kind == "ForVariableDeclaration") {
        formatDeclaration(source, kind);
    } else if (kind.find("NumberLiteral") != string::npos) {
        formatNumber(source);
    } else if (kind == "CharLiteral") {
        formatChar(source);
    } else if (kind == "StringLiteral") {
        formatString(source);
    } else if (kind == "ArrayLiteral") {
        formatArray(source);
    } else if (kind == "BinaryExpression") {
        formatBinary(source);
    } else if (kind == "IndexExpression") {
        formatIndex(source);
    } else if (kind == "CallExpression") {
        formatCall(source);
    } else if (kind == "ParenthesesExpression") {
        formatParentheses(source);
    } else if (kind == "Identifier") {
        formatIdentifier(source);
    } else if (kind == "ExpressionStatement") {
        formatExpression(source);
    } else if (kind == "BlockStatement" || kind == "InlineStatement") {
        formatBody(source, indentLevel);
    } else if (kind == "IfStatement") {
        formatIf(source, indentLevel);
    } else if (kind == "ForStatement") {
        formatFor(source, indentLevel);
    } else if (kind == "WhileStatement") {
        formatWhile(source, indentLevel);
    } else if (kind == "DoWhileStatement") {
        formatDoWhile(source, indentLevel);
    } else if (kind == "ReturnStatement") {
        formatReturn(source);
    } else if (kind == "BreakStatement") {
        formatBreak(source);
    } else if (kind == "ContinueStatement") {
        formatContinue(source);
    } else if (kind == "IncludeStatement") {
        formatInclude(source);
    } else if (kind == "PredefineStatement") {
        formatPredefine(source);
    } else if (kind == "TypeDefinition") {
        formatTypedef(source);
    } else if (kind == "InlineComment") {
        formatComment(source, true);
    } else if (kind == "BlockComment") {
        formatComment(source, false);
    }
}

/**
 * Format program
 * @param source - source code
 */
void Formatter::formatProgram(const json &source) {
    json body = source["body"];
    for (const json &item: body) {
        format(item);
    }
}

/**
 * Format type
 * @param source - source code
 */
void Formatter::formatType(const json &source) {
    json modifiers = source["modifiers"];
    for (const json &modifier: modifiers) {
        stream << string(modifier) + " ";
    }
    string name = source["name"];
    stream << name + " ";
}

/**
 * Format function
 * @param source - source code
 * @param indentLevel - level of indentation
 * @param kind - statement kind
 */
void Formatter::formatFunction(const json &source, int indentLevel, const string &kind) {
    stream << "\n";
    format(source["type"]);
    format(source["identifier"]);
    stream << "(";
    json params = source["parameters"];
    for (int i = 0; i < params.size(); i++) {
        json param = params[i];
        string typeName = param["type"]["name"];
        stream << typeName + " ";
        format(param["identifier"]);
        if (i != params.size() - 1) {
            stream << ", ";
        }
    }
    stream << ")";
    if (kind == "FunctionDeclaration") {
        stream << ";";
    } else {
        stream << " {";
        format(source["body"], indentLevel);
        indent(indentLevel);
        stream << "}";
    }
    stream << "\n";
}

/**
 * Format declaration/definition
 * @param source - source code
 * @param kind - statement kind
 */
void Formatter::formatDeclaration(const json &source, const string &kind) {
    format(source["type"]);
    format(source["identifier"]);
    if (kind.find("Array") != string::npos) {
        json lengths = source["length"];
        for (const json &length: lengths) {
            stream << "[";
            if (!length.is_null()) {
                format(length);
            }
            stream << "]";
        }
    }
    if (kind.find("Definition") != string::npos) {
        stream << " = ";
        json value = source["value"];
        format(value);
    }
    stream << ";";
    if (kind.find("Global") != string::npos) {
        stream << "\n";
    }
}

/**
 * Format number
 * @param source - source code
 */
void Formatter::formatNumber(const json &source) {
    string value = source["value"];
    stream << value;
}

/**
 * Format char
 * @param source - source code
 */
void Formatter::formatChar(const json &source) {
    string value = source["value"];
    stream << "'" + value + "'";
}

/**
 * Format string
 * @param source - source code
 */
void Formatter::formatString(const json &source) {
    string value = source["value"];
    stream << "\"" + value + "\"";
}

/**
 * Format array
 * @param source - source code
 */
void Formatter::formatArray(const json &source) {
    json values = source["value"];
    stream << "{ ";
    for (int i = 0; i < values.size(); i++) {
        format(values[i]);
        if (i != values.size() - 1) {
            stream << ", ";
        }
    }
    stream << " }";
}

/**
 * Format binary expression
 * @param source - source code
 */
void Formatter::formatBinary(const json &source) {
    json left = source["left"];
    json right = source["right"];
    string op = source["operator"];
    format(left);
    stream << " " + op + " ";
    format(right);
}

/**
 * Format index expression
 * @param source - source code
 */
void Formatter::formatIndex(const json &source) {
    string name = source["array"]["name"];
    stream << name;
    json indexes = source["indexes"];
    for (const json &index: indexes) {
        stream << "[";
        format(index);
        stream << "]";
    }
}

/**
 * Format call expression
 * @param source - source code
 */
void Formatter::formatCall(const json &source) {
    string name = source["callee"]["name"];
    stream << name;
    stream << "(";
    json arguments = source["arguments"];
    for (int i = 0; i < arguments.size(); i++) {
        format(arguments[i]);
        if (i != arguments.size() - 1) {
            stream << ", ";
        }
    }
    stream << ")";
}

/**
 * Format parentheses expression
 * @param source - source code
 */
void Formatter::formatParentheses(const json &source) {
    stream << "(";
    format(source["expression"]);
    stream << ")";
}

/**
 * Format identifier
 * @param source - source code
 */
void Formatter::formatIdentifier(const json &source) {
    string name = source["name"];
    stream << name;
}

/**
 * Format expression statement
 * @param source - source code
 */
void Formatter::formatExpression(const json &source) {
    json expression = source["expression"];
    if (!expression.is_null()) {
        format(expression);
    }
    stream << ";";
}

/**
 * Format body
 * @param source - source code
 * @param indentLevel - level of indentation
 */
void Formatter::formatBody(const json &source, int indentLevel) {
    json body = source["body"];
    for (const json &item: body) {
        stream << "\n";
        format(item, indentLevel + 1);
    }
    if (!body.empty()) {
        stream << "\n";
    }
}

/**
 * Format if statement
 * @param source - source code
 * @param indentLevel - level of indentation
 */
void Formatter::formatIf(const json &source, int indentLevel) {
    stream << "if (";
    json condition = source["condition"];
    if (!condition.is_null()) {
        format(condition);
    }
    stream << ") {";
    format(source["body"], indentLevel);
    indent(indentLevel);
    stream << "}";
    json else_ = source["elseBody"];
    if (!else_.is_null()) {
        stream << " else {";
        format(source["elseBody"], indentLevel);
        indent(indentLevel);
        stream << "}";
    }
}

/**
 * Format for statement
 * @param source - source code
 * @param indentLevel - level of indentation
 */
void Formatter::formatFor(const json &source, int indentLevel) {
    stream << "for (";
    json init = source["init"];
    format(init);
    stream << " ";
    json condition = source["condition"];
    if (!condition.is_null()) {
        format(condition);
    }
    stream << "; ";
    json step = source["step"];
    if (!step.is_null()) {
        format(step);
    }
    stream << ") {";
    format(source["body"], indentLevel);
    indent(indentLevel);
    stream << "}";
}

/**
 * Format while statement
 * @param source - source code
 * @param indentLevel - level of indentation
 */
void Formatter::formatWhile(const json &source, int indentLevel) {
    stream << "while (";
    json condition = source["condition"];
    if (!condition.is_null()) {
        format(condition);
    }
    stream << ") {";
    format(source["body"], indentLevel);
    indent(indentLevel);
    stream << "}";
}

/**
 * Format do-while statement
 * @param source - source code
 * @param indentLevel - level of indentation
 */
void Formatter::formatDoWhile(const json &source, int indentLevel) {
    stream << "do {";
    format(source["body"], indentLevel);
    indent(indentLevel);
    stream << "} while (";
    json condition = source["condition"];
    if (!condition.is_null()) {
        format(condition);
    }
    stream << ");";
}

/**
 * Format return statement
 * @param source - source code
 */
void Formatter::formatReturn(const json &source) {
    stream << "return";
    json value = source["value"];
    if (!value.is_null()) {
        stream << " ";
        format(value);
    }
    stream << ";";
}

/**
 * Format break statement
 * @param source - source code
 */
void Formatter::formatBreak(const json &source) {
    stream << "break";
    json label = source["label"];
    if (!label.is_null()) {
        stream << " ";
        format(label);
    }
    stream << ";";
}

/**
 * Format continue statement
 * @param source - source code
 */
void Formatter::formatContinue(const json &source) {
    stream << "continue";
    json label = source["label"];
    if (!label.is_null()) {
        stream << " ";
        format(label);
    }
    stream << ";";
}

/**
 * Format include statement
 * @param source - source code
 */
void Formatter::formatInclude(const json &source) {
    stream << "#include ";
    string file = source["file"];
    stream << file + "\n";
}

/**
 * Format predefine statement
 * @param source - source code
 */
void Formatter::formatPredefine(const json &source) {
    stream << "#define ";
    string identifier = source["identifier"]["name"];
    stream << identifier;

    json arguments = source["arguments"];
    if (!arguments.is_null()) {
        stream << "(";
        for (int i = 0; i < arguments.size(); i++) {
            format(arguments[i]);
            if (i != arguments.size() - 1) {
                stream << ", ";
            }
        }
        stream << ")";
    }
    stream << " ";
    json value = source["value"];
    format(value);
    stream << "\n";
}

/**
 * Format typedef statement
 * @param source - source code
 */
void Formatter::formatTypedef(const json &source) {
    stream << "\n";
    stream << "typedef ";
    format(source["type"]);
    format(source["identifier"]);
    stream << ";\n";
}

/**
 * Format comment
 * @param source - source code
 * @param isInline - is inline comment
 */
void Formatter::formatComment(const json &source, bool isInline) {
    stream << (isInline ? "// " : "/* ");
    string content = source["content"];
    stream << content;
    stream << (isInline ? "\n" : " */");
}
