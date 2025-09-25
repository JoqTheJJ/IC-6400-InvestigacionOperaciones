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
#include <locale.h>

#include <ctype.h>

#define INF __INT_MAX__
#define PI 3.14159265358979323846



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
        "    {\\Large Optimal Routes\\par}\n"
        "    {\\large Floyd's Algorythm\\par}\n"
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
        "    {\\large 12 september 2025\\par}\n"
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

    fprintf(f, "\\section{Floyd's Algorithm}\n");
    fprintf(f, "This program consists of Floyd's algorithm to obtain the shortest path between any pair of nodes in a graph with weighted distances.\n");

    fprintf(f, "Floyd's algorithm compares the distance between any two given nodes and by passing through another city in between, if the result is less than the original then it chooses the shortest one. After contemplating all nodes in the graph, the graph is guaranteed to have all the shortest distances between any two nodes in the graph. These changes are recorded in another matrix called P that helps determine the shortest path between any two nodes.\n");


    fprintf(f, "\\section{Robert W. Floyd (1936–2001)}\n");

    //fprintf(f, "\\begin{figure}\n");
    fprintf(f, "\\begin{center}\n");
    fprintf(f, "\\includegraphics[width=0.25\\textwidth]{floyd.jpg}\n");
    //fprintf(f, "\\caption{\\label{fig:floyd}Robert Floyd}\n");
    fprintf(f, "\\end{center}\n");
    //fprintf(f, "\\end{figure}\n\n");
    
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

/* ################################## FINAL GRAPH ################################## */

int findVertexes(int** P, int* vertexes, int current, int other, int next) {
    if (P[current][other] == 0) return next;

    int midCity = P[current][other]-1;

    next = findVertexes(P, vertexes, current, midCity, next);
    vertexes[next] = midCity;
    return findVertexes(P, vertexes, midCity, other, next+1);
}


int eachCity(FILE* f, int** D, int** P, int n, char** names) {
    if (f == NULL) {
        printf("Error: File null\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        char* currentCity = names[i];
        fprintf(f, "\\section*{Current city: %s}\n", currentCity);

        //printf("i: %d\n", i);

        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            if (D[i][j] == INF) continue;

            //printf("j: %d\n", j);

            int *vertexes = malloc(n * sizeof(int));
            int count = findVertexes(P, vertexes, i, j, 0); // This function finds all intermediate nodes,
            // returns total of intermediate node
            //printf("Found cities!\n");

            fprintf(f, "\\begin{center}\n");
            fprintf(f, "\\begin{tikzpicture}\n");

            // Current node
            fprintf(f, " \\Vertex[x=%d, y=%d, color=LightPink, size=0.5, label={%s}]{%c}\n",0, 0, names[i], 'A' + i);

            int prev = i;

           // Draws intermediate nodes
           // Prev is needed in order to know which was the previous node, so we can draw the edge
            for (int k = 0; k < count; k++) {
                //printf("k: %d\n", k);
                int v = vertexes[k];
                fprintf(f, " \\Vertex[x=%d, y=%d, color=LightBlue, size=0.5, label={%s}]{%c}\n", 2*(k+1), 0, names[v], 'A' + v);

                fprintf(f, " \\Edge[label=$%d$, Direct](%c)(%c)\n", D[prev][v], 'A' + prev, 'A' + v);
                prev = v;
            }


            fprintf(f, " \\Vertex[x=%d, y=%d, color=LightPink, size=0.5, label={%s}]{%c}\n", 2*(count+1), 0, names[j], 'A' + j);

            fprintf(f, " \\Edge[label=$%d$, Direct](%c)(%c)\n", D[prev][j], 'A' + prev, 'A' + j);

            fprintf(f, "\\end{tikzpicture}\n");
            fprintf(f, "\\end{center}\n");

            free(vertexes);
        }
        //printf("Final i: %d\n", i);
    }

    return 0;
}

/* ################################## INITIAL GRAPH ################################## */

int initialGraph(FILE* f, int** D, int n, char** names) {
    fprintf(f, "\\section{Initial Graph}\n");
    fprintf(f, "\\begin{center}\n");
    fprintf(f, "\\begin{tikzpicture}\n");


    if (f == NULL) {
        printf("Error: File null\n");
        return 1;
    }

    float angleIncrement = (2*PI/n); //radians
    float angle = 0;
    float xRadius = 5;
    float yRadius = 5;
    for (int i = 0; i < n; i++) {
        // Create all the vertexes
        // \Vertex[label=$v_1$]{A}  i//5
        float x = (xRadius*cos(angle));
        float y = (yRadius*sin(angle));
        angle += angleIncrement;

        char* currentCity = names[i];
        fprintf(f, " \\Vertex[x=%f, y=%f, color=LightPink, size=0.5, label={%s}]{%c}\n", x, y, currentCity, 'A'+i);



    }
    for (int i = 0; i < n; i++) { //    We are on the row/city A+i
        for (int j = 0; j < n; j++) { // Now we visit each associated city to A+i

            if (i==j) continue;

            if (D[i][j] == INF) continue;

            fprintf(f, " \n \\Edge[bend=-10, label=$%d$, Direct](%c)(%c)", D[i][j] ,'A'+ i, 'A'+ j);
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

    // Force '.' as a decimal separator
    setlocale(LC_NUMERIC, "C");

    documentStart(file);
    introduction(file);

    initialGraph(file, matrix, nodes, names);

    Floyd(matrix, P, nodes, names, file);

    eachCity(file, matrix, P, nodes, names);

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