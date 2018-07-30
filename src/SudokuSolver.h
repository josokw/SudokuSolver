#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include "SudokuGrid.h"
#include <vector>

class SudokuSolver
{
public:
   SudokuSolver(const SudokuGrid &initialSdkg, int nMaxSolutions = 1);
   virtual ~SudokuSolver() = default;
   const std::vector<SudokuGrid> &solve();

private:
   SudokuGrid sdkg_;
   int nMaxSolutions_;
   std::vector<SudokuGrid> solutions_;
   int numberOfNotSolvables_;
   int maxRecursionDepth_;
   int numberOfNakedSingles_;
   int numberOfHiddenSingles_;
   int numberOfNakedPairs_;

   bool solveNakedSingles(SudokuGrid &sdkg);
   bool solveHiddenSingle(SudokuGrid &sdkg);
   bool solveNakedPair(SudokuGrid &sdkg);
   bool solveNakedPair(SudokuGrid::group_t &group);
   /// Brute force solving
   void solveByRecursion(SudokuGrid &sdkg, int level = 0);
};

#endif
