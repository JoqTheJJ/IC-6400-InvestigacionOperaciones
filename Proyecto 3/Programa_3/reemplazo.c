/*
                Project 3: Equipment replacement
                Made by: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
                Date: Friday, September 12, 2025
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
    float inflationPercentage = 0.05; //Inflation percentage


    int* sellPrice = malloc(sizeof(int)*lifespan);   //Selling price on the n year of use
    int* maintenance = malloc(sizeof(int)*lifespan); //Maintenance price (accumulative (?))
    //(until lifespan+1 ???)


    int* inflation = malloc(sizeof(int)*lifespan);   //Inflation to the year n (accumulative)
    float currentCost = buyPrice;
    float newCost;
    for (int i = 0; i < max; ++i){
        newCost = currentCost*; //???
    }


    sellPrice[0] = 400;
    sellPrice[1] = 300;
    sellPrice[2] = 250;
    sellPrice[3] = 100;
    maintenance[0] = 400;
    maintenance[1] = 300;
    maintenance[2] = 250;
    maintenance[3] = 100;
    
    
    calculateC(); //Receives all of the above :P


    return 0;
}