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
    const int unitWidth = 15;
    const int shift = 5;
    const int heightPar = 3;
    const int startX = 50;
    const int statY = screenHeight - 50;

    //Initialazing Main Matrix
    //--------------------------------------------------------------------------------------------------|
    const int size = 55;
    int* mat = GenerateMat(size, max, min); 
    struct Rectangle** boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, statY);

    //Input Variables
    //--------------------------------------------------------------------------------------------------|
    int Input = 0;
    int SortType = 0;

    //Time Interval Variables
    //--------------------------------------------------------------------------------------------------|
    float currentTime = 0;
    float deltaTime = 0.f;
    float timeInterval = 0.0125f;

    //Selection Sort Variables
    //--------------------------------------------------------------------------------------------------|
    int iterator = 0;
    int startPoint = 0;
    int currentTarget = startPoint;
    bool flag = true;
    bool BeginSelectionSort = false;

    //Initialazing Screen
    //--------------------------------------------------------------------------------------------------|
    SetTargetFPS(30);
    InitWindow(screenWidth, screenHeight, "HereWeGo");


    while(!WindowShouldClose()) {
	
	//Input Section
	//----------------------------------------------------------------------------------------------|
	if(Input == 0) {

	    Input = GetKeyPressed();

	    switch(Input) {
		//Generate New Matrix
		case KEY_N:
		    mat = GenerateMat(size, max, min);
		    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, statY);
		    Input = 0;
		    break;
		//Begin Selection Sort    
		case KEY_S:
		    SortType = SelectionSort;
		    break;
		default:
		    Input = 0;
	    }	
	}

	//Ensure Time Interval and Make One Sort Step per timeInterval
	//----------------------------------------------------------------------------------------------|	
	if(currentTime < 0.01f) currentTime = GetTime();
	deltaTime = GetTime() - currentTime;
	
	if((deltaTime > timeInterval) && Input) {
		currentTime = GetTime();
		
		//SelectionSort
		//--------------------------------------------------------------------------------------|
		if(SortType == SelectionSort) {
		    if(startPoint < size - 1) BeginSelectionSort = true;	
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
		    } else if(boxes[iterator]->height < boxes[currentTarget]->height) {
			currentTarget = iterator;
			flag = true;
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
	   
	    //Selection sort Draw Section 
	    if(BeginSelectionSort) {
		if(flag) DrawOutLine(currentTarget, ORANGE, 1, boxes);
		DrawOutLine(iterator, RAYWHITE, 1, boxes);
		DrawOutLine(startPoint, GREEN, 1, boxes);
	    }
	 
	EndDrawing();
    }

    //Free Space and De-Initialaze Screen
    FreeSpace(mat, boxes, size);

    CloseWindow();
    
    return 0;
}
