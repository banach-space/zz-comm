//========================================================================
// FILE:
//		test/integration-boggle-api-test.cpp
//
// AUTHOR:
//		banach-space@github
//
// DESCRIPTION:
//		Integration tests for Boggle-API. Every test in this file runs the
//solver 		on a different board and verifies that the result is correct.
//
//		Test boards used in this file were obtained from internet, most of
//which 		from: 		http://coursera.cs.princeton.edu/algs4/assignments/boggle.html
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
  LoadDictionary("./dictionaries/dictionary-yawl.txt");
  ASSERT_NE(pTrie, nullptr);

  Results results = FindWords("XEHEJLFVDERLIMMO", 4, 4);

  EXPECT_EQ(100u, results.Score);
  EXPECT_EQ(78u, results.Count);
  ASSERT_NE(nullptr, results.Words);

  FreeWords(results);
  // The following check fails as `results` is being passed by value.
  // TODO Change the API
  // EXPECT_EQ(nullptr, results.Words);

  FreeDictionary();
  EXPECT_EQ(nullptr, pTrie);
}

TEST(BoggleIntegrationTestBoggleAPI, ReloadDictionary) {
  LoadDictionary("./dictionaries/dictionary-yawl.txt");
  Results results = FindWords("XEHEJLFVDERLIMMO", 4, 4);

  EXPECT_EQ(100u, results.Score);
  EXPECT_EQ(78u, results.Count);

  FreeWords(results);
  FreeDictionary();

  LoadDictionary("./dictionaries/dictionary-algs4.txt");
  results = FindWords("XEHEJLFVDERLIMMO", 4, 4);

  EXPECT_EQ(10u, results.Score);
  EXPECT_EQ(10u, results.Count);

  FreeWords(results);
  // The following check fails as `results` is being passed by value.
  // TODO Change the API
  // EXPECT_EQ(nullptr, results.Words);

  FreeDictionary();
  EXPECT_EQ(nullptr, pTrie);
}
