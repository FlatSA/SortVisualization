#include<raylib.h>
#include"utils.h"
#include<stdbool.h>
#include<stddef.h>
#include<pthread.h>
#define _OPEN_THREADS
#include<signal.h>
//Required for GuiControls
#define RAYGUI_IMPLEMENTATION
#include"raygui.h"

//raygui embedded style #include "styles/dark.h"
//Colors for styles/dark.h
#include "styles/StyleColors.h"
#include "styles/dark.h"

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
static const int sliderMargin = 65;
static const int sliderHeight = 22;
static const int sliderWidth = (screenWidth - sliderMargin)/2 - 40;
static const int sliderGap = 4;
static const int buttonWidth = 615;
static const int buttonMargin = 20;
static const int sheetMarginX = 425;
static const int sheetMarginY = 150;
static double showControlSheet = false;
static const int frameMargin = 20;
static const int textMarginX = 40;
static const int textMarginY = 100;
static const int fontSize = 20;
static const int textGap = 6;
static const int titleMarginX = 120;
static const int titleMarginY = 30;
static const int titleFontSize = 30;

//Initializing Main Matrix
//--------------------------------------------------------------------------------------------------|
static int size = 20;
static double sizeScale = 20;
static int currentSize = 20; 
static const int maxSize = 78;
static const int minSize = 7;
static int* mat; 
static int* memoryFreeFlag;
static struct Rectangle** boxes;
static struct Rectange* copyBoxe;

//Input Variables
//--------------------------------------------------------------------------------------------------|
static int Input = 0;
static int DuringSort = 0;
static int SortType = none;

//Time Interval Variables
//--------------------------------------------------------------------------------------------------|
static double currentTime = 0;
static double deltaTime = 0.f;
static double timeInterval = 0.20;
static double timeScale = 0.25;
static const double maxInterval = 0.500;
static const double minInterval = 0.;

//Common Sort Variables
//--------------------------------------------------------------------------------------------------|
static int iterator = 0;
static int startPoint = 0;
static int currentTarget = 0;
static bool iterate = true;
static bool elivateSort = false;
static bool initSort = true;
static bool stopSorting = false;

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

pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

//Common Sort Variables Reset Function 
static void Draw();
static void Reset();
static void *MainCycle();
static void *SelectionSortAlgo();
static void *BubbleSortAlgo();
static void *InsertionSortAlgo();

