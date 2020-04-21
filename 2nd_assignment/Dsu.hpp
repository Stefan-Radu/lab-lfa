#pragma once

#include <iostream>
#include <vector>

class Dsu {
public:

  Dsu(int);

  int getParent(int);
  bool link(int, int);

private:

  std::vector < int > parent, weight;
};
