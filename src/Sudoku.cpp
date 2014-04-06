//------------------------------------------------------------------- Sudoku.cpp

//#include <boost/program_options.hpp>

#include "AppInfo.h"
#include "SudokuSolver.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

//namespace po = boost::program_options;

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
   //  po::options_description desc("Allowed options");
   //  desc.add_options()
   //    ("help,h", "Prints this message")
   //    ("first,1", "Search for first solution, otherwise all other solutions")
   //    ("input-file", po::value<vector<string> >(), "Input files puzzles")
   //    ("output-file,o", po::value<string>(), "Output file solution(s)")
   //    //    ("nakedsingles,n", "")
   //    ;

   //  po::positional_options_description pos;
   //  pos.add("input-file", -1);

   //  po::variables_map vm;
   //  store(po::command_line_parser(argc, argv).
   //	options(desc).positional(pos).run(), vm);

   //  po::notify(vm);

   //  if (vm.count("help"))
   //  {
   //    clog << desc << endl;
   //    return 0;
   //  }

   //  vector<string> input;
   //  if(vm.count("input-file"))
   //  {
   //    input = vm["input-file"].as<vector<string> >();
   //  }
   //  else
   //  {
   //    clog << "No input files." << endl;
   //    return 1;
   //  }

   //  string output;
   //  ofstream OutputFile;
   //  if (vm.count("output-file"))
   //  {
   //    output = vm["output-file"].as<string>();
   //    OutputFile.open(output.c_str());
   //    if (!OutputFile)
   //    {
   //      return 1;
   //    }
   //    cout.rdbuf(OutputFile.rdbuf());
   //  //}

   vector<string> input;
   if (argc > 1)
   {
      input.push_back(argv[1]);

      cout << "- " << APPNAME_VERSION << " started\n\n";

      for (vector<string>::const_iterator it = input.begin();
           it != input.end(); ++it)
      {
         try
         {
            ifstream Input(it->c_str());

            if (!Input)
            {
               cerr << "\tERROR:  can not open file " << *it << "\n\n";
               exit(EXIT_FAILURE);
            }

            cout << "---- Sudoku data file: " << *it << "\n\n";

            SudokuGrid sdkg;
            vector<SudokuGrid> solutions(5);

            Input >> sdkg;

            //SudokuSolver SdkSolver(Sdg, vm.count("first"));
            SudokuSolver sdkSolver(sdkg, 1);

            clock_t clockStart = std::clock();
            solutions = sdkSolver.solve();
            clock_t clockEnd = std::clock();
            cout << "---- Processing time: " << fixed << setprecision(3)
                 << double(clockEnd - clockStart) / CLOCKS_PER_SEC
                 << " sec" << endl << endl;

            cout << "---- Sudoku solution(s), " << solutions.size()
                 << " found:\n\n";
            for (size_t j = 0; j < solutions.size(); ++j)
            {
               cout << solutions[j] << endl;
            }
            cout << "- Sudoku solver, solved puzzle: " << *it << endl << endl;
         }
         catch(const exception& e)
         {
            clog << argv[0] << " " << e.what() << endl;
         }
         catch(...)
         {
            clog << "UNKNOWN EXCEPTION" << endl;
         }
      }
   }

   return 0;
}

//--------------------------------------------------------------- eof Sudoku.cpp
