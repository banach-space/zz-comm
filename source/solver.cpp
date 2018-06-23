//========================================================================
// FILE:
//      src/solver.cpp
//
// AUTHOR:
//      banach-space@github
//
// DESCRIPTION:
//      Solver for the Boggle game.
//
//  License: Apache License 2.0
//========================================================================

#include <algorithm>
#include <functional>
#include <iostream>
#include <solver.h>

using namespace Boggle;

namespace {

//--------------------------------------------------------------------
//  NAME:
//      GetPointsForWords()
//  DESCRIPTION:
//      For given input word calculates the corresponding number of points
//      (according to: https://en.wikipedia.org/wiki/Boggle).
//  INPUT:
//      The word to calculate the points for
//  OUTPUT:
//      Number of points
//--------------------------------------------------------------------
unsigned GetPointsForWord(std::string word) {
  unsigned points = 0;
  if (word.length() < 5)
    points = 1;
  else if (word.length() == 5)
    points = 2;
  else if (word.length() == 6)
    points = 3;
  else if (word.length() == 7)
    points = 5;
  else if (word.length() >= 8)
    points = 11;

  return points;
}
} // namespace

Solver::Solver(Boggle::Board &board, Boggle::Trie &dict,
               LetterCount q_num_letters, SolverAlgorithm algo)
    : bboard_(board), dict_(dict), number_of_points_(0),
      number_of_letters_per_q_cube_(q_num_letters), algorithm_(algo) {}

void Solver::Run() {
  // Reset the results
  number_of_points_ = 0;
  results_.clear();

  // Reset the dictionary
  dict_.ResetVisited();

  // Run the solver
  switch (algorithm_) {
  case SolverAlgorithm::kBasic:
    RunBasic();
    break;
  case SolverAlgorithm::kFast:
    RunFast();
    break;
  case SolverAlgorithm::kMultiThreaded:
    RunMultithreaded();
    break;
  }

  // Calculate the number of points
  for (auto word : results_)
    number_of_points_ += GetPointsForWord(word);
}

void Solver::RunBasic() {
  int n_rows = bboard_.GetHeight();
  int n_cols = bboard_.GetWidth();

  // This vector is used to keep track of cubes that have been visited.
  std::vector<std::vector<LetterStatus>> visited;
  for (int r = 1; r <= n_rows; r++) {
    std::vector<LetterStatus> row(n_cols, LetterStatus::kNotVisited);
    visited.emplace_back(row);
  }

  // Loop over all pieces on the board
  for (int r = 0; r < n_rows; r++)
    for (int c = 0; c < n_cols; c++) {
      std::string prefix("");
      FindWord(r, c, prefix, visited);
    }
}

void Solver::RunMultithreaded() {
  int n_rows = bboard_.GetHeight();
  int n_cols = bboard_.GetWidth();

  // Don't run on an empty board
  unsigned const length = n_rows * n_cols;
  if (!length)
    return;

  // Set-up for multithreaded execution
  unsigned const min_per_thread = 25;
  unsigned const max_threads = (length + min_per_thread - 1) / min_per_thread;
  unsigned const hardware_threads = std::thread::hardware_concurrency();
  unsigned const num_threads =
      std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
  unsigned const block_size = length / num_threads;

  std::vector<std::thread> threads(num_threads - 1);

  // Create a job for each thread
  unsigned start = 0;
  for (unsigned i = 0; i < (num_threads - 1); ++i) {
    threads[i] = std::thread(&Solver::RunFastRange, this, start / n_cols,
                             start % n_cols, block_size);
    start += block_size;
  }

  // The final set of pieces is for this thread
  RunFastRange(start / n_cols, start % n_cols, length - (start));

  // Join all threads
  std::for_each(threads.begin(), threads.end(),
                std::mem_fn(&std::thread::join));
}

void Solver::RunFastRange(unsigned row_start, unsigned col_start,
                          unsigned block_size) {
  unsigned n_rows = bboard_.GetHeight();
  unsigned n_cols = bboard_.GetWidth();

  // This vector is used to keep track of cubes that have been visited.
  thread_local std::vector<std::vector<LetterStatus>> visited;
  for (unsigned r = 0; r < n_rows; r++) {
    std::vector<LetterStatus> row(n_cols, LetterStatus::kNotVisited);
    visited.emplace_back(row);
  }

  // Loop over all pieces as specified by the input params
  unsigned count = 0, col = col_start, row = row_start;
  while (count < block_size) {
    std::string prefix("");
    std::vector<std::string> words;
    FindWordFast(row, col, prefix, visited, words, dict_.GetHead());
    UpdateResultsTS(words);
    count++;

    // Update column and row numbers (`col` and `row`), and remember to stay in
    // bounds!
    if ((col + 1) < n_cols)
      col++;
    else {
      col = 0;
      row++;
    }
  }

  // Since `visited` is static, it needs to be reallocated every time this
  // function is called (otherwise we could be using one visited for boards
  // with different dimensions).
  for (unsigned r = 0; r < n_rows; r++)
    visited[r].clear();
  visited.clear();
}

