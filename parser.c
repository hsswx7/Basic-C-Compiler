#include "parser.h"

token *tk;

//Function to be called for 
Node *parser(Node *root)
{
	tk = scanner();
         //Node *root = NULL; //Starting with root node
	root = program();
	// program();
		
	/* Checking if token EOFtk */	
	if(tk->tokenID == 1050) //1050 is the ID for EOFtk 
	{
		printf("Parrsing Completed EOFtk Detected\n");
	}
	else
	{
		printf("ERROR - Found Token ID %d | Token : %s :(%s) | Line: %d (Expected EOFtk)\n", tk->tokenID, tk->tokenName, tk->tokenWord, tk->lineNum);
		exit(0);
	}
	//printf("\nPrintting Tree\n");
	// printParseTree(root,0);
	// checkStaticSemantics (root);
	return root;

}

//<program>  ->     <vars> <block>
Node *program()
{
	Node *node = getNode(programNode);
	node->child1 = vars();
	node->child2 = block();
	return node;
}

//<block> -> Begin <vars> <stats> End
Node * block()
{
	Node *node = getNode(blockNode);
	if(strcasecmp(tk->tokenName,"Begin_Tk") == 0) //Checking for Begin_Tk
	{
		tk = scanner(); //Consuming Begin_Tk
		
		if(strcasecmp(tk->tokenName,"End_tk") == 0) //Checking if There is a End_tk
		{
			printf("ERROR Line %d ",tk->lineNum);
			printToken2(tk);
			printf(" detected right after Begin_Tk\n ", tk->tokenWord);
			exit(0);
		}
		else if (strcasecmp(tk->tokenName,"EOF_tk") == 0)
		{
			printf("ERROR Line %d ",tk->lineNum);
			printToken2(tk);
			printf(" detected right after Begin_Tk\n ", tk->tokenWord);
			exit(0);
		}

		
		node->child1 = vars();

		node->child2 = stats();
		
		if(strcasecmp(tk->tokenName,"End_tk") == 0) //Checking if There is a End_tk
		{
			tk = scanner(); //Consuming End_tk
			return node;
		}
		else
		{
			printf("ERROR - Found Token ");
			printToken2(tk);
			printf(" (Expected End_tk)\n");
			exit(0);
		}
		
	}
	else
	{
		printf("ERROR - Found Token ");
		printToken2(tk);
		printf(" (xpected Begin_Tk)\n");
		exit(0);
	}
	return node;
}

// <vars> -> empty | Var Identifier <mvars> 
Node * vars()
{
	Node *node = getNode(varsNode);

	int temp = tk->lineNum;
	if(strcasecmp(tk->tokenName,"Var_tk") == 0) //Checking for var token
	{
		
		tk = scanner(); //Consuming Var_tk
		if(strcasecmp(tk->tokenName,"ID_tk") == 0)
		{
			node->treeTk = tk;
			tk = scanner(); //Consuming ID_tk
			node->child1 = mvars();
			return node;
		}
		else
		{
			printf("ERROR -(Expected ID_tk) On LINE: %d | Found Token : %s :(%s) ]\n", temp, tk->tokenName, tk->tokenWord);
			exit(0);
		}
	}
	else //if empty there's nothing to do here to return (As in the next token is not Var)
	{
		return node;
	}
}

//<mvars->  empty | : : Identifier <mvars>
Node * mvars()
{
	int temp = tk->lineNum;
	Node *node = getNode(mvarsNode);
	if(strcasecmp(tk->tokenName,":_tk") == 0) //Checking for  : token
	{
		tk = scanner();
		if(strcasecmp(tk->tokenName,":_tk") == 0) //Checking for  : token
		{
			tk = scanner();
			if(strcasecmp(tk->tokenName,"ID_tk") == 0)
			{
				node->treeTk = tk;
				tk = scanner();
				node->child1 = mvars();
				return node;
			}
			else
			{
				printf("ERROR -(Expected ID_tk) On LINE: %d | Found Token : %s ( %s , %d ) ]\n", temp, tk->tokenName, tk->tokenWord,tk->tokenID);
				exit(0);
			}
		}
		else
		{
			printf("ERROR -(Expected :_tk) On LINE: %d | Found Token : %s ( %s , %d )\n", temp, tk->tokenName, tk->tokenWord,tk->tokenID);
			exit(0);
		}
	}
	else //if empty nothing to do just returning as in (next token is not :)
	{
		return node;
	}
	
}

