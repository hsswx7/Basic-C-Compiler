//Harprabh Sangha
//Created for cs4280 Proj 2 Parser 
// 11/13/2016

#include "token.h"
#include "scanner.h"
#include "parser.h"
#include "buildTree.h"
#include "semantics.h"

FILE* fileOpen(char *name);// Opens the file and returns filePtr
void getName(char *name); //removes .extension
token *tk;

int main(int argc, char* argv[])
{
	FILE *filePtr;
	
	switch (argc) 
	{
		case 1: //no parameter
				filePtr = stdin;
				break;

		case 2: // One parameter, use .fs16 file supplied	
				printf("Reading from file provided\n\n");
				char *fileName = (char *)malloc(sizeof(argv[1]) + 7); 
				fileName = argv[1];
				strcat(fileName, ".fs16");
				filePtr = fileOpen(fileName);
				break;

		default:
				printf("Syntax: scanner [file] (.fs16 is implicit)\n");
				exit(0);
    }

	reader(filePtr); //reading the file and putting all the code into a buffer array defined in token.h 
	
	Node *root = NULL; //making root for the tree

	printf("\n------Running Parser--------\n\n");
	root = parser(root); //running the parser and making the tree
	printf("\n-------Printing Tree--------\n");
	printParseTree(root,0); //Printing tree, the second number is used to help with indentation 

	printf("\n------Writing Machine Code / Checking Semantics ----\n"); 
	
	char *target = "out.asm"; // target is preset if user runs the program throuh redirection using comp < filename
	if (argc > 1) //this is true when the user runs comp filename
	{
		target = argv[1]; //sets the target to the file name
		getName(target); //removes the .fs16 i added in the switch statement
		strcat(target, ".asm"); //adds .asm at the end of the filename
	}
	
	printf("\nWriting Code to -> %s\n\n", target); //informing the user of the target filename name
	codeGen(root, target); // Generating Code
	printf("No Semantics Error\n\n"); // This is only printed if there is not error in the Semantics 
	printf("Program Done! \n");
	fclose(filePtr); //closing the file Ptr used to read the original file
	
}

// Fuctions

FILE*  fileOpen(char *name)
{
  FILE *file = fopen (name, "r");
    if (file == 0)
    {
      perror("Could not open file ");
      exit(0);
    }
    else 
    {
      return file;
    }
}
void getName(char *name)
{
  char *pos;
  if ( (pos = strrchr(name, '.')))
  {
    int location,counter = 0;
    int i = 1;
    while (i)
    {
      if (name[counter] == '.')
      {
        location = counter; 
        i = 0;
     }
     counter +=1;
    }
    //printf("Position of the . %d\n", location);
    name[location] = '\0';
  }
}


