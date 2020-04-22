#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <queue>
#include <map>
#include <set>

class Automata {
public:

  Automata(const std::string&);

  void testFile(const std::string&);

  Automata* nfaFromLnfa();

private:

  static const char lambda = '$';

  struct Transition {
    Transition(int, char);

    int state;
    char character;

    bool operator < (const Transition&) const;
    bool operator == (const Transition&) const;
  };

  int initialState;
  int stateCount, transitionCount;

  std::set < char > alphabet;
  std::vector < bool > isFinalState;
  std::vector < std::vector < Transition > > transitions; 
  std::vector < std::vector < Transition > > lambdaTransitions; 


  void initAutomata(const std::string&);
  void testString(const std::string&);

  void makeLamdaClosure(int, std::vector < std::set < int > >&, std::vector < bool >&);
};
