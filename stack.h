#ifndef STACK_H
#define STACK_H

#include "token.h"


typedef struct structStack
{
	char *name; //holds the name of the tk
	int  lineNum; // holds the lineNumber of the token
	int  scope; //putting scope for each variable
}stack;


// Function Definitions 

void push(stack temp); // adds to the top of the stack
int search (stack temp); //searching if the item is already in the stack
int searchDec (stack temp);
void pop(); //removes form stack
void printStack();
char* get(int i);
int getSize ();
int getLocation (stack temp);
int getStackScope();

#endif