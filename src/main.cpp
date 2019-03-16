#include <iostream>
#include "parser.cpp"
#include "formatter.cpp"

int main() {
    try {
        cout << "   ____   ____                          \n"
                "  / ___| |  _ \\ __ _ _ __ ___  ___ _ __ \n"
                " | |     | |_) / _` | '__/ __|/ _ \\ '__|\n"
                " | |___  |  __/ (_| | |  \\__ \\  __/ |   \n"
                "  \\____| |_|   \\__,_|_|  |___/\\___|_|   \n"
                "---------------------------------------------\n"
                "Welcome to C Parser!\n"
                "Please input path of your C file:\n";
        string filename;
        cin >> filename;
        ifstream inputFile(filename);
        if (!inputFile.good()) {
            throw "File doesn't exist!"s;
        }
        string line;
        string code;
        while (getline(inputFile, line)) {
            code += line;
            code.push_back('\n');
        }
        string parsed = Parser(code).parse().dump(2);
        ofstream outputFile("ast.json");
        outputFile << parsed;
        cout << "Parsed successfully!\n"
                "AST is stored in \"ast.json\"\n";
        Formatter formatter(parsed);
        formatter.save("formatted.c");
        cout << "Formatted successfully!\n"
                "Formatted code is stored in \"formatted.c\"\n";
    } catch (const string &e) {
        cout << e + "\n";
    }
    system("pause");
    return 0;
}