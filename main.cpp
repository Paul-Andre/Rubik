/* This is the story of how I solved a Rubik's cube for the first time. I wanted to do it without consulting
 * any tutorials. I stuggled to do it manually and was thinking to write a program for it quite for a big 
 * of time. I finally got to it today.
 *
 * I wrote a program to help me with it. I did it on a friday night. The whole took something like
 * 5 hours and I was listening to some mix of nightcore music youtube suggested me all of a sudden.
 * It's 3:50 AM now. Before this I was drinking wine with Marek and Neeraj in Mountain View.
 *
 * I entered the cube's layout once at the beginning.
 *
 * I had to split the solving into multiple runs of the program and reentering the intermediary solutions.
 * I was adjusting the lines that say "if (sw.mismatches <= 0)" and "int coef = 2;"
 *
 * The biggest thing that prevented me from doing it in one run is memory I guess. A solution could be
 * to "trim" the heap/visisted set/previous mapping every now and then.
 *
 * All in all it took 31 steps (not counting me originally solving the top side)
 *
 * The original layout was:
    655
    644
    444

236 111 552
566 111 554
226 111 562

    333
    634
    333

    526
    222
    434
 */



#include <bits/stdc++.h>
using namespace std;

enum Color {
  NONE = 0,
  BLUE = 1,
  GREEN = 2,
  WHITE = 3,
  YELLOW = 4,
  RED = 5,
  ORANGE = 6,
};

std::ostream& operator<< (std::ostream &out, Color c) {
  if (c == NONE) {
      out <<"\033[31;1m" << (int) c <<"\033[0m";
  } else
  if (c == BLUE) {
      out <<"\033[34;1m" << (int) c <<"\033[0m";
  } else
  if (c == GREEN) {
      out <<"\033[32;1m" << (int) c <<"\033[0m";
  } else
  if (c == WHITE) {
      out <<"\033[1m" << (int) c <<"\033[0m";
  } else
  if (c == YELLOW) {
      out <<"\033[93;1m" << (int) c <<"\033[0m";
  } else
  if (c == RED) {
      out <<"\033[31m" << (int) c <<"\033[0m";
  } else
  if (c == ORANGE) {
      out <<"\033[33m" << (int) c <<"\033[0m";
  }
  return out;
}

struct State{
  Color colors[6][3][3];
};

bool operator==(State const &a, State const &b) {
  for (int i=0; i<6; i++) for (int j=0; j<3; j++) for (int k=0; k<3; k++) {
    if (a.colors[i][j][k] != b.colors[i][j][k] ) {
      return false;
    }
  }
  return true;
}

uint64_t hash64(uint64_t x) {
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = x ^ (x >> 31);
    return x;
}

uint64_t hashCombine(uint64_t acc, uint64_t b) {
  return hash64(acc ^ hash64(b));
}

uint64_t hashCube(Color const c[6][3][3]) {
  uint64_t acc = 0;
  for (int i=0; i<6; i++) for (int j=0; j<3; j++) for (int k=0; k<3; k++) {
    acc = hashCombine(acc, c[i][j][k]);
  }
  return acc;
}


namespace std {
  template<> struct hash<State> {
    size_t operator()(State const& s) const noexcept {
      return hashCube(s.colors);
    }
  };
}

//  sides[face][y][x]
//  
//    0
//  1 2 3   ->     (0,0) (0,1) (0,2)
//    4            (1,0) (1,1) (1,2)
//    5            (2,0) (2,1) (2,2)
//

void printCube(Color c[6][3][3]) {
  cout << "    " << c[0][0][0] << c[0][0][1] << c[0][0][2] <<endl;
  cout << "    " << c[0][1][0] << c[0][1][1] << c[0][1][2] <<endl;
  cout << "    " << c[0][2][0] << c[0][2][1] << c[0][2][2] <<endl;

  cout <<endl;

  cout << c[1][0][0] << c[1][0][1] << c[1][0][2] <<" "<< c[2][0][0] << c[2][0][1] << c[2][0][2] <<" "<< c[3][0][0] << c[3][0][1] << c[3][0][2] <<endl;
  cout << c[1][1][0] << c[1][1][1] << c[1][1][2] <<" "<< c[2][1][0] << c[2][1][1] << c[2][1][2] <<" "<< c[3][1][0] << c[3][1][1] << c[3][1][2] <<endl;
  cout << c[1][2][0] << c[1][2][1] << c[1][2][2] <<" "<< c[2][2][0] << c[2][2][1] << c[2][2][2] <<" "<< c[3][2][0] << c[3][2][1] << c[3][2][2] <<endl;


  cout <<endl;
  cout << "    " << c[4][0][0] << c[4][0][1] << c[4][0][2] <<endl;
  cout << "    " << c[4][1][0] << c[4][1][1] << c[4][1][2] <<endl;
  cout << "    " << c[4][2][0] << c[4][2][1] << c[4][2][2] <<endl;
             
  cout <<endl;
  cout << "    " << c[5][0][0] << c[5][0][1] << c[5][0][2] <<endl;
  cout << "    " << c[5][1][0] << c[5][1][1] << c[5][1][2] <<endl;
  cout << "    " << c[5][2][0] << c[5][2][1] << c[5][2][2] <<endl;
  cout <<endl;
}

