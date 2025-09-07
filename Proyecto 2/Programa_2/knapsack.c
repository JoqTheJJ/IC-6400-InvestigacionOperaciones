/*
                Project 2: Knapsack
                Made by: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
                Date: Friday, September 12, 2025

                Here goes the description

*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>





int ipow(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}



/* ################################## KNAPSACK ################################## */

typedef struct {
    int max;
    int ganadores;
} Cell;


void printCellMatrix(Cell** matrix, int rows, int cols){
    for (int r = 0; r < rows; ++r){
        if (r < 10)
            printf("0");

        printf("%d [ ", r);
        for (int c = 0; c < cols; ++c){

            int res = matrix[r][c].max;
            if (res < 10)
                printf(" ");

            printf("%d{%d}  ", res, matrix[r][c].ganadores);
        }
        printf("]\n");
    }
}

Cell** knapsack(int n, int maxCapacity, int* profits, int* costs, int* quantity){
    int capacity = maxCapacity + 1;

    Cell** res = malloc(sizeof(Cell*) * (capacity));
    for (int i = 0; i < capacity; ++i){
        res[i] = malloc(sizeof(Cell) * (n+1));
        res[i][0].max = 0; //Set first col to 0
    }

    for (int obj = 0; obj < n; ++obj){
        for (int c = 0; c < capacity; ++c){

            int max = -1;
            int newCapacity;
            int current;
            for (int q = 0; q <= quantity[obj]; ++q){

                newCapacity = c - q*costs[obj];
                if (newCapacity < 0){ //Exceeds capacity
                    break;
                }
                current = q*profits[obj] + res[newCapacity][obj].max; //es obj-1 implicito

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



/* ################################## MAIN ################################## */

int main() {

    //Inputs
    int n = 7;
    int capacity = 15;
    
    int* profits = malloc(sizeof(int) * n);
    profits[0] = 7;
    profits[1] = 9;
    profits[2] = 5;
    profits[3] = 12;
    profits[4] = 14;
    profits[5] = 6;
    profits[6] = 12;

    int* costs = malloc(sizeof(int) * n);
    costs[0] = 3;
    costs[1] = 4;
    costs[2] = 2;
    costs[3] = 6;
    costs[4] = 7;
    costs[5] = 3;
    costs[6] = 5;

    int* quantity = malloc(sizeof(int) * n);
    quantity[0] = 1;
    quantity[1] = 1;
    quantity[2] = 1;
    quantity[3] = 1;
    quantity[4] = 1;
    quantity[5] = 1;
    quantity[6] = 1;


    Cell** answer;
    answer = knapsack(n, capacity, profits, costs, quantity);

    printCellMatrix(answer, capacity+1, n+1);


    return 0;
}