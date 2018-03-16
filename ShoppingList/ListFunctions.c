#include "ListFunctions.h"
#include <Windows.h>
#include <stdio.h>
#include "myInputManager.h"
#include <math.h>

void PrintShoppingList(ShoppingList* shoppingList)
{
	int longestItemName = 0;
	int longestUnitName = 0;
	int longestAmount = 0;

	for (int i = 0; i < shoppingList->currentListLength; i++)
	{
		// Find the longest-name item in the shopping list (capping out at MAX_ITEM_NAME_LENGTH)
		if (longestItemName != MAX_ITEM_NAME_LENGTH)
		{
			int itemNameLength = -1;
			while (shoppingList->items[i].name[++itemNameLength] != '\0' && itemNameLength < MAX_ITEM_NAME_LENGTH);
			longestItemName = (itemNameLength > longestItemName) ? itemNameLength : longestItemName;
		}

		// Also find the unit with the longest name in the shopping list (also capping out at MAX_UNIT_NAME_LENGTH)
		if (longestUnitName != MAX_UNIT_NAME_LENGTH)
		{
			int unitNameLength = -1;
			while (shoppingList->items[i].unit[++unitNameLength] != '\0' && unitNameLength < MAX_UNIT_NAME_LENGTH);
			longestUnitName = (unitNameLength > longestUnitName) ? unitNameLength : longestUnitName;
		}

		// Oh! We also need to know how long the longest amount in the list is, so let's find out. This one caps at 7(+2 for floating point.)
		if (longestAmount != 9)
		{
			if ((int)shoppingList->items[i].amount != shoppingList->items[i].amount)
				longestAmount = (log10(shoppingList->items[i].amount) + 3 > longestAmount) ? log10(shoppingList->items[i].amount) + 3 : longestAmount;
			else
				longestAmount = (log10(shoppingList->items[i].amount) + 1 > longestAmount) ? log10(shoppingList->items[i].amount) + 1 : longestAmount;
			longestAmount = (longestAmount > 9) ? 9 : longestAmount;
		}
	}

	char formatStringToPrint[20];
	strncpy(formatStringToPrint, "%d.  %*.*f %*s %*s\n", 20);

	for (int i = 0; i < shoppingList->currentListLength; i++)
	{
		// Format string replacement characters, in order:
		// d : The listed number of the item we're printing currently
		// * : The longest amount string of any item on the list
		// .* : If we're printing this amount with 1 or 0 decimals
		// f : The amount of the item we're printing currently
		// * : The longest unit name string of any item on the list
		// s : The unit name of the item we're printing currently
		// * : The longest item name string of any item on the list
		// s : The item name of the item we're printing currently

		printf("%d.  %*.*f %*s %*s\n", i + 1, longestAmount, ((int)shoppingList->items[i].amount == shoppingList->items[i].amount) ? 0 : 1,
			shoppingList->items[i].amount, longestUnitName, shoppingList->items[i].unit, longestItemName, shoppingList->items[i].name);
	}
}

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int Levenshtein(char *s1, char *s2) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = strlen(s1);
	s2len = strlen(s2);
	unsigned int *column = malloc((s1len + 1) * sizeof(*column));
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}

	unsigned int returnValue = column[s1len];
	free(column);
	return(returnValue);
}

int FindClosestLevenshteinMatch(ShoppingList* shoppingList, char* stringToMatchAgainst, int* distance_returnvar, int* indicesToIgnore, int ignoreCount)
{
	*distance_returnvar = -1;
	int indexOfNearestMatch = -1;
	int i;

	for (i = 0; i < shoppingList->currentListLength; i++)
	{
		int ignoreThisIndex = 0;
		for (int iterator = 0; iterator < ignoreCount; iterator++)
			if (indicesToIgnore[iterator] == i)
				ignoreThisIndex = 1;
		if (ignoreThisIndex)
			continue;

		if (Levenshtein(shoppingList->items[i].name, stringToMatchAgainst) < *distance_returnvar || *distance_returnvar == -1)
		{
			*distance_returnvar = Levenshtein(shoppingList->items[i].name, stringToMatchAgainst);
			indexOfNearestMatch = i;
		}
	}

	return indexOfNearestMatch;
}

