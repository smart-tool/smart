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
 * This is an implementation of the Zhu Takaoka algorithm
 * in R. F. Zhu and T. Takaoka. 
 * On improving the average case of the Boyer-Moore string matching algorithm. 
 * J. Inform. Process., vol.10, n.3, pp.173--177, (1987).
 */

#include "include/define.h"
#include "include/main.h"

void suffixes(unsigned char *x, int m, int *suff) {
   int f, g, i;
 
   suff[m - 1] = m;
   g = m - 1;
   for (i = m - 2; i >= 0; --i) {
      if (i > g && suff[i + m - 1 - f] < i - g)
         suff[i] = suff[i + m - 1 - f];
      else {
         if (i < g)
            g = i;
         f = i;
         while (g >= 0 && x[g] == x[g + m - 1 - f])
            --g;
         suff[i] = f - g;
      }
   }
}
 
void preBmGs(unsigned char *x, int m, int bmGs[]) {
   int i, j, suff[XSIZE];
 
   suffixes(x, m, suff);
 
   for (i = 0; i < m; ++i)
      bmGs[i] = m;
   j = 0;
   for (i = m - 1; i >= 0; --i)
      if (suff[i] == i + 1)
         for (; j < m - 1 - i; ++j)
            if (bmGs[j] == m)
               bmGs[j] = m - 1 - i;
   for (i = 0; i <= m - 2; ++i)
      bmGs[m - 1 - suff[i]] = m - 1 - i;
}

 void preZtBc(unsigned char *x, int m, int ztBc[SIGMA][SIGMA]) {
   int i, j;

   for (i = 0; i < SIGMA; ++i)
      for (j = 0; j < SIGMA; ++j)
         ztBc[i][j] = m;
   for (i = 0; i < SIGMA; ++i)
      ztBc[i][x[0]] = m - 1;
   for (i = 1; i < m - 1; ++i)
      ztBc[x[i - 1]][x[i]] = m - 1 - i;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, ztBc[SIGMA][SIGMA], bmGs[XSIZE], count;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   preZtBc(x, m, ztBc);
   preBmGs(x, m, bmGs);
   for(i=0; i<m; i++) y[n+i]=y[n+m+i]=x[i];
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   j = 0;
   while (j <= n - m) {
      i = m - 1;
      while (i >=0 && x[i] == y[i + j])
         --i;
      if (i < 0) {
         OUTPUT(j);
         j += bmGs[0];
      }
      else
         j += MAX(bmGs[i],ztBc[y[j + m - 2]][y[j + m - 1]]);
   }
   END_SEARCHING
   return count;
}



