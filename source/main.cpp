//========================================================================
// FILE:
//      src/main.cpp
//
// AUTHOR:
//      banach-space@github
//
// DESCRIPTION:
//      This file implements the main() function for the Boggle binary. It's
//      meant to demonstrate how to use the Boggle API (provided by Unity).
//
//  License: Apache License 2.0
//========================================================================
#include <boggle-api.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

int main() {
  BoggleResults results;

  // Load the dictionary
  results.LoadDictionary("./dictionaries/dictionary-yawl.txt");

  //-----------------------------------------------------
  // PROBLEM 1 (run twice)
  //-----------------------------------------------------
  // Run
  results.FindWords("XEHEJLFVDERLIMMO", 4, 4);

  // // Print the results (should be 100 and 78, respecitvely)
  std::cout << "Score: " << results.score << std::endl;
  std::cout << "Count: " << results.count << std::endl;

  // Clear the result
  results.FreeWords();

  // Print the results again (should be 0 and 0)
  std::cout << "Score: " << results.score << std::endl;
  std::cout << "Count: " << results.count << std::endl;

  // Run again
  results.FindWords("XEHEJLFVDERLIMMO", 4, 4);

  // Print the results (should be 100 and 78, respecitively)
  std::cout << "Score: " << results.score << std::endl;
  std::cout << "Count: " << results.count << std::endl;

  // Print the words
  for (size_t i = 0; i < results.count; i++)
    printf("%s\n", results.words[i]);

  results.FreeWords();
  results.FreeDictionary();

  //-----------------------------------------------------
  // PROBLEM 2
  //-----------------------------------------------------
  // Load a very large board
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

  results.LoadDictionary("./dictionaries/dictionary-yawl.txt");

  // Run
  results.FindWords(board.c_str(), 250, 250);

  // Print the results
  std::cout << "Score: " << results.score << std::endl;
  std::cout << "Count: " << results.count << std::endl;

  // Re-run
  results.FreeWords();
  results.FindWords(board.c_str(), 250, 250);

  // Print the results
  std::cout << "Score: " << results.score << std::endl;
  std::cout << "Count: " << results.count << std::endl;

  return 0;
}
