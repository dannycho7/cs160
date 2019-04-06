#include <iostream>
#include <stack>

#include "tree.hpp"
#include "visitor.hpp"

#include "print.hpp"
#include "sum.hpp"
#include "max.hpp"

// This function reads the tree from standard input and creates
// a tree in memory. It returns the root of the tree. You do not
// need to modify this.
// Valid tree inputs have the following format:
//
// (1 (2 (3)) (4) (5 (6) (7)))
//
//   which produces
//
//     1
//    /|\
//   2 4 5
//   |  / \
//   3 6   7

Node* readTree() {
	std::stack<Node*> path;
	Node* root;
	
	char c;
	while (std::cin>>c) {
		if (c=='(') {
			int value;
			std::cin>>value;
			if (std::cin.fail()) {
				std::cerr<<"Invalid characters in input."<<std::endl;
				exit(1);
			}
			
			Node* newNode = new Node(value);
			if (!path.empty()) {
				path.top()->children.push_back(newNode);
			}
			path.push(newNode);
		} else if (c == ')') {
			root = path.top();
			path.pop();
		} else {
			std::cerr<<"Invalid characters in input."<<std::endl;
			exit(1);
		}
	}
	
	return root;
}

// This function frees the tree with given root node. This
// operation is performed recursively.
void deleteTree(Node* root) {
	if (root) {
		for (std::vector<Node*>::iterator it = root->children.begin(); it != root->children.end(); it++) {
			deleteTree(*it);
		}
		root->children.clear();
		delete root;
	}
}

int main(int argc, char* argv[]) {
	Node* tree = readTree();
	
	// Create a print visitor and visit the tree with it.
	PrintVisitor* print = new PrintVisitor;
	std::cout<<"Preorder: ";
	tree->accept(print);
	std::cout<<std::endl;
	delete print;
	
	// Create a sum visitor and visit the tree with it.
	SumVisitor* sum = new SumVisitor;
	tree->accept(sum);
	// Print the computed sum.
	std::cout<<"Sum: "<<sum->sum<<std::endl;
	delete sum;
	
	// Insert code to use your MaxVisitor to find the maximum
	// element in the tree. Reference the code that uses the
	// SumVisitor above.
	//
	// You must print the maximum element in the following format:
	//
	// Max: XX
	//
	// Where XX is the maximum element's value (an integer).
	
	MaxVisitor* max = new MaxVisitor;
	tree->accept(max);
	std::cout << "Max: " << max->max << std::endl;
	delete max;
	
	deleteTree(tree);
	return 0;
}
