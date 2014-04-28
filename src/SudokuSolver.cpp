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

static bool isNotSolved(SudokuGrid::set_t* pSet)
{
  return (pSet->size() > 1);
}

struct IsUnique
{
  size_t* pCount;

  IsUnique(size_t* pCount_)
    : pCount(pCount_)
  {
    fill(pCount, pCount + SudokuGrid::ORDER2, 0);
  }

  void operator()(SudokuGrid::set_t* pSet)
  {
    if (isNotSolved(pSet))
    {
      auto it = (*pSet).begin();
      while (it != (*pSet).end())
      {
        ++(pCount[*it]);
        ++it;
      }
    }
  }

  size_t* UniqueIndex()
  {
    return find(pCount, pCount + SudokuGrid::ORDER2, 1);
  }
};

SudokuSolver::SudokuSolver(const SudokuGrid& initialSdkg, int nMaxSolutions)
  :  _sdkg(initialSdkg)
  ,  _nMaxSolutions(nMaxSolutions)
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
          sdkg.add(*begin(sdkg.getCellCandidates(row, col)),
                   row, col);
          sdkg.calculateAllCellCandidates();
          row = 0;
          col = 0;
        }
      }
    }
  }
  return newNakedSingles;
}

void SudokuSolver::solveHiddenSingles(SudokuGrid& sdkg)
{
  //cout << endl << "----- Solve 'hidden singles': \n\n" << Sdkg <<  endl;

  for (int index = 0; index < SudokuGrid::ORDER2; ++index)
  {
    solveHiddenSinglesPerGroup(sdkg.pRow[index]);
    solveNakedSingles(sdkg);
  }
  for (int index = 0; index < SudokuGrid::ORDER2; ++index)
  {
    solveHiddenSinglesPerGroup(sdkg.pColumn[index]);
    solveNakedSingles(sdkg);
  }
  for (int index = 0; index < SudokuGrid::ORDER2; ++index)
  {
    solveHiddenSinglesPerGroup(sdkg.pBlock[index]);
    solveNakedSingles(sdkg);
  }
}

void SudokuSolver::solveHiddenSinglesPerGroup(SudokuGrid::set_t* group[])
{
  size_t count[SudokuGrid::ORDER2];
  IsUnique isUnique(count);

  for_each(group, group + SudokuGrid::ORDER2, isUnique);
  SudokuGrid::value_t value = *isUnique.UniqueIndex();

  bool ready = false;
  SudokuGrid::set_t** ppSet = group;
  while (ppSet != group + SudokuGrid::ORDER2)
  {
    ready = isAnElementOf(value, **ppSet);
    if (ready)
    {
      ++_numberOfHiddenSingles;
      //cout << "Hidden single: " << Value << endl;
      (*ppSet)->clear();
      (*ppSet)->insert(value);
      ready = true;
    }
    ++ppSet;
  }
}

void SudokuSolver::solveByRecursion(SudokuGrid& sdkg, int level)
{
  bool newNakedSingles {false};
  bool newHiddenSingle {false};

  do
  {
    newNakedSingles = solveNakedSingles(sdkg);
    //newHiddenSingle =
    solveHiddenSingles(sdkg);
  }
  while (newNakedSingles || newHiddenSingle);

  if (!sdkg.isSolvable())
  {
    ++_numberOfNotSolvables;
    return;
  }
  if (sdkg.isSolved())
  {
    _solutions.push_back(sdkg);
    cout << endl;
    cout << "---- Number of solutions:                  "
         << _solutions.size() << endl;
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
      //cout << "-- " << &Sdkg << " #Candidates for [" << row << "][" << column << "] = " << nCandidates
      //     << "  " << C << endl << endl;
      if (nCandidates > 1)
      {
        vector<SudokuGrid> nextSdkg;
        // Store nCandidates-1 in vector
        for (int i = 0; i < nCandidates - 1; ++i)
        {
          nextSdkg.push_back(sdkg);
        }
        auto it = begin(C);
        // Solve Sdkg further
        sdkg.add(*it, row, column);
        solveByRecursion(sdkg, level + 1);
        ++it;

        int n = 0;
        while (it != end(C)
               && _solutions.size() < size_t(_nMaxSolutions))
        {
          nextSdkg[n].add(*it, row, column);
          solveByRecursion(nextSdkg[n], level + 1);
          ++it;
          ++n;
        }
      }
    }
  }
}

//--------------------------------------------------------- eof SudokuSolver.cpp
