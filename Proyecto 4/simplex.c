/*
                Project 4: Simplex
                Made by: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>

//#include <gtk/gtk.h>
//#include <cairo.h>
#include <ctype.h>

#define INVALID_FRACTION 1125899906842624



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

typedef struct {
    int x;
    int y;
    double pivot;
    double pivotM;
} Pivot;

double eps = 1e-2;


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
        "    {\\large November 12 2025\\par}\n"
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
    fprintf(f, "\\usepackage{adjustbox}\n");
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

void problem(FILE* f, double** matriz, double* M, char* problemName, char** variableNames, int amountOfVariables, int saveMatrixes, int* restrictions, // [0:<, 1:=, 2:>]
    int cols, int rows, int maximize){

    fprintf(f, "\\section{Problem: %s}\n", problemName);


    char* verb = maximize ? "maximizing\0" : "minimizing\0" ;

    fprintf(f, "The problem inputted by the user is called \\textquotedblleft %s\\textquotedblright and consists of %s the following fuction:\n\n", problemName, verb);

    fprintf(f, "$");
    fprintf(f, "Z = %s \\cdot %2f", variableNames[0], matriz[0][1]*-1);
    for (int var = 1; var < amountOfVariables; ++var){
        fprintf(f, " + %s \\cdot %2f", variableNames[var], matriz[0][var+1]*-1);
    }
    fprintf(f, "$\n\n\n");

    fprintf(f, "Subject to:\n\n\n");



    //row0 does not have restriction (Z)
    for (int restriction = 1; restriction < rows; ++restriction){
        fflush(f);
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
        
        if (restrictions[restriction-1] == 0){ // leq <=
            fprintf(f, " \\leq ");
        } else if (restrictions[restriction-1] == 2){ // geq >=
            fprintf(f, " \\geq ");
        } else if (restrictions[restriction-1] == 1){ // eq ==
            fprintf(f, " = ");
        }
        fprintf(f, "%2f", matriz[restriction][cols-1]);

        fprintf(f, "$\n\n");
    }



}

void storeMatriz(FILE* f, double** matriz, double* M, char** varNames, int amountOfVariables, int* restrictions, int cols, int rows){


    fprintf(f, "\\begin{adjustbox}{max width=\\textwidth}\n");

    fprintf(f, "    \\begin{tabular}{|c|");
    //Tex table structure
    for (int col = 0; col < cols; ++col){
        fprintf(f, "c|");
    }
    fprintf(f, "}\n");



    fprintf(f, "        \\hline\n");
    //Table headers
    fprintf(f, "        Z ");

    for (int x = 0; x < cols - 2; ++x){
        fprintf(f, " & $%s$", varNames[x]);
    }

    fprintf(f, " & b");
    fprintf(f, "\\\\\n        \\hline\n");




    fprintf(f, "        \\hline\n");
    //Matrix cells
    fprintf(f, "        %.3f", matriz[0][0]);
    for (int col = 1; col < cols; col++){

        if (fabs(M[col]) > eps){
            fprintf(f, "& %.3f + %.1fM ", matriz[0][col], M[col]);
        } else {
            fprintf(f, "& %.3f", matriz[0][col]);
        }
    }
    fprintf(f, "\\\\\n        \\hline\n");


    for (int row = 1; row < rows; ++row){
        fprintf(f, "        %.3f", matriz[row][0]);
        for (int col = 1; col < cols; col++){
            fprintf(f, "& %.3f", matriz[row][col]);
        }
        fprintf(f, "\\\\\n        \\hline\n");
    }
    fprintf(f, "    \\end{tabular}\n");
    fprintf(f, "\\end{adjustbox}\n\n\n");
}

void storeIntermediateMatriz(FILE* f, double** matriz, double* M, char** varNames, int amountOfVariables, int* restrictions, int cols, int rows, TableData* td){

    fprintf(f, "\\begin{adjustbox}{max width=\\textwidth}\n");;

    fprintf(f, "    \\begin{tabular}{|c|");
    //Tex table structure
    for (int col = 0; col < cols; ++col){
        fprintf(f, "c|");
    }
    fprintf(f, "c|"); //Fraction column
    fprintf(f, "}\n");



    fprintf(f, "        \\hline\n");
    //Table headers
    fprintf(f, "        Z ");
    
    for (int x = 0; x < cols - 2; ++x){
        fprintf(f, " & $%s$", varNames[x]);
    }

    fprintf(f, " & b");
    fprintf(f, " & Fractions");
    fprintf(f, "\\\\\n        \\hline\n");

    


    double* fractions = td->fractions;
    int x = td->x_pivot;
    int y = td->y_pivot;

    fprintf(f, "        \\hline\n");
    fprintf(f, "        $%.3f$", matriz[0][0]);
    for (int col = 1; col < cols; col++){

        if (fabs(M[col]) > eps){
            if (y == col){
                fprintf(f, "& \\cellcolor{LightPink}$%.3f + %.1fM$ ", matriz[0][col], M[col]);
            } else {
                fprintf(f, "& %.3f + %.1fM ", matriz[0][col], M[col]);
            }
        } else {
            fprintf(f, "& %.3f", matriz[0][col]);
        }
    }

    fprintf(f, "\\\\\n        \\hline\n");

    //Matrix cells
    for (int row = 1; row < rows; ++row){
        fprintf(f, "        %.3f", matriz[row][0]);



        for (int col = 1; col < cols+1; col++){ //+1 to store fractions

            if (y == col){
                double value = matriz[row][col];
                if (x == row){ //Pivote
                    fprintf(f, "& \\cellcolor{KirbyPink}$%.3f$ ", value);
                } else { //Columna seleccionada
                    fprintf(f, "& \\cellcolor{LightPink}$%.3f$ ", value);
                }

            } else if (col == cols && row != 0){ //Fraction
                double fraction = fractions[row-1];
                if (fraction == INVALID_FRACTION || isinf(fraction) || fraction < 0){
                    fprintf(f, "& $Invalid$ ");
                } else if (x == row){
                    fprintf(f, "& \\cellcolor{KirbyPink}$%.3f$ ", fraction);
                } else {
                    fprintf(f, "& $%.3f$ ", fraction);
                }
            
            } else { //Celda normal
                double value = matriz[row][col];
                fprintf(f, "& %.3f", value);
            }
            
        }
        fprintf(f, "\\\\\n        \\hline\n");
    }
    fprintf(f, "    \\end{tabular}\n");
    fprintf(f, "\\end{adjustbox}\n\n\n");

}



/* ################################ SIMPLEX ################################# */


