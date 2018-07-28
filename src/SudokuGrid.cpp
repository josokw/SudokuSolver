#include "SudokuGrid.h"
#include "SetOperations.h"

#include <numeric>
#include <stdexcept>
#include <string>

using namespace std;

namespace {
const string subhorz(13, '-');
const string subhorz2(13, ' ');
const string horz = "+" + subhorz + "+" + subhorz + "+" + subhorz + "+";
const string horz2 = "|" + subhorz2 + "|" + subhorz2 + "|" + subhorz2 + "|";
const string vert(11, '|');
} // namespace

const SudokuGrid::set_t SudokuGrid::EMPTY;
const SudokuGrid::set_t SudokuGrid::U{
   makeRange<SudokuGrid::value_t>(1, SudokuGrid::ORDER2 + 1)};

ostream &operator<<(ostream &os, const SudokuGrid &sdkg)
{
   for (int row = 0; row < SudokuGrid::ORDER2; ++row) {
      for (int col = 0; col < SudokuGrid::ORDER2; ++col) {
         if ((col != 0) && (col % SudokuGrid::ORDER == 0)) {
            os << "|";
         }
         if (!sdkg.cellIsSolved(row, col)) {
            os << " . ";
         }
         else {
            os << " " << sdkg._cell[row][col] << " ";
         }
      }
      os << endl;
      if ((row != SudokuGrid::ORDER2 - 1) &&
          (row % SudokuGrid::ORDER == (SudokuGrid::ORDER - 1))) {
         // @TODO Remove magic number
         os << string(29, '-') << endl;
      }
   }
   os << endl;

   return os;
}

std::istream &operator>>(std::istream &is, SudokuGrid &sdkg)
{
   for (int row = 0; row < SudokuGrid::ORDER2; ++row) {
      for (int col = 0; col < SudokuGrid::ORDER2; ++col) {
         char value{0};
         is >> std::skipws >> value;
         if (value == '.') {
            value = '0';
         }
         sdkg.add(value - '0', row, col);
      }
   }
   sdkg.calculateAllCellCandidates();
   return is;
}

void writeCandidates(std::ostream &os, const SudokuGrid &sdkg)
{
   for (int row = 0; row < SudokuGrid::ORDER2; ++row) {
      for (int col = 0; col < SudokuGrid::ORDER2; ++col) {
         if (col == 0 and (row % SudokuGrid::ORDER == 0) and
             (row < SudokuGrid::ORDER2 - 1)) {
            os << horz;
            if (row == 0) {
               os << " ID = " << &sdkg << " " << sdkg.getID() << " "
                  << (sdkg.isSolvable() ? "" : " not solvable");
            }
            os << endl << horz2 << endl;
         }
         if (col == 0) {
            os << "| ";
         }
         SudokuGrid::set_t candidates = sdkg.getCellCandidates(row, col);
         for (SudokuGrid::value_t v = 1; v <= SudokuGrid::ORDER; ++v) {
            if (isAnElementOf(v, candidates)) {
               os << v;
            }
            else {
               candidates == SudokuGrid::EMPTY ? os << ' ' : os << '.';
            }
         }
         if ((col != 0) && (col % SudokuGrid::ORDER == 2)) {
            os << " | ";
         }
         else {
            os << " ";
         }
      }
      os << endl;
      for (int col = 0; col < SudokuGrid::ORDER2; ++col) {
         if (col == 0) {
            os << "| ";
         }
         int pos{-1};
         SudokuGrid::set_t candidates{sdkg.getCellCandidates(row, col)};
         for (SudokuGrid::value_t v = SudokuGrid::ORDER + 1;
              v <= 2 * SudokuGrid::ORDER; ++v) {
            if (isAnElementOf(v, candidates)) {
               os << v;
            }
            else {
               if (candidates == SudokuGrid::EMPTY) {
                  ++pos;
                  (pos == 1) ? os << sdkg._cell[row][col] : os << ' ';
               }
               else {
                  os << '.';
               }
            }
         }
         if ((col != 0) && (col % SudokuGrid::ORDER == 2)) {
            os << " | ";
         }
         else {
            os << " ";
         }
      }
      os << endl;
      for (int col = 0; col < SudokuGrid::ORDER2; ++col) {
         if (col == 0) {
            os << "| ";
         }
         SudokuGrid::set_t candidates{sdkg.getCellCandidates(row, col)};
         for (SudokuGrid::value_t v = 2 * SudokuGrid::ORDER + 1;
              v < 3 * SudokuGrid::ORDER + 1; ++v) {
            if (isAnElementOf(v, candidates)) {
               os << v;
            }
            else {
               candidates == SudokuGrid::EMPTY ? os << ' ' : os << '.';
            }
         }
         if ((col != 0) && (col % SudokuGrid::ORDER == 2)) {
            os << " | ";
         }
         else {
            os << " ";
         }
      }
      os << endl << horz2 << endl;
   }
   os << horz << endl;
}

