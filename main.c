#include<raylib.h>
#include"utils.h"
#include<stdbool.h>
#include<stddef.h>
//Required for GuiControls
#define RAYGUI_IMPLEMENTATION
#include"raygui.h"

//raygui embedded styles 
#include "styles/style_terminal.h"

//Screen Related Variables
//--------------------------------------------------------------------------------------------------|
static const int screenWidth = 1350;
static const int screenHeight = 680;
static const int max = 270;
static const int min = 5;
static int unitWidth = 50;
static const int shift = 2;
static const int heightPar = 2;
static const int startX = 50;
static const int startY = screenHeight - 110;
static const int unitGap = 0;
static int triangleGap = 12;
//Gui control panel
static const int panelStartX = 0;
static const int panelStartY = screenHeight - 80; 
static const int panelWidth = screenWidth;
static const int panelHeight = 80;
static const int sliderMargin = 45;
static const int sliderHeight = 22;
static const int sliderWidth = (screenWidth - sliderMargin)/2 - 40;
static const int sliderGap = 4;

//Initializing Main Matrix
//--------------------------------------------------------------------------------------------------|
static int size = 20;
static double sizeScale = 20;
static int currentSize = 20; 
static int* mat; 
static int* memoryFreeFlag;
static struct Rectangle** boxes;

//Input Variables
//--------------------------------------------------------------------------------------------------|
static int Input = 0;
static int SortType = none;

//Time Interval Variables
//--------------------------------------------------------------------------------------------------|
static double currentTime = 0;
static double deltaTime = 0.f;
static double timeInterval = 0.20;
static double timeScale = 0.25;
static const double maxInterval = 0.600;
static const double minInterval = 0.;

//Common Sort Variables
//--------------------------------------------------------------------------------------------------|
static int iterator = 0;
static int startPoint = 0;
static int currentTarget = 0;
static bool iterate = true;
static bool elivateSort = false;

//Selection Sort Variables
//--------------------------------------------------------------------------------------------------|
static bool BeginSelectionSort = false;

//Bubble Sort Variables
//--------------------------------------------------------------------------------------------------|
static bool BeginBubbleSort = false;
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
    triangleGap = AdjustTriangleGap(size, min, max);
    unitWidth = AdjustUnitWidth(screenWidth, startX, shift, size);
    mat = GenerateTriangleMat(size, triangleGap); 
    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
  
    //Initializing Screen
    //--------------------------------------------------------------------------------------------------|
    SetTargetFPS(144);
    InitWindow(screenWidth, screenHeight, "HereWeGo");
    GuiLoadStyleTerminal();

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
		    //currentMat = mat;
		    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
		    Input = 0;
		    break;
		case KEY_T: 
		    FreeSpace(mat, boxes, size);
		    triangleGap = AdjustTriangleGap(size, min, max);
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
	
	//To stop current Sort (should check documentation)
	if(GetKeyPressed() == KEY_Q) 
		Reset();

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
			Swap(boxes, mat, currentTarget, startPoint);
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
			if(startPoint == size - 1) 
			    Reset();
			if(boxes[currentTarget]->height > boxes[iterator]->height) {
			    currentTarget = iterator;
			}
			
		    }
		   
		}

		//BubbleSort per Time Interval
		//--------------------------------------------------------------------------------------|
		if(SortType == BubbleSort && elivateSort) {
		    iterate = true; 
		    if(boxes[iterator]->height < boxes[currentTarget]->height) {
			Swap(boxes, mat, currentTarget, iterator);
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
			    Reset();
			}
		    }
		   
		}

		//InsertionSort per Time Interval
		//--------------------------------------------------------------------------------------|
		if(SortType == InsertionSort && elivateSort) {
			iterate = true;
			if(boxes[currentTarget]->height < boxes[iterator]->height){ 
			    Swap(boxes, mat, iterator, currentTarget); 
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
	    
	    //GUI controls section
	    //------------------------------------------------------------------------------------------------------------------|

	    //Time Interval Control
	    DrawLine(panelStartX, panelStartY, panelWidth, panelStartY, Fade(GRAY, 0.6f));
	    DrawRectangle(panelStartX, panelStartY, panelWidth, panelHeight, Fade(GRAY, 1.0f)); 
	    timeScale = GuiSliderBar((Rectangle){sliderMargin, panelStartY + (panelHeight - 2*sliderHeight - sliderGap)/2, sliderWidth, sliderHeight}, "speed", NULL, timeScale, minInterval, maxInterval);
	    timeInterval = maxInterval - timeScale;
	    
	    //Matrix Size Control
	    if(SortType == none) {
		sizeScale = GuiSliderBar((Rectangle){sliderMargin, panelStartY + sliderGap + sliderHeight + (panelHeight - 2*sliderHeight - sliderGap)/2, sliderWidth, sliderHeight}, "size", NULL, sizeScale, 5.0f, 78.0f);
		if(size  != (int)sizeScale) {
			size = (int)sizeScale;
			memoryFreeFlag = SubCopyMat(size, currentSize, mat, max, min); 	
			free(mat);
			mat = memoryFreeFlag;
			FreeBoxes(boxes, currentSize);
			unitWidth = AdjustUnitWidth(screenWidth, startX, shift, size);
			boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);	
			currentSize = size;
		}
	    }
	   
	    //Selection Sort Draw Section 
	    if(BeginSelectionSort) {
		if(iterator != size - 1) {
		    if(timeInterval < 0.15 && size >= 35) { 
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
		elivateSort = true;
		if(counter != 0) DrawOutLine(size - counter - 1, GREEN, unitGap, boxes);
		DrawOutLine(iterator, RAYWHITE, unitGap, boxes);
		DrawOutLine(currentTarget, RAYWHITE, unitGap, boxes);
	    }

	    //InsertionSort 
	    if(BeginInsertionSort) {
		elivateSort = true;
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
    elivateSort = false;
    BeginSelectionSort = false;
    BeginBubbleSort = false;
    BeginInsertionSort = false;
    counter = 0;
    endCycle = false;
}
