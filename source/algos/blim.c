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
 * This is an implementation of the Bit Parallel Length Invariant Matcher
 * in M. O. Kulekci.
 * A method to overcome computer word size limitation in bit-parallel pattern
 * matching. Proceedings of the 19th International Symposium on Algorithms and
 * Computation, ISAAC 2008, Lecture Notes in Computer Science, vol.5369,
 * pp.496--506, Springer-Verlag, Berlin, Gold Coast, Australia, (2008).
 *
 * Correctness fixes applied to the imported implementation:
 * 1. m=1 formerly read uninitialized stack memory: the scan-order builder loop
 *    did not execute, leaving ScanOrder[]/MScanOrder[] undefined.  A dedicated
 *    single-byte scan below handles m=1 before those arrays are read.
 * 2. The scan-order builder omitted offsets congruent to zero modulo m, leaving
 *    one pattern byte unchecked in every lane and producing false positives.
 *    It now covers all ws offsets.
 * 3. Only complete W-lane windows use the mask matrix.  The remaining start
 *    positions are checked directly, so BLIM never reads beyond the text.
 * Historical note: BLIM appears in the 2010 comprehensive survey by Faro
 * and Lecroq (arXiv 1012.2547) as "[Kül08]" but is never among the 25
 * best results on any text or pattern length -- suggesting it was
 * catalogued as a 2008 research prototype and never fully debugged or
 * published in peer-reviewed form.  Külekci's later EPSM (SSE4 exact
 * packed string matching) is a separate, more developed algorithm.
 *
 * Requires m > 0.  The bit-parallel path supports m <= XSIZE; larger
 * patterns use search_large.
 */

#include "include/define.h"
#include "include/main.h"
#include "include/search_small.h"
#include "include/search_large.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int i, j, k, count;
  unsigned int tmp, F;
  unsigned int ScanOrder[XSIZE + WORD - 1];
  unsigned int MScanOrder[XSIZE + WORD - 1];
  unsigned int *so = ScanOrder;
  unsigned int *mso = MScanOrder;
  unsigned int shift[SIGMA];
  unsigned int *M;
#define WSIZE_CUTOFF 63
  unsigned int s_M[SIGMA * WSIZE_CUTOFF];
  const unsigned int wsize = WORD - 1 + m;

  if (m < 1)
    return 0;
  if (m == 1) {
    BEGIN_PREPROCESSING
    END_PREPROCESSING
    BEGIN_SEARCHING
    count = 0;
    for (i = 0; i < n; i++)
      if (x[0] == y[i])
        OUTPUT(i);
    END_SEARCHING
    return count;
  }
  if (m > XSIZE)
    return search_large(x, m, y, n);
  if (n < (int)wsize) {
    if (m < WORD)
      return search_small(x, m, y, n);
  }

  /* Preprocessing */
  BEGIN_PREPROCESSING
  if (wsize > WSIZE_CUTOFF)
    M = (unsigned int *)malloc(sizeof(unsigned int) * SIGMA * wsize);
  else
    M = s_M;
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(M, 0xff, sizeof(unsigned int) * SIGMA * wsize);
  for (i = 0; i < WORD; i++) {
    tmp = 1U << i;
    for (j = 0; j < m; j++) {
      for (k = 0; k < SIGMA; k++)
        M[((i + j) * SIGMA) + k] &= ~tmp;
      M[x[j] + ((i + j) * SIGMA)] |= tmp;
    }
  }

  for (i = 0; i < SIGMA; i++)
    shift[i] = wsize + 1;
  for (i = 0; i < m; i++)
    shift[x[i]] = wsize - i;

  for (i = m - 1; i >= 0; i--) {
    k = i;
    while (k < (int)wsize) {
      *so = k;
      *mso = SIGMA * k;
      so++;
      mso++;
      k += m;
    }
  }
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  count = 0;
  i = 0;
  while (i <= n - (int)wsize) {
    F = M[MScanOrder[0] + y[i + ScanOrder[0]]] &
        M[MScanOrder[1] + y[i + ScanOrder[1]]];
    for (j = 2; F && j < (int)wsize; j++)
      F &= M[MScanOrder[j] + y[i + ScanOrder[j]]];
    if (F) {
      for (j = 0; j < WORD; j++)
        if (F & (1U << j))
          OUTPUT(i + j);
    }
    if (i + (int)wsize >= n) {
      i += WORD;
      break;
    }
    i += shift[y[i + wsize]];
  }
  for (; i <= n - m; i++)
    if (memcmp(x, &y[i], m) == 0)
      OUTPUT(i);
  if (wsize > WSIZE_CUTOFF)
    free(M);
  END_SEARCHING
  return count;
}
