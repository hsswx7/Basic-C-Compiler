#ifndef BUILDTREE_H
#define BUILDTREE_H

#include "token.h"
#include "scanner.h"

// None terminal token nodes
typedef enum
{
	programNode, varsNode, blockNode, statsNode, mvarsNode, exprNode, MNode, TNode,FNode, 
	RNode, mStatsNode, statNode, inNode, outNode, ifNode, loopNode, assingNode, RONode
}NodeType;


/* Tree Struct */
struct node
{ 
	char *nodeName; //To hold the terminals 
	token *treeTk;  //To hold the token
	struct node *child1; 
	struct node *child2;
	struct node *child3;
	struct node *child4; 
	// struct token *next; //for list used for parse tree
};
typedef struct node Node;

Node *getNode(NodeType nodeName); //sets up the Node and allocate all nessary memory
void printParseTree(Node *root, int level); // prints the tree


#endif
