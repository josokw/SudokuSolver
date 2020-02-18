#include "SudokuGrid.h"
#include "SetOperations.h"

#include <numeric>
#include <stdexcept>
#include <string>

namespace {
const std::string subhorz(SudokuGrid::ORDER2 + SudokuGrid::ORDER + 1, '-');
const std::string subhorz2(SudokuGrid::ORDER2 + SudokuGrid::ORDER + 1, ' ');
const std::string horz = "+" + subhorz + "+" + subhorz + "+" + subhorz + "+";
const std::string horz2 =
   "|" + subhorz2 + "|" + subhorz2 + "|" + subhorz2 + "|";
const std::string vert(11, '|');
} // namespace

const SudokuGrid::set_t SudokuGrid::EMPTY;
const SudokuGrid::set_t SudokuGrid::U{
   makeRange<SudokuGrid::value_t>(1, SudokuGrid::ORDER2 + 1)};

std::ostream &operator<<(std::ostream &os, const SudokuGrid &sdkg)
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
            os << " " << sdkg.cell_[row][col] << " ";
         }
      }
      os << std::endl;
      if ((row != SudokuGrid::ORDER2 - 1) &&
          (row % SudokuGrid::ORDER == (SudokuGrid::ORDER - 1))) {
         /// @todo Remove magic number
         os << std::string(29, '-') << std::endl;
      }
   }
   os << std::endl;

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
            os << "\n" << horz2 << std::endl;
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
      os << std::endl;
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
                  (pos == 1) ? os << sdkg.cell_[row][col] : os << ' ';
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
      os << std::endl;
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
      os << "\n" << horz2 << std::endl;
   }
   os << horz << std::endl;
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
            os << "|" << sdkg.cell_[row][col];
         }
      }
      os << "|.\n";
   }
   os << "\\end{sudoku}\n\n";
}

SudokuGrid::SudokuGrid()
   : id_{}
   , numberOfCellsSolved_{0}
   , isSolvable_{true}
   , cell_{{}}
   , columnSet_{{}}
   , rowSet_{{}}
   , blockSet_{{}}
   , candidates_{{}}
   , pRow{}
   , pColumn{}
   , pBlock{}
{
   for (int row = 0; row < SudokuGrid::ORDER2; ++row) {
      for (int column = 0; column < SudokuGrid::ORDER2; ++column) {
         candidates_[row][column] = U;
      }
   }
   columnSet_.fill(U);
   rowSet_.fill(U);
   for (int row = 0; row < SudokuGrid::ORDER; ++row) {
      for (int column = 0; column < SudokuGrid::ORDER; ++column) {
         blockSet_[row][column] = U;
      }
   }
   mapPointerArraysToCandidates();
}

SudokuGrid::SudokuGrid(const SudokuGrid &sdkg)
   : id_{sdkg.id_}
   , numberOfCellsSolved_{sdkg.numberOfCellsSolved_}
   , isSolvable_{sdkg.isSolvable_}
   , cell_{{}}
   , columnSet_{{}}
   , rowSet_{{}}
   , blockSet_{{}}
   , candidates_{{}}
   , pRow{}
   , pColumn{}
   , pBlock{}
{
   cell_ = sdkg.cell_;
   columnSet_ = sdkg.columnSet_;
   rowSet_ = sdkg.rowSet_;
   blockSet_ = sdkg.blockSet_;
   candidates_ = sdkg.candidates_;
   mapPointerArraysToCandidates();
   // writeCandidates(cout, sdkg);
   // cout << "COPY" << endl;
   // writeCandidates(cout, *this);
}

SudokuGrid &SudokuGrid::operator=(const SudokuGrid &sdkg)
{
   if (this != &sdkg) {
      id_ = sdkg.id_;
      numberOfCellsSolved_ = sdkg.numberOfCellsSolved_;
      isSolvable_ = sdkg.isSolvable_;
      cell_ = sdkg.cell_;
      columnSet_ = sdkg.columnSet_;
      rowSet_ = sdkg.rowSet_;
      blockSet_ = sdkg.blockSet_;
      candidates_ = sdkg.candidates_;
      mapPointerArraysToCandidates();
   }
   return *this;
}

