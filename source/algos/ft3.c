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
 * Fine tuned computation of the Boyer-Moore exact string matching algorithm.
 * In A fast implementation of the good-suffix array for the Boyer-Moore string
 * matching algorithm, FUN 2024, https://arxiv.org/abs/2402.16469v1.
 * Thierry Lecroq, 2024
 * https://github.com/lecroq/goodsuff/blob/main/ft3.c
 */

#include "include/define.h"
#include "include/main.h"

/*
 * good-suffix table: mixed version
 */
void ft3PreBmGs(unsigned char *x, int m, int *bmGs, int *suff) {
  int f, g, i, j, k;
  int sh;
  int mMinus1;
  char c;
 
  mMinus1 = m-1;
  g = mMinus1;
  c = x[mMinus1];
  /* looking for the end of the first run of c thus before suff[i] = 0 */
  for (i = m - 2; i >= 0 && x[i] == c; --i);
  k = mMinus1-i;
  bmGs[mMinus1] = k;
  for (j = m - 2, --k; j > i; --j, --k) {
    suff[j] = bmGs[j] = k;
  }
  for (j = 0; j <= i; ++j) {
    bmGs[j] = m;
  }
  j = 0;
  /* looking for the second run of c */
  for (--i; i >=0 && x[i] != c; --i);
  if (i < 0) {
    return;
  }
  f = i;
  g = i-1;
  while (g >= 0 && x[g] == x[g + mMinus1 - f]) {
    --g;
  }
  suff[i] = f - g;
  if (g < 0) { /* suff[i] == i+1) */
    for (; j < mMinus1 - i; ++j) {
      if (bmGs[j] > mMinus1-i) {
        bmGs[j] = mMinus1 - i;
      }
    }
    --i;
    goto laststep;
  }
  else {
   if (bmGs[mMinus1 - suff[i]] > mMinus1 - i) {
     bmGs[mMinus1 - suff[i]] = mMinus1 - i;
   }
  }
  
  /* after suff[i] = 0 and before suff[i] = i+1 */
  for (--i; i >= 0; --i) {
    if (x[i] != c) {
      continue;
    }
    if (i > g && suff[i + mMinus1 - f] != i - g) {
      if (suff[i + mMinus1 - f] < i-g) {
        suff[i] = suff[i + mMinus1 - f];
      }
      else {
        if (suff[i + mMinus1 - f] > i-g) {
          suff[i] = i-g;
          if (bmGs[mMinus1 - suff[i]] > mMinus1 - i) {
            bmGs[mMinus1 - suff[i]] = mMinus1 - i;
          }
        }
      }
    }
    else {
      if (i < g) {
        g = i-1;
      }
      f = i;
      while (g >= 0 && x[g] == x[g + mMinus1 - f]) {
        --g;
      }
      suff[i] = f - g;
      if (g < 0) { /* suff[i] == i+1 */
         for (; j < mMinus1 - i; ++j) {
           if (bmGs[j] > mMinus1-i) {
             bmGs[j] = mMinus1 - i;
           }
         }
         --i;
         break;
      }
      else {
        if (bmGs[mMinus1 - suff[i]] > mMinus1 - i) {
          bmGs[mMinus1 - suff[i]] = mMinus1 - i;
        }
      }
    }
  }
laststep:
  /* after suff[i] = i+1 */
  sh = mMinus1-f;
  for (; i >= 0; --i) {
    if (x[i] != c) {
      continue;
    }
    if (i+1 <= suff[i + sh]) {
      suff[i] = i+1;
      for (; j < mMinus1 - i; ++j) {
        if (bmGs[j] > mMinus1-i) {
          bmGs[j] = mMinus1 - i;
        }
      }
    }
    else {
      suff[i] = suff[i+sh];
    }
  }
}

void preBmBc(unsigned char *x, int m, int bmBc[]) {
  int i;
  for (i = 0; i < SIGMA; ++i)
    bmBc[i] = m;
  for (i = 0; i < m - 1; ++i)
    bmBc[x[i]] = m - i - 1;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int bcShift, i, j, shift, u, v, turboShift, count, bmGs[XSIZE], bmBc[SIGMA];
  int suff[XSIZE];

  /* Preprocessing */
  BEGIN_PREPROCESSING
  ft3PreBmGs(x, m, bmGs, suff);
  preBmBc(x, m, bmBc);
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  count = 0;
  j = u = 0;
  shift = m;
  while (j <= n - m) {
    i = m - 1;
    while (i >= 0 && x[i] == y[i + j]) {
      --i;
      if (u != 0 && i == m - 1 - shift)
        i -= u;
    }
    if (i < 0) {
      OUTPUT(j);
      shift = bmGs[0];
      u = m - shift;
    } else {
      v = m - 1 - i;
      turboShift = u - v;
      bcShift = bmBc[y[i + j]] - m + 1 + i;
      shift = MAX(turboShift, bcShift);
      shift = MAX(shift, bmGs[i]);
      if (shift == bmGs[i])
        u = MIN(m - shift, v);
      else {
        if (turboShift < bcShift)
          shift = MAX(shift, u + 1);
        u = 0;
      }
    }
    j += shift;
  }
  END_SEARCHING
  return count;
}
