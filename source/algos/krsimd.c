/*
 * SMART: string matching algorithms research tool.
 *
 * Exact polynomial Rabin-Karp with an AVX2 chunked eight-lane rolling hash.
 * Hash matches are always verified with memcmp.  Unlike simdkr.c, which is
 * an endpoint-byte SIMD filter, this computes a 32-bit polynomial hash over
 * every pattern-length window.
 *
 * The AVX2 schedule follows M. Sills, "Optimizing Rabin-Karp Hashing",
 * https://mattsills.github.io/2024/03/02/rabin-karp/ (2024): eight independent
 * rolling hash streams avoid the scalar loop-carried multiplication chain.
 */

#include "include/define.h"
#include "include/main.h"

#include <limits.h>
#include <stdint.h>

#define KRSIMD_BASE UINT32_C(257)

static uint32_t polynomial_hash(const unsigned char *text, int length) {
  uint32_t hash = 0;
  int i;

  for (i = 0; i < length; i++)
    hash = hash * KRSIMD_BASE + text[i];
  return hash;
}

static int scalar_search(unsigned char *x, int m, unsigned char *y, int n) {
  uint32_t factor = 1;
  uint32_t pattern_hash;
  uint32_t text_hash;
  int count = 0;
  int i;

  BEGIN_PREPROCESSING
  for (i = 0; i < m; i++)
    factor *= KRSIMD_BASE;
  pattern_hash = polynomial_hash(x, m);
  text_hash = polynomial_hash(y, m);
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (i = 0; i <= n - m; i++) {
    if (text_hash == pattern_hash && !memcmp(x, y + i, (size_t)m))
      OUTPUT(i);
    if (i < n - m)
      text_hash = text_hash * KRSIMD_BASE + y[i + m] - factor * y[i];
  }
  END_SEARCHING
  return count;
}

#if defined(__AVX2__)

#include <immintrin.h>

static inline __m256i byte0(__m256i words) {
  return _mm256_and_si256(words, _mm256_set1_epi32(0xff));
}

static inline __m256i byte1(__m256i words) {
  return _mm256_and_si256(_mm256_srli_epi32(words, 8),
                          _mm256_set1_epi32(0xff));
}

static inline __m256i byte2(__m256i words) {
  return _mm256_and_si256(_mm256_srli_epi32(words, 16),
                          _mm256_set1_epi32(0xff));
}

static inline __m256i byte3(__m256i words) {
  return _mm256_and_si256(_mm256_srli_epi32(words, 24),
                          _mm256_set1_epi32(0xff));
}

static int avx2_search(unsigned char *x, int m, unsigned char *y, int n) {
  const size_t subblock = (size_t)m * 4;
  const size_t block = subblock * 8;
  const __m256i base = _mm256_set1_epi32(KRSIMD_BASE);
  uint32_t factor = 1;
  const uint32_t pattern_hash = polynomial_hash(x, m);
  const __m256i target = _mm256_set1_epi32((int)pattern_hash);
  const __m256i offsets = _mm256_set_epi32(
      (int)(subblock * 7), (int)(subblock * 6), (int)(subblock * 5),
      (int)(subblock * 4), (int)(subblock * 3), (int)(subblock * 2),
      (int)subblock, 0);
  size_t window = 0;
  int count = 0;
  int i;

  BEGIN_PREPROCESSING
  for (i = 0; i < m; i++)
    factor *= KRSIMD_BASE;
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (; window + block + (size_t)m <= (size_t)n; window += block + 1) {
    __m256i hashes = _mm256_setzero_si256();
    const __m256i factor_vector = _mm256_set1_epi32((int)factor);
    size_t offset;

    for (i = 0; i < m; i++) {
      const __m256i words = _mm256_i32gather_epi32(
          (const int *)(y + window + (size_t)i), offsets, 1);
      hashes = _mm256_add_epi32(_mm256_mullo_epi32(hashes, base),
                                 byte0(words));
    }
    if ((uint32_t)_mm256_extract_epi32(hashes, 0) == pattern_hash &&
        !memcmp(x, y + window, (size_t)m))
      OUTPUT((int)window);

    for (offset = 0; offset < subblock; offset += 4) {
      const __m256i incoming = _mm256_i32gather_epi32(
          (const int *)(y + window + offset + (size_t)m), offsets, 1);
      const __m256i outgoing = _mm256_i32gather_epi32(
          (const int *)(y + window + offset), offsets, 1);
      const __m256i incoming_bytes[] = {byte0(incoming), byte1(incoming),
                                         byte2(incoming), byte3(incoming)};
      const __m256i outgoing_bytes[] = {byte0(outgoing), byte1(outgoing),
                                         byte2(outgoing), byte3(outgoing)};
      int byte_offset;

      for (byte_offset = 0; byte_offset < 4; byte_offset++) {
        uint32_t matches;

        hashes = _mm256_sub_epi32(
            _mm256_add_epi32(_mm256_mullo_epi32(hashes, base),
                             incoming_bytes[byte_offset]),
            _mm256_mullo_epi32(outgoing_bytes[byte_offset], factor_vector));
        matches = (uint32_t)_mm256_movemask_ps(
            _mm256_castsi256_ps(_mm256_cmpeq_epi32(hashes, target)));
        while (matches != 0) {
          const unsigned lane = (unsigned)__builtin_ctz(matches);
          const size_t position =
              window + (size_t)lane * subblock + offset + byte_offset + 1;

          if (!memcmp(x, y + position, (size_t)m))
            OUTPUT((int)position);
          matches &= matches - 1;
        }
      }
    }
  }

  if (window <= (size_t)n - (size_t)m) {
    uint32_t text_hash = polynomial_hash(y + window, m);

    for (; window <= (size_t)n - (size_t)m; window++) {
      if (text_hash == pattern_hash && !memcmp(x, y + window, (size_t)m))
        OUTPUT((int)window);
      if (window < (size_t)n - (size_t)m)
        text_hash = text_hash * KRSIMD_BASE + y[window + (size_t)m] -
                    factor * y[window];
    }
  }
  END_SEARCHING
  return count;
}

#endif

int search(unsigned char *x, int m, unsigned char *y, int n) {
  if (m <= 0 || n < m)
    return 0;
#if defined(__AVX2__)
  if (m <= INT_MAX / 4)
    return avx2_search(x, m, y, n);
#endif
  return scalar_search(x, m, y, n);
}
