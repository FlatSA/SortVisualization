#include<raylib.h>
#include<time.h>
#include<stdlib.h>
#include"utils.h"

void Swap(Rectangle* rec1, Rectangle* rec2) {
	int flag = rec1->height;
	rec1->height = rec2->height;
	rec2->height = flag;

	flag = rec1->y;
	rec1->y = rec2->y;
	rec2->y = flag;
}

int* GenerateMat(int size, int max, int min) {
	srand(time(0));
	int* mat = malloc(size * sizeof(int));        
	for(int i = 0; i < size; i++) {
		mat[i] = (rand()%(max - min + 1) + min);
	}
	return mat;
}

struct Rectangle** GenerateBoxes(int size, int*mat, int width, int shift, int hPar, int sX, int sY) {
	struct Rectangle** boxes = malloc(size * sizeof(Rectangle*));
	for(int i = 0; i < size; i++) {
		boxes[i] = malloc(size * sizeof(Rectangle)); 
		boxes[i]->x = sX + (width + shift)*i;
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

void DrawOutLine(int index, Color color, int edge, struct Rectangle** boxes) {
	int posX = boxes[index]->x - edge;
	int posY = boxes[index]->y /* - edge */;
	int width = boxes[index]->width + edge*2;
	int height = boxes[index]->height + edge*1 /* *2 */;
	DrawRectangle(posX, posY, width, height, color);
}