// <expr>        ->      <M> + <expr> | <M>
Node *expr ()
{
	
	Node *node = getNode(exprNode);
	node->child1 = M();
	if(strcmp(tk->tokenName,"+_tk") == 0) //Checking for  : token
	{
		node->treeTk = tk;
		tk = scanner(); //Consume *_tk token
		node->child2 = expr();
		return node;
	}
	else
	{
		return node;
	}
}
// <M>   ->     <T> - <M> | <T>
Node *M ()
{
	Node *node = getNode(MNode);
	node->child1 = T();
	if(strcasecmp(tk->tokenName,"-_tk") == 0) //Checking for  : token
	{
		node->treeTk = tk;
		tk = scanner(); //Consume -_tk token
		node->child2 = M(); /*I Think M */
		return node;
	}
	else
	{
		return node;
	}
}

// <T>  ->  <F> * <T> | <F> / <T> | <F>
Node *T ()
{
	Node *node = getNode(TNode);
	node->child1 = F();
	if(strcasecmp(tk->tokenName,"*_tk") == 0) //Checking for  : token
	{
		node->treeTk = tk;
		tk = scanner(); //Consume Scan token
		node->child2 = T();
		return node;
	}
	else if(strcasecmp(tk->tokenName,"%_tk") == 0) //Checking for  : token
	{
		node->treeTk = tk;
		tk = scanner(); //Consume Scan token
		node->child2 = T();
		return node;
	}
	else
	{
		return node;
	}
	
}
// <F>   ->   - <F> | <R>
Node *F ()
{
	Node *node = getNode(FNode);
	if(strcmp(tk->tokenName,"-_tk") == 0) //Checking for  : token
	{
		node->treeTk = tk;
		tk = scanner(); //Consume Scan token
		node->child1 = F();
		return node;
	}
	else
	{
		node->child2 = R();
		return node;
	}
	
}
// <R>  -> [ <expr> ] | Identifier | Number   
Node *R ()
{
	int temp = tk->lineNum;
	Node *node = getNode(RNode);
	if(strcasecmp(tk->tokenName,"[_tk") == 0) //Checking for  : token
	{
		tk = scanner();
		
		node->child1 = expr();
		
		if(strcasecmp(tk->tokenName,"]_tk") == 0)
		{
			tk = scanner();
			return node;
		}
		else
		{
			printf("ERROR -(Expected ]_tk) On LINE: %d | Found Token : %s ( %s , %d )\n", temp, tk->tokenName, tk->tokenWord,tk->tokenID);
			exit(0);
		}
	}
	else if(strcasecmp(tk->tokenName,"ID_tk") == 0)
	{
		node->treeTk = tk;
		tk = scanner();
		return node;
	}
	else if(strcasecmp(tk->tokenName,"Num_tk") == 0)
	{
		node->treeTk = tk;
		tk = scanner();
		return node;
	}
	else
	{

		printf("ERROR -(Expected [_tk or ID_tk or Num_tk ) On LINE: %d | Found Token : %s ( %s , %d )\n", temp, tk->tokenName, tk->tokenWord,tk->tokenID);
	        exit(0);	
	}
	
}
// <stat>  <mStat>
Node *stats()
{
	Node *node = getNode(statsNode);
	node->child1 = stat();
	node->child2 = mStat();
	return node;
}

// <mStat> -> <stat> <mStat> | empty
Node *mStat()
{
	Node *node = getNode(mStatsNode);
	if ((strcasecmp(tk->tokenName,"Scan_tk")) == 0 || (strcasecmp(tk->tokenName,"Print_tk") == 0) || (strcasecmp(tk->tokenName,"[_tk") == 0) || (strcasecmp(tk->tokenName,"Begin_Tk") == 0) || (strcasecmp(tk->tokenName, "Loop_tk") == 0)|| (strcasecmp(tk->tokenName,"Begin_Tk") == 0) || (strcasecmp(tk->tokenName, "ID_tk") == 0))
	{
		node->child1 = stat();
		node->child2 = mStat();
		return node;
	}
	else
	{	
		return node;
	}
}

