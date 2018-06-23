cmake_minimum_required(VERSION 3.4.3 FATAL_ERROR)

project(Boggle)

add_executable(Boggle-UnitTest
  ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/src/gtest-all.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/test/trie-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/board-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/solver-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/integration-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/integration-boggle-api-test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/trie.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/solver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/board.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/boggle-api.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/test/test-main.cpp)

add_executable(Boggle
  ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp)

add_executable(Boggle-profile
  ${CMAKE_CURRENT_SOURCE_DIR}/source/trie.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/solver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/board.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/profile.cpp)


add_library(BoggleSolution STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/source/trie.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/solver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/board.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/boggle-api.cpp)

target_include_directories(Boggle-UnitTest PRIVATE SYSTEM
  ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/
  ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/include
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(SUBDIRS_TO_COPY dictionaries boards)

foreach(directory ${SUBDIRS_TO_COPY})
  add_custom_command(TARGET Boggle-UnitTest POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_directory
                     ${CMAKE_SOURCE_DIR}/test/${directory}
                     ${CMAKE_CURRENT_BINARY_DIR}/${directory})
endforeach()

if(UNIX)
  target_link_libraries(Boggle-UnitTest PRIVATE pthread)
  target_link_libraries(BoggleSolution PRIVATE pthread)
  target_link_libraries(Boggle-profile PRIVATE pthread)
  target_link_libraries(Boggle BoggleSolution pthread)
endif()

target_link_libraries(Boggle BoggleSolution)

set(executables Boggle Boggle-profile Boggle-UnitTest)

# Setup compiler definitions and flags.
set(Boggle_COMPILER_DEFINITIONS_MSVC
  _CRT_SECURE_NO_WARNINGS)

target_compile_definitions(Boggle PRIVATE
  "$<$<CXX_COMPILER_ID:MSVC>:${Boggle_COMPILER_DEFINITIONS_MSVC}>")
target_compile_definitions(Boggle-profile PRIVATE
  "$<$<CXX_COMPILER_ID:MSVC>:${Boggle_COMPILER_DEFINITIONS_MSVC}>")
target_compile_definitions(Boggle-UnitTest PRIVATE
  "$<$<CXX_COMPILER_ID:MSVC>:${Boggle_COMPILER_DEFINITIONS_MSVC}>")
target_compile_definitions(BoggleSolution PRIVATE
  "$<$<CXX_COMPILER_ID:MSVC>:${Boggle_COMPILER_DEFINITIONS_MSVC}>")

set(MSVC_WARNING_FLAGS
  -wd4267 # Silence warning C4267: 'initializing': conversion from 'size_t' to 'int', possible loss of data
  # TODO Remove
)

set(Boggle_COMPILER_OPTIONS_GNU -Wall -Werror -std=c++11 -fPIC)
set(Boggle_COMPILER_OPTIONS_CLANG -g -Wall -Werror -std=c++11 -fPIC
  -fcolor-diagnostics )
set(Boggle_COMPILER_OPTIONS_MSVC /W4 /WX ${MSVC_WARNING_FLAGS})

target_compile_options(Boggle-UnitTest PRIVATE
  "$<$<CXX_COMPILER_ID:GNU>:${Boggle_COMPILER_OPTIONS_GNU}>"
  "$<$<CXX_COMPILER_ID:Clang>:${Boggle_COMPILER_OPTIONS_CLANG}>"
  "$<$<CXX_COMPILER_ID:MSVC>:${Boggle_COMPILER_OPTIONS_MSVC}>")

target_compile_options(Boggle PRIVATE
  "$<$<CXX_COMPILER_ID:GNU>:${Boggle_COMPILER_OPTIONS_GNU}>"
  "$<$<CXX_COMPILER_ID:Clang>:${Boggle_COMPILER_OPTIONS_CLANG}>"
  "$<$<CXX_COMPILER_ID:MSVC>:${Boggle_COMPILER_OPTIONS_MSVC}>")

target_compile_options(Boggle-profile PRIVATE
  "$<$<CXX_COMPILER_ID:GNU>:${Boggle_COMPILER_OPTIONS_GNU}>"
  "$<$<CXX_COMPILER_ID:Clang>:${Boggle_COMPILER_OPTIONS_CLANG}>"
  "$<$<CXX_COMPILER_ID:MSVC>:${Boggle_COMPILER_OPTIONS_MSVC}>")

target_compile_options(BoggleSolution PRIVATE
  "$<$<CXX_COMPILER_ID:GNU>:${Boggle_COMPILER_OPTIONS_GNU}>"
  "$<$<CXX_COMPILER_ID:Clang>:${Boggle_COMPILER_OPTIONS_CLANG}>"
  "$<$<CXX_COMPILER_ID:MSVC>:${Boggle_COMPILER_OPTIONS_MSVC}>")
