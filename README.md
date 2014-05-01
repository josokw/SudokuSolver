Sudoku solver
=============

Console application implemented in C++. The initial code was developed in C++98.
It is now refactored to C++11.


Sudoku grid (order 3) contains 9 x 9 cells.
Cell groups: rows (9 x 9), columns (9 x 9) and blocks (9 x 3x3). 
These groups are implemented by STL template sets.
Goal: solve Sudoku puzzles using set operations.

Solving strategies:
- Naked singles
- Hidden singles
- Brute force recursive

Input format
------------
Easy Sudoku:

    0 0 0  7 0 3  0 0 6

    8 0 4  0 5 0  0 2 0

    6 0 2  0 0 4  7 0 9


    9 0 0  4 0 0  3 0 7

    0 6 0  0 0 0  8 0 0

    3 0 8  2 0 7  6 0 5


    0 0 0  1 0 0  0 0 8

    2 0 1  0 0 5  0 6 0

    0 4 0  9 0 2  0 0 1

Building
--------
This program does not need any external libs. 
The makefile is in the src directory.

    cd src

Build it
  
    make

Executing
---------

Some puzzles are available in the puzzles directory.
Go to the src directory for starting the program:

    ./sudoku ../puzzles/Data01.sdk

