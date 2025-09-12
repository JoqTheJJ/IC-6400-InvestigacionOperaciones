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

/* ################################## TEX ################################## */

void makeTitle(FILE* f){
    fprintf(f,
        "\\begin{titlepage}\n"
        "    \\centering\n"
        "    %% Logo\n"
        "    \\includegraphics[width=0.6\\textwidth]{logo-tec.png}\\par\\vspace{1cm}\n"
        "\n"
        "    %% University and course\n"
        "    {\\large Escuela de Ingeniería en Computación\\par}\n"
        "    {\\large Investigación de Operaciones\\par}\n"
        "    \\vspace{2cm}\n"
        "\n"
        "    %% Title\n"
        "    {\\Large Rutas Óptimas\\par}\n"
        "    {\\large Algoritmo de Floyd\\par}\n"
        "    \\vspace{2cm}\n"
        "\n"
        "    %% Group and professor\n"
        "    {\\large Grupo 40\\par}\n"
        "    {\\large Profesor: Francisco Torres Rojas\\par}\n"
        "    \\vspace{3cm}\n"
        "\n"
        "    %% Student info\n"
        "    {\\large Carmen Hidalgo Paz\\par}\n"
        "    {\\large Carné: 2020030538\\par}\n"
        "    \\vspace{1cm}\n"
        "    {\\large Melissa Carvajal Charpentier\\par}\n"
        "    {\\large Carné: 2022197088\\par}\n"
        "    \\vspace{1cm}\n"
        "    {\\large Josué Soto González\\par}\n"
        "    {\\large Carné: 2023207915\\par}\n"
        "    \\vspace{1cm}\n"
        "\n"
        "    %% Date\n"
        "    {\\large 12 de Septiembre del 2025\\par}\n"
        "\\end{titlepage}\n"
    );
}

void documentStart(FILE* f){
    fprintf(f, "\\documentclass{article}\n\n");

    //Packages
    fprintf(f, "\\PassOptionsToPackage{table,svgnames}{xcolor}");
    fprintf(f, "\\usepackage{graphicx}\n");
    fprintf(f, "\\usepackage{tikz-network}\n");
    fprintf(f, "\\usepackage{xcolor}\n\n");

    //Document start
    fprintf(f, "\\begin{document}\n");
    makeTitle(f);

    //Colors
    fprintf(f, "\\definecolor{KirbyPink}{HTML}{D74894}\n");
    fprintf(f, "\\definecolor{LightPink}{HTML}{FFBFBF}\n\n");
    fprintf(f, "\\newpage\n\n\n");
}

void introduction(FILE* f){
    // TODO para Melissa

    fprintf(f, "\\section{Knapsack}\n");
    fprintf(f, "This program consists of Floyd's algorithm to obtain the shortest path between any pair of nodes in a graph with weighted distances.\n");

    fprintf(f, "Floyd's algorithm compares the distance between any two given nodes and by passing through another city in between, if the result is less than the original then it chooses the shortest one. After contemplating all nodes in the graph, the graph is guaranteed to have all the shortest distances between any two nodes in the graph. These changes are recorded in another matrix called P that helps determine the shortest path between any two nodes.\n");

    fprintf(f, "\\section{Robert W. Floyd (1936–2001)}\n");

    fprintf(f, "\\begin{center}\n");
    fprintf(f, "\\includegraphics[width=0.25\\textwidth]{floyd.jpg}\n");
    fprintf(f, "\\end{center}\n");
    
    fprintf(f, "Robert Willoughby Floyd was a computer scientist that lived from 1936 to 2001. He made great advances in computer science and developed an algorithm to find the shortest paths between any two nodes for a directed graph. He was awarded a Turing Award in 1978.\n\n\n");
}

void texTable(int** m, int** changes, int size, int iteration, FILE* f, char c, char** names){
    //TODO
    /*

    texStart(f, 0, iteration, c);


    fprintf(f, "\\begin{center}\n");

    fprintf(f, "    \\begin{tabular}{|c||");
    for (int col = 0; col < size; col++){
        fprintf(f, "c|");
    }
    fprintf(f, "}\n");



    fprintf(f, "        \\hline\n");
    fprintf(f, "        \\textbf{%c} ", c);
    for (int col = 0; col < size; col++){
        fprintf(f, "& \\textbf{%s} ", names[col]);
    }
    fprintf(f, "\\\\\n        \\hline\n");
    fprintf(f, "        \\hline\n");


    for (int i = 0; i < size; ++i){
        fprintf(f, "        \\textbf{%s}", names[i]);
        for (int j = 0; j < size; j++){

            if (m[i][j] == 0){
                fprintf(f, "& $\\infty$ ");
            } else if (changes[i][j]){
                fprintf(f, "& \\cellcolor[HTML]{D74894}$%d$ ", m[i][j]);
            } else {
                fprintf(f, "& %d ", m[i][j]);
            }
        }
        fprintf(f, "\\\\\n        \\hline\n");
    }

    fprintf(f, "    \\end{tabular}\n");
    fprintf(f, "\\end{center}\n\n\n");
    texEnd(f);
    */
}

void printSolution(FILE* f, int* solution, int objects){
    printf("[");
    for (int obj = 0; obj < objects; ++obj){
        printf("x%d:%d ", obj, solution[obj]);
    }
    printf("]\n");
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

void optimalSolutionsAux(int row, int col, Cell** solution, int* res, int objects, int* costs, int* quantity, FILE* f){

    if (col == 0){
        //printf("Prints\n");
        printSolution(f, res, objects);
    }

    int winner = solution[row][col].ganadores;

    //printf("Row: %d, col: %d, winner %d\n", row, col, winner);

    for (int amount = 0; amount <= quantity[col-1]; amount++){
        //printf("Amount: %d, quantity[%d]=%d\n", amount, col-1, quantity[col-1]);
        if (winner % 2){
            res[col - 1] = amount; //Store the amount of the current object

            //printf("Goes to: %d\n", amount);
            optimalSolutionsAux(row - amount*costs[col-1], //row(capacity)) - amount * object cost
                col - 1,  //goes down a col
                solution, 
                res,
                objects,
                costs,
                quantity,
                f
            );
        }
        winner /= 2;
    }
}

void optimalSolutions(Cell** solution, int objects, int maxCapacity, int* costs, int* quantity, FILE* f){

    int* res = malloc(sizeof(int)*objects);
    optimalSolutionsAux(maxCapacity, objects, solution, res, objects, costs, quantity, f);
    free(res);
}

/* ################################## MAIN ################################## */



void main() {

    //Inputs
    int n = 7;
    int capacity = 9;
    
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

    printf("\n\n");

    optimalSolutions(answer, n, capacity, costs, quantity, NULL);

    printf("Holi?\n");
}