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

static int getVariableOffsetInClass(ClassTable* ct, std::string className, std::string methodName, std::string memberName) {
    VariableTable* method_vt = (*(*ct)[className].methods)[methodName].variables;
    std::map<std::string, VariableInfo>::iterator vi_it =  method_vt->find(memberName);
    if (vi_it != method_vt->end())
        return vi_it->second.offset;
    else
        return getMemberOffsetInClass(ct, className, memberName);
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
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitClassNode(ClassNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitMethodNode(MethodNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitParameterNode(ParameterNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitCallNode(CallNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitWhileNode(WhileNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitPlusNode(PlusNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitMinusNode(MinusNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitTimesNode(TimesNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitDivideNode(DivideNode* node) {
    // WRITEME: Replace with code if necessary
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
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitVariableNode(VariableNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitNewNode(NewNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerTypeNode(IntegerTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitBooleanTypeNode(BooleanTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitObjectTypeNode(ObjectTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitNoneNode(NoneNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIdentifierNode(IdentifierNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerNode(IntegerNode* node) {
    // WRITEME: Replace with code if necessary
}
