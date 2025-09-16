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

#define INF INT_MAX



typedef struct {
    int max;
    int ganadores;
} Cell;

int ipow(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

int bitIndex(int x){
    int index = 0;

    while (x > 1){
        x >>= 1;
        index++;
    }
    return index;
}

int min(int x1, int x2){
    if (x1 > x2){
        return x2;
    }
    return x1;
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
        "    {\\large Computer Science\\par}\n"
        "    {\\large Operations Research\\par}\n"
        "    \\vspace{2cm}\n"
        "\n"
        "    %% Title\n"
        "    {\\Large Knapsack Problem\\par}\n"
        "    {\\large Dynamic Programming\\par}\n"
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
        "    {\\large 19 september 2025\\par}\n"
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

    fprintf(f, "\\section{Knapsack Problem}\n");
    fprintf(f, "The \\textit{Knapsack problem} is a classic optimization problem. ");
    fprintf(f, "The goal is to fill a backpack optimally with a set of objects, each with a weight and a profit, ");
    fprintf(f, "in order to maximize the total profit without exceeding the backpack's capacity.\n\n");

    fprintf(f, "There are a few main types of knapsack problems:\n");
    fprintf(f, "\\begin{itemize}\n");
    fprintf(f, "  \\item \\textbf{0/1 Knapsack:} Each object can be taken or not, only one copy per object.\n");
    fprintf(f, "  \\item \\textbf{Bounded Knapsack:} Each object has a limited number of copies.\n");
    fprintf(f, "  \\item \\textbf{Unbounded Knapsack:} Each object can be taken any number of times, as long as the total weight allows.\n");
    fprintf(f, "\\end{itemize}\n\n");

    fprintf(f, "\\subsection{Solution}\n");
    fprintf(f, "A common way to solve these problems is using dynamic programming. ");
    fprintf(f, "We build a table to keep track of the optimal profit for different capacities and numbers of objects. ");
    fprintf(f, "By filling this table, we can find the maximum profit achievable for the given backpack capacity.\n");
    fprintf(f, "The resulting table will show the exact quantity and which objets take in order to archive the optimal weight.\n");

}

void problem(FILE* f, int objects, int capacity, int* profits, int* costs, int* quantity, char** names){

    fprintf(f, "\\section{Problem}\n\n");

    for (int obj = 0; obj < objects; ++obj){
        if (quantity[obj] == INT_MAX){ //infinito
            fprintf(f, "%s: Amount: $\\infty$, Profit:%d, and Cost:%d\n\n", names[obj], profits[obj], costs[obj]);
        } else {
            fprintf(f, "%s: Amount:%d, Profit:%d, and Cost:%d\n\n", names[obj], quantity[obj], profits[obj], costs[obj]);
        }
    }

     fprintf(f, "\n\n\nThis translates to:\n\n");


    fprintf(f, "Maximize $Z = ");
    fprintf(f, "%dX_{\\text{%s}}", profits[0], names[0]);

    for (int obj = 1; obj < objects; ++obj){
        fprintf(f, " + %dX_{\\text{%s}}", profits[obj], names[obj]);
    }
    fprintf(f, "$\n\n");



    fprintf(f, "\n\nSubject to:\n\n");
    fprintf(f, "$%d \\geq ", capacity);
    fprintf(f, "%dX_{\\text{%s}}", costs[0], names[0]);
    

    for (int obj = 1; obj < objects; ++obj){
        fprintf(f, " + %dX_{\\text{%s}}", costs[obj], names[obj]);
    }
    fprintf(f, "$\n\n");



    for (int obj = 0; obj < objects; ++obj){
        if (quantity[obj] == INT_MAX){ //infinito
            fprintf(f, "$X_{\\text{%s}} \\leq \\infty $\n\n", names[obj]);
        } else {
            fprintf(f, "$X_{\\text{%s}} \\leq %d$\n\n", names[obj], quantity[obj]);
        }
    }
    fprintf(f, "\n");

}

void texTable(FILE* f, Cell** m, int objects, int capacity, int* profits, int* costs, int* quantity, char** names){

    fprintf(f, "\\section{Costs Table}\n");

    fprintf(f, "\\begin{center}\n");

    fprintf(f, "    \\begin{tabular}{|c||");
    for (int col = 0; col < objects; ++col){
        fprintf(f, "c|");
    }
    fprintf(f, "}\n");



    fprintf(f, "        \\hline\n");
    fprintf(f, "        \\textbf{Capacity} ");
    for (int col = 0; col < objects; ++col){
        fprintf(f, "& \\textbf{%s} ", names[col]);
    }
    fprintf(f, "\\\\\n        \\hline\n");
    fprintf(f, "        \\hline\n");


    for (int i = 0; i <= capacity; ++i){ 
        fprintf(f, "        \\textbf{%d}", i);
        for (int j = 1; j <= objects; ++j){ // start in 1 to skip 0s column

            int winner = m[i][j].ganadores;
            int draw = winner & (winner-1); // checks for draw (more than one bit set)

            if (draw){ //more than one option
                fprintf(f, "& \\cellcolor[HTML]{3F62FC}$%d$ x={", m[i][j].max); //TODO (blue for now)

                int first = 1;
                int maxAmount = capacity / costs[j-1];
                int iterations = min(quantity[j-1], maxAmount);
                for (int q = 0; q <= iterations; ++q){
                    if (winner % 2){
                        if (!first){
                            fprintf(f, ",");
                        }
                        first = 0;
                        fprintf(f, "%d", q);
                    }
                    winner /= 2;
                }

                fprintf(f, "}");


            } else if (winner == 1){ // Not take the object
                fprintf(f, "& \\cellcolor[HTML]{FC3F3F}$%d$ x={0}", m[i][j].max); //red


            } else { // Take the object in n capacity
                int amount = bitIndex(winner);
                fprintf(f, "& \\cellcolor[HTML]{3FFC45}$%d$ x={%d} ", m[i][j].max, amount); //green
            }
        }
        fprintf(f, "\\\\\n        \\hline\n");
    }

    fprintf(f, "    \\end{tabular}\n");
    fprintf(f, "\\end{center}\n\n\n");
}

void printSolution(FILE* f, int* solution, int objects, char** names){
    printf("[");
    fprintf(f, "$");
    for (int obj = 0; obj < objects; ++obj){
        fprintf(f, "X_{\\text{%s}}=%d \\:", names[obj], solution[obj]);
        printf("x%d:%d ", obj, solution[obj]);
    }
    fprintf(f, "$ \\newline\n");
    printf("]\n");
}


/* ################################## KNAPSACK ################################## */

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

            int maxAmount = capacity / costs[obj];
            int iterations = min(quantity[obj], maxAmount);
            printf("MAX AMOUNT %d     ITERATIONS %d \n", maxAmount, iterations);
            for (int q = 0; q <= iterations; ++q){

                newCapacity = c - q*costs[obj];
                if (newCapacity < 0){ //Exceeds capacity
                    break;
                }
                current = q*profits[obj] + res[newCapacity][obj].max; //is (obj-1) implicitly

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

void optimalSolutionsAux(int capacity, int row, int col, Cell** solution, int* res, int objects, int* costs, int* quantity, char** names, FILE* f){

    if (col == 0){
        //printf("Prints\n");
        printSolution(f, res, objects, names);
        return;
    }

    int winner = solution[row][col].ganadores;

    //printf("Row: %d, col: %d, winner %d\n", row, col, winner);

    int maxAmount = capacity / costs[col-1];
    int iterations = min(quantity[col-1], maxAmount);
    //printf("MAX AMOUNT %d     ITERATIONS %d  QUANTITY(COL-1) %d\n", maxAmount, iterations, quantity[col-1]);
    for (int amount = 0; amount <= iterations; amount++){
        printf("Amount: %d, quantity[%d]=%d\n", amount, col-1, quantity[col-1]);
        if (winner % 2){
            res[col - 1] = amount; //Store the amount of the current object

            //printf("Goes to: %d\n", amount);
            optimalSolutionsAux(capacity,
                row - amount*costs[col-1], //row(capacity)) - amount * object cost
                col - 1,  //goes down a col
                solution, 
                res,
                objects,
                costs,
                quantity,
                names,
                f
            );
        }
        winner /= 2;
    }
}

void optimalSolutions(Cell** solution, int objects, int maxCapacity, int* costs, int* quantity, char** names, FILE* f){

    fprintf(f, "\\section{Optimal Solutions}\n");

    int* res = malloc(sizeof(int)*objects);
    optimalSolutionsAux(maxCapacity, maxCapacity, objects, solution, res, objects, costs, quantity, names, f);
    free(res);
}

/* ################################## MAIN ################################## */

void runKnapsack(int objects, int capacity, int* profits, int* costs, int* quantity, char** names){

    printf("Objetos=%d, Capacidad=%d\n", objects, capacity);
    for (int i = 0; i < objects; i++) {
        printf("Obj %d: cost=%d, profit=%d, qty=%d, name=%s\n",
               i, costs[i], profits[i], quantity[i], names[i]);
    }

    FILE* f = fopen("programToLaTeX.tex", "w");
    if (f == NULL) {
        printf("Error: File null\n");
        return;
    }

    documentStart(f);
    introduction(f);
    fflush(f);


    Cell** answer = knapsack(objects, capacity, profits, costs, quantity);
    for(int i=0;i<=capacity;i++){
        for(int j=0;j<=objects;j++){
            printf("answer[%d][%d] = max:%d ganadores:%d\n", i,j,answer[i][j].max, answer[i][j].ganadores);
        }
    }

    problem(f, objects, capacity, profits, costs, quantity, names);

    //Print results table
    texTable(f, answer, objects, capacity, profits, costs, quantity, names);
    //Print optimal solutions
    optimalSolutions(answer, objects, capacity, costs, quantity, names, f);

    fprintf(f, "\\end{document}\n");

    fclose(f);



    for (int i = 0; i <= capacity; ++i){
        free(answer[i]);
    }
    free(answer);

    int responseCode = system("pdflatex programToLaTeX.tex");
    if (responseCode == 0){
        printf("\n\nLatex compiled without problems\n");
        system("evince --presentation programToLaTeX.pdf &");
    }
}

void test(){

    //Simulate Inputs
    int objects = 7;
    int capacity = 9;
    
    int* profits = malloc(sizeof(int) * objects);
    profits[0] = 7;
    profits[1] = 9;
    profits[2] = 5;
    profits[3] = 12;
    profits[4] = 14;
    profits[5] = 6;
    profits[6] = 12;

    int* costs = malloc(sizeof(int) * objects);
    costs[0] = 3;
    costs[1] = 4;
    costs[2] = 2;
    costs[3] = 6;
    costs[4] = 7;
    costs[5] = 3;
    costs[6] = 5;

    int* quantity = malloc(sizeof(int) * objects);
    quantity[0] = 1;
    quantity[1] = 1;
    quantity[2] = 1;
    quantity[3] = 1;
    quantity[4] = 1;
    quantity[5] = 1;
    quantity[6] = 1;

    char** names = malloc(sizeof(char*) * objects);
    names[0] = "O0";
    names[1] = "O1";
    names[2] = "O2";
    names[3] = "O3";
    names[4] = "O4";
    names[5] = "O5";
    names[6] = "O6";


    runKnapsack(objects, capacity, profits, costs, quantity, names);
}