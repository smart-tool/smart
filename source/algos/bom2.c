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
 * This is an implementation of the Backward Oracle Matching
 * in C. Allauzen and M. Crochemore and M. Raffinot.
 * Factor oracle: a new structure for pattern matching.
 * SOFSEM'99, Theory and Practice of Informatics, Lecture Notes in Computer
 * Science, n.1725, pp.291--306, (1999).
 */

#include "include/define.h"
#include "include/main.h"
#include <assert.h>

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int S[XSIZE];
  int *trans[XSIZE];
  int i, j, p, q = 0;
  int iMinus1, mMinus1, nMinusm, count;
  int allocated = 0;
  int s_trans[M_CUTOFF + 2][SIGMA];

  BEGIN_PREPROCESSING
  unsigned char c;
  count = 0;
  if (m + 2 > M_CUTOFF) {
    allocated = 1;
    for (i = 0; i <= m + 1; i++)
      trans[i] = (int *)malloc(sizeof(int) * SIGMA);
  } else {
    for (i = 0; i <= m + 1; i++)
      trans[i] = s_trans[i];
  }

  // Preprocessing
  for (i = 0; i <= m + 1; i++)
    for (j = 0; j < SIGMA; j++)
      trans[i][j] = UNDEFINED;
  S[m] = m + 1;
  for (i = m; i > 0; --i) {
    iMinus1 = i - 1;
    c = x[iMinus1];
    trans[i][c] = iMinus1;
    p = S[i];
    while (p <= m && (q = trans[p][c]) == UNDEFINED) {
      trans[p][c] = iMinus1;
      p = S[p];
    }
    S[iMinus1] = (p == m + 1 ? m : q);
  }
  END_PREPROCESSING

  // Searching
  BEGIN_SEARCHING
  j = 0;
  nMinusm = n - m;
  mMinus1 = m - 1;
  while (j <= nMinusm) {
    i = mMinus1;
    p = m;
    assert(i + j < n);
    while (i + j >= 0 && (q = trans[p][y[i + j]]) != UNDEFINED) {
      p = q;
      --i;
    }
    if (i < 0) {
      count++;
      ++j;
    }
    j += (i + 1);
  }
  if (allocated) {
    for (i = 0; i <= m + 1; i++)
      free(trans[i]);
  }
  END_SEARCHING
  return count;
}
