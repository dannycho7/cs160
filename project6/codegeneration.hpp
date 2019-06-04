#ifndef __CODEGEN_HPP
#define __CODEGEN_HPP

#include "ast.hpp"
#include "typecheck.hpp"

// This defines the CodeGenerator visitor, which will visit
// the AST and generate x86 assembly code. You will do all
// your implementation of the code generation in the visitor
// functions for this visitor.
//
// NOTE: This visitor will visit _after_ the TypeCheck visitor,
// which means the symbol table will already be completely
// constructed when generating code. You will need to use
// the symbol table when generating code.
class CodeGenerator : public Visitor {
private:
  int currentLabel;
public:
  // This member is the ClassTable pointer for the symbol
  // table. The main file sets this appropraitely to the
  // root of the symbol table constructed by the TypeCheck
  // visitor.
  //
  // NOTE: Remember that it is a _pointer_.
  ClassTable* classTable;
  
  // These members represent the current class and method
  // names (which class we are inside and which method we are
  // inside at any point in the code generation) as well
  // as the class info and method info for the current class
  // and method. This allows us to get information about
  // variables, members, and other class methods very easily.
  //
  // NOTE: These are not automatically set, you will need to
  // maintain/set them as your visitor visits the AST.
  std::string currentClassName;
  std::string currentMethodName;
  ClassInfo currentClassInfo;
  MethodInfo currentMethodInfo;
  
  int nextLabel() {
    return currentLabel++;
  }
  
  CodeGenerator() : currentLabel(0) {}
  
  // All the visitor functions. You will need to write
  // appropriate implementation in codegeneration.cpp.
  virtual void visitProgramNode(ProgramNode* node);
  virtual void visitClassNode(ClassNode* node);
  virtual void visitMethodNode(MethodNode* node);
  virtual void visitMethodBodyNode(MethodBodyNode* node);
  virtual void visitParameterNode(ParameterNode* node);
  virtual void visitDeclarationNode(DeclarationNode* node);
  virtual void visitReturnStatementNode(ReturnStatementNode* node);
  virtual void visitAssignmentNode(AssignmentNode* node);
  virtual void visitCallNode(CallNode* node);
  virtual void visitIfElseNode(IfElseNode* node);
  virtual void visitWhileNode(WhileNode* node);
  virtual void visitPrintNode(PrintNode* node);
  virtual void visitDoWhileNode(DoWhileNode* node);
  virtual void visitPlusNode(PlusNode* node);
  virtual void visitMinusNode(MinusNode* node);
  virtual void visitTimesNode(TimesNode* node);
  virtual void visitDivideNode(DivideNode* node);
  virtual void visitGreaterNode(GreaterNode* node);
  virtual void visitGreaterEqualNode(GreaterEqualNode* node);
  virtual void visitEqualNode(EqualNode* node);
  virtual void visitAndNode(AndNode* node);
  virtual void visitOrNode(OrNode* node);
  virtual void visitNotNode(NotNode* node);
  virtual void visitNegationNode(NegationNode* node);
  virtual void visitMethodCallNode(MethodCallNode* node);
  virtual void visitMemberAccessNode(MemberAccessNode* node);
  virtual void visitVariableNode(VariableNode* node);
  virtual void visitIntegerLiteralNode(IntegerLiteralNode* node);
  virtual void visitBooleanLiteralNode(BooleanLiteralNode* node);
  virtual void visitNewNode(NewNode* node);
  virtual void visitIntegerTypeNode(IntegerTypeNode* node);
  virtual void visitBooleanTypeNode(BooleanTypeNode* node);
  virtual void visitObjectTypeNode(ObjectTypeNode* node);
  virtual void visitNoneNode(NoneNode* node);
  virtual void visitIdentifierNode(IdentifierNode* node);
  virtual void visitIntegerNode(IntegerNode* node);
};

#endif
