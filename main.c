#include<raylib.h>
#include"utils.h"
#include<stdbool.h>

//Screen Related Variables
//--------------------------------------------------------------------------------------------------|
static const int screenWidth = 1200;
static const int screenHeight = 700;
static const int max = 290;
static const int min = 5;
static const int unitWidth = 18;
static const int shift = 3;
static const int heightPar = 2;
static const int startX = 50;
static const int startY = screenHeight - 50;
static const int unitGap = 0;
static const int triangleGap = 5;

//Initializing Main Matrix
//--------------------------------------------------------------------------------------------------|
static const int size = 53;
static int* mat; 
static struct Rectangle** boxes;

//Input Variables
//--------------------------------------------------------------------------------------------------|
static int Input = 0;
static int SortType = none;

//Time Interval Variables
//--------------------------------------------------------------------------------------------------|
static double currentTime = 0;
static double deltaTime = 0.f;
static double timeInterval = 0.0f;

//Common Sort Variables
//--------------------------------------------------------------------------------------------------|
static int iterator = 0;
static int startPoint = 0;
static int currentTarget = 0;
static bool iterate = true;

//Selection Sort Variables
//--------------------------------------------------------------------------------------------------|
static bool BeginSelectionSort = false;

//Bubble Sort Variables
//--------------------------------------------------------------------------------------------------|
static bool BeginBubbleSort = false;
static bool InitStart = false;
static int counter = 0;
    
//InsertionSort Sort Variables
//--------------------------------------------------------------------------------------------------|
static bool BeginInsertionSort = false;
static bool endCycle = false;

//Common Sort Variables Reset Function 
static void Reset();

