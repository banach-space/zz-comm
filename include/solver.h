//========================================================================
//  FILE:
//      include/solver.h
//
//  AUTHOR:
//      banach-space@github
//
//  DESCRIPTION:
//      Implements the solver for Boggle game.
//
//  License: Apache License 2.0
//========================================================================
//
#ifndef _SOLVER_H_
#define _SOLVER_H_

#include <board.h>
#include <mutex>
#include <string>
#include <trie.h>
#include <vector>

namespace Boggle {

// This enum represents a max number of logical letter a printed letter can
// mean. It is used to specify whether a printed letter can correspond to
// multiple logic letters. Currently it used to identify whether:
//    [Q] <---> q (kOne)
//    [Q] <---> qu (kTwo)
//  This is required in order to map between what's being passed to the solver
//  and what the game rules are.
enum class LetterCount { kOne, kTwo };

// The status of a letter on the board.  A letter/cube/piece can only be used
// once per word. Hence, when searching for words each letter can be visited
// only once. This enum is used to mark a letter as visited (kVisited) or not
// yet visited (kNotVisited).
enum class LetterStatus { kVisited, kNotVisited };

// The solver algorithm:
//  * kBasic - for each piece on the board (i.e. potential new word) start
//             traversing the word from the root node
//  * kFast - traverse the dictionary together with the board
//  * kMultiThreaded - a multithreaded version of kFast
enum class SolverAlgorithm { kBasic, kFast, kMultiThreaded };

//========================================================================
// CLASS: Solver
//
// DESCRIPTION:
//    Implements a solver for the Boggle game. Each solver requiers a Boggle
//    board and a dictionary.
//
//    According to Wikipedia (https://en.wikipedia.org/wiki/Boggle), one cube
//    is special and contains two characters "Qu." For simplicity, Solver
//    assumes that only one character is printed on each cube. However, it is
//    possible to set-up the solver so that it will map/interpret [Q] as [Qu].
//========================================================================
class Solver {
public:
  explicit Solver(Boggle::Board &board, Boggle::Trie &dict,
                  LetterCount q_num_letters, SolverAlgorithm);

  //--------------------------------------------------------------------
  //  NAME:
  //      Run()
  //  DESCRIPTION:
  //      Runs the solver and propagate all the relevant internal data with
  //      the solution.
  //   INPUT:
  //      All the required data is passed in the constructor and no
  //      additional set-up is needed before running this method.
  //--------------------------------------------------------------------
  void Run();

  void RunBasic();
  void RunFast();
  void RunMultithreaded();

  void PrintResults();
  unsigned GetNumberOfWords();
  unsigned GetNumberOfPoints();
  std::vector<std::string> GetWords() { return results_; };
  LetterCount GetNumberOfLettersForQ();

private:
  // The board for the current game
  Boggle::Board &bboard_;
  // Dictionary for the current game
  Boggle::Trie &dict_;
  // A vector of words that have been found
  std::vector<std::string> results_;
  // Points corresponding to the words in results_
  unsigned number_of_points_;
  std::mutex m_points;
  std::mutex m_results;

  // Used to identify whether [Q] corresponds to 'q' or 'qu' (default is 'q')
  LetterCount number_of_letters_per_q_cube_;

  SolverAlgorithm algorithm_;

  //--------------------------------------------------------------------
  //  NAME:
  //      RunFastRange()
  //  DESCRIPTION:
  //      Runs RunFast() on the range specified as input.
  //   INPUT:
  //      row_start/col_start - the position on the board to start from
  //      (0-based)
  //      block_size - the number of board pieces to process
  //--------------------------------------------------------------------
  void RunFastRange(unsigned row_start, unsigned col_start,
                    unsigned block_size);

  void UpdateResults(std::string word) {
    std::lock_guard<std::mutex> lock_results(m_results);
    results_.emplace_back(word);
  }

  //--------------------------------------------------------------------
  //  NAME:
  //      UpdateResultsTS
  //  DESCRIPTION:
  //      Updates the results_ vector with words from the input vector. This
  //      implementation is thread-safe (TS).
  //--------------------------------------------------------------------
  void UpdateResultsTS(std::vector<std::string> &words) {
    std::lock_guard<std::mutex> lock_results(m_results);
    results_.insert(results_.end(), words.begin(), words.end());
  }

  //--------------------------------------------------------------------
  //  NAME:
  //      FindWord()
  //  DESCRIPTION:
  //      Looks for valid words on the board starting from the given
  //      location/cube/letter. Calls FindWord() recursively  for all its
  //      neighbours on the board.
  //   INPUT:
  //      row/col - the location on the board to start from (0-based)
  //      prefix - the prefix of the word to be found (can be an empty
  //               string)
  //  OUTPUT:
  //      Propagates relevant member variables with the words that have been
  //      found.
  //--------------------------------------------------------------------
  void FindWord(int row, int col, std::string &prefix,
                std::vector<std::vector<LetterStatus>> &pieces);

  //--------------------------------------------------------------------
  //  NAME:
  //      FindWordFast()
  //  DESCRIPTION:
  //      Looks for valid words on the board starting from the given
  //      location/cube/letter. Calls FindWordFast() recursively  for all its
  //      neighbours on the board. In prinicple, it's very similar to
  //      FindWord(), but it traverses the dictionary together with the board
  //      so that it doesn't have to search for whole words each time.
  //   INPUT:
  //      row/col - the location on the board to start from (0-based)
  //      prefix - the prefix of the word to be found (can be an empty
  //               string)
  //      pieces - a board for marking pieces as visited/not visited
  //      current_head - current head in Trie (used for traversing the
  //      dictionary)
  //  OUTPUT:
  //      Propagates relevant member variables with the words that have been
  //      found.
  //--------------------------------------------------------------------
  void FindWordFast(int row, int col, std::string &prefix,
                    std::vector<std::vector<LetterStatus>> &pieces,
                    std::vector<std::string> &words, TrieNode *current_head);
};
} // namespace Boggle

#endif // #define _SOLVER_H
