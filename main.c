#include<raylib.h>
#include<stdbool.h>
#include<stddef.h> 
#include<pthread.h>
#include<time.h>

//Required for GuiControls
#define RAYGUI_IMPLEMENTATION 
#include "raygui.h" 
//raygui embedded style && Colors for styles/dark.h 
#include "styles/StyleColors.h"
#include "styles/dark.h"

#include"utils.h"

//Screen Related Variables
//--------------------------------------------------------------------------------------------------|
static const int screenWidth = 1350;
static const int screenHeight = 680;
static const int max = 550;
static const int min = 5;
static const int shift = 1;
static const int heightPar = 1; 
static const int startX = 45;
static const int startY = 570;
static const int unitGap = 0;
static const int panelStartX = 0;
static const int panelStartY = 600; 
static const int panelWidth = 1350; 
static const int panelHeight = 80; 
static const int sliderMargin = 65; 
static const int sliderHeight = 22;
static const int sliderWidth = 602;
static const int sliderGap = 4; 
static const int buttonWidth = 615;
static const int buttonMargin = 20;
static const int sheetMarginX = 435; 
static const int sheetMarginY = 75; 
static const int frameMargin = 20;
static const int textMarginX = 40;
static const int textMarginY = 100;
static const int fontSize = 20;
static const int textGap = 6;
static const int titleMarginX = 125;
static const int titleMarginY = 40;
static const int titleFontSize = 25;
static const int maxSize = 420;
static const int minSize = 20;
static int	 triangleGap = 12;
static int	 unitWidth = 50; 
static double	 showControlSheet = false;
static const char *controls[] = {
	"Control Sheet", 
	"KEY_1 - selection sort",
	"KEY_2 - bubble sort", 
	"KEY_3 - insertion sort", 
	"KEY_4 - shaker sort",
	"KEY_5 - iterative merge sort",
	"KEY_6 - recursive merge sort",
	"KEY_7 - quick sort",
	"KEY_8 - heap sort",
	"KEY_9 - radix sort",
	"KEY_Q - stop current sort",
	"KEY_N - generate new matrix",
	"KEY_T - generate triangle matrix",
	"KEY_ENTER - close control sheet",
	"KEY_ESC - close application",
};

//Initializing Main Matrix
//--------------------------------------------------------------------------------------------------|
static int	size = 40;
static double	sizeScale = 40;
static int	currentSize = 40; 
static int      *mat; 
static int      *memoryFreeFlag; 
static char	size_str[10];

static struct Rectangle** boxes;

//Input Variables
//--------------------------------------------------------------------------------------------------|
static int	Input = 0;
static int	DuringSort = 0; 

//Time Interval Variables
//--------------------------------------------------------------------------------------------------|
static double	maxInterval = 4.000;
static double	minInterval = 0.0015;
static double	deltaTime = 0;
static double	stateTime = 0;
static double	timeInterval = 0.900;
static double	timeScale = 4.000 - 0.900;

//Common Sorting Variables
//--------------------------------------------------------------------------------------------------|
static int	iterator = 0;
static int	startPoint = 0;
static int	currentTarget = 0;
static bool	initDraw = false;
static bool	stopSorting = false;

//Selection Sort Variables
//--------------------------------------------------------------------------------------------------|
static bool	DrawSelectionSort = false;

//Bubble Sort Variables
//--------------------------------------------------------------------------------------------------|
static bool	DrawBubbleSort = false;
    
//Insertion Sort Variables
//--------------------------------------------------------------------------------------------------|
static bool	DrawInsertionSort = false;

//Shaker Sort Variables
static bool	DrawShakerSort = false;
static int	endPoint = 0;

//Merge Sort Variables
static bool	DrawRecMergeSort = false; 
static bool	DrawItMergeSort = false; 
static int	mid = 0;
static int	left_start = 0;
static int	right_end = 0;

//Quick Sort Variables
static bool	DrawQuickSort = false;

//Heap Sort Variables
static bool	DrawHeapSort = false;
static int	heap_parent = 0;
static int	heap_child = 0;

//Radix Sort Variables 
static bool	DrawRadixSort = false;
static int	countInd[10] = {0};
static bool	showIndex = false;
static int	chIn = 0;
static int	frIn = 0;

//Initializing mutex and thread ID
static pthread_mutex_t var_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t       sort_thread;
static struct timespec delta; 

