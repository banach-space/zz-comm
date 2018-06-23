//========================================================================
// FILE:
//      src/profile.cpp
//
// AUTHOR:
//      banach-space@github
//
// DESCRIPTION:
//      This file implements the main() function for Boggle-profile. It is used
//      to profile various implementation of the solver.
//
//  License: Apache License 2.0
//========================================================================

#include <board.h>
#include <boggle-api.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <solver.h>
#include <stdio.h>
#include <string>
#include <trie.h>

using namespace std;
using namespace chrono;
using namespace Boggle;

int main() {
  //-----------------------------------------------------
  //  SET-UP
  //-----------------------------------------------------
  // Create Trie
  std::string dictionary_file = "./dictionaries/dictionary-yawl.txt";
  TrieBuilder trie_build(dictionary_file.c_str());
  std::unique_ptr<Trie> trie = trie_build.Get();

  // Board
  std::string file_path("./boards/250x250board.txt");
  std::ifstream input_file(file_path);
  std::string board;
  if (input_file.is_open()) {
    // Read the board
    getline(input_file, board);
    // Tidy up
    input_file.close();
  } else {
    std::cerr << "[main:] Failed to open the file: " << file_path << std::endl;
  }
  Board boggle_board(board.c_str(), 250, 250, false,
                     InputOrientation::kRowMajor);

  //-----------------------------------------------------
  //  SOLVER: Basic
  //-----------------------------------------------------
  // Run the solver
  Solver solver_basic(boggle_board, *trie, LetterCount::kTwo,
                      SolverAlgorithm::kBasic);
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  solver_basic.Run();
  high_resolution_clock::time_point t2 = high_resolution_clock::now();

  // Print the duration
  auto duration = duration_cast<milliseconds>(t2 - t1).count();
  std::cout << "Solver: BASIC" << std::endl;
  std::cout << "  Duration: " << duration << "ms" << std::endl;
  std::cout << "  Score: " << solver_basic.GetNumberOfPoints() << std::endl;

  //-----------------------------------------------------
  //  SOLVER: Fast
  //-----------------------------------------------------
  Solver solver_fast(boggle_board, *trie, LetterCount::kTwo,
                     SolverAlgorithm::kFast);
  t1 = high_resolution_clock::now();
  solver_fast.Run();
  t2 = high_resolution_clock::now();

  // Print the results
  duration = duration_cast<milliseconds>(t2 - t1).count();
  std::cout << "Solver: FAST" << std::endl;
  std::cout << "  Duration: " << duration << "ms" << std::endl;
  std::cout << "  Score: " << solver_fast.GetNumberOfPoints() << std::endl;

  //-----------------------------------------------------
  //  SOLVER: Multithreaded
  //-----------------------------------------------------
  Solver solver_mt(boggle_board, *trie, LetterCount::kTwo,
                   SolverAlgorithm::kMultiThreaded);
  t1 = high_resolution_clock::now();
  solver_mt.Run();
  t2 = high_resolution_clock::now();

  // Print the results
  duration = duration_cast<milliseconds>(t2 - t1).count();
  std::cout << "Solver: MULTITHREADED" << std::endl;
  std::cout << "  Duration: " << duration << "ms" << std::endl;
  std::cout << "  Score: " << solver_mt.GetNumberOfPoints() << std::endl;

  return 0;
}
