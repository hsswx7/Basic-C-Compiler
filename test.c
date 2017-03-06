#include "semantics.h"


static int currentScope = 0;  //Holds the number for the variable in the scope being used for variable definitions
static int beginScope = -1; //number of variables in stack at the begining of the scope
static int checkVarDef = 0; //So I know that there is a variable to check if not I don't bother checking
// static int numVarInScope = 0; //keeps track of how many variables in scope 

FILE *ptr ; //global variable makes is easy for me to print to target file
static int temp; // stores the number of temporary variables 
static int xtemp = 0;
static int loopCounter = 0;
static int plb = 0;

void codeGen(Node *root, char *target) // this just opens the file in the global main
{
	ptr = fopen (target, "w");
    if (ptr == 0)
    {
      perror("Could not open file ");
      exit(0);
    }
	checkStaticSemantics(root); //after the target file is open to wrtie this funciton checks semantics and writes to the target file
	
	int count = 0;
	while (count <= getSize())
	{
		fprintf(ptr, "POP\n"); //popping off global variables
		count++;
	}
	
	
	fprintf(ptr,"STOP\n");
	int i = 0;
	while( i <= xtemp)
	{
		fprintf(ptr, "X%d 0\n", i);
		i++;
	}
	fclose(ptr); 
}


void checkStaticSemantics (Node *root)
{
	if (root == NULL) //Skipping over any NULL Root node
		return; 
	
	/* If the Node is <vars>*/
	if(strcmp(root->nodeName,"<vars>") == 0)
	{		 
		if(root->treeTk!=NULL)
		{
			fprintf(ptr, "PUSH\n");
			
			// If ID token Found in the Var sub tree
			if ((strcmp(root->treeTk->tokenName,"ID_tk") == 0))
				CheckVar(root);
		}
	}
	if(strcmp(root->nodeName,"<mvars>") == 0)
	{	
		if(root->treeTk!=NULL)
		{
			fprintf(ptr, "PUSH\n");
			
			// If ID token Found in the Var sub tree
			if ((strcmp(root->treeTk->tokenName,"ID_tk") == 0))
				CheckVar(root);
		}
	}
	
	if(strcmp(root->nodeName, "<mStat>") == 0)
	{
		if(root->child1 == NULL)
		{
			// printf("\nCurrentScope %d\n", currentScope);
			int i = getSize();
			for(i ; i > 0; i--)
			{
				if(currentScope == getStackScope())
				{
					pop(); //removing for the stack
					fprintf(ptr, "POP\n");
				}
				else
					break;
			}
			currentScope--;
		}
	}
	
	
	/* If Entering <block> Node */
	if(strcmp(root->nodeName, "<block>") == 0)
	{
		if(root->child1->treeTk != NULL)
			currentScope++; //So I know that I am entering the a fresh new begin bracket
	}
	
	
	if(strcmp(root->nodeName, "<out>") == 0)
	{
		processOut(root);
	}
	
	if(strcmp(root->nodeName, "<in>") == 0)
	{
		processIN(root);
	}
	
	if(strcmp(root->nodeName, "<assign>") == 0)
	{
		processAsgn(root);
	}
	
	if(strcmp(root->nodeName, "<if>") == 0)
	{
		loopCounter++;
		root = processIff(root);
	}
	if(strcmp(root->nodeName, "<loop>") == 0)
	{
		loopCounter++;
		root = processLoop(root);
		plb = 0;
		
	}
	
	
	checkStaticSemantics (root->child1);
	checkStaticSemantics (root->child2);
	checkStaticSemantics (root->child3);
	checkStaticSemantics (root->child4);
}

void CheckVar(Node *root)
{
		// If ID token Found in the Var sub tree		
		//Adding vaiables to the stack if there are no variable definitions in the current scope of the stack

		if(getSize() == 0)
		{			
			stack temp; // making temp stack
			temp.name = root->treeTk->tokenWord;
			temp.lineNum = root->treeTk->lineNum;
			temp.scope = currentScope;
			
			if ( (strcmp(root->nodeName,"<vars>") == 0) || (strcmp(root->nodeName,"<mvars>") == 0)) //if the variable has not been defined in the scope
				push(temp); //adding on to the stack
			//verify that the token is not in the stack
		}
		else
		{
		//Creating temp stack to find the item | if found then then error | else I store the temp into the stack
			stack temp; // making temp stack
			temp.name = root->treeTk->tokenWord;
			temp.lineNum = root->treeTk->lineNum;
			temp.scope = currentScope;
			
			// printf("Checking %s %d %d\n", temp.name,temp.lineNum,temp.scope);
			int found = search(temp); //searching if the variable is in the stack 
			// printf("found returned %d\n", found);
			
			// This will happen if the variable is already defined in the scope 
			if (found > -1)
			{
				printf("ERROR -> Variable %s on Line %d has already been declared before in the same Scope\n", temp.name, temp.lineNum);
				exit(1); //exiting after error
			}
			else if ( (strcmp(root->nodeName,"<vars>") == 0) || (strcmp(root->nodeName,"<mvars>") == 0)) //if the variable has not been defined in the scope
			{
				push (temp); //pushing the variable into the scope
				// currentScope++; //Keeping track of h
			}
		}
		return;
}
void CheckIfVarDefined (Node *root)
{
	if (root->treeTk != NULL &&(strcmp(root->treeTk->tokenName,"ID_tk") == 0))
	{	
		//Creating temp stack to find the item | if found then then error | else I store the temp into the stack
		stack temp; // making temp stack
		temp.name = root->treeTk->tokenWord;
		temp.lineNum = root->treeTk->lineNum;
		temp.scope = currentScope;
		
		int found = searchDec(temp); //searching if the variable is in the stack 
		
		// This will happen if the variable is already defined in the scope 
		if (found == -1)
		{
			printf("ERROR -> Variable %s on Line %d has not been declared previously\n", temp.name, temp.lineNum);
			exit(1); //exiting after error
		}
	}
}
void processExe(Node *root)
{
	// child 1 = M | child2 = expr | has Token

	if(root == NULL)
		return;
	
	if (root->treeTk != NULL)
	{
		processM(root->child1);
		
		int localXtemp = xtemp;
		xtemp++;
		
		fprintf(ptr, "STORE X%d\n", localXtemp);

		processExe(root->child2);
		
		fprintf(ptr, "ADD X%d\n", localXtemp);
	}
	else
	{
		processM(root->child1);
	}
	return;
}

