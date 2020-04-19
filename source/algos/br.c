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
 * This is an implementation of the Berry Ravindran algorithm
 * in T. Berry and S. Ravindran. 
 * A fast string matching algorithm and experimental results. 
 * Proceedings of the Prague Stringology Club Workshop '99, pp.16--28, ctu, (1999).
 */

#include "include/define.h"
#include "include/main.h"

void preBrBc(unsigned char *x, int m, int brBc[SIGMA][SIGMA]) {
   int a, b, i;
   for (a = 0; a < SIGMA; ++a)
      for (b = 0; b < SIGMA; ++b)
         brBc[a][b] = m + 2;
   for (a = 0; a < SIGMA; ++a)
      brBc[a][x[0]] = m + 1;
   for (i = 0; i < m - 1; ++i)
      brBc[x[i]][x[i + 1]] = m - i;
   for (a = 0; a < SIGMA; ++a)
      brBc[x[m - 1]][a] = 1;
}


int search(unsigned char *x, int m, unsigned char *y, int n) {
   int j, brBc[SIGMA][SIGMA];
   int i;
   int count;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   preBrBc(x, m, brBc);
   count =0;
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   y[n + 1] = '\0';
   j = 0;
   while (j <= n - m) {
      for(i=0; i<m && x[i]==y[j+i]; i++);
      if (i>=m) count++;
      j += brBc[y[j + m]][y[j + m + 1]];
   }
   END_SEARCHING
   return count;
}


