/*
 *	ALGORITHM Weak Factor Recognizer, Tuned Version, (TWFR)
 *  By Simone Faro, Domenico Cantone and Arianna Pavone
 *
 * This tuned variant has two optimisations:
 *   1.  An initial fast loop which does not require a position test, as the
 *pattern is copied to the end of the data.
 *   2.  Pre-processing is done on q-grams rather than bytes, as for the WFRQ
 *variants.
 *
 * PREPROCESSING:
 *		an hash value is computed for al factors of the pattern with
 *length in [1..8] the computed hash value is always a number in [0...256*256]
 *		if w is a factor of x, and hash(w) is its hash value, than
 *F[hash(w)]=TRUE, otherwise F[hash(w)]=FALSE SEARCHING The algorithm searches
 *for factors of the pattern using a weak recognition method the search phase is
 *very similar to BOM. The window is scanned right to left and for each
 *character a new hash value of the suffix of the window is computed. Let w be
 *the suffix we scanned. If F[hash(w)]=TRUE we continue scanning the next
 *character of the window. Otherwise we stop scanning (w is not a factor of the
 *pattern) and jump to the right, like in BOM.
 */

#include "include/define.h"
#include "include/main.h"
#include "include/GRAPH.h"

#define Q 2
#define HASH(j) (y[j] << 2) + y[j - 1]

/*
 * Pre-process q-gram factors of the pattern.
 */
void preprocessingQ(unsigned char *x, int m, char *F) {
  int i, j;
  unsigned short h;
  int fact = m < 8 ? m : 8;
  for (i = 0; i < 256 * 256; i++)
    F[i] = FALSE;
  for (i = Q - 1; i < m; i++) {
    int stop = (i - fact + 1) >= Q - 1 ? (i - fact + 1) : Q - 1;
    h = (x[i] << 2) + x[i - 1];
    F[h] = TRUE;
    for (j = i - Q; j >= stop; j -= Q) {
      h = (h << 4) + (x[j] << 2) + x[j - 1];
      F[h] = TRUE;
    }
  }
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int i, j, k, count, test;
  char F[256 * 256];
  unsigned short h;
  if (m < Q)
    return -1;

  BEGIN_PREPROCESSING
  /* Preprocessing */
  int plen = m;
  if (m % 2 == 1)
    m = m - 1;
  int mq = m - Q + 1;
  preprocessingQ(x, m, F);
  for (i = 0; i < m; i++)
    y[n + i] = x[i];
  END_PREPROCESSING

  BEGIN_SEARCHING
  /* Searching */
  count = 0;
  if (!memcmp(x, y, plen))
    count++;
  j = m;
  while (j < n) {
    h = HASH(j);
    while (!F[h]) {
      j += mq;
      h = HASH(j);
    }
    i = j - m + Q;
    while ((test = F[h]) && j > i + Q - 1) {
      j -= Q;
      h = (h << 4) + HASH(j);
    }
    if (j == i && test) {
      k = 0;
      i -= Q - 1;
      while (k < plen && x[k] == y[i + k])
        k++;
      if (k == plen && i <= n - plen)
        count++;
    }
    j += m - Q + 1;
  }
  END_SEARCHING
  return count;
}
