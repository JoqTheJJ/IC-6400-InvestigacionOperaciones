/*
                Project 3: Equipment replacement
                Made by: Carmen Hidalgo Paz, Melissa Carvajal y Josué Soto
                Date: Friday, September 26, 2025
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include <gtk/gtk.h>
#include <cairo.h>
#include <ctype.h>





int ipow(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

typedef struct {
    float* G;
    int* GPos;
} Solution;

/* ################################## REPLACEMENT ################################## */

float* inflationCosts(float cost, float inflationPercentage, int years){
    float inf = inflationPercentage + 1; //5% -> 105%

    float* inflation = malloc(sizeof(float) * (years+1));

    float currentCost = cost;
    inflation[0] = 0;
    for (int i = 1; i < years+1; i++){
        currentCost *= inf;
        inflation[i] = currentCost - cost;
    }

    return inflation;
}


float** calculateC(int years, int lifespan, float buyPrice, float* sellPrice, float* maintenance, float* inflation, float earnings){

    float** C = malloc(sizeof(float*) * (years+1));


    for (int i = 0; i < years+1; i++){
        C[i] = malloc(sizeof(float) * (years+1));
        for (int j = 0; j < years+1; j++){
            C[i][j] = INT_MAX; // Infinite invalid cost
        }
    }




    for (int dif = 1; dif <= lifespan; ++dif){
        for (int t = 0; t+dif <= years; ++t){
            C[t][t+dif] = buyPrice - sellPrice[dif-1] + maintenance[dif-1] + inflation[t] + earnings;
            printf("C[%d][%d]= %.2f = buy:%2.f - sell:%.2f + m:%.2f + inf:%.2f + ear:%.2f\n", t, t+dif, C[t][t+dif], buyPrice, sellPrice[dif-1], maintenance[dif-1], inflation[t], earnings);
        }
    }

    return C;
}


Solution replacement(FILE* f, float** C, int years, int lifespan){
    

    float* G    = malloc(sizeof(float) * (years+1));
    int* GPos   = malloc(sizeof(int) * (years+1));   //Stores the binary code of the winners

    //Base case
    G[years]    = 0;
    GPos[years] = 0;

    fprintf(f, "\\section{Solution}\n");
    fprintf(f, "In the following text, you will find the needed operations to determinate the optimal solution for each year.\n\n");
    for (int g = years-1; g >= 0; --g){

        G[g] = FLT_MAX;
        GPos[g] = 0;

        float option;
        printf("[ [ [ g=%d ] ] ]\n", g);

        //Print
        // minimo{
        fprintf(f, "G(%d) = min(\n", g);
        for (int dif = 1; g+dif <= years && dif <= lifespan; dif++){
            //Marcar Proceso
            // Imprimir Calculo
            // G(4) = C[i][j] + G(j)
            // G(4) = %d + %d

            option = C[g][g+dif] + G[g+dif];
            printf("option: %.2f = C[%d][%d] + G[%d] = %.2f + %.2f\n", option, g, g+dif, g+dif, C[g][g+dif], G[g+dif]);
            fprintf(f, "C[%d][%d] + G[%d] = %.2f + %.2f = %.2f\n", g, g+dif, g+dif, C[g][g+dif], G[g+dif], option);
            
            if (option < G[g]){ //Better option
                G[g] = option;
                GPos[g] = ipow(2, dif); //Sets the digit of the candidate option

            } else if (option == G[g]) { //Tie option
                GPos[g] += ipow(2, dif); //Adds the digit of the next candidate option
            }
        }
        // }
        fprintf(f, " ) \n");

        //Marcar G calculado
        // G(algo) = x
        // Ganadores son: 1, 2, 3
    }

    free(C);

    Solution sol;
    sol.G = G;
    sol.GPos = GPos;

    return sol;
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
        "    {\\Large Replacement Problem\\par}\n"
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
        "    {\\large 26 september 2025\\par}\n"
        "\\end{titlepage}\n"
    );
}

