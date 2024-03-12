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
 * download the tool at: http://www.dmi.unict.it/~faro/smart/
 *
 */

#include "include/define.h"
#include "include/main.h"

#include <stdint.h>

static char *twobyte_strstr(const unsigned char *h, const unsigned char *n) {
  uint16_t nw = n[0] << 8 | n[1], hw = h[0] << 8 | h[1];
  for (h++; *h && hw != nw; hw = hw << 8 | *++h)
    ;
  return *h ? (char *)h - 1 : 0;
}

static char *threebyte_strstr(const unsigned char *h, const unsigned char *n) {
  uint32_t nw = (uint32_t)n[0] << 24 | n[1] << 16 | n[2] << 8;
  uint32_t hw = (uint32_t)h[0] << 24 | h[1] << 16 | h[2] << 8;
  for (h += 2; *h && hw != nw; hw = (hw | *++h) << 8)
    ;
  return *h ? (char *)h - 2 : 0;
}

static char *fourbyte_strstr(const unsigned char *h, const unsigned char *n) {
  uint32_t nw = (uint32_t)n[0] << 24 | n[1] << 16 | n[2] << 8 | n[3];
  uint32_t hw = (uint32_t)h[0] << 24 | h[1] << 16 | h[2] << 8 | h[3];
  for (h += 3; *h && hw != nw; hw = hw << 8 | *++h)
    ;
  return *h ? (char *)h - 3 : 0;
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define BITOP(a, b, op)                                                        \
  ((a)[(size_t)(b) / (8 * sizeof *(a))] op(size_t) 1                           \
   << ((size_t)(b) % (8 * sizeof *(a))))

static char *twoway_strstr(const unsigned char *h, const unsigned char *n) {
  const unsigned char *z;
  size_t l, ip, jp, k, p, ms, p0, mem, mem0;
  size_t byteset[32 / sizeof(size_t)] = {0};
  size_t shift[256];

  /* Computing length of needle and fill shift table */
  for (l = 0; n[l] && h[l]; l++)
    BITOP(byteset, n[l], |=), shift[n[l]] = l + 1;
  if (n[l])
    return 0; /* hit the end of h */

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

  /* Initialize incremental end-of-haystack pointer */
  z = h;

  /* Search loop */
  for (;;) {
    /* Update incremental end-of-haystack pointer */
    if (z - h < (intptr_t)l) {
      /* Fast estimate for MAX(l,63) */
      size_t grow = l | 63;
      const unsigned char *z2 = memchr(z, 0, grow);
      if (z2) {
        z = z2;
        if (z - h < (intptr_t)l)
          return 0;
      } else
        z += grow;
    }

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
    for (k = MAX(ms + 1, mem); n[k] && n[k] == h[k]; k++)
      ;
    if (n[k]) {
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

static char *musl_strstr(const char *h, const char *n) {
  /* Return immediately on empty needle */
  if (!n[0])
    return (char *)h;

  /* Use faster algorithms for short needles */
  h = strchr(h, *n);
  if (!h || !n[1])
    return (char *)h;
  if (!h[1])
    return 0;
  if (!n[2])
    return twobyte_strstr((void *)h, (void *)n);
  if (!h[2])
    return 0;
  if (!n[3])
    return threebyte_strstr((void *)h, (void *)n);
  if (!h[3])
    return 0;
  if (!n[4])
    return fourbyte_strstr((void *)h, (void *)n);

  return twoway_strstr((void *)h, (void *)n);
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  //const unsigned char *orig_y = y;
  (void)m; (void)n;
  BEGIN_PREPROCESSING
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  int count = 0;
  unsigned char *p;
  while ((p = (unsigned char *)musl_strstr((char *)y, (char *)x))) {
    OUTPUT(p - orig_y);
    y = p + 1; // can be optimized
  }
  END_SEARCHING
  return count;
}
