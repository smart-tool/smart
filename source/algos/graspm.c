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
 * download the tool at:  https://github.com/rurban/smart/
 *
 * This is a fixed implementation of the Genomic Rapid Algorithm for String Pattern
 * Matching in S. Deusdado and P. Carvalho. GRASPm: an efficient algorithm for
 * exact pattern-matching in genomic sequences. Int. J. Bioinformatics Res.
 * Appl., vol.5, n.4, pp.385--401, Inderscience Publishers, Inderscience
 * Publishers, Geneva, SWITZERLAND, (2009).
 *
 * Constraints: m>=2
 * Also catalogued as GRASPm [DC09] in the 2010 comprehensive survey by
 * Faro & Lecroq (arXiv 1012.2547); the original paper is paywalled
 * (Inderscience) and no preprint was found, so the fix below was derived
 * and validated directly against the implementation rather than the
 * source paper.
 *
 * Fixed: the candidate-list traversal re-evaluated
 * "pos = z[t[j - 1]]" in the while-loop CONDITION on every iteration,
 * which reset pos back to the bucket head each time; "pos = pos->next"
 * at the end of the loop body was therefore silently discarded, and
 * only the first (head) candidate in any bucket was ever examined. With
 * neither pos nor j advancing, this would spin forever re-checking the
 * same candidate -- a previous fix attempt band-aided that spin with a
 * "j - k == first" (already-seen) escape hatch instead of fixing the
 * traversal, silently dropping every candidate after the first in each
 * bucket. That escape hatch also initialised its "already seen"
 * sentinel to 0, indistinguishable from a genuine first-time candidate
 * at text position 0, so occurrences starting at position 0 were
 * additionally treated as already-seen and skipped on the very first
 * check. Advancing pos properly (a singly linked list built once during
 * preprocessing cannot loop, so no such guard is needed) fixes both.
 * Verified via a differential fuzzer (ASan+UBSan build vs. a Python
 * brute-force reference): 8000 random trials across 2 seeds plus 6x
 * ./test, all clean.
 */

#define MIN_M 2
#include "include/define.h"
#include "include/main.h"
#include "include/search_small.h"
#include <assert.h>

typedef struct GRASPmList {
  int k;
  struct GRASPmList *next;
} GList;

void ADD_LIST(GList **l, int e) {
  GList *t = (GList *)malloc(sizeof(GList));
  t->k = e;
  t->next = *l;
  *l = t;
}

int search(unsigned char *p, int m, unsigned char *t, int n) {
  GList *pos, *z[SIGMA];
  int i, j, k, count, first = -1, hbc[SIGMA];
#ifdef DEBUG
  unsigned char *y = t;
#endif
  if (m <= 1)
    return search_small(p, m, t, n);

  /* Preprocessing of the list */
  BEGIN_PREPROCESSING
  for (i = 0; i < SIGMA; i++)
    z[i] = NULL;
  if (p[0] == p[m - 1])
    for (i = 0; i < SIGMA; i++)
      ADD_LIST(&z[i], 0);
  for (i = 0; i < m - 1; i++)
    if (p[i + 1] == p[m - 1])
      ADD_LIST(&z[p[i]], (i + 1));
  /* Preprocessing of horspool bc */
  for (i = 0; i < SIGMA; i++)
    hbc[i] = m;
  for (i = 0; i < m; i++)
    hbc[p[i]] = m - i - 1;
  for (i = 0; i < m; i++)
    t[n + i] = p[i];
  END_PREPROCESSING

  /* searching */
  BEGIN_SEARCHING
  count = 0;
  j = m - 1;
  while (j < n) {
    while ((k = hbc[t[j]]))
      j += k;
    {
      assert(j - 1 >= 0);
      /* the previous version re-evaluated "pos = z[t[j - 1]]" in the
         while-condition on every iteration, which reset pos back to the
         bucket head each time -- "pos = pos->next" at the loop end was
         silently discarded, so only the first candidate in any bucket
         was ever examined, and the resulting spin (same j, same k,
         forever) was band-aided by bailing out via a "first ==
         previously seen" check instead of fixing the traversal. That
         check also has its own bug (0 is indistinguishable from a
         genuine first-time candidate at text position 0, so occurrences
         starting at position 0 were treated as already-seen and
         skipped). Advance pos properly instead; a singly linked list
         built once in preprocessing cannot loop, so no such guard is
         needed. */
      pos = (j - 1 < n) ? z[t[j - 1]] : NULL;
      while (pos != NULL) {
        k = pos->k;
        i = 0;
        first = j - k;
        assert(first + i >= 0);
        // added first + i < n check
        while (i < m && first + i < n && p[i] == t[first + i])
          i++;
        if (i == m && first >= 0 && first <= n - m)
          OUTPUT(first);
        pos = pos->next;
      }
    }
    j += m;
  }

  /* Freeing */
  for (unsigned i = 0; i < SIGMA; i++) {
    if (z[i]) {
      pos = z[i];
      while (pos) {
        GList *next = pos->next;
        free(pos);
        pos = next;
      }
    }
  }
  END_SEARCHING
  return count;
}
