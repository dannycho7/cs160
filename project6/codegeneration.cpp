#include "codegeneration.hpp"

static int getClassOffset(ClassTable* ct, std::string className) {
    int offset = 0;
    std::string superClassName = (*ct)[className].superClassName;
    while (!superClassName.empty()) {
        offset += (*ct)[superClassName].membersSize;
        superClassName = (*ct)[superClassName].superClassName;
    }
    return offset;
}

static int getClassSize(ClassTable* ct, std::string className) {
    return getClassOffset(ct, className) + (*ct)[className].membersSize;
}

static std::string findNameInClass(ClassTable* ct, std::string className, std::string name, void* (*findInClassInfo)(ClassInfo, std::string)) {
  void* vi = NULL;
  ClassInfo ci;
  while (true) {
    ci = ct->find(className)->second;
    vi = findInClassInfo(ci, name);
    if (vi != NULL)
        break;
    className = ci.superClassName;
  }
  return className;
}

static VariableInfo* findVariable(VariableTable* vt, std::string variableName) {
  std::map<std::string, VariableInfo>::iterator vt_it = vt->find(variableName);
  return (vt_it == vt->end()) ? NULL : &(vt_it->second);
}

static void* _findMemberInCI(ClassInfo ci, std::string memberName) {
  return findVariable(ci.members, memberName);
}

static int getMemberOffsetInClass(ClassTable* ct, std::string className, std::string memberName) {
    className = findNameInClass(ct, className, memberName, _findMemberInCI);
    return getClassOffset(ct, className) + (*((*ct)[className].members))[memberName].offset;
}

static int getVariableOffsetInMethod(ClassTable* ct, std::string className, std::string methodName, std::string memberName) {
    VariableTable* method_vt = (*(*ct)[className].methods)[methodName].variables;
    std::map<std::string, VariableInfo>::iterator vi_it =  method_vt->find(memberName);
    return vi_it->second.offset;
}

static bool isVariableLocal(ClassTable* ct, std::string className, std::string methodName, std::string memberName) {
    VariableTable* method_vt = (*(*ct)[className].methods)[methodName].variables;
    std::map<std::string, VariableInfo>::iterator vi_it =  method_vt->find(memberName);
    return vi_it != method_vt->end();
}

static void* _findMethodInCI(ClassInfo ci, std::string methodName) {
  MethodTable* mt = ci.methods;
  std::map<std::string, MethodInfo>::iterator mt_it = mt->find(methodName);
  return (mt_it == mt->end()) ? NULL : &(mt_it->second);
}

static std::string getMethodLabelInClass(ClassTable* ct, std::string className, std::string methodName) {
    className = findNameInClass(ct, className, methodName, _findMethodInCI);
    return className + "_" + methodName;
}

// CodeGenerator Visitor Functions: These are the functions
// you will complete to generate the x86 assembly code. Not
// all functions must have code, many may be left empty.

void CodeGenerator::visitProgramNode(ProgramNode* node) {
    std::cout << ".data" << std::endl;
    std::cout << "printstr: .asciz \"%d\\n\"" << std::endl;
    std::cout << ".globl Main_main" << std::endl;
    std::cout << ".text" << std::endl;
    node->visit_children(this);
}

void CodeGenerator::visitClassNode(ClassNode* node) {
    this->currentClassName = node->identifier_1->name;
    this->currentClassInfo = (*this->classTable)[this->currentClassName];
    node->visit_children(this);
}

void CodeGenerator::visitMethodNode(MethodNode* node) {
    this->currentMethodName = node->identifier->name;
    this->currentMethodInfo = (*this->currentClassInfo.methods)[this->currentMethodName];
    std::cout << "# PROLOGUE START" << std::endl;
    std::cout << getMethodLabelInClass(this->classTable, this->currentClassName, this->currentMethodName) << ":" << std::endl;
    std::cout << "  push %ebp" << std::endl;
    std::cout << "  mov %esp, %ebp" << std::endl;
    std::cout << "  sub $" << this->currentMethodInfo.localsSize << ", %esp" << std::endl;
    std::cout << "  push %ebx" << std::endl;
    std::cout << "  push %edi" << std::endl;
    std::cout << "  push %esi" << std::endl;
    std::cout << "# PROLOGUE END" << std::endl;
    node->visit_children(this);
    std::cout << "# EPILOGUE START" << std::endl;
    std::cout << "  pop %esi" << std::endl;
    std::cout << "  pop %edi" << std::endl;
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  mov %ebp, %esp" << std::endl;
    std::cout << "  pop %ebp" << std::endl;
    std::cout << "  ret" << std::endl;
    std::cout << "# EPILOGUE END" << std::endl;
}

void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {
    std::cout << "# METHOD BODY START" << std::endl;
    node->visit_children(this);
    std::cout << "# METHOD BODY END" << std::endl;
}

