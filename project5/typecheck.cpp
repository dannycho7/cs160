#include "typecheck.hpp"

// Defines the function used to throw type errors. The possible
// type errors are defined as an enumeration in the header file.
void typeError(TypeErrorCode code) {
  switch (code) {
    case undefined_variable:
      std::cerr << "Undefined variable." << std::endl;
      break;
    case undefined_method:
      std::cerr << "Method does not exist." << std::endl;
      break;
    case undefined_class:
      std::cerr << "Class does not exist." << std::endl;
      break;
    case undefined_member:
      std::cerr << "Class member does not exist." << std::endl;
      break;
    case not_object:
      std::cerr << "Variable is not an object." << std::endl;
      break;
    case expression_type_mismatch:
      std::cerr << "Expression types do not match." << std::endl;
      break;
    case argument_number_mismatch:
      std::cerr << "Method called with incorrect number of arguments." << std::endl;
      break;
    case argument_type_mismatch:
      std::cerr << "Method called with argument of incorrect type." << std::endl;
      break;
    case while_predicate_type_mismatch:
      std::cerr << "Predicate of while loop is not boolean." << std::endl;
      break;
    case do_while_predicate_type_mismatch:
      std::cerr << "Predicate of do while loop is not boolean." << std::endl;
      break;
    case if_predicate_type_mismatch:
      std::cerr << "Predicate of if statement is not boolean." << std::endl;
      break;
    case assignment_type_mismatch:
      std::cerr << "Left and right hand sides of assignment types mismatch." << std::endl;
      break;
    case return_type_mismatch:
      std::cerr << "Return statement type does not match declared return type." << std::endl;
      break;
    case constructor_returns_type:
      std::cerr << "Class constructor returns a value." << std::endl;
      break;
    case no_main_class:
      std::cerr << "The \"Main\" class was not found." << std::endl;
      break;
    case main_class_members_present:
      std::cerr << "The \"Main\" class has members." << std::endl;
      break;
    case no_main_method:
      std::cerr << "The \"Main\" class does not have a \"main\" method." << std::endl;
      break;
    case main_method_incorrect_signature:
      std::cerr << "The \"main\" method of the \"Main\" class has an incorrect signature." << std::endl;
      break;
  }
  exit(1);
}

int getVTSize(VariableTable* vt) {
  int size = 0;
  std::map<std::string, VariableInfo>::iterator vt_it;
  for (vt_it = vt->begin(); vt_it != vt->end(); vt_it++)
    size += vt_it->second.size;
  return size;
}

// TypeCheck Visitor Functions: These are the functions you will
// complete to build the symbol table and type check the program.
// Not all functions must have code, many may be left empty.

void TypeCheck::visitProgramNode(ProgramNode* node) {
  this->classTable = new ClassTable();
  node->visit_children(this);
}

void TypeCheck::visitClassNode(ClassNode* node) {
  this->currentMethodTable = new MethodTable();
  this->currentVariableTable = new VariableTable();
  this->currentClassName = node->identifier_1->name;
  this->currentMemberOffset = 0;
  node->visit_children(this);
  if (node->declaration_list) {
    for (std::list<DeclarationNode*>::iterator dn_it = node->declaration_list->begin(); dn_it != node->declaration_list->end(); dn_it++) {
      DeclarationNode* dn = *dn_it;
      for (std::list<IdentifierNode*>::iterator id_it = dn->identifier_list->begin(); id_it != dn->identifier_list->end(); id_it++) {
        VariableInfo vi = {{dn->type->basetype, dn->type->objectClassName}, this->currentMemberOffset, 4};
        (*this->currentVariableTable)[(*id_it)->name] = vi;
        this->currentMemberOffset += 4;
      }
    }
  }
  std::string superClassName = (node->identifier_2 == NULL) ? "" : node->identifier_2->name;
  int membersSize = getVTSize(this->currentVariableTable);
  ClassInfo ci = {superClassName, this->currentMethodTable, this->currentVariableTable, membersSize};
  (*this->classTable)[this->currentClassName] = ci;
}

void TypeCheck::visitMethodNode(MethodNode* node) {
  this->currentParameterOffset = 12;
  VariableTable* vt_before = this->currentVariableTable;
  this->currentVariableTable = new VariableTable();
  node->visit_children(this);
  std::list<CompoundType>* parameters = new std::list<CompoundType>();
  int localsSize = getVTSize(this->currentVariableTable);
  for (std::list<ParameterNode*>::iterator pl_it = node->parameter_list->begin(); pl_it != node->parameter_list->end(); pl_it++) {
    VariableInfo param_vi = (*this->currentVariableTable)[(*pl_it)->identifier->name];
    parameters->push_back(param_vi.type);
    localsSize -= param_vi.size;
  }
  MethodInfo mi = {{node->type->basetype, node->type->objectClassName}, this->currentVariableTable, parameters, localsSize};
  (*this->currentMethodTable)[node->identifier->name] = mi;
  this->currentVariableTable = vt_before;
}

void TypeCheck::visitMethodBodyNode(MethodBodyNode* node) {
  this->currentLocalOffset = -4;
  if (node->declaration_list) {
    for (std::list<DeclarationNode*>::iterator dn_it = node->declaration_list->begin(); dn_it != node->declaration_list->end(); dn_it++) {
      DeclarationNode* dn = *dn_it;
      dn->accept(this);
      for (std::list<IdentifierNode*>::iterator id_it = dn->identifier_list->begin(); id_it != dn->identifier_list->end(); id_it++) {
        VariableInfo vi = {{dn->type->basetype, dn->type->objectClassName}, this->currentLocalOffset, 4};
        (*this->currentVariableTable)[(*id_it)->name] = vi;
        this->currentLocalOffset -= 4;
      }
    }
  }
  node->visit_children(this);
  if (node->returnstatement) {
    node->basetype = node->returnstatement->basetype;
    node->objectClassName = node->returnstatement->objectClassName;
  } else {
    node->basetype = bt_none;
  }
}

