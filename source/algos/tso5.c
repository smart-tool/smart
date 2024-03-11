// optimized word-wise popcount
#include "include/define.h"
#include "include/main.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Constraints: requires m<=64

// searching
int search(unsigned char *P, int m, unsigned char *T, int n) {
#ifndef HAVE_POPCOUNT
  unsigned char PopCount[65536];
#endif
  int j, i;
  uint64_t D;
  uint64_t mask = 0;
  uint64_t B[256], B1[256];

  if (m > 64)
    return -1;
  // memcpy(pat.pat+1, base, m);

  BEGIN_PREPROCESSING
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(B, 0, 256 * 4);
  mask = ~(uint64_t)0 >> (64 - m);
  for (j = 0; j < 256; B[j++] = ~(uint64_t)0)
    ;
  for (j = 0; j < m; ++j)
    B[P[j]] &= ~((uint64_t)1U << j);
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
    if ((D = (B[T[i]] | (B1[T[i - 2]] << 2) | (B1[T[i - 1]] << 1) |
              (B1[T[i + 1]] >> 1) | (B1[T[i + 2]] >> 2))) != ~(uint64_t)0) {
      j = 3;
      while ((j < m) && ((D |= ((B1[T[i - j]] << j) | (B1[T[i + j]] >> j))) !=
                         (~(uint64_t)0)))
        j++;

      // TODO: OUTPUT
#ifdef HAVE_POPCOUNTLL
      count += POPCOUNT64(~D);
#else
      if (D != ~(uint64_t)0) {
        count += POPCOUNT16((~D) & 0xffff);
        if (sizeof(D) > 2) {
          count += POPCOUNT16(((~D) >> 16) & 0xffff);
          if (sizeof(D) > 4) {
            count += POPCOUNT16(((~D) >> 32) & 0xffff);
            if (sizeof(D) > 6)
              count += POPCOUNT16(((~D) >> 48) & 0xffff);
          }
        }
      }
#endif
    }
  }
  END_SEARCHING
  return (count);
}
