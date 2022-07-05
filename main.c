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
    const int unitWidth = 9;
    const int shift = 2;
    const int heightPar = 3;
    const int startX = 50;
    const int statY = screenHeight - 50;

    //Initializing Main Matrix
    //--------------------------------------------------------------------------------------------------|
    const int size = 100;
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
    double timeInterval = 0.0f;

    //Common Sort Variables
    //--------------------------------------------------------------------------------------------------|
    int iterator = 0;
    int startPoint = 0;
    int currentTarget = 0;
    bool iterate = true;

    //Selection Sort Variables
    //--------------------------------------------------------------------------------------------------|
    bool BeginSelectionSort = false;

    //Bubble Sort Variables
    //--------------------------------------------------------------------------------------------------|
    bool BeginBubbleSort = false;
    int counter = 0;
    
    //InsertionSort Sort Variables
    //--------------------------------------------------------------------------------------------------|
    bool BeginInsertionSort = false;
    bool endCycle = false;

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
			if(startPoint == 0) {
			    BeginSelectionSort = true;
			}	
			if(iterator < size -1) {
			    iterator++;
			} else {
			    startPoint++;
			    currentTarget = startPoint;
			    iterator = startPoint + 1;
			}
			if(startPoint == size - 1) {
			    BeginSelectionSort = false;
			    SortType = none;
			    iterate = true;
			    iterator = 0;
			    currentTarget = 0;
			    startPoint = 0;
			    Input = 0;
			}
			if(boxes[currentTarget]->height > boxes[iterator]->height) {
			    currentTarget = iterator;
			}
		    }
		   
		}

		//BubbleSort per Time Interval
		//--------------------------------------------------------------------------------------|
		if(SortType == BubbleSort) {
		    iterate = true; 
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

			if(counter == size - 1) {
			    BeginBubbleSort = false;
			    iterate = true;
			    SortType = none;
			    iterator = 0;
			    startPoint = 0;
			    currentTarget = 0;
			    counter = 0;
			    Input = 0;
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
			    SortType = none;
			    iterator = 0;
			    startPoint = 0;
			    currentTarget = 0;
			    counter = 0;
			    Input = 0;
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
		    DrawOutLine(currentTarget, ORANGE, 1, boxes);
		    DrawOutLine(iterator, RAYWHITE, 1, boxes);
		} else {
		    DrawOutLine(iterator, RAYWHITE, 1, boxes);
		    DrawOutLine(currentTarget, ORANGE, 1, boxes);
		}
		DrawOutLine(startPoint, GREEN, 1, boxes);
	    }

	    //Bubble Sort Draw Section
	    if(BeginBubbleSort) {
		if(counter != 0) DrawOutLine(size - counter - 1, GREEN, 1, boxes);
		DrawOutLine(iterator, RAYWHITE, 1, boxes);
		DrawOutLine(currentTarget, RAYWHITE, 1, boxes);
	    }

	    //InsertionSort 
	    if(BeginInsertionSort) {
		if(currentTarget != size)
		    DrawOutLine(currentTarget, RAYWHITE, 1, boxes);
		DrawOutLine(iterator, RAYWHITE, 1, boxes);
		if(startPoint < size - 1) 
		    DrawOutLine(startPoint + 1, GREEN, 1, boxes);
		
	    }
	 
	EndDrawing();
    }

    //Free Space and De-Initialize Screen
    FreeSpace(mat, boxes, size);

    CloseWindow();
    
    return 0;
}