bool SudokuGrid::removeCandidates(const set_t &rem, int row, int column)
{
   set_t candidates{candidates_[row][column]};
   candidates_[row][column] = candidates_[row][column] - rem;
   return candidates != candidates_[row][column];
}

void SudokuGrid::add(const value_t value, int row, int column)
{
   if (isSolvable()) {
      if (isAnElementOf(value, SudokuGrid::U)) {
         // cout << "-- Add: " << value << " in [" << row << "][" << column
         //      << // "]" << endl;
         if (isAnElementOf(value, columnSet_[column]) &&
             isAnElementOf(value, rowSet_[row]) &&
             isAnElementOf(value, blockSet_[row / SudokuGrid::ORDER]
                                           [column / SudokuGrid::ORDER])) {
            columnSet_[column].erase(value);
            rowSet_[row].erase(value);
            blockSet_[row / SudokuGrid::ORDER][column / SudokuGrid::ORDER]
               .erase(value);
            candidates_[row][column] = SudokuGrid::EMPTY;
            cell_[row][column] = value;
            ++numberOfCellsSolved_;
         }
         else {
            // cout << "ADD ERROR" << endl;
            // writeCandidates(cout, *this);
            // getchar();
            throw std::logic_error(
               "-- [" + std::to_string(row) + "][" + std::to_string(column) +
               "] Sudoku element: " + std::to_string(value) + " not in groups");
         }
      }
      else {
         if (value != 0) {
            throw std::logic_error(
               "-- Sudoku element: " + std::to_string(value) +
               " must be in the range "
               "[1," +
               std::to_string(SudokuGrid::ORDER2) + "]");
         }
         cell_[row][column] = 0;
      }
      calculateAllCellCandidates();
   }
   else {
      std::cout << "ADD to not solvable sudoku grid " << getID() << std::endl;
   }
}

void SudokuGrid::unsafeAdd(const value_t value, int row, int column)
{
   columnSet_[column].erase(value);
   rowSet_[row].erase(value);
   blockSet_[row / SudokuGrid::ORDER][column / SudokuGrid::ORDER].erase(value);
   candidates_[row][column] = SudokuGrid::EMPTY;
   cell_[row][column] = value;
   if (value != 0) {
      ++numberOfCellsSolved_;
   }
   calculateAllCellCandidates();
}

//------------------------------------------------------------------------------

void SudokuGrid::calculateAllCellCandidates()
{
   for (int row = 0; row < SudokuGrid::ORDER2 && isSolvable_; ++row) {
      for (int col = 0; col < SudokuGrid::ORDER2 && isSolvable_; ++col) {
         if (!cellIsSolved(row, col)) {
            candidates_[row][col] = calculateCellCandidates(row, col);
            if (candidates_[row][col].empty()) {
               isSolvable_ = false;
            }
         }
      }
   }
}

SudokuGrid::set_t SudokuGrid::calculateCellCandidates(int row, int column) const
{
   return columnSet_[column] * rowSet_[row] *
          blockSet_[row / SudokuGrid::ORDER][column / SudokuGrid::ORDER];
}

void SudokuGrid::mapPointerArraysToCandidates()
{
   for (int groupIndex = 0; groupIndex < SudokuGrid::ORDER2; ++groupIndex) {
      for (int index = 0; index < SudokuGrid::ORDER2; ++index) {
         pRow[groupIndex][index] = &candidates_[groupIndex][index];
         pColumn[groupIndex][index] = &candidates_[index][groupIndex];
      }
   }
   for (int blockIndex = 0; blockIndex < SudokuGrid::ORDER2; ++blockIndex) {
      int index = 0;
      int starti = SudokuGrid::ORDER * (blockIndex / SudokuGrid::ORDER);
      int startj = SudokuGrid::ORDER * (blockIndex % SudokuGrid::ORDER);

      for (int i = starti; i < starti + SudokuGrid::ORDER; ++i) {
         for (int j = startj; j < startj + SudokuGrid::ORDER; ++j) {
            pBlock[blockIndex][index++] = &candidates_[i][j];
         }
      }
   }
}
