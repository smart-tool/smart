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
 * Note: Broken!
 * Constraints: requires m > ?, may access past the haystack.
 */

#define MIN_M 2
#include "include/define.h"
#include "include/log2.h"
#include "include/main.h"
#include "include/search_small.h"
#include "include/search_large.h"

#if defined __ARM_NEON
/* NEON has no pcmpestr* equivalent; filter candidates with the first and
   last pattern bytes, then verify each candidate exactly. */
#include <arm_neon.h>

static int search_neon(unsigned char *x, int m, unsigned char *y, int n) {
  if (m < 2)
    return search_small(x, m, y, n);
  if (m > n)
    return 0;

  BEGIN_PREPROCESSING
  int count = 0;
  const uint8x16_t first = vdupq_n_u8(x[0]);
  const uint8x16_t last = vdupq_n_u8(x[m - 1]);
  END_PREPROCESSING

  BEGIN_SEARCHING
  int i = 0;
  const int limit = n - m;
  for (; i + 15 <= limit; i += 16) {
    uint8_t matches[16];
    const uint8x16_t starts = vld1q_u8(y + i);
    const uint8x16_t ends = vld1q_u8(y + i + m - 1);
    vst1q_u8(matches, vandq_u8(vceqq_u8(first, starts),
                               vceqq_u8(last, ends)));
    for (int j = 0; j < 16; j++)
      if (matches[j] && memcmp(x + 1, y + i + j + 1, m - 2) == 0)
        OUTPUT(i + j);
  }
  for (; i <= limit; i++)
    if (y[i] == x[0] && y[i + m - 1] == x[m - 1] &&
        memcmp(x + 1, y + i + 1, m - 2) == 0)
      OUTPUT(i);
  END_SEARCHING
  return count;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  return search_neon(x, m, y, n);
}
#else
#ifdef __SSE4_2__
#include <nmmintrin.h>
#else
#define SIMDE_ENABLE_NATIVE_ALIASES
#include "simde/x86/sse4.2.h"
//#define __m128i           simde__m128i
//#define _mm_loadu_si128   simde_mm_loadu_si128
//#define _mm_extract_epi32 simde_mm_extract_epi32
//#define _mm_set_epi8      simde_mm_set_epi8
//#define _mm_and_si128     simde_mm_and_si128
//#define _mm_cmpestrm      simde_mm_cmpestrz
//#define _mm_cmpestri      simde_mm_cmpestrs
//#define _mm_cmpestrc      simde_mm_cmpestrz
//#define _mm_popcnt_u32    __builtin_popcount
//#define _SIDD_UBYTE_OPS         SIMDE_SIDD_UBYTE_OPS
//#define _SIDD_CMP_EQUAL_ORDERED SIMDE_SIDD_CMP_EQUAL_ORDERED
//#define _SIDD_CMP_EQUAL_EACH    SIMDE_SIDD_CMP_EQUAL_EACH
//#define _SIDD_NEGATIVE_POLARITY SIMDE_SIDD_NEGATIVE_POLARITY
#endif

#include <assert.h>
#include <stdint.h>

// BEWARE: code sometimes has access to load bytes after end of string.

const int max_needle = 8;

/* pcmpestri only compares `len` bytes, but the 16-byte register load
   itself can read up to 15 bytes past the compared range. When len <= 16
   copy through a zero-padded local buffer so the load stays in bounds
   (the pattern allocation only has PAD_16(m+1) bytes of slack); when
   len > 16 the operand must stay memory-backed because the instruction
   reads bytes 16..len-1 from the operand address, and those loads are in
   bounds by construction (head + 15 < m). */
static inline __m128i load_sse_padded(const unsigned char *p, unsigned int len) {
  if (len > 16)
    return _mm_loadu_si128((const __m128i *)p);
  unsigned char buf[16] = {0};
  memcpy(buf, p, len);
  return _mm_loadu_si128((const __m128i *)buf);
}


