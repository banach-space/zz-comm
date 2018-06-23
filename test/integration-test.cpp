//========================================================================
// FILE:
//		test/integration-test.cpp
//
// AUTHOR:
//		banach-space@github
//
// DESCRIPTION:
//		Integration tests for Boggle. Every test in this file runs the
//solver on 		a different board and verifies that the result is correct. All tests
//use 		the same dictionary.
//
//		Test boards used in this file were obtained from internet, most of
//which 		from: 		http://coursera.cs.princeton.edu/algs4/assignments/boggle.html
//
// License: Apache License 2.0
//========================================================================
#include <common.h>

using namespace Boggle;

//========================================================================
// UTILITIES
//========================================================================
struct GameBoard {
  std::string pieces_;
  unsigned width_;
  unsigned height_;
  GameBoard(std::string board, unsigned w, unsigned h)
      : pieces_(board), width_(w), height_(h) {}
};

//========================================================================
// TEST FIXTURES
//========================================================================
//------------------------------------------------------------------------
//  NAME:
//      BoggleIntegrationTest
//
//  DESCRIPTION:
//      A gtest test fixture to help encapsulate what's common among tests in
//      this file. All other fixtures in this file inherit from this root
//      fixture.
//
//      See the official documentation for more details:
//      https://github.com/google/googletest/blob/master/googletest/docs/Primer.md#test-fixtures-using-the-same-data-configuration-for-multiple-tests
//------------------------------------------------------------------------
struct BoggleIntegrationTest : public ::testing::Test {
  bool case_sensitive_ = false;
  std::string dictionary_file_ = "./dictionaries/dictionary-yawl.txt";
  std::map<unsigned, GameBoard> test_boards_ = {};

  // This function will loop over all GameBoards in test_boards_ and run the
  // Solver for them.
  void RunTest(SolverAlgorithm algo) {
    for (auto test_case : test_boards_) {
      // Create Boggle board
      Board boggle_board(test_case.second.pieces_.c_str(),
                         test_case.second.height_, test_case.second.width_,
                         case_sensitive_, InputOrientation::kRowMajor);

      // Create Trie
      TrieBuilder trie_build(dictionary_file_.c_str());
      std::unique_ptr<Trie> trie = trie_build.Get();

      // Run the solver
      Solver solver_basic(boggle_board, *trie, LetterCount::kTwo, algo);
      solver_basic.Run();

      // Compare the results
      EXPECT_EQ(test_case.first, solver_basic.GetNumberOfPoints());
    }
  }

  BoggleIntegrationTest() {}
  ~BoggleIntegrationTest() {}

protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

struct BoggleIntegrationTest_TinyBoards : public BoggleIntegrationTest {
  virtual void SetUp() override {
    test_boards_ = {
        {0, {"", 0, 0}},
        {0, {"D", 1, 1}},
        {0, {"RSCL", 2, 2}},
    };
  };
};

struct BoggleIntegrationTest_LargeBoards : public BoggleIntegrationTest {
  virtual void SetUp() override {
    test_boards_ = {
        {26539, {"DSRODGTEMENSRASITODGNTRPREIAESTSCLPD", 6, 6}},
        {13464, {"RSCLSDEIAEGNTRPIAESOLMIDC", 5, 5}},
    };
  };
};

struct BoggleIntegrationTest_RegularBoards : public BoggleIntegrationTest {
  virtual void SetUp() override {
    test_boards_ = {
        {100, {"XEHEJLFVDERLIMMO", 4, 4}},  {200, {"HOCNERXVDTNERIOF", 4, 4}},
        {300, {"EEVEBTRDTSRATMAT", 4, 4}},  {400, {"ESAILTCCTGAHESIR", 4, 4}},
        {500, {"TASLRSNGLAIDGUHO", 4, 4}},  {750, {"ENNETOSDSERLIPNA", 4, 4}},
        {1000, {"TSMENOSNERETAPLA", 4, 4}}, {1111, {"ESILTHESARTNIDEO", 4, 4}},
        {1250, {"RDCESEOSIPRTTIAR", 4, 4}}, {1500, {"PISTSEEANERRDTCO", 4, 4}},
        {2000, {"LINSTAEGESLORESC", 4, 4}}, {4410, {"STNGEIAEDRLSSEPO", 4, 4}},
        {4527, {"SERSPATGLINESERS", 4, 4}}, {4540, {"GNESSRIPETALTSEB", 4, 4}},
    };
  };
};

struct BoggleIntegrationTest_BoardsWithQ : public BoggleIntegrationTest {
  virtual void SetUp() override {
    test_boards_ = {
        {777, {"ASSQRTOGENAADRDP", 4, 4}},
        {255, {"HEGQGONUNDOOARER", 4, 4}},
    };
  };
};

//========================================================================
// TESTS
//========================================================================
TEST_F(BoggleIntegrationTest_TinyBoards, CheckSolver) {
  RunTest(SolverAlgorithm::kBasic);
  RunTest(SolverAlgorithm::kFast);
  RunTest(SolverAlgorithm::kMultiThreaded);
}

TEST_F(BoggleIntegrationTest_LargeBoards, CheckSolver) {
  RunTest(SolverAlgorithm::kBasic);
  RunTest(SolverAlgorithm::kFast);
  RunTest(SolverAlgorithm::kMultiThreaded);
}

TEST_F(BoggleIntegrationTest_RegularBoards, CheckSolver) {
  RunTest(SolverAlgorithm::kBasic);
  RunTest(SolverAlgorithm::kFast);
  RunTest(SolverAlgorithm::kMultiThreaded);
}

TEST_F(BoggleIntegrationTest_BoardsWithQ, CheckSolver) {
  RunTest(SolverAlgorithm::kBasic);
  RunTest(SolverAlgorithm::kFast);
  RunTest(SolverAlgorithm::kMultiThreaded);
}