// Unfortunately can't be used of A*
int countMismatches(Color c[6][3][3]) {
  int count = 0;
  for (int i=0; i<6; i++) {
    for (int j=0; j<3; j++) for (int k=0; k<3; k++) {
      if (c[i][1][1] != c[i][j][k]) count ++;
    }
  }
  return count;
}

void sideCw(Color src[3][3], Color dst[3][3]) {
  Color cc[3][3] = {
    {src[2][0], src[1][0], src[0][0]},
    {src[2][1], src[1][1], src[0][1]},
    {src[2][2], src[1][2], src[0][2]}
  };
  memcpy(dst, cc, sizeof(cc));
}

void sideId(Color src[3][3], Color dst[3][3]) {
  Color cc[3][3];
  memcpy(dst, src, sizeof(cc));
}

void sideCcw(Color src[3][3], Color dst[3][3]) {
  Color cc[3][3] = {
    {src[0][2], src[1][2], src[2][2]},
    {src[0][1], src[1][1], src[2][1]},
    {src[0][0], src[1][0], src[2][0]}
  };
  memcpy(dst, cc, sizeof(cc));
}

void sideFlip(Color src[3][3], Color dst[3][3]) {
  Color a[3][3];
  sideCcw(src, a);
  sideCcw(a, dst);
}

void cubeId(Color src[6][3][3], Color dst[6][3][3]) {
  Color a[6][3][3];
  memcpy(dst, src, sizeof(a));
}

void cubeRotateUp(Color src[6][3][3], Color dst[6][3][3]) {
  sideId(src[2], dst[0]);
  sideId(src[4], dst[2]);
  sideId(src[5], dst[4]);
  sideId(src[0], dst[5]);

  sideCcw(src[1], dst[1]);
  sideCw(src[3], dst[3]);
}

void cubeRotateRight(Color src[6][3][3], Color dst[6][3][3]) {

  sideCcw(src[0], dst[0]);
  sideFlip(src[5], dst[1]);
  sideId(src[1], dst[2]);
  sideId(src[2], dst[3]);

  sideCw(src[4], dst[4]);

  sideFlip(src[3], dst[5]);
}

void cubeRotateRightTwice(Color src[6][3][3], Color dst[6][3][3]) {
  Color a[6][3][3];
  cubeRotateRight(src, a);
  cubeRotateRight(a, dst);
}

void cubeRotateLeft(Color src[6][3][3], Color dst[6][3][3]) {
  Color a[6][3][3];
  Color b[6][3][3];
  cubeRotateRight(src, a);
  cubeRotateRight(a, b);
  cubeRotateRight(b, dst);
}

void cubeRotateUpTwice(Color src[6][3][3], Color dst[6][3][3]) {
  Color a[6][3][3];
  cubeRotateUp(src, a);
  cubeRotateUp(a, dst);
}

void cubeRotateDown(Color src[6][3][3], Color dst[6][3][3]) {
  Color a[6][3][3];
  Color b[6][3][3];
  cubeRotateUp(src, a);
  cubeRotateUp(a, b);
  cubeRotateUp(b, dst);
}

void twistFrontCw(Color src[6][3][3], Color dst[6][3][3]) {
  cubeId(src, dst);
  sideCw(src[2], dst[2]);

  dst[0][2][0] = src[1][2][2];
  dst[0][2][1] = src[1][1][2];
  dst[0][2][2] = src[1][0][2];
  dst[3][0][0] = src[0][2][0];
  dst[3][1][0] = src[0][2][1];
  dst[3][2][0] = src[0][2][2];
  dst[4][0][2] = src[3][0][0];
  dst[4][0][1] = src[3][1][0];
  dst[4][0][0] = src[3][2][0];
  dst[1][2][2] = src[4][0][2];
  dst[1][1][2] = src[4][0][1];
  dst[1][0][2] = src[4][0][0];
}

void twistTopCw(Color src[6][3][3], Color dst[6][3][3]) {
  Color a[6][3][3];
  Color b[6][3][3];
  cubeRotateDown(src, a);
  twistFrontCw(a, b);
  cubeRotateUp(b, dst);
}

