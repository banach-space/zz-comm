//========================================================================
// FILE:
//		test/integration-boggle-api-test.cpp
//
// AUTHOR:
//		banach-space@github
//
// DESCRIPTION:
//		Integration tests for Boggle-API. Every test in this file runs the
//    solver on a different board and verifies that the result is correct.
//
//		Test boards used in this file were obtained from internet, most of
//    which from:	http://coursera.cs.princeton.edu/algs4/assignments/boggle.html
//
// License: Apache License 2.0
//========================================================================
#include <common.h>

using namespace Boggle;

extern std::unique_ptr<Trie> pTrie;

//========================================================================
// TESTS
//========================================================================
TEST(BoggleIntegrationTestBoggleAPI, Basic) {
  BoggleResults results;

  results.LoadDictionary("./dictionaries/dictionary-yawl.txt");
  ASSERT_NE(pTrie, nullptr);

  results.FindWords("XEHEJLFVDERLIMMO", 4, 4);
  EXPECT_EQ(100u, results.score);
  EXPECT_EQ(78u, results.count);
  ASSERT_NE(nullptr, results.words);

  results.FreeWords();
  // The following check fails as `results` is being passed by value.
  // TODO Change the API
  // EXPECT_EQ(nullptr, results.Words);

  results.FreeDictionary();
  EXPECT_EQ(nullptr, pTrie);
}

TEST(BoggleIntegrationTestBoggleAPI, ReloadDictionary) {
  BoggleResults results;

  results.LoadDictionary("./dictionaries/dictionary-yawl.txt");
  results.FindWords("XEHEJLFVDERLIMMO", 4, 4);

  EXPECT_EQ(100u, results.score);
  EXPECT_EQ(78u, results.count);

  results.FreeWords();
  results.FreeDictionary();

  results.LoadDictionary("./dictionaries/dictionary-algs4.txt");
  results.FindWords("XEHEJLFVDERLIMMO", 4, 4);

  EXPECT_EQ(10u, results.score);
  EXPECT_EQ(10u, results.count);

  results.FreeWords();
  // The following check fails as `results` is being passed by value.
  // TODO Change the API
  // EXPECT_EQ(nullptr, results.Words);

  results.FreeDictionary();
  EXPECT_EQ(nullptr, pTrie);
}
