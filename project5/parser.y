%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <iostream>

    #include "ast.hpp"
    
    #define YYDEBUG 1
    #define YYINITDEPTH 10000
    int yylex(void);
    void yyerror(const char *);
    
    extern ASTNode* astRoot;
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

%type <integer_ptr> T_TRUE T_FALSE T_INTEGER
%type <program_ptr> Start
%type <class_list_ptr> ClassDeclList
%type <class_ptr> ClassDecl
%type <identifier_ptr> T_IDENTIFIER OptExtend OptDot
%type <declaration_list_ptr> MemberList DeclList
%type <declaration_ptr> Member Decl
%type <type_ptr> Type ReturnType;
%type <method_list_ptr> MethodList
%type <method_ptr> Method
%type <parameter_list_ptr> ParameterList AddParameterList
%type <parameter_ptr> Parameter
%type <methodbody_ptr> MethodDefn Body
%type <identifier_list_ptr> DeclIdentifierList
%type <statement_list_ptr> StmtList OptElse Block
%type <statement_ptr> Stmt
%type <returnstatement_ptr> OptReturn
%type <expression_ptr> Expression
%type <methodcall_ptr> MethodCall
%type <expression_list_ptr> ArgumentList ArgumentList_p

%%
Start : ClassDeclList { $$ = new ProgramNode($1); astRoot = $$; }
      ;
ClassDeclList : ClassDeclList ClassDecl { $$ = $1; $$->push_back($2); }
              | ClassDecl { $$ = new std::list<ClassNode*>(); $$->push_back($1); }
              ;
ClassDecl : T_IDENTIFIER OptExtend '{' MemberList MethodList '}' { $$ = new ClassNode($1, $2, $4, $5); }
          | T_IDENTIFIER OptExtend '{' MemberList '}' { $$ = new ClassNode($1, $2, $4, NULL); }
          | T_IDENTIFIER OptExtend '{' MethodList '}' { $$ = new ClassNode($1, $2, NULL, $4); }
          | T_IDENTIFIER OptExtend '{' '}' { $$ = new ClassNode($1, $2, NULL, NULL); }
          ;
OptExtend : T_EXTENDS T_IDENTIFIER { $$ = $2; }
          | %empty { $$ = NULL; }
          ;
MemberList : MemberList Member { $$ = $1; $$->push_back($2); }
           | Member { $$ = new std::list<DeclarationNode*>(); $$->push_back($1); }
           ;
Member : Type T_IDENTIFIER ';'  { std::list<IdentifierNode*>* decl_list = new std::list<IdentifierNode*>();
                                  decl_list->push_back($2);
                                  $$ = new DeclarationNode($1, decl_list);
       }
       ;

Type : T_IDENTIFIER { $$ = new ObjectTypeNode($1); }
     | T_INTEGER_TYPE { $$ = new IntegerTypeNode(); }
     | T_BOOLEAN_TYPE { $$ = new BooleanTypeNode(); }
     ;
ReturnType : Type { $$ = $1; }
           | T_NONE { $$ = new NoneNode(); }
           ;
MethodList : MethodList Method { $$ = $1; $$->push_back($2); }
           | Method { $$ = new std::list<MethodNode*>(); $$->push_back($1); }
           ;
Method : T_IDENTIFIER '(' ParameterList ')' T_ARROW ReturnType MethodDefn { $$ = new MethodNode($1, $3, $6, $7); }
       ;
ParameterList : AddParameterList { $$ = $1; }
              | %empty { $$ = new std::list<ParameterNode*>(); }
              ;
AddParameterList : AddParameterList ',' Parameter { $$ = $1; $$->push_back($3); }
                 | Parameter { $$ = new std::list<ParameterNode*>(); $$->push_back($1); }
                 ;
Parameter : Type T_IDENTIFIER { $$ = new ParameterNode($1, $2); }
          ;
MethodDefn : '{' Body '}' { $$ = $2; }
           ;
Body : DeclList StmtList OptReturn { $$ = new MethodBodyNode($1, $2, $3); }
     | DeclList OptReturn { $$ = new MethodBodyNode($1, NULL, $2); }
     | StmtList OptReturn { $$ = new MethodBodyNode(NULL, $1, $2); }
     | OptReturn { $$ = new MethodBodyNode(NULL, NULL, $1); }
     ;
