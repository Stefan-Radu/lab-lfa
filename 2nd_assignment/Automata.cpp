#include "Automata.hpp"
#include "Dsu.hpp"

// ================================= TRANSITION STRUCT ==================================={{{

Automata::Transition::Transition(int y, char a): state(y), character(a) {}

bool Automata::Transition::operator < (const Transition& other) const {
  if (character == other.character) return state < other.state;
  return character < other.character;
}

bool Automata::Transition::operator == (const Transition& other) const {
  return state == other.state and character == other.character;
}

/*}}}*/

// ================================= INITIALIZATION ==================================={{{

Automata::Automata(const std::string &inputFileName) {
  initAutomata(inputFileName);
}

void Automata::initAutomata(const std::string &inputFileName) {

  std::ifstream fin(inputFileName);
  assert(fin.is_open());

  // read number of states
  fin >> stateCount;

  isFinalState.resize(stateCount);
  transitions.resize(stateCount);
  lambdaTransitions.resize(stateCount);

  // nr of characters in the alphabet;
  int charCnt; fin >> charCnt;
  for (int i = 0; i < charCnt; ++ i) {
    char k; fin >> k;
    alphabet.insert(k);
  }

  // nr of initial states
  fin >> initialState;

  // nr of final states;
  fin >> finalStateCount;
  for (int i = 0; i < finalStateCount; ++ i) {
    int k; fin >> k;
    isFinalState[k] = true;
  }

  // nr of transitions;
  fin >> transitionCount;
  for (int i = 0; i < transitionCount; ++ i) {

    int x, y; char a;
    fin >> x >> a >> y;

    assert(x >= 0 and x < stateCount);
    assert(y >= 0 and y < stateCount); 
    assert(alphabet.find(a) != alphabet.end() or a == '$');

    if (a == '$') lambdaTransitions[x].emplace_back(y, a);
    else transitions[x].emplace_back(y, a);
  }

  fin.close();
}
/*}}}*/

// ================================= TESTING WORDS ==================================={{{

void Automata::testFile(const std::string &fileName) {
  std::ifstream fin(fileName);
  assert(fin.is_open());

  std::string s;
  while (fin >> s) {
    testString(s);
  }
}

void Automata::testString(const std::string &s) {
  std::queue < std::pair < int, int > > q;
  std::map < std::pair < int, int >, bool > used;

  q.push({initialState, 0});
  used[{initialState, 0}] = true;

  bool isAccepted = false;
  while (not q.empty()) {

    auto curState = q.front();
    q.pop();

    if (curState.second == (int) s.size()) {
      if(isFinalState[curState.first]) {
        isAccepted = true;
        break;
      }
    }

    for (auto lTrans : lambdaTransitions[curState.first]) {
      if (not used[{lTrans.state, curState.second}]) {
        used[{lTrans.state, curState.second}] = true;
        q.push({lTrans.state, curState.second});
      }
    }

    if (curState.second == (int) s.size()) continue;

    for (auto trans : transitions[curState.first]) {
      if (trans.character == s[curState.second]) {
        if (not used[{trans.state, curState.second + 1}]) {
          used[{trans.state, curState.second + 1}] = true;
          q.push({trans.state, curState.second + 1});
        }
      }
    }
  }

  std::cout << s << ' ';
  if (isAccepted) {
      std::cout << " ACCEPTED\n";
  }
  else {
    std::cout << " REJECTED\n";
  }
}
/*}}}*/

// ================================= AUTOMATA FUNCTIONALITY ==================================={{{

std::ostream& operator << (std::ostream &os, const Automata& a) {

  os << a.stateCount << '\n';
  os << (int) a.alphabet.size() << '\n';
  for (const char &x : a.alphabet) os << x << ' ';
  os << '\n';
  os << a.initialState << '\n';
  os << a.finalStateCount << '\n';
  for (int state = 0; state < a.stateCount; ++ state) {
    if (a.isFinalState[state]) os << state << ' ';
  }
  os << '\n';
  os << a.transitionCount << '\n';
  for (int state = 0; state < a.stateCount; ++ state) {
    for (const Automata::Transition &trans : a.transitions[state]) {
      os << state << ' ' << trans.character << ' ' << trans.state << '\n';
    }
    for (const Automata::Transition &trans : a.lambdaTransitions[state]) {
      os << state << ' ' << trans.character << ' ' << trans.state << '\n';
    }
  }

  return os;
}

Automata* Automata::getNewAutomata(const int &stateCount, const std::vector < int > &finalStates,
    const std::vector < std::vector < Transition > > &transitions) {

  std::ofstream out("_");

  // nr of states
  out << stateCount << '\n';

  // alphabet
  out << (int) this->alphabet.size() << '\n';
  for (const char &c : this->alphabet) out << c << ' ';
  out << '\n';

  // initial state
  out << this->initialState << '\n';

  // final states
  out << (int) finalStates.size() << '\n';
  for (const int &x : finalStates) out << x << ' ';
  out << '\n';

  // transitions

  int transitionCount = 0;
  for (int state = 0; state < stateCount; ++ state) {
    transitionCount += (int) transitions[state].size();
  }

  out << transitionCount << '\n';

  for (int state = 0; state < stateCount; ++ state) {
    for (auto &x : transitions[state]) {
      out << state << ' ' << x.character << ' ' << x.state << '\n'; 
    }
  }

  out.close();
  Automata *aux = new Automata("_");
  std::remove("_");

  return aux;
}

