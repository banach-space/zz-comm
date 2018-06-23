//========================================================================
// FILE:
//		test/trie-test.cpp
//
// AUTHOR:
//		banach-space@github
//
// DESCRIPTION:
//    Unit tests for Trie
//
// License: Apache License 2.0
//========================================================================
#include <common.h>

using namespace Boggle;

TEST(Trie, EmptyTrie) {
  // Parameters
  std::vector<std::string> words = {};
  bool case_sensitive = false;

  // Build Trie
  Trie trie(case_sensitive);
  trie.BuildTrie(words);

  bool found = false;
  bool is_prefix = false;

  // A word that should exist in Trie
  found = trie.IsWord("");
  is_prefix = trie.IsPrefix("");
  EXPECT_FALSE(found);
  EXPECT_FALSE(is_prefix);
}

TEST(Trie, Basic) {
  // Parameters
  std::vector<std::string> words = {"foo"};
  bool case_sensitive = false;

  // Build Trie
  Trie trie(case_sensitive);
  trie.BuildTrie(words);

  bool found = false;
  bool is_prefix = false;

  // 1. A word that should exist in Trie
  found = trie.IsWord(words[0]);
  is_prefix = trie.IsPrefix(words[0]);
  EXPECT_TRUE(found);
  EXPECT_FALSE(is_prefix);

  // 2. A word that shouldn't exist in Trie
  found = trie.IsWord("Andrzej");
  is_prefix = trie.IsPrefix("Andrzej");
  EXPECT_FALSE(found);
  EXPECT_FALSE(is_prefix);
}

TEST(Trie, CreateTrieCaseInsensitive) {
  std::vector<std::string> words_set1 = {"foo", "BAR", "baz", "BARZ"};
  std::vector<std::string> words_set2 = {"FOO", "bar", "BAZ", "barz"};

  bool case_sensitive = false;
  Trie trie(case_sensitive);
  trie.BuildTrie(words_set1);

  bool found = false;
  for (auto word : words_set1) {
    found = trie.IsWord(word);
    EXPECT_TRUE(found) << "The word: " << word;
    ;
  }

  for (auto word : words_set2) {
    found = trie.IsWord(word);
    EXPECT_TRUE(found) << "The word: " << word;
  }
}

TEST(Trie, CreateTrieCaseSensitive) {
  std::vector<std::string> words_set1 = {"foo", "BAR", "baz", "BARZ"};
  std::vector<std::string> words_set2 = {"FOO", "bar", "BAZ", "barz"};

  bool case_sensitive = true;
  Trie trie(case_sensitive);
  trie.BuildTrie(words_set1);

  bool found = false;
  for (auto word : words_set1) {
    found = trie.IsWord(word);
    EXPECT_TRUE(found);
  }

  for (auto word : words_set2) {
    found = trie.IsWord(word);
    EXPECT_FALSE(found);
  }
}

TEST(Trie, TrieBuilderNoFile) {
  std::vector<std::string> words_lower_case = {"foo", "bar", "baz", "barz"};
  std::vector<std::string> words_upper_case = {"FOO", "BAR", "BAZ", "BARZ"};

  // Create an empty TrieBuilder (there will be an error message in stderr,
  // which is expected as there is no "" file.)
  TrieBuilder trie_build("");
  EXPECT_TRUE(trie_build.IsEmpty());

  std::unique_ptr<Trie> trie = trie_build.Get();
  EXPECT_TRUE(trie->IsEmpty());
}

TEST(Trie, TrieBuilderFile) {
  std::vector<std::string> words_lower_case = {"foo", "bar", "baz", "barz"};
  std::vector<std::string> words_upper_case = {"FOO", "BAR", "BAZ", "BARZ"};

  // TrieBuilder creates a case-insensitive Trie
  TrieBuilder trie_build("./dictionaries/data1.txt");
  std::unique_ptr<Trie> trie = trie_build.Get();

  bool found = false;
  for (auto word : words_upper_case) {
    found = trie->IsWord(word);
    EXPECT_TRUE(found);
  }

  for (auto word : words_lower_case) {
    found = trie->IsWord(word);
    EXPECT_TRUE(found);
  }
}
