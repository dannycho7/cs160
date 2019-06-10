#include "codegeneration.hpp"

static std::string getLabelName(int currentLabel) {
    std::stringstream ss;
    ss << "BRANCHLABEL_" << currentLabel;
    return ss.str();
}

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

static std::string getMemberClassTypeInClass(ClassTable* ct, std::string className, std::string memberName) {
    std::string ownerClassName = findNameInClass(ct, className, memberName, _findMemberInCI);
    return (*(*ct)[ownerClassName].members)[memberName].type.objectClassName;
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
            std::string memberClassName = (*this->currentMethodInfo.variables)[node->identifier_1->name].type.objectClassName;
            offset = getMemberOffsetInClass(this->classTable, memberClassName, node->identifier_2->name);
        } else {
            std::cout << "  mov %ebp, %eax" << std::endl;
        }
    } else {
        offset = getMemberOffsetInClass(this->classTable, this->currentClassName, node->identifier_1->name);
        std::cout << "  mov 8(%ebp), %eax" << std::endl;
        if (node->identifier_2) {
            std::cout << "  mov " << offset << "(%eax), %eax" << std::endl;
            std::string memberClassName = getMemberClassTypeInClass(this->classTable, this->currentClassName, node->identifier_1->name);
            offset = getMemberOffsetInClass(this->classTable, memberClassName, node->identifier_2->name);
        }
    }
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  mov %ebx, " << offset << "(%eax)" << std::endl;
    std::cout << "# ASSIGNMENT END" << std::endl;
}

void CodeGenerator::visitCallNode(CallNode* node) {
    node->visit_children(this);
    std::cout << "  pop %eax" << std::endl;
}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
    std::cout << "# IF ELSE START" << std::endl;
    node->expression->accept(this);
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  mov $0, %ebx" << std::endl;
    std::cout << "  cmp %eax, %ebx" << std::endl;
    std::string branchLabelName =  getLabelName(this->nextLabel());
    std::cout << "  je " << branchLabelName << std::endl;
    for (std::list<StatementNode*>::iterator sn_it = node->statement_list_1->begin(); sn_it != node->statement_list_1->end(); sn_it++)
        (*sn_it)->accept(this);
    std::string skipBranchLabelName = getLabelName(this->nextLabel());
    std::cout << "  jmp " << skipBranchLabelName << std::endl;
    std::cout << branchLabelName << ":" << std::endl;
    for (std::list<StatementNode*>::iterator sn_it = node->statement_list_2->begin(); sn_it != node->statement_list_2->end(); sn_it++)
        (*sn_it)->accept(this);
    std::cout << skipBranchLabelName << ":" << std::endl;
    std::cout << "# IF ELSE END" << std::endl;
}

