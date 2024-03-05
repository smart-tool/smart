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
 * Skip Search algorithm, CHARRAS C., LECROQ T., PEHOUSHEK J.D., 1998,
 * A very fast string matching algorithm for small alphabets and long
 * patterns, in Proceedings of the 9th Annual Symposium on
 * Combinatorial Pattern Matching , M. Farach-Colton ed., Piscataway,
 * New Jersey, Lecture Notes in Computer Science 1448, pp 55-64,
 * Springer-Verlag, Berlin.
 * https://www-igm.univ-mlv.fr/~lecroq/string/node31.html#SECTION00310
 *
 * Constraints: requires m>=8
 */

#include "include/define.h"
#include "include/main.h"
#include "include/AUTOMATON.h"
#include "include/search_small.h"

// 32640 + 16320 + 8160 + 4080 + 2040 + 1020 + 510 + 255
#define DSIGMA 65026
#define HS(x, i)                                                               \
  (x[i] << 7) + (x[i + 1] << 6) + (x[i + 2] << 5) + (x[i + 3] << 4) +          \
      (x[i + 4] << 3) + (x[i + 5] << 2) + (x[i + 6] << 1) + x[i + 7]
#define Q 8

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int i, j, count, h, k;
  List ptr, z[DSIGMA];
  struct _cell s_cells[M_CUTOFF];

  if (m < Q)
    return search_small(x, m, y, n);

  BEGIN_PREPROCESSING
  List *allocs = NULL;
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(z, 0, DSIGMA * sizeof(List));
  const int mq = m - Q + 1;
  if (m > M_CUTOFF)
    allocs = (List *)calloc(mq, sizeof(List));
  for (i = 0; i < mq; ++i) {
    if (m > M_CUTOFF) {
      ptr = (List)malloc(sizeof(struct _cell));
      if (ptr == NULL)
        error("SKIP");
      allocs[i] = ptr;
    } else
      ptr = &s_cells[i];
    ptr->element = i;
    ptr->next = z[HS(x, i)];
    z[HS(x, i)] = ptr;
  }
  count = 0;
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (j = mq - 1; j < n; j += mq) {
    for (ptr = z[HS(y, j)]; ptr != NULL; ptr = ptr->next) {
      if ((j - ptr->element) <= n - m) {
        k = 0;
        h = j - ptr->element;
        while (k < m && x[k] == y[h + k])
          k++;
        if (k >= m)
          OUTPUT(h);
      }
    }
  }

  /* Freeing */
  if (m > M_CUTOFF) {
    for (i = 0; i < mq; ++i) {
      free(allocs[i]);
    }
    free(allocs);
  }
  END_SEARCHING
  return count;
}
