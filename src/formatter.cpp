#include <string>
#include <fstream>
#include "grammar.hpp"

class Formatter {

    json src;
    stringstream stream;

    /**
     * Format code with indentation
     * @param indentLevel - level of indentation
     */
    void indent(int indentLevel) {
        if (stream.str().back() == '\n') {
            for (int i = 0; i < indentLevel; i++) {
                stream << "    ";
            }
        }
    }

public:
    /**
     * Constructor of class
     * @param src - source code
     */
    explicit Formatter(const string &src) {
        this->src = json::parse(src);
    };

    /**
     * Format source code
     * @param source - source code
     * @param indentLevel - level of indentation
     */
    void format(const json &source, int indentLevel = 0) {
        indent(indentLevel);
        string kind = source["kind"];
        if (kind == "Program") {
            json body = source["body"];
            for (const json &item: body) {
                format(item);
            }
        } else if (kind == "Type") {
            json modifiers = source["modifiers"];
            for (const json &modifier: modifiers) {
                stream << string(modifier) + " ";
            }
            string name = source["name"];
            stream << name + " ";
        } else if (kind == "FunctionDefinition" || kind == "FunctionDeclaration") {
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
        } else if (kind == "GlobalVariableDeclaration" || kind == "GlobalVariableDefinition"
                   || kind == "GlobalArrayDefinition" || kind == "GlobalArrayDeclaration"
                   || kind == "ArrayDefinition" || kind == "ArrayDeclaration"
                   || kind == "VariableDefinition" || kind == "VariableDeclaration"
                   || kind == "ForVariableDefinition" || kind == "ForVariableDeclaration") {
            format(source["type"]);
            format(source["identifier"]);
            if (kind.find("Array") != string::npos) {
                json lengths = source["length"];
                for (const json &length: lengths) {
                    stream << "[";
                    format(length);
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
        } else if (kind.find("NumberLiteral") != string::npos) {
            string value = source["value"];
            stream << value;
        } else if (kind == "CharLiteral") {
            string value = source["value"];
            stream << "'" + value + "'";
        } else if (kind == "StringLiteral") {
            string value = source["value"];
            stream << "\"" + value + "\"";
        } else if (kind == "ArrayLiteral") {
            json values = source["value"];
            stream << "{ ";
            for (int i = 0; i < values.size(); i++) {
                format(values[i]);
                if (i != values.size() - 1) {
                    stream << ", ";
                }
            }
            stream << " }";
        } else if (kind == "BinaryExpression") {
            json left = source["left"];
            json right = source["right"];
            string op = source["operator"];
            format(left);
            stream << " " + op + " ";
            format(right);
        } else if (kind == "IndexExpression") {
            string name = source["array"]["name"];
            stream << name;
            json indexes = source["indexes"];
            for (const json &index: indexes) {
                stream << "[";
                format(index);
                stream << "]";
            }
        } else if (kind == "CallExpression") {
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
        } else if (kind == "ParenthesesExpression") {
            stream << "(";
            format(source["expression"]);
            stream << ")";
        } else if (kind == "Identifier") {
            string name = source["name"];
            stream << name;
        } else if (kind == "ExpressionStatement") {
            json expression = source["expression"];
            if (!expression.is_null()) {
                format(expression);
            }
            stream << ";";
        } else if (kind == "BlockStatement" || kind == "InlineStatement") {
            json body = source["body"];
            for (const json &item: body) {
                stream << "\n";
                format(item, indentLevel + 1);
            }
            if (!body.empty()) {
                stream << "\n";
            }
        } else if (kind == "IfStatement") {
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
        } else if (kind == "ForStatement") {
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
        } else if (kind == "WhileStatement") {
            stream << "while (";
            json condition = source["condition"];
            if (!condition.is_null()) {
                format(condition);
            }
            stream << ") {";
            format(source["body"], indentLevel);
            indent(indentLevel);
            stream << "}";
        } else if (kind == "DoWhileStatement") {
            stream << "do {";
            format(source["body"], indentLevel);
            indent(indentLevel);
            stream << "} while (";
            json condition = source["condition"];
            if (!condition.is_null()) {
                format(condition);
            }
            stream << ");";
        } else if (kind == "ReturnStatement") {
            stream << "return";
            json value = source["value"];
            if (!value.is_null()) {
                stream << " ";
                format(value);
            }
            stream << ";";
        } else if (kind == "BreakStatement") {
            stream << "break";
            json label = source["label"];
            if (!label.is_null()) {
                stream << " ";
                format(label);
            }
            stream << ";";
        } else if (kind == "ContinueStatement") {
            stream << "continue";
            json label = source["label"];
            if (!label.is_null()) {
                stream << " ";
                format(label);
            }
            stream << ";";
        } else if (kind == "IncludeStatement") {
            stream << "#include ";
            string file = source["file"];
            stream << file + "\n";
        } else if (kind == "PredefineStatement") {
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
        } else if (kind == "TypeDefinition") {
            stream << "\n";
            stream << "typedef ";
            format(source["type"]);
            format(source["identifier"]);
            stream << ";\n";
        } else if (kind == "InlineComment") {
            stream << "// ";
            string content = source["content"];;
            stream << content;
            stream << "\n";
        } else if (kind == "BlockComment") {
            stream << "/* ";
            string content = source["content"];;
            stream << content;
            stream << " */";
        }
    }

    /**
     * Save result
     * @param filename - file to be saved
     */
    void save(const string &filename) {
        format(src);
        ofstream file(filename);
        file << stream.str();
    }
};