int AskUserForListItem(ShoppingList* shoppingList)
{
	char userInput[100];
	int userHasInputANumber = 1;

	do
	{
		scanString(&userInput, 100);
	} while (userInput[0] == '\0');

	userInput[99] = '\0';

	for (int i = 0; userInput[i] != '\0'; i++)
	{
		userInput[i] = tolower(userInput[i]);
		if (!iswdigit(userInput[i]))
			userHasInputANumber = 0;
	}

	if (userHasInputANumber)
	{
		int selection = atoi(userInput);
		if (selection <= shoppingList->currentListLength && selection > 0)
		{
			selection--; // The user has input a 1-indexed number, but our items[] array is zero-indexed (as arrays tend to be.)
			return selection;
		}
		else
			return -1;
	}
	else
	{
		int levenshteinDistance;
		int* indicesToIgnore = NULL;
		int numberOfIgnoredIndices = 0;

		while (1)
		{
			int closestMatchIndex = FindClosestLevenshteinMatch(shoppingList, userInput, &levenshteinDistance, indicesToIgnore, numberOfIgnoredIndices);

			if (levenshteinDistance == 0)
			{
				return closestMatchIndex;
			}
			else if (levenshteinDistance <= strlen(shoppingList->items[closestMatchIndex].name) / 4)
			{
				printf("\"%s\" is pretty close. Is that what you meant? [y]/n: ", shoppingList->items[closestMatchIndex].name);
				char userInput2[100]; // We can't reuse the same user input array since we need to remember the original query
				int userResponse = YesOrNoQuestion(userInput2, 1);

				if (userResponse)
					return closestMatchIndex;
				else
				{
					indicesToIgnore = realloc(indicesToIgnore, sizeof(numberOfIgnoredIndices) * ++numberOfIgnoredIndices);
					indicesToIgnore[numberOfIgnoredIndices - 1] = closestMatchIndex;
				}
			}
			else
			{
				return -1;
			}
		}
	}
}

void RemoveItemFromList(ShoppingList* shoppingList, int index)
{
	if (shoppingList->currentListLength >= index)
	{ // if there are more items in the list after the one we're removing, we can just move them back and overwrite
		for (int i = index + 1; i < shoppingList->currentListLength; i++)
		{
			strcpy(shoppingList->items[i - 1].name, shoppingList->items[i].name);
			strcpy(shoppingList->items[i - 1].unit, shoppingList->items[i].unit);
			shoppingList->items[i - 1].amount = shoppingList->items[i].amount;
		}
		ShoppingListItem listItem;

		// Reallocate the list into the new, smaller memory it needs (reducing currentListLength before execution of this line)
		shoppingList->items = realloc(shoppingList->items, sizeof(listItem) * --shoppingList->currentListLength);
	}
	else if (index == shoppingList->currentListLength + 1)
	{ // if we're removing the last item in the list, we actually need to remove it. Fancy that.
		ShoppingListItem listItem;
		realloc(shoppingList->items, sizeof(listItem) * --shoppingList->currentListLength);
	}
}

void WriteShoppingListFile(ShoppingList* shoppingList, char* name)
{
	FILE* filePointer;
	char stringHandler[MAX_LIST_NAME_LENGTH + 5];

	if (sprintf(stringHandler, "%.*s.txt", MAX_LIST_NAME_LENGTH, name) < 1)
	{
		printf("ERROR: Could not write filename!\n");
		system("pause");
		return;
	}

	filePointer = fopen(stringHandler, "w");
	if (filePointer == NULL)
	{
		printf("ERROR: Unhandled error! Error code %d\n", errno);
		system("pause");
	}

	if (shoppingList->currentListLength == 1)
		fprintf(filePointer, "Shopping list of 1 item:\n\n\0");
	else if (shoppingList->currentListLength > 1)
		fprintf(filePointer, "Shopping list of %d items:\n\n\0", shoppingList->currentListLength);
	else
		return;
	
	for (int i = 0; i < shoppingList->currentListLength; i++)
	{		
		fprintf(filePointer, "%.*f%c%s%c%s\n", ((int)shoppingList->items[i].amount == shoppingList->items[i].amount) ? 0 : 1,
			shoppingList->items[i].amount, -96, shoppingList->items[i].unit, -96, shoppingList->items[i].name);
		// The spaces in the brackets on the previous line are non-breaking spaces! We'll use these as separators when loading the file.
	}

	fprintf(filePointer, "\n\n###CAUTION: Changing this file could make the list impossible to load!###");
	fclose(filePointer);
}