// <stat> -> <in> | <out> | <block> | <if> | <loop> | <assign>
Node *stat()
{
	Node *node = getNode(statsNode);
	if(strcasecmp(tk->tokenName,"Scan_tk") == 0) //Checking for  if Scan and going to in
	{
		return in();
	}
	else if(strcasecmp(tk->tokenName,"Print_tk") == 0) //Checking for  print token and going to out
	{
		return out();
	}
	else if(strcasecmp(tk->tokenName,"Begin_Tk") == 0)
	{
		return block();
	}
	else if(strcasecmp(tk->tokenName,"[_tk") == 0) //Checking for  [ token token and going to iF
	{
		return iF();
	}
	else if(strcasecmp(tk->tokenName, "Loop_tk") == 0) // checkingif loop to go to loop
	{
		return loop();
	}
	else if(strcasecmp(tk->tokenName, "ID_tk") == 0) // checkingif loop to go to loop
	{
		return assign();
	}

}

//<in>  ->      Scan : Identifier .
Node *in ()
{
	int temp = tk->lineNum;
	Node *node = getNode(inNode);
	if(strcasecmp(tk->tokenName,"Scan_tk") == 0) //Checking for  if Scan and going to in
	{
		tk = scanner(); //Consume Scan token
		if(strcasecmp(tk->tokenName,":_tk") == 0) //Checking for  : token
		{
			tk = scanner();
			if(strcasecmp(tk->tokenName,"ID_tk") == 0)
			{
				node->treeTk = tk;
				tk = scanner();
				if(strcasecmp(tk->tokenName,"._tk") == 0)
				{
					tk = scanner();
					return node;
				}
				else
				{
					printf("ERROR -(Expected ._tk) On LINE: %d | Found Token : %s ( %s , %d )\n", temp, tk->tokenName, tk->tokenWord,tk->tokenID);
					exit(0);
				}	
			}
			else
			{
				printf("ERROR -(Expected ID_tk) On LINE: %d | Found Token : %s ( %s , %d )\n", temp, tk->tokenName, tk->tokenWord,tk->tokenID);
				exit(0);
			}
		}
		else
		{
			printf("ERROR -(Expected :_tk) On LINE: %d | Found Token : %s ( %s , %d )\n", temp, tk->tokenName, tk->tokenWord,tk->tokenID);
			exit(0);
		}
	}
	else
	{
		printf("ERROR -(Expected Scan_tk) On LINE: %d | Found Token : %s ( %s , %d )\n", temp, tk->tokenName, tk->tokenWord,tk->tokenID);
		exit(0);
	}
}
//<out>   ->   Print [ <expr>  ] .
Node *out ()
{
	int temp = tk->lineNum;
	Node *node = getNode(outNode);
	if(strcasecmp(tk->tokenName,"Print_tk") == 0) //Checking for  print token and going to out
	{
		tk = scanner();
		
		if(strcasecmp(tk->tokenName,"[_tk") == 0) //Checking for  : token
		{
			tk = scanner();
			node->child1 = expr();
			if(strcasecmp(tk->tokenName,"]_tk") == 0)
			{
				tk = scanner();
		
				if(strcasecmp(tk->tokenName,"._tk") == 0)
				{
					tk = scanner();
					return node;
				}
				else
				{
					printf("ERROR -(Expected ._tk) On LINE: %d | Found Token : %s :(%s) ]\n", temp, tk->tokenName, tk->tokenWord);
					exit(0);
				}	
			}
			else
			{
				printf("ERROR - (Expected ]_tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
				exit(0);
			}
		}
		else
		{
			printf("ERROR - (Expected [_tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
			exit(0);
		}
	}
	else
	{
		printf("ERROR - (Expected Print_tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
		exit(0);
	}
}
// <if>   ->  [ <expr> <RO> <expr> ]  Iff <block>
Node *iF()
{
	int temp = tk->lineNum;
	Node *node = getNode(ifNode);
	if(strcasecmp(tk->tokenName,"[_tk") == 0) //Checking for  print token and going to out
	{
		Node *node = getNode(ifNode);
		tk = scanner();
		node->child1 = expr();
		node->child2 = RO();
		node->child3 =  expr();
		if(tk->tokenID == 1040) // checking for [_tk
		{
			tk = scanner();
			if(strcasecmp(tk->tokenName,"Iff_tk") == 0)
			{
				tk = scanner();
				node->child4 = block();
				return node;
			}
			else
			{
				printf("ERROR - (Expected Iff_tk) Found On  Line: %d | Found Token : %s :( %s ) \n",temp, tk->tokenName, tk->tokenWord);
				exit(0);
			}
		}
		else
		{
			printf("ERROR - (Expected ]_tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
			exit(0);
		}
	}
	else
	{
		printf("ERROR - (Expected [_tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
		exit(0);
	}
}
Node *loop()
{
	int temp = tk->lineNum;
	if(strcasecmp(tk->tokenName, "Loop_tk") == 0) // checkingif loop to go to loop
	{
		Node *node = getNode(loopNode);
		tk = scanner();
		if(strcasecmp(tk->tokenName,"[_tk") == 0)
		{
			tk = scanner();
			node->child1 = expr();
			node->child2 = RO();
			node->child3 = expr();
			if(strcasecmp(tk->tokenName,"]_tk") == 0)
			{
				tk = scanner();
				node->child4 = block();
				return node;
			}
			else
			{
				printf("ERROR - (Expected ]_tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
				exit(0);
			}
		}
		else
		{
			printf("ERROR - (Expected [_tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
            exit(0);
		}
		
	}
	else
	{
		printf("ERROR - (Expected Loop_tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
		exit(0);
	}
}

// <assign> -> Identifier == <expr> .
Node *assign()
{
	int temp = tk->lineNum;

	if(strcasecmp(tk->tokenName, "ID_tk") == 0) 
	{
		Node *node = getNode(assingNode);
		node->treeTk = tk;
		tk = scanner();
		if(strcasecmp(tk->tokenName, "Compare_tk") == 0) 
		{
			tk = scanner();
			node->child1 = expr();
			if(strcasecmp(tk->tokenName,"._tk") == 0)
			{
				tk = scanner();
				return node;
			}
			else
			{
				printf("ERROR - (Expected ._tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
				exit(0);
			}
			
		}
		else
		{
			printf("ERROR - (Expected Compare_tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
			exit(0);
		}
	}
	else
	{
		printf("ERROR - (Expected ID_tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
		exit(0);
	}
}

// <RO> ->  >=> | <=< | = |  > | <  |  =!=    
Node *RO ()
{
	int temp = tk->lineNum;
	Node *node = getNode(RONode);
	if(strcasecmp(tk->tokenName,">=>_tk") == 0)
	{
		node->treeTk = tk;
		tk = scanner();
		return node;
	}
	else if(strcasecmp(tk->tokenName,"<=<_tk") == 0)
	{
		node->treeTk = tk;
		tk = scanner();
		return node;
	}
	else if(strcasecmp(tk->tokenName,"Equal_tk") == 0)
	{
		node->treeTk = tk;
		tk = scanner();
		return node;
	}
	else if(strcasecmp(tk->tokenName,">_tk") == 0)
	{
		node->treeTk = tk;
		tk = scanner();
		return node;
	}
	else if(strcasecmp(tk->tokenName,"<_tk") == 0)
	{
		node->treeTk = tk;
		tk = scanner();
		return node;
	}
	else if(strcasecmp(tk->tokenName,"=!=_tk") == 0)
	{
		node->treeTk = tk;
		tk = scanner();
		return node;
	}
	else
	{
		printf("ERROR - (Expected >=>_tk | <=<_tk | =_tk | >_tk | <_tk | =!=_tk) Found On  Line: %d | Found Token : %s :(%s) \n",temp, tk->tokenName, tk->tokenWord);
		exit(0);
	}

}