void pivotRow(double* fila, Pivot piv, int cols){
    double k = 1/fila[piv.y];

    for (int e = 0; e < cols; ++e){
        fila[e] = fila[e] * k;
    }
}

int fractions(double** matriz, double* M, int cols, int rows, int y, Pivot* piv, TableData* tableData){

    double min = INVALID_FRACTION; //Big number for overwrite
    int x = -1;
    for (int r = 1; r < rows; ++r){
        double frac = matriz[r][cols-1] / matriz[r][y];

        tableData->fractions[r-1] = frac;

        if (matriz[r][y] > 0 && fabs(frac) > -eps && frac < min){ //b positivo o 0
            // frac < min escoge en primero en caso de empate
            min = frac;
            x = r;
            piv->pivot = matriz[r][y];
            piv->pivotM = M[y];

        } else if (matriz[r][y] > 0 && fabs(frac) > -eps && min == INVALID_FRACTION){ //0 degenerado (???) INVALID_FRACTION
            // frac < min escoge en primero en caso de empate
            min = frac;
            x = r;
            piv->pivot = matriz[r][y];
            piv->pivotM = M[y];

        }
    }

    //piv->decisiones = decisiones;
    return x;
}

Pivot escogerPivote(double** matriz, double* M, int cols, int rows, int maximize, TableData* tableData, int artificials){

    Pivot piv;
    piv.x = -1;
    piv.y = -1;

    int* draws = malloc(sizeof(int) * cols);
    int draw = 1;
    if (maximize){ //Maximize
        double mostNegative = 0.1;

        for (int c = 1; c < cols-1-artificials; ++c){ //Pivot according to M
            if (M[c] < 0 && M[c] < mostNegative){
                mostNegative = M[c];
                draw = 1;
                draws[draw-1] = c;
                piv.y = c;

            } else if (M[c] < 0 && fabs(M[c] - mostNegative) < eps) { //M draw
                draws[draw] = c;
                draw++;
                piv.y = c;
            }
        }
        if (mostNegative == 0.1){ //No -M found
            for (int c = 1; c < cols-1-artificials; ++c){
                if (matriz[0][c] < 0 && fabs(M[c]) < eps && matriz[0][c] < mostNegative){
                    mostNegative = matriz[0][c];
                    piv.y = c;
                }
            }
        } else if (draw > 1){ //Draw of M(s)
            for (int d = 0; d < draw; ++d){
                int c = draws[d];
                if (matriz[0][c] < 0 && matriz[0][c] < mostNegative){
                    mostNegative = matriz[0][c];
                    piv.y = c;
                }
            }
        }



    } else { //Minimize
        double mostPositive = -0.1;

        for (int c = 1; c < cols-1-artificials; ++c){ //Pivot according to M
            if (M[c] > 0 && M[c] > mostPositive){
                mostPositive = M[c];
                draw = 1;
                draws[draw-1] = c;
                piv.y = c;
            } else if (M[c] > 0 && fabs(M[c] - mostPositive) < eps) { //M draw
                draws[draw] = c;
                draw++;
                piv.y = c;
            }
        }
        if (mostPositive == -0.1){ //No -M found
            for (int c = 1; c < cols-1-artificials; ++c){
                if (matriz[0][c] > 0 && fabs(M[c]) < eps && matriz[0][c] > mostPositive){
                    mostPositive = matriz[0][c];
                    piv.y = c;
                }
            }
        } else if (draw > 1){ //Draw of M(s)
            for (int d = 0; d < draw; ++d){
                int c = draws[d];
                if (matriz[0][c] > 0 && matriz[0][c] > mostPositive){
                    mostPositive = matriz[0][c];
                    piv.y = c;
                }
            }
        }
    }

    free(draws);
    if (piv.y != -1){
        piv.x = fractions(matriz, M, cols, rows, piv.y, &piv, tableData);
    }

    return piv;
}

