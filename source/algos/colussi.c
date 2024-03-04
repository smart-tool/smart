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
 * This is an implementation of the Colussi algorithm
 * in COLUSSI L., 1991, Correctness and efficiency of the pattern matching
 * algorithms, Information and Computation 95(2):225-251.
 * http://www-igm.univ-mlv.fr/~lecroq/string/node10.html
 */

#include "include/define.h"
#include "include/main.h"

int preColussi(unsigned char *x, int m, int h[], int next[], int shift[]) {
  int i, k, nd, r = 0, q, s;
  int hmax[XSIZE], kmin[XSIZE], nhd0[XSIZE], rmin[XSIZE];

  /* Computation of hmax */
  i = k = 1;
  do {
    while (x[i] == x[i - k])
      i++;
    hmax[k] = i;
    q = k + 1;
    while (hmax[q - k] + k < i) {
      hmax[q] = hmax[q - k] + k;
      q++;
    }
    k = q;
    if (k == i + 1)
      i = k;
  } while (k <= m);

  /* Computation of kmin */
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(kmin, 0, m * sizeof(int));
  for (i = m; i >= 1; --i)
    if (hmax[i] < m)
      kmin[hmax[i]] = i;

  /* Computation of rmin */
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(rmin, 0, m * sizeof(int));
  for (i = m - 1; i >= 0; --i) {
    if (hmax[i + 1] == m)
      r = i + 1;
    if (kmin[i] == 0)
      rmin[i] = r;
    else
      rmin[i] = 0;
  }

  /* Computation of h */
  s = -1;
  r = m;
  for (i = 0; i < m; ++i)
    if (kmin[i] == 0)
      h[--r] = i;
    else
      h[++s] = i;
  nd = s;

  /* Computation of shift */
  for (i = 0; i <= nd; ++i)
    shift[i] = kmin[h[i]];
  for (i = nd + 1; i < m; ++i)
    shift[i] = rmin[h[i]];
  shift[m] = rmin[0];

  /* Computation of nhd0 */
  s = 0;
  for (i = 0; i < m; ++i) {
    nhd0[i] = s;
    if (kmin[i] > 0)
      ++s;
  }

  /* Computation of next */
  for (i = 0; i <= nd; ++i)
    next[i] = nhd0[h[i] - kmin[h[i]]];
  for (i = nd + 1; i < m; ++i)
    next[i] = nhd0[m - rmin[h[i]]];
  next[m] = nhd0[m - rmin[h[m - 1]]];

  return (nd);
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int count;
  int i, j, last, nd, h[XSIZE], next[XSIZE], shift[XSIZE];

  /* Preprocessing */
  BEGIN_PREPROCESSING
  nd = preColussi(x, m, h, next, shift);
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  count = 0;
  i = j = 0;
  last = -1;
  while (j <= n - m) {
    while (i < m && last < j + h[i] && x[h[i]] == y[j + h[i]])
      i++;
    if (i >= m || last >= j + h[i]) {
      OUTPUT(j);
      i = m;
    }
    if (i > nd)
      last = j + m - 1;
    j += shift[i];
    i = next[i];
  }
  END_SEARCHING
  return count;
}
