/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012  Simone Faro and Thierry Lecroq
 *
 * The musl memmem implementation is licensed under
 * the standard MIT license.
 * Copyright © 2005-2020 Rich Felker, et al.
 * This is with a horspool skip.
 */

#include "include/define.h"
#include "include/main.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void preBmBc(unsigned char *P, int m, int bmBc[]) {
  int i;
  for (i = 0; i < SIGMA; i++)
    bmBc[i] = m;
  for (i = 0; i < m - 1; i++)
    bmBc[P[i]] = m - i - 1;
}

static char *twobyte_memmem(const unsigned char *h, size_t k,
                            const unsigned char *n) __nonnull_all {
  assert(h);
  assert(n);
  uint16_t nw = n[0] << 8 | n[1], hw = h[0] << 8 | h[1];
  for (h += 2, k -= 2; k; k--, hw = hw << 8 | *h++)
    if (hw == nw)
      return (char *)h - 2;
  return hw == nw ? (char *)h - 2 : 0;
}

static char *threebyte_memmem(const unsigned char *h, size_t k,
                              const unsigned char *n) __nonnull_all {
  assert(h);
  assert(n);
  uint32_t nw = (uint32_t)n[0] << 24 | n[1] << 16 | n[2] << 8;
  uint32_t hw = (uint32_t)h[0] << 24 | h[1] << 16 | h[2] << 8;
  for (h += 3, k -= 3; k; k--, hw = (hw | *h++) << 8)
    if (hw == nw)
      return (char *)h - 3;
  return hw == nw ? (char *)h - 3 : 0;
}

static char *fourbyte_memmem(const unsigned char *h, size_t k,
                             const unsigned char *n) __nonnull_all {
  assert(h);
  assert(n);
  uint32_t nw = (uint32_t)n[0] << 24 | n[1] << 16 | n[2] << 8 | n[3];
  uint32_t hw = (uint32_t)h[0] << 24 | h[1] << 16 | h[2] << 8 | h[3];
  for (h += 4, k -= 4; k; k--, hw = hw << 8 | *h++)
    if (hw == nw)
      return (char *)h - 4;
  return hw == nw ? (char *)h - 4 : 0;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define BITOP(a, b, op)                                                        \
  ((a)[(size_t)(b) / (8 * sizeof *(a))] op(size_t) 1                           \
   << ((size_t)(b) % (8 * sizeof *(a))))

static char *twoway_memmem(const unsigned char *h, const unsigned char *z,
                           const unsigned char *n, size_t l) __nonnull_all {
  size_t i, ip, jp, k, p, ms, p0, mem, mem0;
  size_t byteset[32 / sizeof(size_t)] = {0};
  size_t shift[256];
  assert(h);
  assert(n);

  BEGIN_PREPROCESSING
  /* Computing length of needle and fill shift table */
  for (i = 0; i < l; i++)
    BITOP(byteset, n[i], |=), shift[n[i]] = i + 1;

  /* Compute maximal suffix */
  ip = -1;
  jp = 0;
  k = p = 1;
  while (jp + k < l) {
    if (n[ip + k] == n[jp + k]) {
      if (k == p) {
        jp += p;
        k = 1;
      } else
        k++;
    } else if (n[ip + k] > n[jp + k]) {
      jp += k;
      k = 1;
      p = jp - ip;
    } else {
      ip = jp++;
      k = p = 1;
    }
  }
  ms = ip;
  p0 = p;

  /* And with the opposite comparison */
  ip = -1;
  jp = 0;
  k = p = 1;
  while (jp + k < l) {
    if (n[ip + k] == n[jp + k]) {
      if (k == p) {
        jp += p;
        k = 1;
      } else
        k++;
    } else if (n[ip + k] < n[jp + k]) {
      jp += k;
      k = 1;
      p = jp - ip;
    } else {
      ip = jp++;
      k = p = 1;
    }
  }
  if (ip + 1 > ms + 1)
    ms = ip;
  else
    p = p0;

  /* Periodic needle? */
  if (memcmp(n, n + p, ms + 1)) {
    mem0 = 0;
    p = MAX(ms, l - ms - 1) + 1;
  } else
    mem0 = l - p;
  mem = 0;
  END_PREPROCESSING

  /* Search loop */
  for (;;) {
    /* If remainder of haystack is shorter than needle, done */
    if (z - h < (ptrdiff_t)l)
      return 0;

    /* Check last byte first; advance by shift on mismatch */
    if (BITOP(byteset, h[l - 1], &)) {
      k = l - shift[h[l - 1]];
      if (k) {
        if (k < mem)
          k = mem;
        h += k;
        mem = 0;
        continue;
      }
    } else {
      h += l;
      mem = 0;
      continue;
    }

    /* Compare right half */
    for (k = MAX(ms + 1, mem); k < l && n[k] == h[k]; k++)
      ;
    if (k < l) {
      h += k - ms;
      mem = 0;
      continue;
    }
    /* Compare left half */
    for (k = ms + 1; k > mem && n[k - 1] == h[k - 1]; k--)
      ;
    if (k <= mem)
      return (char *)h;
    h += p;
    mem = mem0;
  }
}

static void *musl_memmem(const void *h0, size_t k, const void *n0,
                         size_t l) __nonnull_all {
  const unsigned char *h = h0, *n = n0;

  /* Use faster algorithms for short needles */
  h = memchr(h0, *n, k);
  if (!h || l == 1)
    return (void *)h;
  k -= h - (const unsigned char *)h0;
  if (k < l)
    return 0;
  if (l == 2)
    return twobyte_memmem(h, k, n);
  if (l == 3)
    return threebyte_memmem(h, k, n);
  if (l == 4)
    return fourbyte_memmem(h, k, n);

  return twoway_memmem(h, h + k, n, l);
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int bmBc[SIGMA];
#ifdef DEBUG
  const unsigned char *orig_y = y;
#endif

  /* Return immediately on empty needle */
  if (!m)
    return 0;
  /* Return immediately when needle is longer than haystack */
  if (n < m)
    return 0;

  BEGIN_PREPROCESSING
  /* Plain Horspool (hor.c) steps by bmBc[T[s+m-1]] from the CURRENT
     (unshifted) position s, even after reporting a match at s -- and
     since a match means T[s+m-1] == x[m-1], that shift is always the
     same constant bmBc[x[m-1]], safe because it is exactly what
     Horspool itself would do at that position. The previous version
     advanced to p+1 first and then looked up a shift keyed on a
     *different*, further-ahead byte (y[m-1] after the +1, i.e.
     text[p+m] instead of text[p+m-1]), compounding two skips that were
     never proven safe together -- it missed half the occurrences of a
     periodic pattern like "aaaaa" in a run of 'a's (8 found instead of
     16). Precompute the single safe post-match advance instead. */
  int adv = 1;
  if (m > 4) {
    preBmBc(x, m, bmBc);
    adv = bmBc[x[m - 1]];
  }
  END_PREPROCESSING

  BEGIN_SEARCHING
  int count = 0;
  unsigned char *p;
  while ((p = musl_memmem((char *)y, n, (char *)x, m))) {
    OUTPUT(p - orig_y);
    n -= (p + adv) - y;
    if (n < m)
      break;
    y = p + adv;
  }
  END_SEARCHING
  return count;
}
