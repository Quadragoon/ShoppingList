/********************************************************************\
* myInputManager.c                                                   *
*                                                                    *
* Author: Alexandra Hengl, ahl13002@mdh.se                           *
* Desc: Manage user input in simple console applications             *
* Date: 2016 03 25                                                   *
* Revised: 2016 04 01 Stefan Bygde									 *
*                                                                    *
\********************************************************************/

#include "myInputManager.h"


static int isNrValid(char* str)
{
	int i = 0;

		//ignore (jump over) initial whitespaces
	while (isspace(str[i]) != 0)
		i++;

		//Check the first non-whitespace character
	if (str[i] != '0') //, then the user had NOT typed 0, -> err
		return 0;

	return 1;
}

	//Writes (and empties) stdin into str[128] and tries to convert the string to int
	//returns the int on success, zero otherwise
int scanInt(int* var)
{
	char str[MAX_SIZE];
	int temp;

	fgets(str, MAX_SIZE, stdin);	//terminates on 128-1 or '\n' or end-of-file	but appends with '\0'

	temp = atoi(str);

		//if temp is 0, check, maybe the string has been wrong
	if (temp == 0)
		if (isNrValid(str) == 0)
			return 0;

	*var = temp;
	return 1;	//str allowed to contain "123\n" or "___123abc\n", but not "abc123\n"
}

int scanFloat(float* var)
{
	char str[MAX_SIZE];
	float temp;

	fgets(str, MAX_SIZE, stdin);
	
	temp = atof(str);
	if (temp == 0)
		if (isNrValid(str) == 0)
			return 0;

	*var = temp;
	return 1;
}

int scanDouble(double* var)
{
	char str[MAX_SIZE];
	double temp;

	fgets(str, MAX_SIZE, stdin);

	temp = atof(str);
	if (temp == 0)
		if (isNrValid(str) == 0)
			return 0;

	*var = temp;
	return 1;
}

	//Copies (and empties) stdin into str[128], but returns only the first character
void scanChar(char* var)
{
	char str[MAX_SIZE];

	fgets(str, MAX_SIZE, stdin);

	*var = str[0];
}

	//Copies as many bytes as "size" defines, from stdin into str, and replaces the ending '\n' (if there is any) with '\0'
int scanString(char* str, int size)
{
	int len = 0;

	if (size <= 0)
		return 0;

	fgets(str, size, stdin);

	len = strlen(str);	//the actual length might be smaller than "size"

	if (str[len-1] == '\n')		//replaces the '\n' at the end with '\0'
		str[len-1] = '\0';

	return 1;
}