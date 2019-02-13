#include <iostream>
#include <fstream>
#include "parser.cpp"
#include "formatter.cpp"

using namespace std;

int main() {
    try {
        ifstream inputFile("../example/test.c");
        ofstream outputFile("../example/test.json");
        string line;
        string code;
        while (getline(inputFile, line)) {
            code += line;
            code.push_back('\n');
        }
        string parsed = Parser(code).parse().dump(2);
        outputFile << parsed;
        Formatter formatter(parsed);
        formatter.beginFormat("../example/formatted.c");
    } catch (const string &e) {
        cout << e;
    }
    return 0;
}