int main(void) {
    
    /*pthread_t mainThread;
    pthread_create(&mainThread, NULL, &MainCycle, NULL);
    pthread_join(mainThread, NULL);*/

    //Initializing Main MAtrix
    //--------------------------------------------------------------------------------------------------| triangleGap = AdjustTriangleGap(size, min, max);
    unitWidth = AdjustUnitWidth(screenWidth, startX, shift, size);
    mat = GenerateTriangleMat(size, triangleGap); 
    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
  
    //Initializing Screen and raugui style
    //--------------------------------------------------------------------------------------------------|
    SetTargetFPS(200);
    InitWindow(screenWidth, screenHeight, "Let it Sort!");
    GuiLoadStyleDark();

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
		//Generate Triangle Matrix
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
		//Begin Bubble Sort
		case KEY_B:
		    SortType = BubbleSort;
		    BeginBubbleSort = true;
		    startPoint++;
		    iterator++;
		    break;
		//Begin InsertionSort
		case KEY_I:
		    SortType = InsertionSort; 
		    BeginInsertionSort = true;
		    currentTarget = 1;
		    break;
		//Close Control Sheet
		case KEY_ENTER: 
		    showControlSheet = false;
		default:
		    Input = 0;
	    }	
	}
	
	//In case Sort is in Action
	if(Input != 0) {
	    DuringSort = GetKeyPressed();
	    
	    switch(DuringSort) {
		    //Stop sort
		    case KEY_Q: 
			//quit
			stopSorting = true;
			break; 
		    //Close Control Sheet 
		    case KEY_ENTER: 
			showControlSheet = false;
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
		/*if(SortType == SelectionSort) {
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
		   
		}*/
		if(SortType == SelectionSort) {
			if(initSort) {
				pthread_t sort_thread;
				pthread_create(&sort_thread, NULL, SelectionSortAlgo, NULL);
				initSort = false;
			}	    
			pthread_mutex_lock(&condition_mutex);
			pthread_cond_signal(&condition_cond); 
		        pthread_mutex_unlock(&condition_mutex);
		}

		//BubbleSort per Time Interval
		//--------------------------------------------------------------------------------------|
		/*if(SortType == BubbleSort && elivateSort) {
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
		   
		}*/
		if(SortType == BubbleSort) {
			if(initSort) {
				pthread_t sort_thread;
				pthread_create(&sort_thread, NULL, BubbleSortAlgo, NULL);
				initSort = false;
			}
			pthread_mutex_lock(&condition_mutex);
			pthread_cond_signal(&condition_cond); 
		        pthread_mutex_unlock(&condition_mutex);
		}

		//InsertionSort per Time Interval
		//--------------------------------------------------------------------------------------|
		/*if(SortType == InsertionSort && elivateSort) {
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
		}*/
		if(SortType == InsertionSort){
			if(initSort) {
				pthread_t sort_thread;
				pthread_create(&sort_thread, NULL, InsertionSortAlgo, NULL);
				initSort = false;
			}
			pthread_mutex_lock(&condition_mutex);
			pthread_cond_signal(&condition_cond); 
		        pthread_mutex_unlock(&condition_mutex);
		}
		
	}

	//Draw Section
	//----------------------------------------------------------------------------------------------|
    	BeginDrawing();
	  
	    //Drawing Matrix in a Current state	
	    ClearBackground(BACK_COLOR);

	    for (int i = 0; i < size; i++) {
		DrawRectangleRec(*boxes[i], UNIT_COLOR);
	    } 
	   
	    //Sort Iteration Section 
	    //-------------------------------------------------------------------------------------------------------------------|
	    
	    //Selection Sort Draw Section 
	    if(BeginSelectionSort) {
		/*if(iterator != size - 1) {
		    if(timeInterval < 0.05 && size >= 45) { 
		   	DrawOutLine(iterator - 1, ITERATION_COLOR, unitGap, boxes); 
		    }
		    DrawOutLine(currentTarget, SELECTION_TARGET_COLOR, unitGap, boxes);
		    DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		} else {
		    DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		    DrawOutLine(currentTarget, SELECTION_TARGET_COLOR, unitGap, boxes);
		}
		DrawOutLine(startPoint, SORTED_COLOR, unitGap, boxes);*/
		DrawOutLine(currentTarget, SELECTION_TARGET_COLOR, unitGap, boxes);
		DrawOutLine(startPoint, SORTED_COLOR, unitGap, boxes);
		if(iterator == size) DrawOutLine(iterator - 1, ITERATION_COLOR, unitGap, boxes);
		if(iterator < size) DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
	    }

	    //Bubble Sort Draw Section
	    if(BeginBubbleSort) {
		/*elivateSort = true;
		if(counter != 0) DrawOutLine(size - counter - 1, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		DrawOutLine(currentTarget, ITERATION_COLOR, unitGap, boxes);
		*/
		if(startPoint != 0) DrawOutLine(size - 1 - startPoint, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(iterator + 1, ITERATION_COLOR, unitGap, boxes);
		DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);

	    }

	    //InsertionSort 
	    if(BeginInsertionSort) {
		/*elivateSort = true;
		if(currentTarget != size)
		    DrawOutLine(currentTarget, ITERATION_COLOR, unitGap, boxes);
		DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		if(startPoint < size - 1) 
		    DrawOutLine(startPoint + 1, SORTED_COLOR, unitGap, boxes);
		*/
		DrawOutLine(startPoint, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(iterator+1, ITERATION_COLOR, unitGap, boxes);
		if(iterator != -1) DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
	    }

	    //GUI controls section
	    //------------------------------------------------------------------------------------------------------------------|

	    //Time Interval Control
	    DrawLine(panelStartX, panelStartY, panelWidth, panelStartY, UNIT_COLOR);
	    DrawRectangle(panelStartX, panelStartY, panelWidth, panelHeight, PANEL_COLOR); 
	    timeScale = GuiSliderBar((Rectangle){sliderMargin, panelStartY + (panelHeight - 2*sliderHeight - sliderGap)/2, sliderWidth, sliderHeight}, "speed", NULL, timeScale, minInterval, maxInterval);
	    timeInterval = maxInterval - timeScale;
	    
	    //Matrix Size Control
	    if(SortType == none) {
		sizeScale = GuiSliderBar((Rectangle){sliderMargin, panelStartY + sliderGap + sliderHeight + (panelHeight - 2*sliderHeight - sliderGap)/2, sliderWidth, sliderHeight}, "size", NULL, sizeScale, minSize, maxSize);
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

	    //Control Sheet Button 
	    if(GuiButton((Rectangle){sliderMargin + sliderWidth + buttonMargin, panelStartY + (panelHeight - 2*sliderHeight - sliderGap)/2, buttonWidth, sliderHeight*2 + sliderGap}, "Show Control Sheet")) { 
		    if(!showControlSheet) {
			showControlSheet = true;
		    } else {
			showControlSheet = false;
		    }
	    }
	    
	    //Control Sheet set up
	    if(showControlSheet) {
		    //Draw Menu
		    DrawRectangle(sheetMarginX, sheetMarginY, screenWidth - sheetMarginX*2, screenHeight - sheetMarginY*2, MENU_OUTER_GRAY );
		    DrawRectangle(sheetMarginX + frameMargin, sheetMarginY + frameMargin, screenWidth - sheetMarginX*2 - frameMargin*2, screenHeight - sheetMarginY*2 - frameMargin*2, MENU_INNER_GRAY);
		    //Draw Inner Frame
		    DrawLine(sheetMarginX + frameMargin, sheetMarginY + frameMargin, screenWidth - sheetMarginX - frameMargin, sheetMarginY + frameMargin, MENU_OUTLINE_COLOR);
		    DrawLine(sheetMarginX + frameMargin, sheetMarginY + frameMargin, sheetMarginX + frameMargin, screenHeight - sheetMarginY - frameMargin, MENU_OUTLINE_COLOR); 
		    DrawLine(sheetMarginX + frameMargin, screenHeight - sheetMarginY - frameMargin, screenWidth - sheetMarginX - frameMargin, screenHeight - sheetMarginY - frameMargin, MENU_OUTLINE_COLOR);
		    DrawLine(screenWidth - sheetMarginX - frameMargin, sheetMarginY + frameMargin, screenWidth - sheetMarginX - frameMargin, screenHeight - sheetMarginY - frameMargin, MENU_OUTLINE_COLOR);
		    //Draw Outer Frame
		    DrawLine(sheetMarginX, sheetMarginY, screenWidth - sheetMarginX, sheetMarginY, MENU_OUTLINE_COLOR);
		    DrawLine(sheetMarginX, sheetMarginY, sheetMarginX, screenHeight - sheetMarginY, MENU_OUTLINE_COLOR); 
		    DrawLine(sheetMarginX, screenHeight - sheetMarginY, screenWidth - sheetMarginX, screenHeight - sheetMarginY, MENU_OUTLINE_COLOR);
		    DrawLine(screenWidth - sheetMarginX, sheetMarginY, screenWidth - sheetMarginX, screenHeight - sheetMarginY, MENU_OUTLINE_COLOR);
		    //Draw Diag Frame 
		    DrawLine(sheetMarginX, sheetMarginY, sheetMarginX + frameMargin, sheetMarginY + frameMargin, MENU_OUTLINE_COLOR);
		    DrawLine(screenWidth - sheetMarginX - frameMargin, sheetMarginY + frameMargin, screenWidth - sheetMarginX, sheetMarginY, MENU_OUTLINE_COLOR); 
		    DrawLine(sheetMarginX, screenHeight - sheetMarginY, sheetMarginX + frameMargin, screenHeight - sheetMarginY - frameMargin, MENU_OUTLINE_COLOR);
		    DrawLine(screenWidth - sheetMarginX - frameMargin, screenHeight - sheetMarginY - frameMargin, screenWidth - sheetMarginX, screenHeight - sheetMarginY, MENU_OUTLINE_COLOR);
		    //Draw Text 
		    //DrawText(text, x, y, fontSize, Color)
		    //
		    DrawText("Control Sheet", sheetMarginX + frameMargin + titleMarginX, sheetMarginY + frameMargin + titleMarginY, titleFontSize, FONT_COLOR);
		    DrawText("KEY_I - to initiate insertion sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY, fontSize, FONT_COLOR);
		    DrawText("KEY_S - to initiate selection sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize + textGap, fontSize, FONT_COLOR);
		    DrawText("KEY_B - to initiate bubble sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*2 + textGap*2, fontSize, FONT_COLOR);
		    DrawText("KEY_Q - to stop current sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*3 + textGap*3, fontSize, FONT_COLOR);
		    DrawText("KEY_N - to generate new matrix", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*4 + textGap*4, fontSize, FONT_COLOR);
		    DrawText("KEY_T - to generate triangle matrix", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*5 + textGap*5, fontSize, FONT_COLOR);
		    DrawText("KEY_ENTER - to close control sheet", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*6 + textGap*6, fontSize, FONT_COLOR);
		    DrawText("KEY_ESC - to close application", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*7 + textGap*7, fontSize, FONT_COLOR);
	    }
	 
	EndDrawing();
    }

    //Free Space and De-Initialize Screen
    FreeSpace(mat, boxes, size);

    CloseWindow();
    return 0;
}

static void *MainCycle() {

  
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
    initSort = true;
    DuringSort = 0;
    stopSorting = false;
}

static void *SelectionSortAlgo() {
	for(startPoint = 0; startPoint < size - 1; startPoint++) {
		currentTarget = startPoint;
		for(iterator = startPoint + 1; iterator < size; iterator++) {
			if(mat[currentTarget] > mat[iterator])
				currentTarget = iterator;
			
			if(stopSorting){ 
				printf("after first");
				Reset();
				return NULL;
				
			}

			pthread_mutex_lock(&condition_mutex);
			pthread_cond_wait(&condition_cond, &condition_mutex);
			pthread_mutex_unlock(&condition_mutex);

	    	}
		if(currentTarget != startPoint) { 

			printf("%d\n", iterator);
		    	Swap(boxes, mat, currentTarget, startPoint);
			
			if(stopSorting){ 
				printf("after second");
				Reset();
				return NULL;

			}

			pthread_mutex_lock(&condition_mutex);
			pthread_cond_wait(&condition_cond, &condition_mutex);
			pthread_mutex_unlock(&condition_mutex);

		}

	}
	Reset();
	return NULL;
}

static void *BubbleSortAlgo() {
    for(startPoint = 0; startPoint < size - 1; startPoint++) {
	    for(iterator = 0; iterator < size - 1 - startPoint; iterator++) {
		    pthread_mutex_lock(&condition_mutex);
		    pthread_cond_wait(&condition_cond, &condition_mutex);
		    pthread_mutex_unlock(&condition_mutex);

		    if(mat[iterator] > mat[iterator + 1]) {
			    Swap(boxes, mat, iterator, iterator + 1);
			    pthread_mutex_lock(&condition_mutex);
			    pthread_cond_wait(&condition_cond, &condition_mutex);
			    pthread_mutex_unlock(&condition_mutex);

		    }
	    }
    }
    Reset();
}

static void *InsertionSortAlgo() {
	Rectangle unit;
	for(startPoint = 1; startPoint < size; startPoint++) {
	    iterator = startPoint - 1;
	    while( iterator >= 0 && mat[iterator] > mat[iterator+1]) { 
		pthread_mutex_lock(&condition_mutex);
		pthread_cond_wait(&condition_cond, &condition_mutex);
		pthread_mutex_unlock(&condition_mutex);

		Swap(boxes, mat, iterator, iterator+1);
		pthread_mutex_lock(&condition_mutex);
		pthread_cond_wait(&condition_cond, &condition_mutex);
		pthread_mutex_unlock(&condition_mutex);

		iterator--;
	    }	    
		pthread_mutex_lock(&condition_mutex);
		pthread_cond_wait(&condition_cond, &condition_mutex);
		pthread_mutex_unlock(&condition_mutex);
		
		 
	}

	Reset();
}
