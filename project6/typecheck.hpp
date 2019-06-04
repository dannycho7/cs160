#ifndef __TYPECHECK_HPP
#define __TYPECHECK_HPP

#include "ast.hpp"

#include <cstdlib>
#include <iostream>
#include <map>

// Defines a compound type, which is a basetype as well as a
// string representing the class name of an object type.
typedef struct compoundtype {
  BaseType baseType;
  std::string objectClassName;
} CompoundType;

// Defines the information for a variable. This will be the
// data in the variable table (each variable will map to one
// of these). Includes the type, the offset, and the size
// (always 4 bytes or 1 word in our language).
typedef struct variableinfo {
  CompoundType type;
  int offset;
  int size;
} VariableInfo;

// Defines a variable table. Maps from a string (variable
// name) to a variable info.
typedef std::map<std::string, VariableInfo> VariableTable;

// Defines the information for a method. This will be the
// data in the method table (each method will map to one
// of these). Includes return type, the variable table for
// the method (which will have the paramters and locals),
// a list of the types of the parameters, and the size of
// the local variables (used when allocating space in the
// stack frame).
typedef struct methodinfo {
  CompoundType returnType;
  VariableTable *variables;
  std::list<CompoundType> *parameters;
  int localsSize;
} MethodInfo;

// Defines a method table. Maps from a string (method name)
// to a method info.
typedef std::map<std::string, MethodInfo> MethodTable;

// Defines the information for a class. This will be the
// data in the class table (each class will map to one
// of these). Includes the super class name (empty string
// if no super class), the method table, the member table
// (which is a variable table), and the size of the members
// (which is used when allocating on the heap).
typedef struct classinfo {
  std::string superClassName;
  MethodTable *methods;
  VariableTable *members;
  int membersSize;
} ClassInfo;

// Defines a class table. Maps from a string (class name)
// to a class info.
typedef std::map<std::string, ClassInfo> ClassTable;

// This function will print the symbol table. The functions are
// at the bottom of this file, and do not need modification.
void print(ClassTable classTable);

// Defines all the possible type errors that can be thrown
// by the type checker. These are used to print strings out
// that describe the error.
typedef enum {
  undefined_variable,
  undefined_method,
  undefined_class,
  undefined_member,
  not_object,
  expression_type_mismatch,
  argument_number_mismatch,
  argument_type_mismatch,
  while_predicate_type_mismatch,
  do_while_predicate_type_mismatch,
  if_predicate_type_mismatch,
  assignment_type_mismatch,
  return_type_mismatch,
  constructor_returns_type,
  no_main_class,
  main_class_members_present,
  no_main_method,
  main_method_incorrect_signature
} TypeErrorCode;

// Declares a a function which will display type errors, then
// terminate the program with an error status code. The
// possible type errors are defined as an enumeration above.
void typeError(TypeErrorCode code);

// This defines the TypeCheck visitor, which will visit the AST
// and construct the symbol table. You will do all your
// implementation of the symbol table construction in the
// visitor functions for this visitor.
class TypeCheck : public Visitor {
public:
  // This member represents the main class table. You can
  // think of this as the "root" of the symbol table.
  //
  // NOTE: You will need to construct a new ClassTable
  // and set this pointer at the beginning of the TypeCheck
  // visitor pass over the AST.
  ClassTable* classTable;
  
  // These members allow you to keep track of the current
  // method table and and current variable table. This allows
  // you to insert variables and methods as soon as you finish
  // visiting them in the AST.
  // For example, you might insert variables immediately in your
  // visitDeclarationNode function.
  //
  // NOTE: These are pointers. You will need to allocate new
  // tables at certain points while building the symbol table.
  //
  // NOTE: These are not automatically set, you will need to
  // maintain/set them as your visitor visits the AST.
  MethodTable* currentMethodTable;
  VariableTable* currentVariableTable;
  
  // These members allow you to keep track of several current
  // offsets. Important offsets are the current offset in the
  // local variable space, the current offset for parameters,
  // and the current offset for class members.
  //
  // NOTE: The current local offset starts at -4
  // and goes _down_ by 4 for each new variable;
  // the current parameter offset starts at 12 and
  // goes _up_ by 4 for each parameter;
  // and the current member offset starts a 0 and
  // goes _up_ by 4 for each member.
  //
  // NOTE: These are not automatically set, you will need to
  // maintain/set them as your visitor visits the AST.
  int currentLocalOffset;
  int currentParameterOffset;
  int currentMemberOffset;

  // This member allows you to keep track of the name of the
  // current class. This is necessary for type checking.
  std::string currentClassName;
  
  // All the visitor functions. You will need to write
  // appropriate implementation in the typecheck.cpp file.
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
  virtual void visitDoWhileNode(DoWhileNode* node);
  virtual void visitPrintNode(PrintNode* node);
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

// The following functions are used to print the Symbol Table.
// They do not need to be modified at all.

std::string genIndent(int indent);
std::string string(CompoundType type);

void print(VariableTable variableTable, int indent);
void print(MethodTable methodTable, int indent);
void print(ClassTable classTable, int indent);

#endif
