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
 * This is an implementation of the efficient, hybrid, Double-Hash
 * String-Matching Algorithm in Mehmet Bicer and Xiaowen Zhan, IEEE LISAT 2019.
 * http://www.cs.csi.cuny.edu/~zhangx/papers/P_2019_LISAT_Bicer_Zhang.pdf
 */

#include "include/define.h"
#include "include/main.h"

void preBmBc(unsigned char *x, int m, int bmBc[]) {
  int i;
  for (i = 0; i < SIGMA; ++i)
    bmBc[i] = m;
  for (i = 0; i < m - 1; ++i)
    bmBc[x[i]] = m - i - 1;
}

unsigned int BPHashHalf(unsigned char *str, unsigned int len) {
  unsigned int hash = 0;
  unsigned int i = 0;
  for (i = 0; i < len/2; str++, i++) {
    hash = hash << 7 ^ (*str);
  }

  return hash;
}

unsigned int BKDRHash(unsigned char *str, unsigned int len) {
  unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
  unsigned int hash = 0;
  unsigned int i = 0;

  for (i = 0; i < len; str++, i++) {
    hash = (hash * seed) + (*str);
  }

  return hash;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int s, count = 0, bmBc[SIGMA];
  unsigned int h1, h2;

  BEGIN_PREPROCESSING
  preBmBc(x, m, bmBc);
  h1 = BPHashHalf(x, (unsigned)m);
  h2 = BKDRHash(x, (unsigned)m);
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  s = 0;
  while (s <= n - m) {
    if (*x == y[s]
        && h1 == BPHashHalf(&y[s], (unsigned)m)
        && h2 == BKDRHash(&y[s], (unsigned)m))
      OUTPUT(s);
    s += bmBc[y[s + m - 1]];
  }
  END_SEARCHING
  return count;
}
