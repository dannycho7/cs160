#pragma once

#include "helpers.hpp"

#include <climits>

using namespace std;

// SCANNER DEFINITION

// You can freely add member fields and functions to this class.
class Scanner {
    int line;
    int value;
    size_t data_i;
    string data;
public:
    // You really need to implement these four methods for the scanner to work.
    Token nextToken();
    void eatToken(Token);
    int lineNumber();
    int getNumberValue();
    
    Scanner();
};

// PARSER DEFINITION

// You can freely add member fields and functions to this class.
class Parser {
    Scanner scanner;
    
    bool evaluate;
    Token lookahead;

    // You will need to define the recursive descent functions you're going to use here.
    void match(Token t);

    void start();
    void exprlist();
    void exprlist_p();
    void expression();
    void expression_p();
    void term();
    void term_p();
    void val();
public:
    void parse();
    
    Parser(bool);
};