void TypeCheck::visitParameterNode(ParameterNode* node) {
  node->visit_children(this);
  CompoundType ct = {node->type->basetype, node->type->objectClassName};
  VariableInfo param_vi = {ct, this->currentParameterOffset, 4};
  (*this->currentVariableTable)[node->identifier->name] = param_vi;
  this->currentParameterOffset += 4;
}

void TypeCheck::visitDeclarationNode(DeclarationNode* node) {
  node->visit_children(this);
}

void TypeCheck::visitReturnStatementNode(ReturnStatementNode* node) {
  node->visit_children(this);
  node->basetype = node->expression->basetype;
  node->objectClassName = node->expression->objectClassName;
}

void TypeCheck::visitAssignmentNode(AssignmentNode* node) {
  node->visit_children(this);
}

void TypeCheck::visitCallNode(CallNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitIfElseNode(IfElseNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitWhileNode(WhileNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitDoWhileNode(DoWhileNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitPrintNode(PrintNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitPlusNode(PlusNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitMinusNode(MinusNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitTimesNode(TimesNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitDivideNode(DivideNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitGreaterNode(GreaterNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitGreaterEqualNode(GreaterEqualNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitEqualNode(EqualNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitAndNode(AndNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitOrNode(OrNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitNotNode(NotNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitNegationNode(NegationNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitMethodCallNode(MethodCallNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitMemberAccessNode(MemberAccessNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitVariableNode(VariableNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitIntegerLiteralNode(IntegerLiteralNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitBooleanLiteralNode(BooleanLiteralNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitNewNode(NewNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitIntegerTypeNode(IntegerTypeNode* node) {
  node->basetype = bt_integer;
}

void TypeCheck::visitBooleanTypeNode(BooleanTypeNode* node) {
  node->basetype = bt_boolean;
}

void TypeCheck::visitObjectTypeNode(ObjectTypeNode* node) {
  node->visit_children(this);
  node->basetype = bt_object;
  node->objectClassName = node->identifier->name;  
}

void TypeCheck::visitNoneNode(NoneNode* node) {
  node->basetype = bt_none;
}

void TypeCheck::visitIdentifierNode(IdentifierNode* node) {
  // WRITEME: Replace with code if necessary
}

void TypeCheck::visitIntegerNode(IntegerNode* node) {
  // WRITEME: Replace with code if necessary
}


// The following functions are used to print the Symbol Table.
// They do not need to be modified at all.

std::string genIndent(int indent) {
  std::string string = std::string("");
  for (int i = 0; i < indent; i++)
    string += std::string(" ");
  return string;
}

std::string string(CompoundType type) {
  switch (type.baseType) {
    case bt_integer:
      return std::string("Integer");
    case bt_boolean:
      return std::string("Boolean");
    case bt_none:
      return std::string("None");
    case bt_object:
      return std::string("Object(") + type.objectClassName + std::string(")");
    default:
      return std::string("");
  }
}


void print(VariableTable variableTable, int indent) {
  std::cout << genIndent(indent) << "VariableTable {";
  if (variableTable.size() == 0) {
    std::cout << "}";
    return;
  }
  std::cout << std::endl;
  for (VariableTable::iterator it = variableTable.begin(); it != variableTable.end(); it++) {
    std::cout << genIndent(indent + 2) << it->first << " -> {" << string(it->second.type);
    std::cout << ", " << it->second.offset << ", " << it->second.size << "}";
    if (it != --variableTable.end())
      std::cout << ",";
    std::cout << std::endl;
  }
  std::cout << genIndent(indent) << "}";
}

void print(MethodTable methodTable, int indent) {
  std::cout << genIndent(indent) << "MethodTable {";
  if (methodTable.size() == 0) {
    std::cout << "}";
    return;
  }
  std::cout << std::endl;
  for (MethodTable::iterator it = methodTable.begin(); it != methodTable.end(); it++) {
    std::cout << genIndent(indent + 2) << it->first << " -> {" << std::endl;
    std::cout << genIndent(indent + 4) << string(it->second.returnType) << "," << std::endl;
    std::cout << genIndent(indent + 4) << it->second.localsSize << "," << std::endl;
    print(*it->second.variables, indent + 4);
    std::cout <<std::endl;
    std::cout << genIndent(indent + 2) << "}";
    if (it != --methodTable.end())
      std::cout << ",";
    std::cout << std::endl;
  }
  std::cout << genIndent(indent) << "}";
}

void print(ClassTable classTable, int indent) {
  std::cout << genIndent(indent) << "ClassTable {" << std::endl;
  for (ClassTable::iterator it = classTable.begin(); it != classTable.end(); it++) {
    std::cout << genIndent(indent + 2) << it->first << " -> {" << std::endl;
    if (it->second.superClassName != "")
      std::cout << genIndent(indent + 4) << it->second.superClassName << "," << std::endl;
    print(*it->second.members, indent + 4);
    std::cout << "," << std::endl;
    print(*it->second.methods, indent + 4);
    std::cout <<std::endl;
    std::cout << genIndent(indent + 2) << "}";
    if (it != --classTable.end())
      std::cout << ",";
    std::cout << std::endl;
  }
  std::cout << genIndent(indent) << "}" << std::endl;
}

void print(ClassTable classTable) {
  print(classTable, 0);
}
