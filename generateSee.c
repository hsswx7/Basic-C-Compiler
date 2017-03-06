/* generate.c contains assembler generation routines                         */
/*   assembler routines are those from chapter2 of "Program Translation" by  */
/*   P. Calingaert                                                           */

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "token.h"
#include "tree.h"
#include "error.h"
#include "generate.h"

static LabelCntr=0;                                     /* for unique labels */
static VarCntr=0;                                    /* for unique variables */
typedef enum {VAR, LABEL} nameType;
static char Name[20];                        /* for creation of unique names */

static char *newName(nameType what)
{ if (what==VAR)
    sprintf(Name,"V%d",VarCntr++);    /* generate a new label as V0, V1, etc */
  else
    sprintf(Name,"L%d",LabelCntr++);            /* new lables as L0, L1, etc */
  return(Name);
}

static void recGen(nodeType *stats,FILE *out)
{ char label[20], label2[20], argR[20];
  if (stats==NULL)
    return;
  switch (stats->nodeId)
  { case readNode:      fprintf(out,"\tREAD\t%s\n",stats->tokenP->str);
                        break;
    case assignNode:    recGen(stats->child1,out);           /* evaluate rhs */
                        fprintf(out,"\tSTORE\t%s\n",stats->tokenP->str);
                        break;
    case exprNode:	recGen(stats->child2,out);     /* first evaluate rhs */
                        strcpy(argR,newName(VAR));
			fprintf(out,"\tSTORE\t%s\n",argR);
			recGen(stats->child1,out);       /* result is in ACC */
			if (stats->tokenP->tokenId==PLUStk)
			  fprintf(out,"\tADD\t%s\n",argR);
			else if (stats->tokenP->tokenId==MINUStk)
			  fprintf(out,"\tSUB\t%s\n",argR);
                        else error("Invalid tokenId in expression node",
                           stats->tokenP->str,stats->tokenP->line);
			break;

    case condNode:	recGen(stats->child2,out);
			strcpy(argR,newName(VAR));
			fprintf(out,"\tSTORE\t%s\n",argR);
			recGen(stats->child1,out);
			fprintf(out,"\tSUB\t%s\n",argR);
			strcpy(label,newName(LABEL));
			switch (stats->tokenP->tokenId)
			{ case LESStk:  fprintf(out,"\tBRPOS\t%s\n",label);
			 	 	fprintf(out,"\tBRZERO\t%s\n",label);
					break;
			  case LESSEQtk: fprintf(out,"\tBRPOS\t%s\n",label);
					 break;
			  case GREATERtk: fprintf(out,"\tBRNEG\t%s\n",label);
			  		  fprintf(out,"\tBRZERO\t%s\n",label);
					  break;
			  case GREATEREQtk: fprintf(out,"\tBRNEG\t%s\n",label);
					    break;
			  case EQUALtk: fprintf(out,"\tBRNONZ\t%s\n",label);
					break;
			  default: fprintf(out,"\tBRZERO\t%s\n",label);
			}
			break;
    case ifNode:	recGen(stats->child1,out);              /* condition */
			strcpy(label,Name);           /* memorize jump label */
			recGen(stats->child2,out);   /* dependent statements */
			fprintf(out,"%s\tNOOP\n",label);
			break;	
    default:            recGen(stats->child1,out);
                        recGen(stats->child2,out);
                        recGen(stats->child3,out);         
  }
}

/* generate assembler code for parse tree pointed by prog, with variables    */
/*   listed in st subtree                                                    */
void generate(const nodeType *prog, nodeType *st, FILE *out)
{ int i;
  if (prog==NULL || prog->nodeId!=programNode || prog->child1==NULL ||
      prog->child1->nodeId!=programBlockNode)
    error("something wrong with parse tree in generate",NULL,0);
  recGen(prog->child1->child2,out);     /* child1->child2 points to statList */
  fprintf(out,"\tSTOP\n");
  while(st!=NULL)                  /* allocate storage for program variables */
  { fprintf(out,"%s\tSPACE\n",st->tokenP->str);
    st=st->child1;
  }
  for (i=0;i<VarCntr; i++)      /* allocate space for temporary variables V* */
    fprintf(out,"V%d\tSPACE\n",i);
}

