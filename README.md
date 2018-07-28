# Sudoku solver

Console application implemented in C++. The initial code was developed in
C++98. It is now refactored to C++14.

A Sudoku grid (order 3) contains 9 x 9 cells.
Cell groups: rows (9 x 9), columns (9 x 9) and blocks (9 x 3x3).
A Sudoku puzzle contains a partially completed row-column grid of
prescribed set of 9 distinct symbols.  
Each cell group should be filled by exactly one of each element of the
prescibed symbols.
Every Sudoku puzzle should have only one unique solution
(proper Sudoku puzzle).

These Sudoku cell groups are implemented by using the C++ STL std::sets.
For solving we use set operations in different solving strategies.

It is proved (2012) that the minimum number of clues in any (9 x 9) proper
Sudoku puzzle is 17.

Implemented solving strategies:

- Naked singles
- Hidden singles
- Naked pairs
- Brute force recursive backtracking

## Input formats

The Sudoku solver can read several input text formats.

Example 1:

    0 0 0  7 0 3  0 0 6

    8 0 4  0 5 0  0 2 0

    6 0 2  0 0 4  7 0 9


    9 0 0  4 0 0  3 0 7

    0 6 0  0 0 0  8 0 0

    3 0 8  2 0 7  6 0 5


    0 0 0  1 0 0  0 0 8

    2 0 1  0 0 5  0 6 0

    0 4 0  9 0 2  0 0 1

The application can also read basic [SadMan Software Sudoku](http://www.sadmansoftware.com/sudoku/solvingtechniques.php)
*.sdk* files.

Example 2:

    ..5..3.8.
    ..789....
    1........
    9.1.8.25.
    .7..2..3.
    .54.3.8.7
    ........1
    ....584..
    .1.4..3..

The application can also read "one liners", for example evil Sudoku puzzles
*17 givens* [Gordon Royle, The University of Western Australia](http://staffhome.ecm.uwa.edu.au/~00013890/sudoku17-ml).

Example 3:

    100005090000010004000000000000006150004900000020000000080470000500000800000200000

## Building

This program does not need any external libs.
The makefile is in the *src* directory.
For building, type:
  
    make

## Executing

Some puzzles are available in the *puzzles* directory.
Solve one puzzle:

    ./sudoku ../puzzles/Data01.sdk

Solve a number of puzzles:

    ./sudoku ../puzzles/Data01.sdk ../puzzles/Data02.sdk

Example:

    - sudoku v1.0.5 started

    ---- Sudoku file: ../puzzles/g17-0001.sdk

    - Number of clues: 17

     .  .  . | .  .  2 | 7  5  .
     .  1  8 | .  9  . | .  .  .
     .  .  . | .  .  . | .  .  .
    -----------------------------
     4  9  . | .  .  . | .  .  .
     .  3  . | .  .  . | .  .  8
     .  .  . | 7  .  . | 2  .  .
    -----------------------------
     .  .  . | .  3  . | .  .  9
     7  .  . | .  .  . | .  .  .
     5  .  . | .  .  . | .  8  .


    ------------------------- Solution index:  1
    ---- Number of encountered not solvables:  90
    ---- Maximum recursion depth:              18
    ---- Recursion profile:                    1.3.1.2.1.2.2.3.1.1.2.2.1
    ---- Number of encountered naked singles:  524
    ---- Number of encountered hidden singles: 20
    ---- Number of encountered naked pairs:    144

    ---- Processing time: 0.4666 sec

    ---- Sudoku solution(s), 1 found:

     9  4  6 | 1  8  2 | 7  5  3
     3  1  8 | 5  9  7 | 4  2  6
     2  7  5 | 6  4  3 | 8  9  1
    -----------------------------
     4  9  2 | 3  1  8 | 5  6  7
     6  3  7 | 2  5  4 | 9  1  8
     8  5  1 | 7  6  9 | 2  3  4
    -----------------------------
     1  2  4 | 8  3  5 | 6  7  9
     7  8  3 | 9  2  6 | 1  4  5
     5  6  9 | 4  7  1 | 3  8  2


    - Sudoku solver, solved puzzle: ../puzzles/g17-0001.sdk
