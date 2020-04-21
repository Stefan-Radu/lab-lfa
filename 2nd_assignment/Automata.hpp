#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cassert>
#include <queue>
#include <map>

class Automata {
public:

  Automata(const std::string&);
  void testFile(const std::string&);

private:

  static const char lambda = '$';

  struct Transition {
    Transition(int y, char a): nextNode(y), transtionChar(a) {}
    int nextNode;
    char transtionChar;
  };

  int initialState;

  std::vector < bool > isFinalState;
  std::vector < std::vector < Transition > > transitions; 

  void initAutomata(const std::string&);
  void testString(const std::string&);
};
