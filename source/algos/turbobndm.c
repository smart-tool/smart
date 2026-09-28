/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012 Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * contact the authors at: faro@dmi.unict.it, thierry.lecroq@univ-rouen.fr
 * download the tool at: https://github.com/rurban/smart/
 *
 * Turbo BNDM from G. Navarro and M. Raffinot, Fast and Flexible String
 * Matching by Combining Bit-Parallelism and Suffix Automata, ACM Journal
 * of Experimental Algorithmics 5 (2000), Section 5.1.
 */

#include "include/define.h"
#include "include/main.h"

static int lowest_set_bit(unsigned int bits) {
  int bit = 0;

  while ((bits & 1U) == 0) {
    bits >>= 1;
    bit++;
  }
  return bit;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  unsigned int B[SIGMA], D, bit, high_bit, all_bits, borders[32], X;
  int failure[33];
  int i, j, k, step, prefix, matched_prefix, border, count;
  const int filter_len = m > 32 ? 32 : m;

  if (m <= 0 || n < m)
    return 0;

  BEGIN_PREPROCESSING
  memset(B, 0, sizeof(B));
  bit = 1U;
  for (i = filter_len - 1; i >= 0; i--) {
    B[x[i]] |= bit;
    bit <<= 1;
  }

  failure[0] = failure[1] = 0;
  for (i = 1; i < filter_len; i++) {
    k = failure[i];
    while (k > 0 && x[i] != x[k])
      k = failure[k];
    if (x[i] == x[k])
      k++;
    failure[i + 1] = k;
  }

  high_bit = 1U << (filter_len - 1);
  for (i = 0; i < filter_len; i++) {
    borders[i] = 0;
    k = i;
    do {
      borders[i] |= 1U << (filter_len - 1 - k);
      k = failure[k];
    } while (k > 0);
  }
  border = failure[filter_len];
  all_bits = filter_len == 32 ? ~0U : (1U << filter_len) - 1U;
  END_PREPROCESSING

  BEGIN_SEARCHING
  j = 0;
  count = 0;
  prefix = 0;
  while (j <= n - m) {
    k = filter_len - prefix;
    D = all_bits;
    matched_prefix = 0;

    for (step = 1; step <= k; step++) {
      D &= B[y[j + filter_len - step]];
      if (D == 0)
        break;
      if (D & high_bit)
        matched_prefix = step;
      if (step < k)
        D <<= 1;
    }

    if (D == 0) {
      j += filter_len - matched_prefix;
      prefix = matched_prefix;
    } else if (D & (1U << (k - 1))) {
      if (filter_len == m || memcmp(x + filter_len, y + j + filter_len,
                                    (size_t)(m - filter_len)) == 0)
        OUTPUT(j);
      j += filter_len - border;
      prefix = border;
    } else if ((X = D & borders[prefix]) != 0) {
      i = lowest_set_bit(X);
      prefix = filter_len - 1 - i + k;
      j += i - k + 1;
    } else {
      j += lowest_set_bit(D) - k + 1;
      prefix = 0;
    }
  }
  END_SEARCHING
  return count;
}
