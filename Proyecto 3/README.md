# Equipment Replacement Problem Project
### Members of the team
Carmen Hidalgo Paz.

Josué Soto González.

Melissa Carvajal Charpentier

## Description

This program consists of solving The Tool Replacement for Equipment Replacement Problem with dynamic programming by defining a table where the entries are the cost of maintenance and the cost of selling the tool in a given time.
DIRECTIONS:
- Enter a positive number for the Maintenance and Resell Value columns.
- If the cell becomes red, you've entered the wrong value.

### Solution

A common way to solve these problems is using dynamic programming. We build a table to keep track of the cost for each iteration of buying and selling the equipment. Then it is iterated through from the end of the project to the start by picking the options with the least cost that satisfy the restrictions, eventually reaching the optimal cost and how to achieve it.

The resulting document will indicate when to buy and sell the equipment in order to achieve the optimal cost for the duration of the whole project as well as generally describe the problem.

## How to Run

To compile and run the project, use the following commands in your terminal:

```bash
make clean
make
./proyecto_3
