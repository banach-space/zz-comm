//========================================================================
// FILE:
//      src/boggle-api.cpp
//
// AUTHOR:
//      banach-space@github
//
// DESCRIPTION:
//      Implements the Boggle-API
//
//  License: Apache License 2.0
//========================================================================

#include <board.h>
#include <boggle-api.h>
#include <iostream>
#include <solver.h>
#include <string.h>
#include <trie.h>
#include <vector>

using namespace Boggle;

// A pointer to the dictionary. I'd rather avoid using global variables, but
// can't see how to achieve it with this API.
std::unique_ptr<Trie> pTrie;

void LoadDictionary(const char *path) {
  if (pTrie != nullptr) {
    std::cerr << "[boggle-api:] Failed to load a dictionary. Free the current "
                 "dictionary first!"
              << std::endl;
    return;
  }

  // Dictionary in the context of this implementation is equivalent to Trie
  TrieBuilder trie_build(path);
  pTrie = trie_build.Get();
}

void FreeDictionary() {
  // Delete/free the dictionary
  pTrie.reset();
  if (pTrie != nullptr)
    std::cerr << "[boggle-api:] Failed to free the dictionary." << std::endl;
}

Results FindWords(const char *board, unsigned width, unsigned height) {
  // Assumption: we don't care about the case
  bool case_sensitive = false;

  // Create the board
  Board boggle_board(board, height, width, case_sensitive,
                     InputOrientation::kRowMajor);

  // Create and run the solver
  Solver solver(boggle_board, *pTrie, LetterCount::kTwo,
                SolverAlgorithm::kMultiThreaded);
  solver.Run();

  // Copy the score from the solver into Results
  Results result;
  result.Count = solver.GetNumberOfWords();
  result.Score = solver.GetNumberOfPoints();

  // Copy the words from the solver into Results
  std::vector<std::string> words = solver.GetWords();

  char **pointerVec = new char *[result.Count];
  for (size_t i = 0; i < result.Count; i++) {
    // `+1` in the following is for the terminating NULL character.
    pointerVec[i] = new char[words[i].length() + 1];
    strncpy(pointerVec[i], words[i].c_str(), words[i].length() + 1);
  }
  result.Words = pointerVec;

  return result;
}

void FreeWords(Results results) {
  // Clear the words
  for (size_t i = 0; i < results.Count; i++)
    delete[] results.Words[i];

  delete[] results.Words;

  // Reset the remaining bits in results. This is rather pointless as results is
  // being passed by value.
  results.Words = nullptr;
  results.Count = 0;
  results.Score = 0;
}
