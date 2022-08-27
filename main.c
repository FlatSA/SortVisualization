#include<raylib.h>
#include<stdbool.h>
#include<stddef.h> 
#include<pthread.h>
//Required for GuiControls
#define RAYGUI_IMPLEMENTATION
#include"raygui.h"
//raygui embedded style #include "styles/dark.h"
//Colors for styles/dark.h
#include "styles/StyleColors.h"
#include "styles/dark.h"

#include"utils.h"

//Screen Related Variables
//--------------------------------------------------------------------------------------------------|
static const int screenWidth = 1350;
static const int screenHeight = 680;
static const int max = 270;
static const int min = 5;
static int unitWidth = 50; 
static const int shift = 1;
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
static const int maxSize = 1245;
static const int minSize = 7;
static int* mat; 
static int* memoryFreeFlag; static struct Rectangle** boxes;
static struct Rectange* copyBoxe;
static char size_str[10];

//Input Variables
//--------------------------------------------------------------------------------------------------|
static int Input = 0;
static int DuringSort = 0; static int SortType = none;

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
static bool initSort = true;
static bool initDraw = false;
static bool stopSorting = false;

//Selection Sort Variables
//--------------------------------------------------------------------------------------------------|
static bool DrawSelectionSort = false;

//Bubble Sort Variables
//--------------------------------------------------------------------------------------------------|
static bool DrawBubbleSort = false;
    
//Insertion Sort Variables
//--------------------------------------------------------------------------------------------------|
static bool DrawInsertionSort = false;

//Shaker Sort Variables
static bool DrawShakerSort = false;
static int  endPoint = 0;

//Merge Sort Variables
static bool DrawMergeSort = false; //?
static int mid = 0;
static int left_start = 0;
static int right_end = 0;

//Initializing mutex and thread ID
pthread_mutex_t var_mutex	= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;
pthread_t sort_thread/*, mainThread;*/;

//Functions 
//static void Draw();
static int Min(int x, int y);
static void Reset();
static void ThreadSleep();
static void ThreadWake();
static void *SelectionSortAlgo();
static void *BubbleSortAlgo();
static void *InsertionSortAlgo();
static void *ShakerSortAlgo();
static void *MergeSortAlgo();
static void MergeSortRec(int right, int left);

