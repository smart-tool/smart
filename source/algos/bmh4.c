/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012 Simone Faro and Thierry Lecroq
 *
 * Boyer-Moore-Horspool with 4-grams. The q-gram shift table replaces
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

#define BMH4_TABLE_SIZE 16384u
#define BMH4_TABLE_MASK (BMH4_TABLE_SIZE - 1u)

static uint32_t qgram(const unsigned char *s) {
  return ((uint32_t)s[0] << 24) | ((uint32_t)s[1] << 16) |
         ((uint32_t)s[2] << 8) | s[3];
}

static unsigned qgram_slot(uint32_t key) {
  return (key * 2654435761u) & BMH4_TABLE_MASK;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int count = 0, i, j, default_shift;
  uint16_t shift[SIGMA];
  uint32_t keys[BMH4_TABLE_SIZE];
  uint16_t qshift[BMH4_TABLE_SIZE];
  unsigned char used[BMH4_TABLE_SIZE] = {0};

  if (m <= 0 || n < m)
    return 0;

  BEGIN_PREPROCESSING
  if (m >= 4) {
    default_shift = m - 3;
    for (i = 0; i < m - 4; ++i) {
      const uint32_t key = qgram(x + i);
      unsigned slot = qgram_slot(key);

      while (used[slot] && keys[slot] != key)
        slot = (slot + 1) & BMH4_TABLE_MASK;
      used[slot] = 1;
      keys[slot] = key;
      qshift[slot] = m - 4 - i;
    }
  } else {
    default_shift = m;
    for (i = 0; i < SIGMA; ++i)
      shift[i] = default_shift;
    for (i = 0; i < m - 1; ++i)
      shift[x[i]] = m - i - 1;
  }
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (j = 0; j <= n - m;) {
    int amount;

    for (i = m - 1; i >= 0 && x[i] == y[j + i]; --i)
      ;
    if (i < 0)
      OUTPUT(j);

    if (m >= 4) {
      const uint32_t key = qgram(y + j + m - 4);
      unsigned slot = qgram_slot(key);

      // cppcheck-suppress legacyUninitvar
      while (used[slot] && keys[slot] != key)
        slot = (slot + 1) & BMH4_TABLE_MASK;
      // cppcheck-suppress legacyUninitvar
      amount = used[slot] ? qshift[slot] : default_shift;
    } else {
      amount = shift[y[j + m - 1]];
    }
    j += amount;
  }
  END_SEARCHING
  return count;
}
