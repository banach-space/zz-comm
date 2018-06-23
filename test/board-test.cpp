//========================================================================
// FILE:
//		test/board-test.cpp
//
// AUTHOR:
//		banach-space@github
//
// DESCRIPTION:
//    Unit tests for Board
//
// License: Apache License 2.0
//========================================================================
#include <algorithm>
#include <common.h>
#include <set>
#include <string>
#include <utility>

using namespace Boggle;

TEST(Board, EmptyBoard) {
  // General set-up
  unsigned width = 0;
  unsigned height = 0;
  bool case_sensitive = true;
  const char *board = "dzxeaiqut";

  Board boggle_board(board, width, height, case_sensitive,
                     InputOrientation::kRowMajor);

  EXPECT_STRNE(boggle_board.GetBoard().c_str(), board);
  EXPECT_EQ(boggle_board.GetBoard(), "");
}

TEST(Board, RectangularBoard) {
  // General set-up
  unsigned width = 7;
  unsigned height = 1;
  bool case_sensitive = true;
  const char *board = "Andrzej";

  Board boggle_board(board, width, height, case_sensitive,
                     InputOrientation::kRowMajor);

  EXPECT_STREQ(boggle_board.GetBoard().c_str(), board);
  EXPECT_NE(boggle_board.GetBoard(), "");
}

TEST(Board, Constructors) {
  // General set-up
  unsigned width = 3;
  unsigned height = 3;
  bool case_sensitive = true;

  // All boards have identical characters, but:
  // * 1st is constructed with a C-string
  // * 2nd is constructed with a std::string
  // * 3rd is constructed with a std::string in caps
  const char *board = "dzxeaiqut";
  std::string board_str(board);
  std::string board_str_upper;
  std::transform(board_str.begin(), board_str.end(),
                 std::back_inserter(board_str_upper), ::toupper);

  // Create 3 different boards
  Board boggle_board(board, width, height, case_sensitive,
                     InputOrientation::kRowMajor);
  Board boggle_board_str(board_str, width, height, case_sensitive,
                         InputOrientation::kRowMajor);
  Board boggle_board_str_upper(board_str_upper, width, height, case_sensitive,
                               InputOrientation::kRowMajor);

  // Compare the pieces/boards
  EXPECT_STRNE(board_str_upper.c_str(), board);
  EXPECT_EQ(boggle_board.GetBoard(), boggle_board_str.GetBoard());
  EXPECT_NE(boggle_board.GetBoard(), boggle_board_str_upper.GetBoard());
}

TEST(Board, OutOfBounds) {
  // General set-up
  const char *board = "dzxeaiqut";
  bool case_sensitive = false;
  unsigned width = 3;
  unsigned height = 3;

  // Create the board
  Board boggle_board(board, width, height, case_sensitive,
                     InputOrientation::kRowMajor);

  // 1. OUT OF BOUNDS - TRUE
  bool out_of_bounds = false;
  std::set<std::pair<unsigned, unsigned>> test_cases_fail{
      {4, 1}, {44, 1}, {0, 3}, {100, 100}, {-1, 10}, {0, -424234}};

  for (auto item : test_cases_fail) {
    out_of_bounds = boggle_board.OutOfBounds(item.first, item.second);
    EXPECT_TRUE(out_of_bounds);
  }

  // 2. OUT OF BOUNDS - FALSE
  for (unsigned int r = 0; r < height; r++) {
    for (unsigned int c = 0; c < width; c++) {
      out_of_bounds = boggle_board.OutOfBounds(r, c);
      EXPECT_FALSE(out_of_bounds);
    }
  }
}

TEST(Board, Orientation) {
  // General set-up
  bool case_sensitive = false;
  unsigned width = 3;
  unsigned height = 3;

  // Same board, but encoded in row-major and column-major format
  std::string board_row_major("dzxeaiqut");
  std::string board_column_major("deqzauxit");

  Board boggle_board_row_major(board_row_major, width, height, case_sensitive,
                               InputOrientation::kRowMajor);
  Board boggle_board_column_major(board_column_major, width, height,
                                  case_sensitive,
                                  InputOrientation::kColumnMajor);

  // No matter what the format of the input string was
  // (row-major/column-major), the constructed boards should be identical.
  std::string str1 = boggle_board_row_major.GetBoard();
  std::string str2 = boggle_board_column_major.GetBoard();
  EXPECT_EQ(str1, str2);
}
