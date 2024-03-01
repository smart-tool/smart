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
 * This is an implementation of the Shift Vector Matching algorithm
 * in H. Peltola and J. Tarhio.
 * Alternative Algorithms for Bit-Parallel String Matching.
 * Proceedings of the 10th International Symposium on String Processing and
 * Information Retrieval SPIRE'03, (2003).
 */

#include "include/define.h"
#include "include/main.h"

int search_large(unsigned char *x, int m, unsigned char *y, int n);

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int count = 0;
  int s;
  int j;
  unsigned int tmp, h, sv = 0, cv[SIGMA];

  if (m > 32)
    return search_large(x, m, y, n);

  /* Preprocessing */
  BEGIN_PREPROCESSING
  tmp = (~0);
  tmp >>= (WORD - m);
  for (j = 0; j < SIGMA; j++)
    cv[j] = tmp;
  tmp = 1;
  for (j = m - 1; j >= 0; j--) {
    cv[x[j]] &= ~tmp;
    tmp <<= 1;
  }
  END_PREPROCESSING

  /* searching */
  BEGIN_SEARCHING
  if (!memcmp(x, y, m))
    OUTPUT(0);
  s = m;
  while (s < n) {
    sv |= cv[y[s]];
    j = 1;
    while ((sv & 1) == 0) {
      sv |= j > 31 ? 0 : cv[y[s - j]] >> j;
      if (j >= m) {
        OUTPUT(s);
        break;
      }
      j++;
    }
    j = 0;
    h = ~(sv >>= 1);
    h &= -h;
    if (h & 0xffff0000)
      j += 16;
    if (h & 0xff00ff00)
      j += 8;
    if (h & 0xf0f0f0f0)
      j += 4;
    if (h & 0xcccccccc)
      j += 2;
    if (h & 0xaaaaaaaa)
      j += 1;
    sv >>= j;
    s += j + 1;
  }
  END_SEARCHING
  return (count);
}

/*
 * Shift Vector Matching algorithm designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of
 * the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n) {
  int count = 0;
  int s;
  int j, p_len, first, k;
  unsigned int tmp, h, sv = 0, cv[SIGMA];
  p_len = m;
  m = 32;

  /* Preprocessing */
  BEGIN_PREPROCESSING
  tmp = (~0);
  tmp >>= (WORD - m);
  for (j = 0; j < SIGMA; j++)
    cv[j] = tmp;
  tmp = 1;
  for (j = m - 1; j >= 0; j--) {
    cv[x[j]] &= ~tmp;
    tmp <<= 1;
  }
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  if (!memcmp(x, y, p_len))
    OUTPUT(0);
  s = m;
  while (s < n) {
    sv |= cv[y[s]];
    j = 1;
    while ((sv & 1) == 0) {
      sv |= cv[y[s - j]] >> j;
      if (j >= m) {
        k = m;
        first = s - m + 1;
        while (k < p_len && x[k] == y[first + k])
          k++;
        if (k == p_len)
          OUTPUT(first);
        break;
      }
      j++;
    }
    j = 0;
    h = ~(sv >>= 1);
    h &= -h;
    if (h & 0xffff0000)
      j += 16;
    if (h & 0xff00ff00)
      j += 8;
    if (h & 0xf0f0f0f0)
      j += 4;
    if (h & 0xcccccccc)
      j += 2;
    if (h & 0xaaaaaaaa)
      j += 1;
    sv >>= j;
    s += j + 1;
  }
  END_SEARCHING
  return (count);
}
