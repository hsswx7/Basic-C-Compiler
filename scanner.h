//Harprabh Sangha
#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "token.h"

static int location = 0; //this is like am I in my buffer string'
static int numLines = 1; //this is so I can track lines 

void reader(FILE *fp);
void printToken(token *tk); // prints token
void printToken2(token *tk); //prints toke without \n

int checkIfLetter (char letter);  // checks if char is a..z or A..Z
int checkKeyWord (char *value);
char *appendC (char *string, char buffer);//make a char string form char 

token* scanner();
int scan (char check); //identifies the char for the Driver

void setToken(int tkID, char *n, int lnumm, char *add, token **temp); 









#endif
