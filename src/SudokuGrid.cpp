//--------------------------------------------------------------- SudokuGrid.cpp

#include "SetOperations.h"
#include "SudokuGrid.h"

#include <numeric>
#include <string>
#include <stdexcept>

using namespace std;

const SudokuGrid::set_t SudokuGrid::EMPTY;
const SudokuGrid::set_t SudokuGrid::U
{
  makeRange(1, SudokuGrid::ORDER2 + 1)
};

ostream& operator<<(ostream& os, const SudokuGrid& sdkg)
{
  for (int row = 0; row < SudokuGrid::ORDER2; ++row)
  {
    for (int col = 0; col < SudokuGrid::ORDER2; ++col)
    {
      if ((col != 0) && (col % SudokuGrid::ORDER == 0))
      {
        os << "|";
      }
      if (!sdkg.cellIsSolved(row, col))
      {
        os << " . ";
      }
      else
      {
        os << " " << sdkg._cell[row][col] << " ";
      }
    }
    os << endl;
    if ((row != SudokuGrid::ORDER2 - 1)
        && (row % SudokuGrid::ORDER == (SudokuGrid::ORDER - 1)))
    {
      // @TODO Remove magic number
      os << string(29, '-') << endl;
    }
  }
  os << endl;

  return os;
}

std::istream& operator>>(std::istream& is, SudokuGrid& sdkg)
{
  int value {0};

  for (int row = 0; row < SudokuGrid::ORDER2; ++row)
  {
    for (int col = 0; col < SudokuGrid::ORDER2; ++col)
    {
      is >> value;
      if (!sdkg.add(value, row, col))
      {
        cerr << "ERROR for " << value << " in [" << row << "]["
             << col << "]" << endl;
      }
    }
  }

  return is;
}

void writeLatex(std::ostream& os, const SudokuGrid& sdkg)
{
  os << "\n\\begin{sudoku}\n";
  for (int row = 0; row < SudokuGrid::ORDER2; ++row)
  {
    for (int col = 0; col < SudokuGrid::ORDER2; ++col)
    {
      if (!sdkg.cellIsSolved(row, col))
      {
        os << "| ";
      }
      else
      {
        os << "|" << sdkg._cell[row][col];
      }
    }
    os << "|.\n";
  }
  os << "\\end{sudoku}\n\n";
}

SudokuGrid::SudokuGrid()
  : _numberOfCellsSolved {0}
  , _isSolvable {true}
  , _cell {{}}
  , _columnSet {{}}
  , _rowSet {{}}
  , _blockSet {{}}
{
  for (int row = 0; row < SudokuGrid::ORDER2; ++row)
  {
    for (int column = 0; column < SudokuGrid::ORDER2; ++column)
    {
      _candidates[row][column] = U;
    }
  }
  _columnSet.fill(U);
  _rowSet.fill(U);
  for (int row = 0; row < SudokuGrid::ORDER; ++row)
  {
    for (int column = 0; column < SudokuGrid::ORDER; ++column)
    {
      _blockSet[row][column] = U;
    }
  }
  mapPointerArraysToCandidates();
}

SudokuGrid::SudokuGrid(const SudokuGrid& sdkg)
  :  _numberOfCellsSolved {sdkg._numberOfCellsSolved}
  ,  _isSolvable {sdkg._isSolvable}
  ,  _cell {{}}
  ,  _columnSet {{}}
  ,  _rowSet {{}}
  ,  _blockSet {{}}
{
  _cell = sdkg._cell;
  _columnSet = sdkg._columnSet;
  _rowSet = sdkg._rowSet;
  for (int row = 0; row < SudokuGrid::ORDER2; ++row)
  {
    for (int column = 0; column < SudokuGrid::ORDER2; ++column)
    {
      //_cell[row][column] = sdkg._cell[row][column];
      _candidates[row][column] = sdkg._candidates[row][column];
    }
  }
  _blockSet = sdkg._blockSet;
  mapPointerArraysToCandidates();
}

