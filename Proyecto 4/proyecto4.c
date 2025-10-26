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

int fractions(double** matriz, int cols, int rows, int y, Piv* piv){

    double min = 1125899906842624;
    int decisiones = 0;
    int x = -1;
    for (int r = 1; r < rows; ++r){
        double frac = matriz[r][cols-1] / matriz[r][y];

        if (matriz[r][y] > 0 && frac >= 0 && frac < min){
            // frac < min escoge en primero en caso de empate

            min = frac;
            x = r;
            decisiones = 0;
        } else (matriz[r][y] > 0 && frac >= 0 && frac = min){
            decisiones++;
        }
    }

    piv->decisiones = decisiones;
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


    piv.x = fractions(matriz, cols, rows, piv.y, &piv);


    return piv;
}

int pivot(double** matriz, int cols, int rows, int maximize){
    //maximize defines if true maximizes the z function

    Pivot piv = escogerPivote(matriz, cols, rows, maximize);

    printf("Pivote: x:%d, y:%d\n", piv.x, piv.y);

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





/* ################################## MAIN ################################## */

void runSimplex(double** matriz, int cols, int rows, int maximize){

    printMatriz(matriz, cols, rows);

    int yes = 1;
    int status;
    while (yes){
        status = pivot(matriz, cols, rows, maximize);
        if (status){
            yes = 0; //yes = no
        }
        //yes = 0;
        printf("---------------------------\n");
        printMatriz(matriz, cols, rows);

        sleep(1);
    }

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


    runSimplex(matriz, cols, rows, 1);
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


    runSimplex(matriz, cols, rows, 0);
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


    runSimplex(matriz, cols, rows, 0);
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


    runSimplex(matriz, cols, rows, 1); //max
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


    runSimplex(matriz, cols, rows, 1); //max
}

int main(){

    test5();

    return 0;
}