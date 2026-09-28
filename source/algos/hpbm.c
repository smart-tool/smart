/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012 Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * High Performance Parallel Boyer-Moore (HPBM), adapted for shared-memory
 * CPUs from:
 * Y. Jeong, M. Lee, D. Nam, J.-S. Kim, and S. Hwang,
 * "High Performance Parallelization of Boyer-Moore Algorithm on Many-Core
 * Accelerators", ICCAC 2014, pp. 265-272, doi:10.1109/ICCAC.2014.20.
 *
 * The paper partitions the text among workers and uses dynamic scheduling.
 * Here each worker owns a disjoint range of candidate start positions.  A
 * worker may inspect the pattern-length suffix after its range, but only
 * reports starts it owns.  This finds boundary-spanning matches exactly once
 * without copying text.
 */

#include "include/define.h"
#include "include/main.h"

#define HPBM_CHUNK_SIZE (1 << 16)

static void preBmBc(const unsigned char *x, int m, int bmBc[SIGMA]) {
  int i;

  for (i = 0; i < SIGMA; ++i)
    bmBc[i] = m;
  for (i = 0; i < m - 1; ++i)
    bmBc[x[i]] = m - i - 1;
}

static void suffixes(const unsigned char *x, int m, int suff[XSIZE]) {
  int f = 0, g, i;

  suff[m - 1] = m;
  g = m - 1;
  for (i = m - 2; i >= 0; --i) {
    if (i > g && suff[i + m - 1 - f] < i - g) {
      suff[i] = suff[i + m - 1 - f];
    } else {
      if (i < g)
        g = i;
      f = i;
      while (g >= 0 && x[g] == x[g + m - 1 - f])
        --g;
      suff[i] = f - g;
    }
  }
}

static void preBmGs(const unsigned char *x, int m, int bmGs[XSIZE]) {
  int i, j, suff[XSIZE];

  suffixes(x, m, suff);
  for (i = 0; i < m; ++i)
    bmGs[i] = m;
  j = 0;
  for (i = m - 1; i >= 0; --i) {
    if (suff[i] == i + 1) {
      for (; j < m - 1 - i; ++j) {
        if (bmGs[j] == m)
          bmGs[j] = m - 1 - i;
      }
    }
  }
  for (i = 0; i <= m - 2; ++i)
    bmGs[m - 1 - suff[i]] = m - 1 - i;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int bmBc[SIGMA], bmGs[XSIZE], chunk, chunks, count = 0, limit;

  if (m <= 0 || m > XSIZE || n < m)
    return 0;

  BEGIN_PREPROCESSING
  preBmGs(x, m, bmGs);
  preBmBc(x, m, bmBc);
  END_PREPROCESSING

  limit = n - m;
  chunks = limit / HPBM_CHUNK_SIZE + 1;

  BEGIN_SEARCHING
#pragma omp parallel for schedule(dynamic) reduction(+ : count)
  for (chunk = 0; chunk < chunks; ++chunk) {
    long end, j = (long)chunk * HPBM_CHUNK_SIZE;

    end = MIN(j + HPBM_CHUNK_SIZE, (long)limit + 1);
    while (j < end) {
      int i;

      for (i = m - 1; i >= 0 && x[i] == y[j + i]; --i)
        ;
      if (i < 0) {
        OUTPUT(j);
        j += bmGs[0];
      } else {
        int badCharacterShift = bmBc[y[j + i]] - m + 1 + i;
        j += MAX(bmGs[i], badCharacterShift);
      }
    }
  }
  END_SEARCHING

  return count;
}
