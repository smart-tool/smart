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
 * This is an implementation of the Karp Rabin algorithm
 * in R. M. Karp and M. O. Rabin. 
 * Efficient randomized pattern-matching algorithms. ibmjrd, vol.31, n.2, pp.249--260, (1987).
 */

#include "include/define.h"
#include "include/main.h"
#define REHASH(a, b, h) ((((h) - (a)*d) << 1) + (b))

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int d, hx, hy, i, j, count;

   BEGIN_PREPROCESSING
   count = 0;
   /* Preprocessing */
   for (d = i = 1; i < m; ++i)
      d = (d<<1);

   for (hy = hx = i = 0; i < m; ++i) {
      hx = ((hx<<1) + x[i]);
      hy = ((hy<<1) + y[i]);
   }
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   j = 0;
   while (j <= n-m) {
      if (hx == hy && memcmp(x, y + j, m) == 0) OUTPUT(j);
      hy = REHASH(y[j], y[j + m], hy);
      ++j;
   }
   END_SEARCHING
   return count;
}

