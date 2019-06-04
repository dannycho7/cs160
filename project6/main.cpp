#include "ast.hpp"
#include "typecheck.hpp"
#include "codegeneration.hpp"
#include "parser.hpp"

extern int yydebug;
extern int yyparse();

ASTNode* astRoot;

int main(void) {
    yydebug = 0; // Set this to 1 if you want the parser to output debug information and parse process
    
    astRoot = NULL;
    
    yyparse();
    
    if (astRoot) {
        TypeCheck* typecheck = new TypeCheck();
        astRoot->accept(typecheck);
        ClassTable* classTable = typecheck->classTable;
        if (classTable) {
            // Uncomment the following line to print the class table after it is generated
            //print(*classTable);
            CodeGenerator* codegen = new CodeGenerator();
            codegen->classTable = classTable;
            astRoot->accept(codegen);
        }
    }

    return 0;
}
