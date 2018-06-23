//========================================================================
//  FILE:
//      include/board.h
//
//  AUTHOR:
//      banach-space@github
//
//  DESCRIPTION:
//      Data structures related to the boggle game board.
//
//  License: Apache License 2.0
//========================================================================
//
#ifndef _BOARD_H_
#define _BOARD_H_

#include <string>
#include <vector>

namespace Boggle {

// This enum represents the format of the input C-string that represents the
// game board. Two options are implemented: row-major, and column-major. Check
// Wikipedia for reference:
// https://en.wikipedia.org/wiki/Row-_and_column-major_order
enum class InputOrientation { kRowMajor, kColumnMajor };

//========================================================================
// CLASS: Board
//
// DESCRIPTION:
//    Represents a Boggle game board. The letters can be either case-sensitive
//    or case-insensitive. The input C-string containing the board can be either
//    in row-major or column-major format.
//========================================================================
class Board {
public:
  explicit Board(const char *pieces, unsigned height, unsigned width,
                 bool case_sensitive, InputOrientation orientation);
  explicit Board(std::string board, unsigned height, unsigned width,
                 bool case_sensitive, InputOrientation orientation);

  //--------------------------------------------------------------------
  //  NAME:
  //      Print()
  //  DESCRIPTION:
  //      Prints the board in matrix form
  //--------------------------------------------------------------------
  void Print();
  int GetHeight() { return height_; }
  int GetWidth() { return width_; }

  //--------------------------------------------------------------------
  //  NAME:
  //      OutOfBounds()
  //  DESCRIPTION:
  //      Checks whether the input coordinates are out of bounds for this
  //      board.
  //   INPUT:
  //      Board location (row number and column number) to check
  //   OUTPUT:
  //      Return `true` if indeed the input location is outside the board.
  //      `true` otherwise.
  //--------------------------------------------------------------------
  bool OutOfBounds(int row, int col);
  char GetPiece(int row, int col);
  char IsCaseSensitive() { return case_sensitive_; };
  std::string GetBoard();

private:
  // Board pieces/letters. This is stored as vector of rows (i.e. row-major
  // format)
  std::vector<std::vector<char>> pieces_;
  int height_;
  int width_;
  // Is this board case-sensitive?
  bool case_sensitive_;
};

} // namespace Boggle

#endif // #define _BOARD_H_
