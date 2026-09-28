/* Reports found positions in DEBUG builds (OUTPUT per surviving bit k
   of D: occurrence starts at anchor i - k); release builds count only.
   Note: Needed 4 fixes
  1. Stack buffer overflow: HS() hash could reach 765 for high-byte
     characters, indexing B[512] out of bounds on any binary/random
     input.
  2. Coverage gap: stride i += m left Q-1 text positions
     between anchors unexamined for any Q > 1 (only worked by
     coincidence for the Q=1 sibling tsa.c, where window width equals
     stride).
  3. Killed occurrence bit: the forward mask's exponent was
     off by Q-1, always discarding the bit that signals a match
     starting at the anchor's leftmost position (s=0, m, 2m, ...).
  4. Design gap for m==Q: no verification loop runs at all in that
     case, so a match would depend purely on lossy-hash luck — routed
     to the exact search_small fallback instead.
  Constraints: requires m>=2, m<=64. requires T[n + 1] to be accessable.
*/

#define MIN_M 2
#define MAX_M 64
#include "include/define.h"
#include "include/main.h"
#include "include/search_small.h"
#include "include/search_large.h"

#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define Q 2
#define HS(x, i) (((x[i] << 1) + x[i + 1]) & 511)

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
  uint64_t B[512];

  if (m > 64)
    return search_large(P, m, T, n);
  /* m == Q means the inner backward/forward refinement loop never runs
     (j < m - Q + 1 == j < 1 is false immediately), so the match would
     rely solely on the lossy HS hash (16 bits folded into 9) with no
     verification at all -- any hash collision is an unconditional false
     positive. Route to an exact matcher instead. */
  if (m <= Q)
    return search_small(P, m, T, n);

  BEGIN_PREPROCESSING
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(B, 0, 512 * 8);
  for (j = 0; j < m - Q + 1; ++j) {
    assert(j + 1 <= m);
    assert(j < 64);
    assert(HS(P, j) < 512);
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
  /* each anchor i only ever accumulates bits for s in [i-(m-Q), i], a
     window of width m-Q+1; striding by m (as the Q=1 sibling tsa.c
     correctly does, where the window width m-Q+1 == m) leaves a gap of
     Q-1 uncovered text positions between consecutive anchors for any
     Q > 1, silently skipping real occurrences that start in the gap. */
  for (i = m - Q; i <= n - 1; i += m - Q + 1) {
    assert(i + 1 <= n);
    D = B[HS(T, i)];
    j = 1;
    /* the forward mask must account for the Q-gram width: without the
       "- Q + 1" term, the highest occurrence bit (k = m - Q, an
       occurrence starting at the anchor's leftmost valid position) gets
       killed at j=1 before the mask ever reaches it, so occurrences at
       s=0, s=m, s=2m, ... were silently missed (was "~0 << (m - j)"). */
    while ((j < m - Q + 1) &&
           (D &= (((B[HS(T, i - j)] + 1) << j) - 1) &
                 ((B[HS(T, i + j)] >> j) |
                  (~UINT64_C(0) << (m - j - Q + 1))))) {
      j++;
    }

    // each surviving bit k of D marks an occurrence starting at i - k
#ifdef DEBUG
    uint64_t M = D & ((UINT64_C(1) << (m - Q + 1)) - 1);
    while (M) {
      int k = __builtin_ctzll(M);
      OUTPUT(i - k);
      M &= M - 1;
    }
#else
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
#endif
  }
  END_SEARCHING
  return (count);
}
