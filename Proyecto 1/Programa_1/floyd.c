/*
                Project 1: Shortest Paths (Floyd's Algorythm)
                Made by: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
                Date: Friday, September 12, 2025

                This section contains the program used to calculate the shortest
                path between any pair of nodes in a graph with weighted distances.
                This program will do this utilizing the Floyd Algorithm and then
                create a LATEX document with all the tables calculated to get to
                the result.
*/


#include <errno.h>
#include <unistd.h>
#include <limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

#define INF __INT_MAX__



/* ################################## TEX ################################## */

void documentStart(FILE* f){
    fprintf(f, "\\documentclass{article}\n\n");

    //Packages
    fprintf(f, "\\PassOptionsToPackage{table,svgnames}{xcolor}");
    fprintf(f, "\\usepackage{graphicx}\n");
    fprintf(f, "\\usepackage{tikz-network}\n");
    fprintf(f, "\\usepackage{xcolor}\n\n");

    //Document information
    fprintf(f, "\\begin{document}\n");

    fprintf(f, "\\title{Graph Theory}\n");
    fprintf(f, "\\author{Melissa Carvajal, Carmen Hidalgo \\& Josu\\'e Soto}\n");
    //fprintf(f, "\\institute{Investigaci\\'on de Operaciones}\n");
    fprintf(f, "\\date{2025}\n\n");

    //begin
    fprintf(f, "\\definecolor{KirbyPink}{HTML}{D74894}\n");
    fprintf(f, "\\definecolor{LightPink}{HTML}{FFBFBF}\n\n");
    fprintf(f, "\\maketitle\n\n");
    fprintf(f, "\\newpage\n\n\n");
}


void introduction(FILE* f){

    fprintf(f, "\\section{Floyd's Algorithm}\n");
    fprintf(f, "This program consists of Floyd's algorithm to obtain the shortest path between any pair of nodes in a graph with weighted distances.\n");

    fprintf(f, "Floyd's algorithm compares the distance between any two given nodes and by passing through another city in between, if the result is less than the original then it chooses the shortest one. After contemplating all nodes in the graph, the graph is guaranteed to have all the shortest distances between any two nodes in the graph. These changes are recorded in another matrix called P that helps determine the shortest path between any two nodes.\n");


    fprintf(f, "\\section{Robert W. Floyd (1936–2001)}\n");

    fprintf(f, "\\begin{figure}\n");
    fprintf(f, "\\begin{center}\n");
    fprintf(f, "\\includegraphics[width=0.25\\textwidth]{floyd.jpg}\n");
    fprintf(f, "\\caption{\\label{fig:floyd}Robert Floyd}\n");
    fprintf(f, "\\end{center}\n");
    fprintf(f, "\\end{figure}\n\n");
    
    fprintf(f, "Robert Willoughby Floyd was a computer scientist that lived from 1936 to 2001. He made great advances in computer science and developed an algorithm to find the shortest paths between any two nodes for a directed graph. He was awarded a Turing Award in 1978.\n\n\n");
}


void texStart(FILE* f, int titleType, int number, char character){
    //Title Type (title)
    //0 - Table (char)(number)

    fprintf(f, "\\section{");

    switch (titleType){
    case 0:
        fprintf(f, "Table $%c_{%d}$}\n", character, number);
        break;

    default:
        fprintf(f, "Floyd}\n");
        break;
    }
}

void texEnd(FILE* f){
    //Code executes here after each section if needed
    // (Not needed anymore)
}


/* ################################## TEX ################################## */

