//Harprabh Sangha
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"

#define FINAL 1000
#define ERROR  -1


void printToken(token *tk) // Printing the token
{
	printf(" %s : ( %d , %s , %d )\n", tk->tokenWord, tk->tokenID,tk->tokenName, tk->lineNum);
}
 void printToken2(token *tk) // Printing the token without \n
{
	printf(" ( %d , %s , %d )",tk->tokenID,tk->tokenName, tk->lineNum);
}
/*
This reader fucntion takes in the File pointer to read the file.
It modifies the static char buffer[9999] defined in the token.h file
The reader skips /t and records /n. The reader also skips anything after @ untill  empty space or /n
*/
void reader(FILE *fp)  // reading the file to split into different chars
{
	
	// char buffer [1000000]; //buffer to store the char's
	int count = 0;
	int read = 0;
	while (1)
    {
		if((read = fgetc(fp)) == EOF) //If end of file then record and exit
		{
			buffer[count+1] = '\0'; // storing for EOF
			break;
		}
		else
		{
			
			if (read == '@') // if a comment skipping part
			{
				while(1) //read through the file untill I get next line or empty space
				{
					if((read= fgetc(fp)) == '\n')
					{
						buffer[count] = '\n';
						count++;
						break;
					}
					else if (read == ' ') // if white space reading back tokens
					{
						break;
					}
				}
			}
			else if (read == '\t') //skipping the tab if there is a tab
			{
				read = fgetc(fp); //skip the tab
				while(1)
				{
					if (read == '\t')
					{
						read = fgetc(fp); //skip the tab
					}
					else
					{
						buffer[count] = read; //recored the next char after tab skipped
						count++;
						break;
					}
				}
			}
			else if (read == '\n') // if next line
			{
				buffer[count] = '\n'; //storing next line
				count++;
			}
			/*else if (read == ' ')
			{
				/*printf("Space detected\n");
				read = fgetc{fp};
				while(1)
				{
					if (read == ' ')
					{
						read = fgetc{fp};
					}
					else
					{
						buffer[count] = read; //recored the next char after tab skipped
						count++;
						break;
					}
				}*/
			// }
			else // storing the chars 
			{  
				buffer[count] = read;
				count++;
			}
		}
    }
	
	// printf("buffer: ", buffer);
}


int checkIfLetter (char letter) //using ascii to check if a..z or A...Z
{
	int ascii = letter;
	if ( (ascii >= 65 && ascii <=90) || (ascii >= 97 && ascii <=122) )
	{
		// printf("ascii: %d | letter: %c\n",ascii,letter);
		return 1;
	}
	else
	{
		return 0;
	}	
} 
int checkIfDigit (char letter) // using ascii to check if 0..9
{
	int ascii = letter;
	if (ascii >= 48 && ascii <= 57)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

char *appendC (char *string, char buffer) // using the append the char to a char string
{
	char *result = malloc(1+strlen(&buffer));
	strcpy(result, string);
    strcpy(result+strlen(string), &buffer);
    return result;
}

int checkKeyWord (char *value)
{
	int i = 0;

	// printf("Vlaue to comp %s\n", value);
	for(i; i < 14; i++)
	{
		if (strcmp(value,KEYWORDS[i]) == 0)
		{
			switch(i)
			{
				case 0:
					return 23;  //array location in the tokenName array
					break;
				case 1:
					return 24;
					break;
				case 2:
				    return 25;
					break;
				case 3:
					return 26;
					break;
				case 4:
					return 27;
					break;
				case 5:
					return 28;
					break;
				case 6:
					return 29;
					break;
				case 7:
					return 30;
					break;
				case 8:
					return 31;
					break;
				case 9:
					return 32;
					break;
				case 10:
					return 33;
					break;
				case 11:
					return 34;
					break;
				case 12:
					return 35;
					break;
				case 13:
					return 36;
					break;
			}	  
		}
	}
	// printf("Not keyword %s\n", value);
	
	return 0;
}

token* scanner()
{
	token *temp = malloc( sizeof(token) );
	int state = dfaArray[0][0];
	int nextState = 0;
	int index = 0;
	int kwCheck = 0;
	char *add = "\0";

		
	while (state <= FINAL)
	{
		nextState = dfaArray[state][scan(buffer[location])];
			
		if (nextState == ERROR)
		{
			printf("Exiting : Error reading %s at numLines %d\n", add, numLines);
			exit(0);
		}
		else if (nextState >= FINAL)
		{
			if (nextState == FINAL)
			{
				kwCheck = checkKeyWord(add);
				if(kwCheck == 0)
				{
					setToken(nextState, tokenName[0], numLines,add,&temp);
					return temp;
				}
				else
				{
					setToken(tokenID[kwCheck], tokenName[kwCheck], numLines,add,&temp);
					return temp;
				}
			}
			else if (nextState == 1001)
			{
				setToken(nextState, tokenName[1], numLines,add,&temp);
				return temp;
			}
			else
			{
				index = 0;
				while(index < 40 && tokenID[index] != nextState) ++index;
				setToken(nextState, tokenName[index], numLines,add,&temp);
				return temp;
			}
		}
		else
		{
			state = nextState;
			if (buffer [location] == '\n')
			{
				numLines++;
			}
				
			if (buffer[location] != ' ' && buffer[location] != '\n')
			{
				add = appendC(add,buffer[location]);
			}
			location++;
		}
	}
}


void setToken(int tkID, char *n, int lnum, char *add, token **temp)
{
	token *retVal = malloc( sizeof(token) );
	retVal->tokenID = tkID;
	retVal->tokenName = n;
	retVal->lineNum = lnum;
	retVal->tokenWord = add; 
	*temp = retVal;
}

int scan (char check)
{
	if (checkIfLetter(check))  // checking if letter
	{
		return 1;
	}
	else if (checkIfDigit(check)) //checking if digit
	{
		return 2;
	}
	else if (check == '\n') //checking if new line
	{
		return 23;
	}
	else if (check == '\0') //
	{
		return 24;
	}
	else if (check == ' ')
	{
		return 23;
	}
	else
	{
		int ascii = check;
		switch(ascii)
		{
			case 61:  // =
				return 3;
				break;
			case 33: // !
				return 4;
				break;
			case 60: // <
				return 5;
				break;
			case 62:  //>
				return 6;
				break;
			case 58:  // :
				return 7;
				break;
			case 43: // +
				return 8;
				break;
			case 45: // -
				return 9;
				break;
			case 42:  // * 
				return 10;
				break;
			case 47:  // /
				return 11;
				break;
			case 38:  //& 
				return 12;
				break;
			case 37:  //%
				return 13;
				break;
			case 46:   //.
				return 14;
				break;
			case 40:   //(
				return 15;
				break;
			case 41:  // )
 				return 16;
			case 123:  //{
				return 17;
				break;
			case 125: // }
				return 18;
				break;
			case 59: // ;
				return 19;
				break;
			case 91:  // [
				return 20;
				break;
			case 93: // ]
				return 21;
				break;
			case 44: // ,
				return 22;
				break;
			default:
				printf("Error %c not a Char in the languagen\n", check);
				exit(0);
			
		}
		
	}
}