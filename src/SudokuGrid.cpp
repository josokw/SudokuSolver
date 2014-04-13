//--------------------------------------------------------------- SudokuGrid.cpp

#include "SetOperations.h"
#include "SudokuGrid.h"

#include <numeric>
#include <string>
#include <stdexcept>

using namespace std;

const SudokuGrid::set_t SudokuGrid::EMPTY;
const SudokuGrid::set_t SudokuGrid::U(makeRange(1, SudokuGrid::ORDER2 + 1));

ostream& operator<<(ostream& os, const SudokuGrid& Sdkg)
{
  for (int row = 0; row < SudokuGrid::ORDER2; ++row)
  {
    for (int col = 0; col < SudokuGrid::ORDER2; ++col)
    {
      if ((col != 0) && (col % SudokuGrid::ORDER == 0))
      {
        os << "|";
      }
      if (!Sdkg.cellIsSolved(row, col))
      {
        os << " . ";
      }
      else
      {
        os << " " << Sdkg._cell[row][col] << " ";
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
  int value = 0;

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
  :  _numberOfCellsSolved(0)
  ,  _isSolvable(true)
{
  for (int row = 0; row < SudokuGrid::ORDER2; ++row)
  {
    fill(_cell[row], _cell[row] + SudokuGrid::ORDER, 0);
    fill(_candidates[row], _candidates[row] + SudokuGrid::ORDER, U);
  }
  fill(_columnSet, _columnSet + SudokuGrid::ORDER2, U);
  fill(_rowSet, _rowSet + SudokuGrid::ORDER2, U);
  for (int row = 0; row < SudokuGrid::ORDER; ++row)
  {
    fill(_blockSet[row], _blockSet[row] + SudokuGrid::ORDER, U);
  }
  mapPointerArraysToCandidates();
}

SudokuGrid::SudokuGrid(const SudokuGrid& Sdkg)
  :  _numberOfCellsSolved(Sdkg._numberOfCellsSolved)
  ,  _isSolvable(Sdkg._isSolvable)
{
  for (int row = 0; row < SudokuGrid::ORDER2; ++row)
  {
    for (int column = 0; column < SudokuGrid::ORDER2; ++column)
    {
      _cell[row][column] = Sdkg._cell[row][column];
      _candidates[row][column] = Sdkg._candidates[row][column];
    }
  }
  copy(Sdkg._columnSet, Sdkg._columnSet + SudokuGrid::ORDER2, _columnSet);
  copy(Sdkg._rowSet, Sdkg._rowSet + SudokuGrid::ORDER2, _rowSet);
  for (int row = 0; row < SudokuGrid::ORDER; ++row)
  {
    copy(Sdkg._blockSet[row], Sdkg._blockSet[row] +  SudokuGrid::ORDER,
         _blockSet[row]);
  }
  mapPointerArraysToCandidates();
}

SudokuGrid& SudokuGrid::operator=(const SudokuGrid& sdkg)
{
  if (this != &sdkg)
  {
    _numberOfCellsSolved = sdkg._numberOfCellsSolved;
    _isSolvable = sdkg._isSolvable;
    for (int row = 0; row < SudokuGrid::ORDER2; ++row)
    {
      for (int column = 0; column < SudokuGrid::ORDER2; ++column)
      {
        _cell[row][column] = sdkg._cell[row][column];
        _candidates[row][column] = sdkg._candidates[row][column];
      }
    }
    copy(begin(sdkg._columnSet), end(sdkg._columnSet), begin(_columnSet));
    copy(begin(sdkg._rowSet), end(sdkg._rowSet), begin(_rowSet));
    for (int row = 0; row < SudokuGrid::ORDER; ++row)
    {
      for (int column = 0; column < SudokuGrid::ORDER; ++column)
      {
        _blockSet[row][column] = sdkg._blockSet[row][column];
      }
    }
    mapPointerArraysToCandidates();
  }
  return *this;
}

bool SudokuGrid::add(unsigned short value, int row, int column)
{
  bool isAdded = false;

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

void SudokuGrid::unsafeAdd(unsigned short value, int row, int column)
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

//--------------------------------------------------------------------------------------------------

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

std::set<unsigned short> SudokuGrid::calculateCellCandidates(int row, int column) const
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
