// Stefan-Octavian Radu - grupa 131

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <cassert>
#include <map>

using namespace std;

class LNFA {
public:

  LNFA(string fileName) {

    ifstream fin(fileName);
    assert(fin.is_open());

    int n; fin >> n; // nr of states
    isFinalState.resize(n);
    transitions.resize(n);

    int m; fin >> m; // nr of characters in the alphabet;

    set < char > alphabet;
    for (int i = 0; i < m; ++ i) {
      char k; fin >> k;
      alphabet.insert(k);
    }

    fin >> initialState;

    fin >> m; // nr of final states;
    for (int i = 0; i < m; ++ i) {
      int k; fin >> k;
      isFinalState[k] = true;
    }

    fin >> m; // nr of transitions;
    for (int i = 0; i < m; ++ i) {

      int x, y; char a;
      fin >> x >> a >> y;

      assert(x >= 0 and x < n);
      assert(y >= 0 and y < m); 
      assert(alphabet.find(a) != alphabet.end() or a == '$');

      transitions[x].emplace_back(y, a);
    }
  }

  void testFile(string fileName) {

    ifstream fin(fileName);
    assert(fin.is_open());

    string s;
    while (fin >> s) {
      testString(s);
    }
  }

  void testString(string s) {

    queue < pair < int, int > > q;
    map < pair < int, int >, bool > used;

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

    cout << "String " << s;
    if (isAccepted) {
       cout << " ACCEPTED\n";
    }
    else {
      cout << " REJECTED\n";
    }
  }

private:

  static const char lambda = '$';

  struct Transition {
    Transition(int y, char a): nextNode(y), transtionChar(a) {}
    int nextNode;
    char transtionChar;
  };

  int initialState;

  vector < bool > isFinalState;
  vector < vector < Transition > > transitions; 
};

int main () {
  LNFA l("lnfa");
  l.testFile("test0");
}
