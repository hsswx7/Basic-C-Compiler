#include "buildTree.h" 
char *nodeTypeStrArr[]  = {
	"<program>", "<vars>", "<block>", "<stats>", "<mvars>", "<expr>", "<M>", "<T>", "<F>", "<R>", "<mStat>", "<stat>",
	"<in>", "<out>", "<if>", "<loop>", "<assign>", "<RO>"
};

Node *getNode(NodeType nodeName)
{
	Node *node;
	node = (Node *)malloc(sizeof(Node));
	
	node->nodeName = nodeTypeStrArr[nodeName];

	node->treeTk = NULL;
	node->child1 = NULL;
	node->child2 = NULL;
	node->child3 = NULL;
	node->child4 = NULL;
	token *treeTk = (token *) malloc(sizeof(token));

	return node;
	
}


void printParseTree(Node *root, int level) 
{
	if (root == NULL) return;
	printf("%*s" "%d %s ", level * 4, "", level, root->nodeName);
    if(root->treeTk != NULL)
	{
		printToken(root->treeTk);
	}
	
	if(root->treeTk == NULL)
		printf("\n");
	
	printParseTree(root->child1, level + 1);
	printParseTree(root->child2, level + 1);
	printParseTree(root->child3, level + 1);
	printParseTree(root->child4, level + 1);
}
