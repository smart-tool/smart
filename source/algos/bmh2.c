/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012 Simone Faro and Thierry Lecroq
 *
 * Boyer-Moore-Horspool with 2-grams. The q-gram shift table replaces
 * Horspool's final-character table; see T. Lecroq, "Experimental results on
 * string matching algorithms", Software--Practice & Experience 1995.
 * https://www.dmi.unict.it/faro/papers/journal/faroJ13.pdf
 *
 * bmh2` and `bmh4` extend Boyer-Moore-Horspool with 2- and 4-byte terminal
 * q-grams. The shift table excludes the pattern's final q-gram and maps each
 * earlier q-gram to its rightmost shift; an absent q-gram shifts by `m - q + 1`.
 * Patterns shorter than the selected q-gram use ordinary Horspool.
 */

#include "include/define.h"
#include "include/main.h"

#include <stdint.h>

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int count = 0, i, j, default_shift;
  uint16_t shift[1u << 16];

  if (m <= 0 || n < m)
    return 0;
  default_shift = m >= 2 ? m - 1 : m;

  BEGIN_PREPROCESSING
  if (m >= 2) {
    for (i = 0; i < (1 << 16); ++i)
      shift[i] = default_shift;
    for (i = 0; i < m - 2; ++i)
      shift[((unsigned)x[i] << 8) | x[i + 1]] = m - 2 - i;
  } else {
    for (i = 0; i < SIGMA; ++i)
      shift[i] = default_shift;
  }
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (j = 0; j <= n - m;) {
    int amount;

    for (i = m - 1; i >= 0 && x[i] == y[j + i]; --i)
      ;
    if (i < 0)
      OUTPUT(j);

    if (m >= 2)
      amount = shift[((unsigned)y[j + m - 2] << 8) | y[j + m - 1]];
    else
      amount = shift[y[j]];
    j += amount;
  }
  END_SEARCHING
  return count;
}
