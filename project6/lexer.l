%option yylineno
%pointer

%{
    #include <cstdlib>
    #include <cerrno>
    #include <climits>
    #include <limits>
    #include "ast.hpp"
    #include "parser.hpp"
    
	void yyerror(const char *);
%}

/* WRITEME: Copy any definitions and start conditions from Project 5 here. */

%%

  /* WRITEME: Copy your Flex rules and actions from Project 5 here. */

.                 { yyerror("invalid character"); }

%%

int yywrap(void) {
  return 1;
}
