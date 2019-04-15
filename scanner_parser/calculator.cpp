
#include "calculator.hpp"
#include <string>
#include <cmath>
#include <iostream>

using namespace std;

// Scanner implementation

// You may have to modify this constructor, although it might not be neccessary.
Scanner::Scanner() : line(0), 
                     value(0),
                     data_i(0) {
    string buf;
    while (getline(cin, buf)) {
        cerr << "Buf: " << buf << '\n' << endl;
        if (data.length() != 0)
            data += '\n';
        data += buf;
    }
}

// You need to fill this method with the appropriate code for it to work as described in the project description.
Token Scanner::nextToken() {
    // I am a placeholder. Change me into something that can actually decypher the next token without consuming it.
    while (data_i < data.length() && data[data_i] == ' ')
        data_i++;

    Token ret {T_EOF};

    if (data_i == data.length())
        return ret;

    switch (data[data_i]) {
        case '+':
            ret = T_PLUS;
            break;
        case '-':
            ret = T_MINUS;
            break;
        case '*':
            ret = T_MULTIPLY;
            break;
        case '/':
            ret = T_DIVIDE;
            break;
        case 'm':
            if (data_i + 1 >= data.length()) {
                scanError(line, data[data_i]);
            } else {
                if (data[data_i + 1] == 'o') {
                    if (data_i + 2 >= data.length()) {
                        scanError(line, data[data_i + 1]);
                    } else {
                        if (data[data_i + 2 == 'd']) {
                            ret = T_MODULO;
                        } else {
                            scanError(line, data[data_i + 2]);
                        }
                    }
                } else {
                    scanError(line, data[data_i + 1]);
                }
            }
            break;
        case '(':
            ret = T_OPENPAREN;
            break;
        case ')':
            ret = T_CLOSEPAREN;
            break;
        case ';':
            ret = T_SEMICOLON;
            break;
        case '\n':
            ret = T_NEWLN;
            break;
        default:
            if (isdigit(data[data_i])) {
                if (data[data_i] == '0') {
                    if (data_i + 1 >= data.length()) {
                        scanError(line, data[data_i]);
                    } else {
                        if (isdigit(data[data_i + 1])) {
                            scanError(line, data[data_i + 1]);
                        } else {
                            value = 0;
                            ret = T_NUMBER;
                        }
                    }
                } else {
                    int j = data_i;
                    while (j < data.length() && isdigit(data[j]))
                        j++;
                    value = stoi(data.substr(data_i, j - data_i + 1));
                    ret = T_NUMBER;
                }
            } else {
                scanError(line, data[data_i]);
            }
    }

    return ret;
}

// You need to fill this method with the appropriate code for it to work as described in the project description.
void Scanner::eatToken(Token toConsume) {
    // I am a placeholder. I'm not even fun. Change me into something that can actually consume tokens!
    Token found = nextToken();
    if (found == toConsume) {
        switch (found) {
            case T_NUMBER:
                data_i += to_string(value).length();
                break;
            case T_MODULO:
                data_i += 3;
                break;
            default:
                data_i++;
        }
    } else {
        mismatchError(line, toConsume, found);
    }
}

int Scanner::lineNumber() {
    return this->line;
}

int Scanner::getNumberValue() {
    return this->value;
}

// Parser implementation

// You may need to modify this constructor and make it do stuff, although it might not be neccessary.
Parser::Parser(bool eval) : evaluate(eval) {
    // WRITEME
}

void Parser::parse() {
    start();
}

void Parser::start() {
    // I am a placeholder. Implement a recursive descent parser starting from me. Add other methods for different recursive steps.
    // Depending on how you designed your grammar, how many levels of operator precedence you need, etc., you might end up with more
    // or less steps in the process.
    //
    // WRITEME
}

// You will need to add more methods for this to work. Don't forget to also define them in calculator.hpp!
// WRITEME

