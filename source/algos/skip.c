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
 */

#include "include/define.h"
#include "include/main.h"
#include "include/AUTOMATON.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int i, j, count, h, k;
  List ptr, z[SIGMA];

  BEGIN_PREPROCESSING
  List *allocs = (List*)calloc(m, sizeof(List));
  memset(z, 0, SIGMA * sizeof(List));
  for (i = 0; i < m; ++i) {
    ptr = (List)malloc(sizeof(struct _cell));
    if (ptr == NULL)
      error("SKIP");
    ptr->element = i;
    ptr->next = z[x[i]];
    z[x[i]] = ptr;
    allocs[i] = ptr;
  }
  END_PREPROCESSING

  BEGIN_SEARCHING
  count = 0;
  /* Searching */
  for (j = m - 1; j < n; j += m) {
    for (ptr = z[y[j]]; ptr != NULL; ptr = ptr->next) {
      if ((j - ptr->element) <= n - m) {
        k = 0;
        h = j - ptr->element;
        while (k < m && x[k] == y[h + k])
          k++;
        if (k >= m)
          count++;
      }
    }
  }
  /* Freeing */
  for (i = 0; i < m; ++i) {
    free(allocs[i]);
  }
  free(allocs);
  END_SEARCHING
  return count;
}