int main(void) {

    //Initializing Main MAtrix
    //--------------------------------------------------------------------------------------------------|
    mat = GenerateTriangleMat(size, triangleGap); 
    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
  
    //Initializing Screen
    //--------------------------------------------------------------------------------------------------|
    SetTargetFPS(144);
    InitWindow(screenWidth, screenHeight, "HereWeGo");


    while(!WindowShouldClose()) {
	
	//Input Section
	//----------------------------------------------------------------------------------------------|
	if(Input == 0) {

	    Input = GetKeyPressed();

	    switch(Input) {
		//Generate New Matrix
		case KEY_N:
		    FreeSpace(mat, boxes, size);
		    mat = GenerateMat(size, max, min);
		    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
		    Input = 0;
		    break;
		case KEY_T: 
		    FreeSpace(mat, boxes, size);
		    mat = GenerateTriangleMat(size, triangleGap); 
		    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
		    Input = 0;
		    break;
		//Begin Selection Sort    
		case KEY_S:
		    SortType = SelectionSort;
		    BeginSelectionSort = true;
		    break;
		case KEY_B:
		    SortType = BubbleSort;
		    BeginBubbleSort = true;
		    startPoint++;
		    iterator++;
		    break;
		case KEY_I:
		    SortType = InsertionSort; 
		    BeginInsertionSort = true;
		    currentTarget = 1;
		    break;
		default:
		    Input = 0;
	    }	
	}

	//Ensure Time Interval and Make One Sort-Step per timeInterval
	//----------------------------------------------------------------------------------------------|	
	if(currentTime < 0.01f) currentTime = GetTime();
	deltaTime = GetTime() - currentTime;
	
	if((deltaTime > timeInterval) && Input) {
		currentTime = GetTime();
		
		//SelectionSort per Time Interval
		//--------------------------------------------------------------------------------------|
		if(SortType == SelectionSort) {
		    if(!iterate) {
			iterator++;
			iterate = true;
		    }
		    if(iterator == size-1 && currentTarget != startPoint) {
			Swap(boxes[currentTarget], boxes[startPoint]);
			iterate = false;
		    }
		    if(iterate) {
			if(iterator < size -1) {
			    iterator++;
			} else {
			    startPoint++;
			    currentTarget = startPoint;
			    iterator = startPoint + 1;
			}
			if(startPoint == size - 1) {
			    BeginSelectionSort = false;
			    Reset();
			}
			if(boxes[currentTarget]->height > boxes[iterator]->height) {
			    currentTarget = iterator;
			}
			
		    }
		   
		}

		//BubbleSort per Time Interval
		//--------------------------------------------------------------------------------------|
		if(SortType == BubbleSort && InitStart) {
		    iterate = true; 
		    if(boxes[iterator]->height < boxes[currentTarget]->height) {
			Swap(boxes[iterator], boxes[currentTarget]);
			iterate = false;
		    }
		    if(iterate) {
			if(iterator < size - 1 - counter) { 
			    iterator++;
			    currentTarget++; 
			} else {
			    currentTarget = 0;
			    iterator = startPoint;
			    counter++;
			}

			if(counter == size - 1) {
			    BeginBubbleSort = false;
			    counter = 0;
			    Reset();
			}
		    }
		   
		}

		//InsertionSort per Time Interval
		//--------------------------------------------------------------------------------------|
		if(SortType == InsertionSort) {
			iterate = true;
			if(boxes[currentTarget]->height < boxes[iterator]->height){ 
			    Swap(boxes[iterator], boxes[currentTarget]); 
			    iterate = false;    	    
			    endCycle = false;
			} else if(endCycle) {
			    startPoint++;
			    iterator = startPoint; 
			    currentTarget = startPoint + 1;
			    iterate = false;
			}
			if(iterate) {
			    iterator--; 
			    currentTarget--; 
			    if(iterator == -1) {
				startPoint++;
				iterator = startPoint; 
				currentTarget = startPoint + 1; 
			    }
			    endCycle = true;
			}
			if(startPoint == size - 1) {
			    BeginInsertionSort = false;
			    endCycle = false; 
			    Reset();
			}
		}
	}

	//Draw Section
	//----------------------------------------------------------------------------------------------|
    	BeginDrawing();
	  
	    //Drawing Matrix in a Current state	
	    ClearBackground(BLACK);

	    for (int i = 0; i < size; i++) {
		DrawRectangleRec(*boxes[i], RED);
	    }
	   
	    //Selection Sort Draw Section 
	    if(BeginSelectionSort) {
		if(iterator != size - 1) {
		    if(timeInterval < 0.15) { 
		   	DrawOutLine(iterator - 1, RAYWHITE, unitGap, boxes); 
		    }
		    DrawOutLine(currentTarget, ORANGE, unitGap, boxes);
		    DrawOutLine(iterator, RAYWHITE, unitGap, boxes);
		} else {
		    DrawOutLine(iterator, RAYWHITE, unitGap, boxes);
		    DrawOutLine(currentTarget, ORANGE, unitGap, boxes);
		}
		DrawOutLine(startPoint, GREEN, unitGap, boxes);
	    }

	    //Bubble Sort Draw Section
	    if(BeginBubbleSort) {
		InitStart = true;
		if(counter != 0) DrawOutLine(size - counter - 1, GREEN, unitGap, boxes);
		DrawOutLine(iterator, RAYWHITE, unitGap, boxes);
		DrawOutLine(currentTarget, RAYWHITE, unitGap, boxes);
	    }

	    //InsertionSort 
	    if(BeginInsertionSort) {
		if(currentTarget != size)
		    DrawOutLine(currentTarget, RAYWHITE, unitGap, boxes);
		DrawOutLine(iterator, RAYWHITE, unitGap, boxes);
		if(startPoint < size - 1) 
		    DrawOutLine(startPoint + 1, GREEN, unitGap, boxes);
		
	    }
	 
	EndDrawing();
    }

    //Free Space and De-Initialize Screen
    FreeSpace(mat, boxes, size);

    CloseWindow();
    
    return 0;
}

static void Reset() {
    Input = 0;
    SortType = none;
    startPoint = 0; 
    currentTarget = 0;
    iterator = 0;
    iterate = true;
}
