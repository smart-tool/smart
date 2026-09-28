/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012  Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This is an exact, single-pattern specialization of Wu-Manber's hashed
 * SHIFT table.  It uses two-byte blocks, the smallest useful q-gram for the
 * SMART byte-oriented search interface.
 *
 * S. Wu and U. Manber, Agrep - A Fast Approximate Pattern-Matching Tool,
 * Proceedings of the USENIX Technical Conference, 1992, pp. 153-162.
 */

#include "include/define.h"
#include "include/main.h"
#include <stdint.h>

#define SM_TABLE_SIZE (1 << 16)

static int fallback_search(unsigned char *x, int m, unsigned char *y, int n) {
  int count = 0;
  int i, j;

  BEGIN_PREPROCESSING
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (i = 0; i <= n - m; i++) {
    for (j = 0; j < m && x[j] == y[i + j]; j++)
      ;
    if (j == m)
      OUTPUT(i);
  }
  END_SEARCHING
  return count;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  uint16_t shift[SM_TABLE_SIZE];
  int count = 0;
  int i, j, amount;

  if (m <= 0 || n < m)
    return 0;
  if (m < 2)
    return fallback_search(x, m, y, n);

  BEGIN_PREPROCESSING
  for (i = 0; i < SM_TABLE_SIZE; i++)
    shift[i] = (uint16_t)(m - 1);
  for (i = 0; i < m - 1; i++)
    shift[((unsigned)x[i] << 8) | x[i + 1]] = (uint16_t)(m - 2 - i);
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (i = 0; i <= n - m;) {
    const unsigned key = ((unsigned)y[i + m - 2] << 8) | y[i + m - 1];

    amount = shift[key];
    if (amount != 0) {
      i += amount;
      continue;
    }

    for (j = 0; j < m && x[j] == y[i + j]; j++)
      ;
    if (j == m)
      OUTPUT(i);
    i++;
  }
  END_SEARCHING
  return count;
}
