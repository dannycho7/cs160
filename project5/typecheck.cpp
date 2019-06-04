#include "typecheck.hpp"
#define MAIN_CLASS "Main"
#define MAIN_METHOD "main"

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

void* findInClass(ClassTable* ct, std::string className, std::string name, void* (*findInClassInfo)(ClassInfo, std::string)) {
  void* vi = NULL;
  std::map<std::string, ClassInfo>::iterator ct_it;
  while (vi == NULL && !className.empty() && (ct_it = ct->find(className)) != ct->end()) {
    ClassInfo ci = ct_it->second;
    className = ci.superClassName;
    vi = findInClassInfo(ci, name);
  }
  return vi;
}

VariableInfo* findVariable(VariableTable* vt, std::string variableName) {
  std::map<std::string, VariableInfo>::iterator vt_it = vt->find(variableName);
  return (vt_it == vt->end()) ? NULL : &(vt_it->second);
}

void* _findMemberInCI(ClassInfo ci, std::string memberName) {
  return findVariable(ci.members, memberName);
}

VariableInfo* findMember(ClassTable* ct, std::string className, std::string memberName) {
  return static_cast<VariableInfo*>(findInClass(ct, className, memberName, _findMemberInCI));
}

void* _findMethodInCI(ClassInfo ci, std::string methodName) {
  MethodTable* mt = ci.methods;
  std::map<std::string, MethodInfo>::iterator mt_it = mt->find(methodName);
  return (mt_it == mt->end()) ? NULL : &(mt_it->second);
}

MethodInfo* findMethod(ClassTable* ct, std::string className, std::string memberName) {
  return static_cast<MethodInfo*>(findInClass(ct, className, memberName, _findMethodInCI));
}

VariableInfo* getObjectVI(ClassTable* ct, VariableTable* vt, std::string currentClassName, std::string variableName) {
  VariableInfo* vi = NULL;
  if ((vi = findVariable(vt, variableName)) == NULL
    && (vi = findMember(ct, currentClassName, variableName)) == NULL) {
    typeError(undefined_variable);
  }
  if (vi->type.baseType != bt_object)
    typeError(not_object);
  return vi;
}

MethodInfo* validateMethodCall(ClassTable* ct, std::string className, std::string methodName, std::list<ExpressionNode*>* expression_list) {
  MethodInfo* mi = findMethod(ct, className, methodName);
  if (mi == NULL)
    typeError(undefined_method);
  if (expression_list->size() != mi->parameters->size())
    typeError(argument_number_mismatch);
  std::list<CompoundType>::iterator expected_args_it = mi->parameters->begin();
  std::list<ExpressionNode*>::iterator args_it = expression_list->begin();
  while (expected_args_it != mi->parameters->end() && args_it != expression_list->end()) {
    if ((*expected_args_it).baseType != (*args_it)->basetype || (*expected_args_it).objectClassName != (*args_it)->objectClassName)
      typeError(argument_type_mismatch);
    expected_args_it++;
    args_it++;
  }
  return mi;
}

MethodInfo* validateConstructorCall(ClassTable* ct, std::string className, std::list<ExpressionNode*>* expression_list) {
  ClassInfo ci = ct->find(className)->second;
  if (_findMethodInCI(ci, className) == NULL) {
    if (expression_list->size() == 0)
      return NULL;
  } else
    return validateMethodCall(ct, className, className, expression_list);
}

// TypeCheck Visitor Functions: These are the functions you will
// complete to build the symbol table and type check the program.
// Not all functions must have code, many may be left empty.

void TypeCheck::visitProgramNode(ProgramNode* node) {
  this->classTable = new ClassTable();
  node->visit_children(this);
  std::map<std::string, ClassInfo>::iterator ct_it;
  if ((ct_it = this->classTable->find(MAIN_CLASS)) == this->classTable->end()) {
    typeError(no_main_class);
  } else {
    ClassInfo main_ci = ct_it->second;
    if (main_ci.members->size() > 0)
      typeError(main_class_members_present);
    std::map<std::string, MethodInfo>::iterator mt_it;
    if ((mt_it = main_ci.methods->find(MAIN_METHOD)) == main_ci.methods->end()) {
      typeError(no_main_method);
    } else {
      MethodInfo main_mi = mt_it->second;
      if (main_mi.returnType.baseType != bt_none || main_mi.parameters->size() != 0)
        typeError(main_method_incorrect_signature);
    }
  }
}

