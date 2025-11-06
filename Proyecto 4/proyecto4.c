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








typedef struct {
    int x;
    int y;
    int z;
} Dot;

typedef struct {
    int x_pivot, y_pivot;
    double* fractions;
    double pivot;
} TableData;




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
        "    {\\large Simplex Algoritm\\par}\n"

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
    fprintf(f, "\\usepackage{xcolor}\n");
    fprintf(f, "\\usepackage{pgfplots}\n");
    fprintf(f, "\\usepgfplotslibrary{fillbetween}\n");
    fprintf(f, "\\usepackage{amsmath}\n\n");

    //Document information
    fprintf(f, "\\begin{document}\n");

    makeTitle(f);

    //begin
    fprintf(f, "\\definecolor{KirbyPink}{HTML}{D74894}\n");
    fprintf(f, "\\definecolor{LightPink}{HTML}{FFBFBF}\n\n");
    fprintf(f, "\\newpage\n\n\n");
}

void documentEnd(FILE* f){
    fprintf(f, "\n\n");
    fprintf(f, "\\end{document}\n");
    fclose(f);
}

void introduction(FILE* f){
    fprintf(f, "\\section{The Simplex Algorithm}\n");

    fprintf(f, "The simplex algorithm, developed by George Dantzig in 1947, arises from the need to solve linear programming problems.m\n");
    fprintf(f, "This problem was fundamentally proposed by Kantorovich and Koopman, who developed the optimal location problem and the problem of resources.\n");

    fprintf(f, "The Simplex method optimizes an objective function subject to linear constraints, using an iterative process to improve the value of the objective function until the optimal solution is reached. Its ability to solve complex problems and its use in various applications make it an essential tool in the optimization of resources and strategic decisions in industry, economics, and operations research.\\\\\n");
    fprintf(f, "Given the time of its development, it was essentially thought to be solved by hand; however, now there are digital tools that allow the process to be automated. \n");

    fprintf(f, "\\subsection{George Dantzig}\n");
    fprintf(f, "The American mathematician was born in 1914 and died in 2005. In addition to being the creator of the Simplex algorithm, he was head of the Scientific Computing of Operations Research (SCOOP), where he promoted linear programming for strategic purposes during World War II.\n");

       
    fprintf(f, "\\begin{center}\n");
    fprintf(f, "\\includegraphics[width=0.25\\textwidth]{R.jpg}\n");
    fprintf(f, "\\end{center}\n");

}

void problem(FILE* f, double** matriz, char* problemName, char** variableNames, int amountOfVariables, int saveMatrixes, int* restrictions, // [0:<, 1:=, 2:>]
    int cols, int rows, int maximize){

    fprintf(f, "\\section{Problem: %s}\n", problemName);


    char* verb = maximize ? "maximizing\0" : "minimizing\0" ;

    fprintf(f, "The problem inputted by the user is called \\textquotedblleft %s\\textquotedblright and consists of %s the following fuction:\n\n", problemName, verb);

    

    fprintf(f, "$");
    fprintf(f, "Z = %s \\cdot %2f", variableNames[0], matriz[0][1]);
    for (int var = 1; var < amountOfVariables; ++var){
        fprintf(f, " + %s \\cdot %2f", variableNames[var], matriz[0][var+1]);
    }
    fprintf(f, "$\n\n\n");

    fprintf(f, "Subject to:\n\n\n");



    //row0 does not have restriction (Z)
    for (int restriction = 1; restriction < rows; ++restriction){
        fprintf(f, "$");
        int first = 1;

        for (int var = 1; var < amountOfVariables + 1; ++var){
            if (var != 0){
                if (first){
                    first = 0;
                } else {
                    fprintf(f, " + ");
                }
                
                fprintf(f, "%s \\cdot %2f", variableNames[var-1], matriz[restriction][var]);
            }
        }
        
        if (1){ // leq
            fprintf(f, " \\leq ");
        }
        fprintf(f, "%2f", matriz[restriction][cols-1]);

        fprintf(f, "$\n\n");
    }



}

