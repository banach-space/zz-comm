//========================================================================
//  FILE:
//      include/boggle.h
//
//  AUTHOR:
//      banach-space@github
//
//  DESCRIPTION:
//      Defines the API requested in the problem specification.
//
//      This file is a copy and paste from the problem specification. I didn't
//      add any comments.
//
//  License: Apache License 2.0
//========================================================================
//
#ifndef _BOGGLE_H_
#define _BOGGLE_H_

struct Results {
  const char *const *Words; // pointers to unique found words, each terminated
                            // by a non-alpha char
  unsigned Count;           // number of words found
  unsigned Score;           // total score
  void *UserData;           // ignored
};

// input dictionary is a file with one word per line
void LoadDictionary(const char *path); // << TODO
void FreeDictionary();                 // << TODO

// `board` will be exactly `width` * `height` chars, and char 'q' represents the
// 'qu' Boggle cube
Results FindWords(const char *board, unsigned width,
                  unsigned height); // << TODO
// `results` is identical to what was returned from `FindWords`
void FreeWords(Results results); // << TODO

#endif // #define _BOGGLE_H_
