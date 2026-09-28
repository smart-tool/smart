// optimized word-wise popcount
// Constraints: requires m>=2 && m<=64

#define MIN_M 2
#define MAX_M 64
#include "include/define.h"
#include "include/main.h"
#include "include/search_large.h"
#include "include/search_small.h"

#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

// searching
int search(unsigned char *P, int m, unsigned char *T, int n) {
#ifdef DEBUG
  unsigned char *y = T; // alias so OUTPUT can print positions
#endif
#ifndef HAVE_POPCOUNT
  unsigned char PopCount[65536];
#endif
  int j, i;
  uint64_t D;
  uint64_t mask = 0;
  uint64_t B[256], B1[256];

  if (m > 64)
    return search_large(P, m, T, n);
  ;
  if (m < 2)
    return search_small(P, m, T, n);
  ;
  // memcpy(pat.pat+1, base, m);

  BEGIN_PREPROCESSING
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(B, 0, 256 * 8);
  mask = ~UINT64_C(0) >> (64 - m);
  //uint64_t def = ~UINT64_C(0) & mask;
  for (j = 0; j < 256; B[j++] = ~UINT64_C(0))
    ;
  for (j = 0; j < m; ++j)
    B[P[j]] &= ~(UINT64_C(1) << j);
  for (j = 0; j < 256; j++)
    B1[j] = (B[j] & mask);
#ifndef HAVE_POPCOUNT
  for (PopCount[i = 0] = 0; ++i <= 65535;
       PopCount[i] = PopCount[i & (i - 1)] + 1)
    ;
#endif
  int count = 0;
  END_PREPROCESSING

  BEGIN_SEARCHING
  T[n + 1] = 255; // sentinel
  for (i = m - 1; i <= n - 1; i += m) {
    //assert(i - 2 >= 0);
    //assert(i + 2 < n);
    if ((D = (B[T[i]] | ((i - 2 >= 0) ? (B1[T[i - 2]] << 2) : 0) |
              ((i - 1 >= 0) ? (B1[T[i - 1]] << 1) : 0) | (B1[T[i + 1]] >> 1) |
              ((i + 2 <= n) ? (B1[T[i + 2]] >> 2) : 0))) != ~UINT64_C(0)) {
      j = 3;
      //assert(i - j >= 0);
      //assert(i + j < n);
      while ((j < m) &&
             ((D |= ((i - j >= 0 ? B1[T[i - j]] << j : 0) |
                     (i + j < n ? B1[T[i + j]] >> j : 0))) != (~UINT64_C(0)))) {
        j++;
        //assert(j < m && i - j >= 0);
        //assert(j < m && i + j < n);
      }

      // each zero bit k of D (within the m-bit mask) marks an occurrence
      // starting at i - k
#ifdef DEBUG
      uint64_t M = ~D & mask;
      while (M) {
        int k = __builtin_ctzll(M);
        int pos = i - k;
        if (pos <= n - m)
          OUTPUT(pos);
        M &= M - 1;
      }
#else
#ifdef HAVE_POPCOUNTLL
      count += POPCOUNT64(~D & mask);
#else
      if (D != ~UINT64_C(0)) {
        uint64_t M = ~D & mask;
        count += POPCOUNT16(M & 0xffff);
        if (sizeof(D) > 2) {
          count += POPCOUNT16((M >> 16) & 0xffff);
          if (sizeof(D) > 4) {
            count += POPCOUNT16((M >> 32) & 0xffff);
            if (sizeof(D) > 6)
              count += POPCOUNT16((M >> 48) & 0xffff);
          }
        }
      }
#endif
#endif
    }
  }
  END_SEARCHING
  return (count);
}