void TypeCheck::visitClassNode(ClassNode* node) {
  this->currentMethodTable = new MethodTable();
  this->currentVariableTable = new VariableTable();
  this->currentClassName = node->identifier_1->name;
  this->currentMemberOffset = 0;
  std::string superClassName = (node->identifier_2 == NULL) ? "" : node->identifier_2->name;
  if (!superClassName.empty() && this->classTable->find(superClassName) == this->classTable->end())
    typeError(undefined_class);
  ClassInfo ci = {superClassName, this->currentMethodTable, this->currentVariableTable, 0};
  (*this->classTable)[this->currentClassName] = ci;
  node->visit_children(this);
  (*this->classTable)[this->currentClassName].membersSize = getVTSize(this->currentVariableTable);
  for (std::list<DeclarationNode*>::iterator dn_it = node->declaration_list->begin(); dn_it != node->declaration_list->end(); dn_it++) {
    DeclarationNode* dn = *dn_it;
    for (std::list<IdentifierNode*>::iterator id_it = dn->identifier_list->begin(); id_it != dn->identifier_list->end(); id_it++) {
      (*this->currentVariableTable)[(*id_it)->name].offset = this->currentMemberOffset;
      this->currentMemberOffset += 4;
    }
  }
}

bool isTypeOf(ClassTable* ct, std::string class_1, std::string class_2) {
  while (class_1 != class_2) {
    class_1 = (*ct)[class_1].superClassName;
    if (class_1.empty())
      return false;
  }
  return true;
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
  if (node->type->basetype != node->methodbody->basetype
      || !isTypeOf(this->classTable, node->methodbody->objectClassName, node->type->objectClassName))
    typeError(return_type_mismatch);
  if (node->identifier->name == this->currentClassName && node->type->basetype != bt_none)
    typeError(constructor_returns_type);
  this->currentVariableTable = vt_before;
}

void TypeCheck::visitMethodBodyNode(MethodBodyNode* node) {
  this->currentLocalOffset = -4;
  node->visit_children(this);
  for (std::list<DeclarationNode*>::iterator dn_it = node->declaration_list->begin(); dn_it != node->declaration_list->end(); dn_it++) {
    DeclarationNode* dn = *dn_it;
    for (std::list<IdentifierNode*>::iterator id_it = dn->identifier_list->begin(); id_it != dn->identifier_list->end(); id_it++) {
      (*this->currentVariableTable)[(*id_it)->name].offset = this->currentLocalOffset;
      this->currentLocalOffset -= 4;
    }
  }
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
  for (std::list<IdentifierNode*>::iterator id_it = node->identifier_list->begin(); id_it != node->identifier_list->end(); id_it++) {
    if (node->type->basetype == bt_object && this->classTable->find(node->type->objectClassName) == this->classTable->end())
      typeError(undefined_class);
    VariableInfo vi = {{node->type->basetype, node->type->objectClassName}, -1, 4};
    (*this->currentVariableTable)[(*id_it)->name] = vi;
  }
}

void TypeCheck::visitReturnStatementNode(ReturnStatementNode* node) {
  node->visit_children(this);
  node->basetype = node->expression->basetype;
  node->objectClassName = node->expression->objectClassName;
}

void TypeCheck::visitAssignmentNode(AssignmentNode* node) {
  node->visit_children(this);
  CompoundType compound_t;
  VariableInfo* vi;
  if (node->identifier_2 == NULL) {
    vi = NULL;
    std::string variableName = node->identifier_1->name;
    if ((vi = findVariable(this->currentVariableTable, variableName)) == NULL
      && (vi = findMember(this->classTable, this->currentClassName, variableName)) == NULL)
      typeError(undefined_variable);
  } else {
    vi = getObjectVI(this->classTable, this->currentVariableTable, this->currentClassName, node->identifier_1->name);
    if ((vi = findMember(this->classTable, vi->type.objectClassName, node->identifier_2->name)) == NULL)
      typeError(undefined_member);
  }
  compound_t = vi->type;
  if (compound_t.baseType != node->expression->basetype
      || !isTypeOf(this->classTable, node->expression->objectClassName, compound_t.objectClassName))
    typeError(assignment_type_mismatch);
}

