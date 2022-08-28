#ifndef FLATSA_UTILS 
#define FLATSA_UTILS 

#include<raylib.h>

//Initializing and De-Initializing Functions for Matrix
//------------------------------------------------------------------------------------------------------|
int* GenerateMat(int size, int max, int min);

int* SubCopyMat(int newSize, int currentSize, int* currentMat, int max, int min);

int* GenerateTriangleMat(int size, int gap);

struct Rectangle** GenerateBoxes(int size, int* mat, int width, int shift, int hPar, int sX, int sY);

void FreeSpace(int* mat, struct Rectangle** boxes, int size);

void FreeBoxes(struct Rectangle** boxes, int size);

//Functions For Drawing on Screen
//------------------------------------------------------------------------------------------------------|
void DrawOutLine(int index, Color color, int edge, struct Rectangle** boxes);

//SortRelated Functions and Enum
//------------------------------------------------------------------------------------------------------|
enum SortType {
	none = 10,
	SelectionSort,
	BubbleSort,
	InsertionSort,
	ShakerSort,
	MergeSort,
	QuickSort,
};

void Swap(struct Rectangle** boxes, int* mat, int index1, int index2);

void Replace(struct Rectangle** boxes, int* mat, int replace, int replacefrom);

//Adjust visual settings 
//------------------------------------------------------------------------------------------------------|
int AdjustUnitWidth(int screenWidth, int startX, int shift, int size);

int AdjustTriangleGap(int size, int min, int max);

#endif
