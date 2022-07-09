#!/bin/bash
#Works only on GNU Linux 
#Requires raylib installation
set -e 

[ -z `ldconfig -p | grep libraylib` ] && echo "Please, install raylib first." || gcc main.c utils.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o SortVisual && ./SortVisual
