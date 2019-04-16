#pragma once

#include "helpers.hpp"

#include <climits>
#include <vector>

using namespace std;

// SCANNER DEFINITION

// You can freely add member fields and functions to this class.
class Scanner {
    int line;
    int value;
    string value_s;
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
    vector<int> results;

    // You will need to define the recursive descent functions you're going to use here.
    void match(Token t);

    void start();
    void exprlist();
    void exprlist_p();
    int expression();
    int expression_p(int p_val);
    int term();
    int term_p(int p_val);
    int val();
public:
    void parse();
    
    Parser(bool);
};
