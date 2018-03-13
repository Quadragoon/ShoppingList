/********************************************************************\
* myInputManager.h                                                   *
*                                                                    *
* Author: Alexandra Hengl, ahl13002@mdh.se                           *
* Date: 2016 03 25                                                   *
* Revised by: Stefan Bygde, 2016 04 01 								 *
*																	 *
* Desc: Manage user input in simple console applications             *
*                                                                    *
\********************************************************************/

#ifndef MY_INPUT_MANAGER_H
#define MY_INPUT_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>	//needed for Mac OS

#define MAX_SIZE 128

	/*Desc: Function to get an integer-type user input and assign it to var				*\
	* Param: address of (pointer to) an integer-type variable							*\
	* Return: 1 (true) on success, 0 (false) in case of invalid (non-integer) input     */
int scanInt(int* var);

	/*Desc: Function to get a floating-point-type user input and assign it to var		*\
	* Param: address of (pointer to) a floating-point-type variable						*\
	* Return: 1 (true) on success, 0 (false) in case of invalid (non-float) input		*/
int scanFloat(float* var);

	/*Desc: Function to get a double-type user input and assign it to var				*\
	* Param: address of (pointer to) a double-type variable								*\
	* Return: 1 (true) on success, 0 (false) in case of invalid (non-double) input     */
int scanDouble(double* var);
	
	/*Desc: Function to get a character-type user input and assign it to var			*\
	* Param: address of (pointer to) a character-type variable							*/
void scanChar(char* var);

	/*Desc: Function to get a string from the user and assign it to str					*\
	* Param: str: address of (pointer to) a string (character array)					*\
	*		 size: the size of the string in bytes - must be larger than 0				*\
	* Return: 1 (true) on success, 0 (false) in case of invalid size					*/
int scanString(char* str, int size);

#endif