void twistBottomCw(Color src[6][3][3], Color dst[6][3][3]) {
  Color a[6][3][3];
  Color b[6][3][3];
  cubeRotateUp(src, a);
  twistFrontCw(a, b);
  cubeRotateDown(b, dst);
}

void twistBackCw(Color src[6][3][3], Color dst[6][3][3]) {
  Color a[6][3][3];
  Color b[6][3][3];
  cubeRotateUpTwice(src, a);
  twistFrontCw(a, b);
  cubeRotateUpTwice(b, dst);
}

void twistRightCw(Color src[6][3][3], Color dst[6][3][3]) {
  Color a[6][3][3];
  Color b[6][3][3];
  cubeRotateLeft(src, a);
  twistFrontCw(a, b);
  cubeRotateRight(b, dst);
}

void twistLeftCw(Color src[6][3][3], Color dst[6][3][3]) {
  Color a[6][3][3];
  Color b[6][3][3];
  cubeRotateRight(src, a);
  twistFrontCw(a, b);
  cubeRotateLeft(b, dst);
}

typedef void Transformation (Color src[6][3][3], Color dst[6][3][3]);

template <Transformation f, int n>
void repeated(Color src[6][3][3], Color dst[6][3][3]) {
  Color prev[6][3][3];
  cubeId(src, prev);
  for(int i=0; i<n; i++) {
    Color prev2[6][3][3];
    cubeId(prev, prev2);
    f(prev2, prev);
  }
  cubeId(prev, dst);
}


struct StateAndWeight{
  State state;
  int mismatches;
  int distance;
};

bool operator<(StateAndWeight const &a, StateAndWeight const &b) {
  // return a.mismatches > b.mismatches;
  int coef = 2; // 12 for proven A*, lower for less optimal solution and faster computation
  return a.distance * coef + a.mismatches > b.distance * coef + b.mismatches;
  // return a.distance > b.distance;
}

int main() {
  State initialState;
  {
    // fucking kill me
  int aa[6][3][3]=  {
    { {4,4,4},
      {4,4,4},
      {4,4,4} },

    { {6,6,6},
      {6,6,6},
      {6,6,6} },
       { {1,1,1},
         {1,1,3},
         {1,1,1} },
          { {5,5,5},
            {5,5,5},
            {5,2,5} },

    { {3,3,3},
      {3,3,3},
      {3,1,3} },

    { {2,5,2},
      {2,2,2},
      {2,2,2} }
  };
    for (int i=0; i<6; i++) for (int j=0; j<3; j++) for (int k=0; k<3; k++) {
      initialState.colors[i][j][k] = (Color)(aa[i][j][k]);
    }
  }

  printCube(initialState.colors);
  cout <<endl;

  priority_queue<StateAndWeight> q;
  unordered_set<State> seen;
  unordered_map<State, pair<State, int>> previous;
  auto push = [&q, &seen](State s, int dist) {
    q.push({s, countMismatches(s.colors), dist });
  };
  push(initialState, 0);

  int smallestMismatch = 1000;

  State result;

  while(!q.empty()) {
    StateAndWeight sw = q.top();
    q.pop();
    State &s = sw.state;
    if (sw.mismatches < smallestMismatch) {
      smallestMismatch = sw.mismatches;
      cout << sw.mismatches << endl;
    }
    if (sw.mismatches <= 0) {
      result = s;
      break;
    }
    
    if (seen.count(s) != 0) continue;
    seen.insert(s);

    vector<Transformation *> transformations = {
      twistFrontCw,
      repeated<twistFrontCw, 2>,
      repeated<twistFrontCw, 3>,
      
      twistBackCw,
      repeated<twistBackCw, 2>,
      repeated<twistBackCw, 3>,
      
      twistTopCw,
      repeated<twistTopCw, 2>,
      repeated<twistTopCw, 3>,
      
      twistBottomCw,
      repeated<twistBottomCw, 2>,
      repeated<twistBottomCw, 3>,
      
      twistLeftCw,
      repeated<twistLeftCw, 2>,
      repeated<twistLeftCw, 3>,
      
      twistRightCw,
      repeated<twistRightCw, 2>,
      repeated<twistRightCw, 3>,
    };

    for (auto f: transformations) {
      State t;
      f(s.colors, t.colors);
      int newDistance = sw.distance + 1;
      push(t, newDistance);
      auto it = previous.find(t);
      if (it == previous.end() || newDistance < it->second.second ) {
        previous[t] = make_pair(s, newDistance);
      }
    }
  }

  State itState = result;
    printCube(itState.colors);
    cout <<endl;
  int steps = 0;
  while (!(itState == initialState)) {

    itState = previous[itState].first;
    printCube(itState.colors);
    cout <<endl;
    steps++;
  }
  cout <<steps;



  return 0;
}