void documentStart(FILE* f){
    fprintf(f, "\\documentclass{article}\n\n");

    //Packages
    fprintf(f, "\\PassOptionsToPackage{table,svgnames}{xcolor}");
    fprintf(f, "\\usepackage{graphicx}\n");
    fprintf(f, "\\usepackage{pdflscape}\n");
    fprintf(f, "\\usepackage{adjustbox}\n");
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

void introduction(FILE* f) {

    fprintf(f, "\\section{Replacement Problem}\n");
    fprintf(f, "The \\textit{Replacement Problem} involves determining the optimal time to replace a tool or piece of equipment that deteriorates with use. ");
    fprintf(f, "As equipment ages, its efficiency decreases and operating and maintenance costs rise, while replacing it incurs an immediate acquisition cost. ");
    fprintf(f, "The objective is to minimize the total accumulated cost over a given planning horizon by balancing these two factors.\n\n");

    fprintf(f, "Several variations of the problem may include additional real-world considerations, such as:\n");
    fprintf(f, "\\begin{itemize}\n");
    fprintf(f, "  \\item Expected annual profits\n");
    fprintf(f, "  \\item Inflation rate\n");
    fprintf(f, "  \\item Emergence of more modern or efficient equipment\n");
    fprintf(f, "\\end{itemize}\n\n");

    fprintf(f, "\\subsection{Solution Approach}\n");
    fprintf(f, "To solve this problem, a dynamic programming approach based on Bellman's equation is used. ");
    fprintf(f, "The Bellman equation is defined as:\n");
    fprintf(f, "\\begin{center}\n");
    fprintf(f, "$G(t) = \\min(C(t, x) + G(x))$\n");
    fprintf(f, "\\end{center}\n");
    fprintf(f, "Where:\n");
    fprintf(f, "\\begin{itemize}\n");
    fprintf(f, "  \\item $G(t)$ is the minimum total cost from year $t$ to the end of the time horizon.\n");
    fprintf(f, "  \\item $C(t, x)$ represents the cost of operating the equipment from year $t$ to $x$, considering maintenance, replacement, and inflation.\n");
    fprintf(f, "  \\item $x$ is the next possible year for replacement.\n");
    fprintf(f, "\\end{itemize}\n");

    fprintf(f, "The algorithm works backwards from the final year, evaluating two options at each step: keeping the current equipment or replacing it. ");
    fprintf(f, "The option with the lower cost is selected to build the optimal policy.\n\n");

    fprintf(f, "\\subsection{Data Structures}\n");
    fprintf(f, "Three main tables are used in the implementation:\n");
    fprintf(f, "\\begin{itemize}\n");
    fprintf(f, "  \\item \\textbf{C:} Stores individual operating costs between any two years, accounting for inflation and other variables.\n");
    fprintf(f, "  \\item \\textbf{G:} Contains the minimum accumulated cost from each year onward, computed using dynamic programming.\n");
    fprintf(f, "  \\item \\textbf{GPOS:} Stores the optimal replacement policy, indicating in which years replacements should occur.\n");
    fprintf(f, "\\end{itemize}\n\n");

}


void problem(FILE* f,
    int years,
    int lifespan,
    float buyPrice,
    float* sellPrice,
    float* timeMaintenance,
    float* maintenance,
    float* inflation,
    float inflationRate,
    float earnings,
    float** C){

    fprintf(f, "\\section{Problem}\n\n");
    
    fprintf(f, "Time of the duration of the project: %d\n", years);
    fprintf(f, "Lifespan of the equipment: %d\n\n", lifespan);

    fprintf(f, "Price of new equipment: %.2f\n", buyPrice);
    fprintf(f, "Earnings: %.2f\n", earnings);
    fprintf(f, "Inflation rate: %.2f\n\n", inflationRate);



    //tabular
    fprintf(f, "\\begin{center}\n");
    fprintf(f, "\\begin{adjustbox}{max width=\\textwidth}\n");
    fprintf(f, "    \\begin{tabular}{|c||");
    for (int col = 0; col < 4; ++col){
        fprintf(f, "c|");
    }
    fprintf(f, "}\n");

    //first row
    fprintf(f, "        \\hline\n");
    fprintf(f, "        \\cellcolor[HTML]{FFBFBF}\\textbf{Time passed} ");
    fprintf(f, "& \\cellcolor[HTML]{FFBFBF}\\textbf{Maintenance} ");
    fprintf(f, "& \\cellcolor[HTML]{FFBFBF}\\textbf{Maintenance (accumulative)} ");
    fprintf(f, "& \\cellcolor[HTML]{FFBFBF}\\textbf{Selling price} ");
    fprintf(f, "& \\cellcolor[HTML]{FFBFBF}\\textbf{Additional cost for inflation} ");

    

    fprintf(f, "\\\\\n        \\hline\n");
    fprintf(f, "        \\hline\n");


    //table
    for (int i = 0; i < years+1; ++i){ 
        fprintf(f, "        \\cellcolor[HTML]{FFBFBF}\\textbf{i=%d}", i+1);
        
        fprintf(f, "& %.2f", sellPrice[i]);
        fprintf(f, "& %.2f", timeMaintenance[i]);
        fprintf(f, "& %.2f", maintenance[i]);
        fprintf(f, "& %.2f", inflation[i]);

        fprintf(f, "\\\\\n        \\hline\n");
    }

    fprintf(f, "    \\end{tabular}\n");
    fprintf(f, "\\end{adjustbox}\n\n\n");
    fprintf(f, "\\end{center}\n\n\n");







    fprintf(f, "\\subsection{Table of Costs C_{\\text{ij}}}\n");
    fprintf(f, "The table represents with a number the cost from buying a new bicicle on the year i and selling it on the year j where the maintenance costs are already included and the \\\"year 0\\\" marks the start of the project. The table has - where a value is invalid either due to the lifespan of the equipment or the dration of the project.\n");

    //tabular
    fprintf(f, "\\begin{center}\n");
    fprintf(f, "\\begin{adjustbox}{max width=\\textwidth}\n");
    fprintf(f, "    \\begin{tabular}{|c||");
    for (int col = 0; col < years+1; ++col){
        fprintf(f, "c|");
    }
    fprintf(f, "}\n");

    //first row
    fprintf(f, "        \\hline\n");
    fprintf(f, "        \\cellcolor[HTML]{FFBFBF}\\textbf{C} ");
    for (int col = 0; col < years+1; ++col){
        fprintf(f, "& \\cellcolor[HTML]{FFBFBF}\\textbf{j=%d} ", col);
    }
    fprintf(f, "\\\\\n        \\hline\n");
    fprintf(f, "        \\hline\n");


    //table
    for (int i = 0; i < years+1; ++i){ 
        fprintf(f, "        \\cellcolor[HTML]{FFBFBF}\\textbf{i=%d}", i);
        for (int j = 1; j < years+1; ++j){ // start in 1 to skip 0s column

            if (C[i][j] >= 2147483648){
                fprintf(f, "& $-$ ");
            } else {
                fprintf(f, "& $%.2f$ \\$", C[i][j]);
            }
        }
        fprintf(f, "\\\\\n        \\hline\n");
    }

    fprintf(f, "    \\end{tabular}\n");
    fprintf(f, "\\end{adjustbox}\n\n\n");
    fprintf(f, "\\end{center}\n\n\n");
    fprintf(f, "\n");
}

/* ################################## TEX ################################## */



/* ################################## MAIN ################################## */

void runReplacement(int years, int lifespan, float buyPrice, float* sellPrice, float* timeMaintenance, float inflation, float earnings){

    
    FILE* f = fopen("programToLaTeX.tex", "w");

    if (f == NULL) {
        printf("Error: File null\n");
        return;
    }

    printf("Maintenance");
    float* maintenance = malloc(sizeof(float)*lifespan);
    for (int x = 0; x < lifespan; ++x){ //Accumulative maintenance costs
        maintenance[x] = 0;
        for (int i = 0; i < x+1; ++x){
            maintenance[x] += timeMaintenance[i];
        }
    }

    float* inflationValues = inflationCosts(buyPrice, inflation, years);
    float** C = calculateC(years, lifespan, buyPrice, sellPrice, maintenance, inflationValues, earnings);

    printf("Problem");
    problem(f, years, lifespan, buyPrice, sellPrice, timeMaintenance, maintenance, inflationValues, inflation, earnings, C);

    

    printf("Replacement");
    Solution solution = replacement(f, C, years, lifespan);
    float* G = solution.G;
    int* GPos = solution.GPos;

    
    fprintf(f, "\\subsection{Results}\n\n");
    
    for (int g = 0; g < years+1; ++g){
        int solution = GPos[g];
        printf("G(%d) = %f (%d)\n", g, G[g], GPos[g]);

        fprintf(f, "G(%d) = %f \n",  g, G[g]);
        fprintf(f, "Winners: ");
        
        if(GPos[g] % 2)
            fprintf(f, "%d ", g);
        solution /= 2;
        
    }

    
    
    
    //fprintf(f, "\\section{Graph}\n");
    //fprintf(f, "\\begin{tikzpicture}\n");
    // for (int g = 0; g <= years; ++g){
    //     fprintf(f, " \\Vertex[x=%d, size=0.5, label={%d}]{%c}\n", g,  g, 'A' + g);
    // }
    // rana(solution, years, 0);
    // fprintf(f, "\\end{tikzpicture}\n");
    
    printf("\n\n");
    fclose(f);
}


//void rana(Solution sol, int years, int winner){
//     //fprintf(f, " \\Vertex[x=%d, y=%d, color=LightBlue, size=0.5, label={%s}]{%c}\n", 2*(k+1), 0, names[v], 'A' + v);
//     //fprintf(f, " \\Edge[label=$%d$, Direct](%c)(%c)\n", D[prev][v], 'A' + prev, 'A' + v);
    
//     for(int y = 0; y <= years; ++y){
//         winner = sol.GPos[y];
//         if (winner % 2){
//             fprintf(f, " \\Edge[Direct](%c)(%c)\n", 'A' + y, 'A' + (winner+y));
//             rana(sol, years);
//         }
//         winner /= 2;

//     }    
// }




void main(){
    int lifespan = 3;   //Lifespan
    float buyPrice = 500; //Buying price
    int years = 5;        //Years for the project
    float inflationPercentage = 0.05; //Inflation percentage
    float earnings = 0;


    float* sellPrice = malloc(sizeof(float)*lifespan);   //Selling price on the n year of use
    float* timeMaintenance = malloc(sizeof(float)*lifespan);
    
    sellPrice[0] = 400;
    sellPrice[1] = 300;
    sellPrice[2] = 250;

    timeMaintenance[0] = 30;
    timeMaintenance[1] = 40;
    timeMaintenance[2] = 60;


    runReplacement(years, lifespan, buyPrice, sellPrice, timeMaintenance, inflationPercentage, earnings);
}