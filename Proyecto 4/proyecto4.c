/*
                Project 4: Simplex
                Made by: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <cairo.h>
#include <ctype.h>





/* ################################## TEX ################################### */

void makeTitle(FILE* f){

    fprintf(f,
        "\\begin{titlepage}\n"
        "    \\centering\n"
        "    %% Logo\n"
        "    \\includegraphics[width=0.6\\textwidth]{logo-tec.png}\\par\\vspace{1cm}\n"
        "\n"
        "    %% University and course\n"
        "    {\\large Computer Science\\par}\n"
        "    {\\large Operations Research\\par}\n"
        "    \\vspace{2cm}\n"
        "\n"
        "    %% Title\n"
        "    {\\Simplex Algoritm\\par}\n"

        "    \\vspace{2cm}\n"
        "\n"
        "    %% Group and professor\n"
        "    {\\large Group 40\\par}\n"
        "    {\\large Professor: Francisco Torres Rojas\\par}\n"
        "    \\vspace{3cm}\n"
        "\n"
        "    %% Student info\n"
        "    {\\large Carmen Hidalgo Paz\\par}\n"
        "    {\\large Id: 2020030538\\par}\n"
        "    \\vspace{1cm}\n"
        "    {\\large Melissa Carvajal Charpentier\\par}\n"
        "    {\\large Id: 2022197088\\par}\n"
        "    \\vspace{1cm}\n"
        "    {\\large Josué Soto González\\par}\n"
        "    {\\large Id: 2023207915\\par}\n"
        "    \\vspace{1cm}\n"
        "\n"
        "    %% Date\n"
        "    {\\large 26 september 2025\\par}\n"
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
    fprintf(f, "\\usepackage{pgfplots}\n\n");
    //Document information
    fprintf(f, "\\begin{document}\n");

    makeTitle(f);
    //fprintf(f, "\\title{Graph Theory - Operations Research}\n");
    //fprintf(f, "\\author{Melissa Carvajal, Carmen Hidalgo \\& Josu\\'e Soto}\n");
    //fprintf(f, "\\institute{Investigaci\\'on de Operaciones}\n");
    //fprintf(f, "\\date{2025}\n\n");

    //begin
    fprintf(f, "\\definecolor{KirbyPink}{HTML}{D74894}\n");
    fprintf(f, "\\definecolor{LightPink}{HTML}{FFBFBF}\n\n");
    //fprintf(f, "\\maketitle\n\n");
    fprintf(f, "\\newpage\n\n\n");
}

void introduction(FILE* f){
    fprintf(f, "\\section{The Simplex Algorithm}\n");

    fprintf(f, "The simplex algorithm, developed by George Dantzig in 1947, arises from the need to solve linear programming problems.m\n");
    fprintf(f, "This problem was fundamentally proposed by Kantorovich and Koopman, who developed the optimal location problem and the problem of resources.\n");

    fprintf(f, "The Simplex method optimizes an objective function subject to linear constraints, using an iterative process to improve the value of the objective function until the optimal solution is reached. Its ability to solve complex problems and its use in various applications make it an essential tool in the optimization of resources and strategic decisions in industry, economics, and operations research.\\\\\n");
    fprintf(f, "Given the time of its development, it was essentially thought to be solved by hand; however, now there are digital tools that allow the process to be automated. \n");

    fprintf(f, "\\subsection{George Dantzig}\n");
    fprintf(f, "The American mathematician was born in 1914 and died in 2005. In addition to being the creator of the Simplex algorithm, he was head of the Scientific Computing of Operations Research (SCOOP), where he promoted linear programming for strategic purposes during World War II.\n");
}
/* ################################ SIMPLEX ################################# */

typedef struct {
    int x;
    int y;
    int decisiones;
} Pivot;


void pivotRow(double* fila, Pivot piv, int cols){
    double k = 1/fila[piv.y];

    for (int e = 0; e < cols; ++e){
        fila[e] = fila[e] * k;
    }
}

int fractions(double** matriz, int cols, int rows, int y){

    double min = 1125899906842624;
    int decisiones = 0;
    int x = -1;
    for (int r = 1; r < rows; ++r){
        double frac = matriz[r][cols-1] / matriz[r][y];

        if (matriz[r][y] > 0 && frac > 0 && frac < min){ //b positivo
            // frac < min escoge en primero en caso de empate

            min = frac;
            x = r;
            decisiones = 0;

        } else if (matriz[r][y] > 0 && frac >= 0 && min == 1125899906842624){ //0 degenerado
            // frac < min escoge en primero en caso de empate

            min = frac;
            x = r;
            decisiones = 0;
        } else if (matriz[r][y] > 0 && frac >= 0 && frac == min){
            decisiones++;
        }
    }

    //piv->decisiones = decisiones;
    return x;
}

Pivot escogerPivote(double** matriz, int cols, int rows, int maximize){

    Pivot piv;
    piv.x = -1;
    piv.y = -1;
    piv.decisiones = 0;

    if (maximize){ //Maximize
        double mostNegative = 0.1;
        for (int c = 1; c < cols-1; ++c){
            if (matriz[0][c] < 0 && matriz[0][c] < mostNegative){
                mostNegative = matriz[0][c];
                piv.y = c;
            }
        }
    } else { //Minimize
        double mostPositive = -0.1;
        for (int c = 1; c < cols-1; ++c){
            if (matriz[0][c] > 0 && matriz[0][c] > mostPositive){
                mostPositive = matriz[0][c];
                piv.y = c;
            }
        }
    }

    if (piv.y != -1){
        piv.x = fractions(matriz, cols, rows, piv.y);
    }

    return piv;
}

int pivot(double** matriz, int cols, int rows, int maximize){
    //maximize defines if true maximizes the z function

    Pivot piv = escogerPivote(matriz, cols, rows, maximize);

    printf("Pivote: x:%d, y:%d\n", piv.x, piv.y);

    if (piv.x == -1 && piv.y == -1){
        //Revisar soluciones multiples

        for (int col = 1; col < cols-1; ++col){
            
            if (matriz[0][col] == 0){ //Variable con 0


                double suma = 0;
                for (int row = 1; row < rows; ++row){
                    suma += matriz[row][col];
                }

                printf("Col:%d  Double %.20f\n", col, suma);
                if (suma != 1){
                    return -col; //Soluciones Multiples
                }
            }
        }


    }

    if (piv.y == -1){
        return 1; //Finalizamos
    }
    if (piv.x == -1){
        return 2; //No acotado
    }



    pivotRow(matriz[piv.x], piv, cols);

    for (int r = 0; r < rows; ++r){
        if (r != piv.x && matriz[r][piv.y] != 0){


            double k = matriz[r][piv.y] * -1;

            for (int c = 0; c < cols; ++c){
                matriz[r][c] += k * matriz[piv.x][c];
            }

        }
    }

    return 0; //Pivoteado
}

void printMatriz(double** matriz, int cols, int rows){

    for (int r = 0; r < rows; ++r){
        for (int c = 0; c < cols; ++c){
            printf("%.2f\t", matriz[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}





void solucionesMultiples(double** matriz, int cols, int rows, int maximize, double columnaPivoteNegativa){

    Pivot piv;
    piv.y = -columnaPivoteNegativa;
    piv.x = fractions(matriz, cols, rows, piv.y);

    printf("Pivote Especial: x:%d, y:%d\n", piv.x, piv.y);

    pivotRow(matriz[piv.x], piv, cols);

    for (int r = 0; r < rows; ++r){
        if (r != piv.x && matriz[r][piv.y] != 0){


            double k = matriz[r][piv.y] * -1;

            for (int c = 0; c < cols; ++c){
                matriz[r][c] += k * matriz[piv.x][c];
            }

        }
    }
}

/* ################################## SOLS ################################## */

void extractSolutions(double** solucionOriginal, double** matriz, int amountOfVariables, int cols, int rows, int maximize){

    double* solution1 = malloc(sizeof(double) * amountOfVariables);
    double* solution2 = malloc(sizeof(double) * amountOfVariables);

    for (int col = 1; col <= amountOfVariables; ++col){

        int index = -1;
        double suma = 0;
        for (int row = 1; row < rows; ++row){

            double valor = solucionOriginal[row][col];
            if (valor != 0){
                suma += valor;
                index = row;
            }
        }

        printf("Sol[1] Suma: %.20f \n", suma);
        if (suma != 1){
            solution1[col - 1] = 0;
        } else {
            solution1[col - 1] = solucionOriginal[index][cols-1];
        }
    }

    

    for (int col = 1; col <= amountOfVariables; ++col){

        int index = -1;
        double suma = 0;
        for (int row = 1; row < rows; ++row){


            double valor = matriz[row][col];
            if (valor != 0){
                suma += valor;
                index = row;
            }
        }

        if (suma != 1){
            solution2[col - 1] = 0;
        } else {
            solution2[col - 1] = matriz[index][cols-1];
        }
    }




    //solution1
    for (int x = 0; x < amountOfVariables; x++){
        printf("x%d = %.2f\t", x, solution1[x]);
    }
    printf("\n");

    for (int x = 0; x < amountOfVariables; x++){
        printf("x%d = %.2f\t", x, solution2[x]);
    }
    printf("\n");


    double alpha = 0.25;
    for (int sol = 0; sol < 3; sol++){
        for (int x = 0; x < amountOfVariables; x++){
            double value = solution1[x]*alpha + solution2[x]*(1-alpha);
            printf("x%d = %.2f\t", x, value);
        }
        printf("\n");
        alpha += 0.25;
    }

}








/* ################################## MAIN ################################## */

void runSimplex(double** matriz, int amountOfVariables, int cols, int rows, int maximize){



    //Title

    //Introduction

    //Problema (double** matriz, int amountOfVariables, int cols, int rows, int maximize)

    //GuardarMatriz

    printMatriz(matriz, cols, rows);
    fprintf(f, "\\section{Result Analysis}");
    int status = 0;
    while (status == 0){
        status = pivot(matriz, cols, rows, maximize);
        
        printf("---------------------------\n");

        // if (Guardar == activado)
        // GuardarMatriz

        printMatriz(matriz, cols, rows);

        sleep(1);
    }

    //GuardarMatriz Final

    if (status == 2){
        //Reporte no acotado
        // fprintf(f, "\\subsection{Unbounded problems}\n");
        // fprintf(f, "Sometimes the simplex algorithm may be faced with an unbounded problem, as a result of poor constraint management at the time of modeling.\\\\\n");
        // fprintf(f, "In this case it is found in:");
        //poner tabla?
    }
    if (status < 0){
        //Soluciones multiples
        // Con (-col) de codigo de status
        // fprintf(f, "\\subsection{Multiple solutions}\n");
        // fprintf(f, "It happens when an infinite number of solutions can be found to the same problem, through a particular formula.\\\\\n");
        // fprintf(f, "This phenomenon is not typical of all the problems that the simplex algorithm encounters, it is only when a non-basic variable has a value of 0. This means that it can be manipulated to find more solutions, without affecting the gain.\\\\\n");
        // fprintf(f, "Here is where it happens in this problem:\\\\\n");
        //poner tabla?

        double** solucionOriginal = malloc(sizeof(double*) * rows);
        for (int r = 0; r < rows; ++r){
            solucionOriginal[r] = malloc(sizeof(double) * cols);
            for (int c = 0; c < cols; ++c){
                solucionOriginal[r][c] = matriz[r][c];
            }
        }

        solucionesMultiples(matriz, cols, rows, maximize, status);

        //GuardarMatriz Solucion 2
        printMatriz(matriz, cols, rows);

        extractSolutions(solucionOriginal, matriz, amountOfVariables, cols, rows, maximize); //Solucion es multiple...
    } else {

        //Solucion

    }

    fprintf(f, "\\section{Graph}\n");
    //Dibujo 2D

    //Dibujo 3D

    //CloseDocument

}



void test1(){

    //Test

    /*
    double matriz[3][6] = {
        {1, -3, -4, 0, 0, 0},
        {0, 1, 1, 1, 0, 40},
        {0, 1, 2, 0, 1, 60}
    };*/

    int rows = 3;
    int cols = 6;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    matriz[0][0] = 1;
    matriz[0][1] = -3;
    matriz[0][2] = -4;
    matriz[0][3] = 0;
    matriz[0][4] = 0;
    matriz[0][5] = 0;

    matriz[1][0] = 0;
    matriz[1][1] = 1;
    matriz[1][2] = 1;
    matriz[1][3] = 1;
    matriz[1][4] = 0;
    matriz[1][5] = 40;

    matriz[2][0] = 0;
    matriz[2][1] = 1;
    matriz[2][2] = 2;
    matriz[2][3] = 0;
    matriz[2][4] = 1;
    matriz[2][5] = 60;


    runSimplex(matriz, 0, cols, rows, 1);
}

void test2(){

    int rows = 3;
    int cols = 6;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    matriz[0][0] = 1;
    matriz[0][1] = -2;
    matriz[0][2] = 3;
    matriz[0][3] = 0;
    matriz[0][4] = 0;
    matriz[0][5] = 0;

    matriz[1][0] = 0;
    matriz[1][1] = 1;
    matriz[1][2] = 1;
    matriz[1][3] = 1;
    matriz[1][4] = 0;
    matriz[1][5] = 4;

    matriz[2][0] = 0;
    matriz[2][1] = 1;
    matriz[2][2] = -1;
    matriz[2][3] = 0;
    matriz[2][4] = 1;
    matriz[2][5] = 6;


    runSimplex(matriz, 0, cols, rows, 0);
}

void test3(){

    int rows = 4;
    int cols = 10;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    matriz[0][0] = 1;
    matriz[0][1] = 5;
    matriz[0][2] = 10;
    matriz[0][3] = -1;
    matriz[0][4] = -5;
    matriz[0][5] = -20;
    matriz[0][6] = 0;
    matriz[0][7] = 0;
    matriz[0][8] = 0;
    matriz[0][9] = 0;

    matriz[1][0] = 0;
    matriz[1][1] = 2;
    matriz[1][2] = 1;
    matriz[1][3] = 0;
    matriz[1][4] = 0;
    matriz[1][5] = 0;
    matriz[1][6] = 1;
    matriz[1][7] = 0;
    matriz[1][8] = 0;
    matriz[1][9] = 10;

    matriz[2][0] = 0;
    matriz[2][1] = 5;
    matriz[2][2] = 0;
    matriz[2][3] = -1;
    matriz[2][4] = 0;
    matriz[2][5] = -3;
    matriz[2][6] = 0;
    matriz[2][7] = 1;
    matriz[2][8] = 0;
    matriz[2][9] = 10;

    matriz[3][0] = 0;
    matriz[3][1] = 0;
    matriz[3][2] = 2;
    matriz[3][3] = 0;
    matriz[3][4] = -1;
    matriz[3][5] = 0;
    matriz[3][6] = 0;
    matriz[3][7] = 0;
    matriz[3][8] = 1;
    matriz[3][9] = 2;


    runSimplex(matriz, 0, cols, rows, 0);
}

void test4(){

    int rows = 3;
    int cols = 8;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    matriz[0][0] = 1;
    matriz[0][1] = -36;
    matriz[0][2] = -30;
    matriz[0][3] = 3;
    matriz[0][4] = 4;
    matriz[0][5] = 0;
    matriz[0][6] = 0;
    matriz[0][7] = 0;

    matriz[1][0] = 0;
    matriz[1][1] = 1;
    matriz[1][2] = 1;
    matriz[1][3] = -1;
    matriz[1][4] = 0;
    matriz[1][5] = 1;
    matriz[1][6] = 0;
    matriz[1][7] = 5;

    matriz[2][0] = 0;
    matriz[2][1] = 6;
    matriz[2][2] = 5;
    matriz[2][3] = 0;
    matriz[2][4] = -1;
    matriz[2][5] = 0;
    matriz[2][6] = 1;
    matriz[2][7] = 10;


    runSimplex(matriz, 0, cols, rows, 1); //max
}

void test5(){

    int rows = 4;
    int cols = 7;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    matriz[0][0] = 1;
    matriz[0][1] = -2;
    matriz[0][2] = -1;
    matriz[0][3] = 0;
    matriz[0][4] = 0;
    matriz[0][5] = 0;
    matriz[0][6] = 0;

    matriz[1][0] = 0;
    matriz[1][1] = 3;
    matriz[1][2] = 1;
    matriz[1][3] = 1;
    matriz[1][4] = 0;
    matriz[1][5] = 0;
    matriz[1][6] = 6;

    matriz[2][0] = 0;
    matriz[2][1] = 1;
    matriz[2][2] = -1;
    matriz[2][3] = 0;
    matriz[2][4] = 1;
    matriz[2][5] = 0;
    matriz[2][6] = 2;

    matriz[3][0] = 0;
    matriz[3][1] = 0;
    matriz[3][2] = 1;
    matriz[3][3] = 0;
    matriz[3][4] = 0;
    matriz[3][5] = 1;
    matriz[3][6] = 3;


    runSimplex(matriz, 0, cols, rows, 1); //max
}

void test6(){ //Soluciones multiples

    int rows = 5;
    int cols = 9;
    int variables = 3;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    matriz[0][0] = 1;
    matriz[0][1] = -60;
    matriz[0][2] = -35;
    matriz[0][3] = -20;
    matriz[0][4] = 0;
    matriz[0][5] = 0;
    matriz[0][6] = 0;
    matriz[0][7] = 0;
    matriz[0][8] = 0;

    matriz[1][0] = 0;
    matriz[1][1] = 8;
    matriz[1][2] = 6;
    matriz[1][3] = 1;
    matriz[1][4] = 1;
    matriz[1][5] = 0;
    matriz[1][6] = 0;
    matriz[1][7] = 0;
    matriz[1][8] = 48;

    matriz[2][0] = 0;
    matriz[2][1] = 4;
    matriz[2][2] = 2;
    matriz[2][3] = 1.5;
    matriz[2][4] = 0;
    matriz[2][5] = 1;
    matriz[2][6] = 0;
    matriz[2][7] = 0;
    matriz[2][8] = 20;

    matriz[3][0] = 0;
    matriz[3][1] = 2;
    matriz[3][2] = 1.5;
    matriz[3][3] = 0.5;
    matriz[3][4] = 0;
    matriz[3][5] = 0;
    matriz[3][6] = 1;
    matriz[3][7] = 0;
    matriz[3][8] = 8;

    matriz[4][0] = 0;
    matriz[4][1] = 0;
    matriz[4][2] = 1;
    matriz[4][3] = 0;
    matriz[4][4] = 0;
    matriz[4][5] = 0;
    matriz[4][6] = 0;
    matriz[4][7] = 1;
    matriz[4][8] = 5;


    runSimplex(matriz, variables, cols, rows, 1); //max
}

void test7(){ //Soluciones multiples

    int rows = 3;
    int cols = 6;
    int variables = 2;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    matriz[0][0] = 1;
    matriz[0][1] = -4;
    matriz[0][2] = -14;
    matriz[0][3] = 0;
    matriz[0][4] = 0;
    matriz[0][5] = 0;

    matriz[1][0] = 0;
    matriz[1][1] = 2;
    matriz[1][2] = 7;
    matriz[1][3] = 1;
    matriz[1][4] = 0;
    matriz[1][5] = 21;

    matriz[2][0] = 0;
    matriz[2][1] = 7;
    matriz[2][2] = 2;
    matriz[2][3] = 0;
    matriz[2][4] = 1;
    matriz[2][5] = 21;

    runSimplex(matriz, variables, cols, rows, 1); //max
}

int main(){

    test7();

    return 0;
}