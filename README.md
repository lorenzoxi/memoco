# Methods and Models for Combinatorial Optimization (a.a. 2023-2024)

## Exercise

Solving with a CPLEX API based solver (part I) and with an alternative method (parte II) the following TSP problem:

>A company produces boards with holes used to build electric panels. Boards are positioned over a machine and a drill moves over the board, stops at the desired positions and makes the holes. Once a board is drilled, a new board is positioned and the process is iterated many times. Given the position of the holes on the board, the company asks us to determine the hole sequence that minimizes the total drilling time, taking into account that the time needed for making an hole is the same and constant for
all the holes.

## Part 1:. Implementing an Integer Linear Programming model with the Cplex API

### Running instruction for CPLEX solver

1. ```$ cd project01/```  
2. ```$ cmake .```  
3. ```$ cmake --build .```  
4. ```$ ./project01```

## Part 2: Implementing an alternative solution method: a genetic algorithm

### Running instruction for G.A. solver

1. ```$ cd project02/```  
2. ```$ cmake .```  
3. ```$ cmake --build .```  
4. ```$ ./project01```

**Note**: programs executed and tested with:

- ```g++ (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0```
- ```cmake version 3.10.2```
  