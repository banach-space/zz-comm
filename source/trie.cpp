//========================================================================
// FILE:
//      src/trie.cpp
//
// AUTHOR:
//      banach-space@github
//
// DESCRIPTION:
//      Trie related data structures
//
//  License: Apache License 2.0
//========================================================================

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <trie.h>
#include <vector>

using namespace std;
using namespace Boggle;

bool TrieNode::IsWord(bool check_if_visited) {
  std::lock_guard<std::mutex> lock(m_);

  if (!word_)
    return false;

  // We *don't care* whether this node has been visited.
  if (!check_if_visited)
    return true;

  // We *do care* whether this node has been visited.
  if (word_ && visited_) {
    return false;
  } else {
    visited_ = true;
    return true;
  }
}

void TrieNode::Reset() {
  std::lock_guard<std::mutex> lock(m_);
  visited_ = false;
}

void Trie::BuildTrie(std::vector<std::string> &words) {
  for (auto word : words) {
    InsertWord(word);
  }
}

void Trie::InsertWord(std::string word) {

  // If we don't care about the case, then transform `word` to lower case.
  if (!case_sensitive_)
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);

  // `current_head` points to a TrieNode that we are currently visiting.
  TrieNode *current_head = head_.get();

  // Helper variables
  map<char, std::unique_ptr<TrieNode>>::iterator it;
  int length = word.length();
  char word_letter = 0;
  int index = 0;

  // Loop over the part of `word` (i.e. its prefix) that's already in Trie (we
  // neither want nor need to add the corresponding characters).
  for (index = 0; index < length; index++) {
    word_letter = word[index];

    if ((it = current_head->children_.find(word_letter)) !=
        current_head->children_.end()) {
      current_head = it->second.get();
    } else
      break;
  }

  // At this point current_head corresponds to the last character in `word`
  // that's already present in Trie. If we haven't exhausted all the characters
  // from `word` than the corresponding TrieNode is part of a prefix (as other
  // words from `word` will follow it).
  if (index < length)
    current_head->prefix_ = true;

  // Loop over the remaining characters in `word` and add them to Trie.
  for (; index < length; index++) {
    word_letter = word[index];
    std::unique_ptr<TrieNode> new_node = Boggle::make_unique<TrieNode>();

    // Each character that we add to Trie is either a part of a prefix (when
    // `word_letter` *is* to be followed by other letters), or the last letter
    // in
    // `word` (*is not* followed by other letters).
    if (index == (length - 1)) {
      new_node->word_ = true;
    } else
      new_node->prefix_ = true;

    // Add `new_node` to children_ of the current head
    current_head->children_[word_letter] = std::move(new_node);
    // Update the current head
    current_head = current_head->children_[word_letter].get();
  }
}

bool Trie::IsWord(string word, bool check_if_visited) {
  if (head_->children_.empty())
    return false;

  // If we don't care about the case, then transofrm `word` to lower case.
  if (!case_sensitive_) {
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
  }

  TrieNode *current_head = head_.get();
  map<char, std::unique_ptr<TrieNode>>::iterator it;

  for (size_t i = 0; i < word.length(); ++i) {
    if ((it = current_head->children_.find(word[i])) ==
        current_head->children_.end()) {
      return false;
    }
    current_head = it->second.get();
  }

  // If it's not a word then return
  return (it->second->IsWord(check_if_visited));
}

TrieNode *Trie::GetNode(char ch, TrieNode *current_head) {
  // If we don't care about the case, then transofrm `word` to lower case.
  if (!case_sensitive_)
    ch = static_cast<char>(tolower(ch));

  map<char, std::unique_ptr<TrieNode>>::iterator it;

  if ((it = current_head->children_.find(ch)) ==
      current_head->children_.end()) {
    return nullptr;
  }

  return it->second.get();
}

bool Trie::IsLetterWord(char letter, TrieNode *current_head,
                        bool check_if_visited) {
  if (current_head->children_.empty())
    return false;

  // If we don't care about the case, then transofrm `word` to lower case.
  if (!case_sensitive_)
    letter = static_cast<char>(tolower(letter));

  map<char, std::unique_ptr<TrieNode>>::iterator it;

  if ((it = current_head->children_.find(letter)) ==
      current_head->children_.end()) {
    return false;
  }

  return (it->second->IsWord(check_if_visited));
}

bool Trie::IsLetterPrefix(char letter, TrieNode *current_head) {
  if (current_head->children_.empty())
    return false;

  // If we don't care about the case, then transofrm `word` to lower case.
  if (!case_sensitive_)
    letter = static_cast<char>(tolower(letter));

  map<char, std::unique_ptr<TrieNode>>::iterator it;
  if ((it = current_head->children_.find(letter)) ==
      current_head->children_.end()) {
    return false;
  }

  if (it->second->prefix_)
    return true;
  else
    return false;
}

void Trie::ResetVisited() { ResetVisitedTree(head_->children_); }

void Trie::ResetVisitedTree(map<char, std::unique_ptr<TrieNode>> const &tree) {
  if (tree.empty()) {
    return;
  }

  for (auto it = tree.begin(); it != tree.end(); ++it) {
    it->second->Reset();
    ResetVisitedTree(it->second->children_);
  }
}

bool Trie::IsPrefix(string prefix) {
  if (head_->children_.empty())
    return false;

  // If we don't care about the case, then transofrm `word` to lower case.
  if (!case_sensitive_)
    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

  TrieNode *current_head = head_.get();
  map<char, std::unique_ptr<TrieNode>>::iterator it;

  for (size_t i = 0; i < prefix.length(); ++i) {
    if ((it = current_head->children_.find(prefix[i])) ==
        current_head->children_.end()) {
      return false;
    }
    current_head = it->second.get();
  }

  if (it->second->prefix_)
    return true;
  else
    return false;
}

void Trie::PrintTree(map<char, std::unique_ptr<TrieNode>> const &tree) {
  if (tree.empty()) {
    return;
  }

  for (auto it = tree.begin(); it != tree.end(); ++it) {
    std::cout << it->first << std::endl;
    PrintTree(it->second->children_);
  }
}

void Trie::Print() { PrintTree(head_->children_); }

TrieBuilder::TrieBuilder() : trie_(nullptr) {}

TrieBuilder::TrieBuilder(std::vector<std::string> &words) {
  trie_ = Boggle::make_unique<Trie>(false);

  for (auto word : words) {
    trie_->InsertWord(word);
  }
}

TrieBuilder::TrieBuilder(const char *file_path) {
  trie_ = Boggle::make_unique<Trie>(false);
  std::ifstream input_file(file_path);
  if (input_file.is_open()) {

    // Read the samples into the "samples" vector
    std::string line;
    while (std::getline(input_file, line)) {
      trie_->InsertWord(line);
    }

    // Tidy up
    input_file.close();
  } else {
    std::cerr << "[trie:] Failed to open the file: " << file_path << std::endl;
  }
}
