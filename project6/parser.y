%{
    #include <cstdlib>
    #include <cstdio>
    #include <iostream>
    #include "ast.hpp"
    
    #define YYDEBUG 1
    #define YYINITDEPTH 10000
    
    int yylex(void);
    void yyerror(const char *);
    
    extern ASTNode* astRoot;
%}

%error-verbose
// %glr-parser
/* NOTE: You may use the %glr-parser directive, which may allow your parser to
         work even with some shift/reduce conflicts remianing. */

/* WRITEME: Copy your token and precedence specifiers from Project 5 here. */

/* WRITEME: Copy your type specifiers from Project 5 here. */

%%

/* WRITEME: This rule is a placeholder. Replace it with your grammar
            rules and actions from Project 5. */

Start :           { /* Empty action */ }
      ;

%%

extern int yylineno;

void yyerror(const char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  exit(1);
}