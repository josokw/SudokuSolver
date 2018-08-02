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

using namespace std;

int main(int argc, char *argv[])
{
   vector<string> inputPuzzles;

   if (argc > 1) {
      cout << "- " << APPNAME_VERSION << " started\n\n";

      for (int i = 1; i < argc; ++i) {
         inputPuzzles.push_back(argv[i]);
      }

      for (const auto &fileName : inputPuzzles) {
         try {
            ifstream Input(fileName.c_str());

            if (!Input) {
               cerr << "- ERROR:  can not open file " << fileName << "\n\n";
               exit(EXIT_FAILURE);
            }

            cout << "---- Sudoku puzzle file: " << fileName << "\n\n";

            SudokuGrid puzzle;
            vector<SudokuGrid> solutions(5);

            Input >> puzzle;
            cout << "- Number of clues: " << puzzle.getNumberOfCellsSolved()
                 << "\n\n"
                 << puzzle << endl;
            SudokuSolver sdkSolver(puzzle, 15);

            auto startTP = chrono::system_clock::now();
            solutions = sdkSolver.solve();
            chrono::duration<double> dur =
               chrono::system_clock::now() - startTP;

            cout << "---- Processing time: " << fixed << setprecision(4)
                 << dur.count() << " sec\n"
                 << endl;
            cout << "---- Sudoku solution(s), " << solutions.size()
                 << " found:\n"
                 << endl;

            for (const auto &solution : solutions) {
               cout << solution << "\n";
            }
         } catch (const exception &e) {
            clog << "- EXCEPTION " << argv[0] << ": " << e.what() << endl;
         } catch (...) {
            clog << "- UNKNOWN EXCEPTION " << argv[0] << endl;
         }
      }
   }
   else {
      cout << "- USAGE: " << argv[0] << " <Sudoku files>" << endl;
   }

   return 0;
}
