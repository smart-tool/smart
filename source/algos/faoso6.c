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
 * download the tool at:  https://github.com/rurban/smart/
 *
 * This is an implementation of the Fast Average Optimal Shift Or algorithm
 * in K. Fredriksson and S. Grabowski.
 * Practical and Optimal String Matching. SPIRE, Lecture Notes in Computer
 * Science, vol.3772, pp.376--387, Springer-Verlag, Berlin, (2005).
 *
 * Constraints: requires m>6
 */

#define MIN_M 7
#include <stdint.h>
#include "include/define.h"
#include "include/log2.h"
#include "include/main.h"
#include "include/search_small.h"

void verify(unsigned char *y, int j, int n, unsigned char *x, int m, int q,
            int u, uint32_t D, uint32_t mm, int *count) {
  int c, mq, v, z, i, k;
  unsigned s;

  D = (D & mm) ^ mm;
  mq = m / q - 1;
  while (D != 0) {
    s = LOG2(D);
    v = mq + u;
    c = -mq * q;
    z = s % v - mq;
    c -= (s / v + z * q);
    i = j + c;
    k = 0;
    if (i >= 0 && i <= n - m)
      while (k < m && x[k] == y[i + k])
        k++;
    if (k == m)
      OUTPUTP(i);
    D &= ~(1U << s);
  }
}

int search_large(unsigned char *x, int m, unsigned char *y, int n, int q);

int search(unsigned char *x, int m, unsigned char *y, int n) {
  unsigned int B[SIGMA], h;
  uint32_t D, mm, masq;
  int i, j, u, count;
  int uq, uq1, mq;
  const int q = 6;

  u = 2;
  /* The bit-parallel filter packs q=6 sub-streams into a 32-bit D/mm/masq
     register, each needing floor(m/q)+1 bits plus a 1-bit separator; the
     last bit position used is floor(m/q)*q + q - 1, which only stays
     within [0,31] for m <= 29 (m=30 already needs bit 35 -- a real
     undefined-behavior shift, reproduced by ASan/UBSan). 32-u+1=31 was
     too high a threshold for q=6 specifically (it happens to work for
     the q=2/q=4 siblings' narrower per-stream layout). */
  if (m > 29)
    return search_large(x, m, y, n, q);
  if (m <= q)
    return search_small(x, m, y, n);

  /* Preprocessing */
  BEGIN_PREPROCESSING
  masq = 0;
  mq = m / q;
  h = mq;
  for (j = 0; j < q; ++j) {
    masq |= (1U << h);
    masq |= (1U << h);
    h += mq;
    ++h;
  }
  for (i = 0; i < SIGMA; ++i)
    B[i] = ~0U;
  h = mm = 0;
  for (j = 0; j < q; ++j) {
    for (i = 0; i < mq; ++i) {
      B[x[i * q + j]] &= ~(1U << h);
      ++h;
    }
    mm |= (1U << (h - 1));
    ++h;
    mm |= (1U << (h - 1));
    ++h;
    --h;
  }
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  count = 0;
  D = ~mm;
  j = 0;
  uq = u * q;
  uq1 = (u - 1) * q;
  while (j < n) {
    D = (D << 1) | (B[y[j]] & ~masq);
    D = (D << 1) | (B[y[j + q]] & ~masq);
    if ((D & mm) != mm)
      verify(y, j + uq1, n, x, m, q, u, D, mm, &count);
    D &= ~mm;
    j += uq;
  }
  END_SEARCHING
  return count;
}

/*
 * Fast Average Optimal Shift Or algorithm designed for large patterns.
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of
 * the pattern.
 */

void verify_large(unsigned char *y, int j, int n, unsigned char *x, int m,
                  int q, int u, uint32_t D, uint32_t mm, int *count,
                  int p_len) {
  int s, c, mq, v, z, i, k;

  D = (D & mm) ^ mm;
  mq = m / q - 1;
  while (D != 0) {
    s = LOG2(D);
    v = mq + u;
    c = -mq * q;
    z = s % v - mq;
    c -= (s / v + z * q);
    i = j + c;
    k = 0;
    if (i >= 0 && i <= n - m)
      while (k < p_len && x[k] == y[i + k])
        k++;
    if (k == p_len)
      OUTPUTP(i);
    D &= ~(1U << s);
  }
}

int search_large(unsigned char *x, int m, unsigned char *y, int n, int q) {
  uint32_t B[SIGMA], mm, D;
  uint32_t masq;
  unsigned h;
  int i, j, u, count, p_len;
  int uq, uq1, mq;

  u = 2;
  p_len = m;
  /* must match search()'s dispatch threshold: the largest window for
     which q=6's bit-parallel filter fits in a 32-bit register (see
     comment in search()) */
  m = 29;

  /* Preprocessing */
  BEGIN_PREPROCESSING
  masq = 0U;
  mq = m / q;
  h = mq;
  for (j = 0; j < q; ++j) {
    masq |= (1UL << h);
    h += mq;
    ++h;
  }

  for (i = 0; i < SIGMA; ++i)
    B[i] = ~0U;
  h = mm = 0;
  for (j = 0; j < q; ++j) {
    for (i = 0; i < mq; ++i) {
      B[x[i * q + j]] &= ~(1UL << h);
      ++h;
    }
    mm |= (1UL << (h - 1));
    ++h;
    mm |= (1UL << (h - 1));
    ++h;
    --h;
  }
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  count = 0;
  D = ~mm;
  j = 0;
  uq = u * q;
  uq1 = (u - 1) * q;
  while (j < n) {
    D = (D << 1) | (B[y[j]] & ~masq);
    D = (D << 1) | (B[y[j + q]] & ~masq);
    if ((D & mm) != mm)
      verify_large(y, j + uq1, n, x, m, q, u, D, mm, &count, p_len);
    D &= ~mm;
    j += uq;
  }
  END_SEARCHING
  return count;
}
