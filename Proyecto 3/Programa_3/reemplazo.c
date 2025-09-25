/*
                Project 3: Equipment replacement
                Made by: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
                Date: Friday, September 12, 2025
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

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

typedef struct {
    float* G;
    int* GPos;
} Solution;

/* ################################## REPLACEMENT ################################## */

float* inflationCosts(float cost, float inflationPercentage, int years){
    float inf = inflationPercentage + 1; //5% -> 105%

    float* inflation = malloc(sizeof(float) * (years+1));

    float currentCost = cost;
    inflation[0] = 0;
    for (int i = 1; i < years+1; i++){
        currentCost *= inf;
        inflation[i] = currentCost - cost;
    }

    return inflation;
}


float** calculateC(int years, int lifespan, float buyPrice, float* sellPrice, float* maintenance, float* inflation){

    float** C = malloc(sizeof(float*) * (years+1));


    for (int i = 0; i < years+1; i++){
        C[i] = malloc(sizeof(float) * (years+1));
        for (int j = 0; j < years+1; j++){
            C[i][j] = INT_MAX; // Infinite invalid cost
        }
    }




    for (int dif = 1; dif <= lifespan; ++dif){
        for (int t = 0; t+dif <= years; ++t){
            C[t][t+dif] = buyPrice - sellPrice[dif-1] + maintenance[dif-1] + inflation[t];
            printf("C[%d][%d]= %.2f = buy:%2.f - sell:%.2f + m:%.2f + inf:%.2f\n", t, t+dif, C[t][t+dif], buyPrice, sellPrice[dif-1], maintenance[dif-1], inflation[t]);
        }
    }

    return C;
}


Solution replacement(FILE* f, int years, int lifespan, float buyPrice, float* sellPrice, float* maintenance, float* inflation){

    float** C = calculateC(years, lifespan, buyPrice, sellPrice, maintenance, inflation);

    for (int i = 0; i < years+1; i++){
        if (i == 0){
            printf("C\t");
            for (int j = 0; j < years+1; j++){
                printf("%d\t", j);
            }
            printf("\n");
        }

        printf("%d\t", i);
        for (int j = 0; j < years+1; j++){
            if (C[i][j] >= 2147483648){
                printf("<->\t", C[i][j]);
            } else {
                printf("%2.f\t", C[i][j]);
            }
        }
        printf("\n");
    }

    float* G    = malloc(sizeof(float) * (years+1));
    int* GPos   = malloc(sizeof(int) * (years+1));   //Stores the binary code of the winners

    //Base case
    G[years]    = 0;
    GPos[years] = 0;


    for (int g = years-1; g >= 0; --g){

        G[g] = FLT_MAX;
        GPos[g] = 0;

        float option;
        printf("[ [ [ g=%d ] ] ]\n", g);

        //Print
        // minimo{
        for (int dif = 1; g+dif <= years && dif <= lifespan; dif++){
            //Marcar Proceso
            // Imprimir Calculo
            // G(4) = C[i][j] + G(j)
            // G(4) = %d + %d

            option = C[g][g+dif] + G[g+dif];
            printf("option: %.2f = C[%d][%d] + G[%d] = %.2f + %.2f\n",
                option, g, g+dif, g+dif, C[g][g+dif], G[g+dif]);
            
            if (option < G[g]){ //Better option
                G[g] = option;
                GPos[g] = ipow(2, dif); //Sets the digit of the candidate option

            } else if (option == G[g]) { //Tie option
                GPos[g] += ipow(2, dif); //Adds the digit of the next candidate option
            }
        }
        // }

        //Marcar G calculado
        // G(algo) = x
        // Ganadores son: 1, 2, 3
    }

    free(C);

    Solution sol;
    sol.G = G;
    sol.GPos = GPos;

    return sol;
}



/* ################################## TEX ################################## */



/* ################################## TEX ################################## */



/* ################################## MAIN ################################## */

void runReplacement(int years, int lifespan, float buyPrice, float* sellPrice, float* maintenance, float* inflation){

    FILE* f = fopen("programToLaTeX.tex", "w");
    if (f == NULL) {
        printf("Error: File null\n");
        return;
    }

    Solution solution = replacement(f, years, lifespan, buyPrice, sellPrice, maintenance, inflation);
    float* G = solution.G;
    int* GPos = solution.GPos;

    for (int g = 0; g < years+1; ++g){
        printf("G(%d) = %f (%d)\n", g, G[g], GPos[g]);
    }

    printf("\n\n");
    fclose(f);
}






void main(){
    int lifespan = 3;   //Lifespan
    float buyPrice = 500; //Buying price
    int years = 5;        //Years for the project
    float inflationPercentage = 0.05; //Inflation percentage


    float* sellPrice = malloc(sizeof(float)*lifespan);   //Selling price on the n year of use
    float* maintenance = malloc(sizeof(float)*lifespan); //Maintenance price (accumulative)
    float* inflation = inflationCosts(buyPrice, inflationPercentage, years);


    sellPrice[0] = 400;
    sellPrice[1] = 300;
    sellPrice[2] = 250;
    maintenance[0] = 30;
    maintenance[1] = 70;
    maintenance[2] = 130;


    runReplacement(years, lifespan, buyPrice, sellPrice, maintenance, inflation);
}


void test(){

    int lifespan = 4;   //Lifespan
    float buyPrice = 500; //Buying price
    int years = 7;        //Years for the project
    float inflationPercentage = 0.05; //Inflation percentage


    float* sellPrice = malloc(sizeof(float)*lifespan);   //Selling price on the n year of use
    float* maintenance = malloc(sizeof(float)*lifespan); //Maintenance price (accumulative)
    float* inflation = inflationCosts(buyPrice, inflationPercentage, years);


    sellPrice[0] = 400;
    sellPrice[1] = 300;
    sellPrice[2] = 250;
    sellPrice[3] = 100;
    maintenance[0] = 400;
    maintenance[1] = 300;
    maintenance[2] = 250;
    maintenance[3] = 100;


    runReplacement(years, lifespan, buyPrice, sellPrice, maintenance, inflation);
}