SudokuGrid& SudokuGrid::operator=(const SudokuGrid& sdkg)
{
  if (this != &sdkg)
  {
    _numberOfCellsSolved = sdkg._numberOfCellsSolved;
    _isSolvable = sdkg._isSolvable;
    _cell = sdkg._cell;
    for (int row = 0; row < SudokuGrid::ORDER2; ++row)
    {
      for (int column = 0; column < SudokuGrid::ORDER2; ++column)
      {
        //_cell[row][column] = sdkg._cell[row][column];
        _candidates[row][column] = sdkg._candidates[row][column];
      }
    }
    _columnSet = sdkg._columnSet;
    _rowSet = sdkg._rowSet;
    _blockSet = sdkg._blockSet;
    mapPointerArraysToCandidates();
  }
  return *this;
}

bool SudokuGrid::add(value_t value, int row, int column)
{
  bool isAdded {false};

  if (isAnElementOf(value, SudokuGrid::U))
  {
    //cout << "-- Add: " << Value << " in [" << Row << "][" << Column << "]" << endl;
    if (isAnElementOf(value, _columnSet[column])
        && isAnElementOf(value, _rowSet[row])
        && isAnElementOf(value, _blockSet[row / SudokuGrid::ORDER][column / SudokuGrid::ORDER]))
    {
      _columnSet[column].erase(value);
      _rowSet[row].erase(value);
      _blockSet[row / SudokuGrid::ORDER][column / SudokuGrid::ORDER].erase(value);
      _candidates[row][column] = SudokuGrid::EMPTY;
      _cell[row][column] = value;
      isAdded = true;
      ++_numberOfCellsSolved;
    }
    else
    {
      throw std::logic_error("-- Sudoku elements must be in the range "
                             "[0," + to_string(SudokuGrid::ORDER2) + "]");
    }
  }
  else
  {
    if (value != 0)
    {
      throw std::logic_error("-- Sudoku element: " +
                             to_string(value) + " must be in the range "
                             "[0," + to_string(SudokuGrid::ORDER2) + "]");
    }
    _cell[row][column] = 0;
    isAdded = true;
  }

  return isAdded;
}

void SudokuGrid::unsafeAdd(value_t value, int row, int column)
{
  _columnSet[column].erase(value);
  _rowSet[row].erase(value);
  _blockSet[row / SudokuGrid::ORDER][column / SudokuGrid::ORDER].erase(value);
  _candidates[row][column] = SudokuGrid::EMPTY;
  _cell[row][column] = value;
  if (value != 0)
  {
    ++_numberOfCellsSolved;
  }
}

//------------------------------------------------------------------------------

void SudokuGrid::calculateAllCellCandidates()
{
  for (int row = 0; row < SudokuGrid::ORDER2 && _isSolvable; ++row)
  {
    for (int col = 0; col < SudokuGrid::ORDER2 && _isSolvable; ++col)
    {
      if (!cellIsSolved(row, col))
      {
        _candidates[row][col] = calculateCellCandidates(row, col);
        if (_candidates[row][col].empty())
        {
          _isSolvable = false;
        }
      }
    }
  }
}

SudokuGrid::set_t SudokuGrid::calculateCellCandidates(int row, int column) const
{
  return _columnSet[column]
         * _rowSet[row]
         * _blockSet[row / SudokuGrid::ORDER][column / SudokuGrid::ORDER];
}

void SudokuGrid::mapPointerArraysToCandidates()
{
  for (int i = 0; i < SudokuGrid::ORDER2; ++i)
  {
    for (int j = 0; j < SudokuGrid::ORDER2; ++j)
    {
      pRow[i][j] = &_candidates[i][j];
      pColumn[i][j] = &_candidates[j][i];
    }
  }

  for (int blockIndex = 0; blockIndex < SudokuGrid::ORDER2; ++blockIndex)
  {
    int index = 0;
    int starti = SudokuGrid::ORDER * (blockIndex / SudokuGrid::ORDER);
    int startj = SudokuGrid::ORDER * (blockIndex % SudokuGrid::ORDER);

    for (int i = starti; i < starti + SudokuGrid::ORDER; ++i)
    {
      for (int j = startj; j < startj + SudokuGrid::ORDER; ++j)
      {
        pBlock[blockIndex][index++] = &_candidates[i][j];
      }
    }
  }
}

//----------------------------------------------------------- eof SudokuGrid.cpp
