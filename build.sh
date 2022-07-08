set -e 
gcc main.c utils.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o SortVisual
./SortVisual
