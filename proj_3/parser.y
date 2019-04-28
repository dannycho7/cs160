%{
    #include <cstdlib>
    #include <cstdio>
    #include <iostream>

    #define YYDEBUG 1

    int yylex(void);
    void yyerror(const char *);
%}

%error-verbose

/* WRITEME: List all your tokens here */
%token T_RETURN T_IF T_ELSE T_WHILE T_DO
%token T_PRINT T_NEW T_EXTENDS
%token T_INTEGER_TYPE T_BOOLEAN_TYPE T_NONE
%token T_EQUALS T_AND T_OR T_NOT T_GEQ
%token T_TRUE T_FALSE T_INTEGER T_IDENTIFIER
/* WRITEME: Specify precedence here */

%%

/* WRITEME: This rule is a placeholder, since Bison requires
            at least one rule to run successfully. Replace
            this with your appropriate start rules. */
Start :
      ;

/* WRITME: Write your Bison grammar specification here */

%%

extern int yylineno;

void yyerror(const char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  exit(1);
}
