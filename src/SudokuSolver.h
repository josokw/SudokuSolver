//----------------------------------------------------------------------------------- SudokuSolver.h

#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <vector>

#include "SudokuGrid.h"

class SudokuSolver
{
public:
  SudokuSolver(const SudokuGrid& initialSdkg, int nMaxSolutions = 1);
  virtual ~SudokuSolver() = default;

  const std::vector<SudokuGrid>& solve();

private:
  SudokuGrid _sdkg;
  int _nMaxSolutions;
  std::vector<SudokuGrid> _solutions;
  int _numberOfNotSolvables;
  int _maxRecursionDepth;
  int _numberOfNakedSingles;
  int _numberOfHiddenSingles;

  bool solveNakedSingles(SudokuGrid& sdkg);
  void solveHiddenSingles(SudokuGrid& sdkg);
  void solveHiddenSinglesPerGroup(SudokuGrid::set_t* group[]);

  /// Brute force solving
  void solveByRecursion(SudokuGrid& sdkg, int level = 0);
};

#endif

//------------------------------------------------------------------------------- eof SudokuSolver.h

