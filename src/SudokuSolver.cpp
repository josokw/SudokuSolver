//------------------------------------------------------------- SudokuSolver.cpp

#include "SetOperations.h"
#include "SudokuGrid.h"
#include "SudokuSolver.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <set>
#include <iomanip>

using namespace std;

SudokuSolver::SudokuSolver(const SudokuGrid& initialSdkg, bool wellformed)
  :  _sdkg(initialSdkg)
  ,  _wellformed(wellformed)
  ,  _solutions()
  ,  _numberOfNotSolvables(0)
  ,  _numberOfNakedSingles(0)
  ,  _numberOfHiddenSingles(0)
{ }

const std::vector<SudokuGrid>& SudokuSolver::solve()
{
  _solutions.clear();
  cout << _sdkg << endl;
  solveByRecursion(_sdkg);

  return _solutions;
}

bool SudokuSolver::solveNakedSingles(SudokuGrid& sdkg)
{
  bool newNakedSingles {false};
  sdkg.calculateAllCellCandidates();
  for (int row = 0;
       (row < SudokuGrid::ORDER2)
       && sdkg.isSolvable()
       && !sdkg.isSolved();
       ++row)
  {
    for (int col = 0;
         (col < SudokuGrid::ORDER2)
         && sdkg.isSolvable()
         && !sdkg.isSolved();
         ++col)
    {
      if (!sdkg.cellIsSolved(row, col))
      {
        // Check single candidate status
        if (sizeOf(sdkg.getCellCandidates(row, col)) == 1)
        {
          newNakedSingles = true;
          ++_numberOfNakedSingles;
          sdkg.unsafeAdd(*begin(sdkg.getCellCandidates(row, col)),
                         row, col);
          sdkg.calculateAllCellCandidates();
          row = 0;
          col = 0;
        }
      }
    }
  }
  //cout << "-- number of cells solved: " << Sdkg.GetNumberOfCellsSolved() << endl;
  if (!sdkg.isSolvable())
  {
    ++_numberOfNotSolvables;
  }
  return newNakedSingles;
}

bool SudokuSolver::solveHiddenSingle(SudokuGrid& sdkg)
{
  //cout << endl << "----- Solve 'hidden singles': \n\n" << sdkg <<  endl;
  sdkg.calculateAllCellCandidates();
  array<size_t, SudokuGrid::ORDER2> countCandidates {{}};
  int stack {0};
  bool newHiddenSingle {false};

  while (!newHiddenSingle && stack < SudokuGrid::ORDER)
  {
    SudokuGrid::set_t result;
    for (int col = stack * SudokuGrid::ORDER; col < (stack + 1) * SudokuGrid::ORDER; ++col)
    {
      for (int row = 0; row < SudokuGrid::ORDER2; ++row)
      {
        SudokuGrid::set_t candidates {*sdkg.pColumn[col][row]};
        for_each(begin(candidates), end(candidates),
                 [&countCandidates](SudokuGrid::value_t val) { ++countCandidates[val - 1]; });
      }
    }
    auto pv = find_if(begin(countCandidates), end(countCandidates),
                      [](size_t e) { return e == 1; });
    if (pv != end(countCandidates))
    {
      newHiddenSingle = true;
      ++_numberOfHiddenSingles;
      cout << "Stack = " << stack << " Hidden single found: ";
      SudokuGrid::value_t candidateValue = 1 + pv - begin(countCandidates);
      cout << candidateValue << endl;
      writeCandidates(cout, sdkg);
      //getchar();

      bool found {false};
      for (int col = stack * SudokuGrid::ORDER;
           !found && col < (stack + 1) * SudokuGrid::ORDER; ++col)
      {
        for (int row = 0; !found && row < SudokuGrid::ORDER2; ++row)
        {
          if (isAnElementOf(candidateValue, sdkg.getCellCandidates(row, col)))
          {
            cout << "Solved HS" << endl;
            sdkg.add(candidateValue, row, col);
            sdkg.calculateAllCellCandidates();
            found = true;
            writeCandidates(cout, sdkg);
            //getchar();
          }
        }
      }
    }
    ++stack;
  }
  return newHiddenSingle;
}

void SudokuSolver::solveByRecursion(SudokuGrid& sdkg)
{
  bool newNakedSingles {false};
  bool newHiddenSingle {false};

  do
  {
    newNakedSingles = solveNakedSingles(sdkg);
    newHiddenSingle = solveHiddenSingle(sdkg);
  } while (newNakedSingles || newHiddenSingle);

  if (!sdkg.isSolvable())
  {
    return;
  }
  if (sdkg.isSolved())
  {
    _solutions.push_back(sdkg);
    cout << "---- Number of encountered not solvables:  "
         << _numberOfNotSolvables << endl;
    cout << "---- Number of encountered naked singles:  "
         << _numberOfNakedSingles << endl;
    cout << "---- Number of encountered hidden singles: "
         << _numberOfHiddenSingles << endl << endl;
  }
  else
  {
    // find first cell containing a set of more then 1 candidate solutions
    bool found = false;
    int row = 0;
    int column = 0;
    while (row < SudokuGrid::ORDER2 && !found)
    {
      while (column < SudokuGrid::ORDER2 && !found)
      {
        found = !sdkg. cellIsSolved(row, column);
        if (!found)
        {
          ++column;
        }
      }
      if (!found)
      {
        ++row;
        column = 0;
      }
    }
    if (found)
    {
      auto C = sdkg.getCellCandidates(row, column);
      auto nCandidates = sizeOf(C);
      //cout << "-- " << &sdkg << " #Candidates for [" << row << "]["
      //     << column << "] = " << nCandidates
      //     << "  " << C << endl << endl;
      if (nCandidates > 1)
      {
        vector<SudokuGrid> nextSdkg;
        // Store nCandidates-1 in vector
        for (int i = 0; i < nCandidates - 1; ++i)
        {
          nextSdkg.push_back(sdkg);
        }
        auto it = C.begin();
        // Solve Sdkg further
        if (sdkg.add(*it, row, column))
        {
          solveByRecursion(sdkg);
        }
        ++it;

        int n = 0;
        while (it != C.end() && !(_wellformed && !_solutions.empty()))
        {
          cout << "R" << endl;
          if (nextSdkg[n].add(*it, row, column))
          {
            solveByRecursion(nextSdkg[n]);
          }
          else
          {
            cout << "$$#@$#%&^%^&^%%$$" << endl;
          }
          ++it;
          ++n;
        }
      }
    }
  }
}

//--------------------------------------------------------- eof SudokuSolver.cpp
