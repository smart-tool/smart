// fallback for algos with m<32 or similar constraints
// from musl memmem
#include <stddef.h>
#include <stdint.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define BITOP(a, b, op)                                                        \
  ((a)[(size_t)(b) / (8 * sizeof *(a))] op(size_t) 1                           \
   << ((size_t)(b) % (8 * sizeof *(a))))

static unsigned char *twoway_memmem(const unsigned char *h,
                                    const unsigned char *z,
                                    const unsigned char *n, size_t l) {
  size_t i, ip, jp, k, p, ms, p0, mem, mem0;
  size_t byteset[32 / sizeof(size_t)] = {0};
  size_t shift[256];

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
    if (z - h < (ptrdiff_t)l) {
      return NULL;
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
    if (k <= mem) {
      return (unsigned char *)h;
    }
    h += p;
    mem = mem0;
  }
}

//static inline void *musl_memmem(const void *h0, size_t k, const void *n0, size_t l) {
//  const unsigned char *h = h0, *n = n0;
//  return twoway_memmem(h, h + k, n, l);
//}

int search_large(unsigned char *x, int m, unsigned char *y, int n) {
  BEGIN_SEARCHING
  //const unsigned char *orig_y = y;
  int count = 0;
  unsigned char *p;
  while ((p = twoway_memmem(y, y + n, x, (size_t)m))) {
    OUTPUT(p - orig_y);
    n -= (p + 1) - y;
    y = p + 1; // can be optimized
  }
  END_SEARCHING
  return count;
}
