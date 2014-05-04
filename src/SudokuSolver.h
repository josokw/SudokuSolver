//--------------------------------------------------------------- SudokuSolver.h

#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include "SudokuGrid.h"
#include <vector>

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
  int _numberOfNakedPairs;

  bool solveNakedSingles(SudokuGrid& sdkg);
  bool solveHiddenSingle(SudokuGrid& sdkg);
  bool solveNakedPair(SudokuGrid& sdkg);
  /// Brute force solving
  void solveByRecursion(SudokuGrid& sdkg, int level = 0);
};

#endif

//----------------------------------------------------------- eof SudokuSolver.h
