#include "Automata.hpp"

Automata::Automata(const std::string &inputFileName) {
  initAutomata(inputFileName);
}

void Automata::initAutomata(const std::string &inputFileName) {

  std::ifstream fin(inputFileName);
  assert(fin.is_open());

  // read number of states
  int n; fin >> n;

  isFinalState.resize(n);
  transitions.resize(n);

  // nr of characters in the alphabet;
  int m; fin >> m;

  std::set < char > alphabet;
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
  fin >> m;
  for (int i = 0; i < m; ++ i) {

    int x, y; char a;
    fin >> x >> a >> y;

    assert(x >= 0 and x < n);
    assert(y >= 0 and y < m); 
    assert(alphabet.find(a) != alphabet.end() or a == '$');

    transitions[x].emplace_back(y, a);
  }
}

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

    used[curState] = false;

    if (curState.second == (int) s.size()) {
      if(isFinalState[curState.first]) {
        isAccepted = true;
        break;
      }
    }

    for (auto transition : transitions[curState.first]) {

      if (transition.transtionChar == '$') {
        if (not used[{transition.nextNode, curState.second}]) {
          used[{transition.nextNode, curState.second}] = true;
          q.push({transition.nextNode, curState.second});
        }
        continue;
      }

      if (curState.second == (int) s.size()) continue;

      if (transition.transtionChar == s[curState.second]) {
        if (not used[{transition.nextNode, curState.second + 1}]) {
          used[{transition.nextNode, curState.second + 1}] = true;
          q.push({transition.nextNode, curState.second + 1});
        }
      }
    }
  }

  std::cout << "std::string " << s;
  if (isAccepted) {
      std::cout << " ACCEPTED\n";
  }
  else {
    std::cout << " REJECTED\n";
  }
}