/*}}}*/

// ================================= LNFA TO NFA TRANSFORM ==================================={{{

void Automata::makeLamdaClosure(int curNode, std::vector < std::set < int > > &closure, std::vector < bool > &used) {

  used[curNode] = true;
  closure[curNode].insert(curNode);

  for (const Transition &trans : lambdaTransitions[curNode]) {

    if (not used[trans.state]) {
      makeLamdaClosure(trans.state, closure, used);
    }

    for (const int &node : closure[trans.state]) {
      closure[curNode].insert(node);
    }
  }
}

Automata* Automata::nfaFromLnfa() {

  std::vector < std::set < int > > lambdaClosure(this->stateCount);
  std::vector < bool > used(this->stateCount);
  
  // Step 1: lambda closure

  for (int state = 0; state < this->stateCount; ++ state) {
    if (used[state]) continue;
    makeLamdaClosure(state, lambdaClosure, used);
  }

  // Step 2: delta transition function for nfa

  std::vector < std::set < Transition > > nfaTransitionsAux(this->stateCount);

  for (int state = 0; state < this->stateCount; ++ state) {
    for (const int &nextState : lambdaClosure[state]) {
      for (const Transition &trans : transitions[nextState]) {
        for (const int &nextNextState : lambdaClosure[trans.state]) {
          nfaTransitionsAux[state].insert({nextNextState, trans.character});
        }
      }
    }
  }

  // Step 3: initial and final states

  std::vector < bool > nfaIsFinalStateAux(this->stateCount, false);
  for (int state = 0; state < this->stateCount; ++ state) {
    for (const int &x : lambdaClosure[state]) {
      if (isFinalState[x]) {
        nfaIsFinalStateAux[state] = true;
        break;
      }
    }
  }

  // Step 4: remove redundand states & output

  int nfaStateCount = this->stateCount;

  Dsu dsu(this->stateCount);
  for (int i = 0; i < this->stateCount; ++ i) {
    for (int j = i + 1; j < this -> stateCount; ++ j) {
      if (nfaIsFinalStateAux[i] != nfaIsFinalStateAux[j]) continue;
      if (nfaTransitionsAux[i] == nfaTransitionsAux[j]) {
        if (dsu.link(i, j)) -- nfaStateCount;
      }
    }
  }

  std::map < int, int > normalize;
  for (int state = 0; state < this->stateCount; ++ state) {
    normalize[dsu.getParent(state)] = 1;
  }

  int index = 0;
  for (auto &x : normalize) x.second = index ++;

  std::vector < int > nfaFinalStates;
  nfaFinalStates.reserve(nfaStateCount);
  std::vector < std::vector < Transition > > nfaTransitions(nfaStateCount);

  std::fill(used.begin(), used.end(), false);
  for (int i = 0; i < this->stateCount; ++ i) {
    int state = dsu.getParent(i);
    if (not used[state]) {
      used[state] = true;
      if (nfaIsFinalStateAux[state]) nfaFinalStates.emplace_back(normalize[state]);

      std::set < Transition > aux;
      for (const Transition &trans : nfaTransitionsAux[state]) {
        aux.insert({normalize[dsu.getParent(trans.state)], trans.character});
      }

      for (const Transition &trans : aux) nfaTransitions[normalize[state]].emplace_back(trans);
    }
  }

  return getNewAutomata(nfaStateCount, nfaFinalStates, nfaTransitions);
}

/*}}}*/

// ================================= NFA TO DFA TRANSFORM ==================================={{{

Automata* Automata::dfsFromNfa() {

  // Step 1 & 3: remove nondeterminism and rename states

  std::map < std::set < int >, int > normalize;
  std::vector < std::vector < Transition > > dfaTransitions(1);

  std::queue < std::set < int > > q;

  int dfaStateCount = 0;
  q.push({initialState});
  normalize[{initialState}] = dfaStateCount ++;

  while (not q.empty()) {

    std::set < int > curState = q.front();
    q.pop();

    for (const char &x : alphabet) {

      std::set < int > nextState;
      for (const int &state : curState) {
        for (const Transition &trans : transitions[state]) {
          if (trans.character != x) continue;
          nextState.insert(trans.state);
        }
      }

      if (nextState.size() == 0) continue;

      if (normalize.find(nextState) == normalize.end()) {
        q.push(nextState);
        normalize[nextState] = dfaStateCount ++;
        dfaTransitions.push_back(std::vector < Transition >());
      }

      dfaTransitions[normalize[curState]].emplace_back(normalize[nextState], x);
    }
  }

  // Step 2: innitial and final stated of the dfa

  std::vector < int > dfaFinalStates;

  for (const auto &x : normalize) {

    bool fs = false;
    for (const int &state : x.first) {
      if (isFinalState[state]) {
        fs = true;
        break;
      }
    }

    if (fs) dfaFinalStates.emplace_back(x.second);
  }

  return getNewAutomata(dfaStateCount, dfaFinalStates, dfaTransitions);
}

/*}}}*/
