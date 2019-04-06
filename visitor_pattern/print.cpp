#include "print.hpp"

// The visitNode function for this visitor first prints the
// node then recursively visits the children. This leads to
// a preorder traversal/print of the tree.
void PrintVisitor::visitNode(Node* node){
	std::cout<<node->value<<" ";
	node->visit_children(this);
}
