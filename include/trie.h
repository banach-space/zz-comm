//========================================================================
//  FILE:
//      include/Trie.h
//
//  AUTHOR:
//      banach-space@github
//
//  DESCRIPTION:
//      The Trie data structure and other supporting structures.
//
//  License: Apache License 2.0
//========================================================================
//
#ifndef _TRIE_H_
#define _TRIE_H_

#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace Boggle {

// Implementation of make_unique
//
// (Although make_unique is available in C++14, I didn't want to make any
// assumptions about the available compiler and instead defined make_unique
// here.  This definition was copied from Scott Meyers's "Effective Modern
// C++").
template <typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts &&... params) {
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

class Trie;
class TrieNode;

//========================================================================
// CLASS: TrieNode
//
// DESCRIPTION:
//      Implements a data structure that corresponds to a node in the Trie data
//      structure. Designed so that solving Boggle is easier.
//========================================================================
class TrieNode {
public:
  TrieNode() : word_(false), prefix_(false), visited_(false) {}

  friend Trie;

  std::map<char, std::unique_ptr<TrieNode>> children_;
  bool IsWord(bool check_if_visited); // { return word_; };
  bool IsPrefix() { return prefix_; };
  bool Visited() { return visited_; };
  void Reset();

private:
  // word_ is set to `true` if this TrieNode is the last letter in a valid word.
  // `false` otherwise.
  bool word_;
  // prefix_ is set to `true` if this TrieNode is part of a prefix (i.e. has
  // children). `false` otherwise.
  bool prefix_;
  // visited_ is set to `true` if you want to mark this TrieNode as "has already
  // been visited".
  bool visited_;
  std::mutex m_;
};

//========================================================================
// CLASS: Trie
//
// DESCRIPTION:
//      Implements the Trie data structure. This Trie can be either case-
//      sensitive or case-insensitive.
//
//      See Wikipedia for reference:
//        https://en.wikipedia.org/wiki/Trie
//========================================================================
class Trie {
public:
  explicit Trie(bool lower) : case_sensitive_(lower) {
    head_ = make_unique<TrieNode>();
  };
  virtual ~Trie(){};

  //--------------------------------------------------------------------
  //  NAME:
  //      BuildTrie()
  //
  //  DESCRIPTION:
  //      Loops over all input words and adds them to Trie
  //   INPUT:
  //      A vector of words to add.
  //  OUTPUT:
  //      None
  //--------------------------------------------------------------------
  void BuildTrie(std::vector<std::string> &words);

  //--------------------------------------------------------------------
  //  NAME:
  //      InsertWord()
  //
  //  DESCRIPTION:
  //      Adds a word to Trie
  //   INPUT:
  //      A word to be added to Trie
  //  OUTPUT:
  //      None
  //--------------------------------------------------------------------
  void InsertWord(std::string word);

  //--------------------------------------------------------------------
  //  NAME:
  //      IsWord()
  //
  //  DESCRIPTION:
  //      Every Trie represents a dictionary. This method checks whether the
  //      input word is in the dictionary corresponding to this Trie.
  //   INPUT:
  //      word - A word to be checked.
  //      check_if_visited - if `true`, then also check whether `word` has
  //      already been visited and return `false` even if it exists in the
  //      dictionary (this option was added for the solver)
  //  OUTPUT:
  //      `true` if the word is in the dictionary.
  //--------------------------------------------------------------------
  bool IsWord(std::string word, bool check_if_visited = false);

  //--------------------------------------------------------------------
  //  NAME:
  //      IsLetterWord()
  //
  //  DESCRIPTION:
  //      Checks if the node corresponding to the input letter (relative to
  //      current_head) is a word.
  //   INPUT:
  //      letter - Letter to check
  //      check_if_visited - if `true`, then also check whether `word` has
  //      already been visited and return `false` even if it exists in the
  //      dictionary (this option was added for the solver)
  //      current_head - the Trie node to check for
  //  OUTPUT:
  //      `true`if the node corresponding to this letter (relative to
  //      current_node_) is a word
  //--------------------------------------------------------------------
  bool IsLetterWord(char letter, TrieNode *current_head,
                    bool check_if_visited = false);

  //--------------------------------------------------------------------
  //  NAME:
  //      IsLetterPrefix()
  //
  //  DESCRIPTION:
  //      Checks if the node corresponding to the input letter (relative to
  //      current_head) is a prefix
  //   INPUT:
  //      letter - Letter to check
  //      current_head - the Trie node to check for
  //  OUTPUT:
  //      `true`if the node corresponding to this letter (relative to
  //      current_node_) corresponds to a word
  //--------------------------------------------------------------------
  bool IsLetterPrefix(char letter, TrieNode *current_head);

  //--------------------------------------------------------------------
  //  NAME:
  //      IsPrefix()
  //
  //  DESCRIPTION:
  //      Every Trie represents a dictionary. A prefix `p` belongs to a
  //      dictionary if there's at least one word in the dictionary for which
  //      `p` is a prefix.
  //  INPUT:
  //      A prefix to be checked.
  //  OUTPUT:
  //      None
  //--------------------------------------------------------------------
  bool IsPrefix(std::string prefix);

  //--------------------------------------------------------------------
  //  NAME:
  //      Print()
  //
  //  DESCRIPTION:
  //      Prints this Trie. It's a very basic method which prints one character
  //      in Trie after another.
  //      TODO Extend so that the printed output really depicts Trie
  //  INPUT:
  //      None
  //  OUTPUT:
  //      None
  //--------------------------------------------------------------------
  void Print();

  //--------------------------------------------------------------------
  //  NAME:
  //      ResetVisited()
  //  DESCRIPTION:
  //      Goes through all nodes and sets them as "not yet visited"
  //--------------------------------------------------------------------
  void ResetVisited();

  //--------------------------------------------------------------------
  //  NAME:
  //      GetNode()
  //
  //  DESCRIPTION:
  //      Get a pointer to the node corresponding to the input letter (relative
  //      to current_head_).
  //  INPUT:
  //      letter - letter for which to look for a node
  //  OUTPUT:
  //      Pointer to the node corresponding to the input character or nullptr.
  //--------------------------------------------------------------------
  TrieNode *GetNode(char letter, TrieNode *current_head);
  TrieNode *GetHead() { return head_.get(); };

  bool IsEmpty() { return head_->children_.empty(); };
  bool IsCaseSensitive() { return case_sensitive_; };

private:
  // There's only head_ per Trie and it's always the top node
  std::unique_ptr<TrieNode> head_;
  bool case_sensitive_;

  //--------------------------------------------------------------------
  //  NAME:
  //      PrintTree()
  //
  //  DESCRIPTION:
  //      Prints all characters from the input map. The structure of the map is
  //      identical to the one from TrieNode::children_. Calls PrintTree()
  //      iterative for all TrieNodes from the input map.
  //  INPUT:
  //      A map of children.
  //  OUTPUT:
  //      None
  //--------------------------------------------------------------------
  void PrintTree(std::map<char, std::unique_ptr<TrieNode>> const &tree);

  //--------------------------------------------------------------------
  //  NAME:
  //      ResetVisitedTree()
  //  DESCRIPTION:
  //      Goes through all nodes in the input map and sets them as "not yet
  //      visited"
  //  INPUT:
  //      A map of children.
  //--------------------------------------------------------------------
  void ResetVisitedTree(std::map<char, std::unique_ptr<TrieNode>> const &tree);
};

//========================================================================
// CLASS: TrieBuilder
//
// DESCRIPTION:
//    Encapsulates building a case-insensitive Trie.
//========================================================================
class TrieBuilder {
public:
  explicit TrieBuilder(std::vector<std::string> &words);
  explicit TrieBuilder(const char *file_path);
  explicit TrieBuilder();
  virtual ~TrieBuilder(){};
  bool IsEmpty() { return trie_->IsEmpty(); };

  std::unique_ptr<Trie> Get() { return std::move(trie_); }

private:
  std::unique_ptr<Trie> trie_;
};
} // namespace Boggle

#endif // #define _TRIE_H_
