#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <chrono>
#include "parser.cpp"
#include "formatter.cpp"
#include "grammar.cpp"

using namespace std::chrono;

long getTime() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

int main() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
    try {
#ifdef unix
        cout << "\033[1;31m   ____   ____                          \033[0m\n"
                "\033[1;32m  / ___| |  _ \\ __ _ _ __ ___  ___ _ __ \033[0m\n"
                "\033[1;33m | |     | |_) / _` | '__/ __|/ _ \\ '__|\033[0m\n"
                "\033[1;34m | |___  |  __/ (_| | |  \\__ \\  __/ |   \033[0m\n"
                "\033[1;35m  \\____| |_|   \\__,_|_|  |___/\\___|_|   \033[0m\n"
                "\033[1;36m---------------------------------------------\033[0m\n"
                "\033[1;34mWelcome to C Parser!\033[0m\n"
                "\033[1;34mPlease input path of your C file:\033[0m\n";
#elif defined(_WIN32)
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, 10);
        cout << "   ____   ____                          \n";
        SetConsoleTextAttribute(hConsole, 11);
        cout << "  / ___| |  _ \\ __ _ _ __ ___  ___ _ __ \n";
        SetConsoleTextAttribute(hConsole, 12);
        cout << " | |     | |_) / _` | '__/ __|/ _ \\ '__|\n";
        SetConsoleTextAttribute(hConsole, 13);
        cout << " | |___  |  __/ (_| | |  \\__ \\  __/ |   \n";
        SetConsoleTextAttribute(hConsole, 14);
        cout << "  \\____| |_|   \\__,_|_|  |___/\\___|_|   \n";
        SetConsoleTextAttribute(hConsole, 15);
        cout << "---------------------------------------------\n";
        SetConsoleTextAttribute(hConsole, 11);
        cout << "Welcome to C Parser!\n";
        SetConsoleTextAttribute(hConsole, 11);
        cout << "Please input path of your C file:\n";
        SetConsoleTextAttribute(hConsole, 15);
#endif
        string filename;
        cin >> filename;
        ifstream inputFile(filename);
        if (!inputFile.good()) {
            throw runtime_error("File doesn't exist!"s);
        }
        string line;
        string code;
        while (getline(inputFile, line)) {
            code += line;
            code.push_back('\n');
        }
        long beforeParse = getTime();
        string parsed = Parser(code).parse().dump(2);
        long afterParse = getTime();
        ofstream outputFile("ast.json");
        outputFile << parsed;
#ifdef unix
        cout << "\033[1;32m\nParsed successfully!\033[0m\n"
                "\033[1;33mAST is stored in \"ast.json\"\033[0m\n";
#elif defined(_WIN32)
        SetConsoleTextAttribute(hConsole, 10);
        cout << "\nParsed successfully!\n";
        SetConsoleTextAttribute(hConsole, 14);
        cout << "AST is stored in \"ast.json\"\n";
        SetConsoleTextAttribute(hConsole, 15);
#endif
        cout << "Parsing took " << afterParse - beforeParse << "ms\n";
        long beforeFormat = getTime();
        Formatter formatter(parsed);
        formatter.save("formatted.c");
        long afterFormat = getTime();
#ifdef unix
        cout << "\033[1;32m\nFormatted successfully!\033[0m\n"
                "\033[1;33mFormatted code is stored in \"formatted.c\"\033[0m\n";
#elif defined(_WIN32)
        SetConsoleTextAttribute(hConsole, 10);
        cout << "\nFormatted successfully!\n";
        SetConsoleTextAttribute(hConsole, 14);
        cout << "Formatted code is stored in \"formatted.c\"\n";
        SetConsoleTextAttribute(hConsole, 15);
#endif
        cout << "Formatting took " << afterFormat - beforeFormat << "ms\n";
    } catch (exception &e) {
#ifdef unix
        cout << "\033[1;31m"s + e.what() + "\033[0m\n";
#elif defined(_WIN32)
        SetConsoleTextAttribute(hConsole, 12);
        cout << e.what() << "\n";
        SetConsoleTextAttribute(hConsole, 15);
#endif
    }
#ifdef _WIN32
    system("pause");
#else
    cin.ignore();
    cout << "Press any key to continue ...\n";
    cin.get();
#endif
    return 0;
}