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
 * This is an implementation of the Raita algorithm
 * in T. Raita. 
 * Tuning the Boyer-Moore-Horspool string searching algorithm. Softw. Pract. Exp., vol.22, n.10, pp.879--884, (1992).
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
   int j, bmBc[SIGMA], count;
   unsigned char c, firstCh, *secondCh, middleCh, lastCh;
    if(m<2) return -1;
    
   /* Preprocessing */
   BEGIN_PREPROCESSING
   preBmBc(x, m, bmBc);
   firstCh = x[0];
   secondCh = x + 1;
   middleCh = x[m/2];
   lastCh = x[m - 1];
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   j = 0;
   while (j <= n - m) {
      c = y[j + m - 1];
      if (lastCh == c && middleCh == y[j + m/2] &&
          firstCh == y[j] &&
          memcmp(secondCh, y + j + 1, m - 2) == 0)
         OUTPUT(j);
      j += bmBc[c];
   }
   END_SEARCHING
   return count;
}



