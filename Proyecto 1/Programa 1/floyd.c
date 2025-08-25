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




void Floyd(int** D, int n){


    int** P = malloc(sizeof(int*) * n); //Matrix P

    for (int i = 0; i < n; i++){
        P[i] = malloc(sizeof(int) * n);
        for (int j = 0; j < n; j++){
            P[i][j] = 0; // 0 state (No change done)
        }
    }

    int** changes = malloc(sizeof(int*) * n); //Changes made to D/P

    for (int i = 0; i < n; i++){
        changes[i] = malloc(sizeof(int) * n);
        for (int j = 0; j < n; j++){
            changes[i][j] = 0; // 0 state (No change done)
        }
    }




    for (int node = 0; node < n; node++){

        saveToTexFile(D, P, changes, n) //Save current state (before changes)

        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){

                changes[i][j] = 0; //No change done
                sum = D[i][nodo] + D[nodo][j];

                if (i != j && //No node to node
                D[i][j] > sum){  //Direct distance is greater than going through the node

                    D[i][j] =  sum;
                    P[i][j] = node;
                    changes[i][j] = 1; //Change registered

                }
            }
        }
    }

    saveToTexFile(D, P, changes, n) //Save final state




}


void saveToTexFile(int** D, int** P, int** changes, int n){
    //saves (adds) it to a file

    //text
    for (int row = 0; row < n; row++){
        //saveRow
        for (int col = 0; col < n; col++){

            if (change[i][j]){ //Checks if a change happened
                //saveCell WITH color (changed)
            } else {
                //saveCell WITHOUT color (unchanged)
            }
        }
    }
    //text

    //new page
}


int main(int argc, char *argv[]) {

    int nodes = 5;

    int** matrixConnections = malloc(sizeof(int*) * nodes); //Matrix D[0]

    for (int i = 0; i < nodes; i++){
        matrixConnections[i] = malloc(sizeof(int) * nodes);
        for (int j = 0; j < nodes; j++){
            matrixConnections[i][j] = 10;
        }
    }




    Floyd(matrixConnections, nodes);




    //Free memory
    for (int i = 0; i < nodes; i++){
        free(matrixConnections[i]);
    }
    free(matrixConnections)

    return 0;
}