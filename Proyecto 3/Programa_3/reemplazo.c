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

int** calculateC(int years, int lifespan, int buyPrice, int* sellPrice, int* maintenance, int* inflation){

    int** C = malloc(sizeof(int*) * (lifespan+1));


    for (int i = 0; i < years; i++){
        C[i] = malloc(sizeof(int) * (lifespan+1));
        for (int j = 0; j < years; j++){
            C[i][j] = INT_MAX; // Infinite invalid cost
        }
    }




    for (int dif = 0; dif <= lifespan; ++dif){
        for (int t = 0; t+dif < years; ++t){
            C[t][t+dif] = buyPrice - sellPrice[dif] + maintenance[dif] + inflation[t];
        }
    }

    return C;
}


void replacement(int years, int lifespan, int buyPrice, int* sellPrice, int* maintenance, int* inflation){

    int** C = calculateC(years, lifespan, buyPrice, sellPrice, maintenance, inflation);

    int* G    = malloc(sizeof(int) * (years+1));
    int* GPos = malloc(sizeof(int) * (years+1)); //Stores the binary code of the winners

    //Base case
    G[years]    = 0;
    GPos[years] = 0;


    for (int g = years-1; g >= 0; --g){

        G[g] = INT_MAX;
        GPos[g] = 0;

        int option;

        //Print
        // minimo
        for (int dif = 1; g+dif < years; dif++){
            //Marcar Proceso
            // Imprimir Calculo
            // G(4) = C[i][j] + G(j)
            // G(4) = %d + %d


            option = C[g][g+dif] + G[g+dif];
            
            if (option < G[g]){ //Better option
                G[g] = option;
                GPos[g] = ipow(2, g+dif); //Sets the digit of the candidate option

            } else if (option == G[g]) { //Tie option
                GPos[g] += ipow(2, g+dif); //Adds the digit of the next candidate option
            }
        }
        //

        //Marcar G calculado
        // G(algo) = x
        // Ganadores son: 1, 2, 3
    }

    free(C);

}



/* ################################## TEX ################################## */



/* ################################## TEX ################################## */



/* ################################## MAIN ################################## */

int main(int argc, char *argv[]) {

    int lifespan = 4;   //Lifespan
    int buyPrice = 500; //Buying price
    int years = 7;        //Years for the project
    float inflationPercentage = 0.05; //Inflation percentage


    int* sellPrice = malloc(sizeof(int)*lifespan);   //Selling price on the n year of use
    int* maintenance = malloc(sizeof(int)*lifespan); //Maintenance price (accumulative (?))
    //(until lifespan+1 ???)


    int* inflation = malloc(sizeof(int)*lifespan);   //Inflation to the year n (accumulative)
    /*float currentCost = buyPrice;
    float newCost;
    for (int i = 0; i < years; ++i){
        newCost = currentCost*2; //???
    }*/


    sellPrice[0] = 400;
    sellPrice[1] = 300;
    sellPrice[2] = 250;
    sellPrice[3] = 100;
    maintenance[0] = 400;
    maintenance[1] = 300;
    maintenance[2] = 250;
    maintenance[3] = 100;
    inflation[0] = 0;
    inflation[1] = 10;
    inflation[2] = 20;
    inflation[3] = 30;
    inflation[4] = 40;
    inflation[5] = 50;
    inflation[6] = 60;
    



    //Title

    //Problem

    replacement(years, lifespan, buyPrice, sellPrice, maintenance, inflation);

    //Print Table


    return 0;
}