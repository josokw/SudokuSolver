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

SudokuSolver::SudokuSolver(const SudokuGrid& initialSdkg, int nMaxSolutions)
  :  _sdkg(initialSdkg)
  ,  _nMaxSolutions(nMaxSolutions)
  ,  _solutions()
  ,  _numberOfNotSolvables(0)
  ,  _maxRecursionDepth(0)
  ,  _numberOfNakedSingles(0)
  ,  _numberOfHiddenSingles(0)
  ,  _numberOfNakedPairs(0)
{ }

const std::vector<SudokuGrid>& SudokuSolver::solve()
{
  _solutions.clear();
  _sdkg.setID("1");
  solveByRecursion(_sdkg);
  return _solutions;
}

bool SudokuSolver::solveNakedSingles(SudokuGrid& sdkg)
{
  bool newNakedSingles {false};

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
          sdkg.add(*begin(sdkg.getCellCandidates(row, col)),row, col);
          sdkg.calculateAllCellCandidates();
          row = 0;
          col = 0;
          //cout << "NS" << endl;
          //writeCandidates(cout, sdkg);
          //getchar();
        }
      }
    }
  }
  return newNakedSingles;
}

bool SudokuSolver::solveHiddenSingle(SudokuGrid& sdkg)
{
  bool newHiddenSingle {false};

  if (sdkg.isSolvable())
  {
    array<size_t, SudokuGrid::ORDER2> countCandidates {{}};
    int stack {0};

    while (!newHiddenSingle && stack < SudokuGrid::ORDER)
    {
      SudokuGrid::set_t result;
      for (int col = stack * SudokuGrid::ORDER;
           col < (stack + 1) * SudokuGrid::ORDER;
           ++col)
      {
        for (int row = 0; row < SudokuGrid::ORDER2; ++row)
        {
          SudokuGrid::set_t candidates {*sdkg.pColumn[col][row]};
          for_each(begin(candidates), end(candidates),
                   [&countCandidates](SudokuGrid::value_t val)
                   { ++countCandidates[val - 1]; });
        }
      }
      auto pv = find_if(begin(countCandidates), end(countCandidates),
                        [](size_t e) { return e == 1; });
      if (pv != end(countCandidates))
      {
        newHiddenSingle = true;
        ++_numberOfHiddenSingles;
        //cout << "Stack = " << stack << " Hidden single found: ";
        SudokuGrid::value_t candidateValue = 1 + pv - begin(countCandidates);
        //cout << candidateValue << " in " << sdkg.getID() << endl;
        //writeCandidates(cout, sdkg);
        //getchar();

        bool found {false};
        for (int col = stack * SudokuGrid::ORDER;
             !found && col < (stack + 1) * SudokuGrid::ORDER; ++col)
        {
          for (int row = 0; !found && row < SudokuGrid::ORDER2; ++row)
          {
            if (isAnElementOf(candidateValue, sdkg.getCellCandidates(row, col)))
            {
              sdkg.add(candidateValue, row, col);
              sdkg.calculateAllCellCandidates();
              found = true;
              //cout << "Solved HS" << endl;
              //writeCandidates(cout, sdkg);
              //getchar();
            }
          }
        }
      }
      ++stack;
    }
    int band {0};
    while (!newHiddenSingle && band < SudokuGrid::ORDER)
    {
      SudokuGrid::set_t result;
      for (int row = band * SudokuGrid::ORDER;
           row < (band+ 1) * SudokuGrid::ORDER;
           ++row)
      {
        for (int col = 0; col < SudokuGrid::ORDER2; ++col)
        {
          SudokuGrid::set_t candidates {*sdkg.pRow[col][row]};
          for_each(begin(candidates), end(candidates),
                   [&countCandidates](SudokuGrid::value_t val)
                   { ++countCandidates[val - 1]; });
        }
      }
      auto pv = find_if(begin(countCandidates), end(countCandidates),
                        [](size_t e) { return e == 1; });
      if (pv != end(countCandidates))
      {
        newHiddenSingle = true;
        ++_numberOfHiddenSingles;
        //cout << "Stack = " << stack << " Hidden single found: ";
        SudokuGrid::value_t candidateValue = 1 + pv - begin(countCandidates);
        //cout << candidateValue << " in " << sdkg.getID() << endl;
        //writeCandidates(cout, sdkg);
        //getchar();

        bool found {false};
        for (int row = band * SudokuGrid::ORDER;
             !found && row < (band + 1) * SudokuGrid::ORDER; ++row)
        {
          for (int col = 0; !found && col < SudokuGrid::ORDER2; ++col)
          {
            if (isAnElementOf(candidateValue, sdkg.getCellCandidates(row, col)))
            {
              sdkg.add(candidateValue, row, col);
              sdkg.calculateAllCellCandidates();
              found = true;
              //cout << "Solved HS band" << endl;
              //writeCandidates(cout, sdkg);
              //getchar();
            }
          }
        }
      }
      ++band;
    }
  }
  return newHiddenSingle;
}

