// ShoppingList.cpp : Defines the entry point for the console application.
//

#include "ShoppingList.h"
#include "ListFunctions.h"
#include "MenuOptions.h"
#include "myInputManager.h"
#include <locale.h>

int main()
{
	setlocale(LC_ALL, "");

	char userInput[16];
	int userChoice = 0;

	ShoppingList shoppingList;
	shoppingList.currentListLength = 0;

	do
	{
		printf("Welcome to ShoppingList! What would you like to do?\n\n");
		printf("1: Add item to shopping list\n");
		printf("2: Change item on shopping list\n");
		printf("3: Remove item from shopping list\n");
		printf("4: Print shopping list on screen\n");
		printf("5: Save shopping list\n");
		printf("6: Load shopping list\n");
		printf("7: Exit\n\n");
		printf("Your choice: ");

		do
		{
			scanString(userInput, 15);

			for (int i = 0; userInput[i]; i++)
				userInput[i] = tolower(userInput[i]);

			if (strcmp(userInput, "1") == 0 || Levenshtein(userInput, "add") <= 1 || Levenshtein(userInput, "add item") <= 2)
				userChoice = 1;
			else if (strcmp(userInput, "2") == 0 || Levenshtein(userInput, "change") <= 1 || Levenshtein(userInput, "change item") <= 2)
				userChoice = 2;
			else if (strcmp(userInput, "3") == 0 || Levenshtein(userInput, "remove") <= 1 || Levenshtein(userInput, "remove item") <= 2)
				userChoice = 3;
			else if (strcmp(userInput, "4") == 0 || Levenshtein(userInput, "print") <= 1 || Levenshtein(userInput, "print list") <= 2)
				userChoice = 4;
			else if (strcmp(userInput, "5") == 0 || Levenshtein(userInput, "save") <= 1 || Levenshtein(userInput, "save list") <= 2
				||Levenshtein(userInput, "save file") <= 2)
				userChoice = 5;
			else if (strcmp(userInput, "6") == 0 || Levenshtein(userInput, "load") <= 1 || Levenshtein(userInput, "load list") <= 2 
				|| Levenshtein(userInput, "load file") <= 2)
				userChoice = 6;
			else if (strcmp(userInput, "7") == 0 || Levenshtein(userInput, "exit") <= 1 || Levenshtein(userInput, "quit") <= 1)
				userChoice = 7;
			else
				userChoice = 0;

			switch (userChoice)
			{
			case 1:
				MenuOption_AddItem(&shoppingList);
				break;
			case 2:
				MenuOption_ChangeItem(&shoppingList);
				break;
			case 3:
				MenuOption_RemoveItem(&shoppingList);
				break;
			case 4:
				MenuOption_PrintList(&shoppingList);
				break;
			case 5:
				MenuOption_SaveList(&shoppingList);
				break;
			case 6:
				MenuOption_LoadList(&shoppingList);
				break;
			case 7:
				MenuOption_Quit(&shoppingList);
				return 0;
				break;
			default:
				printf("Invalid choice, please try again: ");
				break;
			}
		} while (userChoice == 0);

		system("cls");

	} while (1);

	return 0;
}