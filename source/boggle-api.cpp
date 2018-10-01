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
#include <cstring>
#include <trie.h>
#include <vector>

using namespace Boggle;

// A pointer to the dictionary. I'd rather avoid using global variables, but
// can't see how to achieve it with this API.
std::unique_ptr<Trie> pTrie;

void BoggleResults::LoadDictionary(const char *path) {
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

void BoggleResults::FreeDictionary() {
  // Delete/free the dictionary
  pTrie.reset();
  if (pTrie != nullptr)
    std::cerr << "[boggle-api:] Failed to free the dictionary." << std::endl;
}

void BoggleResults::FindWords(const char *board, unsigned width, unsigned height) {
  // Assumption: we don't care about the case
  bool case_sensitive = false;

  // Create the board
  Board boggle_board(board, height, width, case_sensitive,
                     InputOrientation::kRowMajor);

  // Create and run the solver
  Solver solver(boggle_board, *pTrie, LetterCount::kTwo,
                SolverAlgorithm::kMultiThreaded);
  solver.Run();

  // Copy the score from the solver into BoggleResults
  count = solver.GetNumberOfWords();
  score = solver.GetNumberOfPoints();

  // Copy the words from the solver into BoggleResults
  std::vector<std::string> words_temp = solver.GetWords();

  char **pointerVec = new char *[count];
  for (size_t i = 0; i < count; i++) {
    // `+1` in the following is for the terminating NULL character.
    pointerVec[i] = new char[words_temp[i].length() + 1];
    strncpy(pointerVec[i], words_temp[i].c_str(), words_temp[i].length() + 1);
  }

  words = pointerVec;
}

void BoggleResults::FreeWords() {
  // Clear the words
  for (size_t i = 0; i < count; i++)
    delete[] words[i];

  delete[] words;

  // Reset the remaining bits in results. This is rather pointless as results is
  // being passed by value.
  words = nullptr;
  count = 0;
  score = 0;
}
