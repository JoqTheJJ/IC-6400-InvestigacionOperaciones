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

#include <gtk/gtk.h>
#include <cairo.h>
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
    fprintf(f, "Z = %s \\cdot %2f", variableNames[0], matriz[0][1]*-1);
    for (int var = 1; var < amountOfVariables; ++var){
        fprintf(f, " + %s \\cdot %2f", variableNames[var], matriz[0][var+1]*-1);
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

void storeMatriz(FILE* f, double** matriz, char** varNames, int amountOfVariables, int* restrictions, int cols, int rows){

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
    for (int x = 0; x < amountOfVariables; ++x){
        fprintf(f, " & $s_{%d}$", x+1);
    }

    /*
    for (int x = 0; x < cols - 2; ++x){
        fprintf(f, " & $%s$", varNames[x]);
    }*/

    /**
    for (int s = 0; s < restrictions[0]; ++s){
        fprintf(f, " & $s_{%d}$", s+1);
    }
    for (int e = 0; e < restrictions[2]; ++e){
        fprintf(f, " & $e_{%d}$", e+1);
    }
    for (int a = 0; a < restrictions[1] + restrictions[2]; ++a){
        fprintf(f, " & $a_{%d}$", a+1);
    }**/
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

void storeIntermediateMatriz(FILE* f, double** matriz, char** varNames, int amountOfVariables, int* restrictions, int cols, int rows, TableData* td){

    fprintf(f, "\\begin{center}\n");

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
    
    for (int x = 0; x < amountOfVariables; ++x){
        fprintf(f, " & $x_{%d}$", x+1);
    }
    for (int x = 0; x < amountOfVariables; ++x){
        fprintf(f, " & $s_{%d}$", x+1);
    }

    /*
    for (int x = 0; x < cols - 2; ++x){
        fprintf(f, " & $%s$", varNames[x]);
    }*/

    /*
    for (int s = 0; s < restrictions[0]; ++s){
        fprintf(f, " & $s_{%d}$", s+1);
    }
    for (int e = 0; e < restrictions[2]; ++e){
        fprintf(f, " & $e_{%d}$", e+1);
    }
    for (int a = 0; a < restrictions[1] + restrictions[2]; ++a){
        fprintf(f, " & $a_{%d}$", a+1);
    }*/
    fprintf(f, " & b");
    fprintf(f, " & Fractions");
    fprintf(f, "\\\\\n        \\hline\n");

    


    double* fractions = td->fractions;
    int x = td->x_pivot;
    int y = td->y_pivot;

    fprintf(f, "        \\hline\n");
    //Matrix cells
    for (int row = 0; row < rows; ++row){
        fprintf(f, "        %.3f", matriz[row][0]);



        for (int col = 1; col < cols+1; col++){ //+1 to store fractions

            double value = matriz[row][col];
            

            if (y == col){
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
                fprintf(f, "& %.3f", value);
            }
            
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

int fractions(double** matriz, int cols, int rows, int y, TableData* tableData){

    double min = INVALID_FRACTION; //Big number for overwrite
    int decisiones = 0;
    int x = -1;
    for (int r = 1; r < rows; ++r){
        double frac = matriz[r][cols-1] / matriz[r][y];

        tableData->fractions[r-1] = frac;

        if (matriz[r][y] > 0 && frac > 0 && frac < min){ //b positivo
            // frac < min escoge en primero en caso de empate

            min = frac;
            x = r;
            decisiones = 0;

        } else if (matriz[r][y] > 0 && frac >= 0 && min == INVALID_FRACTION){ //0 degenerado
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

Pivot escogerPivote(double** matriz, int cols, int rows, int maximize, TableData* tableData){

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

    tableData->x_pivot = piv.x;
    tableData->y_pivot = piv.y;
    tableData->pivot = INVALID_FRACTION;

    if (piv.x == -1 && piv.y == -1){
        //Revisar soluciones multiples

        for (int col = 1; col < cols-1; ++col){
            
            if (matriz[0][col] == 0){ //Variable con 0


                double suma = 0;
                for (int row = 1; row < rows; ++row){
                    suma += matriz[row][col];
                }

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
    tableData->pivot = matriz[piv.x][piv.y];



    double oldZ = matriz[0][cols-1];

    pivotRow(matriz[piv.x], piv, cols);

    for (int r = 0; r < rows; ++r){
        if (r != piv.x && matriz[r][piv.y] != 0){


            double k = matriz[r][piv.y] * -1;

            for (int c = 0; c < cols; ++c){
                matriz[r][c] += k * matriz[piv.x][c];
            }

        }
    }

    if (oldZ == matriz[0][cols-1]){
        return 3; //Degenerate (Pivot succesful)
    }

    return 0; //Pivot succesful
}

void solucionesMultiples(FILE* f, double** matriz, int cols, int rows, int maximize, double columnaPivoteNegativa){

    Pivot piv;
    piv.y = -columnaPivoteNegativa;
    TableData* tableData = malloc(sizeof(TableData));
    tableData->fractions = malloc(sizeof(double)*(rows-1));
    piv.x = fractions(matriz, cols, rows, piv.y, tableData);
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






/* ################################## SOLS ################################## */


void extractSolution(FILE* f, double** matriz, int amountOfVariables, char** variableNames, int cols, int rows, int maximize){

    double* solution = malloc(sizeof(double) * amountOfVariables);
    double eps = 1e-2;

    int ones;
    int nonZero;
    for (int col = 1; col <= amountOfVariables; ++col){

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
    

}


void extractSolutions(FILE* f, double** solucionOriginal, double** matriz, int amountOfVariables, int cols, int rows, int maximize){

    double* solution1 = malloc(sizeof(double) * amountOfVariables);
    double* solution2 = malloc(sizeof(double) * amountOfVariables);
    double eps = 1e-2;

    int ones;
    int nonZero;
    for (int col = 1; col <= amountOfVariables; ++col){

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
            }
        }

        if (ones == 1 && !nonZero){
            solution1[col - 1] = solucionOriginal[index][cols-1];
        } else {
            solution1[col - 1] = 0;
        }
    }

    for (int col = 1; col <= amountOfVariables; ++col){

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
    printf("->] Compiling...\n");
    int responseCode = system("pdflatex -file-line-error -interaction=batchmode -halt-on-error simplex.tex >/dev/null 2>&1");
    if (responseCode == 0){
        printf("->] Latex compiled without problems\n");
        system("evince --presentation simplex.pdf &");
    }
}

void runSimplex(double** matriz, char* problemName, char** names, int amountOfVariables, int saveMatrixes, int* restrictions, // [0:<, 1:=, 2:>]
    int cols, int rows, int maximize){


    FILE* f = fopen("simplex.tex", "w");

    if (!f){
        printf("Could not write file\n");
    }

    if (!maximize){
        for (int col = 0; col < cols; ++col){
            matriz[0][col] *= -1;
        }
    }

    for (int i = 0; i < cols-2-amountOfVariables; ++i){
        printf("Restriction[%d] = %d\n", i, restrictions[i]);
    }

    char** variableNames = malloc(sizeof(char*)*(cols-2));
    int index = 0;
    for (int x = 0; x < amountOfVariables; ++x){
        variableNames[x] = names[x];
        index = x;
    }
    for (true; index < cols - 2 - amountOfVariables; index++){
        variableNames[index] = "SSS";
    }


    documentStart(f);

    introduction(f);

    problem(f, matriz, problemName, variableNames, amountOfVariables, saveMatrixes, restrictions, // [0:<, 1:=, 2:>]
    cols, rows, maximize);


    fprintf(f, "\\section{Initial Matrix}");
    storeMatriz(f, matriz, variableNames, amountOfVariables, restrictions, cols, rows);

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

        status = pivot(matriz, cols, rows, maximize, tableData);

        if (saveMatrixes && status == 0){
            fprintf(f, "\\subsection{Pivot Table}");
            storeIntermediateMatriz(f, matriz, variableNames, amountOfVariables, restrictions, cols, rows, tableData);
        }

        if (status == 3){
            degenerate = 1;
            fprintf(f, "\\section{Degenerate Table}");
            fprintf(f, "In this intermediate step, the problem degenerates. The basic variable with a value of zero is detailed below: \\\\\n");
            storeIntermediateMatriz(f, matriz, variableNames, amountOfVariables, restrictions, cols, rows, tableData);
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

    if (status == 2){

        fprintf(f, "\\section{Result Analysis}");

        //Reporte no acotado
        int unboundedColumn = tableData->y_pivot;
        fprintf(f, "\\subsection{Unbounded problems}\n");
        fprintf(f, "Sometimes the simplex algorithm may be faced with an unbounded problem, as a result of poor constraint management at the time of modeling. In the simplex table is represented by a column full of negatives\\\\\n");
        fprintf(f, "In this case it is found in the column %d where there are no valid fractions:", unboundedColumn);

        

        //Last table
        storeIntermediateMatriz(f, matriz, variableNames, amountOfVariables, restrictions, cols, rows, tableData);
    
    } else if (status < 0){

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
        storeMatriz(f, solucionOriginal, variableNames, amountOfVariables, restrictions, cols, rows);

        //GuardarMatriz Solucion 2
        fprintf(f, "\\subsection{Second solution table}\n");
        storeMatriz(f, matriz, variableNames, amountOfVariables, restrictions, cols, rows);

        //Multiple solutions with alpha
        fprintf(f, "\\subsection{Equation and Solutions}\n");
        extractSolutions(f, solucionOriginal, matriz, amountOfVariables, cols, rows, maximize);

    } else {

        //Solucion

        fprintf(f, "\\section{Unique Solution}");

        fprintf(f, "\\subsection{Explanation}\n");
        fprintf(f, "In this case, the problem has a single optimal solution that satisfies the established constraints.\\\\\n");
        //REDACTAR

        //Solution Matrix
        fprintf(f, "\\subsection{Solution table}\n");
        storeMatriz(f, matriz, variableNames, amountOfVariables, restrictions, cols, rows);

        //Solution
        fprintf(f, "\\subsection{Unique Solution}\n");
        extractSolution(f, matriz, amountOfVariables, variableNames, cols, rows, maximize);

    }

    //fprintf(f, "\\section{Graph}\n");
    free(tableData);

    //Dibujo 2D

    //Dibujo 3D

    documentEnd(f);
    compileTex();
}


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

/*int main(){

    test7();

    return 0;
}*/