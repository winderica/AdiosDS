#include <string>
#include <vector>
#include <fstream>
#include "../lib/json.hpp"

using namespace std;
using json = nlohmann::json;

class Formatter {

    json src;
    stringstream stream;

    void indent(int indentLevel) {
        if (stream.str().back() == '\n') {
            for (int i = 0; i < indentLevel; i++) {
                stream << "    ";
            }
        }
    }

public:
    explicit Formatter(const string &src) {
        this->src = json::parse(src);
    };

    void format(const json &source, int indentLevel = 0) {
        indent(indentLevel);
        string kind = source["kind"];
        if (kind == "Program") {
            vector<json> body = source["body"];
            for (const json &item: body) {
                format(item);
            }
        } else if (kind == "Type") {
            vector<string> modifiers = source["modifiers"];
            for (const string &modifier: modifiers) {
                stream << modifier + " ";
            }
            string name = source["name"];
            stream << name + " ";
        } else if (kind == "FunctionDefinition" || kind == "FunctionDeclaration") {
            format(source["type"]);
            format(source["identifier"]);
            stream << "(";
            vector<json> params = source["parameters"];
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
                   || kind == "ArrayDefinition" || kind == "ArrayDeclaration"
                   || kind == "VariableDefinition" || kind == "VariableDeclaration"
                   || kind == "ForVariableDefinition" || kind == "ForVariableDeclaration"
                ) {
            format(source["type"]);
            format(source["identifier"]);
            if (kind.find("Array") != string::npos) {
                vector<json> lengths = source["length"];
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
            if (kind.find("For") == string::npos) {
                stream << "\n";
            }
            stream << ";";
        } else if (kind == "NumberLiteral") {
            string value = source["value"];
            stream << value;
        } else if (kind == "CharLiteral") {
            string value = source["value"];
            stream << "'" + value + "'";
        } else if (kind == "StringLiteral") {
            string value = source["value"];
            stream << "\"" + value + "\"";
        } else if (kind == "ArrayLiteral") {
            vector<json> values = source["value"];
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
            vector<json> indexes = source["indexes"];
            for (const json &index: indexes) {
                stream << "[";
                format(index);
                stream << "]";
            }
        } else if (kind == "CallExpression") {
            string name = source["callee"]["name"];
            stream << name;
            stream << "(";
            vector<json> arguments = source["arguments"];
            for (int i = 0; i < arguments.size(); i++) {
                format(arguments[i]);
                if (i != arguments.size() - 1) {
                    stream << ", ";
                }
            }
            stream << ")";
        } else if (kind == "Identifier") {
            string name = source["name"];
            stream << name;
        } else if (kind == "ExpressionStatement") {
            json expression = source["expression"];
            format(expression);
            stream << ";";
        } else if (kind == "BlockStatement" || kind == "InlineStatement") {
            vector<json> body = source["body"];
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
            format(condition);
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
            format(condition);
            stream << "; ";
            json step = source["step"];
            format(step);
            stream << ") {";
            format(source["body"], indentLevel);
            indent(indentLevel);
            stream << "}";
        } else if (kind == "WhileStatement") {
            stream << "while (";
            json condition = source["condition"];
            format(condition);
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
            format(condition);
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
        } else if (kind == "TypeDefinition") {
            stream << "typedef ";
            format(source["type"]);
            format(source["identifier"]);
            stream << ";\n";
        }
    }

    void beginFormat(const string &filename) {
        format(src);
        ofstream file(filename);
        file << stream.str();
    }
};