#include<raylib.h>
#include<stdbool.h>
#include<stddef.h> 
#include<pthread.h>
//Required for GuiControls
#define RAYGUI_IMPLEMENTATION
#include"raygui.h" //raygui embedded style
//Colors for styles/dark.h 
#include "styles/StyleColors.h"
#include "styles/dark.h"

#include"utils.h"

//Screen Related Variables
//--------------------------------------------------------------------------------------------------|
static const int screenWidth = 1350;
static const int screenHeight = 680;
static const int max = 550;
static const int min = 5;
static int unitWidth = 50; static const int shift = 1;
static const int heightPar = 1; 
static const int startX = 50;
static const int startY = screenHeight - 110;
static const int unitGap = 0;
static int triangleGap = 12;
static int changed = true;
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
static const int sheetMarginY = 100; //150
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
static const int maxSize = 1000;
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
static bool DrawRecMergeSort = false;
static bool DrawItMergeSort = false; 
static int mid = 0;
static int left_start = 0;
static int right_end = 0;

//Quick Sort Variables
static bool DrawQuickSort = false;

//Heap Sort Variables
static bool DrawHeapSort = false;
static int heap_parent = 0;
static int heap_child = 0;

//Radix Sort Variables 
static bool DrawRadixSort = false;
static int countInd[10] = {0};
static bool showIndex = false;
static int chIn = 0;
static int frIn = 0;


//Initializing mutex and thread ID
pthread_mutex_t var_mutex	= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;
pthread_t sort_thread/*, mainThread;*/;

//Functions 
//static void Draw();
static int Min(int x, int y);
static void swap(int *a, int *b);
static void Reset();
static void ThreadSleep();
static void ThreadWake();
static void *SelectionSortAlgo();
static void *BubbleSortAlgo();
static void *InsertionSortAlgo();
static void *ShakerSortAlgo();
static void *ItMergeSortAlgo();
static void *RecMergeSortAlgo();
static int MergeSortRec(int l, int r);
static int Merge(int l, int m, int r);
static void *QuickSortAlgo();
static int QuickSortRec(int low, int high);
static int Partition(int low, int high);
static void *HeapSortAlgo();
static int Heapify(int N, int i);
static int Pow(int i, int j);
static void *RadixSortAlgo();
static int CountSort(int exp);
static int getMax();