void Solver::RunFast() {
  int n_rows = bboard_.GetHeight();
  int n_cols = bboard_.GetWidth();

  // This vector is used to keep track of cubes that have been visited.
  static thread_local std::vector<std::vector<LetterStatus>> visited;
  for (int r = 0; r < n_rows; r++) {
    std::vector<LetterStatus> row(n_cols, LetterStatus::kNotVisited);
    visited.emplace_back(row);
  }

  // Loop over all pieces on the board
  for (int r = 0; r < n_rows; r++)
    for (int c = 0; c < n_cols; c++) {
      std::string prefix("");
      std::vector<std::string> words;
      FindWordFast(r, c, prefix, visited, words, dict_.GetHead());
      UpdateResultsTS(words);
    }

  // Since `visited` is static, it needs to be reallocated every time this
  // function is called (otherwise we could be using one visited for boards
  // with different dimensions).
  for (int r = 0; r < n_rows; r++)
    visited[r].clear();
  visited.clear();
}

void Solver::FindWord(int row, int col, std::string &prefix,
                      std::vector<std::vector<LetterStatus>> &visited) {
  // Make sure that this is a valid location to be visiting
  if (bboard_.OutOfBounds(row, col) ||
      (visited[row][col] == LetterStatus::kVisited))
    return;

  // Update prefix (add `current_letter`)
  char current_letter = bboard_.GetPiece(row, col);
  prefix += current_letter;

  // [Q] --> `qu`?
  if (((current_letter == 'q') || (current_letter == 'Q')) &&
      (number_of_letters_per_q_cube_ == LetterCount::kTwo))
    prefix += 'u';

  // If this is a valid word then add it to results_
  if (dict_.IsWord(prefix, true) && (prefix.length() >= 3)) {
    UpdateResults(prefix);
  }

  // If this is a valid prefix, visit all neighbours
  if (dict_.IsPrefix(prefix)) {
    visited[row][col] = LetterStatus::kVisited;
    FindWord(row - 1, col - 1, prefix, visited);
    FindWord(row - 1, col, prefix, visited);
    FindWord(row - 1, col + 1, prefix, visited);
    FindWord(row, col - 1, prefix, visited);
    FindWord(row, col + 1, prefix, visited);
    FindWord(row + 1, col - 1, prefix, visited);
    FindWord(row + 1, col, prefix, visited);
    FindWord(row + 1, col + 1, prefix, visited);

    visited[row][col] = LetterStatus::kNotVisited;
  }

  // Update prefix (remove `current_letter`)
  prefix.pop_back();
  if (((current_letter == 'q') || (current_letter == 'Q')) &&
      (number_of_letters_per_q_cube_ == LetterCount::kTwo))
    prefix.pop_back();

  return;
}

void Solver::FindWordFast(int row, int col, std::string &prefix,
                          std::vector<std::vector<LetterStatus>> &visited,
                          std::vector<std::string> &words,
                          TrieNode *current_head) {
  // Make sure that this is a valid location to be visiting
  if (bboard_.OutOfBounds(row, col) ||
      (visited[row][col] == LetterStatus::kVisited))
    return;

  // Update prefix (add `current_letter`)
  char current_letter = bboard_.GetPiece(row, col);
  prefix += current_letter;

  // Is this the [Qu] board?
  bool is_qu = false;

  // [Q] --> `qu`?
  if (((current_letter == 'q') || (current_letter == 'Q')) &&
      (number_of_letters_per_q_cube_ == LetterCount::kTwo)) {
    is_qu = true;
    prefix += 'u';
    current_letter = static_cast<char>(tolower(current_letter));
  }

  if (!is_qu) {
    // If this is a valid word then add it to results_
    if ((prefix.length() >= 3) &&
        dict_.IsLetterWord(current_letter, current_head, true)) {
      words.emplace_back(prefix);
    }
  } else {
    // current_letter is `q`. Check whether it's a prefix?
    if (!dict_.IsLetterPrefix(current_letter, current_head)) {
      // Remove `u`
      prefix.pop_back();
      // Remove `q`
      prefix.pop_back();
      return;
    } else {
      TrieNode *current_node = dict_.GetNode(current_letter, current_head);
      current_head = current_node;

      // [Qu] --> u (we are done dealing with `q`)
      current_letter = 'u';
      if ((prefix.length() >= 3) &&
          dict_.IsLetterWord(current_letter, current_head, true)) {
        words.emplace_back(prefix);
      }
    }
  }

  // If this is a valid prefix, visit all neighbours
  if (dict_.IsLetterPrefix(current_letter, current_head)) {
    TrieNode *current_node = dict_.GetNode(current_letter, current_head);
    if (current_node == nullptr) {
      return;
    }
    current_head = current_node;

    visited[row][col] = LetterStatus::kVisited;
    FindWordFast(row - 1, col - 1, prefix, visited, words, current_head);
    FindWordFast(row - 1, col, prefix, visited, words, current_head);
    FindWordFast(row - 1, col + 1, prefix, visited, words, current_head);
    FindWordFast(row, col - 1, prefix, visited, words, current_head);
    FindWordFast(row, col + 1, prefix, visited, words, current_head);
    FindWordFast(row + 1, col - 1, prefix, visited, words, current_head);
    FindWordFast(row + 1, col, prefix, visited, words, current_head);
    FindWordFast(row + 1, col + 1, prefix, visited, words, current_head);

    visited[row][col] = LetterStatus::kNotVisited;
  }

  // Update prefix (remove `current_letter`)
  prefix.pop_back();

  // Additional clean-up for [Qu] pieces
  if (is_qu) {
    // Remove `q`
    prefix.pop_back();
  }

  return;
}

void Solver::PrintResults() {
  for (auto word : results_)
    std::cout << word << std::endl;
}

unsigned Solver::GetNumberOfWords() { return results_.size(); }

unsigned Solver::GetNumberOfPoints() { return number_of_points_; }
