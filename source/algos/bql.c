/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012  Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This is an implementation of BQL (BNDMq Long) in
 * B. Ďurian, H. Peltola, L. Salmela, and J. Tarhio,
 * "Bit-Parallel Search Algorithms for Long Patterns",
 * SEA 2010, Algorithm 3.
 * https://users.aalto.fi/~tarhio/papers/sea10.pdf
 *
 * The q-gram table is a filter: condensed q-grams and superimposed
 * positions can collide, so every candidate is verified exactly.
 *
 * Constraints: requires m >= 8 (q = 7, s = 2).
 */

#define MIN_M 8
#include "include/define.h"
#include "include/main.h"
#include "include/search_small.h"

#define Q 7
#define S 2
#define ASIZE (1U << (Q * S))
#define AMASK (ASIZE - 1)

int search(unsigned char *x, int m, unsigned char *y, int n) {
  unsigned int B[ASIZE];
  const int mq1 = m - Q + 1;
  const int w = MIN(m, WORD);
  const unsigned int high_bit = 1U << (w - 1);
  int count = 0;
  int i, j, k;
  unsigned int ch;

  if (m <= Q)
    return search_small(x, m, y, n);
  if (n < m)
    return 0;

  BEGIN_PREPROCESSING
  for (i = 0; i < (int)ASIZE; i++)
    B[i] = 0;

  ch = 0;
  const int padded_m = m - (m % w) + w;
  for (i = m - 1; i >= 0; i--) {
    ch = ((ch << S) + x[i]) & AMASK;
    if (i < mq1)
      B[ch] |= 1U << ((padded_m - i) % w);
  }
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (i = mq1 - 1; i <= n - Q; i += mq1) {
    ch = 0;
    for (j = i + Q - 1; j >= i; j--)
      ch = ((ch << S) + y[j]) & AMASK;

    unsigned int D = B[ch];
    const int first = i - mq1 + 1;
    if ((D & 1U) && i + m <= n && memcmp(x, y + i, m) == 0)
      OUTPUT(i);

    j = i;
    while (D != 0 && j > first) {
      j--;
      if (D & high_bit) {
        for (k = j; k >= first; k -= w) {
          if (k + m <= n && memcmp(x, y + k, m) == 0)
            OUTPUT(k);
        }
      }
      ch = ((ch << S) + y[j]) & AMASK;
      D = (D << 1) & B[ch];
    }
  }
  END_SEARCHING
  return count;
}