// pattern up to length max_needle using raw intel sse4.2 instructions: should
// max_needle be longer?
int search_rawsse(unsigned char *x, int m, unsigned char *y, int n) {
  if (m > max_needle)
    return -1;

  int count = 0;
  unsigned char *cur = y; // walking pointer; y stays the origin for OUTPUT
  __m128i needle_reg = _mm_loadu_si128((__m128i *)x);

  int step = 16 - m + 1; // experiment with aligned text blocks: step = 8.
  uint32_t result_mask = (1U << step) - 1;

  int number_of_steps = (n + step - 16) / step;
  int steps_size = number_of_steps * step;
  unsigned char *steps_end = cur + steps_size;

  while (cur != steps_end) { // full 16 bytes
    __m128i haystack_reg = _mm_loadu_si128((__m128i *)cur);
    __m128i mask_reg = _mm_cmpestrm(needle_reg, m, haystack_reg, 16,
                                    _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED);
    uint32_t mask = _mm_extract_epi32(mask_reg, 0);
#ifdef DEBUG
    // each set bit b marks a match starting at (cur - y) + b
    uint32_t hits = mask & result_mask;
    while (hits) {
      int b = __builtin_ctz(hits);
      OUTPUT((int)(cur - y) + b);
      hits &= hits - 1;
    }
#else
    count += _mm_popcnt_u32(mask & result_mask);
#endif
    cur += step;
  }

  n -= steps_size; // remainder under 16 bytes
  if (n >= m) {
    /* fewer than 16 bytes may remain: the plain 16-byte load over-reads
       the text allocation (which only has PAD_16(n+m+1) bytes) */
    __m128i haystack_reg = load_sse_padded(cur, (unsigned int)n);
    __m128i mask_reg = _mm_cmpestrm(needle_reg, m, haystack_reg, n,
                                    _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED);
    step = n - m + 1;
    result_mask = (1U << step) - 1;
    uint32_t mask = _mm_extract_epi32(mask_reg, 0);
#ifdef DEBUG
    uint32_t hits = mask & result_mask;
    while (hits) {
      int b = __builtin_ctz(hits);
      OUTPUT((int)(cur - y) + b);
      hits &= hits - 1;
    }
#else
    count += _mm_popcnt_u32(mask & result_mask);
#endif
  }

  return count;
}

/* copied from tw.c */
/* Computing of the maximal suffix for <= */
int maxSuf(char *x, int m, int *p) {
  int ms, j, k;
  char a, b;

  ms = -1;
  j = 0;
  k = *p = 1;
  while (j + k < m) {
    a = x[j + k];
    b = x[ms + k];
    if (a < b) {
      j += k;
      k = 1;
      *p = j - ms;
    } else if (a == b)
      if (k != *p)
        ++k;
      else {
        j += *p;
        k = 1;
      }
    else { /* a > b */
      ms = j;
      j = ms + 1;
      k = *p = 1;
    }
  }
  return (ms);
}

/* copied from tw.c */
/* Computing of the maximal suffix for >= */
int maxSufTilde(char *x, int m, int *p) {
  int ms, j, k;
  char a, b;

  ms = -1;
  j = 0;
  k = *p = 1;
  while (j + k < m) {
    a = x[j + k];
    b = x[ms + k];
    if (a > b) {
      j += k;
      k = 1;
      *p = j - ms;
    } else if (a == b)
      if (k != *p)
        ++k;
      else {
        j += *p;
        k = 1;
      }
    else { /* a < b */
      ms = j;
      j = ms + 1;
      k = *p = 1;
    }
  }
  return (ms);
}

/* copied from kmp.c */
void preKmp(unsigned char *x, int m, int kmpNext[]) {
  int i, j;
  i = 0;
  j = kmpNext[0] = -1;
  while (i < m) {
    while (j > -1 && x[i] != x[j])
      j = kmpNext[j];
    i++;
    j++;
    if (i < m && x[i] == x[j])
      kmpNext[i] = kmpNext[j];
    else
      kmpNext[i] = j;
  }
}

/* compute period length and critical factorization */
/* if CP does not find exact period length then use KMP */
void compute(unsigned char *x, int m, int *mu, int *pi) {
  int i, j, ell, p, per, q;

  /* Preprocessing */
  i = maxSuf((char *)x, m, &p);
  j = maxSufTilde((char *)x, m, &q);
  if (i > j) {
    ell = i;
    per = p;
  } else {
    ell = j;
    per = q;
  }

  *mu = ell + 1;

  if (!memcmp(x, x + per, m - per)) {
    *pi = per;
  } else {
    int kmpNext[XSIZE];
    preKmp(x, m, kmpNext);
    *pi = m - kmpNext[m];
  }
}

// algorithm is not exactly like paper- allow overflow anchor match.
// TODO: double up loops to avoid the one comparison for the length


