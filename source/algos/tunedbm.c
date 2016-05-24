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
 * This is an implementation of the Tuned Boyer Moore algorithm
 * in A. Hume and D. M. Sunday. Fast string searching. Softw. Pract. Exp., vol.21, n.11, pp.1221--1248, (1991).
 */

#include "include/define.h"
#include "include/main.h"

void preBmBc(unsigned char *x, int m, int bmBc[]) {
   int i;
   for (i = 0; i < SIGMA; ++i)
      bmBc[i] = m;
   for (i = 0; i < m - 1; ++i)
      bmBc[x[i]] = m - i - 1;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int j, k, shift, bmBc[SIGMA], count;
 
   /* Preprocessing */
   BEGIN_PREPROCESSING
   preBmBc(x, m, bmBc);
   shift = bmBc[x[m - 1]];
   bmBc[x[m - 1]] = 0;
   memset(y + n, x[m - 1], m);
   END_PREPROCESSING
 
   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   j = 0;
   while (j <= n-m) {
      k = bmBc[y[j + m -1]];
      while (k !=  0) {
         j += k; k = bmBc[y[j + m -1]];
         j += k; k = bmBc[y[j + m -1]];
         j += k; k = bmBc[y[j + m -1]];
      }
      if (memcmp(x, y + j, m - 1) == 0 && j <= n-m)
         if(j<=n-m) OUTPUT(j);
      j += shift;                          
   }
   END_SEARCHING
   return count;
}