//Functions 
static int	Min(int x, int y);
static void	swap(int *a, int *b);
static void	Reset();
static void	ThreadSleep();
static void	ThreadWake();
static void	*SelectionSortAlgo();
static void	*BubbleSortAlgo();
static void	*InsertionSortAlgo();
static void	*ShakerSortAlgo();
static void	*ItMergeSortAlgo();
static void	*RecMergeSortAlgo();
static int	MergeSortRec(int l, int r);
static int	Merge(int l, int m, int r);
static void	*QuickSortAlgo();
static int	QuickSortRec(int low, int high);
static int	Partition(int low, int high);
static void	*HeapSortAlgo();
static int	Heapify(int N, int i);
static int	Pow(int i, int j);
static void	*RadixSortAlgo();
static int	CountSort(int exp);

int main(void) {

    //Initializing Main MAtrix
    //--------------------------------------------------------------------------------------------------| 
    triangleGap = AdjustTriangleGap(size, min, max);
    unitWidth = AdjustUnitWidth(screenWidth, startX, shift, size);
    mat = GenerateTriangleMat(size, triangleGap); 
    boxes = GenerateBoxes(size, mat, unitWidth, shift, heightPar, startX, startY);
  
    //Initializing Screen and raugui style
    //--------------------------------------------------------------------------------------------------|
    SetTargetFPS(60);
    InitWindow(screenWidth, screenHeight, "Hopana!");
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
		case KEY_ONE:
		    DrawSelectionSort = true;
		    pthread_create(&sort_thread, NULL, SelectionSortAlgo, NULL);
		    break;
		//Begin Bubble Sort
		case KEY_TWO:
		    DrawBubbleSort = true;
		    pthread_create(&sort_thread, NULL, BubbleSortAlgo, NULL);	
		    break;
		//Begin Insertion Sort
		case KEY_THREE:
		    DrawInsertionSort = true;
		    pthread_create(&sort_thread, NULL, InsertionSortAlgo, NULL);
		    break;
		//Begin Shaker Sort
		case KEY_FOUR:
		    DrawShakerSort = true;
		    pthread_create(&sort_thread, NULL, ShakerSortAlgo, NULL);
		    break; //Begin Merge Sort
		case KEY_FIVE:
		    DrawItMergeSort = true;
		    pthread_create(&sort_thread, NULL, ItMergeSortAlgo, NULL);
		    break;
		case KEY_SIX:
		    DrawRecMergeSort = true;
		    pthread_create(&sort_thread, NULL, RecMergeSortAlgo, NULL);
		    break;
		//Begin Quick Sort
		case KEY_SEVEN:
		    DrawQuickSort = true;
		    pthread_create(&sort_thread, NULL, QuickSortAlgo, NULL);
		    break;
		//Begin Heap Sort
		case KEY_EIGHT:
		    DrawHeapSort = true;
		    pthread_create(&sort_thread, NULL, HeapSortAlgo, NULL);
		    break;
		case KEY_NINE:
		    DrawRadixSort = true;
                    pthread_create(&sort_thread, NULL, RadixSortAlgo, NULL);
		    break;
		//Close Control Sheet
		case KEY_ENTER: 
		    if(showControlSheet) {
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
		    stopSorting = true;
		    break; 
		//Close Control Sheet 
		case KEY_ENTER: 
		    showControlSheet = (showControlSheet)? false : true;
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

	    //Drawing size of matrix
	    sprintf(size_str, "%d", size);
	    DrawText(size_str, 10, 15, 12, FONT_COLOR);

	    //Sort Iteration Section 
	    //-------------------------------------------------------------------------------------------------------------------|
	    
	    //Selection Sort Draw Section 
	    if(DrawSelectionSort) {
		pthread_mutex_lock(&var_mutex);
		DrawOutLine(currentTarget, LIGHT_YELLOW_COLOR, unitGap, boxes);
		DrawOutLine(startPoint, GREEN_COLOR, unitGap, boxes);
		if(timeInterval < 0.06) DrawOutLine(iterator - 1, LIGHT_WHITE_COLOR, unitGap, boxes);
		if(iterator < size) DrawOutLine(iterator, LIGHT_WHITE_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }
	    //Bubble Sort Draw Section
	    else if(DrawBubbleSort) {
		pthread_mutex_lock(&var_mutex);
		if(startPoint != 0) DrawOutLine(size - 1 - startPoint, GREEN_COLOR, unitGap, boxes);
		DrawOutLine(iterator + 1, LIGHT_WHITE_COLOR, unitGap, boxes);
		DrawOutLine(iterator, LIGHT_WHITE_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);

	    }
	    //InsertionSort 
	    else if(DrawInsertionSort) {	
		pthread_mutex_lock(&var_mutex);
		DrawOutLine(startPoint, GREEN_COLOR, unitGap, boxes);
		DrawOutLine(iterator+1, LIGHT_WHITE_COLOR, unitGap, boxes);
		if(iterator != -1) DrawOutLine(iterator, LIGHT_WHITE_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }
	    //ShakerSort
	    else if(DrawShakerSort) {
		pthread_mutex_lock(&var_mutex);
		if(startPoint != 0) DrawOutLine(startPoint, GREEN_COLOR, unitGap, boxes);
		if(endPoint != size - 1) DrawOutLine(endPoint, GREEN_COLOR, unitGap, boxes);
		if(iterator != size - 1) DrawOutLine(iterator+1, LIGHT_WHITE_COLOR, unitGap, boxes);	
		DrawOutLine(iterator, LIGHT_WHITE_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }
	    //MergeSort
	    else if(DrawItMergeSort || DrawRecMergeSort) {
		pthread_mutex_lock(&var_mutex);
		DrawOutLine(mid, BLUE_COLOR, unitGap, boxes);
		DrawOutLine(left_start, DARK_BLUE_COLOR, unitGap, boxes);
		DrawOutLine(right_end, DARK_BLUE_COLOR, unitGap, boxes);
		DrawOutLine(iterator, LIGHT_WHITE_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }
	    //QuickSort
	    else if(DrawQuickSort) {
		pthread_mutex_lock(&var_mutex);
		if(currentTarget > -1) DrawOutLine(currentTarget, BLUE_COLOR, unitGap, boxes);
		DrawOutLine(startPoint, DARK_BLUE_COLOR, unitGap, boxes);
		DrawOutLine(iterator, LIGHT_WHITE_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }
	    //HeapSort 
	    else if(DrawHeapSort) {
		pthread_mutex_lock(&var_mutex);
		int i = 0;
		int j = 0;
		int end = 0;
		while(i < startPoint) {
		    i = Pow(2, j)-1;
		    end = i * 2;
		    if(j%2 == 0) { 
			for(; i <= end; i++) {
			    if(i >= startPoint) break;
			    DrawOutLine(i, LIGHT_PINK_COLOR, unitGap, boxes);
			} 
		    }
		    j++;
		}
		if(startPoint < size) DrawOutLine(startPoint, GREEN_COLOR, unitGap, boxes);
		DrawOutLine(heap_parent, LIGHT_WHITE_COLOR, unitGap, boxes);
		DrawOutLine(heap_child, LIGHT_WHITE_COLOR, unitGap, boxes);
		pthread_mutex_unlock(&var_mutex);
	    }
	    //Radix Sort
	    else if(DrawRadixSort) {
		pthread_mutex_lock(&var_mutex);
		if(showIndex) {
		    for(int i = 1; i < 10; i++)
			if(countInd[i] < size) DrawOutLine(countInd[i], LIGHT_YELLOW_COLOR, unitGap, boxes);
		    DrawOutLine(chIn, LIGHT_WHITE_COLOR, unitGap, boxes);
		    DrawOutLine(frIn, LIGHT_WHITE_COLOR, unitGap, boxes);
		} else {
		    DrawOutLine(iterator, LIGHT_WHITE_COLOR, unitGap, boxes);
		}
		pthread_mutex_unlock(&var_mutex);
	    }

		  
	    //GUI controls section
	    //------------------------------------------------------------------------------------------------------------------|

	    //Time Interval Control
	    DrawLine(panelStartX, panelStartY, panelWidth, panelStartY, UNIT_COLOR);
	    DrawRectangle(panelStartX, panelStartY, panelWidth, panelHeight, PANEL_COLOR); 
	    timeScale = GuiSliderBar((Rectangle){sliderMargin, panelStartY + (panelHeight - 2*sliderHeight - (double)sliderGap)/2, sliderWidth, sliderHeight}, "delay", NULL, timeScale, minInterval, maxInterval);
	    timeInterval = maxInterval - timeScale;
	    DrawRectangle(sliderMargin + sliderWidth - 10, panelStartY + (panelHeight - 2*sliderHeight - sliderGap)/2 - 2, 2, sliderHeight + 4, UNIT_COLOR);
	    
	    //Matrix Size Control
	    if(!initDraw) {
		sizeScale = GuiSliderBar((Rectangle){sliderMargin, panelStartY + sliderGap + sliderHeight + (panelHeight - 2*sliderHeight - (double)sliderGap)/2, 
							sliderWidth, sliderHeight}, "size", NULL, sizeScale, minSize, maxSize);
		if(size  != (int)sizeScale) {
		    size = (int)sizeScale;
		    if(size > 200) { 
			double per = timeScale/(maxInterval - minInterval); 
			maxInterval = 0.100; 
			timeScale = per*(maxInterval - minInterval); 
			timeInterval = maxInterval - timeScale; 
		    } else { 
			double per = timeScale/(maxInterval - minInterval); 
			maxInterval = 4.000; 
			timeScale = per*(maxInterval - minInterval); 
			timeInterval = maxInterval - timeScale; 
		    }
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
	    if(GuiButton((Rectangle){sliderMargin + sliderWidth + buttonMargin, panelStartY + (panelHeight - 2*sliderHeight - (double)sliderGap)/2, 
					buttonWidth, sliderHeight*2 + sliderGap}, "Show Control Sheet (Press ENTER)")) { 
		showControlSheet = (!showControlSheet)? true : false;
	    }
	    
	    //Control Sheet set up
	    if(showControlSheet) {
		//Draw Menu
		DrawRectangle(sheetMarginX, sheetMarginY, screenWidth - sheetMarginX*2, screenHeight - sheetMarginY*2, PANEL_COLOR );
		DrawRectangle(sheetMarginX + frameMargin, sheetMarginY + frameMargin, screenWidth - sheetMarginX*2 - frameMargin*2, screenHeight - sheetMarginY*2 - frameMargin*2, BACK_COLOR);
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
		for(int i = 0; i <= 13; i++) {
		    DrawText(controls[i+1], sheetMarginX + frameMargin + textMarginX, sheetMarginY + frameMargin + textMarginY + fontSize * i + textGap * i, fontSize, FONT_COLOR);  
		}
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
    DrawRadixSort = false;
    initDraw = false;
    stopSorting = false;
    mid = 0;
    left_start = 0;
    right_end = 0; 
    heap_child = 0;
    heap_parent = 0;
    showIndex = false;
}

static void SetDelta() {
    pthread_mutex_lock(&var_mutex);
    delta.tv_sec = 0;
    delta.tv_nsec = timeInterval * Pow(10, 8);
    pthread_mutex_unlock(&var_mutex);
}

static void *SelectionSortAlgo() {
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
	    //ThreadSleep(); 
	    SetDelta();
	    nanosleep(&delta, &delta);
	}

	if(ct != st) { 

	    pthread_mutex_lock(&var_mutex);
	    Swap(boxes, mat, currentTarget, startPoint);
	    pthread_mutex_unlock(&var_mutex);
    	
	    if(stopSorting) { 
		Reset();
		return NULL;	
	    }
	    //ThreadSleep();	
	    SetDelta();
	    nanosleep(&delta, &delta);
	}
    }

    Reset();
    return NULL;
}

static void *BubbleSortAlgo() {
    initDraw = true;
    int st;
    int i;

    for(st = 0; st < size - 1; st++) {
	for(i = 0; i < size - 1 - st; i++) {

	    pthread_mutex_lock(&var_mutex);
	    startPoint = st;
	    iterator = i;
	    pthread_mutex_unlock(&var_mutex);
	    
	    if(stopSorting) { 
		Reset();
		return NULL;	
	    }
	    //ThreadSleep();
	    SetDelta();
	    nanosleep(&delta, &delta);

	    if(mat[i] > mat[i + 1]) {
		pthread_mutex_lock(&var_mutex);
		Swap(boxes, mat, iterator, iterator + 1);
		pthread_mutex_unlock(&var_mutex);

		if(stopSorting) { 
		    Reset();
		    return NULL;	
		}
		//ThreadSleep();
		SetDelta();
		nanosleep(&delta, &delta);
	    }
	}
    }

    Reset();
    return NULL;
}

static void *InsertionSortAlgo() {
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
	    //ThreadSleep();
	    SetDelta();
	    nanosleep(&delta, &delta);

	    pthread_mutex_lock(&var_mutex);
	    Swap(boxes, mat, iterator, iterator+1);
	    pthread_mutex_unlock(&var_mutex);

	    if(stopSorting) {
		Reset();
		return NULL;
	    }
	    //ThreadSleep();
	    SetDelta();
	    nanosleep(&delta, &delta);

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
	    //ThreadSleep();
	    SetDelta();
	    nanosleep(&delta, &delta);
	}
    }
    
    Reset();
    return NULL;
}

static void *ShakerSortAlgo() {

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
	    //ThreadSleep();
	    SetDelta();
	    nanosleep(&delta, &delta);

	    if(mat[i] > mat[i+1]) {
		pthread_mutex_lock(&var_mutex);
		Swap(boxes, mat, iterator, iterator+1);
		pthread_mutex_unlock(&var_mutex);
		swapped = true;

		if(stopSorting) {
		    Reset();
		    return NULL;
		}
		//ThreadSleep();
		SetDelta();
		nanosleep(&delta, &delta);
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
	    //ThreadSleep();
	    SetDelta();
	    nanosleep(&delta, &delta);

	    if(mat[i] > mat[i+1]) {
		pthread_mutex_lock(&var_mutex);
		Swap(boxes, mat, iterator, iterator+1);
		pthread_mutex_unlock(&var_mutex);
		swapped = true;

		if(stopSorting) {
		    Reset();
		    return NULL;
		}
		//ThreadSleep();
		SetDelta();
		nanosleep(&delta, &delta);
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
		//ThreadSleep();
		SetDelta();
		nanosleep(&delta, &delta);

		pthread_mutex_lock(&var_mutex);
		iterator = md + 1 + j;
		pthread_mutex_unlock(&var_mutex);

		if(stopSorting) {
		    Reset();
		    return NULL;
		}
		//ThreadSleep();
		SetDelta();
		nanosleep(&delta, &delta);
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
		    //ThreadSleep();
		    SetDelta();
		    nanosleep(&delta, &delta);
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
		    //ThreadSleep();
		    SetDelta();
		    nanosleep(&delta, &delta);
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
		//ThreadSleep();
		SetDelta();
		nanosleep(&delta, &delta);
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
		//ThreadSleep();
		SetDelta();
		nanosleep(&delta, &delta);
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
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);

	pthread_mutex_lock(&var_mutex);
	iterator = m + 1 + j;
	pthread_mutex_unlock(&var_mutex);

	if(stopSorting) return -1;
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);
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
	    //ThreadSleep();
	    SetDelta();
	    nanosleep(&delta, &delta);
	} else {
	    mat[k] = R[j];
	    pthread_mutex_lock(&var_mutex);
	    iterator = k;
	    boxes[k]->height = mat[k] * heightPar;
	    boxes[k]->y = startY - mat[k] * heightPar;
	    pthread_mutex_unlock(&var_mutex);
	    j++;

	    if(stopSorting) return -1;
	    //ThreadSleep();
	    SetDelta();
	    nanosleep(&delta, &delta);
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
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);
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
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);
    }
    return 0;
}
 


static int MergeSortRec(int l, int r) {
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
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);

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
		//ThreadSleep();    
		SetDelta();
		nanosleep(&delta, &delta);
	    }
	}
    }

    pthread_mutex_lock(&var_mutex);
    Swap(boxes, mat, i+1, high);
    pthread_mutex_unlock(&var_mutex);

    if(stopSorting) return (-1);
    //ThreadSleep();
    SetDelta();
    nanosleep(&delta, &delta);

    return (i+1);
}

