#ifndef _PRINT_HPP
#define _PRINT_HPP


#include <iostream>

#include "tree.hpp"

// A visitor which prints the tree in pre-order.
// This visitor is completely implemented, and you
// can use it for reference.
class PrintVisitor : public Visitor {
public:
	void visitNode(Node* node);
};

#endif