int main(void) {

    //Initializing Main MAtrix
    //--------------------------------------------------------------------------------------------------| 
    triangleGap = AdjustTriangleGap(size, min, max);
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
		    DrawSelectionSort = true;
		    break;
		//Begin Bubble Sort
		case KEY_B:
		    SortType = BubbleSort;
		    DrawBubbleSort = true;
		    break;
		//Begin Insertion Sort
		case KEY_I:
		    SortType = InsertionSort; 
		    DrawInsertionSort = true;
		    //currentTarget = 1;
		    break;
		//Begin Shaker Sort
		case KEY_FOUR:
		    SortType = ShakerSort;
		    DrawShakerSort = true;
		    break;
		case KEY_FIVE:
		    SortType = MergeSort;
		    DrawMergeSort = true;
		    break;
		//Close Control Sheet
		case KEY_ENTER: 
		    if(showControlSheet) {
			showControlSheet = false;
			break;
		    }
		    showControlSheet = true;
		    break;
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
			if(showControlSheet) {
			showControlSheet = false;
			break;
		    }
		    showControlSheet = true;
		    break;
	    }
	}

	//Ensure Time Interval and Make One Sort-Step per timeInterval
	//----------------------------------------------------------------------------------------------|	
	if(currentTime < 0.01f) currentTime = GetTime();
	deltaTime = GetTime() - currentTime;
	
	if((deltaTime > timeInterval) && Input) {
		currentTime = GetTime();
		
		if(SortType == SelectionSort) {
			if(initSort) { pthread_create(&sort_thread, NULL, SelectionSortAlgo, NULL);
				initSort = false;
			}	    
			ThreadWake();	
		}

		if(SortType == BubbleSort) {
			if(initSort) {
				pthread_create(&sort_thread, NULL, BubbleSortAlgo, NULL);
				initSort = false;
			}
			ThreadWake();
		}

		if(SortType == InsertionSort) {
			if(initSort) {
				pthread_create(&sort_thread, NULL, InsertionSortAlgo, NULL);
				initSort = false;
			}
			ThreadWake();	
		}

		if(SortType == ShakerSort) {
			if(initSort) {
				pthread_create(&sort_thread, NULL, ShakerSortAlgo, NULL);
				initSort = false;
			}
			ThreadWake();
		}	

		if(SortType == MergeSort) {
			if(initSort) {
				pthread_create(&sort_thread, NULL, MergeSortAlgo, NULL);
				initSort = false;
			}
			ThreadWake();
		}
	}

	//Draw Section
	//----------------------------------------------------------------------------------------------|
    	BeginDrawing();
	  
	    //Drawing Matrix in a Current state	
	    ClearBackground(BACK_COLOR);

	    pthread_mutex_lock(&var_mutex);
	    for (int i = 0; i < size; i++) {
		DrawRectangleRec(*boxes[i], UNIT_COLOR);
	    } 
	    pthread_mutex_unlock(&var_mutex);

	    //Draw size of matrix
	    sprintf(size_str, "%d", size);
	    DrawText(size_str, 10, 15, 12, RAYWHITE);
	   
	    //Sort Iteration Section 
	    //-------------------------------------------------------------------------------------------------------------------|
	    
	    //Selection Sort Draw Section 
	    if(DrawSelectionSort && initDraw) {
		pthread_mutex_lock(&var_mutex);
		DrawOutLine(currentTarget, SELECTION_TARGET_COLOR, unitGap, boxes);
		DrawOutLine(startPoint, SORTED_COLOR, unitGap, boxes);
	    	if(timeInterval < 0.06) DrawOutLine(iterator - 1, ITERATION_COLOR, unitGap, boxes);
		if(iterator < size) DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }

	    //Bubble Sort Draw Section
	    if(DrawBubbleSort && initDraw) {
		pthread_mutex_lock(&var_mutex);
    		if(startPoint != 0) DrawOutLine(size - 1 - startPoint, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(iterator + 1, ITERATION_COLOR, unitGap, boxes);
		DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);

	    }

	    //InsertionSort 
	    if(DrawInsertionSort && initDraw) {	
		pthread_mutex_lock(&var_mutex);
		DrawOutLine(startPoint, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(iterator+1, ITERATION_COLOR, unitGap, boxes);
		if(iterator != -1) DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }
	    
	    //ShakerSort
	    if(DrawShakerSort && initDraw) {
		pthread_mutex_lock(&var_mutex);
		if(startPoint != 0) DrawOutLine(startPoint, SORTED_COLOR, unitGap, boxes);
		if(endPoint != size - 1) DrawOutLine(endPoint, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		DrawOutLine(iterator+1, ITERATION_COLOR, unitGap, boxes);	
		pthread_mutex_unlock(&var_mutex);
	    }

	    //MergeSort
	    if(DrawMergeSort && initDraw) {
		pthread_mutex_lock(&var_mutex);
		DrawOutLine(mid, SORTED_COLOR, unitGap, boxes);
	    	DrawOutLine(left_start, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(right_end, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
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
	    if(GuiButton((Rectangle){sliderMargin + sliderWidth + buttonMargin, panelStartY + (panelHeight - 2*sliderHeight - sliderGap)/2, 
				    buttonWidth, sliderHeight*2 + sliderGap}, "Show Control Sheet (Press ENTER)")) { 
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

static int Min(int x, int y) {
	return (x<y)? x :y;
}

static int Max(int x, int y) {
	return (x>y)? x :y;
}

static void Reset() {
    Input = 0;
    SortType = none; 
    startPoint = 0; 
    currentTarget = 0;
    iterator = 0;
    DrawSelectionSort = false;
    DrawBubbleSort = false;
    DrawInsertionSort = false;
    DrawShakerSort = false;
    DrawMergeSort = false;
    initSort = true;
    initDraw = false;
    stopSorting = false;
    mid = 0;
    left_start = 0;
    right_end = 0;

    printf("Input: %d\n", Input);
    printf("SortType: %d\n", SortType);
    printf("startPoint: %d\n", startPoint);
    printf("currentTarget: %d\n", currentTarget);
    printf("iterator: %d\n", iterator);
    printf("DrawSelectionSort: %d\n", DrawSelectionSort);
    printf("DrawBubbleSort: %d\n", DrawBubbleSort);
    printf("DrawInsertionSort: %d\n", DrawInsertionSort);
    printf("DrawShakerSort: %d\n", DrawShakerSort);
    printf("DrawMergeSort: %d\n", DrawMergeSort);
    printf("initSort: %d\n", initSort);
    printf("initDraw: %d\n", initDraw);
    printf("stopSorting: %d\n", stopSorting);
    printf("mid: %d\n", mid);
    printf("left_start: %d\n", left_start);
    printf("right_end: %d\n", right_end);
    printf("\n");
}

static void ThreadSleep() {
    pthread_mutex_lock(&condition_mutex);
    pthread_cond_wait(&condition_cond, &condition_mutex);
    pthread_mutex_unlock(&condition_mutex);
}

static void ThreadWake() {
    pthread_mutex_lock(&condition_mutex);
    pthread_cond_signal(&condition_cond); 
    pthread_mutex_unlock(&condition_mutex);
}


static void *SelectionSortAlgo() {
    for(startPoint = 0; startPoint < size - 1; startPoint++) {
	currentTarget = startPoint;

	for(iterator = startPoint + 1; iterator < size; iterator++) {
	    
	    initDraw = true;

	    if(mat[currentTarget] > mat[iterator])
		currentTarget = iterator;

	    if(stopSorting){ 
		Reset();
		return NULL;
				
	    }
	    ThreadSleep();

	}

	if(currentTarget != startPoint) { 

	    pthread_mutex_lock(&var_mutex);
	    Swap(boxes, mat, currentTarget, startPoint);
	    pthread_mutex_unlock(&var_mutex);
			
	    if(stopSorting) { 
		Reset();
		return NULL;	
	    }
	    ThreadSleep();	
	}
    }

    Reset();
    return NULL;
}

static void *BubbleSortAlgo() {
    for(startPoint = 0; startPoint < size - 1; startPoint++) {
	for(iterator = 0; iterator < size - 1 - startPoint; iterator++) {
	    
	    initDraw = true;

	    if(stopSorting) { 
		Reset();
		return NULL;	
	    }
	    ThreadSleep();

	    if(mat[iterator] > mat[iterator + 1]) {
		    pthread_mutex_lock(&var_mutex);
		    Swap(boxes, mat, iterator, iterator + 1);
		    pthread_mutex_unlock(&var_mutex);

		    if(stopSorting) { 
			Reset();
			return NULL;	
		    }
		    ThreadSleep();
	    }

	}
    }

    Reset();
    return NULL;
}

static void *InsertionSortAlgo() {
    for(startPoint = 1; startPoint < size; startPoint++) {
	iterator = startPoint - 1;

	initDraw = true;
	while( iterator >= 0 && mat[iterator] > mat[iterator+1]) { 

	    if(stopSorting) {
		Reset();
		return NULL;
	    }
	    ThreadSleep();

	    Swap(boxes, mat, iterator, iterator+1);

	    if(stopSorting) {
		Reset();
		return NULL;
	    }
	    ThreadSleep();

	    iterator--;
	}	    

	if(iterator >= 0) {
	    if(stopSorting) {
		Reset();
		return NULL;
	    }
	    ThreadSleep();
	}

    }
    
    Reset();
    return NULL;
}

static void *ShakerSortAlgo() {
    bool swapped = true;
    startPoint = 0;
    endPoint = size - 1;

    while(swapped) {
	    
	swapped = false;
	
	for(iterator = startPoint; iterator < endPoint; iterator++) {

	    initDraw = true;

	    if(stopSorting) {
		Reset();
		return NULL;
	    }
	    ThreadSleep();
	    if(mat[iterator] > mat[iterator+1]) {
		Swap(boxes, mat, iterator, iterator+1);
		swapped = true;

		if(stopSorting) {
		    Reset();
		    return NULL;
		}
		ThreadSleep();
	    }
	}

	if(!swapped) {
	    Reset();
	    return NULL;
	}
	
	swapped = false;

	endPoint--;

	for(iterator = endPoint - 1; iterator >= startPoint; --iterator) {
	    if(stopSorting) {
		Reset();
		return NULL;
	    }
	    ThreadSleep();
	    if(mat[iterator] > mat[iterator+1]) {
		Swap(boxes, mat, iterator, iterator+1);
		swapped = true;

		if(stopSorting) {
		    Reset();
		    return NULL;
		}
		ThreadSleep();
	    }
	}	

	startPoint++;
    }

    Reset();
    return NULL;
}

static void *MergeSortAlgo() {
    int curr_size;

    for(curr_size=1; curr_size <= size-1 ; curr_size = 2*curr_size) {
	for(left_start = 0; left_start<size-1; left_start += 2*curr_size) {
	    mid = Min(left_start+curr_size-1, size-1);

	    right_end = Min(left_start + 2*curr_size - 1, size-1);

	    int i, j, k;
	    int n1 = mid - left_start + 1;
	    int n2 = right_end - mid;
	    int n3 = 0; 
	    if(n2 < n1) n3 = n2;
	    int L[n1], R[n2];

	    for(i = 0; i<n1; i++) {
		L[i] = mat[left_start + i];
	    }
	    for(j = 0; j<n2; j++) {
		R[j] = mat[mid + 1 + j];
	    }
	    
	    initDraw = true;
	    for(j = 0; j<n3; j++) {
		iterator = left_start + j;
		ThreadSleep();
		iterator = mid + 1 + j;
		ThreadSleep();
	    }

	    i = 0;
	    j = 0;
	    k = left_start;

	    while(i < n1 && j < n2) {
		iterator = k;
		if(L[i] <= R[j]) {
		    mat[k] = L[i];
		    pthread_mutex_lock(&var_mutex);
		    FreeBoxes(boxes, size);
		    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
		    pthread_mutex_unlock(&var_mutex);
		    i++;
		    ThreadSleep();
		} else {
		    pthread_mutex_lock(&var_mutex);
		    FreeBoxes(boxes, size);
		    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
		    pthread_mutex_unlock(&var_mutex);
		    mat[k] = R[j];
		    j++;
		    ThreadSleep();
		}
		k++;
	    } 

	    while(i < n1) {
		mat[k] = L[i];
		iterator = k;
		pthread_mutex_lock(&var_mutex);
		FreeBoxes(boxes, size);
		boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
		pthread_mutex_unlock(&var_mutex);
		i++;
		k++;
		ThreadSleep();
	    }

	    while(j < n2) {
		mat[k] = R[j];
		iterator = k;
		pthread_mutex_lock(&var_mutex);
		FreeBoxes(boxes, size);
		boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
		pthread_mutex_unlock(&var_mutex);
		j++;
		k++;
		ThreadSleep();
	    }
	    initDraw = false;
	}
    }
    Reset();
    return NULL;
}
