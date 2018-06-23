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
  // Load the dictionary
  LoadDictionary("./dictionaries/dictionary-yawl.txt");

  //-----------------------------------------------------
  // PROBLEM 1 (run twice)
  //-----------------------------------------------------
  // Run
  Results results = FindWords("XEHEJLFVDERLIMMO", 4, 4);

  // // Print the results (should be 100 and 78, respecitvely)
  std::cout << "Score: " << results.Score << std::endl;
  std::cout << "Count: " << results.Count << std::endl;

  // Clear the result
  FreeWords(results);

  // Print the results again (should be 0 and 0)
  std::cout << "Score: " << results.Score << std::endl;
  std::cout << "Count: " << results.Count << std::endl;

  // Run again
  results = FindWords("XEHEJLFVDERLIMMO", 4, 4);

  // Print the results (should be 100 and 78, respecitively)
  std::cout << "Score: " << results.Score << std::endl;
  std::cout << "Count: " << results.Count << std::endl;

  // Print the words
  for (size_t i = 0; i < results.Count; i++)
    printf("%s\n", results.Words[i]);

  FreeWords(results);
  FreeDictionary();

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

  LoadDictionary("./dictionaries/dictionary-yawl.txt");

  // Run
  results = FindWords(board.c_str(), 250, 250);

  // Print the results
  std::cout << "Score: " << results.Score << std::endl;
  std::cout << "Count: " << results.Count << std::endl;

  // Re-run
  FreeWords(results);
  results = FindWords(board.c_str(), 250, 250);

  // Print the results
  std::cout << "Score: " << results.Score << std::endl;
  std::cout << "Count: " << results.Count << std::endl;

  return 0;
}