int pivot(double** matriz, double* M, int cols, int rows, int maximize, TableData* tableData, int artificials){
    //maximize defines if true maximizes the z function

    Pivot piv = escogerPivote(matriz, M, cols, rows, maximize, tableData, artificials);

    tableData->x_pivot = piv.x;
    tableData->y_pivot = piv.y;
    tableData->pivot = INVALID_FRACTION;

    if (piv.x == -1 && piv.y == -1){
        //Revisar soluciones multiples

        for (int col = 1; col < cols-1; ++col){


            
            if (fabs(matriz[0][col]) < eps){ //Variable con 0

                int ones = 0; //La cantidad de 1s
                int nonZero = 0; //Flag de valores no 0 y no 1
                for (int row = 1; row < rows; ++row){
                    double valor = matriz[row][col];

                    if (fabs(valor - 1.0) < eps){ // ==1
                        ones++; //1s found

                    } else if (!(fabs(valor) < eps)){ // != 0
                        nonZero = 1; //Non zero (non one) value found
                        break;
                    }
                }


                if (!(ones == 1 && !nonZero)){
                    return -col;
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
    tableData->pivot = matriz[piv.x][piv.y];



    double oldZ = matriz[0][cols-1];

    pivotRow(matriz[piv.x], piv, cols);

    for (int r = 0; r < rows; ++r){
        if (r != piv.x && matriz[r][piv.y] != 0){

            double k = matriz[r][piv.y] * -1;
            for (int c = 0; c < cols; ++c){
                matriz[r][c] += k * matriz[piv.x][c];
            }

            double mk = M[piv.y] * -1;
            for (int c = 0; c < cols; ++c){
                M[c] += mk * matriz[piv.x][c];
            }

        }
    }

    if (oldZ == matriz[0][cols-1]){
        return 3; //Degenerate (Pivot succesful)
    }

    return 0; //Pivot succesful
}

void solucionesMultiples(FILE* f, double** matriz, double* M, int cols, int rows, int maximize, double columnaPivoteNegativa){

    Pivot piv;
    piv.y = -columnaPivoteNegativa;
    TableData* tableData = malloc(sizeof(TableData));
    tableData->fractions = malloc(sizeof(double)*(rows-1));
    piv.x = fractions(matriz, M, cols, rows, piv.y, &piv, tableData);
    free(tableData->fractions);
    free(tableData);

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

void normalizeMatriz(FILE* f, double** matriz, double* M, int amountOfVariables, int* restrictions, int cols, int rows, int artificials, int maximize){

    double sign = maximize ? -1 : 1;

    for (int col = cols - artificials - 1; col < cols-1; ++col){ //Column of the restriction

        int row = -1;
        for (int r = 0; r < rows; ++r){ //Row corresponding to that restriction
            if (fabs(matriz[r][col] - 1) < eps){
                row = r;
                break;
            }
        }

        for (int var = 1; var < cols; ++var){
            M[var] += matriz[row][var] * sign;
        }
    }
}




/* ################################## SOLS ################################## */

void writeSolution(FILE* f, double** matriz, double* M, char** variableNames, double* solution, int cols, int rows, int maximize){

    fprintf(f, "The final result of ");
    if (maximize){
        fprintf(f, "maximizing ");
    } else {
        fprintf(f, "minimizing ");
    }
    fprintf(f, " the given function is %.3f as a result of setting the variables to the values: \n\n", matriz[0][cols-1]);

    fprintf(f, "\\begin{itemize}\n");
    for (int var = 1; var < cols-1; ++var){
        fprintf(f, "\\item $%s = %.3f$\n", variableNames[var-1], solution[var-1]);
    }
    fprintf(f, "\\end{itemize}\n");
}


void writeSolution2(FILE* f, double** matriz, double* M, char** variableNames, double* solution, int cols, int rows, int maximize){

    fprintf(f, "Another possible result of ");
    if (maximize){
        fprintf(f, "maximizing ");
    } else {
        fprintf(f, "minimizing ");
    }
    fprintf(f, " the given function with the same value is a result of setting the variables to the values: \n\n");

    fprintf(f, "\\begin{itemize}\n");
    for (int var = 1; var < cols-1; ++var){
        fprintf(f, "\\item $%s = %.3f$\n", variableNames[var-1], solution[var-1]);
    }
    fprintf(f, "\\end{itemize}\n");
}

void extractSolution(FILE* f, double** matriz, double* M, int amountOfVariables, char** variableNames, int cols, int rows, int maximize){

    double* solution = malloc(sizeof(double) * (cols - 2));
    double eps = 1e-2;

    int ones;
    int nonZero;
    for (int col = 1; col < cols-1; ++col){

        int index = -1;
        ones = 0; //La cantidad de 1s
        nonZero = 0; //Flag de valores no 0 y no 1
        for (int row = 1; row < rows; ++row){

            double valor = matriz[row][col];

            if (fabs(valor - 1.0) < eps){ // ==1
                ones++; //1s found
                index = row;

            } else if (!(fabs(valor) < eps)){ // != 0
                nonZero = 1; //Non zero (non one) value found
            }
        }

        if (ones == 1 && !nonZero){
            solution[col - 1] = matriz[index][cols-1];
        } else {
            solution[col - 1] = 0;
        }
    }


    fprintf(f, "\\textbf{Solution} \n");
    fprintf(f, "$$\n");
    fprintf(f, "\\begin{bmatrix}\n");

    for (int x = 0; x < amountOfVariables; x++){
        fprintf(f, "%s = %.2f \\\\ ", variableNames[x], solution[x]);
    }
    fprintf(f, "\n\\end{bmatrix}\n");
    fprintf(f, "$$\n\n");
    
    fprintf(f, "\\subsection{Optimal Solution}\n");
    writeSolution(f, matriz, M, variableNames, solution, cols, rows, maximize);

    free(solution);
}


void extractSolutions(FILE* f, double** solucionOriginal, double** matriz, double* M, int amountOfVariables, char** variableNames, int cols, int rows, int maximize){

    double* solution1 = malloc(sizeof(double) * (cols - 2));
    double* solution2 = malloc(sizeof(double) * (cols - 2));
    double eps = 1e-2;

    int ones;
    int nonZero;
    for (int col = 1; col < cols-1; ++col){

        int index = -1;
        ones = 0; //La cantidad de 1s
        nonZero = 0; //Flag de valores no 0 y no 1

        for (int row = 1; row < rows; ++row){

            double valor = solucionOriginal[row][col];

            if (fabs(valor - 1.0) < eps){ // ==1
                ones++; //1s found
                index = row;

            } else if (!(fabs(valor) < eps)){ // != 0
                nonZero = 1; //Non zero (non one) value found
                break;
            }
        }

        if (ones == 1 && !nonZero){
            solution1[col - 1] = solucionOriginal[index][cols-1];
        } else {
            solution1[col - 1] = 0;
        }
    }

    for (int col = 1; col < cols-1; ++col){

        int index = -1;
        ones = 0; //La cantidad de 1s
        nonZero = 0; //Flag de valores no 0 y no 1
        for (int row = 1; row < rows; ++row){

            double valor = matriz[row][col];

            if (fabs(valor - 1.0) < eps){ // ==1
                ones++; //1s found
                index = row;

            } else if (!(fabs(valor) < eps)){ // != 0
                nonZero = 1; //Non zero (non one) value found
            }
        }

        if (ones == 1 && !nonZero){
            solution2[col - 1] = matriz[index][cols-1];
        } else {
            solution2[col - 1] = 0;
        }
    }



    fprintf(f, "\\textbf{Ecuation} \n");
    fprintf(f, "$$\n");
    fprintf(f, "\\alpha \\cdot \n");
    fprintf(f, "\\begin{bmatrix}\n");
    for (int i = 0; i < amountOfVariables; ++i){
        fprintf(f, "%s = %.2f \\\\ ", variableNames[i], solution1[i]);
    }
    fprintf(f, "\\end{bmatrix}\n");
    fprintf(f, "+ (1 - \\alpha) \\cdot \n");
    fprintf(f, "\\begin{bmatrix}\n");
    for (int i = 0; i < amountOfVariables; ++i){
        fprintf(f, "%s = %.2f \\\\ ", variableNames[i], solution2[i]);
    }
    fprintf(f, " \\end{bmatrix}\n");
    fprintf(f, "$$\n");


    fprintf(f, "\\textbf{Other solutions} \n");
    double alpha = 0.25;
    for (int sol = 0; sol < 3; sol++){
        fprintf(f, "$$\\alpha = %.2f \\Rightarrow \n", alpha);
        fprintf(f, "\\begin{bmatrix}\n");

        for (int x = 0; x < amountOfVariables; x++){
            double value = solution1[x] * alpha + solution2[x] * (1 - alpha);
            fprintf(f, "%s = %.2f \\\\ ", variableNames[x], value);
        }

        fprintf(f, "\n\\end{bmatrix}\n");
        fprintf(f, "$$\n\n");
        alpha += 0.25;
    }
    fprintf(f, "\\subsection{Optimal Solutions}\n");
    writeSolution(f, matriz, M, variableNames, solution1, cols, rows, maximize);
    writeSolution2(f, matriz, M, variableNames, solution2, cols, rows, maximize);

    free(solution1);
    free(solution2);
}


/* ################################## MAIN ################################## */


void compileTex(char* problemName){
    printf("->] Compiling...\n");
    char command[300];
    sprintf(command, "pdflatex -file-line-error -interaction=batchmode -halt-on-error %s.tex >/dev/null 2>&1", problemName);
    int responseCode = system(command);
    if (responseCode == 0){
        printf("->] Latex compiled without problems\n");
        char pdfCommand[300];
        sprintf(pdfCommand, "evince --presentation %s.pdf &", problemName);
        system(pdfCommand);
    } else {
        printf("->] Error in LaTex compilation, check simplex.log for details\n");
        printf("> Terminating program...\n");
    }
}

void runSimplex(double** matriz, char* problemName, char** variableNames, int amountOfVariables, int saveMatrixes, int* restrictions, // [0:<, 1:=, 2:>]
    int cols, int rows, int maximize){

    // Malloc M Vector (First row)
    double* M = malloc(sizeof(double) * cols);
    for (int c = 0; c < cols; ++c){
        M[c] = 0;
    }

    //FILE* f = fopen("simplex.tex", "w");
    char texFileName[256];
    sprintf(texFileName, "%s.tex", problemName);

    FILE* f = fopen(texFileName, "w");

    if (!f){
        printf("Could not write file\n");
    }

    //Initialize M Vector
    int artificials = 0;
    for (int restriction = 0; restriction < rows - 1; ++restriction){
        if (restrictions[restriction] > 0){
            artificials++;
        }
    }
    for (int col = cols-2; col > cols-2-artificials; col--){
        if (maximize){
            M[col] = 1; //Assign 1M to cost (first row)
        } else {
            M[col] = -1; //Assign 1M to cost (first row)
        }
    }

    /*
    printf("Rows:%d  Cols:%d\n", rows, cols);
    printf("MVector: ");
    for (int x = 0; x < cols; ++x){
        printf("%d:%.2f ", x, M[x]);
    }
    printf("\n");

    for (int row = 0; row < rows; ++row){
        printf("%02d:\t", row);
        for (int col = 0; col < cols; ++col){
            printf("%.2f\t", matriz[row][col]);
        }
        printf("\n");
    }
    */


    if (!maximize){ //Invert first row (except Z and B)
        for (int col = 1; col < cols-1; ++col){
            matriz[0][col] *= -1;
        }
    }







    /*
    const char *newNameTemplate = "x_%d";
    for (int x = 0; x < amountOfVariables; ++x){

        if (variableNames[x][0] == 'x'){
            int len = snprintf(NULL, 0, newNameTemplate, x+1);
            char* newName = malloc(len + 2);
            snprintf(newName, len + 1, newNameTemplate, x+1);

            variableNames[x] = newName;
        }
    }*/

    documentStart(f);

    introduction(f);

    problem(f, matriz, M, problemName, variableNames, amountOfVariables, saveMatrixes, restrictions, // [0:<, 1:=, 2:>]
    cols, rows, maximize);


    if (artificials > 0){

        fprintf(f, "\\section{Initial Matrix with M cost}");
        fprintf(f, "The initial simplex table is shown below, where the cost of M is represented in the first row. This cost is added to the objective function to penalize the presence of artificial variables in the basis.\\\\\n\n");
        storeMatriz(f, matriz, M, variableNames, amountOfVariables, restrictions, cols, rows);

        normalizeMatriz(f, matriz, M, amountOfVariables, restrictions, cols, rows, artificials, maximize);

        fprintf(f, "\\section{Initial Normalized Matrix}\n");
        storeMatriz(f, matriz, M, variableNames, amountOfVariables, restrictions, cols, rows);

    } else {

        fprintf(f, "\\section{Initial Matrix }");
        fprintf(f, "The initial simplex table is shown below.\\\\\n\n");
        storeMatriz(f, matriz, M, variableNames, amountOfVariables, restrictions, cols, rows);

    }
    


    int status = 0; //En ejecucion
    int degenerate = 0;
    TableData* tableData = malloc(sizeof(TableData));
    tableData->fractions = malloc(sizeof(double)*(rows-1));
    if (saveMatrixes){
        fprintf(f, "\\section{Intermediate Matrixes}");
        fprintf(f, "The intermediate tables are shown below. A column is added to show the fractions of each row. The selected column to enter the basis is colored in pink while the pivot and selected fraction value are colored in a darker shade of pink.\\\\\n");
    }

    while (status == 0){

        for (int row = 0; row < rows-1; ++row){
            tableData->fractions[row] = INVALID_FRACTION;
        }
        tableData->pivot = INVALID_FRACTION;

        status = pivot(matriz, M, cols, rows, maximize, tableData, artificials);

        if (saveMatrixes && status == 0){
            fprintf(f, "\\subsection{Pivot Table}");
            storeIntermediateMatriz(f, matriz, M, variableNames, amountOfVariables, restrictions, cols, rows, tableData);
        }

        if (status == 3){
            degenerate = 1;
            fprintf(f, "\\section{Degenerate Table}");
            fprintf(f, "In this intermediate step, the problem degenerates. The basic variable with a value of zero is detailed below: \\\\\n");
            storeIntermediateMatriz(f, matriz, M, variableNames, amountOfVariables, restrictions, cols, rows, tableData);
            status = 0;
            if (saveMatrixes){
                fprintf(f, "\\section{Intermediate Matrixes}");
            }
        }
    }

    //GuardarMatriz Final

    if (degenerate){
        fprintf(f, "\\section{Degenerate Problem}");

        fprintf(f, "Sometimes the simplex algorithm may be faced with a degenerate problem, indicated by the presence of variables inside the base that have a value of 0 which in turn makes objetive function not get closer to the objective. In the simplex table is represented by a column where the minimal value taken is 0.\\\\\n");
        fprintf(f, "In this situation the program will take the first fraction that satisfies the restrictions. \\\\\n");

    }




    int nonFeasible = 0;
    for (int c = 0; c < cols - artificials - 1; ++c){ //Check for M values in the matri
        if (fabs(M[c]) > eps){
            nonFeasible = 1;
        }
    }




    if (nonFeasible){ // Non feasible - Matrix has M values

        fprintf(f, "\\section{Result Analysis}");

        //Reporte no factible
        fprintf(f, "\\subsection{Non feasible problems}\n");
        fprintf(f, "Sometimes the simplex algorithm may be faced with a non feasible problem, as a result of the model used. In the simplex table this is represented by coeficients of M different than zero in the first row.\\\\\n\n");

        //Last table
        storeMatriz(f, matriz, M, variableNames, amountOfVariables, restrictions, cols, rows);

    

    } else if (status == 2){

        fprintf(f, "\\section{Result Analysis}");

        //Reporte no acotado
        int unboundedColumn = tableData->y_pivot;
        fprintf(f, "\\subsection{Unbounded problems}\n");
        fprintf(f, "Sometimes the simplex algorithm may be faced with an unbounded problem, as a result of poor constraint management at the time of modeling. In the simplex table this is represented by a column full of negatives.\\\\\n");
        fprintf(f, "In this case it is found in the column %d where there are no valid fractions:\\\\\n", unboundedColumn);

        //Last table
        storeIntermediateMatriz(f, matriz, M, variableNames, amountOfVariables, restrictions, cols, rows, tableData);


    
    } else if (status < 0){

        fprintf(f, "\\section{Multiple Solutions}");

        //Soluciones multiples
        // Con (-col) de codigo de status
        
        fprintf(f, "\\subsection{Explanation}\n");
        fprintf(f, "It happens when an infinite number of solutions can be found to the same problem, through a particular formula.\\\\\n");
        fprintf(f, "This phenomenon is not typical of all the problems that the simplex algorithm encounters, it is only when a non-basic variable has a value of 0. This means that it can be manipulated to find more solutions, without affecting the gain.\\\\\n");
        fprintf(f, "Here is where it happens in this problem: \n\n");

        
        double** solucionOriginal = malloc(sizeof(double*) * rows);
        for (int r = 0; r < rows; ++r){
            solucionOriginal[r] = malloc(sizeof(double) * cols);
            for (int c = 0; c < cols; ++c){
                solucionOriginal[r][c] = matriz[r][c];
            }
        }

        //Calculate multiple solutions
        solucionesMultiples(f, matriz, M, cols, rows, maximize, status);

        //GuardarMatriz Solucion Original
        fprintf(f, "\\subsection{First solution table}\n");
        storeMatriz(f, solucionOriginal, M, variableNames, amountOfVariables, restrictions, cols, rows);

        //GuardarMatriz Solucion 2
        fprintf(f, "\\subsection{Second solution table}\n");
        storeMatriz(f, matriz, M, variableNames, amountOfVariables, restrictions, cols, rows);

        //Multiple solutions with alpha
        fprintf(f, "\\subsection{Equation and Solutions}\n");
        extractSolutions(f, solucionOriginal, matriz, M, amountOfVariables, variableNames, cols, rows, maximize);

        for (int r = 0; r < rows; ++r){
            free(solucionOriginal[r]);
        }
        free(solucionOriginal);

    } else {

        //Solucion

        fprintf(f, "\\section{Unique Solution}");

        fprintf(f, "\\subsection{Explanation}\n");
        fprintf(f, "In this case, the problem has a single optimal solution that satisfies the established constraints.\\\\\n");
        //REDACTAR

        //Solution Matrix
        fprintf(f, "\\subsection{Solution table}\n");
        storeMatriz(f, matriz, M, variableNames, amountOfVariables, restrictions, cols, rows);

        //Solution
        fprintf(f, "\\subsection{Unique Solution}\n");
        extractSolution(f, matriz, M, amountOfVariables, variableNames, cols, rows, maximize);

    }

    free(tableData->fractions);
    free(tableData);

    documentEnd(f);
    compileTex(problemName);
}

void test1(){


    int maximize = 0; //Minimizar

    int rows = 4;
    int cols = 8;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    int amountOfVariables = 2;
    int amountOfrestrictions = rows - 1;
    int amountOfVariablesRestrictions = cols - amountOfVariables - 2;

    int* restrictions = malloc(sizeof(int) * amountOfrestrictions);
    restrictions[0] = 0;
    restrictions[1] = 2;
    restrictions[2] = 1;


    char** variableNames = malloc(sizeof(char*) * (cols-2));
    variableNames[0] = "x1";
    variableNames[1] = "x2";
    variableNames[2] = "s1";
    variableNames[3] = "e1";
    variableNames[4] = "a1";
    variableNames[5] = "a2";


    matriz[0][0] = 1;
    matriz[0][1] = -2;
    matriz[0][2] = -3;
    matriz[0][3] = 0;
    matriz[0][4] = 0;
    matriz[0][5] = 0;
    matriz[0][6] = 0;
    matriz[0][7] = 0;

    matriz[1][0] = 0;
    matriz[1][1] = 0.5;
    matriz[1][2] = 0.25;
    matriz[1][3] = 1;
    matriz[1][4] = 0;
    matriz[1][5] = 0;
    matriz[1][6] = 0;
    matriz[1][7] = 4;

    matriz[2][0] = 0;
    matriz[2][1] = 1;
    matriz[2][2] = 3;
    matriz[2][3] = 0;
    matriz[2][4] = -1;
    matriz[2][5] = 1;
    matriz[2][6] = 0;
    matriz[2][7] = 20;

    matriz[3][0] = 0;
    matriz[3][1] = 1;
    matriz[3][2] = 1;
    matriz[3][3] = 0;
    matriz[3][4] = 0;
    matriz[3][5] = 0;
    matriz[3][6] = 1;
    matriz[3][7] = 10;

    if (!maximize){ //Invert first row (except Z and B)
        for (int col = 1; col < cols-1; ++col){
            matriz[0][col] *= -1;
        }
    }

    



    runSimplex(matriz, "Test", variableNames, amountOfVariables, 1, restrictions, // [0:<, 1:=, 2:>]
    cols, rows, maximize);
}

void test2(){


    int maximize = 1; //Maximizar

    int rows = 4;
    int cols = 8;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    int amountOfVariables = 2;
    int amountOfrestrictions = rows - 1;
    int amountOfVariablesRestrictions = cols - amountOfVariables - 2;

    int* restrictions = malloc(sizeof(int) * amountOfrestrictions);
    restrictions[0] = 0; //<
    restrictions[1] = 0; //<
    restrictions[2] = 2; //>


    char** variableNames = malloc(sizeof(char*) * (cols-2));
    variableNames[0] = "x1";
    variableNames[1] = "x2";
    variableNames[2] = "s1";
    variableNames[3] = "s2";
    variableNames[4] = "e1";
    variableNames[5] = "a1";


    matriz[0][0] = 1;
    matriz[0][1] = -10000;
    matriz[0][2] = -3000;
    matriz[0][3] = 0;
    matriz[0][4] = 0;
    matriz[0][5] = 0;
    matriz[0][6] = 0;
    matriz[0][7] = 0;

    matriz[1][0] = 0;
    matriz[1][1] = 1;
    matriz[1][2] = 1;
    matriz[1][3] = 1;
    matriz[1][4] = 0;
    matriz[1][5] = 0;
    matriz[1][6] = 0;
    matriz[1][7] = 7;

    matriz[2][0] = 0;
    matriz[2][1] = 10;
    matriz[2][2] = 4;
    matriz[2][3] = 0;
    matriz[2][4] = 1;
    matriz[2][5] = 0;
    matriz[2][6] = 0;
    matriz[2][7] = 40;

    matriz[3][0] = 0;
    matriz[3][1] = 0;
    matriz[3][2] = 1;
    matriz[3][3] = 0;
    matriz[3][4] = 0;
    matriz[3][5] = -1;
    matriz[3][6] = 1;
    matriz[3][7] = 3;

    if (!maximize){ //Invert first row (except Z and B)
        for (int col = 1; col < cols-1; ++col){
            matriz[0][col] *= -1;
        }
    }

    



    runSimplex(matriz, "Test", variableNames, amountOfVariables, 1, restrictions, // [0:<, 1:=, 2:>]
    cols, rows, maximize);
}

void test3(){


    int maximize = 1; //Maximizar

    int rows = 5;
    int cols = 10;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    int amountOfVariables = 2;
    int amountOfrestrictions = rows - 1;
    int amountOfVariablesRestrictions = cols - amountOfVariables - 2;

    int* restrictions = malloc(sizeof(int) * amountOfrestrictions);
    restrictions[0] = 0; //<
    restrictions[1] = 0; //<
    restrictions[2] = 2; //>
    restrictions[3] = 2; //>


    char** variableNames = malloc(sizeof(char*) * (cols-2));
    variableNames[0] = "x1";
    variableNames[1] = "x2";
    variableNames[2] = "s1";
    variableNames[3] = "s2";
    variableNames[4] = "e1";
    variableNames[5] = "e2";
    variableNames[6] = "a1";
    variableNames[7] = "a2";


    matriz[0][0] = 1;
    matriz[0][1] = -300;
    matriz[0][2] = -200;
    matriz[0][3] = 0;
    matriz[0][4] = 0;
    matriz[0][5] = 0;
    matriz[0][6] = 0;
    matriz[0][7] = 0;
    matriz[0][8] = 0;
    matriz[0][9] = 0;

    matriz[1][0] = 0;
    matriz[1][1] = 1.0/40.0;
    matriz[1][2] = 1.0/60.0;
    matriz[1][3] = 1;
    matriz[1][4] = 0;
    matriz[1][5] = 0;
    matriz[1][6] = 0;
    matriz[1][7] = 0;
    matriz[1][8] = 0;
    matriz[1][9] = 1;

    matriz[2][0] = 0;
    matriz[2][1] = 0.02;
    matriz[2][2] = 0.02;
    matriz[2][3] = 0;
    matriz[2][4] = 1;
    matriz[2][5] = 0;
    matriz[2][6] = 0;
    matriz[2][7] = 0;
    matriz[2][9] = 1;

    matriz[3][0] = 0;
    matriz[3][1] = 1;
    matriz[3][2] = 0;
    matriz[3][3] = 0;
    matriz[3][4] = 0;
    matriz[3][5] = -1;
    matriz[3][6] = 0;
    matriz[3][7] = 1;
    matriz[3][8] = 0;
    matriz[3][9] = 30;

    matriz[4][0] = 0;
    matriz[4][1] = 0;
    matriz[4][2] = 1;
    matriz[4][3] = 0;
    matriz[4][4] = 0;
    matriz[4][5] = 0;
    matriz[4][6] = -1;
    matriz[4][7] = 0;
    matriz[4][8] = 1;
    matriz[4][9] = 20;

    if (!maximize){ //Invert first row (except Z and B)
        for (int col = 1; col < cols-1; ++col){
            matriz[0][col] *= -1;
        }
    }

    
    runSimplex(matriz, "Test", variableNames, amountOfVariables, 1, restrictions, // [0:<, 1:=, 2:>]
    cols, rows, maximize);
}

void test4(){


    int maximize = 1; //Maximizar

    int rows = 5;
    int cols = 10;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    int amountOfVariables = 2;
    int amountOfrestrictions = rows - 1;
    int amountOfVariablesRestrictions = cols - amountOfVariables - 2;

    int* restrictions = malloc(sizeof(int) * amountOfrestrictions);
    restrictions[0] = 0; //<
    restrictions[1] = 0; //<
    restrictions[2] = 2; //>
    restrictions[3] = 2; //>


    char** variableNames = malloc(sizeof(char*) * (cols-2));
    variableNames[0] = "x1";
    variableNames[1] = "x2";
    variableNames[2] = "s1";
    variableNames[3] = "s2";
    variableNames[4] = "e1";
    variableNames[5] = "e2";
    variableNames[6] = "a1";
    variableNames[7] = "a2";


    matriz[0][0] = 1;
    matriz[0][1] = -200;
    matriz[0][2] = -300;
    matriz[0][3] = 0;
    matriz[0][4] = 0;
    matriz[0][5] = 0;
    matriz[0][6] = 0;
    matriz[0][7] = 0;
    matriz[0][8] = 0;
    matriz[0][9] = 0;

    matriz[1][0] = 0;
    matriz[1][1] = 0.03;
    matriz[1][2] = 0.02;
    matriz[1][3] = 1;
    matriz[1][4] = 0;
    matriz[1][5] = 0;
    matriz[1][6] = 0;
    matriz[1][7] = 0;
    matriz[1][8] = 0;
    matriz[1][9] = 1;

    matriz[2][0] = 0;
    matriz[2][1] = 0.02;
    matriz[2][2] = 0.02;
    matriz[2][3] = 0;
    matriz[2][4] = 1;
    matriz[2][5] = 0;
    matriz[2][6] = 0;
    matriz[2][7] = 0;
    matriz[2][9] = 1;

    matriz[3][0] = 0;
    matriz[3][1] = 1;
    matriz[3][2] = 0;
    matriz[3][3] = 0;
    matriz[3][4] = 0;
    matriz[3][5] = -1;
    matriz[3][6] = 0;
    matriz[3][7] = 1;
    matriz[3][8] = 0;
    matriz[3][9] = 30;

    matriz[4][0] = 0;
    matriz[4][1] = 0;
    matriz[4][2] = 1;
    matriz[4][3] = 0;
    matriz[4][4] = 0;
    matriz[4][5] = 0;
    matriz[4][6] = -1;
    matriz[4][7] = 0;
    matriz[4][8] = 1;
    matriz[4][9] = 20;

    if (!maximize){ //Invert first row (except Z and B)
        for (int col = 1; col < cols-1; ++col){
            matriz[0][col] *= -1;
        }
    }

    
    runSimplex(matriz, "Test", variableNames, amountOfVariables, 1, restrictions, // [0:<, 1:=, 2:>]
    cols, rows, maximize);
}

void test5(){


    int maximize = 1; //Maximizar

    int rows = 5;
    int cols = 10;
    double** matriz = malloc(sizeof(double*) * rows);
    for (int r = 0; r < rows; ++r){
        matriz[r] = malloc(sizeof(double) * cols);
    }

    int amountOfVariables = 2;
    int amountOfrestrictions = rows - 1;
    int amountOfVariablesRestrictions = cols - amountOfVariables - 2;

    int* restrictions = malloc(sizeof(int) * amountOfrestrictions);
    restrictions[0] = 0; //<
    restrictions[1] = 0; //<
    restrictions[2] = 2; //>
    restrictions[3] = 2; //>


    char** variableNames = malloc(sizeof(char*) * (cols-2));
    variableNames[1] = "x1";
    variableNames[0] = "x2";
    variableNames[2] = "s1";
    variableNames[3] = "s2";
    variableNames[4] = "e1";
    variableNames[5] = "e2";
    variableNames[6] = "a1";
    variableNames[7] = "a2";


    matriz[0][0] = 1;
    matriz[0][2] = -300;
    matriz[0][1] = -200;
    matriz[0][3] = 0;
    matriz[0][4] = 0;
    matriz[0][5] = 0;
    matriz[0][6] = 0;
    matriz[0][7] = 0;
    matriz[0][8] = 0;
    matriz[0][9] = 0;

    matriz[1][0] = 0;
    matriz[1][2] = 0.03;
    matriz[1][1] = 0.02;
    matriz[1][3] = 1;
    matriz[1][4] = 0;
    matriz[1][5] = 0;
    matriz[1][6] = 0;
    matriz[1][7] = 0;
    matriz[1][8] = 0;
    matriz[1][9] = 1;

    matriz[2][0] = 0;
    matriz[2][2] = 0.02;
    matriz[2][1] = 0.02;
    matriz[2][3] = 0;
    matriz[2][4] = 1;
    matriz[2][5] = 0;
    matriz[2][6] = 0;
    matriz[2][7] = 0;
    matriz[2][9] = 1;

    matriz[3][0] = 0;
    matriz[3][2] = 1;
    matriz[3][1] = 0;
    matriz[3][3] = 0;
    matriz[3][4] = 0;
    matriz[3][5] = -1;
    matriz[3][6] = 0;
    matriz[3][7] = 1;
    matriz[3][8] = 0;
    matriz[3][9] = 30;

    matriz[4][0] = 0;
    matriz[4][2] = 0;
    matriz[4][1] = 1;
    matriz[4][3] = 0;
    matriz[4][4] = 0;
    matriz[4][5] = 0;
    matriz[4][6] = -1;
    matriz[4][7] = 0;
    matriz[4][8] = 1;
    matriz[4][9] = 20;

    if (!maximize){ //Invert first row (except Z and B)
        for (int col = 1; col < cols-1; ++col){
            matriz[0][col] *= -1;
        }
    }

    
    runSimplex(matriz, "Test", variableNames, amountOfVariables, 1, restrictions, // [0:<, 1:=, 2:>]
    cols, rows, maximize);
}
 



// int main(){

//     test1();
//     test2();
//     test3();
//     test4();
//     test5();

//     return 0;
// }