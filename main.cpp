#include <iostream>
#include <fstream>
#include "parser.cpp"

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
        outputFile << Parser(code).parse().dump(2);
    } catch (const string &e) {
        cout << e;
    }
    return 0;
}