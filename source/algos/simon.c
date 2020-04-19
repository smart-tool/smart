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
*/


#include "include/define.h"
#include "include/main.h"
#include "include/AUTOMATON.h"

 
int getTransitionSimon(unsigned char *x, int m, int p, List L[],
                  char c) {
   List cell;
 
   if (p < m - 1 && x[p + 1] == c)
      return(p + 1);
   else if (p > -1) {
      cell = L[p];
      while (cell != NULL)
         if (x[cell->element] == c)
            return(cell->element);
         else
            cell = cell->next;
      return(-1);
   }
   else
      return(-1);
}
 
 
void setTransitionSimon(int p, int q, List L[]) {
   List cell;
 
   cell = (List)malloc(sizeof(struct _cell));
   if (cell == NULL)
      error("SIMON/setTransition");
   cell->element = q;
   cell->next = L[p];
   L[p] = cell;
}
 
 
int preSimon(unsigned char *x, int m, List L[]) {
   int i, k, ell;
   List cell;
 
   memset(L, 0, (m - 2)*sizeof(List));
   ell = -1;
   for (i = 1; i < m; ++i) {
      k = ell;
      cell = (ell == -1 ? NULL : L[k]);
      ell = -1;
      if (x[i] == x[k + 1])
         ell = k + 1;
      else
         setTransitionSimon(i - 1, k + 1, L);
      while (cell != NULL) {
         k = cell->element;
         if (x[i] == x[k])
            ell = k;
         else
            setTransitionSimon(i - 1, k, L);
         cell = cell->next;
      }
   }
   return(ell);
}


int search(unsigned char *x, int m, unsigned char *y, int n) {
   int j, ell, state, count;
   List L[XSIZE];
 
   /* Preprocessing */
   BEGIN_PREPROCESSING
   ell = preSimon(x, m, L);
   END_PREPROCESSING
 
   count = 0;
   /* Searching */
   BEGIN_SEARCHING
   for (state = -1, j = 0; j < n; ++j) {
      state = getTransitionSimon(x, m, state, L, y[j]);
      if (state >= m - 1) {
         OUTPUT(j - m + 1);
         state = ell;
      }
   }
   END_SEARCHING
   return count;
}