void CodeGenerator::visitWhileNode(WhileNode* node) {
    std::cout << "# WHILE START" << std::endl;
    std::string beginLoopLabelName = getLabelName(this->nextLabel());
    std::cout << beginLoopLabelName << ":" << std::endl;
    node->expression->accept(this);
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  mov $0, %ebx" << std::endl;
    std::cout << "  cmp %eax, %ebx" << std::endl;
    std::string endLoopLabelName = getLabelName(this->nextLabel());
    std::cout << "  je " << endLoopLabelName << std::endl;
    for (std::list<StatementNode*>::iterator sn_it = node->statement_list->begin(); sn_it != node->statement_list->end(); sn_it++)
        (*sn_it)->accept(this);
    std::cout << "  jmp " << beginLoopLabelName << std::endl;
    std::cout << endLoopLabelName << ":" << std::endl;
    std::cout << "# WHILE END" << std::endl;
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
    node->visit_children(this);
    std::cout << "  push $printstr" << std::endl;
    std::cout << "  call printf" << std::endl;
    std::cout << "  add $8, %esp" << std::endl;
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {
    std::cout << "# DO WHILE START" << std::endl;
    std::string beginLoopLabelName = getLabelName(this->nextLabel());
    std::cout << beginLoopLabelName << ":" << std::endl;
    for (std::list<StatementNode*>::iterator sn_it = node->statement_list->begin(); sn_it != node->statement_list->end(); sn_it++)
        (*sn_it)->accept(this);
    node->expression->accept(this);
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  mov $0, %ebx" << std::endl;
    std::cout << "  cmp %eax, %ebx" << std::endl;
    std::string endLoopLabelName = getLabelName(this->nextLabel());
    std::cout << "  je " << endLoopLabelName << std::endl;
    std::cout << "  jmp " << beginLoopLabelName << std::endl;
    std::cout << endLoopLabelName << ":" << std::endl;
    std::cout << "# DO WHILE START" << std::endl;
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
    std::cout << "# GREATER START" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  mov $0, %edx" << std::endl;
    std::cout << "  cmp %ebx, %eax" << std::endl;
    std::cout << "  setg %dl" << std::endl;
    std::cout << "  push %edx" << std::endl;
    std::cout << "# GREATER END" << std::endl;
}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {
    std::cout << "# GREATER EQUAL START" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  mov $0, %edx" << std::endl;
    std::cout << "  cmp %ebx, %eax" << std::endl;
    std::cout << "  setge %dl" << std::endl;
    std::cout << "  push %edx" << std::endl;
    std::cout << "# GREATER EQUAL END" << std::endl;
}

void CodeGenerator::visitEqualNode(EqualNode* node) {
    std::cout << "# EQUAL START" << std::endl;
    node->visit_children(this);    
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  mov $0, %edx" << std::endl;
    std::cout << "  cmp %ebx, %eax" << std::endl;
    std::cout << "  sete %dl" << std::endl;
    std::cout << "  push %edx" << std::endl;
    std::cout << "# EQUAL END" << std::endl;
}

void CodeGenerator::visitAndNode(AndNode* node) {
    std::cout << "# AND START" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;   
    std::cout << "  and %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "# AND END" << std::endl;
}

void CodeGenerator::visitOrNode(OrNode* node) {
    std::cout << "# OR START" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %ebx" << std::endl;
    std::cout << "  pop %eax" << std::endl;   
    std::cout << "  or %ebx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "# OR END" << std::endl;
}

void CodeGenerator::visitNotNode(NotNode* node) {
    std::cout << "# NOT START" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %eax" << std::endl;   
    std::cout << "  not %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "# NOT END" << std::endl;
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
    std::cout << "# NEGATION START" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %eax" << std::endl;   
    std::cout << "  neg %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "# NEGATION END" << std::endl;
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
    if (node->identifier_2) {
        std::string objectName = node->identifier_1->name;
        methodName = node->identifier_2->name;
        if (isVariableLocal(this->classTable, this->currentClassName, this->currentMethodName, objectName)) {
            objectClassName = (*this->currentMethodInfo.variables)[objectName].type.objectClassName;
            int obj_ebp_offset = getVariableOffsetInMethod(this->classTable, this->currentClassName, this->currentMethodName, objectName);
            std::cout << "  push " << obj_ebp_offset << "(%ebp)" << std::endl;
        } else {
            objectClassName = (*this->currentClassInfo.members)[objectName].type.objectClassName;
            int memberOffset = getMemberOffsetInClass(this->classTable, this->currentClassName, objectName);
            std::cout << "  mov 8(%ebp), %eax" << std::endl;
            std::cout << "  push " << memberOffset << "(%eax)" << std::endl;
        }
    } else {
        std::cout << "  push 8(%ebp)" << std::endl;
    }
    std::cout << "# PRE-CALL SEQUENCE END" << std::endl;
    std::cout << "  call " << getMethodLabelInClass(this->classTable, objectClassName, methodName) << std::endl;
    std::cout << "# POST-RETURN SEQUENCE START" << std::endl;
    std::cout << "  add $" << 4 * (node->expression_list->size() + 1) << ", %esp" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %ecx" << std::endl;
    std::cout << "  xchg %eax, (%esp)" << std::endl;
    std::cout << "# POST-RETURN SEQUENCE END" << std::endl;
}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {
    std::cout << "# MEMBER ACCESS START" << std::endl;
    std::string objectName = node->identifier_1->name;
    std::string memberName = node->identifier_2->name;
    std::string objectClassName;
    if (isVariableLocal(this->classTable, this->currentClassName, this->currentMethodName, objectName)) {
        objectClassName = (*this->currentMethodInfo.variables)[objectName].type.objectClassName;
        int obj_ebp_offset = getVariableOffsetInMethod(this->classTable, this->currentClassName, this->currentMethodName, objectName);
        std::cout << "  mov " << obj_ebp_offset << "(%ebp), %eax" << std::endl;
    } else {
        objectClassName = (*this->currentClassInfo.members)[objectName].type.objectClassName;
        int memberOffset = getMemberOffsetInClass(this->classTable, this->currentClassName, objectName);
        std::cout << "  mov 8(%ebp), %eax" << std::endl;
        std::cout << "  mov " << memberOffset << "(%eax), %eax" << std::endl;
    }
    
    int objectMemberOffset= getMemberOffsetInClass(this->classTable, objectClassName, memberName);
    std::cout << "  push " << objectMemberOffset << "(%eax)" << std::endl;
    std::cout << "# MEMBER ACCESS END" << std::endl;
}

void CodeGenerator::visitVariableNode(VariableNode* node) {
    std::string variableName = node->identifier->name;
    if (isVariableLocal(this->classTable, this->currentClassName, this->currentMethodName, variableName)) {
        int offset = getVariableOffsetInMethod(this->classTable, this->currentClassName, this->currentMethodName, variableName);
        std::cout << "  push " << offset << "(%ebp)" << std::endl;
    } else {
        int memberOffset = getMemberOffsetInClass(this->classTable, this->currentClassName, variableName);
        std::cout << "  mov 8(%ebp), %eax" << std::endl;
        std::cout << "  add $" << memberOffset << ", %eax" << std::endl;
        std::cout << "  push (%eax)" << std::endl;
    }
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {
    node->visit_children(this);
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
    node->visit_children(this);
}

void CodeGenerator::visitNewNode(NewNode* node) {
    std::cout << "# NEW NODE START" << std::endl;
    std::cout << "  push $" << getClassSize(this->classTable, node->identifier->name) << std::endl;
    std::cout << "  call malloc" << std::endl;
    std::cout << "  add $4, %esp" << std::endl;
    std::cout << "  push %eax" << std::endl;
    std::cout << "# NEW NODE END" << std::endl;
}

void CodeGenerator::visitIntegerTypeNode(IntegerTypeNode* node) {}

void CodeGenerator::visitBooleanTypeNode(BooleanTypeNode* node) {}

void CodeGenerator::visitObjectTypeNode(ObjectTypeNode* node) {}

void CodeGenerator::visitNoneNode(NoneNode* node) {}

void CodeGenerator::visitIdentifierNode(IdentifierNode* node) {}

void CodeGenerator::visitIntegerNode(IntegerNode* node) {
    std::cout << "  push $" << node->value << std::endl;
}
