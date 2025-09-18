# Knapsack Problem Project
### Members of the team
Carmen Hidalgo Paz
Josué Soto González
Melissa Carvajal Charpentier

## Description

The *Knapsack problem* is a classic optimization problem. The goal is to fill a backpack optimally with a set of objects, each with a weight and a profit, in order to maximize the total profit without exceeding the backpack's capacity.

There are a few main types of knapsack problems:

- **0/1 Knapsack:** Each object can be taken or not, only one copy per object.
- **Bounded Knapsack:** Each object has a limited number of copies.
- **Unbounded Knapsack:** Each object can be taken any number of times, as long as the total weight allows.

### Solution

A common way to solve these problems is using dynamic programming. We build a table to keep track of the optimal profit for different capacities and numbers of objects. By filling this table, we can find the maximum profit achievable for the given backpack capacity.

The resulting table will show the exact quantity and which objects to take in order to achieve the optimal weight.

## How to Run

To compile and run the project, use the following commands in your terminal:

```bash
make clean
make
./proyecto_2