int main(void) {

    //Initializing Main MAtrix
    //--------------------------------------------------------------------------------------------------| 
    triangleGap = AdjustTriangleGap(size, min, max);
    unitWidth = AdjustUnitWidth(screenWidth, startX, shift, size);
    mat = GenerateTriangleMat(size, triangleGap); 
    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
  
    //Initializing Screen and raugui style
    //--------------------------------------------------------------------------------------------------|
    SetTargetFPS(300);
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
		    changed = true;
		    Input = 0;
		    break;
		//Generate Triangle Matrix
		case KEY_T: 
		    FreeSpace(mat, boxes, size);
		    triangleGap = AdjustTriangleGap(size, min, max);
		    mat = GenerateTriangleMat(size, triangleGap); 
		    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
		    changed = true;
		    Input = 0;
		    break;
		//Begin Selection Sort    
		case KEY_ONE:
		    SortType = SelectionSort; 
		    DrawSelectionSort = true;
		    break;
		//Begin Bubble Sort
		case KEY_TWO:
		    SortType = BubbleSort;
		    DrawBubbleSort = true;
		    break;
		//Begin Insertion Sort
		case KEY_THREE:
		    SortType = InsertionSort; 
		    DrawInsertionSort = true;
		    //currentTarget = 1;
		    break;
		//Begin Shaker Sort
		case KEY_FOUR:
		    SortType = ShakerSort;
		    DrawShakerSort = true;
		    break; //Begin Merge Sort
		case KEY_FIVE:
		    SortType = ItMergeSort;
		    DrawItMergeSort = true;
		    break;
		case KEY_SIX:
		    SortType = RecMergeSort;
		    DrawRecMergeSort = true;
		    break;
		//Begin Quick Sort
		case KEY_SEVEN:
		    SortType = QuickSort;
		    DrawQuickSort = true;
		    break;
		//Begin Heap Sort
		case KEY_EIGHT:
		    SortType = HeapSort;
		    DrawHeapSort = true;
		    break;
		case KEY_NINE:
		    SortType = RadixSort;
		    DrawRadixSort = true;
		    break;
		//Close Control Sheet
		case KEY_ENTER: 
		    if(showControlSheet) {
			changed = true;
			showControlSheet = false;
		    } else {
			showControlSheet = true;
		    }
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
			/*if(showControlSheet) {
			    showControlSheet = false;
			} else {
			    showControlSheet = true;
			}*/
			showControlSheet = (showControlSheet)? false : true;
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

		if(SortType == ItMergeSort) {
			if(initSort) {
				pthread_create(&sort_thread, NULL, ItMergeSortAlgo, NULL);
				initSort = false;
			}
			ThreadWake();
		}

		if(SortType == RecMergeSort) {
			if(initSort) {
				pthread_create(&sort_thread, NULL, RecMergeSortAlgo, NULL); 
				initSort = false;
			}
			ThreadWake();
		}

		if(SortType == QuickSort) {
			if(initSort) {
				pthread_create(&sort_thread, NULL, QuickSortAlgo, NULL);
				initSort = false;
			}
			ThreadWake();
		}

		if(SortType == HeapSort) {
			if(initSort) {
				pthread_create(&sort_thread, NULL, HeapSortAlgo, NULL);
				initSort = false;
			}
	    		ThreadWake();
		}

		if(SortType == RadixSort) {
			if(initSort) {
				pthread_create(&sort_thread, NULL, RadixSortAlgo, NULL);
				initSort = false;
			}
			ThreadWake();
		}
	}

	//Draw Section
	//----------------------------------------------------------------------------------------------|
    	BeginDrawing();
	if(changed) {  

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
		if(iterator != size - 1) DrawOutLine(iterator+1, ITERATION_COLOR, unitGap, boxes);	
		DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }

	    //MergeSort
	    if((DrawItMergeSort || DrawRecMergeSort) && initDraw) {
		pthread_mutex_lock(&var_mutex);
		DrawOutLine(mid, SORTED_COLOR, unitGap, boxes);
	    	DrawOutLine(left_start, SELECTION_TARGET_COLOR, unitGap, boxes);
		DrawOutLine(right_end, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }

	    //QuickSort
	    if(DrawQuickSort && initDraw) {
		pthread_mutex_lock(&var_mutex);
		if(currentTarget > -1) DrawOutLine(currentTarget, SELECTION_TARGET_COLOR, unitGap, boxes);
		DrawOutLine(startPoint, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }

	    //HeapSort 
	    if(DrawHeapSort && initDraw) {
		pthread_mutex_lock(&var_mutex);
		int i = 0;
		int j = 0;
		int end = 0;
		while(i < startPoint) {
		    i = Pow(2, j)-1;
		    end = i * 2;
		    if(j%2 == 0) {
			for( ;i <= end; i++) {
			    if(i >= startPoint) break;
			    DrawOutLine(i, ITERATION_COLOR, unitGap, boxes);
			} 
		    }
		    j++;
		}
		if(startPoint < size) DrawOutLine(startPoint, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(heap_parent, SORTED_COLOR, unitGap, boxes);
		DrawOutLine(heap_child, SORTED_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }

	    if(DrawRadixSort && initDraw) {
		pthread_mutex_lock(&var_mutex);
		if(showIndex) {
		    for(int i = 1; i < 10; i++)
			if(countInd[i] < size) DrawOutLine(countInd[i], SELECTION_TARGET_COLOR, unitGap, boxes);
		    DrawOutLine(chIn, SORTED_COLOR, unitGap, boxes);
		    DrawOutLine(frIn, SORTED_COLOR, unitGap, boxes);
		} else {
		    DrawOutLine(iterator, ITERATION_COLOR, unitGap, boxes);
		}
		pthread_mutex_unlock(&var_mutex);
	    }

	    if(!initDraw && (deltaTime > timeInterval)) changed = false;
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
			changed = true;
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
		    /*if(!showControlSheet) {
			showControlSheet = true;
		    } else {
			showControlSheet = false;
		    }*/
		    changed = true;
		    showControlSheet = (!showControlSheet)? true : false;
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
		    DrawText("KEY_1 - selection sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY, fontSize, FONT_COLOR);
		    DrawText("KEY_2 - bubble sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize + textGap, fontSize, FONT_COLOR);
		    DrawText("KEY_3 - insertion sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*2 + textGap*2, fontSize, FONT_COLOR);
		    DrawText("KEY_4 - shaker sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*3 + textGap*3, fontSize, FONT_COLOR);
		    DrawText("KEY_5 - iterative merge sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*4 + textGap*4, fontSize, FONT_COLOR);
		    DrawText("KEY_6 - recursive merge sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*5 + textGap*5, fontSize, FONT_COLOR);
		    DrawText("KEY_7 - quick sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*6 + textGap*6, fontSize, FONT_COLOR);
		    DrawText("KEY_8 - heap sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*7 + textGap*7, fontSize, FONT_COLOR);
		    DrawText("KEY_Q - stop current sort", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*8 + textGap*8, fontSize, FONT_COLOR);
		    DrawText("KEY_N - generate a new matrix", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*9 + textGap*9, fontSize, FONT_COLOR);
		    DrawText("KEY_T - generate a triangle matrix", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*10 + textGap*10, fontSize, FONT_COLOR);
		    DrawText("KEY_ENTER - close control sheet", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*11 + textGap*11, fontSize, FONT_COLOR);
		    DrawText("KEY_ESC - close application", sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize*12 + textGap*12, fontSize, FONT_COLOR);

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
    DrawSelectionSort = false;
    DrawBubbleSort = false;
    DrawInsertionSort = false;
    DrawShakerSort = false;
    DrawItMergeSort = false;
    DrawRecMergeSort = false;
    DrawQuickSort = false;
    DrawHeapSort = false;
    initSort = true;
    initDraw = false;
    stopSorting = false;
    mid = 0;
    left_start = 0;
    right_end = 0; heap_child = 0;
    heap_parent = 0;
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
    changed = true;
    initDraw = true;
    int st;
    int i;
    int ct;

    for(st = 0; st < size - 1; st++) {
	ct = st;

	pthread_mutex_lock(&var_mutex);
	startPoint = st;
	currentTarget = st;
	pthread_mutex_unlock(&var_mutex);

	for(i = st + 1; i < size; i++) {

	    pthread_mutex_lock(&var_mutex);
	    iterator = i;
	    pthread_mutex_unlock(&var_mutex);

	    if(mat[currentTarget] > mat[iterator]) {
		ct = i;

		pthread_mutex_lock(&var_mutex);
		currentTarget = i;
		pthread_mutex_unlock(&var_mutex);
	    }

	    if(stopSorting){ 
		Reset();
		return NULL;
				
	    }
	    ThreadSleep(); }

	if(ct != st) { 

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
    changed = true;
    int st;
    int i;

    for(st = 0; st < size - 1; st++) {
	for(i = 0; i < size - 1 - st; i++) {

	    pthread_mutex_lock(&var_mutex);
	    startPoint = st;
	    iterator = i;
	    pthread_mutex_unlock(&var_mutex);
	    
	    initDraw = true;

	    if(stopSorting) { 
		Reset();
		return NULL;	
	    }
	    ThreadSleep();

	    if(mat[i] > mat[i + 1]) {
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
    changed = true;
    int height;
    int y;
    int key;
    int st;
    int i;

    for(st = 1; st < size; st++) {
	
	i = st - 1;
	pthread_mutex_lock(&var_mutex);
	startPoint = st;
	iterator = i;
	pthread_mutex_unlock(&var_mutex);

	initDraw = true;
	while(i >= 0 && mat[i] > mat[i+1]) { 

	    if(stopSorting) {
		Reset();
		return NULL;
	    }
	    ThreadSleep();

	    pthread_mutex_lock(&var_mutex);
	    Swap(boxes, mat, iterator, iterator+1);
	    pthread_mutex_unlock(&var_mutex);

	    if(stopSorting) {
		Reset();
		return NULL;
	    }
	    ThreadSleep();

	    i--;
	    pthread_mutex_lock(&var_mutex);
	    iterator--;
	    pthread_mutex_unlock(&var_mutex);
	}	    
	
	if(i>= 0) {
	    
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
    changed = true;

    bool swapped = true;
    int strP = 0;
    int endP = size - 1;

    pthread_mutex_lock(&var_mutex);
    startPoint = strP;
    endPoint = endP;
    pthread_mutex_unlock(&var_mutex);

    while(swapped) {
	    
	swapped = false;
	
	for(int i = strP; i < endP; i++) {
	    pthread_mutex_lock(&var_mutex);
	    iterator = i;
	    pthread_mutex_unlock(&var_mutex);

	    initDraw = true;

	    if(stopSorting) {
		Reset();
		return NULL;
	    }
	    ThreadSleep();

	    if(mat[i] > mat[i+1]) {
		pthread_mutex_lock(&var_mutex);
		Swap(boxes, mat, iterator, iterator+1);
		pthread_mutex_unlock(&var_mutex);
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

	endP--;
	pthread_mutex_lock(&var_mutex);
	endPoint--;
	pthread_mutex_unlock(&var_mutex);

	for(int i = endP - 1; i >= strP; i--) {
	    pthread_mutex_lock(&var_mutex);
	    iterator = i;
	    pthread_mutex_unlock(&var_mutex);

	    if(stopSorting) {
		Reset();
		return NULL;
	    }
	    ThreadSleep();
	    if(mat[i] > mat[i+1]) {
		pthread_mutex_lock(&var_mutex);
		Swap(boxes, mat, iterator, iterator+1);
		pthread_mutex_unlock(&var_mutex);
		swapped = true;

		if(stopSorting) {
		    Reset();
		    return NULL;
		}
		ThreadSleep();
	    }
	}	

	strP++;
	pthread_mutex_lock(&var_mutex);
	startPoint++;
	pthread_mutex_unlock(&var_mutex);
    }

    Reset();
    return NULL;
}

static int Min(int x, int y) {
	return (x<y)? x :y;
}

static void *ItMergeSortAlgo() {
    changed = true;
    int curr_size;
    int ls;
    int re;
    int md;

    for(curr_size = 1; curr_size <= size - 1 ; curr_size = 2 * curr_size) {
	for(ls = 0; ls<size - 1; ls += 2*curr_size) {

	    md = Min(ls + curr_size - 1, size - 1);
	    re = Min(ls + 2*curr_size - 1, size - 1);

	    pthread_mutex_lock(&var_mutex);
	    mid = md;
	    left_start = ls;
	    right_end = re;
	    pthread_mutex_unlock(&var_mutex);

	    int i, j, k;
	    int n1 = md - ls + 1;
	    int n2 = re - md;
	    int n3 = n2; 
	    int L[n1], R[n2];

	    for(i = 0; i<n1; i++) {
		L[i] = mat[left_start + i];
	    }
	    for(j = 0; j<n2; j++) {
		R[j] = mat[mid + 1 + j];
	    }
	    
	    initDraw = true;
	    for(j = 0; j<n3; j++) {

		pthread_mutex_lock(&var_mutex);
		iterator = ls + j;
		pthread_mutex_unlock(&var_mutex);

		if(stopSorting) {
		    Reset();
		    return NULL;
		}
		ThreadSleep();

		pthread_mutex_lock(&var_mutex);
		iterator = md + 1 + j;
		pthread_mutex_unlock(&var_mutex);

		if(stopSorting) {
		    Reset();
		    return NULL;
		}
		ThreadSleep();
	    }

	    i = 0;
	    j = 0;
	    k = ls;

	    while(i < n1 && j < n2) {
		if(L[i] <= R[j]) {
		    mat[k] = L[i];
		    pthread_mutex_lock(&var_mutex);
		    iterator = k;
		    boxes[k]->height = mat[k] * heightPar;
		    boxes[k]->y = startY - mat[k] * heightPar;
		    pthread_mutex_unlock(&var_mutex);
		    i++;

		    if(stopSorting) {
			Reset();
			return NULL;
		    }
		    ThreadSleep();
		} else {
		    mat[k] = R[j];
		    pthread_mutex_lock(&var_mutex);
		    iterator = k;
		    boxes[k]->height = mat[k] * heightPar;
		    boxes[k]->y = startY - mat[k] * heightPar;
		    pthread_mutex_unlock(&var_mutex);
		    j++;

		    if(stopSorting) {
			Reset();
			return NULL;
		    }
		    ThreadSleep();
		}
		k++;
	    } 

	    while(i < n1 && n2 != 0) {
		mat[k] = L[i];
		pthread_mutex_lock(&var_mutex);
		iterator = k;
		boxes[k]->height = mat[k] * heightPar;
		boxes[k]->y = startY - mat[k] * heightPar;
		pthread_mutex_unlock(&var_mutex);
		i++;
		k++;

		if(stopSorting) {
		    Reset();
		    return NULL;
		}
		ThreadSleep();
	    }

	    while(j < n2 && n1 != 0) {
		mat[k] = R[j];
		pthread_mutex_lock(&var_mutex);
		iterator = k;
		boxes[k]->height = mat[k] * heightPar;
		boxes[k]->y = startY - mat[k] * heightPar;
		pthread_mutex_unlock(&var_mutex);
		j++;
		k++;

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

static int Merge(int l, int m, int r) {

    pthread_mutex_lock(&var_mutex);
    left_start = l;
    mid = m;
    right_end = r;
    pthread_mutex_unlock(&var_mutex);

    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    int n3 = n2; 
    int L[n1], R[n2];

    for(i = 0; i<n1; i++) {
	L[i] = mat[left_start + i];
    }
    for(j = 0; j<n2; j++) {
	R[j] = mat[mid + 1 + j];
    }
	    
    initDraw = true;
    for(j = 0; j<n3; j++) {

	pthread_mutex_lock(&var_mutex);
	iterator = l + j;
	pthread_mutex_unlock(&var_mutex);

	if(stopSorting) return -1; 
	ThreadSleep();

	pthread_mutex_lock(&var_mutex);
	iterator = m + 1 + j;
	pthread_mutex_unlock(&var_mutex);

	if(stopSorting) return -1;
	ThreadSleep();
    }

    i = 0;
    j = 0;
    k = l;

    while(i < n1 && j < n2) {
	if(L[i] <= R[j]) {
	    mat[k] = L[i];
	    pthread_mutex_lock(&var_mutex);
	    iterator = k;
	    boxes[k]->height = mat[k] * heightPar;
	    boxes[k]->y = startY - mat[k] * heightPar;
	    pthread_mutex_unlock(&var_mutex);
	    i++;

	    if(stopSorting) return -1;
	    ThreadSleep();
	} else {
	    mat[k] = R[j];
	    pthread_mutex_lock(&var_mutex);
	    iterator = k;
	    boxes[k]->height = mat[k] * heightPar;
	    boxes[k]->y = startY - mat[k] * heightPar;
	    pthread_mutex_unlock(&var_mutex);
	    j++;

	    if(stopSorting) return -1;
	    ThreadSleep();
	}
	k++;
    } 

    while(i < n1 && n2 != 0) {
	mat[k] = L[i];
	pthread_mutex_lock(&var_mutex);
	iterator = k;
	boxes[k]->height = mat[k] * heightPar;
	boxes[k]->y = startY - mat[k] * heightPar;
	pthread_mutex_unlock(&var_mutex);
	i++;
	k++;

	if(stopSorting) return -1;
	ThreadSleep();
    }

    while(j < n2 && n1 != 0) {
	mat[k] = R[j];
	pthread_mutex_lock(&var_mutex);
	iterator = k;
	boxes[k]->height = mat[k] * heightPar;
	boxes[k]->y = startY - mat[k] * heightPar;
	pthread_mutex_unlock(&var_mutex);
	j++;
	k++;

	if(stopSorting) return -1;
	ThreadSleep();
    }
    return 0;
}
 


static int MergeSortRec(int l, int r) {
    changed = true;
    if(l < r) {
	int md = l + (r - l)/2;
	int res;

	res = MergeSortRec(l, md);
	if(res == -1) return - 1;
	res = MergeSortRec(md+1, r);
	if(res == -1) return -1;
	res = Merge(l, md, r);
	if(res == -1) return - 1;
    }
    return 0;
}

static void *RecMergeSortAlgo() {
    MergeSortRec(0, size-1); 
    Reset();
    return NULL;
}

static void *QuickSortAlgo() {
    QuickSortRec(0, size - 1);
    Reset();
    return NULL;
}

static int QuickSortRec(int low, int high) {
     int ret;
     if(low < high) {
	int pi = Partition(low, high);

	if(pi == -1) return -1;
	ret = QuickSortRec(low, pi - 1);

	if(ret == -1 || pi == -1) return -1;
	ret = QuickSortRec(pi + 1, high);
     } 
     return 0;
}

static int Partition(int low, int high) {
    changed = true;
    int pivot = mat[high];
    int i = (low - 1);

    pthread_mutex_lock(&var_mutex);
    startPoint = high;
    currentTarget = i;
    iterator = low;
    pthread_mutex_unlock(&var_mutex);

    initDraw = true;

    for(int j = low; j<=high - 1; j++) {

	pthread_mutex_lock(&var_mutex);
	iterator = j;
	pthread_mutex_unlock(&var_mutex);

	if(stopSorting) return (-1);
	ThreadSleep();

	if(mat[j] < pivot) {
	    i++;
	    
	    pthread_mutex_lock(&var_mutex);
	    currentTarget = i;
	    pthread_mutex_unlock(&var_mutex);

	    if(i != j) {
		pthread_mutex_lock(&var_mutex);
		Swap(boxes, mat, i, j);
		pthread_mutex_unlock(&var_mutex);

		if(stopSorting) return (-1);
		ThreadSleep();    
	    }
	}
    }

    pthread_mutex_lock(&var_mutex);
    Swap(boxes, mat, i+1, high);
    pthread_mutex_unlock(&var_mutex);

    if(stopSorting) return (-1);
    ThreadSleep();

    return (i+1);
}

static void swap(int* a, int* b) {
    int k = *b;
    *b = *a;
    *a = k;
}

static int Heapify(int N, int i) {
    changed = true;
    initDraw = true;
    
    int largest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    if(left < N && mat[left] > mat[largest])
	largest = left;

    if(right < N && mat[right] > mat[largest])
	largest = right;

    if(largest != i) {
	pthread_mutex_lock(&var_mutex);
	heap_child = i;
	heap_parent = largest;
	pthread_mutex_unlock(&var_mutex);

	if(stopSorting) return -1;
	ThreadSleep();

	pthread_mutex_lock(&var_mutex);
	Swap(boxes, mat, i, largest); 
	pthread_mutex_unlock(&var_mutex);

	if(stopSorting) return -1;
	ThreadSleep();

	Heapify(N, largest);
    }
    return 0;
}

static void *HeapSortAlgo() {
    int res;
    int N = size;
    pthread_mutex_lock(&var_mutex);
    startPoint = N;
    pthread_mutex_unlock(&var_mutex);

    for(int i = N/2 - 1; i>= 0; i--) 
	res = Heapify(N, i);

    if(res == -1) {
	Reset();
	return NULL;
    }

    for(int i = N-1; i>= 0; i--) {

	pthread_mutex_lock(&var_mutex);
	heap_child = i;
	heap_parent = 0;
	pthread_mutex_unlock(&var_mutex);

	if(stopSorting) {
	    Reset();
	    return NULL;
	}
	ThreadSleep();

	pthread_mutex_lock(&var_mutex);
	startPoint = i;
	Swap(boxes, mat, 0, i); 
	pthread_mutex_unlock(&var_mutex);

	if(stopSorting) {
	    Reset();
	    return NULL;
	}
	ThreadSleep();

	res = Heapify(i, 0); 
	if(res == -1) {
	    Reset();
	    return NULL;
	}

    }

    Reset();
    return NULL;
}

static int Pow(int i, int j){
    if(j == 0) return 1;
    int res = 1;
    for(int x = 0; x <= j-1; x++) {
	res *= i;
    }
    return res;
}

int getMax(int n) {
    int mx = mat[0];
    for(int i = 1; i < n; i++) 
	if(mat[i] > mx)
	    mx = mat[i];
    return mx;
}

static int CountSort(int exp) {
    int output[size];
    int copy[size];
    int i, count[10] = {0};

    for(i = 0; i < size; i++) {
	pthread_mutex_lock(&var_mutex);
	iterator = i;
	pthread_mutex_unlock(&var_mutex);
	count[(mat[i] / exp) % 10]++;

	if(stopSorting) return -1;
	ThreadSleep();
    }

    for(i = 1; i < 10; i++) {
	count[i] += count[i-1];
    }

    for(i = 9; i >=1 ; i--) {
	count[i] = count[i-1];
    }
    count[0] = 0;

    for(i = 0; i < 10; i++) {
	countInd[i] = count[i];
    }

    showIndex = true;

    for(i = 0; i < size; i++) {
	copy[i] = mat[i];
    }

    for(i = 0; i < size; i++) {
	pthread_mutex_lock(&var_mutex);
	chIn = count[(copy[i]/ exp) % 10];
	frIn = i;
	pthread_mutex_unlock(&var_mutex);

	if(stopSorting) return -1;
	ThreadSleep();

	mat[count[(copy[i]/ exp) % 10]] = copy[i];
	pthread_mutex_lock(&var_mutex);
	boxes[count[(copy[i]/ exp) % 10]]->height = copy[i] * heightPar;
	boxes[count[(copy[i]/ exp) % 10]]->y = startY - copy[i] * heightPar;
	pthread_mutex_unlock(&var_mutex);
	count[(copy[i]/ exp) %10]++;

	if(stopSorting) return -1;
	ThreadSleep();
    }
    showIndex = false;
    return 0;
} 


static void * RadixSortAlgo() {
    initDraw = true;
    changed = true;
    int m = getMax(size);
    int res = 0;

    for(int exp = 1; m / exp > 0; exp *= 10) {
	res = CountSort(exp);
	if(res == -1) break;
    }

    Reset();
    return NULL;
}
