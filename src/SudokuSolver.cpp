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
  size_t* _pCount;

  IsUnique(size_t* pCount)
    : _pCount(pCount)
  {
    fill(_pCount, _pCount + SudokuGrid::ORDER2, 0);
  }

  void operator()(SudokuGrid::set_t* pSet)
  {
    if (isNotSolved(pSet))
    {
      auto it = (*pSet).begin();
      while (it != (*pSet).end())
      {
        ++(_pCount[*it]);
        ++it;
      }
    }
  }

  size_t* UniqueIndex()
  {
    return find(_pCount, _pCount + SudokuGrid::ORDER2, 1);
  }
};

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

void SudokuSolver::solveNakedSingles(SudokuGrid& sdkg)
{
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
          ++_numberOfNakedSingles;
          sdkg.unsafeAdd(*sdkg.getCellCandidates(row, col).begin(),
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
}

void SudokuSolver::solveHiddenSingles(SudokuGrid& sdkg)
{
  //cout << endl << "----- Solve 'hidden singles': \n\n" << sdkg <<  endl;
  for (auto& row: sdkg.pRow)
  {
    solveHiddenSinglesPerGroup(row);
    solveNakedSingles(sdkg);
  }
  for (auto& colomn: sdkg.pColumn)
  {
    solveHiddenSinglesPerGroup(colomn);
    solveNakedSingles(sdkg);
  }
  for (auto& block: sdkg.pBlock)
  {
    solveHiddenSinglesPerGroup(block);
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
      //cout << "Hidden single: " << value << endl;
      (*ppSet)->clear();
      (*ppSet)->insert(value);
      ready = true;
    }
    ++ppSet;
  }
}

void SudokuSolver::solveByRecursion(SudokuGrid& sdkg)
{
  //cout << "---- Solve by recursion:" << endl << endl;
  solveNakedSingles(sdkg);
  solveHiddenSingles(sdkg);
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
