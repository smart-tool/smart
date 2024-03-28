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
 * This is an implementation of the recursive Galil Seiferas algorithm
 * in GALIL Z., SEIFERAS J., 1983, Time-space optimal string matching,
 * Journal of Computer and System Science 26(3):280-294.
 *
 * TODO: sometimes tripping over
 *       gs.c:96: newP1: Assertion `s + p1 + q1 <= m' failed
 */

#include "include/define.h"
#include "include/main.h"
#include <assert.h>

unsigned char *x, *y;
int k, m, n, p, p1, p2, q, q1, q2, s;
int count;

void newP1();
void newP2();

void _search() {
  while (p <= n - m) {
    //if (s + p + q > n)
    //  return;
    assert(s + q <= m);
    assert(s + p + q <= n);
    while (s + p + q < n && x[s + q] == y[s + p + q])
      ++q;
    if (q == m - s && memcmp(x, y + p, s + 1) == 0)
      OUTPUT(p);
    if (q == p1 + q1) {
      p += p1;
      q -= p1;
    } else {
      p += (q / k + 1);
      q = 0;
    }
  }
}

void parse() {
  while (1) {
    assert(s + p1 + q1 <= m);
    while (x[s + q1] == x[s + p1 + q1])
      ++q1;
    while (p1 + q1 >= k * p1) {
      s += p1;
      q1 -= p1;
    }
    p1 += (q1 / k + 1);
    q1 = 0;
    if (p1 >= p2)
      break;
  }
  newP1();
}

void newP2() {
  assert(s + p2 + q2 <= m);
  while (x[s + q2] == x[s + p2 + q2] && p2 + q2 < k * p2)
    ++q2;
  if (p2 + q2 == k * p2)
    parse();
  else if (s + p2 + q2 == m)
    _search();
  else {
    if (q2 == p1 + q1) {
      p2 += p1;
      q2 -= p1;
    } else {
      p2 += (q2 / k + 1);
      q2 = 0;
    }
    newP2();
  }
}

void newP1() {
  assert(s + p1 + q1 <= m);
  while (x[s + q1] == x[s + p1 + q1])
    ++q1;
  if (p1 + q1 >= k * p1) {
    p2 = q1;
    q2 = 0;
    newP2();
  } else {
    if (s + p1 + q1 == m)
      _search();
    else {
      p1 += (q1 / k + 1);
      q1 = 0;
      newP1();
    }
  }
}

int search(unsigned char *argX, int argM, unsigned char *argY, int argN) {

  count = 0;
  /* Searching */
  BEGIN_SEARCHING
  x = argX;
  m = argM;
  y = argY;
  n = argN;
  k = 4;
  p = q = s = q1 = p2 = q2 = 0;
  p1 = 1;
  newP1();
  END_SEARCHING
  return count;
}
