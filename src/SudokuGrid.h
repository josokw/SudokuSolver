#ifndef SUDOKUGRID_H
#define SUDOKUGRID_H

#include <array>
#include <iostream>
#include <set>

/// A Sudoku grid of order N contains N x N x N cells:
/// N x N rows, N x N columns and N blocks.
///
/// Sudoku grid of order 3 terms:
/// Band: Three boxes connected horizontally to make a 9 by 3 rectangle.
/// Block: One of the 3 by 3 squares of cells in the grid when it is
///        divided into nine equal squares.
/// Cell: A single square in the puzzle where one digit can be placed.
/// Column: 9 cells of the puzzle running in a straight line from top to bottom
/// Grid: The full 9 by 9 set of cells
/// Group: A block, row or column.
/// Pair: Two cells in the same group with only the same two candidates.
/// Rectangle: This can refer to any rectangular set of cells of any size.
/// Row: 9 cells of the puzzle running in a straight line from left to right.
/// Square: This can refer to any square set of cells of any size.
/// Stack: Three blocks stacked vertically to make a 3 by 9 rectangle.
///
/// Well formed Sudoku puzzles have only 1 solution.
///

class SudokuGrid
{
   friend class SudokuSolver;
   friend bool operator==(const SudokuGrid &lhs, const SudokuGrid &rhs);
   friend std::ostream &operator<<(std::ostream &os, const SudokuGrid &sdkg);
   friend std::istream &operator>>(std::istream &is, SudokuGrid &sdkg);
   friend void writeCandidates(std::ostream &os, const SudokuGrid &sdkg);
   friend void writeLatex(std::ostream &os, const SudokuGrid &sdkg);

public:
   using value_t = int;
   using set_t = std::set<value_t>;

   static const int ORDER = 3;
   static const int ORDER2 = (ORDER * ORDER);
   static const int ORDER4 = (ORDER2 * ORDER2);

   using candidates_t = std::array<std::array<set_t, ORDER2>, ORDER2>;
   using group_t = std::array<std::array<set_t *, ORDER2>, ORDER2>;

   SudokuGrid();
   SudokuGrid(const SudokuGrid &sdkg);
   SudokuGrid &operator=(const SudokuGrid &sdkg);
   void setID(const std::string &id) { id_ = id; }
   const std::string &getID() const { return id_; }
   /// Checks if a Sudoku puzzle is solved.
   /// All cells should be solved to fulfill this condition.
   bool isSolved() const { return numberOfCellsSolved_ == SudokuGrid::ORDER4; }
   /// Checks if a cell is already solved.
   bool cellIsSolved(int row, int column) const
   {
      return cell_[row][column] != 0;
   }
   /// Returns the number of solved cells
   int getNumberOfCellsSolved() const { return numberOfCellsSolved_; }
   /// Returns solvability status.
   bool isSolvable() const { return isSolvable_; }

private:
   /// Set EMPTY: empty set.
   static const set_t EMPTY;
   /// Set U: Universe, this set contains all allowed cell values.
   static const set_t U;
   std::string id_;
   int numberOfCellsSolved_;
   bool isSolvable_;
   /// Contains for every cell the solution, if the contained value equals 0
   /// then a solution still needs to be calculated.
   std::array<std::array<value_t, ORDER2>, ORDER2> cell_;
   /// All group sets containing all candidates.
   std::array<set_t, ORDER2> columnSet_;
   std::array<set_t, ORDER2> rowSet_;
   std::array<std::array<set_t, ORDER>, ORDER> blockSet_;
   /// Contains for every cell not yet solved, the set of candidate solutions.
   candidates_t candidates_;
   /// Group arrays of pointers to #candidates_
   group_t pRow;
   group_t pColumn;
   group_t pBlock;

   bool removeCandidates(const set_t &rem, int row, int column);
   /// Adds an allowed solution for a certain cell to Cell.
   /// Increments numberOfCellsSolved_.
   void add(const value_t value, int row, int column);
   /// Adds a solution for a certain cell to Cell.
   void unsafeAdd(const value_t value, int row, int column);
   /// Calculates all candidate solutions for every cell not yet solved.
   void calculateAllCellCandidates();
   /// Gets all candidates for a certain cell.
   const set_t &getCellCandidates(int row, int column) const
   {
      return candidates_[row][column];
   }
   /// Populate arrays of pointers
   void mapPointerArraysToCandidates();
   /// Calculates all candidates for a certain cell.
   /// The sets #columnSet_, #rowSet_ and #blockSet_ must be up to date.
   /// Sets #isSolvable_ status.
   SudokuGrid::set_t calculateCellCandidates(int row, int column) const;
};

#endif
