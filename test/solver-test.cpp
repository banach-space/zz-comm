//========================================================================
// FILE:
//		test/solver-test.cpp
//
// AUTHOR:
//		banach-space@github
//
// DESCRIPTION:
//    Unit tests for Solver
//
// License: Apache License 2.0
//========================================================================
#include <algorithm>
#include <common.h>
#include <set>
#include <string>
#include <tuple>
#include <utility>

using namespace Boggle;
//========================================================================
// TEST FIXTURES
//========================================================================
//------------------------------------------------------------------------
//  NAME:
//      SolverUnitTest
//
//  DESCRIPTION:
//      A gtest test fixture to help encapsulate what's common among tests in
//      this file. All other fixtures in this file inherit from this root
//      fixture.
//
//      See the official documentation for more details:
//      https://github.com/google/googletest/blob/master/googletest/docs/Primer.md#test-fixtures-using-the-same-data-configuration-for-multiple-tests
//------------------------------------------------------------------------
struct SolverUnitTest : public ::testing::Test {
  bool case_sensitive_ = true;
  const char *board_ = "AndrzejWarzynski";
  std::string dictionary_file_ = "./dictionaries/andrzej.txt";
  InputOrientation orientation_ = InputOrientation::kRowMajor;

  SolverUnitTest() {}
  ~SolverUnitTest() {}

protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

//========================================================================
// TESTS
//========================================================================
TEST_F(SolverUnitTest, EmptyBoard) {
  const char *empty_board = "";

  // Create an empty board
  Board boggle_board(empty_board, 0, 0, case_sensitive_, orientation_);

  // Create a non-empty Trie
  TrieBuilder trie_build(dictionary_file_.c_str());
  std::unique_ptr<Trie> trie = trie_build.Get();

  // Run the solver
  Solver solver(boggle_board, *trie, LetterCount::kTwo,
                SolverAlgorithm::kMultiThreaded);
  solver.Run();

  // Number of points should be 0
  EXPECT_EQ(0u, solver.GetNumberOfPoints());
}

TEST_F(SolverUnitTest, EmptyTrie) {
  // Create a non-empty board
  Board boggle_board(board_, 2, 8, case_sensitive_,
                     InputOrientation::kRowMajor);

  // Create an empty Trie (there will be an error message in stderr, which is
  // expected as there is no "" file.)
  TrieBuilder trie_build("");
  std::unique_ptr<Trie> trie = trie_build.Get();
  bool empty = trie->IsEmpty();
  EXPECT_EQ(true, empty);

  // Run the solver
  Solver solver(boggle_board, *trie, LetterCount::kTwo,
                SolverAlgorithm::kMultiThreaded);
  solver.Run();

  // Number of points should be 0
  EXPECT_EQ(0u, solver.GetNumberOfPoints());
}

TEST_F(SolverUnitTest, Basic) {
  // General set-up
  std::tuple<unsigned, std::pair<unsigned, unsigned>> test_cases[]{
      std::make_tuple(5, std::pair<unsigned, unsigned>(2, 8)),
      std::make_tuple(16, std::pair<unsigned, unsigned>(8, 2))};

  for (auto test_case : test_cases) {
    // Create the board
    Board boggle_board(board_, std::get<1>(test_case).first,
                       std::get<1>(test_case).second, case_sensitive_,
                       orientation_);

    // Create Trie
    TrieBuilder trie_build(dictionary_file_.c_str());
    std::unique_ptr<Trie> trie = trie_build.Get();

    // Run the solver
    Solver solver(boggle_board, *trie, LetterCount::kTwo,
                  SolverAlgorithm::kMultiThreaded);
    solver.Run();

    // Compare the results
    EXPECT_EQ(std::get<0>(test_case), solver.GetNumberOfPoints());
  }
}
