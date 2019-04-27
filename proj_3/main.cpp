#include "parser.hpp"

#include <iostream>
#include <string>

extern int yydebug;
extern int yyparse();
extern int yylex (void);

int main(int argc, char **argv) {
  yydebug = (argc == 2 && argv[1] == std::string("-d")) ? 1 : 0;
  
  if (argc == 2 && argv[1] == std::string("-s")) {
    int t;
    while ((t = yylex())) std::cout << t << std::endl;
      return 0;
  }
  
  yyparse();
  
  return 0;
}