bool SudokuSolver::solveNakedPair(SudokuGrid& sdkg)
{
  bool newNakedPair {false};

  if (sdkg.isSolvable())
  {
    //writeCandidates(cout, sdkg);
    //getchar();
     // group: rows
    newNakedPair = solveNakedPair(sdkg.pRow);
    // group: columns
    if (!newNakedPair)
    {
      newNakedPair = solveNakedPair(sdkg.pColumn);
    }
    // group: blocks
    if (!newNakedPair)
    {
      newNakedPair = solveNakedPair(sdkg.pBlock);
    }
  }
  if (newNakedPair)
  {
    ++_numberOfNakedPairs;
  }
  return newNakedPair;
}

bool SudokuSolver::solveNakedPair(SudokuGrid::group_t& group)
{
  bool newNakedPair {false};
  SudokuGrid::set_t toBeRemoved;

  for (int groupIndex = 0;
       !newNakedPair && groupIndex < SudokuGrid::ORDER2;
       ++groupIndex)
  {
    auto startIndex = group[groupIndex].cbegin();
    auto endIndex = group[groupIndex].cend();
    startIndex = find_if(startIndex, endIndex, [](const SudokuGrid::set_t* set) { return set->size() == 2; });
    auto pIndex1 = startIndex;
    if (startIndex != endIndex)
    {
      ++startIndex;
      startIndex = find_if(startIndex, endIndex, [pIndex1](const SudokuGrid::set_t* set) { return *set == **pIndex1; });
      auto pIndex2 = startIndex;
      if (startIndex != endIndex)
      {
        toBeRemoved = **pIndex1;
        auto pIndex = group[groupIndex].begin();
        while (pIndex != endIndex)
        {
          if (pIndex != pIndex1 && pIndex != pIndex2)
          {
            if ((**pIndex).size() > 2)
            {
              if (removeElements(toBeRemoved, **pIndex))
              {
                newNakedPair = true;
                //cout << "Naked pair " << groupIndex << " " << toBeRemoved << **pIndex << endl;
              }
            }
          }
          ++pIndex;
        }
      }
    }
  }
  return newNakedPair;
}

void SudokuSolver::solveByRecursion(SudokuGrid& sdkg, int level)
{
  bool newNakedSingles {false};
  bool newHiddenSingle {false};
  bool newNakedPair {false};

  _maxRecursionDepth = max(_maxRecursionDepth, level);
  do
  {
    newNakedSingles = solveNakedSingles(sdkg);
    newHiddenSingle = solveHiddenSingle(sdkg);
    newNakedPair = solveNakedPair(sdkg);
  }
  while (newNakedSingles || newHiddenSingle || newNakedPair);

  if (!sdkg.isSolvable())
  {
    ++_numberOfNotSolvables;
    return;
  }
  if (sdkg.isSolved())
  {
    _solutions.push_back(sdkg);
    cout << "---- Number of solutions:                  "
         << _solutions.size() << endl;
    cout << "---- Number of encountered not solvables:  "
         << _numberOfNotSolvables << endl;
    cout << "---- Maximum recursion depth:              "
         << _maxRecursionDepth << endl;
    cout << "---- Recursion profile:                    "
         << sdkg.getID() << endl;
    cout << "---- Number of encountered naked singles:  "
         << _numberOfNakedSingles << endl;
    cout << "---- Number of encountered hidden singles: "
         << _numberOfHiddenSingles << endl;
    cout << "---- Number of encountered naked pairs:    "
         << _numberOfNakedPairs << endl << endl;
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
        sdkg.setID(sdkg.getID() + ".1");
        sdkg.add(*it, row, column);
        solveByRecursion(sdkg, level + 1);
        ++it;

        int n = 0;
        while (it != end(C)
               && _solutions.size() < size_t(_nMaxSolutions))
        {
          nextSdkg[n].add(*it, row, column);
          nextSdkg[n].setID(nextSdkg[n].getID() + "." + to_string(n + 2));
          solveByRecursion(nextSdkg[n], level + 1);
          ++it;
          ++n;
        }
      }
    }
  }
}

//--------------------------------------------------------- eof SudokuSolver.cpp
