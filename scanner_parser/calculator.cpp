
#include "calculator.hpp"
#include <string>
#include <cmath>
#include <iostream>

using namespace std;

// Scanner implementation

// You may have to modify this constructor, although it might not be neccessary.
Scanner::Scanner() : line(1), 
                     value(0),
                     data_i(0) {
    string buf;
    while (getline(cin, buf)) {
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
                    if (data_i + 1 >= data.length() && isdigit(data[data_i + 1])) {
                        scanError(line, data[data_i + 1]);
                    } else {
                        value = 0;
                        ret = T_NUMBER;
                    }
                } else {
                    size_t j = data_i;
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
            case T_NEWLN:
                line++;
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

void Parser::match(Token t) {
    scanner.eatToken(t);
    lookahead = scanner.nextToken();
}

void Parser::start() {
    // I am a placeholder. Implement a recursive descent parser starting from me. Add other methods for different recursive steps.
    // Depending on how you designed your grammar, how many levels of operator precedence you need, etc., you might end up with more
    // or less steps in the process.
    lookahead = scanner.nextToken();
    exprlist();
    scanner.eatToken(T_EOF);
}

// You will need to add more methods for this to work. Don't forget to also define them in calculator.hpp!
void Parser::exprlist() {
    expression();
    exprlist_p();
}

void Parser::exprlist_p() {
    switch (lookahead) {
        case T_SEMICOLON:
            match(T_SEMICOLON);
            if (T_NEWLN == scanner.nextToken())
                match(T_NEWLN);
            expression();
            exprlist_p();
            break;
        case T_EOF:
            break;
        default:
            parseError(scanner.lineNumber(), lookahead);
    }
}
void Parser::expression() {
    term();
    expression_p();
}

void Parser::expression_p() {
    switch (lookahead) {
        case T_PLUS:
        case T_MINUS:
            match(lookahead);
            term();
            expression_p();
            break;
        case T_CLOSEPAREN:
        case T_SEMICOLON:
        case T_EOF:
            break;
        default:
            parseError(scanner.lineNumber(), lookahead);
    }

}
void Parser::term() {
    val();
    term_p();
}

void Parser::term_p() {
    switch (lookahead) {
        case T_MULTIPLY:
        case T_DIVIDE:
        case T_MODULO:
            match(lookahead);
            val();
            term_p();
            break;
        case T_PLUS:
        case T_MINUS:
        case T_CLOSEPAREN:
        case T_SEMICOLON:
        case T_EOF:
            break;
        default:
            parseError(scanner.lineNumber(), lookahead);
    }
}

void Parser::val() {
    switch (lookahead) {
        case T_OPENPAREN:
            match(T_OPENPAREN);
            expression();
            match(T_CLOSEPAREN);
            break;
        case T_NUMBER:
            match(T_NUMBER);
            break;
        default:
            parseError(scanner.lineNumber(), lookahead);
    }
}
