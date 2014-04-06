//----------------------------------------------------------------- SudokuGrid.h

#ifndef SUDOKUGRID_H
#define SUDOKUGRID_H

#include <iostream>
#include <set>

/// A Sudoku grid of order N contains N x N x N cells:
/// N x N rows, N x N columns and N blocks.
///
/// Well formed Sudoku puzzles have only 1 solution.
//
/// Sudoku Grid (of order 3) terms:
/// Band (floor): Three boxes connected horizontally to make a 9 by 3 rectangle.
/// Box (block): One of the 3 by 3 squares of cells in the grid when it is
///              divided into nine equal squares.
/// Cell (square): A single square in the puzzle where one digit can be placed.
/// Chute: A band or a stack.
/// Column: 9 cells of the puzzle running in a straight line from top to bottom
/// Grid: The full 9 by 9 set of cells
/// Group (house, unit, scope): A box, row or column
/// Line: A box or column.
/// Pair: Two cells in the same group with only the same two candidates.
/// Rectangle: This can refer to any rectangular set of cells of any size.
/// Row: 9 cells of the puzzle running in a straight line from left to right
/// Square: This can refer to any square set of cells of any size.
/// Stack (tower): Three boxes stacked vertically to make a 3 by 9 rectangle.
///
class SudokuGrid
{
   friend bool operator==(const SudokuGrid& lhs, const SudokuGrid& rhs);
   friend std::ostream& operator<<(std::ostream& os, const SudokuGrid& sdkg);
   friend std::istream& operator>>(std::istream& is, SudokuGrid& sdkg);
   friend void writeLatex(std::ostream& os, const SudokuGrid& sdkg);

public:
   typedef unsigned short value_t;
   typedef std::set<value_t> set_t;

   static const int ORDER = 3;
   static const int ORDER2 = ORDER * ORDER;
   static const int ORDER4 = ORDER2 * ORDER2;

   SudokuGrid();
   SudokuGrid(const SudokuGrid& sdkg);
   SudokuGrid& operator=(const SudokuGrid& sdkg);

   /// Adds an allowed solution for a certain cell to Cell.
   /// Increments _numberOfCellsSolved.
   bool add(unsigned short Value, int Row, int Column);
   /// Adds a solution for a certain cell to Cell.
   void unsafeAdd(unsigned short Value, int Row, int Column);
   /// Calculates all candidate solutions for every cell not yet solved.
   void calculateAllCellCandidates();
   /// Gets all candidates for a certain cell.
   const std::set<unsigned short>& getCellCandidates(int row, int column) const
   { return _candidates[row][column]; }
   /// Checks if a Sudoku puzzle is solved.
   /// All cells should be solved to fulfill this condition.
   bool isSolved() const
   { return _numberOfCellsSolved == SudokuGrid::ORDER4; }
   /// Checks if a cell is already solved.
   bool cellIsSolved(int row, int column) const
   { return _cell[row][column] != 0; }
   /// Returns the number of solved cells
   int getNumberOfCellsSolved() const
   { return _numberOfCellsSolved; }
   /// Returns solvability status.
   bool isSolvable() const
   { return _isSolvable; }

private:
   /// Set EMPTY, empty set
   static const set_t EMPTY;
   /// Set U: Universe, this set contains all allowed cell values (solutions).
   static const set_t U;
   /// Number of cells solved
   int _numberOfCellsSolved;
   bool _isSolvable;
   /// Contains for every cell the solution, if the contained value equals 0
   /// then a solution still needs to be calculated.
   unsigned short _cell[ORDER2][ORDER2];
   /// All sets containing candidates.
   set_t _columnSet[ORDER2];
   set_t _rowSet[ORDER2];
   set_t _blockSet[ORDER][ORDER];
   /// Contains for every cell not yet solved, the set of candidate solutions.
   set_t _candidates[ORDER2][ORDER2];

public:
   /// Group arrays of pointers to Candidates
   set_t* pRow[ORDER2][ORDER2];
   set_t* pColumn[ORDER2][ORDER2];
   set_t* pBlock[ORDER2][ORDER2];

private:
   /// Populate arrays of pointers
   void mapPointerArraysToCandidates();
   /// Calculates all candidates for a certain cell.
   /// The sets U, ColumnSet, RowSet and BlockSet must be up to date.
   /// Sets IsSolveable status.
   set_t calculateCellCandidates(int row, int column) const;

   /// Generates a set containing [b..e)
   static set_t makeRange(int b, int e)
   {
      set_t result;
      for (int i = b; i < e; ++i)
      {
         result.insert(i);
      }
      return result;
   }
};

#endif

//------------------------------------------------------------- eof SudokuGrid.h