void CodeGenerator::visitParameterNode(ParameterNode* node) {}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {
    node->visit_children(this);
    std::cout << "  pop %eax" << std::endl;
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {
    std::cout << "# ASSIGNMENT START" << std::endl;
    node->visit_children(this);
    
    int offset;
    if (isVariableLocal(this->classTable, this->currentClassName, this->currentMethodName, node->identifier_1->name)) {
        offset = getVariableOffsetInMethod(this->classTable, this->currentClassName, this->currentMethodName, node->identifier_1->name);
        if (node->identifier_2) {
            std::cout << "  mov " << offset << "(%ebp), %eax" << std::endl;
        } else {
            std::cout << "  mov %ebp, %eax" << std::endl;
        }
    } else {
        int memberOffset = getMemberOffsetInClass(this->classTable, this->currentClassName, this->currentMethodName);
        std::cout << "  movl $" << memberOffset << " %ebx" << std::endl;
        std::cout << "  mov 8(%ebp), %eax" << std::endl;
        std::cout << "  add %ebx, %eax" << std::endl;     
    }
    if (node->identifier_2) {
        std::string memberClassName = (*this->currentMethodInfo.variables)[node->identifier_1->name].type.objectClassName;
        offset = getMemberOffsetInClass(this->classTable, memberClassName, node->identifier_2->name);
    }   
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  mov %ebx, " << offset << "(%eax)" << std::endl;
    std::cout << "# ASSIGNMENT END" << std::endl;
}

void CodeGenerator::visitCallNode(CallNode* node) {
    node->visit_children(this);
}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitWhileNode(WhileNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
    node->visit_children(this);
    std::cout << "  push $printstr" << std::endl;
    std::cout << "  call printf" << std::endl;
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitPlusNode(PlusNode* node) {
    std::cout << "# PLUS START" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  add %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "# PLUS END" << std::endl;
}

void CodeGenerator::visitMinusNode(MinusNode* node) {
    std::cout << "# SUBTRACT START" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  sub %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "# SUBTRACT END" << std::endl;
}

void CodeGenerator::visitTimesNode(TimesNode* node) {
    std::cout << "# MULTIPLY START" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  imul %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "# MULTIPLY END" << std::endl;
}

void CodeGenerator::visitDivideNode(DivideNode* node) {
    std::cout << "# DIVIDE START" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;    
    std::cout << "  cdq" << std::endl;
    std::cout << "  idiv %ebx" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "# DIVIDE END" << std::endl;
}

void CodeGenerator::visitGreaterNode(GreaterNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitEqualNode(EqualNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitAndNode(AndNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitOrNode(OrNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitNotNode(NotNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitMethodCallNode(MethodCallNode* node) {
    std::cout << "# PRE-CALL SEQUENCE START" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "  push %ecx" << std::endl;
    std::cout << "  push %edx" << std::endl;
    for (std::list<ExpressionNode*>::reverse_iterator en_it = node->expression_list->rbegin(); en_it != node->expression_list->rend(); en_it++)
        (*en_it)->accept(this);
    std::string objectClassName = this->currentClassName;
    std::string methodName = node->identifier_1->name;
    int obj_ebp_offset = 8;
    if (node->identifier_2) {
        std::string objectName = node->identifier_1->name;
        objectClassName = (*this->currentMethodInfo.variables)[objectName].type.objectClassName;
        methodName = node->identifier_2->name;
        obj_ebp_offset = getVariableOffsetInMethod(this->classTable, objectClassName, methodName, objectName);
    }
    std::cout << "  push " << obj_ebp_offset << "(%ebp)" << std::endl;
    std::cout << "# PRE-CALL SEQUENCE END" << std::endl;
    std::cout << "  call " << getMethodLabelInClass(this->classTable, objectClassName, methodName) << std::endl;
    std::cout << "# POST-RETURN SEQUENCE START" << std::endl;
    std::cout << "  add $" << 4 * node->expression_list->size() << ", %esp" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %ecx" << std::endl;
    std::cout << "  xchg %eax, (%esp)" << std::endl;
    std::cout << "# POST-RETURN SEQUENCE END" << std::endl;
}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitVariableNode(VariableNode* node) {
    std::string variableName = node->identifier->name;
    if (isVariableLocal(this->classTable, this->currentClassName, this->currentMethodName, variableName)) {
        int offset = getVariableOffsetInMethod(this->classTable, this->currentClassName, this->currentMethodName, variableName);
        std::cout << "  push " << offset << "(%ebp)" << std::endl;
    } else {

    }
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {
    node->visit_children(this);
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
    node->visit_children(this);
}

void CodeGenerator::visitNewNode(NewNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerTypeNode(IntegerTypeNode* node) {}

void CodeGenerator::visitBooleanTypeNode(BooleanTypeNode* node) {}

void CodeGenerator::visitObjectTypeNode(ObjectTypeNode* node) {}

void CodeGenerator::visitNoneNode(NoneNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIdentifierNode(IdentifierNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerNode(IntegerNode* node) {
    std::cout << "  push $" << node->value << std::endl;
}