void TypeCheck::visitCallNode(CallNode* node) {
  node->visit_children(this);
  node->basetype = node->methodcall->basetype;
  node->objectClassName = node->methodcall->objectClassName;  
}

void TypeCheck::visitIfElseNode(IfElseNode* node) {
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean)
    typeError(if_predicate_type_mismatch);
}

void TypeCheck::visitWhileNode(WhileNode* node) {
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean)
    typeError(while_predicate_type_mismatch);
}

void TypeCheck::visitDoWhileNode(DoWhileNode* node) {
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean)
    typeError(do_while_predicate_type_mismatch);
}

void TypeCheck::visitPrintNode(PrintNode* node) {
  node->visit_children(this);
}

void TypeCheck::visitPlusNode(PlusNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_integer;
}

void TypeCheck::visitMinusNode(MinusNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_integer;
}

void TypeCheck::visitTimesNode(TimesNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_integer;
}

void TypeCheck::visitDivideNode(DivideNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_integer;
}

void TypeCheck::visitGreaterNode(GreaterNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitGreaterEqualNode(GreaterEqualNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_integer || node->expression_2->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitEqualNode(EqualNode* node) {
  node->visit_children(this);
  if (!((node->expression_1->basetype == bt_integer && node->expression_2->basetype == bt_integer)
    || (node->expression_1->basetype == bt_boolean && node->expression_2->basetype == bt_boolean)))
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitAndNode(AndNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_boolean || node->expression_2->basetype != bt_boolean)
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitOrNode(OrNode* node) {
  node->visit_children(this);
  if (node->expression_1->basetype != bt_boolean || node->expression_2->basetype != bt_boolean)
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitNotNode(NotNode* node) {
  node->visit_children(this);
  if (node->expression->basetype != bt_boolean)
    typeError(expression_type_mismatch);
  node->basetype = bt_boolean;
}

void TypeCheck::visitNegationNode(NegationNode* node) {
  node->visit_children(this);
  if (node->expression->basetype != bt_integer)
    typeError(expression_type_mismatch);
  node->basetype = bt_integer;
}

void TypeCheck::visitMethodCallNode(MethodCallNode* node) {
  node->visit_children(this);
  MethodInfo* mi;
  if (node->identifier_2) {
    VariableInfo* vi = getObjectVI(this->classTable, this->currentVariableTable, this->currentClassName, node->identifier_1->name);
    mi = validateMethodCall(this->classTable, vi->type.objectClassName, node->identifier_2->name, node->expression_list);
  } else {
    mi = validateMethodCall(this->classTable, this->currentClassName, node->identifier_1->name, node->expression_list);
  }
  node->basetype = mi->returnType.baseType;
  node->objectClassName = mi->returnType.objectClassName; 
}

void TypeCheck::visitMemberAccessNode(MemberAccessNode* node) {
  node->visit_children(this);
  VariableInfo* vi = getObjectVI(this->classTable, this->currentVariableTable, this->currentClassName, node->identifier_1->name);
  if ((vi = findMember(this->classTable, vi->type.objectClassName, node->identifier_2->name)) == NULL) {
    typeError(undefined_member);
  } else {
    node->basetype = vi->type.baseType;
    node->objectClassName = vi->type.objectClassName;
  }
}

void TypeCheck::visitVariableNode(VariableNode* node) {
  VariableInfo* vi = NULL;
  std::string variableName = node->identifier->name;
  if ((vi = findVariable(this->currentVariableTable, variableName)) == NULL
    && (vi = findMember(this->classTable, this->currentClassName, variableName)) == NULL) {
    typeError(undefined_variable);
  } else {
    node->basetype = vi->type.baseType;
    node->objectClassName = vi->type.objectClassName;
  }
}

void TypeCheck::visitIntegerLiteralNode(IntegerLiteralNode* node) {
  node->basetype = bt_integer;
}

void TypeCheck::visitBooleanLiteralNode(BooleanLiteralNode* node) {
  node->basetype = bt_boolean;
}

void TypeCheck::visitNewNode(NewNode* node) {
  node->visit_children(this);
  std::string className = node->identifier->name;
  if (this->classTable->find(className) == this->classTable->end()) {
    typeError(undefined_class);
  } else {
    validateConstructorCall(this->classTable, className, node->expression_list);
    node->basetype = bt_object;
    node->objectClassName = className;
  }
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
