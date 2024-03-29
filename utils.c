#include<raylib.h>
#include<time.h>
#include<stdlib.h>
#include"utils.h"

//Initializing and De-Initializing Functions for Matrix
//------------------------------------------------------------------------------------------------------|
int* GenerateMat(int size, int max, int min) {
	srand(time(0));
	int* mat = malloc(size * sizeof(int));        
	for(int i = 0; i < size; i++) {
		mat[i] = (rand() % (max - min + 1) + min);
	}
	return mat;
}

int* SubCopyMat(int newSize, int currentSize, int* currentMat, int max, int min) {
	if(newSize < currentSize) {
	    int* newMat = malloc(newSize * sizeof(int));
	    for(int i = 0; i < newSize; i++) {
		    newMat[i] = currentMat[i];
	    }
	    return newMat;
	}
	if(newSize > currentSize) {
	    int* newMat = malloc(newSize * sizeof(int));
	    for(int i = 0; i < currentSize; i++) {
		    newMat[i] = currentMat[i];
	    }
	    for(int j = currentSize; j < newSize; j++) {
		    newMat[j] = (rand() % (max - min + 1) + min);
	    }
	    return newMat;
	}	

	return currentMat;
}

int* GenerateTriangleMat(int size, int gap) {
	srand(time(0));
	int* mat = malloc(size * sizeof(int));
	for(int i = 1; i < size + 1; i++) {
		mat[i - 1] = i * gap; 
	}
	int j;
	int swapFlag;
	for(int i = 0; i < size; i++) {
		j = rand() % size;
		if(j != i) {
			swapFlag = mat[j]; 
			mat[j] = mat[i];
			mat[i] = swapFlag;
		}
	}	
	return mat;
 }

struct Rectangle** GenerateBoxes(int size, int* mat, int width, int shift, int hPar, int sX, int sY) {
	struct Rectangle** boxes = malloc(size * sizeof(Rectangle*));
	for(int i = 0; i < size; i++) {
		boxes[i] = malloc(size * sizeof(Rectangle)); 
		boxes[i]->x = sX + (width + shift) * i;
		boxes[i]->y = sY - mat[i] * hPar;
		boxes[i]->width = width;
		boxes[i]->height = mat[i] * hPar;
	}
	return boxes;
}

void FreeSpace(int* mat, struct Rectangle** boxes, int size) {
	free(mat);

	for(int i = 0; i < size; i++) {
		free(boxes[i]);
	}
	free(boxes);
}

void FreeBoxes(struct Rectangle** boxes, int size) {
	for(int i =0; i < size; i++) {
		free(boxes[i]);
	}
	free(boxes);
}

//Functions For Drawing on Screen
//------------------------------------------------------------------------------------------------------|
void DrawOutLine(int index, Color color, int edge, struct Rectangle** boxes) {
	int posX = boxes[index]->x - edge;
	int posY = boxes[index]->y /* - edge */;
	int width = boxes[index]->width + edge * 2;
	int height = boxes[index]->height + edge * 1 /* *2 */;
	DrawRectangle(posX, posY, width, height, color);
}

//SortRelated Functions
//------------------------------------------------------------------------------------------------------|
void Swap(struct Rectangle** boxes, int* mat, int index1, int index2) {
	if(mat[index1] == mat[index2]) return;
	int flag = boxes[index1]->height;
	boxes[index1]->height = boxes[index2]->height;
	boxes[index2]->height = flag;

	flag = boxes[index1]->y;
	boxes[index1]->y = boxes[index2]->y;
	boxes[index2]->y = flag;

	flag = mat[index1];
	mat[index1] = mat[index2];
	mat[index2] = flag;
}

int AdjustUnitWidth(int screenWidth, int startX, int shift, int size) {
	int width = ((screenWidth - 2 * startX) - shift * (size - 1)) / size;
	return width;
}

int AdjustTriangleGap(int size, int min, int max) {
	return (max - min) / size;
}
