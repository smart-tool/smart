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
 * This is an implementation of the Backward Oracle Matching
 * in C. Allauzen and M. Crochemore and M. Raffinot. 
 * Factor oracle: a new structure for pattern matching. 
 * SOFSEM'99, Theory and Practice of Informatics, Lecture Notes in Computer Science, n.1725, pp.291--306, (1999).
 */

#include "include/define.h"
#include "include/main.h"
#include "include/GRAPH.h"

int getTransition(unsigned char *x, int p, List L[], unsigned char c) {
   List cell;
   if (p > 0 && x[p - 1] == c) return(p - 1);
   else {
      cell = L[p];
      while (cell != NULL)
         if (x[cell->element] == c)
            return(cell->element);
         else
            cell = cell->next;
      return(UNDEFINED);
   }
}

void setTransition(int p, int q, List L[]) {
   List cell;
   cell = (List)malloc(sizeof(struct _cell));
   if (cell == NULL)
      error("BOM/setTransition");
   cell->element = q;
   cell->next = L[p];
   L[p] = cell;
}

void oracle(unsigned char *x, int m, char T[], List L[]) {
   int i, p, q;
   int S[XSIZE + 1];
   char c;
   S[m] = m + 1;
   for (i = m; i > 0; --i) {
      c = x[i - 1];
      p = S[i];
      while (p <= m &&
             (q = getTransition(x, p, L, c)) ==
             UNDEFINED) {
         setTransition(p, i - 1, L);
         p = S[p];
      }
      S[i - 1] = (p == m + 1 ? m : q);
   }
   p = 0;
   while (p <= m) {
      T[p] = TRUE;
      p = S[p];
   }
}


int search(unsigned char *x, int m, unsigned char *y, int n) {
   char T[XSIZE + 1];
   List L[XSIZE + 1];
   int i, j, r, period, q, shift, count;

	BEGIN_PREPROCESSING
   /* Preprocessing */
   memset(L, 0, (m + 1)*sizeof(List));
   memset(T, FALSE, (m + 1)*sizeof(char));
   oracle(x, m, T, L);
	END_PREPROCESSING

	BEGIN_SEARCHING
   /* Searching */
   count = 0;
   j = 0;
   while (j <= n - m) {
      i = m - 1;
      r = m;
      shift = m;
      while (i + j >= 0 && (q = getTransition(x, r, L, y[i + j])) != UNDEFINED) {
         r = q;
         if (T[r] == TRUE) {
            period = shift;
            shift = i;
         }
         --i;
      }
      if (i < 0) {
         OUTPUT(j);
         shift = period;
      }
      j += shift;
   }
   for(i=0; i<=m; i++) free(L[i]);
 	END_SEARCHING
  return count;
}