static void swap(int* a, int* b) {
    int k = *b;
    *b = *a;
    *a = k;
}

static int Heapify(int N, int i) {
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
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);

	pthread_mutex_lock(&var_mutex);
	Swap(boxes, mat, i, largest); 
	pthread_mutex_unlock(&var_mutex);

	if(stopSorting) return -1;
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);

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
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);

	pthread_mutex_lock(&var_mutex);
	startPoint = i;
	Swap(boxes, mat, 0, i); 
	pthread_mutex_unlock(&var_mutex);

	if(stopSorting) {
	    Reset();
	    return NULL;
	}
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);

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
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);
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
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);

	mat[count[(copy[i]/ exp) % 10]] = copy[i];
	pthread_mutex_lock(&var_mutex);
	boxes[count[(copy[i]/ exp) % 10]]->height = copy[i] * heightPar;
	boxes[count[(copy[i]/ exp) % 10]]->y = startY - copy[i] * heightPar;
	pthread_mutex_unlock(&var_mutex);
	count[(copy[i]/ exp) %10]++;

	if(stopSorting) return -1;
	//ThreadSleep();
	SetDelta();
	nanosleep(&delta, &delta);
    }
    showIndex = false;
    return 0;
} 


static void * RadixSortAlgo() {
    initDraw = true;
    int m = getMax(size);
    int res = 0;

    for(int exp = 1; m / exp > 0; exp *= 10) {
	res = CountSort(exp);
	if(res == -1) break;
    }

    Reset();
    return NULL;
}
