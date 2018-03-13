#include "MenuOptions.h"
#include "ListFunctions.h"
#include <Windows.h>
#include <stdio.h>
#include "myInputManager.h"
#include <wctype.h>

void MenuOption_AddItem(ShoppingList* shoppingList)
{
	if (shoppingList->currentListLength == 0)
		shoppingList->items = NULL;

	ShoppingListItem listItem;
	shoppingList->items = realloc(shoppingList->items, sizeof(listItem) * ++shoppingList->currentListLength);

	char userInput[100];
	float amountInput;
	int i = 0;

	printf("Name of item to add: ");
	scanString(userInput, 100);
	for (i = 0; userInput[i] != '\0' && i < MAX_ITEM_NAME_LENGTH; i++)
		userInput[i] = tolower(userInput[i]);
	if (i == MAX_ITEM_NAME_LENGTH)
	{
		userInput[MAX_ITEM_NAME_LENGTH - 1] = '.';
		userInput[MAX_ITEM_NAME_LENGTH] = '\0';
	}

	if (shoppingList->currentListLength > 1) // we already incremented the variable, so we're really checking if it's > 0 - that is, if there's something on the list
	{
		int levenshteinDistance;
		FindClosestLevenshteinMatch(shoppingList, userInput, &levenshteinDistance, NULL, 0);

		if (levenshteinDistance == 0)
		{
			shoppingList->items = realloc(shoppingList->items, sizeof(listItem) * --shoppingList->currentListLength);
			printf("That item is already on the shopping list!\nUse the other functions if you want to change the list.\n");
			system("pause");
			return;
		}
	}

	strncpy(shoppingList->items[shoppingList->currentListLength - 1].name, userInput, MAX_ITEM_NAME_LENGTH + 1);
	// since MAX_LENGTH is a count and not a zero-index, we need to go one higher ----^
	i = 0;
	printf("Unit of item: ");
	scanString(userInput, 100);
	for (i = 0; userInput[i] != '\0' && i < MAX_UNIT_NAME_LENGTH; i++)
		userInput[i] = tolower(userInput[i]);
	if (i == MAX_UNIT_NAME_LENGTH)
	{
		userInput[MAX_UNIT_NAME_LENGTH - 1] = '.';
		userInput[MAX_UNIT_NAME_LENGTH] = '\0';
	}
	strncpy(shoppingList->items[shoppingList->currentListLength - 1].unit, userInput, MAX_UNIT_NAME_LENGTH + 1);

	printf("Amount of item: ");
	do
	{
		while (!scanFloat(&amountInput))
			printf("Invalid input! Try again: ");
		if (amountInput <= 0)
			printf("Invalid input! Try again: ");
		else if (amountInput > MAX_AMOUNT)
			printf("There's no way you need that much/many %s. Try again: ", listItem.name);
	} while (amountInput <= 0 || amountInput > MAX_AMOUNT);

	shoppingList->items[shoppingList->currentListLength - 1].amount = amountInput;
	shoppingList->listHasUnsavedChanges = 1;
}

void MenuOption_ChangeItem(ShoppingList* shoppingList)
{
	if (shoppingList->currentListLength > 0)
	{
		printf("The list currently looks like this:\n\n");
		PrintShoppingList(shoppingList);
		printf("\nWhich item would you like to change (by number or name)? ");

		int selection = AskUserForListItem(shoppingList);

		if (selection >= 0)
		{
			float inputAmount = 0.0f;
			printf("Change to how much/many? ");

			char stringContainer[99];

			do
			{
				while (!scanFloat(&inputAmount))
					printf("Invalid input! Try again: ");
				if (inputAmount <= 0)
					printf("Invalid input! Try again: ");
				else if (inputAmount > MAX_AMOUNT)
					printf("There's no way you need that much/many %s. Try again: ", shoppingList->items[selection].name);
			} while (inputAmount <= 0 || inputAmount > MAX_AMOUNT);

			if (shoppingList->items[selection].amount != inputAmount)
			{
				shoppingList->items[selection].amount = inputAmount;
				shoppingList->listHasUnsavedChanges = 1;
			}
		}
		else
		{
			printf("Sorry, couldn't find the item you meant. Returning to menu.\n");
			system("pause");
		}
	}
	else
	{
		printf("There's no list to change.\n");
		system("pause");
	}
}

void MenuOption_RemoveItem(ShoppingList* shoppingList)
{
	if (shoppingList->currentListLength)
	{
		printf("The list currently looks like this:\n\n");
		PrintShoppingList(shoppingList);
		printf("\nWhich item would you like to remove (by number or name)? ");

		int selection = AskUserForListItem(shoppingList);

		if (selection >= 0)
		{
			RemoveItemFromList(shoppingList, selection);
			shoppingList->listHasUnsavedChanges = 1;
		}
		else
		{
			printf("Couldn't find the item. Returning to menu.\n");
			system("pause");
		}
	}
	else
	{
		printf("There's no list to change.\n");
		system("pause");
	}
}

void MenuOption_PrintList(ShoppingList* shoppingList)
{
	if (shoppingList->currentListLength)
	{
		PrintShoppingList(shoppingList);
		system("pause");
	}
}

void MenuOption_SaveList(ShoppingList* shoppingList)
{
	if (!shoppingList->currentListLength) // if there's no list, there's nothing to save
		return;

	char userInput[100];
	printf("Enter a filename (max 30 characters): ");
	scanString(&userInput, 100);
	userInput[99] = '\0';
	for (int i = 0; i < 100 && userInput[i] != '\0'; i++)
	{
		if (!iswalnum(userInput[i]))
		{
			for (int y = i; y < 100 && userInput[y] != '\0'; y++)
			{
				userInput[y] = userInput[y + 1];
			}
			i--;
		}
	}
	WriteShoppingListFile(shoppingList, userInput);
	shoppingList->listHasUnsavedChanges = 0;
}

void MenuOption_LoadList(ShoppingList* shoppingList)
{
	char userInput[100];

	if (shoppingList->listHasUnsavedChanges && shoppingList->currentListLength) // second condition evaluates true if there is a list currently
	{
		printf("The current list looks like this:\n");
		PrintShoppingList(shoppingList);
		printf("\n\nWould you like to save this list before loading a new one [y]/n? ");
		int userChoice = YesOrNoQuestion(userInput, 1);
		if (userChoice)
			MenuOption_SaveList(shoppingList);
	}

	printf("Enter the name of the shopping list you want to load: ");
	scanString(&userInput, 100);
	userInput[99] = '\0';
	for (int i = 0; i < 100 && userInput[i] != '\0'; i++)
	{
		if (!iswalnum(userInput[i]))
		{
			for (int y = i; y < 100 && userInput[y] != '\0'; y++)
			{
				userInput[y] = userInput[y + 1];
			}
			i--;
		}
	}
	ReadShoppingListFile(shoppingList, userInput);
	shoppingList->listHasUnsavedChanges = 0;
}

void MenuOption_Quit(ShoppingList* shoppingList)
{
	char userInput[100];

	if (shoppingList->listHasUnsavedChanges && shoppingList->currentListLength) // second condition evaluates true if there is a list currently
	{
		printf("The current list looks like this and has unsaved changes:\n");
		PrintShoppingList(shoppingList);
		printf("\n\nWould you like to save this list before exiting [y]/n? ");
		int userChoice = YesOrNoQuestion(userInput, 1);
		if (userChoice)
			MenuOption_SaveList(shoppingList);
	}
}