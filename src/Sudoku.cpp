//------------------------------------------------------------------- Sudoku.cpp

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

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  vector<string> input;

  if (argc > 1)
  {
    cout << "- " << APPNAME_VERSION << " started\n\n";

    for (int i = 1; i < argc; ++i)
    {
      input.push_back(argv[i]);
    }

    for (auto fileName: input)
    {
      try
      {
        ifstream Input(fileName.c_str());

        if (!Input)
        {
          cerr << "- ERROR:  can not open file " << fileName << "\n\n";
          exit(EXIT_FAILURE);
        }

        cout << "---- Sudoku file: " << fileName << "\n\n";

        SudokuGrid sdkg;
        vector<SudokuGrid> solutions(5);

        Input >> sdkg;
        cout << "- Number of clues: " << sdkg.getNumberOfCellsSolved() << endl;
        cout << endl << sdkg << endl;
        SudokuSolver sdkSolver(sdkg);

        auto t1 = chrono::system_clock::now();
        solutions = sdkSolver.solve();
        chrono::duration<double> d = chrono::system_clock::now() - t1;
        cout << "---- Processing time: " << fixed << setprecision(4)
             << d.count() << " sec" << endl << endl;
        cout << "---- Sudoku solution(s), " << solutions.size()
             << " found:\n";
        for (auto& solution: solutions)
        {
          cout << endl << solution << endl;
        }
        cout << "- Sudoku solver, solved puzzle: " << fileName << endl << endl;
      }
      catch(const exception& e)
      {
        clog << "- EXCEPTION " << argv[0] << ": " << e.what() << endl;
      }
      catch(...)
      {
        clog << "- UNKNOWN EXCEPTION " << argv[0] << endl;
      }
    }
  }
  else
  {
    cout << "- USAGE: " << argv[0] << " <Sudoku files>" << endl;
  }

  return 0;
}

//--------------------------------------------------------------- eof Sudoku.cpp
