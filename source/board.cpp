//========================================================================
// FILE:
//      src/board.cpp
//
// AUTHOR:
//      banach-space@github
//
// DESCRIPTION:
//      Board related data structures
//
//  License: Apache License 2.0
//========================================================================

#include <algorithm>
#include <board.h>
#include <iostream>
#include <string>

using namespace Boggle;

Board::Board(std::string board, unsigned height, unsigned width,
             bool case_sensitive, InputOrientation orientation)
    : height_(height), width_(width), case_sensitive_(case_sensitive) {

  // If we don't care about the case then make everything lower-case
  if (!case_sensitive_)
    std::transform(board.begin(), board.end(), board.begin(), ::tolower);

  // Loop over board_str and propagate this->pieces_ with letters.
  for (int r = 1; r <= height_; r++) {
    std::vector<char> row;
    row.reserve(width_);

    for (int c = 1; c <= width_; c++) {
      if (orientation == InputOrientation::kRowMajor)
        row.emplace_back(board[(r - 1) * width + c - 1]);
      else
        row.emplace_back(board[r - 1 + (c - 1) * height_]);
    }

    pieces_.emplace_back(row);
  }
}

Board::Board(const char *board, unsigned height, unsigned width,
             bool case_sensitive, InputOrientation orientation)
    : Board(std::string(board), height, width, case_sensitive, orientation) {}

void Board::Print() {
  for (auto row : pieces_) {
    std::cout << "|";

    for (auto &letter : row) {
      std::cout << letter << " ";
    }

    std::cout << '|' << std::endl;
  }
}

bool Board::OutOfBounds(int row, int col) {
  if ((row < 0) || (col < 0) || (row > (height_ - 1)) || (col > (width_ - 1)))
    return true;
  else
    return false;
}

char Board::GetPiece(int row, int col) {
  if (!OutOfBounds(row, col))
    return pieces_[row][col];
  else
    return -1;
}

std::string Board::GetBoard() {
  std::string output("");

  for (auto row : pieces_) {
    std::string temp(row.data(), row.size());
    output += temp;
  }

  return output;
}
