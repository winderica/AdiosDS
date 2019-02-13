#include <iostream>
#include <fstream>
#include "parser.cpp"
#include "formatter.cpp"

using namespace std;

int main() {
    try {
        ifstream inputFile("../test.c");
        ofstream outputFile("../test.json");
        string line;
        string code;
        while (getline(inputFile, line)) {
            code += line;
            code.push_back('\n');
        }
        string parsed = Parser(code).parse().dump(2);
        outputFile << parsed;
        Formatter formatter(parsed);
        formatter.beginFormat();
    } catch (const string &e) {
        cout << e;
    }
    return 0;
}