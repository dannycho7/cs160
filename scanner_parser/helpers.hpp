// DO NOT MODIFY THIS FILE IN ANY WAY!

#pragma once

#include <string>

// HELPER TYPES AND FUNCTIONS

// This is the token type, it contains all possible tokens in our language.

enum Token {
    T_PLUS,
    T_MINUS,
    T_MULTIPLY,
    T_DIVIDE,
    T_MODULO,
    T_OPENPAREN,
    T_CLOSEPAREN,
    T_NUMBER,
    T_SEMICOLON,
    T_NEWLN,
    T_EOF
};

// This function will convert a token to a string, for display.
std::string tokenToString(Token toConvert);

// Throw this error when the parser expects a given token from the scanner
// and the next token the scanner finds does not match.
void mismatchError(int line, Token expected, Token found);

// Throw this error when the parser encounters a token that is not valid
// at the beginning of a specific grammar rule.
void parseError(int line, Token found);

// Throw this error when an invalid character is found in the input, or
// if a bad character is found as part of a print token.
void scanError(int line, char badCharacter);

// Throw this error when a number is out of bounds, which means greater than signed INT_MAX.
void outOfBoundsError(int line, long number);

// Throw this error when a number is divided by 0.
void divideByZeroError(int line, long number);