void texTable(int** m, int** changes, int size, int iteration, FILE* f, char c, char** names){

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

            if (m[i][j] == INF){
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
}

void saveToTexFile(int** D, int** P, int** changes, int n, FILE* f, char** names, int iteration){

    texTable(D, changes, n, iteration, f, 'D', names);
    texTable(P, changes, n, iteration, f, 'P', names);

}

int finalGraph(FILE* f, int** D, int** P, int n, char** names) {

    fprintf(f, "\\section{Final Graph}\n");
    fprintf(f, "\\begin{center}\n");
    fprintf(f, "\\begin{tikzpicture}\n");


    if (f == NULL) {
        printf("Error: File null\n");
        return 1;
    }
    for (int i = 0; i < n; i++) {
        // Create all the vertexes
        // \Vertex[label=$v_1$]{A}  i//5
        char* currentCity = names[i];
        fprintf(f, " \\Vertex[x=%d, y=%d, color=LightPink, size=0.5, label=$%s$]{%c}\n", 2*(i%5), 2*(i/5), currentCity, 'A'+i);


    }
    for (int i = 0; i < n; i++) { //    We are on the row/city A+i
        for (int j = 0; j < n; j++) { // Now we visit each associated city to A+i

            if (i==j) continue;

            if (D[i][j] == INF) continue;

            fprintf(f, " \n \\Edge[bend=-30, label=$%d$, Direct](%c)(%c)", D[i][j] ,'A'+ i, 'A'+ j);
            // This line draws the directed arrow, where:
            // %d corresponds to the distance between cities
            // %c corresponds to the cities

        }
    }
    fprintf(f, "\\end{tikzpicture}\n");
    fprintf(f, "\\end{center}\n");

    return 0;
}

int initialGraph(FILE* f, int** D, int n, char** names) {
    fprintf(f, "\\section{Initial Graph}\n");
    fprintf(f, "\\begin{center}\n");
    fprintf(f, "\\begin{tikzpicture}\n");


    if (f == NULL) {
        printf("Error: File null\n");
        return 1;
    }
    for (int i = 0; i < n; i++) {
        // Create all the vertexes
        // \Vertex[label=$v_1$]{A}  i//5
        char* currentCity = names[i];
        fprintf(f, " \\Vertex[x=%d, y=%d, color=LightPink, size=0.5, label=$%s$]{%c}\n", 2*(i%5), 2*(i/5), currentCity, 'A'+i);


    }
    for (int i = 0; i < n; i++) { //    We are on the row/city A+i
        for (int j = 0; j < n; j++) { // Now we visit each associated city to A+i

            if (i==j) continue;

            if (D[i][j] == INF) continue;

            fprintf(f, " \n \\Edge[bend=-30, label=$%d$, Direct](%c)(%c)", D[i][j] ,'A'+ i, 'A'+ j);
            // This line draws the directed arrow, where:
            // %d corresponds to the distance between cities
            // %c corresponds to the cities

        }
    }
    fprintf(f, "\\end{tikzpicture}\n");
    fprintf(f, "\\end{center}\n");

    return 0;
}


/* ################################## FLOYD ################################## */

void Floyd(int** D, int** P, int n, char** names, FILE* f){



    int** changes = malloc(sizeof(int*) * n); //Changes made to D/P

    for (int i = 0; i < n; ++i){
        changes[i] = malloc(sizeof(int) * n);
        for (int j = 0; j < n; ++j){
            P[i][j] = 0; // 100 state (No change done)
            changes[i][j] = 0; // 0 state (No change done)
        }
    }

    





    for (int node = 0; node < n; node++){

        saveToTexFile(D, P, changes, n, f, names, node); //Save current state (before changes)

        for (int i = 0; i < n; i++){
            int sum;
            for (int j = 0; j < n; j++){

                changes[i][j] = 0; //No change done

                if(D[i][node] == INF || D[node][j] == INF){
                    sum = INF;
                } else {
                    sum = D[i][node] + D[node][j];
                }
                

                if (i != j && //No node to node
                D[i][j] > sum){  //Direct distance is greater than going through the node

                    D[i][j] =  sum;
                    P[i][j] = node+1;
                    changes[i][j] = 1; //Change registered

                }
            }
        }
    }

    saveToTexFile(D, P, changes, n, f, names, n); //Save final state


}

/* ################################## MAIN ################################## */

int runFloyd(char** names, int** matrix, int nodes) {

    int** P = malloc(sizeof(int*) * nodes);
    for (int i = 0; i < nodes; i++){
        P[i] = malloc(sizeof(int) * nodes);
    }

    //File
    FILE* file;
    file = fopen("programToLaTeX.tex", "w");
    if (file == NULL) {
        printf("Error: File null\n");
        return 1;
    }


    documentStart(file);
    introduction(file);

    initialGraph(file, matrix, nodes, names);

    Floyd(matrix, P, nodes, names, file);

    finalGraph(file, matrix, P, nodes, names);

    fprintf(file, "\\end{document}\n");
    fclose(file);


    //Free memory
    for (int i = 0; i < nodes; i++){
        free(matrix[i]);
        free(P[i]);
    }
    free(matrix);
    free(P);


    int responseCode = system("pdflatex programToLaTeX.tex");
    if (responseCode == 0){
        printf("\n\nLatex compiled without problems\n");
        system("evince --presentation programToLaTeX.pdf &");
    }

    return 0;
}