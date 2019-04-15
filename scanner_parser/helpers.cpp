// DO NOT MODIFY THIS FILE IN ANY WAY!

#include "helpers.hpp"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

std::string tokenToString(Token toConvert) {
    switch (toConvert) {
        case T_PLUS:
            return std::string("+");
        case T_MINUS:
            return std::string("-");
        case T_MULTIPLY:
            return std::string("*");
        case T_DIVIDE:
            return std::string("/");
        case T_MODULO:
            return std::string("mod");
        case T_OPENPAREN:
            return std::string("(");
        case T_CLOSEPAREN:
            return std::string(")");
        case T_SEMICOLON:
            return std::string("Semicolon");
        case T_NUMBER:
            return std::string("number");
        case T_EOF:
            return std::string("EOF");
        case T_NEWLN:
            return std::string("Newline");
        default:
            return std::string("Unknown");
    }
}

void mismatchError(int line, Token expected, Token found) {
    std::cerr << "Parse error: expected " << tokenToString(expected) << " found " << tokenToString(found) << " at line " << line << std::endl;
    exit(1);
}

void parseError(int line, Token found) {
    std::cerr << "Parse error: found invalid token " << tokenToString(found) << " at line " << line << std::endl;
    exit(1);
}

void scanError(int line, char badCharacter) {
    std::cerr << "Scan error: found invalid character " << badCharacter << " at line " << line << std::endl;
    exit(1);
}

void outOfBoundsError(int line, long number) {
    std::cerr << "Semantic error: number " << number << " out of bounds at line " << line << std::endl;
    exit(1);
}

void divideByZeroError(int line, long number) {
    std::cerr << "Semantic error: dividing " << number << " with zero at line " << line << std::endl;
    exit(1);
}