void writeLatex(std::ostream &os, const SudokuGrid &sdkg)
{
   os << "\n\\begin{sudoku}\n";
   for (int row = 0; row < SudokuGrid::ORDER2; ++row) {
      for (int col = 0; col < SudokuGrid::ORDER2; ++col) {
         if (!sdkg.cellIsSolved(row, col)) {
            os << "| ";
         }
         else {
            os << "|" << sdkg._cell[row][col];
         }
      }
      os << "|.\n";
   }
   os << "\\end{sudoku}\n\n";
}

SudokuGrid::SudokuGrid()
   : _id{}
   , _numberOfCellsSolved{0}
   , _isSolvable{true}
   , _cell{{}}
   , _columnSet{{}}
   , _rowSet{{}}
   , _blockSet{{}}
   , _candidates{{}}
   , pRow{}
   , pColumn{}
   , pBlock{}
{
   for (int row = 0; row < SudokuGrid::ORDER2; ++row) {
      for (int column = 0; column < SudokuGrid::ORDER2; ++column) {
         _candidates[row][column] = U;
      }
   }
   _columnSet.fill(U);
   _rowSet.fill(U);
   for (int row = 0; row < SudokuGrid::ORDER; ++row) {
      for (int column = 0; column < SudokuGrid::ORDER; ++column) {
         _blockSet[row][column] = U;
      }
   }
   mapPointerArraysToCandidates();
}

SudokuGrid::SudokuGrid(const SudokuGrid &sdkg)
   : _id{sdkg._id}
   , _numberOfCellsSolved{sdkg._numberOfCellsSolved}
   , _isSolvable{sdkg._isSolvable}
   , _cell{{}}
   , _columnSet{{}}
   , _rowSet{{}}
   , _blockSet{{}}
   , _candidates{{}}
   , pRow{}
   , pColumn{}
   , pBlock{}
{
   _cell = sdkg._cell;
   _columnSet = sdkg._columnSet;
   _rowSet = sdkg._rowSet;
   _blockSet = sdkg._blockSet;
   _candidates = sdkg._candidates;
   mapPointerArraysToCandidates();
   // writeCandidates(cout, sdkg);
   // cout << "COPY" << endl;
   // writeCandidates(cout, *this);
}

SudokuGrid &SudokuGrid::operator=(const SudokuGrid &sdkg)
{
   if (this != &sdkg) {
      _id = sdkg._id;
      _numberOfCellsSolved = sdkg._numberOfCellsSolved;
      _isSolvable = sdkg._isSolvable;
      _cell = sdkg._cell;
      _columnSet = sdkg._columnSet;
      _rowSet = sdkg._rowSet;
      _blockSet = sdkg._blockSet;
      _candidates = sdkg._candidates;
      mapPointerArraysToCandidates();
   }
   return *this;
}

