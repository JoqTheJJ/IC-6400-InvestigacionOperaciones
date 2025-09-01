/*
                Project 1: Shortest Paths (Floyd's Algorythm)
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



/* ################################## REPLACEMENT ################################## */

int** calculateC(int lifespan){

    int** C = malloc(sizeof(int*) * (lifespan+1));


    for (int i = 0; i < n; i++){
        C[i] = malloc(sizeof(int) * (lifespan+1));
        for (int j = 0; j < n; j++){
            C[i][j] = INT_MAX; // Infinite invalid cost
        }
    }




    for (int dif = 0; dif <= lifespan; ++dif){
        for (int t = 0; t+dif < max; ++t){
            C[t][t+dif] = buyPrice - sellPrice[dif] + maintenance[dif] + inflation[t];
        }
    }

    return C;
}


void replacement(){

    int** C = calculateC();

    int* G    = malloc(sizeof(int) * (max+1));
    int* GPos = malloc(sizeof(int) * (max+1)); //Stores the binary code of the winners

    //Base case
    G[max]    = 0;
    GPos[max] = 0;


    for (int g = max-1; g >= 0; --g){
        G[g] = INT_MAX;
        GPos[g] = 0;

        int option;
        for (int dif = 1; g+dif < max; dif++){
            option = C[g][g+dif] + G[g+dif];
            
            if (option < G[g]){ //Better
                G[g] = option;
                Gpos[g] = ipow(2, g+dif); //Sets the digit of the candidate option

            } else (option == G[g]) { //Draw
                Gpos[g] += ipow(2, g+dif); //Adds the digit of the next candidate option
            }
        }
    }
}



/* ################################## TEX ################################## */



/* ################################## TEX ################################## */



/* ################################## MAIN ################################## */

int main(int argc, char *argv[]) {

    int lifespan = 4;   //Lifespan
    int buyPrice = 500; //Buying price
    int max = 7;        //Years for the project

    int sellPrice[lifespan] = {400, 300, 250, 100};   //Selling price on the n year of use
    int maintenance[lifespan] = {400, 300, 250, 100}; //Maintenance price (accumulative (?))
    //(until lifespan+1 ???)

    int inflation[max] = {0, 20, 30, 40, 50, 60, 70}; //Inflation to the year n

    
    calculateC(); //Receives all of the above :P


    return 0;
}