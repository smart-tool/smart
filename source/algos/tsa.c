#include "include/define.h"
#include "include/main.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// searching
int search(unsigned char *P, int m, unsigned char *T, int n) {
  unsigned char PopCount[65536];
  int j, i;
  uint64_t D;
  uint64_t B[256];

  if (m > 64)
    return -1;

  BEGIN_PREPROCESSING
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(B, 0, 256 * 4);
  // TODO use __builtin_popcount()
  for (j = 0; j < m; ++j)
    B[P[j]] |= ((uint64_t)1U << (j));
  for (PopCount[i = 0] = 0; ++i <= 65535;
       PopCount[i] = PopCount[i & (i - 1)] + 1)
    ;
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

    count += PopCount[D & 0xffff];
    if (sizeof(D) > 2) {
      count += PopCount[(D >> 16) & 0xffff];
      if (sizeof(D) > 4) {
        count += PopCount[(D >> 32) & 0xffff];
        if (sizeof(D) > 6)
          count += PopCount[(D >> 48) & 0xffff];
      }
    }
  }
  END_SEARCHING
  return (count);
}