int search(unsigned char *x, int m, unsigned char *y, int n) {
  if (m <= max_needle) // raw SSE instructions for short patterns
    return search_rawsse(x, m, y, n);

  int mu, pi, count = 0;
  unsigned char *cur = y; // walking pointer; y stays the origin for OUTPUT
  compute(x, m, &mu, &pi);
  /* compute() can return a degenerate factorization (garbage or non-positive
     period from the maxSuf locals on some patterns; the asserts below are
     stripped under -DNDEBUG). A zero period makes the shift-by-pi loop below
     spin forever on periodic input, so fall back to an exact search. */
  if (mu <= 0 || pi <= 0)
    return (m < 32) ? search_small(x, m, y, n) : search_large(x, m, y, n);

  // safety - remove?
  assert(mu > 0);
  assert(mu < pi && mu < m && pi > 0 && pi <= m);

  // SSE instruction search anchor block after critical factorization;
  __m128i needle_reg;
  int needle_length = m - mu; // maximum length
  if (needle_length > 16) {
    needle_reg = _mm_loadu_si128((__m128i *)&x[mu]);
    needle_length = 16;
  } else {
    /* pcmpestri with an explicit needle length only ever compares from
       byte 0 of the register, so loading x[m-16] and masking (the old
       code masked with the TEXT length n, i.e. never) left the needle
       bytes at the wrong offset and silently missed occurrences -- and
       loading straight from x[mu] would over-read the pattern
       allocation for some m. Copy the needle suffix into a zero-padded
       local buffer instead: always the right bytes, always in bounds. */
    unsigned char needle_buf[16] = {0};
    memcpy(needle_buf, &x[mu], (size_t)needle_length);
    needle_reg = _mm_loadu_si128((__m128i *)needle_buf);
  }

loop: // optimize further !
  while (n >= m) {
    // search for first part immediately after critical factorization to anchor
    __m128i haystack_reg = _mm_loadu_si128((__m128i *)&cur[mu]);

    int haystack_length = n - mu;
    if (haystack_length > 16)
      haystack_length = 16;

    int idx =
        _mm_cmpestri(needle_reg, needle_length, haystack_reg, haystack_length,
                     _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED);
    if (!_mm_cmpestrc(needle_reg, needle_length, haystack_reg, haystack_length,
                      _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED)) {
      cur += haystack_length;
      n -= haystack_length;

      goto loop;
    }

    cur += idx;
    n -= idx;
    if (n < m) // anchor at idx; break loop;
      break;

    int head0 = 0, head;

    // compare remainder after critical factorization and after anchor.
    if (needle_length <= haystack_length - idx) {
      // full occurrence of needle
      head = mu + needle_length;
    } else {
      // overflowing occurrence of needle: idx < haystack_length
      head = mu + haystack_length - idx;
    }

  loop1:
    // compare remainder as above; or after shift by period with head =
    // max(mu,m-pi) this is memcmp with index of first mismatch.
    while (head < m) {
      unsigned int b = m - head;

      __m128i b0 = load_sse_padded(&x[head], b),
              b1 = load_sse_padded(&cur[head], b);

      unsigned int idx = _mm_cmpestri(b0, b, b1, b,
                                      _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH |
                                          _SIDD_NEGATIVE_POLARITY);
      if (_mm_cmpestrc(b0, b, b1, b,
                       _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH |
                           _SIDD_NEGATIVE_POLARITY)) {
        // mismatch
        head += idx + 1;
        int step = head - mu;
        cur += step;
        n -= step;

        goto loop; // continue main loop
      }

      head += idx;
      /*
        if (b > 16)
          head += 16;
        else
          head += b;
      */
    }

    // loop2:
    //  now compare prefix up to mu: this is just memcmp.
    while (head0 < mu) {
      unsigned int b = mu - head0;

      __m128i b0 = load_sse_padded(&x[head0], b),
              b1 = load_sse_padded(&cur[head0], b);

      unsigned int idx = _mm_cmpestri(b0, b, b1, b,
                                      _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH |
                                          _SIDD_NEGATIVE_POLARITY);
      if (_mm_cmpestrc(b0, b, b1, b,
                       _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH |
                           _SIDD_NEGATIVE_POLARITY)) {
        // mismatch
        goto loop3;
      }

      head0 += idx;
      /*
        if (b > 16)
          head0 += 16;
        else
          head0 += b;
      */
    }

    // first part match as well: got an OCCURRENCE !
#ifdef DEBUG
    OUTPUT((int)(cur - y));
#else
    count++;
#endif

  loop3:
    // shift by pi
    cur += pi;
    n -= pi;
    head = m - pi;
    if (head < mu) { // odd: can do better probably
      head0 = head;
      head = mu;
    }

    goto loop1;
  }

  return count;
}
#endif