// <M>   ->     <T> - <M> | <T>
void processM(Node *root)
{
	// Child1 = t | child2 = M
	if(root == NULL)
		return;

	if (root->treeTk != NULL)
	{
		int localXtemp = xtemp;
		xtemp++;
		
		processM(root->child2);
		fprintf(ptr, "STORE X%d\n", localXtemp);
		processT(root->child1);
		fprintf(ptr, "SUB X%d\n", localXtemp);
	}
	else
	{
		processT(root->child1);
	}
	return;
}

// <T>  ->  <F> * <T> | <F> / <T> | <F>
void processT(Node *root)
{
	// c1 = f
	// c2 = t
	if(root == NULL)
		return;

	if(root->treeTk != NULL)
	{
		processT(root->child2);
		
		int localXtemp = xtemp;
		xtemp++;
		fprintf(ptr, "STORE X%d\n", localXtemp);
		
		processF(root->child1);
		
		if(strcasecmp(root->treeTk->tokenName,"*_tk") == 0)
		{
			fprintf(ptr, "MULT X%d\n", localXtemp);
		}
		else if(strcasecmp(root->treeTk->tokenName,"%_tk") == 0)
		{
			fprintf(ptr, "DIV X%d\n", localXtemp);
		}
	}
	else
	{
		processF(root->child1);
	}
	return;
	
}
void processF(Node *root)
{
	//child1 = f | child2 = R
	if(root == NULL)
		return;
	
	if (root->treeTk != NULL)
	{
		processF(root->child1);
		fprintf(ptr, "MULT -1\n");
	}
	else
	{
		processR(root->child2);
	}
	return;
}

void processR (Node *root)
{
	if(root->treeTk !=NULL)  // if R is storing a token
	{
		if (strcmp(root->treeTk->tokenName,"ID_tk") == 0) 
		{
			CheckIfVarDefined (root);
			stack temp1; // making temp stack
			temp1.name = root->treeTk->tokenWord;
			temp1.lineNum = root->treeTk->lineNum;
			temp1.scope = currentScope;
			int l = getLocation (temp1); //searching if the variable is in the stack
			// printf("ProcessR GetLocation %d | Name %s  | Scope: %d\n", l,temp1.name,currentScope);
			int x = (getSize() - l);
			fprintf(ptr,"STACKR %d\n", x);
		}
		else // of NUM Token
		{
			fprintf(ptr,"LOAD %s\n", root->treeTk->tokenWord);
		}
	}
	else // [expr] so nothing to do
	{
		processExe(root->child1);
	}
	return;
	
}

void processIN (Node *root)
{
	// CheckVar(root);
	CheckIfVarDefined (root);
	stack temp1; // making temp stack
	temp1.name = root->treeTk->tokenWord;
	temp1.lineNum = root->treeTk->lineNum;
	temp1.scope = currentScope;
	int l = getLocation (temp1); //searching if the variable is in the stack
	// printf("GetLocation %d | Name %s  | Scope: %d\n", l,temp1.name,currentScope);
	int x = (getSize() - l);
	fprintf(ptr, "READ X%d\n", xtemp);
	fprintf(ptr, "LOAD X%d\n", xtemp);
	fprintf(ptr, "STACKW %d\n", x);
	return;
}

void processAsgn(Node *root)
{
	processExe(root->child1);
	
	CheckIfVarDefined (root);
	stack temp1; // making temp stack
	temp1.name = root->treeTk->tokenWord;
	temp1.lineNum = root->treeTk->lineNum;
	temp1.scope = currentScope;
	int l = getLocation (temp1); //searching if the variable is in the stack
	//index of the vaiable
	int x = (getSize() - l);
	fprintf(ptr, "STACKW %d\n", x);
	return;
}

