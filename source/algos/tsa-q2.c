// Note: Does not support OUTPUT with the found pos yet, only the count
// Note: Broken!
// Constraints: requires m>=2, m<=64. requires T[n + 1] to be accessable.

#include "include/define.h"
#include "include/main.h"
#include "include/search_small.h"
#include "include/search_large.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define Q 2
#define HS(x, i) (x[i] << 1) + x[i + 1]

// searching
int search(unsigned char *P, int m, unsigned char *T, int n) {
#ifndef HAVE_POPCOUNT
  unsigned char PopCount[65536];
#endif
  int j, i;
  uint64_t D;
  uint64_t B[512];

  if (m > 64)
    return search_large(P, m, T, n);
  if (m < 2)
    return search_small(P, m, T, n);

  BEGIN_PREPROCESSING
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(B, 0, 512 * 8);
  for (j = 0; j < m - Q + 1; ++j) {
    assert(j + 1 <= m);
    B[HS(P, j)] |= (UINT64_C(1) << (j));
  }
  // for (j=0; j<256; ++j) B1[j] = B[j]+1;
#ifndef HAVE_POPCOUNT
  for (PopCount[i = 0] = 0; ++i <= 65535;
       PopCount[i] = PopCount[i & (i - 1)] + 1)
    ;
#endif
  int count = 0;
  END_PREPROCESSING

  BEGIN_SEARCHING
  T[n + 1] = 255; // sentinel
  for (i = m - Q; i <= n - 1; i += m) {
    assert(i + 1 <= n);
    D = B[HS(T, i)];
    j = 1;
    assert(i + j + 1 <= n);
    while ((j < m - Q + 1) &&
           (D &= (((B[HS(T, i - j)] + 1) << j) - 1) &
            ((B[HS(T, i + j)] >> j) | (~UINT64_C(0) << (m - j))))) {
      j++;
      assert((j < m - Q + 1) || (i + j + 1 <= n));
    }

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
