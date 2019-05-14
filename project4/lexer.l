%option yylineno
%pointer

%{
    #include <stdlib.h>
    #include <errno.h>
    #include <limits.h>
    #include "ast.hpp"
    #include "parser.hpp"
    void yyerror(const char *);
%}

%x comment

%%
[[:space:]]              /* ignore whitespace */
"/*"                    { BEGIN(comment); }
<comment>.|\n           /* ignore any character */
<comment><<EOF>>        { yyerror("dangling comment"); }
<comment>"*/"           { BEGIN(INITIAL); }
[+\-*/.>{}()=;,]        { return *yytext; }
"return"                { return T_RETURN; }
"if"                    { return T_IF; }
"else"                  { return T_ELSE; }
"while"                 { return T_WHILE; }
"do"                    { return T_DO; }
"print"                 { return T_PRINT; }
"new"                   { return T_NEW; }
"extends"               { return T_EXTENDS; }
"->"                    { return T_ARROW; }
"integer"               { return T_INTEGER_TYPE; }
"boolean"               { return T_BOOLEAN_TYPE; }
"none"                  { return T_NONE; }
"equals"                { return T_EQUALS; }
"and"                   { return T_AND; }
"or"                    { return T_OR; }
"not"                   { return T_NOT; }
">="                    { return T_GEQ; }
"true"                  { yylval.integer_ptr = new IntegerNode(1); return T_TRUE; }
"false"                 { yylval.integer_ptr = new IntegerNode(0); return T_FALSE; }
0|[1-9][0-9]*           { yylval.integer_ptr = new IntegerNode(atoi(yytext)); return T_INTEGER; }
[[:alpha:]][[:alnum:]]* { yylval.identifier_ptr = new IdentifierNode(yytext); return T_IDENTIFIER; }
.                       { yyerror("invalid character"); }
%%

int yywrap(void) {
  return 1;
}
