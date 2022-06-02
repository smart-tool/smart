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
 * This is an implementation of the TVSBS algorithm
 * in R. Thathoo and A. Virmani and S. S. Lakshmi and N. Balakrishnan and K. Sekar. 
 * TVSBS: A Fast Exact Pattern Matching Algorithm for Biological Sequences. 
 * J. Indian Acad. Sci., Current Sci., vol.91, n.1, pp.47--53, (2006).
 */

#include "include/define.h"
#include "include/main.h"

void TVSBSpreBrBc(unsigned char *x, int m, int brBc[SIGMA][SIGMA]) {
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


int search(unsigned char *x, int m, unsigned char *y, int n){
   int count,i,j =0;
   int BrBc[SIGMA][SIGMA];
   unsigned char firstCh, lastCh;

   BEGIN_PREPROCESSING
   count = 0;
   TVSBSpreBrBc(x, m, BrBc);
   firstCh = x[0];
   lastCh = x[m -1];
   for(i=0; i<m; i++) y[n+i]=y[n+m+i]=x[i];
   END_PREPROCESSING

   BEGIN_SEARCHING
   while(j <= n - m){
      if(lastCh == y[j + m - 1] && firstCh == y[j]) {
         for(i = m-2; i > 0 && x[i] == y[j + i]; i--);
         if(i <= 0) count++;
      }
      j += BrBc[y[j + m]][y[j+m+1]];
   }
   END_SEARCHING
   return count;
 }
