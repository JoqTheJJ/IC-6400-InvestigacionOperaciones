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





/* ################################## TEX ################################## */

void frameStart(FILE* f, int frameType, int number, char character){
    //Frame Type (title)
    //0 - Table (char)(number)

    fprintf(f, "\n\n\n\\begin{frame}\n");
    fprintf(f, "\\frametitle{");

    switch (frameType){
    case 0:
        fprintf(f, "Table %c$_{%d}$}\n", character, number);
        break;

    default:
        fprintf(f, "Floyd}\n");
        break;
    }
}

void frameEnd(FILE* f){
    fprintf(f, "\\end{frame}");
}


/* ################################## TEX ################################## */

void frameTable(int** m, int** changes, int size, int iteration, FILE* f, char c){

    frameStart(f, 0, iteration, c);


    fprintf(f, "\\begin{center}\n");

    fprintf(f, "    \\begin{tabular}{|c||");
    for (int col = 0; col < size; col++){
        fprintf(f, "c|");
    }
    fprintf(f, "}\n");



    fprintf(f, "        \\hline\n");
    fprintf(f, "        \\textbf{%c} ", c);
    for (int col = 0; col < size; col++){
        fprintf(f, "& \\textbf{%d} ", col);
    }
    fprintf(f, "\\\\\n        \\hline\n");
    fprintf(f, "        \\hline\n");


    for (int i = 0; i < size; ++i){
        fprintf(f, "        \\textbf{%d}", i);
        for (int j = 0; j < size; j++){
            if (changes[i][j]){
                fprintf(f, "& \\cellcolor{yellow}%d ", m[i][j]);
            } else {
                fprintf(f, "& %d ", m[i][j]);
            }
        }
        fprintf(f, "\\\\\n        \\hline\n");
    }

    fprintf(f, "    \\end{tabular}\n");
    fprintf(f, "\\end{center}\n\n\n");
    frameEnd(f);
}

void saveToTexFile(int** D, int** P, int** changes, int n, FILE* f, int iteration){
    
    frameTable(D, changes, n, iteration, f, 'D');

    frameTable(P, changes, n, iteration, f, 'P');


}

/* ################################## FLOYD ################################## */

void Floyd(int** D, int n, FILE* f){


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

        saveToTexFile(D, P, changes, n, f, node); //Save current state (before changes)

        for (int i = 0; i < n; i++){
            int sum;
            for (int j = 0; j < n; j++){

                changes[i][j] = 0; //No change done
                sum = D[i][node] + D[node][j];

                if (i != j && //No node to node
                D[i][j] > sum){  //Direct distance is greater than going through the node

                    D[i][j] =  sum;
                    P[i][j] = node;
                    changes[i][j] = 1; //Change registered

                }
            }
        }
    }

    saveToTexFile(D, P, changes, n, f, n); //Save final state


}

/* ################################## MAIN ################################## */

int main(int argc, char *argv[]) {

    srand(0);

    int nodes = 5;

    int** matrixConnections = malloc(sizeof(int*) * nodes); //Matrix D[0]

    for (int i = 0; i < nodes; i++){
        matrixConnections[i] = malloc(sizeof(int) * nodes);
        for (int j = 0; j < nodes; j++){
            matrixConnections[i][j] = rand() % 10 + 1;
        }
    }

    //File
    FILE* file;
    file = fopen("programToLaTeX.tex", "w");
    if (file == NULL) {
        printf("Error: File null\n");
        return 1;
    }


    Floyd(matrixConnections, nodes, file);




    //Free memory
    for (int i = 0; i < nodes; i++){
        free(matrixConnections[i]);
    }
    free(matrixConnections);

    return 0;
}