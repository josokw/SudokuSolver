//--------------------------------------------------------------- SudokuSolver.h

#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include "SudokuGrid.h"

#include <vector>
#include <ctime>

class SudokuSolver
{
public:
  SudokuSolver(const SudokuGrid& initialSdkg, bool wellformed);

  const std::vector<SudokuGrid>& solve();

private:
  SudokuGrid _sdkg;
  bool _wellformed;
  std::vector<SudokuGrid> _solutions;
  int _numberOfNotSolvables;
  int _numberOfNakedSingles;
  int _numberOfHiddenSingles;

  bool solveNakedSingles(SudokuGrid& sdkg);
  bool solveHiddenSingle(SudokuGrid& sdkg);
  /// Brute force solving
  void solveByRecursion(SudokuGrid& sdkg);
};

#endif

//----------------------------------------------------------- eof SudokuSolver.h
