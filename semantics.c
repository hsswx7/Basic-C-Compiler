#include "semantics.h"


static int currentScope = 0;  //Holds the number for the variable in the scope being used for variable definitions

FILE *ptr ; //global variable makes is easy for me to print to target file
static int temp; // stores the number of temporary variables 
static int xtemp = 0;
static int loopCounter = 0;
static int loopDetect = 0;
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
	/* If Entering <block> Node */
	if(strcmp(root->nodeName, "<block>") == 0)
	{
		if(root->child1->treeTk != NULL)
		{
			currentScope++; //So I know that I am entering the a fresh new begin bracket
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
		else
			return;
	}
	
	if(strcmp(root->nodeName, "<mStat>") == 0 && loopDetect == 0)
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
	
	if(strcmp(root->nodeName, "<out>") == 0)
	{
		// printf("Out Calling Expr\n");
		checkStaticSemantics(root->child1);
		fprintf(ptr, "STORE X%d\n", xtemp);
		fprintf(ptr, "WRITE X%d\n", xtemp);
		return;
	}
	
	if(strcmp(root->nodeName, "<in>") == 0)
	{
		CheckIfVarDefined (root);
		
		int x = getStackW(root); // getting the x value for stackW
		
		fprintf(ptr, "READ X%d\n", xtemp);
		fprintf(ptr, "LOAD X%d\n", xtemp);
		fprintf(ptr, "STACKW %d\n", x);
		return;
	}
	
	if(strcmp(root->nodeName, "<assign>") == 0)
	{
		checkStaticSemantics(root->child1);
	
		CheckIfVarDefined (root);
		
		int x = getStackW(root); // getting the x value for stackW
		
		fprintf(ptr, "STACKW %d\n", x);
		return;
	}
	
	if(strcmp(root->nodeName, "<if>") == 0)
	{
		loopCounter++;
		
		int localXtemp = xtemp;
		xtemp++;
		int p = loopCounter;
		// child1 expr | child2 RO | child3 expr | child4 block
	
		fprintf(ptr, "\nLOOP%d: ", loopCounter);
		
		checkStaticSemantics(root->child1);
		
		fprintf(ptr, "STORE X%d\n", localXtemp);
		
		checkStaticSemantics(root->child3);
		
		fprintf(ptr, "SUB X%d\n", localXtemp);
		
		checkStaticSemantics(root->child2);
	
		checkStaticSemantics(root->child4);
	
		fprintf(ptr,"\nOUT%d: NOOP\n", p);	
		return;
	}
	
	
	if(strcmp(root->nodeName, "<loop>") == 0)
	{
		loopDetect++;
		loopCounter++;
		
		int localXtemp = xtemp;
		xtemp++;
		int p = loopCounter;
		// child1 expr | child2 RO | child3 expr | child4 block
	
		fprintf(ptr, "\nLOOP%d: ", loopCounter);
		checkStaticSemantics(root->child1);
		
		fprintf(ptr, "STORE X%d\n", localXtemp);
		checkStaticSemantics(root->child3);
		
		fprintf(ptr, "SUB X%d\n", localXtemp);
		checkStaticSemantics(root->child2);
	
		checkStaticSemantics(root->child4);
		fprintf(ptr, "BR LOOP%d \n", p);

		fprintf(ptr,"\nOUT%d: NOOP\n", p);
		loopDetect--;
		return;
	}
	
	if(strcmp(root->nodeName, "<RO>") == 0)
	{
		processRO(root);
		return;
	}

	if(strcmp(root->nodeName, "<expr>") == 0)
	{
		// printf("Expr Called\n");
		if (root->treeTk != NULL)
		{
			checkStaticSemantics(root->child1);
		
			int localXtemp = xtemp;
			xtemp++;
		
			fprintf(ptr, "STORE X%d\n", localXtemp);

			checkStaticSemantics(root->child2);
		
			fprintf(ptr, "ADD X%d\n", localXtemp);
			return;
		}
		else
		{
			// printf("Expr Calles M \n");
			checkStaticSemantics(root->child1);
			return;
		}
	}
	
	// <M>   ->     <T> - <M> | <T>
	if(strcmp(root->nodeName, "<M>") == 0)
	{
		// printf("M Called\n");
		if (root->treeTk != NULL)
		{
			int localXtemp = xtemp;
			xtemp++;
			
			checkStaticSemantics(root->child2);
			fprintf(ptr, "STORE X%d\n", localXtemp);
			checkStaticSemantics(root->child1);
			fprintf(ptr, "SUB X%d\n", localXtemp);
			return;
		}
		else
		{
			// printf("M Calles T\n");
			checkStaticSemantics(root->child1);
			return;
		}
	}
	
	// <T>  ->  <F> * <T> | <F> / <T> | <F>
	if(strcmp(root->nodeName, "<T>") == 0)
	{
		// printf("T Called\n");
		if(root->treeTk != NULL)
		{
			checkStaticSemantics(root->child2);
			
			int localXtemp = xtemp;
			xtemp++;
			fprintf(ptr, "STORE X%d\n", localXtemp);
			
			checkStaticSemantics(root->child1);
			
			if(strcasecmp(root->treeTk->tokenName,"*_tk") == 0)
			{
				fprintf(ptr, "MULT X%d\n", localXtemp);
			}
			else if(strcasecmp(root->treeTk->tokenName,"%_tk") == 0)
			{
				fprintf(ptr, "DIV X%d\n", localXtemp);
			}
			return;
		}
		else
		{
			// printf("T Calls F\n");
			checkStaticSemantics(root->child1);
			return;
		}
	}
	
	// <F>   ->   - <F> | <R>
	if(strcmp(root->nodeName, "<F>") == 0)
	{
		//child1 = f | child2 = R
		//printf("F called\n");
		if (root->treeTk != NULL)
		{
			// printf("F Calls F\n");
			checkStaticSemantics(root->child1);
			fprintf(ptr, "MULT -1\n");
			return;
		}
		else
		{
			//printf("F calls R\n");
			// printf("%s\n", root->child2->nodeName);
			checkStaticSemantics(root->child2);
			return;
		}

	}
	
	if(strcmp(root->nodeName, "<R>") == 0)
	{
		// printf("R Called\n");
		if(root->treeTk !=NULL)  // if R is storing a token
		{
			// printf("R has a Tree Token\n");
			if (strcmp(root->treeTk->tokenName,"ID_tk") == 0) 
			{
				CheckIfVarDefined (root);
				int x = getStackW(root); // getting the x value for stackW
				fprintf(ptr,"STACKR %d\n", x);
				return;
			}
			else // of NUM Token
			{
				fprintf(ptr,"LOAD %s\n", root->treeTk->tokenWord);
				return;
			}
		}
		else // [expr] so nothing to do
		{
			checkStaticSemantics(root->child1);
			return;
		}
	}
	
	
	checkStaticSemantics (root->child1);
	checkStaticSemantics (root->child2);
	checkStaticSemantics (root->child3);
	checkStaticSemantics (root->child4);
	return;
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
int getStackW(Node *root)
{
	stack temp1; // making temp stack
	temp1.name = root->treeTk->tokenWord;
	temp1.lineNum = root->treeTk->lineNum;
	temp1.scope = currentScope;
	int l = getLocation (temp1); //searching if the variable is in the stack
	int x = (getSize() - l);
	if ( x == -1)
		x = 0;
	return x;
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
