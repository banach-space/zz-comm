zz-puzzle
========
[![Build Status](https://travis-ci.org/banach-space/zz-puzzle.svg?branch=add_all)](https://travis-ci.org/banach-space/zz-puzzle)
[![Build status](https://ci.appveyor.com/api/projects/status/uiy0j9afon9j7m09?svg=true)](https://ci.appveyor.com/project/banach-space/zz-puzzle)

A solver for the [Boggle game](https://en.wikipedia.org/wiki/Boggle). More
specifically, 3 different solvers (2 single threaded and one multithreaded), a
test suite, integration tests and example code showing how to run, benchmark,
etc.

Status
--------
**WORK-IN-PROGRESS**

## Platform Support
**zz-puzzle** supports Windows and Linux. It was tested with the following setups:
  * Windows 7 + VS 2015 + CMake 3.4.3
  * Linux 4.11.2 + clang-4.0/GCC-6.3 + CMake 3.4.3

## Build
It is assumed that **zz-puzzle** will be built in \<build-dir\> and that the
top-level source code directory is \<source-dir\>.

### Windows
First, configure the build (it is assumed that Developer's Command Prompt is
used so that all required build tools are already in the path):

```
  mkdir <build-dir>
  cd <build-dir>
  cmake <source-dir>
```

Then you can build (change `Release` to `Debug` if you want to build in debug
mode):

```
  cd <build-dir>
  cmake --build . --config Release
```
### Linux
First, configure the build (`Release` should be replaced by `Debug` to build in
debug mode):

```
  mkdir <build-dir>
  cd <build-dir>
  cmake -DCMAKE_BUILD_TYPE=Release <source-dir>
```

Then you can build
```
  cd <build-dir>
  cmake --build .
```

## Implementation details
The implementation is split into 4 major components: Board, Solver,
Trie/Dictionary and the Boggle-API. The dictionary was implemented with the aid
of a Trie data structure and hence it is usually referred to as Trie.
Boggle-API is a wrapper for Board/Solver/Trie and implements the API requested
in the problem specification. 

For code formatting Google's C++ coding convention was used:
https://google.github.io/styleguide/cppguide.html 

## Usage
The build script generates 3 binary objects (for brevity, using Windows names
with the *.exe extension):
  * `Boggle-UnitTest.exe` (executable, runs all unit and integration tests)
  * `Boggle-profile.exe` (executable, compares execution of all the
    implementations of the solver)
  * `Boggle.exe` (executable, runs a sample game, see /<source-dir/>\source\main.cpp
    for implementation)
  * `BoggleSolution.lib` (static library, implements the
    Boggle API requested in the problem specification)
The easiest way to start is to run Boggle-UnitTest (there are 19 tests and all
of them *pass* on both Linux and Windows). Here's how you can run it on Windows:
```
  cd <build-dir>
  Release\Boggle-UnitTest.exe
```

As the next step you can run your own game with libBoggleSolution.a. Check
/<source-dir/>/source/main.cpp for a sample implementation. You can also run
`Boggle.exe` to see it in action.
```
  cd <build-dir>
  Boggle.exe
  Release\Boggle.exe
```

NOTE: I assumed that both Boggle.exe and Boggle-UnitTest.exe are executed from
the /<build/> directory. That's for simplicity so that the binaries can locate
the dictionaries that I've attached. 

## Tests
All tests were implemented with the aid of Google Test
[here](https://github.com/google/googletest), and are located in the
/<source-dir/>/test directory. There are unit tests for Board (board-test.cpp),
Solver (solver-test.cpp) and Trie (trie-test.cpp), and integration tests for
all of them (integration-test.cpp). There are also integration tests for
Boggle-API (integration-boggle-api-test.cpp).
