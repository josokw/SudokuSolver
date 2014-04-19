Sudoku solver
=============

Console application implemented in C++. The initial code was developed in C++98.

Sudoku grid (order 3) contains 9 x 9 cells.
Cell groups: rows (9 x 9), columns (9 x 9) and blocks (9 x 3x3). 
These groups are implemented by STL sets.
Goal: solve Sudoku puzzles using set operations (STL set templates).

Solving strategies:
- Naked singles
- Hidden singles (buggy)
- Brute force recursive

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


Under construction.
