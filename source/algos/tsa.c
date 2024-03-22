// Note: Does not support OUTPUT with the found pos yet, only the count
// Constraints: requires m<=64

#include "include/define.h"
#include "include/main.h"
#include "include/search_large.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// searching
int search(unsigned char *P, int m, unsigned char *T, int n) {
#ifndef HAVE_POPCOUNT
  unsigned char PopCount[65536];
#endif
  int j, i;
  uint64_t D;
  uint64_t B[256];

  if (m > 64)
    return search_large(P, m, T, n);

  BEGIN_PREPROCESSING
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(B, 0, 256 * 4);
  for (j = 0; j < m; ++j)
    B[P[j]] |= (UINT64_C(1) << j);
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
    D = B[T[i]];
    j = 1;
    while ((j < m) && (D &= (((B[T[i - j]] + 1) << j) - 1) &
                            ((B[T[i + j]] >> j) | ((~(uint64_t)0) << (m - j)))))
      j++;

    // TODO: OUTPUT
#ifdef HAVE_POPCOUNT64
    count += POPCOUNT64(D);
#else
    count += POPCOUNT16(D & 0xffff);
    if (sizeof(D) > 2) {
      count += POPCOUNT16((D >> 16) & 0xffff);
      if (sizeof(D) > 4) {
        count += POPCOUNT16((D >> 32) & 0xffff);
        if (sizeof(D) > 6)
          count += POPCOUNT16((D >> 48) & 0xffff);
      }
    }
#endif
  }
  END_SEARCHING
  return (count);
}
