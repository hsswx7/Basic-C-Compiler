//Harprabh Sangha
//This contains the prototypes for parser.c
#ifndef PARSER_H
#define PARSER_H


#include "buildTree.h"
#include "scanner.h"


/* Prototypes for the CFG */
Node *parser(Node *root);
Node *program();
Node *block();
Node *vars();
Node *mvars();
Node *expr();
Node *M();
Node *T();
Node *F();
Node *R();
Node *stats();
Node *mStat();
Node *stat();
Node *in ();
Node *out ();
Node *iF ();
Node *loop ();
Node *assign ();
Node *RO ();



#endif
