#pragma once
#include "ShoppingList.h"

void PrintShoppingList(ShoppingList* shoppingList);
int Levenshtein(char *s1, char *s2);
int AskUserForListItem(ShoppingList * shoppingList);
void RemoveItemFromList(ShoppingList* shoppingList, int index);
void WriteShoppingListFile(ShoppingList* shoppingList, char* name);
int YesOrNoQuestion(char* userInputArray, int defaultYes);