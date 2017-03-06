#include "stack.h"

stack arr[100]; //making array to hold all the variables 
static int location = 0; //holds the location of where I am in the stack


void push(stack temp)
{
	//test 
	// printf("I'm Pushing %s | %d\n", temp.name, temp.lineNum);
	
	arr[location] = temp;
	
	//Test Adding to the arr
	 // printf("Added to the arr | Location %d | %s | %d | %d\n", location , arr[location].name, arr[location].lineNum, arr[location].scope);
	location++;
}

int search (stack temp)
{
	// printf("Comparing to the arr | Location %d | %s | %d | %d\n", location , temp.name, temp.lineNum, temp.scope);
	int x = location-1; //This was I don't mess with location variable
	while (x >= 0)
	{
		
		if(strcmp(arr[x].name,temp.name) == 0 && (temp.scope == arr[x].scope))
		{
			return 1000;
		}
		x--;
	}
	return -1;
	
}
int searchDec (stack temp)
{
	// printf("Search DEc| Location %d | %s | %d | %d\n", location , temp.name, temp.lineNum, temp.scope);
	int x = location-1; //This was I don't mess with location variable
	// printf("Location %d\n", location);
	while (x >= 0)
	{
		
		if( (strcmp(arr[x].name, temp.name ) == 0)) 
		{
			// printf("x %d Yay Arr | Location %d | %s | %d | %d\n", x , location , arr[x].name, arr[x].lineNum, arr[x].scope);
			return 1000;
		}
		else
		{
			// printf("x %d else arr | Location %d | %s | %d | %d\n", x ,location , arr[x].name, arr[x].lineNum, arr[x].scope);
			// printf("x %d Temp arr | Location %d | %s | %d | %d\n", x ,location , temp.name, temp.lineNum, temp.scope);
			// if ()
		}
		x--;
	}
	
	return -1;
	
}

int getLocation (stack temp)
{
	int x = location-1; //This was I don't mess with location variable
	while (x >= 0)
	{
		
		if((strcmp(arr[x].name,temp.name) == 0 )&& (temp.scope == arr[x].scope)) 
		{
			
			return x; // returning arbritay number that is grater than -1
		}
		x--;
	}
}
void pop()
{
	// printf("pop\n");
	location--;
}

void printStack()
{
	printf("Printing Stack bottom up\n");
	int x = 0;
	while (x <= location)
	{
		printf("arr %d %s | %d\n", x, arr[x].name, arr[x].lineNum);
		x++;
	}
}

int getSize ()
{
	// printf("Size %d\n", location-1);
	return location-1;
}
char* get(int i)
{
	char *temp = arr[i].name; 
	return temp;
}
int getStackScope()
{
	int x = location;
	int i = arr[x-1].scope;
	// printf("\nlocation %d\n",location);
	 // printf("\nreturn i %d\n", i);
	return i;
}