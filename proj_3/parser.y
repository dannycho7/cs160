%{
    #include <cstdlib>
    #include <cstdio>
    #include <iostream>

    #define YYDEBUG 1

    int yylex(void);
    void yyerror(const char *);
%}

%error-verbose

%token T_RETURN T_IF T_ELSE T_WHILE T_DO
%token T_PRINT T_NEW T_EXTENDS
%token T_ARROW T_INTEGER_TYPE T_BOOLEAN_TYPE T_NONE
%token T_EQUALS T_AND T_OR T_NOT T_GEQ
%token T_TRUE T_FALSE T_INTEGER T_IDENTIFIER

%left T_OR
%left T_AND
%left '>' T_GEQ T_EQUALS
%left '+' '-'
%left '*' '/'
%precedence T_NOT T_U_MINUS

%%

Start : ClassDecl ClassDeclList;
ClassDeclList : ClassDecl ClassDeclList | %empty;
ClassDecl : T_IDENTIFIER OptExtend ClassDefn;
OptExtend : T_EXTENDS T_IDENTIFIER | %empty;
ClassDefn : '{' MemberList '}';
MemberList : Member MemberList | MethodList;
Member : Type T_IDENTIFIER ';';
Type : T_IDENTIFIER | T_INTEGER_TYPE | T_BOOLEAN_TYPE;
ReturnType : Type | T_NONE;
MethodList : Method MethodList | %empty;
Method : T_IDENTIFIER '(' ParameterList ')' T_ARROW ReturnType MethodDefn;
ParameterList : Parameter AddParameterList | %empty;
AddParameterList : ',' Parameter AddParameterList | %empty;
Parameter : Type T_IDENTIFIER;
MethodDefn : '{' Body '}';
Body : DeclList;
DeclList : Decl DeclList | StmtList;
Decl : Type T_IDENTIFIER AddDeclIdentifierList ';';
AddDeclIdentifierList : ',' T_IDENTIFIER AddDeclIdentifierList | %empty;
StmtList : Stmt StmtList | OptReturn;
Stmt : T_IDENTIFIER OptDot '=' Expression ';'
     | MethodCall ';'
     | T_IF Expression '{' Block '}' OptElse
     | T_WHILE Expression '{' Block '}'
     | T_DO '{' Block '}' T_WHILE '(' Expression ')' ';'
     | T_PRINT Expression ';'
     ;
OptDot : '.' T_IDENTIFIER | %empty;
OptElse: T_ELSE '{' Block '}' | %empty;
OptReturn : T_RETURN Expression ';' | %empty;
Block : Stmt StmtList;

Expression : Expression '+' Expression
           | Expression '-' Expression
           | Expression '*' Expression
           | Expression '/' Expression
           | Expression '>' Expression
           | Expression T_GEQ Expression
           | Expression T_EQUALS Expression
           | Expression T_AND Expression
           | Expression T_OR Expression
           | T_NOT Expression
           | '-' Expression %prec T_U_MINUS
           | T_IDENTIFIER
           | T_IDENTIFIER '.' T_IDENTIFIER
           | MethodCall
           | '(' Expression ')'
           | T_INTEGER
           | T_TRUE
           | T_FALSE
           | T_NEW T_IDENTIFIER
           | T_NEW T_IDENTIFIER '(' ArgumentList ')'
           ;
MethodCall : T_IDENTIFIER '(' ArgumentList ')'
           | T_IDENTIFIER '.' T_IDENTIFIER '(' ArgumentList ')'
           ;
ArgumentList : ArgumentList_p
          | %empty
          ;
ArgumentList_p : ArgumentList_p ',' Expression
           | Expression
           ;
%%

extern int yylineno;

void yyerror(const char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  exit(1);
}