bool SudokuGrid::removeCandidates(const set_t &rem, int row, int column)
{
   set_t candidates{_candidates[row][column]};
   _candidates[row][column] = _candidates[row][column] - rem;
   return candidates != _candidates[row][column];
}

void SudokuGrid::add(const value_t value, int row, int column)
{
   if (isSolvable()) {
      if (isAnElementOf(value, SudokuGrid::U)) {
         // cout << "-- Add: " << value << " in [" << row << "][" << column
         //      << // "]" << endl;
         if (isAnElementOf(value, _columnSet[column]) &&
             isAnElementOf(value, _rowSet[row]) &&
             isAnElementOf(value, _blockSet[row / SudokuGrid::ORDER]
                                           [column / SudokuGrid::ORDER])) {
            _columnSet[column].erase(value);
            _rowSet[row].erase(value);
            _blockSet[row / SudokuGrid::ORDER][column / SudokuGrid::ORDER]
               .erase(value);
            _candidates[row][column] = SudokuGrid::EMPTY;
            _cell[row][column] = value;
            ++_numberOfCellsSolved;
         }
         else {
            // cout << "ADD ERROR" << endl;
            // writeCandidates(cout, *this);
            // getchar();
            throw std::logic_error(
               "-- [" + to_string(row) + "][" + to_string(column) +
               "] Sudoku element: " + to_string(value) + " not in groups");
         }
      }
      else {
         if (value != 0) {
            throw std::logic_error("-- Sudoku element: " + to_string(value) +
                                   " must be in the range "
                                   "[1," +
                                   to_string(SudokuGrid::ORDER2) + "]");
         }
         _cell[row][column] = 0;
      }
      calculateAllCellCandidates();
   }
   else {
      cout << "ADD to not solvable sudoku grid " << getID() << endl;
   }
}

void SudokuGrid::unsafeAdd(const value_t value, int row, int column)
{
   _columnSet[column].erase(value);
   _rowSet[row].erase(value);
   _blockSet[row / SudokuGrid::ORDER][column / SudokuGrid::ORDER].erase(value);
   _candidates[row][column] = SudokuGrid::EMPTY;
   _cell[row][column] = value;
   if (value != 0) {
      ++_numberOfCellsSolved;
   }
   calculateAllCellCandidates();
}

//------------------------------------------------------------------------------

void SudokuGrid::calculateAllCellCandidates()
{
   for (int row = 0; row < SudokuGrid::ORDER2 && _isSolvable; ++row) {
      for (int col = 0; col < SudokuGrid::ORDER2 && _isSolvable; ++col) {
         if (!cellIsSolved(row, col)) {
            _candidates[row][col] = calculateCellCandidates(row, col);
            if (_candidates[row][col].empty()) {
               _isSolvable = false;
            }
         }
      }
   }
}

SudokuGrid::set_t SudokuGrid::calculateCellCandidates(int row, int column) const
{
   return _columnSet[column] * _rowSet[row] *
          _blockSet[row / SudokuGrid::ORDER][column / SudokuGrid::ORDER];
}

void SudokuGrid::mapPointerArraysToCandidates()
{
   for (int groupIndex = 0; groupIndex < SudokuGrid::ORDER2; ++groupIndex) {
      for (int index = 0; index < SudokuGrid::ORDER2; ++index) {
         pRow[groupIndex][index] = &_candidates[groupIndex][index];
         pColumn[groupIndex][index] = &_candidates[index][groupIndex];
      }
   }
   for (int blockIndex = 0; blockIndex < SudokuGrid::ORDER2; ++blockIndex) {
      int index = 0;
      int starti = SudokuGrid::ORDER * (blockIndex / SudokuGrid::ORDER);
      int startj = SudokuGrid::ORDER * (blockIndex % SudokuGrid::ORDER);

      for (int i = starti; i < starti + SudokuGrid::ORDER; ++i) {
         for (int j = startj; j < startj + SudokuGrid::ORDER; ++j) {
            pBlock[blockIndex][index++] = &_candidates[i][j];
         }
      }
   }
}
