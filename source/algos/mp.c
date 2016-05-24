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
 * This is an implementation of the Morris Pratt algorithm
 * in J. H. Morris and V. R. Pratt. A linear pattern-matching algorithm. (1970).
 */

#include "include/define.h"
#include "include/main.h"

void preMp(unsigned char *x, int m, int mpNext[]) {
   int i, j;
   i = 0;
   j = mpNext[0] = -1;
   while (i < m) {
      while (j > -1 && x[i] != x[j])
         j = mpNext[j];
      mpNext[++i] = ++j;
   }
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, mpNext[XSIZE], count;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   preMp(x, m, mpNext);
   END_PREPROCESSING

   count = 0;
   BEGIN_SEARCHING
   /* Searching */
   i = j = 0;
   while (j < n) {
      while (i > -1 && x[i] != y[j])
         i = mpNext[i];
      i++;
      j++;
      if (i >= m) {
         OUTPUT(j - i);
         i = mpNext[i];
      }
   }
   END_SEARCHING
   return count;
}

