%option yylineno
%pointer

%{
    #include <cstdlib>
    #include <cerrno>
    #include <climits>
    #include <limits>
    #include "parser.hpp"
    
	void yyerror(const char *);
%}

/* WRITEME: Write any definitions here. You can find information on
            definitions here:
            ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html#SEC10 */

/* WRITEME: If you are going to use start conditions, write them here.
            You can find more information on start conditions here:
            ftp://ftp.gnu.org/old-gnu/Manuals/flex-2.5.4/html_mono/flex.html#SEC11
            You will probably want to use exclusive conditions rather
            than inclusive conditions (use the %x instead of the %s). */
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
"integer"               { return T_INTEGER_TYPE; }
"boolean"               { return T_BOOLEAN_TYPE; }
"none"                  { return T_NONE; }
"equals"                { return T_EQUALS; }
"and"                   { return T_AND; }
"or"                    { return T_OR; }
"not"                   { return T_NOT; }
">="                    { return T_GEQ; }
"true"                  { return T_TRUE; }
"false"                 { return T_FALSE; }
0|[1-9][0-9]*           { return T_INTEGER; }
[[:alpha:]][[:alnum:]]* { return T_IDENTIFIER; }
.                       { printf("%s\n", yytext); yyerror("invalid character"); }
%%

int yywrap(void) {
  return 1;
}
