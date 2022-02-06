#ifndef FLATSA_UTILS 
#define FLATSA_UTILS 

#include<raylib.h>

//Initializing and De-Initializing Functions for Matrix
//------------------------------------------------------------------------------------------------------|
int* GenerateMat(int size, int max, int min);

struct Rectangle** GenerateBoxes(int size, int* mat, int width, int shift, int hPar, int sX, int sY);

void FreeSpace(int* mat, struct Rectangle** boxes, int size);

//Functions For Drawing on Screen
//------------------------------------------------------------------------------------------------------|
void DrawOutLine(int index, Color color, int edge, struct Rectangle** boxes);

//SortRelated Functions and Enum
//------------------------------------------------------------------------------------------------------|
enum SortType {
	SelectionSort = 10,
};

void Swap(Rectangle* rec1, Rectangle* rec2);

#endif