void processOut(Node * root)
{
	if (root == NULL) //Skipping over any NULL Root node
		return; 
		
	if (strcmp(root->nodeName, "<expr>") == 0)
	{
		processExe(root);

		fprintf(ptr, "STORE X%d\n", xtemp);
		fprintf(ptr, "WRITE X%d\n", xtemp);
	}
	
	processOut(root->child1);
	return;
}

void processBlock(Node *root)
{
	checkStaticSemantics (root);
	
	return;
}
void processLoopBlock(Node *root)
{
	if (root == NULL) //Skipping over any NULL Root node
		return; 
		
	/* If Entering <block> Node */
	if(strcmp(root->nodeName, "<block>") == 0)
	{
		if(root->child1->treeTk != NULL)
			currentScope++; //So I know that I am entering the a fresh new begin bracket
	}
	
	/* If the Node is <vars>*/
	if(strcmp(root->nodeName,"<vars>") == 0)
	{		 
		if(root->treeTk!=NULL)
		{
			fprintf(ptr, "PUSH\n");
			
			// If ID token Found in the Var sub tree
			if ((strcmp(root->treeTk->tokenName,"ID_tk") == 0))
				CheckVar(root);
		}
	}
	if(strcmp(root->nodeName,"<mvars>") == 0)
	{	
		if(root->treeTk!=NULL)
		{
			fprintf(ptr, "PUSH\n");
			
			// If ID token Found in the Var sub tree
			if ((strcmp(root->treeTk->tokenName,"ID_tk") == 0))
				CheckVar(root);
		}
	}
	
	if(strcmp(root->nodeName, "<mStat>") == 0)
	{
		if(root->child1 == NULL)
		{
			int i = getSize();
			for(i ; i > 0; i--)
			{
				if(currentScope == getStackScope())
				{
					// pop(); //removing for the stack
					plb++;
				}
				else
					break;
			}
			currentScope--;
		}	
	}
	
	
	if(strcmp(root->nodeName, "<out>") == 0)
	{
		processOut(root);
	}
	
	if(strcmp(root->nodeName, "<in>") == 0)
	{
		processIN(root);
	}
	
	if(strcmp(root->nodeName, "<assign>") == 0)
	{
		processAsgn(root);
	}
	
	if(strcmp(root->nodeName, "<if>") == 0)
	{
		loopCounter++;
		root = processIff(root);
	}
	if(strcmp(root->nodeName, "<loop>") == 0)
	{
		loopCounter++;
		root = processLoop(root);
	}
	
	
	processLoopBlock (root->child1);
	processLoopBlock (root->child2);
	processLoopBlock(root->child3);
	processLoopBlock (root->child4);
	
}

Node *processLoop(Node *root)
{
	int localXtemp = xtemp;
	xtemp++;
	int p = loopCounter;
	// child1 expr | child2 RO | child3 expr | child4 block
	
	fprintf(ptr, "\nLOOP%d: ", loopCounter);
	processExe(root->child1);
	fprintf(ptr, "STORE X%d\n", localXtemp);
	processExe(root->child3);
	fprintf(ptr, "SUB X%d\n", localXtemp);
	processRO(root->child2);
	
	processBlock(root->child4);
	fprintf(ptr, "BR LOOP%d \n", p);

	fprintf(ptr,"\nOUT%d: NOOP\n", p);
	return root->child4->child1;
}
Node *processIff(Node *root)
{
	int localXtemp = xtemp;
	xtemp++;
	int p = loopCounter;
	// child1 expr | child2 RO | child3 expr | child4 block
	
	fprintf(ptr, "\nLOOP%d: ", loopCounter);
	processExe(root->child1);
	fprintf(ptr, "STORE X%d\n", localXtemp);
	processExe(root->child3);
	fprintf(ptr, "SUB X%d\n", localXtemp);
	processRO(root->child2);
	
	processBlock(root->child4);
	
	
	fprintf(ptr,"\nOUT%d: NOOP\n", p);
	
	return root->child4->child1;
}
void processRO(Node *root)
{
	if(strcmp(root->treeTk->tokenWord, "<=<") == 0)
	{
		fprintf(ptr, "BRNEG OUT%d\n",loopCounter);
	}
	else if(strcmp(root->treeTk->tokenWord, ">=>") == 0)
	{
		fprintf(ptr, "BRPOS OUT%d\n",loopCounter);
	}
	else if(strcmp(root->treeTk->tokenWord, "=") == 0)
	{
		fprintf(ptr, "BRPOS OUT%d\n",loopCounter);
		fprintf(ptr, "BRNEG OUT%d\n",loopCounter);
	}
	else if(strcmp(root->treeTk->tokenWord, "<") == 0)
	{
		fprintf(ptr, "BRZNEG OUT%d\n",loopCounter);
	}
	else if(strcmp(root->treeTk->tokenWord, ">") == 0)
	{
		fprintf(ptr, "BRZPOS OUT%d\n",loopCounter);
	}
	else if(strcmp(root->treeTk->tokenWord, "=!=") == 0)
	{
		fprintf(ptr, "BRZERO OUT%d\n",loopCounter);
	}	
}