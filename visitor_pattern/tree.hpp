#ifndef _TREE_HPP
#define _TREE_HPP

#include <vector>

#include "visitor.hpp"

// The node class we use for our tree structure. It contains
// a value (integer) and a list of zero or more children. It
// extends visitable, meaning that it is a structure capable
// of being visited. This means it must implement the accept
// and visit_children virtual methods.
class Node : Visitable {
public:
	int value;
	std::vector<Node*> children;
	
	// The node constructor. Note the constructor chaining for
	// the value.
	Node(int newValue) : value(newValue) {}
	
	// The visit_children method for this structure is simple,
	// it just visits each child in the list in turn.
	void visit_children(Visitor* v) {
		for (std::vector<Node*>::iterator it = this->children.begin(); it != this->children.end(); it++) {
			(*it)->accept(v);
		}
	}
	
	// The accept method allows the node to call back into the
	// visitor with the appropriate function. This is important
	// when there are many types of visitable objects.
	void accept(Visitor* v) {
		v->visitNode(this);
	}
};

#endif
