#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "token.h"
#include "parser.h"
#include "buildTree.h"
#include "stack.h"


void codeGen(Node *root, char *target); // this is called before checkStaticSemantics beacuse it opens the target file for me to write into
void checkStaticSemantics(Node *root);
void CheckVar(Node *root);
void CheckIfVarDefined (Node *root);

void processRO(Node *root);


#endif
