/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012 Simone Faro and Thierry Lecroq
 *
 * Succinct Backward DAWG Matching (SBDM), K. Fredriksson, JEA 13 (2008),
 * doi:10.1145/1412228.1455263. The backward DAWG factor test is represented
 * by a BWT index with rank directories instead of an explicit automaton.
 */

#include "include/define.h"
#include "include/main.h"

#include <stdint.h>

#define SBDM_ALPHABET (SIGMA + 1)
#define SBDM_WORDS ((XSIZE + 1 + 63) / 64)

static const int *sbdm_rank;
static int sbdm_length;
static int sbdm_step;

static int suffix_compare(const void *left, const void *right) {
  const int a = *(const int *)left;
  const int b = *(const int *)right;
  const int a_second =
      a + sbdm_step < sbdm_length ? sbdm_rank[a + sbdm_step] : -1;
  const int b_second =
      b + sbdm_step < sbdm_length ? sbdm_rank[b + sbdm_step] : -1;

  if (sbdm_rank[a] != sbdm_rank[b])
    return sbdm_rank[a] - sbdm_rank[b];
  return a_second - b_second;
}

static int fallback_search(unsigned char *x, int m, unsigned char *y, int n) {
  int count = 0, i, j, shift[SIGMA];

  BEGIN_PREPROCESSING
  for (i = 0; i < SIGMA; ++i)
    shift[i] = m;
  for (i = 0; i < m - 1; ++i)
    shift[x[i]] = m - i - 1;
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (j = 0; j <= n - m;) {
    for (i = m - 1; i >= 0 && x[i] == y[j + i]; --i)
      ;
    if (i < 0) {
      OUTPUT(j);
      ++j;
    } else {
      const int amount = shift[y[j + m - 1]];

      j += amount > 0 ? amount : 1;
    }
  }
  END_SEARCHING
  return count;
}

static unsigned
rank_symbol(const uint64_t bits[SBDM_ALPHABET][SBDM_WORDS],
            const uint16_t prefix[SBDM_ALPHABET][SBDM_WORDS + 1],
            unsigned symbol, int position) {
  const int word = position / 64;
  const int offset = position % 64;
  uint64_t mask;

  if (offset == 0)
    return prefix[symbol][word];
  mask = (UINT64_C(1) << offset) - 1;
  return prefix[symbol][word] + POPCOUNT64(bits[symbol][word] & mask);
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int count = 0, alphabet[SBDM_ALPHABET] = {0}, cumulative[SBDM_ALPHABET];
  int sa[XSIZE + 1], rank[XSIZE + 1], next_rank[XSIZE + 1];
  uint64_t bits[SBDM_ALPHABET][SBDM_WORDS] = {{0}};
  uint16_t prefix[SBDM_ALPHABET][SBDM_WORDS + 1] = {{0}};
  const int length = m + 1;
  int i, j, k, sentinel_row = 0;

  if (m <= 0 || n < m)
    return 0;
  if (m > XSIZE)
    return fallback_search(x, m, y, n);

  BEGIN_PREPROCESSING
  for (i = 0; i < length; ++i) {
    sa[i] = i;
    rank[i] = i == m ? 0 : x[i] + 1;
  }
  for (k = 1; k < length; k <<= 1) {
    int classes = 0;

    sbdm_rank = rank;
    sbdm_length = length;
    sbdm_step = k;
    qsort(sa, length, sizeof(*sa), suffix_compare);
    next_rank[sa[0]] = 0;
    for (i = 1; i < length; ++i) {
      if (suffix_compare(&sa[i - 1], &sa[i]) != 0)
        ++classes;
      next_rank[sa[i]] = classes;
    }
    memcpy(rank, next_rank, length * sizeof(*rank));
    if (classes == length - 1)
      break;
  }
  for (i = 0; i < length; ++i) {
    const int suffix = sa[i];
    const unsigned symbol = suffix == 0 ? 0 : x[suffix - 1] + 1;

    ++alphabet[symbol];
    if (symbol == 0)
      sentinel_row = i;
    bits[symbol][i / 64] |= UINT64_C(1) << (i % 64);
  }
  cumulative[0] = 0;
  for (i = 1; i < SBDM_ALPHABET; ++i)
    cumulative[i] = cumulative[i - 1] + alphabet[i - 1];
  for (i = 0; i < SBDM_ALPHABET; ++i) {
    for (j = 0; j < SBDM_WORDS; ++j)
      prefix[i][j + 1] = prefix[i][j] + POPCOUNT64(bits[i][j]);
  }
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (i = 0; i <= n - m;) {
    int left = 0, right = m, shift = m;

    for (j = m; j > 0;) {
      const unsigned symbol = y[i + --j] + 1;

      left = cumulative[symbol] + rank_symbol(bits, prefix, symbol, left);
      right =
          cumulative[symbol] + rank_symbol(bits, prefix, symbol, right + 1) - 1;
      if (left > right)
        break;
      if (left <= sentinel_row && sentinel_row <= right) {
        if (j > 0)
          shift = j;
        else
          OUTPUT(i);
      }
    }
    i += shift;
  }
  END_SEARCHING
  return count;
}
