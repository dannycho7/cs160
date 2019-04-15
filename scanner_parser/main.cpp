// DO NOT MODIFY THIS FILE IN ANY WAY!

#include <iostream>
#include <cstdlib>
#include <cstring>

#include "calculator.hpp"

int main(int argc, char* argv[]) {
    if(argc == 2) {
        if(strcmp(argv[1], "-s") == 0) {
            Scanner scanner;
            Token t;

            std::string result = "";
            while ((t = scanner.nextToken()) != T_EOF) {
                if(t == T_SEMICOLON) {
                    scanner.eatToken(t);
                    scanner.nextToken();
                    continue;  
                } 

                result +=  " " + tokenToString(t);
                if(t == T_NUMBER) result += std::to_string(scanner.getNumberValue());
                scanner.eatToken(t);
            }

            std::cout << result << std::endl;
        } else if(strcmp(argv[1], "-e") == 0) {
            Parser parser(true);
            parser.parse();
        }
    } else {
        Parser parser(false);
        parser.parse();
    }

    return 0;
}
