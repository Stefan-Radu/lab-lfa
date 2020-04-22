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
  int m; fin >> m;

  for (int i = 0; i < m; ++ i) {
    char k; fin >> k;
    alphabet.insert(k);
  }

  fin >> initialState;

  // nr of final states;
  fin >> m;
  for (int i = 0; i < m; ++ i) {
    int k; fin >> k;
    isFinalState[k] = true;
  }

  // nr of transitions;
  fin >> transitionCount;
  for (int i = 0; i < transitionCount; ++ i) {

    int x, y; char a;
    fin >> x >> a >> y;

    assert(x >= 0 and x < stateCount);
    assert(y >= 0 and y < transitionCount); 
    assert(alphabet.find(a) != alphabet.end() or a == '$');

    if (a == '$') lambdaTransitions[x].emplace_back(y, a);
    else transitions[x].emplace_back(y, a);
  }
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
  std::vector < std::set < Transition > > nfaTransitions(nfaStateCount);

  std::fill(used.begin(), used.end(), false);
  for (int i = 0; i < this->stateCount; ++ i) {
    int state = dsu.getParent(i);
    if (not used[state]) {
      used[state] = true;
      if (nfaIsFinalStateAux[state]) nfaFinalStates.emplace_back(normalize[state]);
      for (const Transition &trans : nfaTransitionsAux[state]) {
        nfaTransitions[normalize[state]].insert({normalize[dsu.getParent(trans.state)], trans.character});
      }
    }
  }

  // output
  std::ofstream out("nfa_from_lnfa");

  // nr of states
  out << nfaStateCount << '\n';

  // alphabet
  out << (int) alphabet.size() << '\n';
  for (const char &c : alphabet) out << c << ' ';
  out << '\n';

  // initial state
  out << initialState << '\n';

  // final states
  out << (int) nfaFinalStates.size() << '\n';
  for (int &x : nfaFinalStates) out << x << ' ';
  out << '\n';

  // transitions

  int transitionCnt = 0;
  for (int state = 0; state < nfaStateCount; ++ state) {
    transitionCnt += (int) nfaTransitions[state].size();
  }

  out << transitionCnt << '\n';

  for (int state = 0; state < nfaStateCount; ++ state) {
    for (auto &x : nfaTransitions[state]) {
      out << state << ' ' << x.character << ' ' << x.state << '\n'; 
    }
  }

  out.close();

  Automata *aux = new Automata("nfa_from_lnfa");
  std::remove("nfa_from_lnfa");

  return aux;
}

/*}}}*/
