/*
 * From http://0x80.pl/articles/simd-strfind.html
 * A generic SSE2 Karp-Rabin-like string search with first and last bytes
 * as rolling hash.
 *
 * Copyright (c) 2008-2016, Wojciech Muła
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "include/define.h"
#include "include/main.h"

#if defined __AVX2__

#include <assert.h>
#include <immintrin.h>
#include <stdint.h>
#define FORCE_INLINE inline __attribute__((always_inline))

static FORCE_INLINE int
avx2_strstr_generic(const unsigned char *needle, int m,
                    const unsigned char *s, int n) {

  assert(m > 1);
  assert(n > 0);
  const __m256i first = _mm256_set1_epi8(needle[0]);
  const __m256i last = _mm256_set1_epi8(needle[m - 1]);
  for (int i = 0; i < n; i += 64) {

    const __m256i block_first1 = _mm256_loadu_si256((const __m256i *)(s + i));
    const __m256i block_last1 =
        _mm256_loadu_si256((const __m256i *)(s + i + m - 1));

    const __m256i block_first2 =
        _mm256_loadu_si256((const __m256i *)(s + i + 32));
    const __m256i block_last2 =
        _mm256_loadu_si256((const __m256i *)(s + i + m - 1 + 32));

    const __m256i eq_first1 = _mm256_cmpeq_epi8(first, block_first1);
    const __m256i eq_last1 = _mm256_cmpeq_epi8(last, block_last1);

    const __m256i eq_first2 = _mm256_cmpeq_epi8(first, block_first2);
    const __m256i eq_last2 = _mm256_cmpeq_epi8(last, block_last2);

    const uint32_t mask1 =
        _mm256_movemask_epi8(_mm256_and_si256(eq_first1, eq_last1));
    const uint32_t mask2 =
        _mm256_movemask_epi8(_mm256_and_si256(eq_first2, eq_last2));
    uint64_t mask = mask1 | ((uint64_t)mask2 << 32);

    while (mask != 0) {
      const int bitpos = __builtin_ctzll(mask);
      if (memcmp(s + i + bitpos + 1, needle + 1, m - 2) == 0) {
        return i + bitpos;
      }
      mask = mask & (mask - 1); // clear_leftmost_set
    }
  }
  return -1;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  return avx2_strstr_generic(x, m, y, n);
}

#elif defined __SSE2__

#include <assert.h>
#include <nmmintrin.h>
#include <stdint.h>
#define FORCE_INLINE inline __attribute__((always_inline))

static FORCE_INLINE int
sse2_strstr_generic(const unsigned char *needle, int m,
                    const unsigned char *s, int n) {
  assert(m > 1);
  assert(n > 0);

  const __m128i first = _mm_set1_epi8(needle[0]);
  const __m128i last = _mm_set1_epi8(needle[m - 1]);

  for (int i = 0; i < n; i += 16) {
    const __m128i block_first = _mm_loadu_si128((const __m128i *)(s + i));
    const __m128i block_last =
        _mm_loadu_si128((const __m128i *)(s + i + m - 1));

    const __m128i eq_first = _mm_cmpeq_epi8(first, block_first);
    const __m128i eq_last = _mm_cmpeq_epi8(last, block_last);
    uint16_t mask = _mm_movemask_epi8(_mm_and_si128(eq_first, eq_last));

    while (mask != 0) {
      const uint16_t bitpos = __builtin_ctz(mask);
      // the first byte and last byte already match
      if (memcmp(s + i + bitpos + 1, needle + 1, m - 2) == 0) {
        return i + bitpos;
      }
      mask = mask & (mask - 1); // clear_leftmost_set
    }
  }
  return -1;
}

#if 0
/* A Karp-Rabin string search with 8 MPSADBW (Manhattan distances) between
 * given 4-byte sub-vector from one register and eight subsequent 4-byte
 * sub-vector from second register.
 */
static size_t sse4_strstr_size4(const unsigned char *needle, int m,
                                const unsigned char *s, int text_size) {
    assert(m > 4);
    assert(text_size > 0);
    size_t needle_size = (size_t)m;
    size_t n = (size_t)text_size;
    const __m128i prefix = _mm_loadu_si128((const __m128i *)needle);
    const __m128i zeros = _mm_setzero_si128();
    for (size_t i = 0; i < n; i += 8) {
      const __m128i data = _mm_loadu_si128((const __m128i *)(s + i));
      const __m128i result = _mm_mpsadbw_epu8(data, prefix, 0);
      const __m128i cmp = _mm_cmpeq_epi16(result, zeros);
      unsigned mask = _mm_movemask_epi8(cmp) & 0x5555;

      while (mask != 0) {
        const auto bitpos = __builtin_ctz(mask) / 2;
        if (memcmp(s + i + bitpos + 4, needle + 4, needle_size - 4) == 0) {
          OUTPUT(i + bitpos);
        }
      mask = mask & (mask - 1); // clear_leftmost_set
    }
  }
  return -1;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  return sse2_strstr_generic(x, m, y, n);
}

#endif

#endif