void ReadShoppingListFile(ShoppingList* shoppingList, char* name)
{
	FILE* filePointer;
	char stringHandler[MAX_LIST_NAME_LENGTH + 5];

	sprintf(stringHandler, "%s.txt", name);

	if ((filePointer = fopen(stringHandler, "r")) == NULL)
	{
		if (errno == 2)
			printf("ERROR: Cannot load list by that name, file not found.\n");
		else
			printf("ERROR: Unhandled error! Error code: %d\n", errno);
		system("pause");
		return;
	}

	int maxLength = MAX_ITEM_NAME_LENGTH * 2 + MAX_UNIT_NAME_LENGTH * 2 + 10;
	char line[MAX_ITEM_NAME_LENGTH * 2 + MAX_UNIT_NAME_LENGTH * 2 + 10];

	if (shoppingList->currentListLength)
		free(shoppingList->items);

	shoppingList->currentListLength = 0;
	shoppingList->items = NULL; // these two lines are preparation for allocating memory later
	ShoppingListItem listItem;  // we'll use both of these to realloc() to the items[] array
	#define itemIndex shoppingList->currentListLength - 1

	while (fgets(line, maxLength, filePointer))
	{
		if (iswdigit(line[0]))
		{
			shoppingList->currentListLength++;
			shoppingList->items = realloc(shoppingList->items, sizeof(listItem) * shoppingList->currentListLength);
		}
		else
			continue;

		int i = 0;

		float loadedAmount = 0;
		for (i; iswdigit(line[i]); i++)
		{
			loadedAmount *= 10;
			if (loadedAmount > MAX_AMOUNT)
				break;
			loadedAmount += line[i] - '0';
		}

		if (loadedAmount > MAX_AMOUNT)
		{
			shoppingList->currentListLength--;
			continue;
		}

		if (line[i] == '.')
		{
			i++;
			loadedAmount += ((line[i] - '0') / 10.0f);
		}

		shoppingList->items[itemIndex].amount = loadedAmount;

		while (line[i++] != -96 && line[i] != '\0'); // look for one of the non-breaking spaces we use as a delimiter
		if (i > log10(MAX_AMOUNT) + 3 || line[i] == '\0' || line[i] == '\n')
		{
			shoppingList->currentListLength--;
			continue;
		}
		int nameStartPosition = i;

		#define NAME_POS i - nameStartPosition

		do  //... and then immediately start looking for the next one while we read the unit name
		{
			if (NAME_POS < MAX_UNIT_NAME_LENGTH)
				shoppingList->items[itemIndex].unit[NAME_POS] = (line[i] == -96) ? '\0' : line[i];
			else if (NAME_POS == MAX_UNIT_NAME_LENGTH)
			{
				shoppingList->items[itemIndex].unit[NAME_POS - 1] = '.';
				shoppingList->items[itemIndex].unit[NAME_POS] = '\0';
			}
		} while (line[i++] != -96 && line[i] != '\0');

		if (i > (log10(MAX_AMOUNT) + 4) + MAX_UNIT_NAME_LENGTH || line[i] == '\0' || line[i] == '\n')
		{
			shoppingList->currentListLength--;
			continue;
		}

		nameStartPosition = i;
		do  //... and then start looking for a NewLine while we read the item name
		{
			if (NAME_POS < MAX_ITEM_NAME_LENGTH)
				shoppingList->items[itemIndex].name[NAME_POS] = (line[i] == '\n') ? '\0' : line[i];
			else if (NAME_POS == MAX_ITEM_NAME_LENGTH)
			{
				shoppingList->items[itemIndex].name[NAME_POS - 1] = '.';
				shoppingList->items[itemIndex].name[NAME_POS] = '\0';
			}
		} while (line[i++] != '\n');
	}

	shoppingList->items = realloc(shoppingList->items, sizeof(listItem) * shoppingList->currentListLength);
}

int YesOrNoQuestion(char* userInputArray, int defaultYes)
{
	while (1)
	{
		scanString(userInputArray, 15);

		for (int i = 0; userInputArray[i] != '\0'; i++)
			userInputArray[i] = towlower(userInputArray[i]);

		if ((strcmp(userInputArray, "") == 0 && defaultYes) || strcmp(userInputArray, "y") == 0 || strcmp(userInputArray, "yes") == 0
			|| Levenshtein(userInputArray, "confirm") <= 1)
			return 1;
		else if ((strcmp(userInputArray, "") == 0 && !defaultYes) || strcmp(userInputArray, "n") == 0 || strcmp(userInputArray, "no") == 0
			|| Levenshtein(userInputArray, "cancel") <= 1)
			return 0;
		else
			printf("Couldn't parse input, try again: ");
	}
}