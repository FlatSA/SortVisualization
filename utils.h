#ifndef SANEK_UTILS 
#define SANEK_UTILS 

#include<raylib.h>

/********** Functions for initialazing variables **************/

int* GenerateMat(int size, int max, int min);

struct Rectangle** GenerateBoxes(int size, int* mat, int width, int shift, int hPar, int sX, int sY);

void FreeSpace(int* mat, struct Rectangle** boxes, int size);

/*************************************************************/

/*Should be used ONLY in Draw Section*/
void DrawOutLine(int index, Color color, int edge, struct Rectangle** boxes);

/********* Sort related functions and enum ******************/

enum SortType {
	SelectionSort = 10,
};

void Swap(Rectangle* rec1, Rectangle* rec2);

#endif