DeclList : DeclList Decl { $$ = $1; $$->push_back($2); }
         | Decl { $$ = new std::list<DeclarationNode*>(); $$->push_back($1); }
         ;
Decl : Type DeclIdentifierList ';' { $$ = new DeclarationNode($1, $2); }
     ;
DeclIdentifierList : DeclIdentifierList ',' T_IDENTIFIER { $$ = $1; $$->push_back($3); }
                   | T_IDENTIFIER { $$ = new std::list<IdentifierNode*>(); $$->push_back($1); }
                   ;
StmtList : StmtList Stmt { $$ = $1; $$->push_back($2); }
         | Stmt { $$ = new std::list<StatementNode*>(); $$->push_back($1); }
         ;
Stmt : T_IDENTIFIER OptDot '=' Expression ';' { $$ = new AssignmentNode($1, $2, $4); }
     | MethodCall ';' { $$ = new CallNode($1); }
     | T_IF Expression '{' Block '}' OptElse { $$ = new IfElseNode($2, $4, $6); }
     | T_WHILE Expression '{' Block '}' { $$ = new WhileNode($2, $4); }
     | T_DO '{' Block '}' T_WHILE '(' Expression ')' ';' { $$ = new DoWhileNode($3, $7); }
     | T_PRINT Expression ';' { $$ = new PrintNode($2); }
     ;
OptDot : '.' T_IDENTIFIER { $$ = $2; }
       | %empty { $$ = NULL; }
       ;
OptElse : T_ELSE '{' Block '}' { $$ = $3; }
        | %empty { $$ = NULL; }
        ;
OptReturn : T_RETURN Expression ';' { $$ = new ReturnStatementNode($2); }
          | %empty { $$ = NULL; }
          ;
Block : StmtList { $$ = $1; }
      ;
Expression : Expression '+' Expression { $$ = new PlusNode($1, $3); }
           | Expression '-' Expression { $$ = new MinusNode($1, $3); }
           | Expression '*' Expression { $$ = new TimesNode($1, $3); }
           | Expression '/' Expression { $$ = new DivideNode($1, $3); }
           | Expression '>' Expression { $$ = new GreaterNode($1, $3); }
           | Expression T_GEQ Expression { $$ = new GreaterEqualNode($1, $3); }
           | Expression T_EQUALS Expression { $$ = new EqualNode($1, $3); }
           | Expression T_AND Expression { $$ = new AndNode($1, $3); }
           | Expression T_OR Expression { $$ = new OrNode($1, $3); }
           | T_NOT Expression { $$ = new NotNode($2); }
           | '-' Expression %prec T_U_MINUS { $$ = new NegationNode($2); }
           | T_IDENTIFIER { $$ = new VariableNode($1); }
           | T_IDENTIFIER '.' T_IDENTIFIER { $$ = new MemberAccessNode($1, $3); }
           | MethodCall { $$ = $1; }
           | '(' Expression ')' { $$ = $2; }
           | T_INTEGER { $$ = new IntegerLiteralNode($1); }
           | T_TRUE { $$ = new BooleanLiteralNode($1); }
           | T_FALSE { $$ = new BooleanLiteralNode($1); }
           | T_NEW T_IDENTIFIER { $$ = new NewNode($2, new std::list<ExpressionNode*>()); }
           | T_NEW T_IDENTIFIER '(' ArgumentList ')' { $$ = new NewNode($2, $4); }
           ;
MethodCall : T_IDENTIFIER OptDot '(' ArgumentList ')' { $$ = new MethodCallNode($1, $2, $4); }
           ;
ArgumentList : ArgumentList_p { $$ = $1; }
             | %empty { $$ = new std::list<ExpressionNode*>(); }
             ;
ArgumentList_p : ArgumentList_p ',' Expression { $$ = $1; $$->push_back($3); }
               | Expression { $$ = new std::list<ExpressionNode*>(); $$->push_back($1); }
               ;
%%

extern int yylineno;

void yyerror(const char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  exit(0);
}