void storeMatriz(FILE* f, double** matriz, int amountOfVariables, int* restrictions, int cols, int rows){

    fprintf(f, "\\begin{center}\n");

    fprintf(f, "    \\begin{tabular}{|c|");
    //Tex table structure
    for (int col = 0; col < cols; ++col){
        fprintf(f, "c|");
    }
    fprintf(f, "}\n");



    fprintf(f, "        \\hline\n");
    //Table headers
    fprintf(f, "        Z ");
    for (int x = 0; x < amountOfVariables; ++x){
        fprintf(f, " & $x_{%d}$", x+1);
    }
    for (int s = 0; s < restrictions[0]; ++s){
        fprintf(f, " & $s_{%d}$", s+1);
    }
    for (int e = 0; e < restrictions[2]; ++e){
        fprintf(f, " & $e_{%d}$", e+1);
    }
    for (int a = 0; a < restrictions[1] + restrictions[2]; ++a){
        fprintf(f, " & $a_{%d}$", a+1);
    }
    fprintf(f, " & b");
    fprintf(f, "\\\\\n        \\hline\n");




    fprintf(f, "        \\hline\n");
    //Matrix cells
    for (int row = 0; row < rows; ++row){
        fprintf(f, "        %.3f", matriz[row][0]);
        for (int col = 1; col < cols; col++){
            fprintf(f, "& %.3f", matriz[row][col]);
        }
        fprintf(f, "\\\\\n        \\hline\n");
    }
    fprintf(f, "    \\end{tabular}\n");
    fprintf(f, "\\end{center}\n\n\n");
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

int fractions(double** matriz, int cols, int rows, int y, TableData tableData){

    double min = 1125899906842624;
    int decisiones = 0;
    int x = -1;
    for (int r = 1; r < rows; ++r){
        double frac = matriz[r][cols-1] / matriz[r][y];

        if (frac == INVALID){
            frac = INVALID;
        }

        tableData->fractions[r-1] = frac;

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

Pivot escogerPivote(double** matriz, int cols, int rows, int maximize, TableData tableData){

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
        piv.x = fractions(matriz, cols, rows, piv.y, tableData);
    }

    return piv;
}

int pivot(double** matriz, int cols, int rows, int maximize, TableData* tableData){
    //maximize defines if true maximizes the z function

    Pivot piv = escogerPivote(matriz, cols, rows, maximize, tableData);

    printf("Pivote: x:%d, y:%d\n", piv.x, piv.y);
    tableData->x = piv.x;
    tableData->y = piv.y;
    tableData->pivot = matriz[piv.x][piv.y];

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

void solucionesMultiples(FILE* f, double** matriz, int cols, int rows, int maximize, double columnaPivoteNegativa){

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

void extractSolutions(FILE* f, double** solucionOriginal, double** matriz, int amountOfVariables, int cols, int rows, int maximize){

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


    // double alpha = 0.25;
    // for (int sol = 0; sol < 3; sol++){
    //     for (int x = 0; x < amountOfVariables; x++){
    //         double value = solution1[x]*alpha + solution2[x]*(1-alpha);
    //         printf("x%d = %.2f\t", x, value);
    //     }
    //     printf("\n");
    //     alpha += 0.25;
    // }

    fprintf(f, "\\textbf{Ecuation} \n");
    fprintf(f, "$$\n");
    fprintf(f, "x = \\alpha \\cdot \n");
    fprintf(f, "\\begin{bmatrix}\n");
    for (int i = 1; i <= amountOfVariables; ++i){
        fprintf(f, "%.2f \\\\ ", solution1[i]);
    }
    fprintf(f, "\\end{bmatrix}\n");
    fprintf(f, "+ (1 - \\alpha) \\cdot \n");
    fprintf(f, "\\begin{bmatrix}\n");
    for (int i = 1; i <= amountOfVariables; ++i){
        fprintf(f, "%.2f \\\\ ", solution2[i]);
    }
    fprintf(f, " \\end{bmatrix}\n");
    fprintf(f, "$$\n");


    fprintf(f, "\\textbf{Other solutions} \n");
    double alpha = 0.25;
    for (int sol = 0; sol < 3; sol++){
        fprintf(f, "$$\\alpha = %.2f \\Rightarrow x = \n", alpha);
        fprintf(f, "\\begin{bmatrix}\n");

        for (int x = 0; x < amountOfVariables; x++){
            double value = solution1[x] * alpha + solution2[x] * (1 - alpha);
            fprintf(f, "%.2f \\\\ ", value);
        }

        fprintf(f, "\n\\end{bmatrix}\n");
        fprintf(f, "$$\n\n");
        alpha += 0.25;
    }

}

void drawing2D(FILE* f, double** matriz, int x1, int b1, int x2, int b2){
    fprintf(f, "\\begin{center} \n");
    fprintf(f, "\\begin{tikzpicture} \n");
    fprintf(f, "\\begin{axis}[ \n");
    fprintf(f, "    axis lines = middle,");
    fprintf(f, "    xlabel=$x$, ylabel=$y$,");
    fprintf(f, "    xmin=0, xmax=50,");         
    fprintf(f, "    ymin=0, ymax=50, ");        
    fprintf(f, "    samples=200,  ");           
    fprintf(f, "    domain=-5:5,");
    fprintf(f, "    legend pos=outer north east,");
    fprintf(f, "]");


    fprintf(f, "\\addplot[name path=ineq1, thick, blue] {%d*x + %d}; ", x1, b1);
    fprintf(f, "\\addplot[name path=lower, draw=none, domain=-50:50] {-5};");

    fprintf(f, "\\addplot[fill=blue!30, opacity=0.5]");
    fprintf(f, "fill between[of=ineq1 and lower, soft clip={domain=-5:5}];");
    fprintf(f, "\\addlegendentry{$y \\leq %dx + %d$}", x1, b1);

    fprintf(f, "\\addplot[name path=ineq2, thick, red] {%d*x + %d};", x2, b2);
    fprintf(f, "\\addplot[name path=upper, draw=none, domain=-5:5] {5};");

    fprintf(f, "\\addplot[fill=red!30, opacity=0.5]");
    fprintf(f, "fill between[of=ineq2 and upper, soft clip={domain=-5:5}];");
    fprintf(f, "\\addlegendentry{$y \\geq %dx + %d$}", x2, b2);

    fprintf(f, "\\end{axis}");
    fprintf(f, "\\end{tikzpicture}");
    fprintf(f, "\\end{center}");

}

void drawing3D(FILE* f){

    fprintf(f, "\\section{3D Drawing of the Problem}");

    fprintf(f, "\\begin{center}");
    fprintf(f, "\\begin{adjustbox}{max width=\\textwidth}\n");
    fprintf(f, "\\begin{tikzpicture}\n");


    fprintf(f, "\\begin{axis}[view={45}{45}, xlabel=$x$, ylabel=$y$, zlabel=$z$]\n");

    fprintf(f, "\\addplot3[only marks, scatter, mark=*]\n");


        
    Dot* dots = malloc(sizeof(Dot)*10);

    fprintf(f, "coordinates{\n");

    for (;;){

    }

    fprintf(f, "}\n");

    free(dots);


    fprintf(f, "\\end{tikzpicture}\n");
    fprintf(f, "\\end{adjustbox}\n\n\n");
    fprintf(f, "\\end{center}\n\n\n");
}



/* ################################## MAIN ################################## */


void compileTex(){
    int responseCode = system("pdflatex simplex.tex");
    if (responseCode == 0){
        printf("\n\nLatex compiled without problems\n");
        system("evince --presentation simplex.pdf &");
    }
}

void runSimplex(double** matriz, char* problemName, char** variableNames, int amountOfVariables, int saveMatrixes, int* restrictions, // [0:<, 1:=, 2:>]
    int cols, int rows, int maximize){

    FILE* f = fopen("simplex.tex", "w");

    if (!f){
        printf("Could not write file\n");
    }

    documentStart(f);

    introduction(f);

    problem(f, matriz, problemName, variableNames, amountOfVariables, saveMatrixes, restrictions, // [0:<, 1:=, 2:>]
    cols, rows, maximize);

    fprintf(f, "\\section{Initial Matrix}");
    storeMatriz(f, matriz, amountOfVariables, restrictions, cols, rows);

    fprintf(f, "\\section{Result Analysis}");
    int status = 0;
    TableData* tableData = malloc(sizeof(TableData));
    tableData.fractions = malloc(sizeof(double)*(rows-1));
    while (status == 0){
        status = pivot(matriz, cols, rows, maximize, tableData);
        
        printf("---------------------------\n");

        if (saveMatrixes){
            storeIntermediateMatriz(f, matriz, amountOfVariables, restrictions, cols, rows, tableData);
        }

        sleep(1);
    }

    //GuardarMatriz Final

    if (status == 2){

        fprintf(f, "\\section{Result Analysis}");

        //Reporte no acotado
        fprintf(f, "\\subsection{Unbounded problems}\n");
        fprintf(f, "Sometimes the simplex algorithm may be faced with an unbounded problem, as a result of poor constraint management at the time of modeling.\\\\\n");
        fprintf(f, "In this case it is found in:");
        //poner tabla?
        storeMatriz(f, matriz, amountOfVariables, restrictions, cols, rows);
    }
    if (status < 0){

        fprintf(f, "\\section{Multiple Solutions}");

        //Soluciones multiples
        // Con (-col) de codigo de status
        
        fprintf(f, "\\subsection{Explanation}\n");
        fprintf(f, "It happens when an infinite number of solutions can be found to the same problem, through a particular formula.\\\\\n");
        fprintf(f, "This phenomenon is not typical of all the problems that the simplex algorithm encounters, it is only when a non-basic variable has a value of 0. This means that it can be manipulated to find more solutions, without affecting the gain.\\\\\n");
        fprintf(f, "Here is where it happens in this problem: \n");


        double** solucionOriginal = malloc(sizeof(double*) * rows);
        for (int r = 0; r < rows; ++r){
            solucionOriginal[r] = malloc(sizeof(double) * cols);
            for (int c = 0; c < cols; ++c){
                solucionOriginal[r][c] = matriz[r][c];
            }
        }

        //Calculate multiple solutions
        solucionesMultiples(f, matriz, cols, rows, maximize, status);

        //GuardarMatriz Solucion Original
        fprintf(f, "\\subsection{First solution table}\n");
        storeMatriz(f, solucionOriginal, amountOfVariables, restrictions, cols, rows);

        //GuardarMatriz Solucion 2
        fprintf(f, "\\subsection{Second solution table}\n");
        storeMatriz(f, matriz, amountOfVariables, restrictions, cols, rows);

        //Multiple solutions with alpha
        fprintf(f, "\\subsection{Equation and Solutions}\n");
        extractSolutions(f, solucionOriginal, matriz, amountOfVariables, cols, rows, maximize);

    } else {

        //Solucion

        fprintf(f, "\\section{Unique Solution}");

        fprintf(f, "\\subsection{Multiple solutions}\n");
        fprintf(f, "It happens when an infinite number of solutions can be found to the same problem, through a particular formula.\\\\\n");
        fprintf(f, "This phenomenon is not typical of all the problems that the simplex algorithm encounters, it is only when a non-basic variable has a value of 0. This means that it can be manipulated to find more solutions, without affecting the gain.\\\\\n");
        fprintf(f, "Here is where it happens in this problem: \n");

    }

    fprintf(f, "\\section{Graph}\n");

    //Dibujo 2D

    //Dibujo 3D

    documentEnd(f);
    compileTex();
}


/*
void test1(){

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
*/

void test7(){ //Soluciones multiples

    int rows = 3;
    int cols = 6;
    int variables = 2;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    char* varNames[2];
    varNames[0] = "AA";
    varNames[1] = "BB";

    int* restrictions = malloc(sizeof(int) * 3);
    restrictions[0] = 2;
    restrictions[1] = 0;
    restrictions[2] = 0;

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

    runSimplex(matriz, "Hi I am testy the test", varNames, variables, 1, restrictions, cols, rows, 1); //max
}

int main(){

    test7();

    return 0;
}