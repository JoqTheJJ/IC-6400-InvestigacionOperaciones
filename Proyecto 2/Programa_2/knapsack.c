/*
                Project 2: Knapsack
                Made by: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
                Date: Friday, September 12, 2025

                Here goes the description

*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <cairo.h>
#include <ctype.h>





int ipow(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}



/* ################################## KNAPSACK ################################## */

struct Cell {
    int max;
    int ganadores;
}


Cell** knapsack(int n, int capacity, int* profits, int* costs, int* quantity){

    Cell** res = malloc(sizeof(Cell*) * (capacity));
    for (int i = 0; i < capacity; ++i){
        res[i] = malloc(sizeof(Cell) * (n+1));
        res[i][0].max = 0; //Set first col to 0
    }

    for (int obj = 0; obj < n; ++obj){
        for (int c = 0; c < capacity; ++c){

            int max = -1;
            int newCapacity;
            for (int q = 0; q < quantity[obj]; ++q){
                newCapacity = capacity - q*costs[obj];
                current = q*profits[obj] + res[newCapacity][obj]; //es obj-1 implicito

                if (current > max){
                    max = current;
                    res[c][obj+1].max = current;
                    res[c][obj+1].ganadores = ipow(2, q);

                } else if (current == max){
                    res[c][obj+1].ganadores += ipow(2, q);
                }
            }
        }
    }

    return res;
}









/* ################################## TEX ################################## */



/* ################################## TEX ################################## */



/* ################################## MAIN ################################## */

int main(int argc, char *argv[]) {

        
    calculateC(); //Receives all of the above :P


    return 0;
}