#include "AppInfo.h"
#include "SudokuSolver.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[])
{
   std::vector<std::string> inputPuzzles;

   if (argc > 1) {
      std::cout << "- " << APPNAME_VERSION << " started\n\n";

      for (int i = 1; i < argc; ++i) {
         inputPuzzles.push_back(argv[i]);
      }

      for (const auto &fileName : inputPuzzles) {
         try {
            std::ifstream Input(fileName.c_str());

            if (!Input) {
               std::cerr << "- ERROR:  can not open file " << fileName
                         << "\n\n";
               exit(EXIT_FAILURE);
            }

            std::cout << "---- Sudoku puzzle file: " << fileName << "\n\n";

            SudokuGrid puzzle;
            std::vector<SudokuGrid> solutions(5);

            Input >> puzzle;
            std::cout << "- Number of clues: "
                      << puzzle.getNumberOfCellsSolved() << "\n\n"
                      << puzzle << std::endl;
            SudokuSolver sdkSolver(puzzle, 15);

            auto startTP = std::chrono::system_clock::now();
            solutions = sdkSolver.solve();
            std::chrono::duration<double> dur =
               std::chrono::system_clock::now() - startTP;

            std::cout << "---- Processing time: " << std::fixed
                      << std::setprecision(4) << dur.count() << " sec\n\n";
            std::cout << "---- Sudoku solution(s), " << solutions.size()
                      << " found:\n"
                      << std::endl;

            for (const auto &solution : solutions) {
               std::cout << solution << "\n";
            }
         } catch (const std::exception &e) {
            std::clog << "- EXCEPTION " << argv[0] << ": " << e.what()
                      << std::endl;
         } catch (...) {
            std::clog << "- UNKNOWN EXCEPTION " << argv[0] << std::endl;
         }
      }
   }
   else {
      std::cout << "- USAGE: " << argv[0] << " <Sudoku files>" << std::endl;
   }

   return 0;
}
