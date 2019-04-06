#ifndef _VISITOR_HPP
#define _VISITOR_HPP

#include "tree.hpp"

// Forward declaration of node class
class Node;

// The abstract base class for all visitors. Each of your visitors
// will need to extend this base class. Visitors must provide implementation
// of all the virtual functions.
class Visitor {
public:
	virtual void visitNode(Node* node) = 0;
};

// The abstract base class for all visitable data structures. Each
// structure would extend this base class. In this project there is
// only one structure, a tree.
class Visitable {
public:
	virtual void visit_children(Visitor* v) = 0;
	virtual void accept(Visitor* v) = 0;
};

#endif
