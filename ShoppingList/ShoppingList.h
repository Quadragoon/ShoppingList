#pragma once

#define MAX_AMOUNT 9999999
#define MAX_UNIT_NAME_LENGTH 20
#define MAX_ITEM_NAME_LENGTH 20

#define MAX_LIST_NAME_LENGTH 30

typedef struct ShoppingListItem
{
	float amount;
	char unit[MAX_UNIT_NAME_LENGTH + 1]; // we're leaving extra space for \0 in both of these
	char name[MAX_ITEM_NAME_LENGTH + 1];
} ShoppingListItem;

typedef struct ShoppingList
{
	ShoppingListItem* items;
	unsigned char currentListLength;
	unsigned char listHasUnsavedChanges;
} ShoppingList;