/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012  Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * contact the authors at: faro@dmi.unict.it, thierry.lecroq@univ-rouen.fr
 * download the tool at: http://www.dmi.unict.it/~faro/smart/
 *
 * This is an implementation of the Alpha Skip Search algorithm
 * in C. Charras and T. Lecroq and J. D. Pehoushek.
 * A Very Fast String Matching Algorithm for Small Alphabets and Long Patterns.
 * Proceedings of the 9th Annual Symposium on Combinatorial Pattern Matching,
 * Lecture Notes in Computer Science, n.1448, pp.55--64, Springer-Verlag,
 * Berlin, rutgers, (1998).
 */

#include "include/define.h"
#include "include/log2.h"
#include "include/main.h"

#define getZ(i) z[(i)]
#define NIL 0
#define delta(q, a) automaton->trans[(q)*SIGMA + (a)]
#define primary(q, a) automaton->prim[(q)*SIGMA + (a)]
#define failure(q) automaton->fail[(q)]
#define terminal(q) automaton->term[(q)]
#define shift(q) automaton->sh[(q)]
#define depth(q) automaton->_depth[(q)]

struct _listOfIntegers {
  int position;
  struct _listOfIntegers *next;
};

typedef struct _listOfIntegers *ListOfIntegers;

struct _automaton {
  int *trans, *fail, *sh, root, currentState, nodeCounter;
  char *term, **prim;
  ListOfIntegers *list;
  int *_depth;
};

typedef struct _automaton *Automaton;
typedef int Node;

void freeListOfIntegers(ListOfIntegers list) {
  ListOfIntegers old;
  old = list;
  while (list) {
    list = list->next;
    free(old);
    old = list;
  }
}

//NOLINTBEGIN(clang-analyzer-unix.Malloc)
void setZ(ListOfIntegers *z, int node, int position) {
  ListOfIntegers cell;

  cell = (ListOfIntegers)malloc(sizeof(struct _listOfIntegers));
  cell->position = position;
  cell->next = z[node];
  z[node] = cell;
}

Node addNode(Automaton automaton, Node art, Node node, unsigned char c) {
  Node childNode, suffixNode, suffixChildNode;

  childNode = automaton->nodeCounter++;
  delta(node, c) = childNode;
  suffixNode = failure(node);
  if (suffixNode == art)
    failure(childNode) = node;
  else {
    suffixChildNode = delta(suffixNode, c);
    if (suffixChildNode == NIL)
      suffixChildNode = addNode(automaton, art, suffixNode, c);
    failure(childNode) = suffixChildNode;
  }
  return (childNode);
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  struct _automaton s_automaton;
  Automaton automaton = &s_automaton;
  int i, j, ell, pos, shift, q, count;
  ListOfIntegers cell, *z;
  Node art, root, node, childNode;
  // 2 + (2 * m - m/2 + 1) * m/2
#define S_CUTOFF 32
  int s_trans[S_CUTOFF * SIGMA] = {0};
  int s_fail[S_CUTOFF] = {0};
  char s_term[S_CUTOFF] = {0};
  ListOfIntegers s_z[S_CUTOFF] = {0};

  /* Preprocessing */
  BEGIN_PREPROCESSING
  ell = LOG2(m);
  if (ell < 1)
    ell = 1;
  else if (ell > m / 2)
    ell = m / 2;
  const int size = 2 + (2 * m - ell + 1) * ell;
  if (size > S_CUTOFF) {
    automaton->trans = (int *)calloc(size * SIGMA, sizeof(int));
    automaton->term = (char *)calloc(size, 1);
    automaton->fail = (int *)calloc(size, sizeof(int));
    z = (ListOfIntegers *)calloc(size, sizeof(ListOfIntegers));
  } else {
    automaton->trans = s_trans;
    automaton->term = s_term;
    automaton->fail = s_fail;
    z = s_z;
  }
  automaton->nodeCounter = 0;

  art = automaton->nodeCounter++;
  automaton->root = automaton->nodeCounter++;
  root = automaton->root;
  failure(root) = art;

  node = root;
  node = addNode(automaton, art, node, x[0]);
  failure(node) = root;
  for (i = 1; i < ell; ++i)
    node = addNode(automaton, art, node, x[i]);
  pos = 0;
  setZ(z, node, pos);
  pos++;
  for (i = ell; i < m - 1; ++i) {
    node = failure(node);
    childNode = delta(node, x[i]);
    if (childNode == NIL)
      node = addNode(automaton, art, node, x[i]);
    else
      node = childNode;
    setZ(z, node, pos);
    pos++;
  }
  node = failure(node);
  childNode = delta(node, x[i]);
  if (childNode == NIL)
    node = addNode(automaton, art, node, x[i]);
  else
    node = childNode;
  setZ(z, node, pos);
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  count = 0;
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memcpy(y + n, x, m);
  shift = m - ell + 1;
  j = m - ell;
  while (1) {
    while (delta(root, y[j]) == NIL)
      j += shift;
    node = delta(root, y[j]);
    for (q = 1; q < ell; ++q)
      node = delta(node, y[j + q]);
    if (node != NIL)
      for (cell = getZ(node); cell != NULL; cell = cell->next) {
        if (memcmp(x, y + j - cell->position, m) == 0) {
          if (j - cell->position <= n - m) {
            OUTPUT(j - cell->position);
          } else {
            for (i = automaton->root; i < automaton->nodeCounter; ++i)
              freeListOfIntegers(z[i]);
            if (size > S_CUTOFF) {
              free(z);
              free(automaton->fail);
              free(automaton->term);
              free(automaton->trans);
            }
            //free(automaton);
            END_SEARCHING
            return count;
          }
        }
      }
    j += shift;
  }
}
//NOLINTEND(clang-analyzer-unix.Malloc)
