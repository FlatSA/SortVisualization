#include<raylib.h>
#include"utils.h"
#include<stdbool.h>


int main(void) {

    //Screen Related Variables
    //--------------------------------------------------------------------------------------------------|
    const int screenWidth = 1200;
    const int screenHeight = 700;
    const int max = 190;
    const int min = 5;
    const int unitWidth = 30;
    const int shift = 5;
    const int heightPar = 3;
    const int startX = 50;
    const int statY = screenHeight - 50;

    //Initializing Main Matrix
    //--------------------------------------------------------------------------------------------------|
    const int size = 32;
    int* mat = GenerateMat(size, max, min); 
    struct Rectangle** boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, statY);

    //Input Variables
    //--------------------------------------------------------------------------------------------------|
    int Input = 0;
    int SortType = 0;

    //Time Interval Variables
    //--------------------------------------------------------------------------------------------------|
    double currentTime = 0;
    double deltaTime = 0.f;
    double timeInterval = 0.025f;

    //Common Sort Variables
    //--------------------------------------------------------------------------------------------------|
    int iterator = 0;
    int startPoint = 0;
    int currentTarget = 0;

    //Selection Sort Variables
    //--------------------------------------------------------------------------------------------------|
    bool flag = true;
    bool BeginSelectionSort = false;

    //Bubble Sort Variables
    //--------------------------------------------------------------------------------------------------|
    bool BeginBubbleSort = false;
    bool iterate = true;
    int counter = 0;

    //Initializing Screen
    //--------------------------------------------------------------------------------------------------|
    SetTargetFPS(60);
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
		    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, statY);
		    Input = 0;
		    break;
		//Begin Selection Sort    
		case KEY_S:
		    SortType = SelectionSort;
		    break;
		case KEY_B:
		    SortType = BubbleSort;
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
		
		//SelectionSort
		//--------------------------------------------------------------------------------------|
		if(SortType == SelectionSort) {
		    if(startPoint == 0) BeginSelectionSort = true;	
		    if(iterator < size - 1) {
			iterator++;
			if(currentTarget == startPoint) flag = false;
		    } else {			    
			if(currentTarget != startPoint) Swap(boxes[currentTarget], boxes[startPoint]);
			startPoint++;
			iterator = startPoint + 1;
			currentTarget = startPoint;
		    }
		    if(startPoint == size - 1) {
			BeginSelectionSort = false;
			SortType = 0;
			startPoint = 0;
			iterator = 0;
			currentTarget = 0;
			Input = 0;
			flag = true;
		    } else if(boxes[iterator]->height < boxes[currentTarget]->height) {
			currentTarget = iterator;
			flag = true;
		    }
		}

		//BubbleSort
		//--------------------------------------------------------------------------------------|
		if(SortType == BubbleSort) {
		    
		    if(boxes[iterator]->height < boxes[currentTarget]->height) {
			Swap(boxes[iterator], boxes[currentTarget]);
			iterate = false;
		    }
		    if(iterate) {
			if(startPoint == 0) {
			    BeginBubbleSort = true;	
			    iterator++;
			    startPoint++;
			    } else {
				if(iterator < size - 1 - counter) { 
				    iterator++;
				    currentTarget++; 
				} else {
				    currentTarget = 0;
				    iterator = startPoint;
				    counter++;
				}
			    }

			if(counter == size) {
			    BeginBubbleSort = false;
			    iterate = true;
			    SortType = 0;
			    iterator = 0;
			    startPoint = 0;
			    currentTarget = 0;
			    counter = 0;
			    Input = 0;
			}
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
		if(flag) DrawOutLine(currentTarget, ORANGE, 1, boxes);
		DrawOutLine(iterator, RAYWHITE, 1, boxes);
		DrawOutLine(startPoint, GREEN, 1, boxes);
	    }

	    //Bubble Sort Draw Section
	    if(BeginBubbleSort) {
		DrawOutLine(iterator, RAYWHITE, 1, boxes);
		DrawOutLine(currentTarget, RAYWHITE, 1, boxes);
		if(counter != 0) DrawOutLine(size - counter, GREEN, 1, boxes);
		iterate = true;
	    }
	 
	EndDrawing();
    }

    //Free Space and De-Initialize Screen
    FreeSpace(mat, boxes, size);

    CloseWindow();
    
    return 0;
}
